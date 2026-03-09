# Design: Debug Support for Nautilus JIT-Compiled Functions

## Goal

Enable stepping through JIT-compiled Nautilus functions in a debugger (gdb/lldb), with:
1. **Source locations** from the original user C++ code mapped to JIT'd instructions
2. **Full call hierarchy** — the entire chain of user calls that led to each traced operation
3. **Value representation** — `val<T>` values visible as named locals in the debugger

---

## Architecture Overview

```
User Code (val<T> operations)
    ↓
Tracing — Tag system captures return address chain as Trie path (already done)
    ↓  NEW: Add parent pointer to TrieNode for upward traversal
    ↓  NEW: Resolve address chain → SourceLocation hierarchy via backward-cpp
    ↓
TraceOperation — already stores Snapshot containing Tag*
    ↓  NEW: Attach resolved SourceLocation chain to TraceOperation
    ↓
IR Operations — carry OperationIdentifier
    ↓  NEW: Attach SourceLocation chain to Operation
    ↓
MLIR Lowering — currently uses getNameLoc(name) → NameLoc(FileLineColLoc("Query_1", 0, 0))
    ↓  NEW: Build nested CallSiteLoc from SourceLocation chain
    ↓  NEW: Emit llvm.dbg.value for val<T> variables
    ↓
LLVM — debug info preserved through optimization
    ↓
Executable — debugger can step through with inlined call stack, inspect values
```

---

## Key Insight: Full Call Hierarchy via CallSiteLoc

The Tag system already captures the **full call chain** as a path through the trie:

```
root → addr[0] → addr[1] → addr[2] → ... → addr[n]  (stops at startAddress)
       (innermost)                         (outermost user frame)
```

When the user writes:
```cpp
void helper(val<int>& x) { x = x + 1; }           // helper.cpp:5
void process(val<int>& x) { helper(x); }           // process.cpp:10
auto f = engine.invoke([](val<int> x) { process(x); return x; }); // main.cpp:20
```

The tag for `x + 1` captures addresses for: `val::operator+` → `helper` → `process` → lambda.
After filtering out nautilus internals, we get the user-relevant chain:
```
helper.cpp:5  ←called from  process.cpp:10  ←called from  main.cpp:20
```

MLIR's `CallSiteLoc` represents exactly this:
```cpp
auto loc = CallSiteLoc::get(
    FileLineColLoc::get(ctx, "helper.cpp", 5, 0),
    CallSiteLoc::get(
        FileLineColLoc::get(ctx, "process.cpp", 10, 0),
        FileLineColLoc::get(ctx, "main.cpp", 20, 0)
    )
);
```

A debugger then shows this as an inlined call stack — the user sees exactly which chain of calls led to each JIT'd instruction.

---

## Detailed Design

### Step 1: Add Parent Pointer to TrieNode

**File:** `nautilus/src/nautilus/tracing/tag/Trie.hpp`

**Problem:** The current `TrieNode` has only `content` (private `T`) and `children` (private `vector<unique_ptr<TrieNode<T>>>`). Given a `Tag*` (which is `TrieNode<TagAddress>*`), we cannot walk back to the root to reconstruct the full address chain. There are no existing accessors — `append()` is the only public method.

**Current code (lines 15-42):**
```cpp
template <typename T>
class TrieNode {
public:
	explicit TrieNode() : content(0) {}
	explicit TrieNode(T value) : content(value) {}

	TrieNode<T>* append(T& value) {
		auto found = std::find_if(children.begin(), children.end(),
		    [value](std::unique_ptr<TrieNode<T>>& x) { return x->content == value; });
		if (found == children.end()) {
			return children.emplace_back(std::make_unique<TrieNode<T>>(value)).get();
		} else {
			return found->get();
		}
	}

private:
	T content;
	std::vector<std::unique_ptr<TrieNode<T>>> children;
};
```

**Changes:**
1. Add `TrieNode<T>* parent_` member (private, initialized to `nullptr`)
2. In `append()`, set `node->parent_ = this` when creating new children
3. Add `getAddressChain()` method that walks parent pointers to collect addresses
4. Note: `content` is private but accessible within member functions, so `getAddressChain()` works

```cpp
template <typename T>
class TrieNode {
public:
	explicit TrieNode() : content(0), parent_(nullptr) {}
	explicit TrieNode(T value) : content(value), parent_(nullptr) {}

	TrieNode<T>* append(T& value) {
		auto found = std::find_if(children.begin(), children.end(),
		    [value](std::unique_ptr<TrieNode<T>>& x) { return x->content == value; });
		if (found == children.end()) {
			auto& node = children.emplace_back(std::make_unique<TrieNode<T>>(value));
			node->parent_ = this;
			return node.get();
		} else {
			return found->get();
		}
	}

	/// Walk parent chain to collect full address sequence (leaf → root).
	/// Returns innermost address first, outermost last.
	/// Stops at root (parent_ == nullptr), which is the rootTagThreeNode in TagRecorder.
	std::vector<T> getAddressChain() const {
		std::vector<T> chain;
		for (auto* node = this; node->parent_ != nullptr; node = node->parent_) {
			chain.push_back(node->content);
		}
		return chain;
	}

private:
	T content;
	TrieNode<T>* parent_ = nullptr;
	std::vector<std::unique_ptr<TrieNode<T>>> children;
};
```

**Impact:** Adds one pointer per trie node. The trie is long-lived (owned by `TagRecorder::rootTagThreeNode`).

---

### Step 2: Add `getTag()` Accessor to Snapshot

**File:** `nautilus/src/nautilus/tracing/Snapshot.hpp`

**Problem:** `Snapshot` stores `Tag* tag` as a **private** member with no accessor. Only `friend std::hash<Snapshot>` can access it. The `SourceLocationResolutionPhase` needs to read the `Tag*` from each `TraceOperation`'s `Snapshot`.

**Current code (lines 8-26):**
```cpp
struct Snapshot {
public:
	Snapshot(Tag* tag, uint64_t staticValueHash);
	Snapshot();
	Snapshot(const Snapshot& snapshot) = default;
	Snapshot& operator=(const Snapshot& snapshot) = default;
	bool operator==(const Snapshot& rhs) const;
	bool operator!=(const Snapshot& rhs) const;

private:
	friend std::hash<Snapshot>;
	uint64_t staticValueHash;
	Tag* tag;
};
```

**Change:** Add a public accessor:
```cpp
public:
	Tag* getTag() const { return tag; }
```

---

### Step 3: SourceLocation Struct and Chain

**New file:** `nautilus/src/nautilus/tracing/tag/SourceLocation.hpp`

```cpp
#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace nautilus::tracing {

struct SourceLocation {
	std::string file;
	uint32_t line = 0;
	uint32_t column = 0;
	std::string function;

	bool isValid() const {
		return line != 0 && !file.empty();
	}
};

/// A chain of source locations representing the call hierarchy.
/// Index 0 = innermost (where the operation is), last = outermost caller.
using SourceLocationChain = std::vector<SourceLocation>;

} // namespace nautilus::tracing
```

---

### Step 4: SourceLocationResolver — Addresses to File:Line

**New files:** `nautilus/src/nautilus/tracing/tag/SourceLocationResolver.hpp` and `.cpp`

**Resolution uses backward-cpp** (bundled in `third_party/backward-cpp/`). Nautilus already links it via `Backward::Interface` when `ENABLE_STACKTRACE=ON` (see `nautilus/CMakeLists.txt`).

**backward-cpp API details** (from `third_party/backward-cpp/backward.hpp`):
- `backward::ResolvedTrace` (line 676) extends `Trace` (has `void* addr`, `size_t idx`)
- `backward::ResolvedTrace::SourceLoc` (line 678) has: `std::string function`, `std::string filename`, `unsigned line`, `unsigned col`
- `backward::TraceResolverImplBase::resolve(ResolvedTrace t)` (line 1240) — resolves a single address
- `backward::TraceResolverImplBase::load_addresses(void* const* addresses, int count)` (line 1231) — initializes resolver state

**Header:**
```cpp
#pragma once

#include "SourceLocation.hpp"
#include "Tag.hpp"
#include <unordered_map>

namespace nautilus::tracing {

class SourceLocationResolver {
public:
	/// Resolve a Tag* into a full call hierarchy of source locations.
	/// Walks parent pointers (via getAddressChain()), resolves each address,
	/// filters out nautilus-internal frames.
	SourceLocationChain resolve(Tag* tag) const;

	/// Resolve a single TagAddress to a source location.
	SourceLocation resolve(TagAddress address) const;

private:
	bool isUserFrame(const SourceLocation& loc) const;

	mutable std::unordered_map<TagAddress, SourceLocation> cache_;
};

} // namespace nautilus::tracing
```

**Implementation:**
```cpp
#include "SourceLocationResolver.hpp"
#include <backward.hpp>

namespace nautilus::tracing {

SourceLocation SourceLocationResolver::resolve(TagAddress address) const {
	if (auto it = cache_.find(address); it != cache_.end()) {
		return it->second;
	}

	SourceLocation loc;

	// Construct a ResolvedTrace with the target address.
	// backward::ResolvedTrace extends Trace which has {void* addr, size_t idx}.
	backward::ResolvedTrace trace;
	trace.addr = reinterpret_cast<void*>(address);
	trace.idx = 0;

	// Initialize the resolver with our address, then resolve it.
	backward::TraceResolver resolver;
	void* addrPtr = trace.addr;
	resolver.load_addresses(&addrPtr, 1);
	auto resolved = resolver.resolve(trace);

	if (!resolved.source.filename.empty()) {
		loc.file = resolved.source.filename;
		loc.line = resolved.source.line;
		loc.column = resolved.source.col;
		loc.function = resolved.source.function;
	}

	cache_[address] = loc;
	return loc;
}

SourceLocationChain SourceLocationResolver::resolve(Tag* tag) const {
	// getAddressChain() returns innermost first, outermost last.
	auto addresses = tag->getAddressChain();
	SourceLocationChain chain;
	for (auto addr : addresses) {
		auto loc = resolve(addr);
		if (loc.isValid() && isUserFrame(loc)) {
			chain.push_back(std::move(loc));
		}
	}
	return chain;
}

bool SourceLocationResolver::isUserFrame(const SourceLocation& loc) const {
	// Filter out nautilus-internal frames (val<T> operators, tracing infra, etc.)
	return loc.file.find("nautilus/src/nautilus/") == std::string::npos &&
	       loc.file.find("nautilus/include/nautilus/") == std::string::npos;
}

} // namespace nautilus::tracing
```

**Build integration:** This file includes `<backward.hpp>` from `third_party/backward-cpp/`. It needs `Backward::Interface` linked, which is already done in `nautilus/CMakeLists.txt` when `ENABLE_STACKTRACE` is on. If debug support should work without stacktraces, consider making the resolver conditional or always linking backward-cpp when `ENABLE_COMPILER` is on.

---

### Step 5: Attach SourceLocationChain to TraceOperation

**File:** `nautilus/src/nautilus/tracing/TraceOperation.hpp`

**Current `TraceOperation` class (lines 48-60):** All members are **public**.
```cpp
class TraceOperation {
public:
	TraceOperation(Snapshot& tag, Op op, Type resultType, TypedValueRef ref, std::vector<InputVariant>&& input);
	TraceOperation(Op op, std::vector<InputVariant>&& input);

	Snapshot tag;                    // public
	Op op;                          // public
	Type resultType;                // public
	TypedValueRef resultRef;        // public
	std::vector<InputVariant> input; // public
};
```

**Change:** Add a new public member:
```cpp
	SourceLocationChain sourceLocations; // NEW: populated by SourceLocationResolutionPhase
```

Add `#include "tag/SourceLocation.hpp"` at the top.

---

### Step 6: SourceLocationResolutionPhase

**New file:** `nautilus/src/nautilus/tracing/phases/SourceLocationResolutionPhase.hpp` (and `.cpp`)

This phase runs after tracing, iterating over all `TraceOperation`s to resolve their `Snapshot::tag` pointers to source locations.

```cpp
#pragma once

#include "nautilus/tracing/ExecutionTrace.hpp"

namespace nautilus::tracing {

class SourceLocationResolutionPhase {
public:
	static void apply(ExecutionTrace& trace);
};

} // namespace nautilus::tracing
```

**Implementation:**
```cpp
#include "SourceLocationResolutionPhase.hpp"
#include "nautilus/tracing/tag/SourceLocationResolver.hpp"

namespace nautilus::tracing {

void SourceLocationResolutionPhase::apply(ExecutionTrace& trace) {
	SourceLocationResolver resolver;
	// ExecutionTrace::blocks is a public member: std::vector<Block> blocks
	// Block::operations is a public member: std::vector<TraceOperation> operations
	for (auto& block : trace.blocks) {
		for (auto& op : block.operations) {
			// Snapshot::getTag() returns Tag* (newly added accessor)
			Tag* tag = op.tag.getTag();
			if (tag != nullptr) {
				op.sourceLocations = resolver.resolve(tag);
			}
		}
	}
}

} // namespace nautilus::tracing
```

**Pipeline integration in `nautilus/src/nautilus/compiler/JITCompiler.cpp`:**

Insert the phase between SSA creation and IR conversion (around line 72-76):

```cpp
// Current code:
auto ssaCreationPhase = tracing::SSACreationPhase();
auto afterSSA = ssaCreationPhase.apply(std::move(executionTrace));
dumpHandler.dump("after_ssa", "trace", [&]() { return afterSSA->toString(); });

// NEW: Resolve source locations (only when debug.enabled)
if (options.getOptionOrDefault("debug.enabled", false)) {
	tracing::SourceLocationResolutionPhase::apply(*afterSSA);
}

// Existing code continues:
auto irGenerationPhase = tracing::TraceToIRConversionPhase();
const auto ir = irGenerationPhase.apply(std::move(afterSSA), compilationId);
```

The `#include` for the new header must be added at the top of `JITCompiler.cpp`.

---

### Step 7: Propagate SourceLocationChain to IR Operations

**File:** `nautilus/src/nautilus/compiler/ir/operations/Operation.hpp`

**Current Operation class (lines 38-102):**
- Members `opType`, `identifier`, `stamp` are **`const`** and in the `protected` section
- `inputs` is non-const protected

**Change:** Add a mutable member and accessor/setter:
```cpp
// In the public section:
	const tracing::SourceLocationChain& getSourceLocations() const { return sourceLocations; }
	void setSourceLocations(tracing::SourceLocationChain locs) { sourceLocations = std::move(locs); }

// In the protected section, after inputs:
	tracing::SourceLocationChain sourceLocations;
```

Add `#include "nautilus/tracing/tag/SourceLocation.hpp"` at the top.

**File:** `nautilus/src/nautilus/tracing/phases/TraceToIRConversionPhase.cpp`

In each `process*` method, after creating the IR operation and calling `frame.setValue()`, copy the source locations. The cleanest approach is to add a helper in `IRConversionContext` and call it after each operation creation.

**Add a private helper to `IRConversionContext` (in TraceToIRConversionPhase.hpp):**
```cpp
void copySourceLocations(compiler::ir::Operation* irOp, TraceOperation& traceOp) {
	if (!traceOp.sourceLocations.empty()) {
		irOp->setSourceLocations(traceOp.sourceLocations);
	}
}
```

**Then in each process method, after `frame.setValue(...)`, add the copy call.**

Example for `processBinaryOperator` (line 204-213):
```cpp
template <typename OpType>
void TraceToIRConversionPhase::IRConversionContext::processBinaryOperator(
    ValueFrame& frame, compiler::ir::BasicBlock* currentBlock, TraceOperation& op) {
	auto leftInput = frame.getValue(createValueIdentifier(op.input[0]));
	auto rightInput = frame.getValue(createValueIdentifier(op.input[1]));
	auto resultIdentifier = createValueIdentifier(op.resultRef);
	auto operation = currentBlock->addOperation<OpType>(resultIdentifier, leftInput, rightInput);
	frame.setValue(resultIdentifier, operation);
	copySourceLocations(operation, op);  // NEW
}
```

Apply the same pattern to: `processUnaryOperator`, `processTernaryOperator`, `processLogicalComperator`, `processLoad`, `processStore`, `processCall`, `processConst`, `processCast`, `processBinaryComp`, `processShift`, and inline for `RETURN` in `processOperation`.

---

### Step 8: Build Nested CallSiteLoc in MLIR Lowering

**Files:** `nautilus/src/nautilus/compiler/backends/mlir/MLIRLoweringProvider.hpp` and `.cpp`

**Current `getNameLoc` (MLIRLoweringProvider.cpp lines 84-87):**
```cpp
mlir::Location MLIRLoweringProvider::getNameLoc(const std::string& name) {
	auto baseLocation = mlir::FileLineColLoc::get(builder->getStringAttr("Query_1"), 0, 0);
	return mlir::NameLoc::get(builder->getStringAttr(name), baseLocation);
}
```

This is called **57 times** throughout the file, with operation-descriptive names like `"binOpResult"`, `"loadedValue"`, `"comparison"`, `"return"`, etc.

**Strategy:** Keep `getNameLoc` for the non-debug path. Add a new overload that takes an `Operation*` and uses real source locations when available, falling back to the existing behavior:

**Add to MLIRLoweringProvider.hpp** (private section):
```cpp
	::mlir::Location getNameLoc(const std::string& name, const ir::Operation* op);
```

**Implementation in MLIRLoweringProvider.cpp:**
```cpp
mlir::Location MLIRLoweringProvider::getNameLoc(const std::string& name, const ir::Operation* op) {
	auto& chain = op->getSourceLocations();
	if (chain.empty()) {
		// Fallback: use existing behavior
		return getNameLoc(name);
	}

	// Build FileLineColLoc for each frame in the call hierarchy
	std::vector<mlir::Location> frameLocs;
	for (auto& loc : chain) {
		frameLocs.push_back(mlir::FileLineColLoc::get(
		    builder->getStringAttr(loc.file), loc.line, loc.column));
	}

	// Build nested CallSiteLoc chain: inner ← mid ← outer
	// chain[0] = innermost (actual operation site)
	// chain[n-1] = outermost caller
	mlir::Location result = frameLocs[0];
	for (size_t i = 1; i < frameLocs.size(); ++i) {
		result = mlir::CallSiteLoc::get(result, frameLocs[i]);
	}

	// Wrap in NameLoc to preserve the operation-descriptive name
	return mlir::NameLoc::get(builder->getStringAttr(name), result);
}
```

**Updating call sites:** The `generateMLIR` methods receive typed `ir::Operation*` pointers (e.g., `ir::AddOperation*`). These inherit from `ir::Operation`, so the source locations are accessible via the base class. Change call sites from:

```cpp
// Before:
builder->create<mlir::arith::AddIOp>(getNameLoc("binOpResult"), lhs, rhs);

// After:
builder->create<mlir::arith::AddIOp>(getNameLoc("binOpResult", addOp), lhs, rhs);
```

Each `generateMLIR(ir::XxxOperation* op, ValueFrame&)` method already has the operation pointer as its first argument. Add it as the second argument to `getNameLoc`.

**Note:** Some `getNameLoc` calls are in utility methods (`getConstInt`, `getConstBool`) that don't have an `Operation*` available. These keep using the single-argument `getNameLoc(name)` fallback.

---

### Step 9: DWARF Debug Info + GDB JIT Registration

For the debugger to actually use MLIR locations, we need proper LLVM debug info.

#### 9a: Debug Metadata Pass (in main library, NOT as a plugin)

**New files:** `nautilus/src/nautilus/compiler/backends/mlir/DebugInfoPass.hpp` and `.cpp`

**Important:** The existing files in `src/nautilus/llvm-passes/` are a **separate Clang plugin** (loaded via `LLVM_ATTRIBUTE_WEAK`, requires Clang 19+, not supported on ARM). Debug info passes must be part of the **main library**, invoked from `LLVMIROptimizer`, not as external plugins.

The pass operates on the LLVM Module after MLIR→LLVM lowering:

```cpp
#pragma once

#include <llvm/IR/Module.h>

namespace nautilus::compiler::mlir {

/// Adds DWARF debug metadata to a JIT-compiled LLVM module.
/// Creates DICompileUnit, DIFile, DISubprogram, and attaches
/// debug locations (from MLIR FileLineColLoc) to LLVM instructions.
class DebugInfoPass {
public:
	/// Add debug info to the module. The "execute" function gets a DISubprogram.
	/// Instructions that already have DebugLoc (from MLIR lowering) are preserved.
	/// Instructions without DebugLoc get one from a neighboring instruction.
	static void run(llvm::Module& module);
};

} // namespace nautilus::compiler::mlir
```

**Implementation approach:**
1. Create `DIBuilder` for the module
2. Create `DIFile` for each unique source file referenced in instruction debug locations
3. Create `DICompileUnit` (language: `DW_LANG_C_plus_plus`, producer: "nautilus")
4. Create `DISubprogram` for the "execute" function and attach it
5. For instructions without debug locations, propagate from neighbors
6. Finalize the `DIBuilder`

**MLIR's LLVM dialect lowering** already converts `FileLineColLoc` and `CallSiteLoc` to LLVM `DebugLoc` on instructions — but only if a `DISubprogram` exists. Without it, locations are silently dropped. This pass ensures the subprogram exists.

#### 9b: Debug Variable Pass

**New files:** `nautilus/src/nautilus/compiler/backends/mlir/DebugVariablePass.hpp` and `.cpp`

```cpp
#pragma once

#include <llvm/IR/Module.h>
#include <unordered_map>
#include <string>

namespace nautilus::compiler::mlir {

/// Inserts llvm.dbg.value intrinsics for SSA values, making them visible
/// as local variables in the debugger.
class DebugVariablePass {
public:
	/// nameMap: maps LLVM value names (from IR OperationIdentifier) → human-readable names.
	/// If empty, uses positional naming: v0, v1, v2, ...
	static void run(llvm::Module& module,
	                const std::unordered_map<std::string, std::string>& nameMap = {});
};

} // namespace nautilus::compiler::mlir
```

**Implementation approach:**
1. For each SSA value in the "execute" function that has a debug location:
   - Create `DILocalVariable` with name from nameMap or positional name
   - Map nautilus `Type` → DWARF base type (`DW_ATE_signed`, `DW_ATE_float`, etc.)
   - Insert `llvm.dbg.value` at the definition point
2. This uses `DIBuilder::insertDbgValueIntrinsic()`

#### 9c: Integration in LLVMIROptimizer

**File:** `nautilus/src/nautilus/compiler/backends/mlir/LLVMIROptimizer.cpp`

The optimizer returns a lambda `std::function<llvm::Error(llvm::Module*)>`. Add debug passes before the main optimization pipeline when debug mode is on:

```cpp
// Inside the lambda returned by getLLVMOptimizerPipeline:
if (options.getOptionOrDefault("debug.enabled", false)) {
	DebugInfoPass::run(*llvmIRModule);
	DebugVariablePass::run(*llvmIRModule);
}

// Then apply LLVM optimizations (with reduced level when debugging)
int optLevel = options.getOptionOrDefault("debug.enabled", false)
    ? options.getOptionOrDefault("debug.optimization_level", 0)
    : getOptimizationLevel(options);
auto optPipeline = ::mlir::makeOptimizingTransformer(optLevel, SIZE_LEVEL, targetMachinePtr);
```

#### 9d: GDB JIT Registration

**File:** `nautilus/src/nautilus/compiler/backends/mlir/JITCompiler.cpp`

**Current code (lines 25-28):**
```cpp
::mlir::ExecutionEngineOptions options;
options.jitCodeGenOptLevel = llvm::CodeGenOptLevel::Aggressive;
options.transformer = optPipeline;
auto maybeEngine = ::mlir::ExecutionEngine::create(*mlirModule, options);
```

**Change:** When debug is enabled, enable GDB notification and adjust codegen opt:
```cpp
::mlir::ExecutionEngineOptions options;
if (nautilusOptions.getOptionOrDefault("debug.enabled", false)) {
	options.jitCodeGenOptLevel = llvm::CodeGenOptLevel::None;
	options.enableGDBNotificationListener = true;
	options.enablePerfNotificationListener = true;
} else {
	options.jitCodeGenOptLevel = llvm::CodeGenOptLevel::Aggressive;
}
options.transformer = optPipeline;
```

**Note:** `enableGDBNotificationListener` and `enablePerfNotificationListener` are fields on `mlir::ExecutionEngineOptions`. Verify exact field names against the MLIR version bundled with the project. If they don't exist, the GDB JIT interface can be enabled manually by registering `llvm::JITEventListener::createGDBRegistrationListener()` on the underlying LLVM execution engine.

---

### Step 10: Value Representation in Debugger

**Goal:** `val<T>` values visible as named locals when stopped in debugger.

**Variable naming strategy (two tiers):**

1. **Automatic (default):** Name based on IR `OperationIdentifier` id: `v0`, `v1`, `v2`, etc. The `OperationIdentifier` has `uint32_t getId()` (Operation.hpp line 32). Combined with source locations, a developer can correlate values.

2. **User-annotated (opt-in, future):** A lightweight API:
   ```cpp
   val<int64_t> counter = 0;
   nautilus::debug::name(counter, "counter");
   ```
   This would record a `(ValueRef → "counter")` mapping in `TraceContext`, propagated to IR Operations and then to the LLVM name map passed to `DebugVariablePass::run()`.

---

## Configuration

**File:** `nautilus/include/nautilus/options.hpp`

The `Options` class is a generic key-value store using `std::variant<int, double, std::string, bool>`. Options are accessed via `getOptionOrDefault<T>(name, default)`.

**Existing MLIR options for reference:**
- `mlir.optimizationLevel` (int, default: 3)
- `mlir.inline_invoke_calls` (bool, default: false)

**New debug options:**
```cpp
options.setOption("debug.enabled", false);          // Master switch
options.setOption("debug.optimization_level", 0);   // LLVM opt level override (int)
```

When `debug.enabled` is `true`:
- `SourceLocationResolutionPhase` runs in `JITCompiler::compile()` between SSA and IR phases
- `getNameLoc(name, op)` produces real `CallSiteLoc` chains in MLIR lowering
- `DebugInfoPass` and `DebugVariablePass` run on the LLVM module
- GDB JIT registration is enabled on ExecutionEngine
- LLVM optimization reduced to `debug.optimization_level` (default 0)

---

## Implementation Order

| Phase | What | Files to Create/Modify |
|-------|------|----------------------|
| 1 | Add parent pointer to `TrieNode` + `getAddressChain()` | Modify: `nautilus/src/nautilus/tracing/tag/Trie.hpp` |
| 2 | Add `getTag()` to `Snapshot` | Modify: `nautilus/src/nautilus/tracing/Snapshot.hpp` |
| 3 | `SourceLocation` struct + `SourceLocationChain` typedef | Create: `nautilus/src/nautilus/tracing/tag/SourceLocation.hpp` |
| 4 | `SourceLocationResolver` using backward-cpp | Create: `nautilus/src/nautilus/tracing/tag/SourceLocationResolver.hpp`, `.cpp` |
| 5 | Add `sourceLocations` field to `TraceOperation` | Modify: `nautilus/src/nautilus/tracing/TraceOperation.hpp` |
| 6 | `SourceLocationResolutionPhase` | Create: `nautilus/src/nautilus/tracing/phases/SourceLocationResolutionPhase.hpp`, `.cpp` |
| 7 | Add `sourceLocations` + getter/setter to `Operation` | Modify: `nautilus/src/nautilus/compiler/ir/operations/Operation.hpp` |
| 8 | Copy `sourceLocations` in `TraceToIRConversionPhase` | Modify: `nautilus/src/nautilus/tracing/phases/TraceToIRConversionPhase.hpp` (add helper), `.cpp` (call in each process method) |
| 9 | Add `getNameLoc(name, op)` overload in MLIR lowering | Modify: `nautilus/src/nautilus/compiler/backends/mlir/MLIRLoweringProvider.hpp`, `.cpp` |
| 10 | Update `generateMLIR` call sites to pass op pointer | Modify: `nautilus/src/nautilus/compiler/backends/mlir/MLIRLoweringProvider.cpp` (57 call sites) |
| 11 | `DebugInfoPass` (DICompileUnit/DISubprogram) | Create: `nautilus/src/nautilus/compiler/backends/mlir/DebugInfoPass.hpp`, `.cpp` |
| 12 | `DebugVariablePass` (llvm.dbg.value) | Create: `nautilus/src/nautilus/compiler/backends/mlir/DebugVariablePass.hpp`, `.cpp` |
| 13 | Integrate debug passes in `LLVMIROptimizer` | Modify: `nautilus/src/nautilus/compiler/backends/mlir/LLVMIROptimizer.cpp` |
| 14 | GDB JIT registration in `JITCompiler` | Modify: `nautilus/src/nautilus/compiler/backends/mlir/JITCompiler.cpp` |
| 15 | Wire up `SourceLocationResolutionPhase` in pipeline | Modify: `nautilus/src/nautilus/compiler/JITCompiler.cpp` (insert between SSA and IR phases) |
| 16 | CMakeLists.txt updates | Modify: `nautilus/src/nautilus/CMakeLists.txt` (add new source files) |
| 17 | Tests | Create: test files for source location resolution, debug info emission |

---

## Key Design Decisions

| Decision | Choice | Rationale |
|----------|--------|-----------|
| Call hierarchy representation | Nested `CallSiteLoc` | Standard MLIR/LLVM mechanism; debuggers natively display inlined frames |
| Walking tag chain | Add parent pointer to `TrieNode` | Minimal overhead (1 ptr/node), enables upward traversal |
| Frame filtering | Exclude paths containing `nautilus/src/nautilus/` and `nautilus/include/nautilus/` | User only cares about their code, not val<T> internals |
| When to resolve addresses | Post-SSA phase in `JITCompiler::compile()`, before IR conversion | Doesn't slow hot tracing loop; addresses stable within process |
| Resolution library | backward-cpp (`third_party/backward-cpp/`) | Already bundled and linked (`Backward::Interface`); no new deps |
| Preserving operation names | `NameLoc` wrapping `CallSiteLoc` chain | Keeps descriptive names ("binOpResult") for MLIR dumps while adding real locations |
| Variable naming | Positional `v{id}` + opt-in `debug::name()` API | C++ can't expose variable names at runtime without macros |
| Debug passes location | `src/nautilus/compiler/backends/mlir/` (main library) | Existing `src/nautilus/llvm-passes/` is a Clang plugin (separate build, Clang 19+ only, no ARM). Debug passes must be in the main library. |
| `Operation` modification | Mutable `sourceLocations` with setter (not in constructor) | `Operation` has `const` members (`opType`, `identifier`, `stamp`); changing constructors would cascade to all 20+ subclasses |
| Scope | MLIR backend only | Primary backend; most impactful; other backends can be added later |

---

## Appendix: Key File Locations (All paths relative to repo root)

### Existing files referenced in this design

| File | Key Details |
|------|------------|
| `nautilus/src/nautilus/tracing/tag/Trie.hpp` | `TrieNode<T>` — content and children are **private** |
| `nautilus/src/nautilus/tracing/tag/Tag.hpp` | `using Tag = TrieNode<TagAddress>; using TagAddress = uint64_t;` |
| `nautilus/src/nautilus/tracing/tag/TagRecorder.hpp` | Owns `Tag rootTagThreeNode` (note spelling), `TagAddress startAddress` |
| `nautilus/src/nautilus/tracing/tag/TagRecorder.cpp` | `createReferenceTagBacktrace()` / `createReferenceTagBuildin()` — builds trie path from backtrace |
| `nautilus/src/nautilus/tracing/Snapshot.hpp` | `Tag* tag` and `uint64_t staticValueHash` are **private**, no getters |
| `nautilus/src/nautilus/tracing/TraceOperation.hpp` | All members (`tag`, `op`, `resultType`, `resultRef`, `input`) are **public** |
| `nautilus/src/nautilus/tracing/Block.hpp` | `std::vector<TraceOperation> operations` is a **public member** (not a getter) |
| `nautilus/src/nautilus/tracing/ExecutionTrace.hpp` | `std::vector<Block> blocks` is **public**; also has `getBlocks()` method |
| `nautilus/src/nautilus/compiler/ir/operations/Operation.hpp` | `opType`, `identifier`, `stamp` are **`const protected`**; `inputs` is non-const protected |
| `nautilus/src/nautilus/tracing/phases/TraceToIRConversionPhase.cpp` | `processOperation()` dispatches on `operation.op`; each `process*` method creates IR ops |
| `nautilus/src/nautilus/compiler/backends/mlir/MLIRLoweringProvider.hpp` | `getNameLoc(name)` is **private**; 57 call sites in `.cpp` |
| `nautilus/src/nautilus/compiler/backends/mlir/MLIRLoweringProvider.cpp` | `getNameLoc` returns `NameLoc(name, FileLineColLoc("Query_1", 0, 0))` |
| `nautilus/src/nautilus/compiler/backends/mlir/JITCompiler.cpp` | Creates `ExecutionEngineOptions` with `jitCodeGenOptLevel` and `transformer` |
| `nautilus/src/nautilus/compiler/backends/mlir/LLVMIROptimizer.cpp` | Returns lambda `Error(Module*)` that applies LLVM opt pipeline |
| `nautilus/src/nautilus/compiler/JITCompiler.cpp` | Top-level pipeline: trace → SSA → IR → backend. Lines 63-87. |
| `nautilus/include/nautilus/options.hpp` | `Options` = `unordered_map<string, variant<int,double,string,bool>>` with `getOptionOrDefault<T>()` |
| `nautilus/src/nautilus/exceptions/Stacktrace.cpp` | Existing backward-cpp usage: `StackTrace::load_here(32)`, `Printer::print()` |
| `third_party/backward-cpp/backward.hpp` | `ResolvedTrace::SourceLoc` has `filename`, `line`, `col`, `function` (line 678) |

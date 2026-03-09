# Design: Debug Support for Nautilus JIT-Compiled Functions

## Goal

Enable stepping through JIT-compiled Nautilus functions in a debugger (gdb/lldb), with:
1. **Source locations** from the original user C++ code mapped to JIT'd instructions
2. **Full call hierarchy** — not just the innermost frame, but the entire chain of user calls that led to each traced operation
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
MLIR Lowering — currently uses getNameLoc("Query_1", 0, 0)
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
    FileLineColLoc("helper.cpp", 5, 0),     // where the op actually is
    CallSiteLoc::get(
        FileLineColLoc("process.cpp", 10, 0),  // called from here
        FileLineColLoc("main.cpp", 20, 0)       // which was called from here
    )
);
```

A debugger then shows this as an inlined call stack — the user sees exactly which chain of calls led to each JIT'd instruction.

---

## Detailed Design

### Step 1: Add Parent Pointer to TrieNode

**Problem:** The current `TrieNode` only has children. Given a `Tag*` (leaf node), we cannot walk back to the root to reconstruct the full address chain.

**Solution:** Add a `parent` pointer to `TrieNode`.

```cpp
// Modified: src/nautilus/tracing/tag/Trie.hpp
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

    // NEW: Walk parent chain to collect full address sequence (leaf → root)
    std::vector<T> getAddressChain() const {
        std::vector<T> chain;
        for (auto* node = this; node->parent_ != nullptr; node = node->parent_) {
            chain.push_back(node->content);
        }
        return chain;  // innermost first, outermost last
    }

    T getContent() const { return content; }
    TrieNode<T>* getParent() const { return parent_; }

private:
    T content;
    TrieNode<T>* parent_;
    std::vector<std::unique_ptr<TrieNode<T>>> children;
};
```

**Impact:** Minimal — adds one pointer per node. The trie is already long-lived (owned by `TagRecorder`).

---

### Step 2: SourceLocation Struct and Chain

```cpp
// NEW: src/nautilus/tracing/tag/SourceLocation.hpp
namespace nautilus::tracing {

struct SourceLocation {
    std::string file;
    uint32_t line = 0;
    uint32_t column = 0;
    std::string function;

    static SourceLocation unknown();
    bool isValid() const { return line != 0 && !file.empty(); }
};

// A chain of source locations representing the call hierarchy
// Index 0 = innermost (where the operation is), last = outermost caller
using SourceLocationChain = std::vector<SourceLocation>;

} // namespace nautilus::tracing
```

---

### Step 3: SourceLocationResolver — Addresses to File:Line

```cpp
// NEW: src/nautilus/tracing/tag/SourceLocationResolver.hpp
namespace nautilus::tracing {

class SourceLocationResolver {
public:
    /// Resolve a Tag* into a full call hierarchy of source locations.
    /// Walks parent pointers to get address chain, resolves each address,
    /// filters out nautilus-internal frames (frames in src/nautilus/).
    SourceLocationChain resolve(Tag* tag) const;

    /// Resolve a single address to a source location.
    SourceLocation resolve(TagAddress address) const;

private:
    /// Filter out frames that are internal to nautilus (val<T> operators, tracing, etc.)
    bool isUserFrame(const SourceLocation& loc) const;

    mutable std::unordered_map<TagAddress, SourceLocation> cache_;
};

} // namespace nautilus::tracing
```

**Resolution uses `backward-cpp`** (already in `third_party/`):
```cpp
SourceLocation SourceLocationResolver::resolve(TagAddress address) const {
    if (auto it = cache_.find(address); it != cache_.end()) {
        return it->second;
    }

    backward::StackTrace st;
    // Load from a single address
    void* addr = reinterpret_cast<void*>(address);
    st.load_from(&addr, 1);

    backward::TraceResolver resolver;
    resolver.load_stacktrace(st);
    auto trace = resolver.resolve(st[0]);

    SourceLocation loc;
    if (!trace.source.filename.empty()) {
        loc.file = trace.source.filename;
        loc.line = trace.source.line;
        loc.function = trace.source.function;
    }

    cache_[address] = loc;
    return loc;
}

SourceLocationChain SourceLocationResolver::resolve(Tag* tag) const {
    auto addresses = tag->getAddressChain();  // innermost first
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
    // Filter out nautilus internals
    return loc.file.find("nautilus/src/nautilus/") == std::string::npos
        && loc.file.find("nautilus/include/nautilus/") == std::string::npos;
}
```

---

### Step 4: Attach SourceLocationChain to TraceOperation

```cpp
// Modified: TraceOperation.hpp
struct TraceOperation {
    Snapshot tag;
    Op op;
    Type resultType;
    TypedValueRef resultRef;
    std::vector<InputVariant> input;
    SourceLocationChain sourceLocations;  // NEW: full call hierarchy
};
```

**When to resolve:** After tracing completes, before IR conversion. A new `SourceLocationResolutionPhase` iterates all `TraceOperation`s:

```cpp
// NEW: src/nautilus/tracing/phases/SourceLocationResolutionPhase.hpp
class SourceLocationResolutionPhase {
public:
    static void apply(ExecutionTrace& trace);
};

// Implementation
void SourceLocationResolutionPhase::apply(ExecutionTrace& trace) {
    SourceLocationResolver resolver;
    for (auto& block : trace.getBlocks()) {
        for (auto& op : block.operations()) {
            op.sourceLocations = resolver.resolve(op.tag.getTag());
        }
    }
}
```

This runs **once**, after tracing, with caching — so it doesn't slow down the hot tracing loop.

---

### Step 5: Propagate to IR Operations

```cpp
// Modified: Operation.hpp
class Operation {
    OperationType opType;
    OperationIdentifier identifier;
    Type stamp;
    std::vector<Operation*> inputs;
    tracing::SourceLocationChain sourceLocations;  // NEW
};
```

`TraceToIRConversionPhase::processOperation()` copies `sourceLocations` when creating each IR `Operation*`.

---

### Step 6: Build Nested CallSiteLoc in MLIR Lowering

This is where the call hierarchy becomes debugger-visible.

```cpp
// Modified: MLIRLoweringProvider
mlir::Location MLIRLoweringProvider::getLocation(const ir::Operation* op) {
    auto& chain = op->getSourceLocations();
    if (chain.empty()) {
        return mlir::UnknownLoc::get(context);
    }

    // Build FileLineColLoc for each frame
    std::vector<mlir::Location> frameLocs;
    for (auto& loc : chain) {
        frameLocs.push_back(mlir::FileLineColLoc::get(
            builder->getStringAttr(loc.file), loc.line, loc.column));
    }

    // Build nested CallSiteLoc chain: inner ← mid ← outer
    // chain[0] = innermost (actual operation site)
    // chain[n] = outermost caller
    mlir::Location result = frameLocs[0];
    for (size_t i = 1; i < frameLocs.size(); ++i) {
        result = mlir::CallSiteLoc::get(result, frameLocs[i]);
    }

    return result;
}
```

**Result in debugger:** When stopped at a JIT'd instruction, gdb/lldb shows:
```
#0  helper.cpp:5     (x + 1)
#1  process.cpp:10   (helper(x))
#2  main.cpp:20      (process(x))
```

---

### Step 7: DWARF Debug Info + GDB JIT Registration

For the debugger to actually use these locations, we need:

1. **DICompileUnit + DISubprogram** — Attach to the LLVM function after MLIR→LLVM lowering. A custom LLVM pass in `src/nautilus/llvm-passes/` creates:
   - `DIFile` for each unique source file in the location chain
   - `DICompileUnit` for the JIT'd module
   - `DISubprogram` for the JIT'd function
   - Debug locations on each LLVM instruction (from the MLIR locations)

2. **GDB JIT Interface** — Enable `enableGDBNotificationListener` on `mlir::ExecutionEngine` so the debugger is notified when JIT'd code is loaded.

3. **Reduced optimization** — When debug mode is on, use `-O0` or `-O1` to preserve variable liveness and instruction ordering.

---

### Step 8: Value Representation in Debugger

**Goal:** `val<T>` values visible as named locals when stopped in debugger.

**Approach:** After MLIR→LLVM lowering, a custom LLVM pass inserts `llvm.dbg.value` intrinsics.

**Variable naming strategy** (two tiers):

1. **Automatic (default):** Name based on IR value ref: `v0`, `v1`, `v2`, etc. Combined with source locations, the user can correlate values.

2. **User-annotated (opt-in):** A lightweight API to attach names during tracing:
   ```cpp
   val<int64_t> counter = 0;
   nautilus::debug::name(counter, "counter");
   ```
   This records a `(ValueRef → "counter")` mapping in `TraceContext`, propagated through the pipeline.

**LLVM pass implementation:**
```cpp
// NEW: src/nautilus/llvm-passes/DebugVariablePass.hpp
class DebugVariablePass : public llvm::PassInfoMixin<DebugVariablePass> {
public:
    llvm::PreservedAnalyses run(llvm::Function& F, llvm::FunctionAnalysisManager& AM);
};

// For each SSA value with a debug location:
// 1. Create DILocalVariable with the value's name and type
// 2. Insert llvm.dbg.value at the definition point
// 3. Map nautilus types to DWARF base types (int64 → DW_ATE_signed)
```

---

## Configuration

Extend the existing `Options` system:

```cpp
options["debug.enabled"] = false;          // Master switch for all debug features
options["debug.optimization_level"] = 0;   // Override opt level when debugging
```

When `debug.enabled` is true:
- Source locations resolved from tags (full call hierarchy)
- MLIR ops get nested `CallSiteLoc` locations
- LLVM debug info (DICompileUnit, DISubprogram) is generated
- `llvm.dbg.value` intrinsics emitted for value visibility
- GDB JIT registration enabled
- Optimization reduced to preserve debug fidelity

---

## Implementation Order

| Phase | What | Files |
|-------|------|-------|
| 1 | Add parent pointer to `TrieNode` | `Trie.hpp` |
| 2 | `SourceLocation` struct + `SourceLocationChain` | New: `tag/SourceLocation.hpp` |
| 3 | `SourceLocationResolver` (backward-cpp) | New: `tag/SourceLocationResolver.hpp/.cpp` |
| 4 | `SourceLocationResolutionPhase` | New: `tracing/phases/SourceLocationResolutionPhase.hpp/.cpp` |
| 5 | Attach chain to `TraceOperation` + `Operation` | `TraceOperation.hpp`, `Operation.hpp` |
| 6 | Propagate in `TraceToIRConversionPhase` | `TraceToIRConversionPhase.cpp` |
| 7 | Build `CallSiteLoc` chain in MLIR lowering | `MLIRLoweringProvider.hpp/.cpp` |
| 8 | LLVM debug info pass (DICompileUnit, DISubprogram) | New: `llvm-passes/DebugInfoPass.hpp/.cpp` |
| 9 | LLVM debug variable pass (llvm.dbg.value) | New: `llvm-passes/DebugVariablePass.hpp/.cpp` |
| 10 | GDB JIT registration in ExecutionEngine | `JITCompiler.cpp` |
| 11 | Options integration (`debug.enabled`) | `options.hpp`, `JITCompiler.cpp` |
| 12 | Tests | New test files |

---

## Key Design Decisions

| Decision | Choice | Rationale |
|----------|--------|-----------|
| Call hierarchy representation | Nested `CallSiteLoc` | Standard MLIR/LLVM mechanism; debuggers natively display inlined frames |
| Walking tag chain | Add parent pointer to TrieNode | Minimal overhead (1 ptr/node), enables upward traversal |
| Frame filtering | Exclude `nautilus/src/` and `nautilus/include/` paths | User only cares about their code, not val<T> internals |
| When to resolve | Post-tracing phase with caching | Doesn't slow hot tracing loop; addresses are stable within process |
| Resolution library | backward-cpp (already bundled) | No new dependencies |
| Variable naming | Positional (v0, v1) + opt-in `debug::name()` | Pragmatic balance; C++ can't expose variable names at runtime |
| Scope | MLIR backend only | Primary backend, most impactful |
| Snapshot.tag access | Add `getTag()` accessor to Snapshot | Currently private; needed by resolution phase |

---

## Files Summary

### New Files
- `src/nautilus/tracing/tag/SourceLocation.hpp` — Struct + chain typedef
- `src/nautilus/tracing/tag/SourceLocationResolver.hpp/.cpp` — Address→file:line resolution
- `src/nautilus/tracing/phases/SourceLocationResolutionPhase.hpp/.cpp` — Post-tracing phase
- `src/nautilus/llvm-passes/DebugInfoPass.hpp/.cpp` — DICompileUnit/DISubprogram creation
- `src/nautilus/llvm-passes/DebugVariablePass.hpp/.cpp` — llvm.dbg.value insertion

### Modified Files
- `src/nautilus/tracing/tag/Trie.hpp` — Add parent pointer + getAddressChain()
- `src/nautilus/tracing/Snapshot.hpp` — Add getTag() accessor
- `src/nautilus/tracing/TraceOperation.hpp` — Add sourceLocations field
- `src/nautilus/compiler/ir/operations/Operation.hpp` — Add sourceLocations field
- `src/nautilus/tracing/phases/TraceToIRConversionPhase.cpp` — Copy sourceLocations
- `src/nautilus/compiler/backends/mlir/MLIRLoweringProvider.hpp/.cpp` — CallSiteLoc chain
- `src/nautilus/compiler/backends/mlir/JITCompiler.cpp` — GDB JIT registration
- `src/nautilus/compiler/backends/mlir/LLVMIROptimizer.cpp` — Debug passes integration
- `include/nautilus/options.hpp` or engine options — debug.enabled flag

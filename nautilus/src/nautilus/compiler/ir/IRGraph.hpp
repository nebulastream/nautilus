
#pragma once

#include "nautilus/JITCompiler.hpp"
#include "nautilus/common/Arena.hpp"
#include "nautilus/compiler/ir/operations/Operation.hpp"
#include "nautilus/tracing/tag/SourceLocationResolver.hpp"
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace nautilus::compiler::ir {

class FunctionOperation;

/// Per-operation debug metadata baked at IR-conversion time.
///
/// Resolution of the live trace @c Tag chain happens once during
/// @c TraceToIRConversionPhase, while the @c TagRecorder is still in
/// scope, and the result is stashed on the @c IRGraph keyed by
/// @c OperationIdentifier. The IR is then self-contained: the trace
/// arena (and the trie owned by @c TagRecorder) can be torn down,
/// passes can run, and the dump path can resolve @c (file, line, ...)
/// from the side-table without re-entering any resolver.
///
/// Frames are stored outer-to-inner: element 0 is the caller furthest
/// from the user-visible source site, the last element is the innermost
/// user frame. @c variableName, when present, is the user-declared
/// name recovered from DWARF at the innermost frame's coordinates
/// (e.g. "sum", "factor"); empty when no DWARF was available or no
/// matching DIE was found.
struct OperationDebugInfo {
	std::vector<tracing::SourceFrame> frames;
	std::optional<std::string> variableName;

	[[nodiscard]] bool empty() const noexcept {
		return frames.empty() && !variableName.has_value();
	}
};

/// Options that control how `IRGraph::toString` renders the graph.  Default-
/// constructed options reproduce the historic output byte-for-byte.
struct IRPrintOptions {
	/// When true, the per-op trailer prints the source-location stack
	/// recorded on the @c IRGraph side-table during IR conversion.
	bool showSourceLocations = false;

	/// When true and the side-table carries a recovered variable name
	/// for the op, the trailer adds a @c [var=<name>] tail. Implies
	/// @c showSourceLocations: variable annotations live on the same
	/// line as the source-location trailer.
	bool showVariableNames = false;
};

/**
 * @brief The IRGraph represents a fragment of nautilus ir.
 *
 * All IR nodes (FunctionOperation, BasicBlock, Operation subclasses,
 * BasicBlockArgument) referenced by this graph are allocated from a
 * `common::Arena` that the graph owns through a `common::ArenaPool::Handle`
 * for its full lifetime.  The graph itself owns no IR node directly: it
 * only stores raw pointers into the arena.  When the IRGraph is destroyed
 * the Handle is destroyed; depending on whether the Handle is pool-backed
 * the Arena is either recycled into its pool (typical, engine path) or
 * deleted outright (standalone path, used by tests and benchmarks).
 *
 * Each IRGraph holds its own Arena — distinct from the engine-scoped trace
 * Arena — so the IR survives across `compile()` cycles (e.g. while the
 * tiered compiler caches it for asynchronous tier-1 promotion) without
 * being invalidated by the trace arena's `softReset()`.
 */
class IRGraph {
public:
	/// Constructs an IR graph backed by a freshly heap-allocated standalone
	/// Arena (no pool).  Used by tests and benchmarks.
	explicit IRGraph(const CompilationUnitID& id);

	/// Constructs an IR graph backed by the supplied Arena Handle.  When
	/// the Handle is pool-backed the Arena is recycled on destruction.
	IRGraph(common::ArenaPool::Handle arena, const CompilationUnitID& id);

	~IRGraph() = default;

	/**
	 * @brief Adds a function operation to the IR graph
	 * @param functionOperation Pointer to the arena-allocated function op
	 * @return The same pointer, for convenience
	 */
	FunctionOperation* addFunctionOperation(FunctionOperation* functionOperation);

	/**
	 * @brief Gets all function operations in the IR graph
	 * @return Vector of function operations
	 */
	const std::vector<FunctionOperation*>& getFunctionOperations() const;

	/**
	 * @brief Gets a specific function operation by name in O(1) via an internal index.
	 * @param name The name of the function
	 * @return Pointer to the function operation if found, nullptr otherwise
	 */
	const FunctionOperation* getFunctionOperation(const std::string& name) const;

	std::string toString() const;

	std::string toString(const IRPrintOptions& options) const;

	[[nodiscard]] const CompilationUnitID& getId() const;

	/// Returns the arena that backs every IR node in this graph.  Phases
	/// that mint new nodes (e.g. TraceToIRConversionPhase) allocate through
	/// this arena.
	[[nodiscard]] common::Arena& getArena() const {
		return *arena_;
	}

	/// Records resolved debug metadata for an operation. Merges with any
	/// existing entry: empty fields in @p info never overwrite a non-
	/// empty existing field, so callers can attach the source-location
	/// stack and the DWARF-resolved variable name in either order.
	void setDebugInfo(OperationIdentifier id, OperationDebugInfo info);

	/// Returns the recorded debug metadata for @p id, or @c nullptr if
	/// nothing was captured for it. The common (option-off) case is
	/// always @c nullptr — the side-table is empty.
	[[nodiscard]] const OperationDebugInfo* getDebugInfo(OperationIdentifier id) const;

	/// Read-only access to the whole side-table. Useful for iterating
	/// the resolved metadata without going through the per-op accessor.
	[[nodiscard]] const std::unordered_map<OperationIdentifier, OperationDebugInfo>& getDebugInfoMap() const {
		return debugInfoByOpId_;
	}

private:
	common::ArenaPool::Handle arena_;
	std::vector<FunctionOperation*> functionOperations;
	// Name -> function pointer. The string_view is backed by the name field
	// owned by FunctionOperation, which is stable for the lifetime of the
	// graph (the FunctionOperation itself lives in the arena).
	std::unordered_map<std::string_view, FunctionOperation*> functionOperationsByName;
	/// Resolved debug metadata side-table, keyed by operation id.
	/// Populated only when the user opted into source-location capture
	/// at compile time. Empty otherwise — zero overhead for the default
	/// path.
	std::unordered_map<OperationIdentifier, OperationDebugInfo> debugInfoByOpId_;
	const CompilationUnitID id;
};

} // namespace nautilus::compiler::ir

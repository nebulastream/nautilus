
#pragma once

#include "nautilus/JITCompiler.hpp"
#include "nautilus/common/Arena.hpp"
#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>

namespace nautilus::compiler::ir {

class FunctionOperation;

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

	[[nodiscard]] const CompilationUnitID& getId() const;

	/// Returns the arena that backs every IR node in this graph.  Phases
	/// that mint new nodes (e.g. TraceToIRConversionPhase) allocate through
	/// this arena.
	[[nodiscard]] common::Arena& getArena() const {
		return *arena_;
	}

private:
	common::ArenaPool::Handle arena_;
	std::vector<FunctionOperation*> functionOperations;
	// Name -> function pointer. The string_view is backed by the name field
	// owned by FunctionOperation, which is stable for the lifetime of the
	// graph (the FunctionOperation itself lives in the arena).
	std::unordered_map<std::string_view, FunctionOperation*> functionOperationsByName;
	const CompilationUnitID id;
};

} // namespace nautilus::compiler::ir

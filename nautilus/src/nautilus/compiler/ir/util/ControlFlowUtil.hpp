
#pragma once

#include "nautilus/compiler/ir/IRGraph.hpp"
#include "nautilus/compiler/ir/blocks/BasicBlock.hpp"
#include "nautilus/compiler/ir/blocks/BasicBlockInvocation.hpp"
#include "nautilus/compiler/ir/operations/Operation.hpp"
#include <vector>

namespace nautilus::compiler::ir {

/**
 * @brief Return the outgoing `BasicBlockInvocation`s of a terminator operation.
 *
 * Non-terminator operations return an empty vector. The implementation
 * switches on `Operation::OperationType` and uses the existing `as<T>`
 * cast helpers — deliberately not a virtual method, matching the
 * no-vtable-on-`Operation` design enforced by the arena allocator and
 * documented in `OperationDispatcher.hpp:48-51`.
 *
 * The returned pointers refer to invocation sub-objects owned by @p op.
 * Callers must not outlive @p op.
 */
std::vector<BasicBlockInvocation*> getSuccessorInvocations(Operation& op);

/**
 * @brief Return the successor blocks of a terminator operation.
 *
 * Thin wrapper over @ref getSuccessorInvocations that projects each
 * invocation to its target block. Non-terminators return `{}`.
 */
std::vector<BasicBlock*> getSuccessors(Operation& op);

/**
 * @brief Rebuild every block's predecessor list from the CFG.
 *
 * `IRGraph`s emerging from `TraceToIRConversionPhase` do not carry
 * predecessor lists — `BasicBlockInvocation::setBlock` is a plain setter
 * and initial construction does not know the enclosing block. The pass
 * manager calls this once at the start of `run()` so subsequent passes
 * can rely on `BasicBlock::getPredecessors()` and the mutating helpers
 * (`replaceSuccessor`, `addNextBlock`, `replaceTerminatorOperation`)
 * maintain the invariant from there.
 */
void rebuildPredecessorLists(IRGraph& ir);

} // namespace nautilus::compiler::ir

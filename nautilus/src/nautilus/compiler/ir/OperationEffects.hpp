#pragma once

#include "nautilus/compiler/ir/IRGraph.hpp"
#include "nautilus/compiler/ir/operations/Operation.hpp"
#include "nautilus/compiler/ir/operations/OperationProperties.hpp"

namespace nautilus::compiler::ir {

/**
 * @brief What one operation may do to memory, and whether it can be moved.
 *
 * `OperationProperties` answers this per *opcode*, from a constexpr table.
 * That is exact for every operation whose effects are a property of the
 * instruction -- an add is pure whatever its operands -- but not for a call,
 * where the effects belong to the callee. The table therefore has to assume
 * the worst for both call opcodes, so every call is treated as reading and
 * writing all of memory even when the callee is known to do neither.
 *
 * These helpers ask the same question per *operation*: for a call they consult
 * the callee's entry in the module function table, and for everything else
 * they return exactly what the opcode table says. Passes that route through
 * here can move or eliminate a call the callee's attributes prove is safe;
 * passes that do not keep the old, conservative answer.
 */
struct OperationEffects {
	bool mayRead = true;
	bool mayWrite = true;
	/// Safe to remove when its result is unused, and safe to duplicate or
	/// hoist. Implied by "reads nothing, writes nothing, returns normally".
	bool isPure = false;
};

/// Effects of @p op in the context of @p ir.
///
/// @p ir supplies the function table used to resolve a call's callee. An
/// operation whose callee cannot be resolved (an indirect call, or an id the
/// table does not know) keeps the conservative answer.
[[nodiscard]] OperationEffects effectsOf(const IRGraph& ir, const Operation& op);

/// Convenience wrapper mirroring `isPureOp`, for pass code that only needs the
/// movable/removable question.
[[nodiscard]] inline bool isPureOperation(const IRGraph& ir, const Operation& op) {
	return effectsOf(ir, op).isPure;
}

} // namespace nautilus::compiler::ir

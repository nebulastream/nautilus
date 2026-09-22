#pragma once

#include "nautilus/compiler/ir/operations/CallOperation.hpp"
#include "nautilus/compiler/ir/operations/IndirectCallOperation.hpp"
#include <cstddef>

namespace nautilus::compiler::ir {

/// A potentially-throwing call records, per live destructor, the address of
/// the object its landing pad destroys on unwind. That address is an operand
/// the call reads just like its arguments, but it is stored in the call's
/// destructor list rather than in `Operation::inputs` (backends read inputs
/// as the call's argument list). Anything that tracks or rewrites uses must
/// visit these operands too, through the helpers below: otherwise a pass that
/// replaces or removes the address value -- e.g. block-argument pruning
/// dropping a loop-carried pointer whose only use is a destructor -- leaves
/// the landing pad pointing at an operation no longer in the function.

inline size_t getDestructorOperandCount(const Operation& op) {
	if (const auto* call = dyn_cast<CallOperation>(&op)) {
		return call->getDestructors().size();
	}
	if (const auto* indirect = dyn_cast<IndirectCallOperation>(&op)) {
		return indirect->getDestructors().size();
	}
	return 0;
}

inline Operation* getDestructorOperand(const Operation& op, size_t index) {
	if (const auto* call = dyn_cast<CallOperation>(&op)) {
		return call->getDestructors()[index].address;
	}
	return cast<IndirectCallOperation>(&op)->getDestructors()[index].address;
}

inline void setDestructorOperand(Operation& op, size_t index, Operation* address) {
	if (auto* call = dyn_cast<CallOperation>(&op)) {
		call->setDestructorAddress(index, address);
		return;
	}
	cast<IndirectCallOperation>(&op)->setDestructorAddress(index, address);
}

} // namespace nautilus::compiler::ir

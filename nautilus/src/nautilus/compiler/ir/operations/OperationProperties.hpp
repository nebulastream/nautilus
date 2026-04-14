
#pragma once

#include "nautilus/compiler/ir/operations/Operation.hpp"
#include <array>
#include <cstdint>

namespace nautilus::compiler::ir {

/**
 * @brief Compile-time property table keyed by Operation::OperationType.
 *
 * Properties are packed into a single byte per op type so the table fits
 * trivially in L1 and every query lowers to a constant-indexed array load
 * followed by a mask. Use the helpers below instead of ad-hoc type chains.
 */
struct OpFlags {
	static constexpr uint8_t None = 0;
	static constexpr uint8_t Constant = 1u << 0;    // produces a compile-time constant value
	static constexpr uint8_t Terminator = 1u << 1;  // ends a basic block (branch/if/return)
	static constexpr uint8_t MayReadMem = 1u << 2;  // loads from memory
	static constexpr uint8_t MayWriteMem = 1u << 3; // stores or calls that may mutate memory
	static constexpr uint8_t Pure = 1u << 4;        // side-effect-free and deterministic
};

namespace detail {

using OpType = Operation::OperationType;

// Size the table by the largest enumerator we use. The enum is `uint8_t`, so
// a 256-entry table is safe regardless of which value is highest.
constexpr std::size_t kOpFlagTableSize = 256;

constexpr std::array<uint8_t, kOpFlagTableSize> buildOpFlagTable() {
	std::array<uint8_t, kOpFlagTableSize> t {};

	auto set = [&](OpType op, uint8_t flags) {
		t[static_cast<std::size_t>(op)] = flags;
	};

	// Constants: pure and marked Constant.
	set(OpType::ConstIntOp, OpFlags::Constant | OpFlags::Pure);
	set(OpType::ConstFloatOp, OpFlags::Constant | OpFlags::Pure);
	set(OpType::ConstBooleanOp, OpFlags::Constant | OpFlags::Pure);
	set(OpType::ConstPtrOp, OpFlags::Constant | OpFlags::Pure);

	// Pure arithmetic / logical / bit / compare / cast / select.
	for (auto op : {OpType::AddOp, OpType::SubOp, OpType::MulOp, OpType::DivOp, OpType::ModOp, OpType::AndOp,
	                OpType::OrOp, OpType::NotOp, OpType::NegateOp, OpType::ShiftOp, OpType::CompareOp,
	                OpType::BinaryComp, OpType::CastOp, OpType::SelectOp}) {
		set(op, OpFlags::Pure);
	}

	// Terminators.
	set(OpType::BranchOp, OpFlags::Terminator);
	set(OpType::IfOp, OpFlags::Terminator);
	set(OpType::ReturnOp, OpFlags::Terminator);

	// Memory ops.
	set(OpType::LoadOp, OpFlags::MayReadMem);
	set(OpType::StoreOp, OpFlags::MayWriteMem);

	// Calls may do anything.
	set(OpType::ProxyCallOp, OpFlags::MayReadMem | OpFlags::MayWriteMem);
	set(OpType::IndirectCallOp, OpFlags::MayReadMem | OpFlags::MayWriteMem);

	// The rest (alloca, function-op, function-address-of, block args/invocations, yield) have no flags.
	return t;
}

inline constexpr auto kOpFlagTable = buildOpFlagTable();

constexpr uint8_t flagsFor(OpType op) {
	return kOpFlagTable[static_cast<std::size_t>(op)];
}

} // namespace detail

constexpr bool isConstantOp(Operation::OperationType op) {
	return (detail::flagsFor(op) & OpFlags::Constant) != 0;
}

constexpr bool isTerminatorOp(Operation::OperationType op) {
	return (detail::flagsFor(op) & OpFlags::Terminator) != 0;
}

constexpr bool isPureOp(Operation::OperationType op) {
	return (detail::flagsFor(op) & OpFlags::Pure) != 0;
}

constexpr bool mayReadMemory(Operation::OperationType op) {
	return (detail::flagsFor(op) & OpFlags::MayReadMem) != 0;
}

constexpr bool mayWriteMemory(Operation::OperationType op) {
	return (detail::flagsFor(op) & OpFlags::MayWriteMem) != 0;
}

} // namespace nautilus::compiler::ir

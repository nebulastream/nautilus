#pragma once

#include "nautilus/compiler/ir/operations/AllocaOperation.hpp"
#include "nautilus/compiler/ir/operations/ArithmeticOperations/AddOperation.hpp"
#include "nautilus/compiler/ir/operations/ArithmeticOperations/DivOperation.hpp"
#include "nautilus/compiler/ir/operations/ArithmeticOperations/ModOperation.hpp"
#include "nautilus/compiler/ir/operations/ArithmeticOperations/MulOperation.hpp"
#include "nautilus/compiler/ir/operations/ArithmeticOperations/SubOperation.hpp"
#include "nautilus/compiler/ir/operations/BinaryOperations/BinaryCompOperation.hpp"
#include "nautilus/compiler/ir/operations/BinaryOperations/NegateOperation.hpp"
#include "nautilus/compiler/ir/operations/BinaryOperations/ShiftOperation.hpp"
#include "nautilus/compiler/ir/operations/BranchOperation.hpp"
#include "nautilus/compiler/ir/operations/CastOperation.hpp"
#include "nautilus/compiler/ir/operations/ConstBooleanOperation.hpp"
#include "nautilus/compiler/ir/operations/ConstFloatOperation.hpp"
#include "nautilus/compiler/ir/operations/ConstIntOperation.hpp"
#include "nautilus/compiler/ir/operations/ConstPtrOperation.hpp"
#include "nautilus/compiler/ir/operations/FunctionAddressOfOperation.hpp"
#include "nautilus/compiler/ir/operations/FunctionOperation.hpp"
#include "nautilus/compiler/ir/operations/IfOperation.hpp"
#include "nautilus/compiler/ir/operations/IndirectCallOperation.hpp"
#include "nautilus/compiler/ir/operations/LoadOperation.hpp"
#include "nautilus/compiler/ir/operations/LogicalOperations/AndOperation.hpp"
#include "nautilus/compiler/ir/operations/LogicalOperations/CompareOperation.hpp"
#include "nautilus/compiler/ir/operations/LogicalOperations/NotOperation.hpp"
#include "nautilus/compiler/ir/operations/LogicalOperations/OrOperation.hpp"
#include "nautilus/compiler/ir/operations/Operation.hpp"
#include "nautilus/compiler/ir/operations/ProxyCallOperation.hpp"
#include "nautilus/compiler/ir/operations/ReturnOperation.hpp"
#include "nautilus/compiler/ir/operations/SelectOperation.hpp"
#include "nautilus/compiler/ir/operations/StoreOperation.hpp"
#include "nautilus/exceptions/NotImplementedException.hpp"
#include <memory>
#include <string>
#include <utility>

namespace nautilus::compiler::ir {

/**
 * @brief CRTP base that owns the single `switch (op->getOperationType())` for
 * lowering IR to any backend.
 *
 * Each backend lowering provider inherits `OperationDispatcher<Self>` and
 * overrides the `visitXxx` hooks it cares about. Extra per-backend arguments
 * (e.g. a `short` block index, a frame reference) are forwarded verbatim via
 * `Args&&...`, so every backend keeps its natural call signature.
 *
 * Why CRTP and not virtuals: the rest of the IR layer deliberately avoids
 * virtual dispatch on `Operation` (see the LLVM-style `classof`/`as` helpers
 * in `Operation.hpp`). Mirroring that philosophy here keeps the dispatch a
 * single inlineable switch with zero indirect calls.
 *
 * Why the `switch` has no `default:` label: with `-Wswitch-enum` (or even
 * the default warning level on Clang) this turns any newly-added
 * `Operation::OperationType` enumerator into a compile error until every
 * backend wires it in, replacing the previous "silently missing case" class
 * of bug.
 *
 * Default behaviour:
 *  - Non-structural ops (the 27 ops that actually carry semantics) fall back
 *    to throwing `NotImplementedException` if a backend forgets to override.
 *  - Structural ops (`FunctionOp`, `BasicBlockArgument`, `BlockInvocation`,
 *    `MLIR_YIELD`) fall back to a no-op, since every current backend treats
 *    them that way.
 */
template <typename Derived>
class OperationDispatcher {
public:
	/// Dispatch a single operation to the matching `visitXxx` hook on Derived.
	template <typename... Args>
	void dispatch(Operation* op, Args&&... args) {
		using OT = Operation::OperationType;
		auto& d = static_cast<Derived&>(*this);
		switch (op->getOperationType()) {
		// Arithmetic
		case OT::AddOp:
			d.visitAdd(as<AddOperation>(op), std::forward<Args>(args)...);
			return;
		case OT::SubOp:
			d.visitSub(as<SubOperation>(op), std::forward<Args>(args)...);
			return;
		case OT::MulOp:
			d.visitMul(as<MulOperation>(op), std::forward<Args>(args)...);
			return;
		case OT::DivOp:
			d.visitDiv(as<DivOperation>(op), std::forward<Args>(args)...);
			return;
		case OT::ModOp:
			d.visitMod(as<ModOperation>(op), std::forward<Args>(args)...);
			return;

		// Logical / bitwise
		case OT::AndOp:
			d.visitAnd(as<AndOperation>(op), std::forward<Args>(args)...);
			return;
		case OT::OrOp:
			d.visitOr(as<OrOperation>(op), std::forward<Args>(args)...);
			return;
		case OT::NotOp:
			d.visitNot(as<NotOperation>(op), std::forward<Args>(args)...);
			return;
		case OT::NegateOp:
			d.visitNegate(as<NegateOperation>(op), std::forward<Args>(args)...);
			return;
		case OT::ShiftOp:
			d.visitShift(as<ShiftOperation>(op), std::forward<Args>(args)...);
			return;
		case OT::BinaryComp:
			d.visitBinaryComp(as<BinaryCompOperation>(op), std::forward<Args>(args)...);
			return;
		case OT::CompareOp:
			d.visitCompare(as<CompareOperation>(op), std::forward<Args>(args)...);
			return;

		// Casts
		case OT::CastOp:
			d.visitCast(as<CastOperation>(op), std::forward<Args>(args)...);
			return;

		// Constants
		case OT::ConstIntOp:
			d.visitConstInt(as<ConstIntOperation>(op), std::forward<Args>(args)...);
			return;
		case OT::ConstFloatOp:
			d.visitConstFloat(as<ConstFloatOperation>(op), std::forward<Args>(args)...);
			return;
		case OT::ConstBooleanOp:
			d.visitConstBoolean(as<ConstBooleanOperation>(op), std::forward<Args>(args)...);
			return;
		case OT::ConstPtrOp:
			d.visitConstPtr(as<ConstPtrOperation>(op), std::forward<Args>(args)...);
			return;

		// Memory
		case OT::LoadOp:
			d.visitLoad(as<LoadOperation>(op), std::forward<Args>(args)...);
			return;
		case OT::StoreOp:
			d.visitStore(as<StoreOperation>(op), std::forward<Args>(args)...);
			return;
		case OT::AllocaOp:
			d.visitAlloca(as<AllocaOperation>(op), std::forward<Args>(args)...);
			return;

		// Control flow
		case OT::IfOp:
			d.visitIf(as<IfOperation>(op), std::forward<Args>(args)...);
			return;
		case OT::BranchOp:
			d.visitBranch(as<BranchOperation>(op), std::forward<Args>(args)...);
			return;
		case OT::ReturnOp:
			d.visitReturn(as<ReturnOperation>(op), std::forward<Args>(args)...);
			return;
		case OT::SelectOp:
			d.visitSelect(as<SelectOperation>(op), std::forward<Args>(args)...);
			return;

		// Calls
		case OT::ProxyCallOp:
			d.visitProxyCall(as<ProxyCallOperation>(op), std::forward<Args>(args)...);
			return;
		case OT::IndirectCallOp:
			d.visitIndirectCall(as<IndirectCallOperation>(op), std::forward<Args>(args)...);
			return;
		case OT::FunctionAddressOfOp:
			d.visitFunctionAddressOf(as<FunctionAddressOfOperation>(op), std::forward<Args>(args)...);
			return;

		// Structural ops: defaults are no-op; overridable if a backend cares.
		case OT::FunctionOp:
			d.visitFunction(as<FunctionOperation>(op), std::forward<Args>(args)...);
			return;
		case OT::BasicBlockArgument:
			d.visitBasicBlockArgument(op, std::forward<Args>(args)...);
			return;
		case OT::BlockInvocation:
			d.visitBlockInvocation(op, std::forward<Args>(args)...);
			return;
		case OT::MLIR_YIELD:
			d.visitMlirYield(op, std::forward<Args>(args)...);
			return;
		}
		// No default: adding an enumerator to OperationType is a compile error
		// (under -Wswitch-enum) until every backend wires it in. The throw is
		// only reachable if a corrupted uint8_t ever slips in.
		throw NotImplementedException("OperationDispatcher: unknown OperationType");
	}

	// ── Default hooks ────────────────────────────────────────────────────────
	// Non-structural ops: throw if a backend forgot to override.
	template <typename... Args>
	void visitAdd(AddOperation*, Args&&...) {
		unhandled("AddOp");
	}
	template <typename... Args>
	void visitSub(SubOperation*, Args&&...) {
		unhandled("SubOp");
	}
	template <typename... Args>
	void visitMul(MulOperation*, Args&&...) {
		unhandled("MulOp");
	}
	template <typename... Args>
	void visitDiv(DivOperation*, Args&&...) {
		unhandled("DivOp");
	}
	template <typename... Args>
	void visitMod(ModOperation*, Args&&...) {
		unhandled("ModOp");
	}
	template <typename... Args>
	void visitAnd(AndOperation*, Args&&...) {
		unhandled("AndOp");
	}
	template <typename... Args>
	void visitOr(OrOperation*, Args&&...) {
		unhandled("OrOp");
	}
	template <typename... Args>
	void visitNot(NotOperation*, Args&&...) {
		unhandled("NotOp");
	}
	template <typename... Args>
	void visitNegate(NegateOperation*, Args&&...) {
		unhandled("NegateOp");
	}
	template <typename... Args>
	void visitShift(ShiftOperation*, Args&&...) {
		unhandled("ShiftOp");
	}
	template <typename... Args>
	void visitBinaryComp(BinaryCompOperation*, Args&&...) {
		unhandled("BinaryComp");
	}
	template <typename... Args>
	void visitCompare(CompareOperation*, Args&&...) {
		unhandled("CompareOp");
	}
	template <typename... Args>
	void visitCast(CastOperation*, Args&&...) {
		unhandled("CastOp");
	}
	template <typename... Args>
	void visitConstInt(ConstIntOperation*, Args&&...) {
		unhandled("ConstIntOp");
	}
	template <typename... Args>
	void visitConstFloat(ConstFloatOperation*, Args&&...) {
		unhandled("ConstFloatOp");
	}
	template <typename... Args>
	void visitConstBoolean(ConstBooleanOperation*, Args&&...) {
		unhandled("ConstBooleanOp");
	}
	template <typename... Args>
	void visitConstPtr(ConstPtrOperation*, Args&&...) {
		unhandled("ConstPtrOp");
	}
	template <typename... Args>
	void visitLoad(LoadOperation*, Args&&...) {
		unhandled("LoadOp");
	}
	template <typename... Args>
	void visitStore(StoreOperation*, Args&&...) {
		unhandled("StoreOp");
	}
	template <typename... Args>
	void visitAlloca(AllocaOperation*, Args&&...) {
		unhandled("AllocaOp");
	}
	template <typename... Args>
	void visitIf(IfOperation*, Args&&...) {
		unhandled("IfOp");
	}
	template <typename... Args>
	void visitBranch(BranchOperation*, Args&&...) {
		unhandled("BranchOp");
	}
	template <typename... Args>
	void visitReturn(ReturnOperation*, Args&&...) {
		unhandled("ReturnOp");
	}
	template <typename... Args>
	void visitSelect(SelectOperation*, Args&&...) {
		unhandled("SelectOp");
	}
	template <typename... Args>
	void visitProxyCall(ProxyCallOperation*, Args&&...) {
		unhandled("ProxyCallOp");
	}
	template <typename... Args>
	void visitIndirectCall(IndirectCallOperation*, Args&&...) {
		unhandled("IndirectCallOp");
	}
	template <typename... Args>
	void visitFunctionAddressOf(FunctionAddressOfOperation*, Args&&...) {
		unhandled("FunctionAddressOfOp");
	}

	// Structural ops: no-op defaults (every current backend skips these).
	template <typename... Args>
	void visitFunction(FunctionOperation*, Args&&...) {
	}
	template <typename... Args>
	void visitBasicBlockArgument(Operation*, Args&&...) {
	}
	template <typename... Args>
	void visitBlockInvocation(Operation*, Args&&...) {
	}
	template <typename... Args>
	void visitMlirYield(Operation*, Args&&...) {
	}

private:
	[[noreturn]] static void unhandled(const char* name) {
		throw NotImplementedException(std::string("OperationDispatcher: unhandled ") + name);
	}
};

} // namespace nautilus::compiler::ir

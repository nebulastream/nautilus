// fadec's FE_MEM macro uses aggregate initialization without nested braces.
// Suppress the resulting warning (Clang: -Wmissing-braces).
#ifdef __clang__
#pragma clang diagnostic ignored "-Wmissing-braces"
#endif

#include "NautilusCompiler.hpp"

#include "NautilusIRAdaptor.hpp"
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
#include "nautilus/compiler/ir/operations/IfOperation.hpp"
#include "nautilus/compiler/ir/operations/LoadOperation.hpp"
#include "nautilus/compiler/ir/operations/LogicalOperations/AndOperation.hpp"
#include "nautilus/compiler/ir/operations/LogicalOperations/CompareOperation.hpp"
#include "nautilus/compiler/ir/operations/LogicalOperations/NotOperation.hpp"
#include "nautilus/compiler/ir/operations/LogicalOperations/OrOperation.hpp"
#include "nautilus/compiler/ir/operations/ProxyCallOperation.hpp"
#include "nautilus/compiler/ir/operations/ReturnOperation.hpp"
#include "nautilus/compiler/ir/operations/SelectOperation.hpp"
#include "nautilus/compiler/ir/operations/StoreOperation.hpp"
#include "nautilus/tracing/Types.hpp"
#include "tpde/x64/CompilerX64.hpp"
#include <span>

using namespace ::tpde::x64;

namespace nautilus::compiler::tpde {

using Type = nautilus::Type;
using OT = ir::Operation::OperationType;

// ─── Helpers ──────────────────────────────────────────────────────────────────

NautilusCompiler::NautilusValueParts NautilusCompiler::type_to_value_parts(Type t) {
	using namespace ::tpde::x64;
	switch (t) {
		case Type::f32:
			return {4, PlatformConfig::FP_BANK};
		case Type::f64:
			return {8, PlatformConfig::FP_BANK};
		case Type::b:
		case Type::i8:
		case Type::ui8:
			return {1, PlatformConfig::GP_BANK};
		case Type::i16:
		case Type::ui16:
			return {2, PlatformConfig::GP_BANK};
		case Type::i32:
		case Type::ui32:
			return {4, PlatformConfig::GP_BANK};
		case Type::i64:
		case Type::ui64:
		case Type::ptr:
			return {8, PlatformConfig::GP_BANK};
		default:
			return {8, PlatformConfig::GP_BANK};
	}
}

uint32_t NautilusCompiler::type_bytes(Type t) {
	switch (t) {
		case Type::b:
		case Type::i8:
		case Type::ui8:
			return 1;
		case Type::i16:
		case Type::ui16:
			return 2;
		case Type::i32:
		case Type::ui32:
		case Type::f32:
			return 4;
		case Type::i64:
		case Type::ui64:
		case Type::f64:
		case Type::ptr:
		default:
			return 8;
	}
}

uint32_t NautilusCompiler::gp_size(Type t) {
	return type_bytes(t);
}

bool NautilusCompiler::is_signed(Type t) {
	switch (t) {
		case Type::i8:
		case Type::i16:
		case Type::i32:
		case Type::i64:
			return true;
		default:
			return false;
	}
}

bool NautilusCompiler::is_float(Type t) {
	return t == Type::f32 || t == Type::f64;
}

// ─── compile_inst dispatch ────────────────────────────────────────────────────

bool NautilusCompiler::compile_inst(IRInstRef inst, InstRange /*remaining*/) {
	const ir::Operation* op = this->adaptor->get_op(inst);
	switch (op->getOperationType()) {
		case OT::ConstIntOp:
			return compile_const_int(inst);
		case OT::ConstBooleanOp:
			return compile_const_bool(inst);
		case OT::ConstFloatOp:
			return compile_const_float(inst);
		case OT::ConstPtrOp:
			return compile_const_ptr(inst);
		case OT::AddOp:
			return compile_add(inst);
		case OT::SubOp:
			return compile_sub(inst);
		case OT::MulOp:
			return compile_mul(inst);
		case OT::DivOp:
			return compile_div(inst);
		case OT::ModOp:
			return compile_mod(inst);
		case OT::AndOp:
			return compile_and(inst);
		case OT::OrOp:
			return compile_or(inst);
		case OT::NotOp:
			return compile_not(inst);
		case OT::NegateOp:
			return compile_negate(inst);
		case OT::BinaryComp:
			return compile_binary_comp(inst);
		case OT::ShiftOp:
			return compile_shift(inst);
		case OT::CompareOp:
			return compile_compare(inst);
		case OT::LoadOp:
			return compile_load(inst);
		case OT::StoreOp:
			return compile_store(inst);
		case OT::AllocaOp:
			return compile_alloca(inst);
		case OT::CastOp:
			return compile_cast(inst);
		case OT::SelectOp:
			return compile_select(inst);
		case OT::IfOp:
			return compile_if(inst);
		case OT::BranchOp:
			return compile_branch(inst);
		case OT::ReturnOp:
			return compile_return(inst);
		case OT::ProxyCallOp:
			return compile_proxy_call(inst);
		// Structural ops not emitted as instructions
		case OT::BasicBlockArgument:
		case OT::BlockInvocation:
		case OT::FunctionOp:
		case OT::LoopOp:
		case OT::MLIR_YIELD:
			return true;
		default:
			return false;
	}
}

// ─── Constants ────────────────────────────────────────────────────────────────

bool NautilusCompiler::compile_const_int(IRInstRef inst) {
	const auto* op = this->adaptor->get_op(inst)->dynCast<ir::ConstIntOperation>();
	auto [res_vr, res] = this->result_ref_single(IRValueRef{inst.id});
	AsmReg dst = res.alloc_reg();
	uint64_t val = static_cast<uint64_t>(op->getValue());
	uint32_t sz = gp_size(op->getStamp());
	if (sz <= 4) {
		ASM(MOV32ri, dst, static_cast<uint32_t>(val));
	} else {
		ASM(MOV64ri, dst, val);
	}
	res.set_modified();
	return true;
}

bool NautilusCompiler::compile_const_bool(IRInstRef inst) {
	const auto* op = this->adaptor->get_op(inst)->dynCast<ir::ConstBooleanOperation>();
	auto [res_vr, res] = this->result_ref_single(IRValueRef{inst.id});
	AsmReg dst = res.alloc_reg();
	ASM(MOV32ri, dst, op->getValue() ? 1u : 0u);
	res.set_modified();
	return true;
}

bool NautilusCompiler::compile_const_float(IRInstRef inst) {
	const auto* op = this->adaptor->get_op(inst)->dynCast<ir::ConstFloatOperation>();
	auto [res_vr, res] = this->result_ref_single(IRValueRef{inst.id});
	AsmReg dst = res.alloc_reg();
	if (op->getStamp() == Type::f32) {
		float fval = static_cast<float>(op->getValue());
		uint32_t bits;
		std::memcpy(&bits, &fval, 4);
		uint64_t bits64 = bits;
		this->materialize_constant(&bits64, PlatformConfig::FP_BANK, 4, dst);
	} else {
		double dval = op->getValue();
		uint64_t bits;
		std::memcpy(&bits, &dval, 8);
		this->materialize_constant(&bits, PlatformConfig::FP_BANK, 8, dst);
	}
	res.set_modified();
	return true;
}

bool NautilusCompiler::compile_const_ptr(IRInstRef inst) {
	const auto* op = this->adaptor->get_op(inst)->dynCast<ir::ConstPtrOperation>();
	auto [res_vr, res] = this->result_ref_single(IRValueRef{inst.id});
	AsmReg dst = res.alloc_reg();
	uint64_t ptr_val = reinterpret_cast<uint64_t>(op->getValue());
	ASM(MOV64ri, dst, ptr_val);
	res.set_modified();
	return true;
}

// ─── Arithmetic ───────────────────────────────────────────────────────────────

bool NautilusCompiler::compile_add(IRInstRef inst) {
	const auto* op = this->adaptor->get_op(inst)->dynCast<ir::AddOperation>();
	Type stamp = op->getStamp();

	IRValueRef lhs_ref{op->getLeftInput()->getIdentifier().getId()};
	IRValueRef rhs_ref{op->getRightInput()->getIdentifier().getId()};

	auto [lhs_vr, lhs] = this->val_ref_single(lhs_ref);
	auto [rhs_vr, rhs] = this->val_ref_single(rhs_ref);
	auto [res_vr, res] = this->result_ref_single(IRValueRef{inst.id});

	if (is_float(stamp)) {
		AsmReg l = lhs.load_to_reg();
		AsmReg r = rhs.load_to_reg();
		AsmReg d = res.alloc_try_reuse(lhs);
		if (stamp == Type::f32) {
			if (d != l)
				ASM(SSE_MOVSSrr, d, l);
			ASM(SSE_ADDSSrr, d, r);
		} else {
			if (d != l)
				ASM(SSE_MOVSDrr, d, l);
			ASM(SSE_ADDSDrr, d, r);
		}
	} else if (stamp == Type::ptr) {
		AsmReg l = lhs.load_to_reg();
		AsmReg r = rhs.load_to_reg();
		AsmReg d = res.alloc_try_reuse(lhs);
		ASM(LEA64rm, d, FE_MEM(l, 1, r, 0));
	} else {
		uint32_t sz = gp_size(stamp);
		AsmReg l = lhs.load_to_reg();
		AsmReg r = rhs.load_to_reg();
		AsmReg d = res.alloc_try_reuse(lhs);
		if (sz <= 4) {
			if (d == l) {
				ASM(ADD32rr, d, r);
			} else {
				ASM(LEA64rm, d, FE_MEM(l, 1, r, 0));
			}
		} else {
			if (d == l) {
				ASM(ADD64rr, d, r);
			} else {
				ASM(LEA64rm, d, FE_MEM(l, 1, r, 0));
			}
		}
	}
	res.set_modified();
	return true;
}

bool NautilusCompiler::compile_sub(IRInstRef inst) {
	const auto* op = this->adaptor->get_op(inst)->dynCast<ir::SubOperation>();
	Type stamp = op->getStamp();

	IRValueRef lhs_ref{op->getLeftInput()->getIdentifier().getId()};
	IRValueRef rhs_ref{op->getRightInput()->getIdentifier().getId()};

	auto [lhs_vr, lhs] = this->val_ref_single(lhs_ref);
	auto [rhs_vr, rhs] = this->val_ref_single(rhs_ref);
	auto [res_vr, res] = this->result_ref_single(IRValueRef{inst.id});

	if (is_float(stamp)) {
		AsmReg l = lhs.load_to_reg();
		AsmReg r = rhs.load_to_reg();
		AsmReg d = res.alloc_try_reuse(lhs);
		if (stamp == Type::f32) {
			if (d != l)
				ASM(SSE_MOVSSrr, d, l);
			ASM(SSE_SUBSSrr, d, r);
		} else {
			if (d != l)
				ASM(SSE_MOVSDrr, d, l);
			ASM(SSE_SUBSDrr, d, r);
		}
		res.set_modified();
	} else {
		AsmReg l = lhs.load_to_reg();
		AsmReg r = rhs.load_to_reg();
		AsmReg d = res.alloc_try_reuse(lhs);
		if (gp_size(stamp) <= 4) {
			if (d != l)
				ASM(MOV32rr, d, l);
			ASM(SUB32rr, d, r);
		} else {
			if (d != l)
				ASM(MOV64rr, d, l);
			ASM(SUB64rr, d, r);
		}
		res.set_modified();
	}
	return true;
}

bool NautilusCompiler::compile_mul(IRInstRef inst) {
	const auto* op = this->adaptor->get_op(inst)->dynCast<ir::MulOperation>();
	Type stamp = op->getStamp();

	IRValueRef lhs_ref{op->getLeftInput()->getIdentifier().getId()};
	IRValueRef rhs_ref{op->getRightInput()->getIdentifier().getId()};

	auto [lhs_vr, lhs] = this->val_ref_single(lhs_ref);
	auto [rhs_vr, rhs] = this->val_ref_single(rhs_ref);
	auto [res_vr, res] = this->result_ref_single(IRValueRef{inst.id});

	if (is_float(stamp)) {
		AsmReg l = lhs.load_to_reg();
		AsmReg r = rhs.load_to_reg();
		AsmReg d = res.alloc_try_reuse(lhs);
		if (stamp == Type::f32) {
			if (d != l)
				ASM(SSE_MOVSSrr, d, l);
			ASM(SSE_MULSSrr, d, r);
		} else {
			if (d != l)
				ASM(SSE_MOVSDrr, d, l);
			ASM(SSE_MULSDrr, d, r);
		}
	} else {
		AsmReg l = lhs.load_to_reg();
		AsmReg r = rhs.load_to_reg();
		AsmReg d = res.alloc_try_reuse(lhs);
		if (gp_size(stamp) <= 4) {
			if (d != l)
				ASM(MOV32rr, d, l);
			ASM(IMUL32rr, d, r);
		} else {
			if (d != l)
				ASM(MOV64rr, d, l);
			ASM(IMUL64rr, d, r);
		}
	}
	res.set_modified();
	return true;
}

bool NautilusCompiler::compile_div(IRInstRef inst) {
	const auto* op = this->adaptor->get_op(inst)->dynCast<ir::DivOperation>();
	Type stamp = op->getStamp();

	IRValueRef lhs_ref{op->getLeftInput()->getIdentifier().getId()};
	IRValueRef rhs_ref{op->getRightInput()->getIdentifier().getId()};

	auto [lhs_vr, lhs] = this->val_ref_single(lhs_ref);
	auto [rhs_vr, rhs] = this->val_ref_single(rhs_ref);
	auto [res_vr, res] = this->result_ref_single(IRValueRef{inst.id});

	if (is_float(stamp)) {
		AsmReg l = lhs.load_to_reg();
		AsmReg r = rhs.load_to_reg();
		AsmReg d = res.alloc_try_reuse(lhs);
		if (stamp == Type::f32) {
			if (d != l)
				ASM(SSE_MOVSSrr, d, l);
			ASM(SSE_DIVSSrr, d, r);
		} else {
			if (d != l)
				ASM(SSE_MOVSDrr, d, l);
			ASM(SSE_DIVSDrr, d, r);
		}
		res.set_modified();
	} else {
		// Integer division: quotient in rax, remainder in rdx
		// Claim rax and rdx BEFORE loading operands so they won't be used by lhs/rhs
		Base::ScratchReg rax_scratch{this};
		Base::ScratchReg rdx_scratch{this};
		rax_scratch.alloc_specific(AsmReg{AsmReg::AX});
		rdx_scratch.alloc_specific(AsmReg{AsmReg::DX});

		AsmReg l = lhs.load_to_reg(); // won't use rax/rdx (they're fixed)
		AsmReg r = rhs.load_to_reg(); // won't use rax/rdx

		// Move dividend to rax
		if (gp_size(stamp) <= 4) {
			ASM(MOV32rr, AsmReg{AsmReg::AX}, l);
		} else {
			ASM(MOV64rr, AsmReg{AsmReg::AX}, l);
		}

		if (gp_size(stamp) <= 4) {
			if (is_signed(stamp)) {
				ASM(CDQ);
				ASM(IDIV32r, r);
			} else {
				ASM(XOR32rr, AsmReg{AsmReg::DX}, AsmReg{AsmReg::DX});
				ASM(DIV32r, r);
			}
		} else {
			if (is_signed(stamp)) {
				ASM(CQO);
				ASM(IDIV64r, r);
			} else {
				ASM(XOR64rr, AsmReg{AsmReg::DX}, AsmReg{AsmReg::DX});
				ASM(DIV64r, r);
			}
		}

		// Quotient is in rax; discard rdx (remainder)
		rdx_scratch.reset();
		res.set_value(std::move(rax_scratch));
		// set_value(ScratchReg&&) marks modified internally, no set_modified() needed
	}
	return true;
}

bool NautilusCompiler::compile_mod(IRInstRef inst) {
	const auto* op = this->adaptor->get_op(inst)->dynCast<ir::ModOperation>();
	Type stamp = op->getStamp();

	IRValueRef lhs_ref{op->getLeftInput()->getIdentifier().getId()};
	IRValueRef rhs_ref{op->getRightInput()->getIdentifier().getId()};

	auto [lhs_vr, lhs] = this->val_ref_single(lhs_ref);
	auto [rhs_vr, rhs] = this->val_ref_single(rhs_ref);
	auto [res_vr, res] = this->result_ref_single(IRValueRef{inst.id});

	// Claim rax and rdx BEFORE loading operands
	Base::ScratchReg rax_scratch{this};
	Base::ScratchReg rdx_scratch{this};
	rax_scratch.alloc_specific(AsmReg{AsmReg::AX});
	rdx_scratch.alloc_specific(AsmReg{AsmReg::DX});

	AsmReg l = lhs.load_to_reg();
	AsmReg r = rhs.load_to_reg();

	// Move dividend to rax
	if (gp_size(stamp) <= 4) {
		ASM(MOV32rr, AsmReg{AsmReg::AX}, l);
	} else {
		ASM(MOV64rr, AsmReg{AsmReg::AX}, l);
	}

	if (gp_size(stamp) <= 4) {
		if (is_signed(stamp)) {
			ASM(CDQ);
			ASM(IDIV32r, r);
		} else {
			ASM(XOR32rr, AsmReg{AsmReg::DX}, AsmReg{AsmReg::DX});
			ASM(DIV32r, r);
		}
	} else {
		if (is_signed(stamp)) {
			ASM(CQO);
			ASM(IDIV64r, r);
		} else {
			ASM(XOR64rr, AsmReg{AsmReg::DX}, AsmReg{AsmReg::DX});
			ASM(DIV64r, r);
		}
	}

	// Remainder is in rdx; discard rax (quotient)
	rax_scratch.reset();
	res.set_value(std::move(rdx_scratch));
	return true;
}

// ─── Logical / bitwise ────────────────────────────────────────────────────────

bool NautilusCompiler::compile_and(IRInstRef inst) {
	const auto* op = this->adaptor->get_op(inst)->dynCast<ir::AndOperation>();
	IRValueRef lhs_ref{op->getLeftInput()->getIdentifier().getId()};
	IRValueRef rhs_ref{op->getRightInput()->getIdentifier().getId()};

	auto [lhs_vr, lhs] = this->val_ref_single(lhs_ref);
	auto [rhs_vr, rhs] = this->val_ref_single(rhs_ref);
	auto [res_vr, res] = this->result_ref_single(IRValueRef{inst.id});

	AsmReg l = lhs.load_to_reg();
	AsmReg r = rhs.load_to_reg();
	AsmReg d = res.alloc_try_reuse(lhs);
	if (d != l)
		ASM(MOV32rr, d, l);
	ASM(AND32rr, d, r);
	res.set_modified();
	return true;
}

bool NautilusCompiler::compile_or(IRInstRef inst) {
	const auto* op = this->adaptor->get_op(inst)->dynCast<ir::OrOperation>();
	IRValueRef lhs_ref{op->getLeftInput()->getIdentifier().getId()};
	IRValueRef rhs_ref{op->getRightInput()->getIdentifier().getId()};

	auto [lhs_vr, lhs] = this->val_ref_single(lhs_ref);
	auto [rhs_vr, rhs] = this->val_ref_single(rhs_ref);
	auto [res_vr, res] = this->result_ref_single(IRValueRef{inst.id});

	AsmReg l = lhs.load_to_reg();
	AsmReg r = rhs.load_to_reg();
	AsmReg d = res.alloc_try_reuse(lhs);
	if (d != l)
		ASM(MOV32rr, d, l);
	ASM(OR32rr, d, r);
	res.set_modified();
	return true;
}

bool NautilusCompiler::compile_not(IRInstRef inst) {
	const auto* op = this->adaptor->get_op(inst)->dynCast<ir::NotOperation>();
	IRValueRef input_ref{op->getInputs()[0]->getIdentifier().getId()};

	auto [in_vr, in_part] = this->val_ref_single(input_ref);
	auto [res_vr, res] = this->result_ref_single(IRValueRef{inst.id});

	AsmReg src = in_part.load_to_reg();
	AsmReg d = res.alloc_try_reuse(in_part);
	if (d != src)
		ASM(MOV32rr, d, src);
	ASM(XOR32ri, d, 1u);
	res.set_modified();
	return true;
}

bool NautilusCompiler::compile_negate(IRInstRef inst) {
	const auto* op = this->adaptor->get_op(inst);
	IRValueRef input_ref{op->getInputs()[0]->getIdentifier().getId()};
	Type stamp = op->getStamp();

	auto [in_vr, in_part] = this->val_ref_single(input_ref);
	auto [res_vr, res] = this->result_ref_single(IRValueRef{inst.id});

	if (is_float(stamp)) {
		AsmReg src = in_part.load_to_reg();
		AsmReg d = res.alloc_try_reuse(in_part);
		if (stamp == Type::f32) {
			uint64_t sign_mask = 0x80000000ULL;
			if (d != src)
				ASM(SSE_MOVSSrr, d, src);
			Base::ScratchReg tmp{this};
			AsmReg tmp_reg = tmp.alloc_gp();
			ASM(MOV32ri, tmp_reg, static_cast<uint32_t>(sign_mask));
			// Use XORPS via materialize for sign flip - simpler to inline a const
			// Actually: negate via XOR with a materialized sign mask in another XMM
			// For simplicity, use XOR directly with a constant materialized in XMM
			Base::ScratchReg xmm_tmp{this};
			AsmReg xmm_tmp_reg = xmm_tmp.alloc(PlatformConfig::FP_BANK);
			this->materialize_constant(&sign_mask, PlatformConfig::FP_BANK, 4, xmm_tmp_reg);
			ASM(SSE_XORPSrr, d, xmm_tmp_reg);
		} else {
			uint64_t sign_mask = 0x8000000000000000ULL;
			if (d != src)
				ASM(SSE_MOVSDrr, d, src);
			Base::ScratchReg xmm_tmp{this};
			AsmReg xmm_tmp_reg = xmm_tmp.alloc(PlatformConfig::FP_BANK);
			this->materialize_constant(&sign_mask, PlatformConfig::FP_BANK, 8, xmm_tmp_reg);
			ASM(SSE_XORPDrr, d, xmm_tmp_reg);
		}
	} else {
		AsmReg src = in_part.load_to_reg();
		AsmReg d = res.alloc_try_reuse(in_part);
		if (gp_size(stamp) <= 4) {
			if (d != src)
				ASM(MOV32rr, d, src);
			ASM(NEG32r, d);
		} else {
			if (d != src)
				ASM(MOV64rr, d, src);
			ASM(NEG64r, d);
		}
	}
	res.set_modified();
	return true;
}

bool NautilusCompiler::compile_binary_comp(IRInstRef inst) {
	const auto* op = this->adaptor->get_op(inst)->dynCast<ir::BinaryCompOperation>();
	IRValueRef lhs_ref{op->getLeftInput()->getIdentifier().getId()};
	IRValueRef rhs_ref{op->getRightInput()->getIdentifier().getId()};

	auto [lhs_vr, lhs] = this->val_ref_single(lhs_ref);
	auto [rhs_vr, rhs] = this->val_ref_single(rhs_ref);
	auto [res_vr, res] = this->result_ref_single(IRValueRef{inst.id});

	uint32_t sz = gp_size(op->getStamp());
	AsmReg l = lhs.load_to_reg();
	AsmReg r = rhs.load_to_reg();
	AsmReg d = res.alloc_try_reuse(lhs);
	if (d != l) {
		sz <= 4 ? ASM(MOV32rr, d, l) : ASM(MOV64rr, d, l);
	}

	switch (op->getType()) {
		case ir::BinaryCompOperation::BAND:
			sz <= 4 ? ASM(AND32rr, d, r) : ASM(AND64rr, d, r);
			break;
		case ir::BinaryCompOperation::BOR:
			sz <= 4 ? ASM(OR32rr, d, r) : ASM(OR64rr, d, r);
			break;
		case ir::BinaryCompOperation::XOR:
			sz <= 4 ? ASM(XOR32rr, d, r) : ASM(XOR64rr, d, r);
			break;
	}
	res.set_modified();
	return true;
}

bool NautilusCompiler::compile_shift(IRInstRef inst) {
	const auto* op = this->adaptor->get_op(inst)->dynCast<ir::ShiftOperation>();
	IRValueRef lhs_ref{op->getLeftInput()->getIdentifier().getId()};
	IRValueRef rhs_ref{op->getRightInput()->getIdentifier().getId()};

	auto [lhs_vr, lhs] = this->val_ref_single(lhs_ref);
	auto [rhs_vr, rhs] = this->val_ref_single(rhs_ref);
	auto [res_vr, res] = this->result_ref_single(IRValueRef{inst.id});

	// Claim CX for shift count before loading operands so they avoid CX
	Base::ScratchReg cx_scratch{this};
	cx_scratch.alloc_specific(AsmReg{AsmReg::CX});

	AsmReg l = lhs.load_to_reg();
	AsmReg r = rhs.load_to_reg();

	// Move shift count to CX (raw ASM on our scratch register)
	ASM(MOV64rr, AsmReg{AsmReg::CX}, r);

	AsmReg d = res.alloc_try_reuse(lhs);
	uint32_t sz = gp_size(op->getStamp());
	if (d != l) {
		sz <= 4 ? ASM(MOV32rr, d, l) : ASM(MOV64rr, d, l);
	}

	AsmReg cx = AsmReg{AsmReg::CX};
	if (op->getType() == ir::ShiftOperation::LS) {
		sz <= 4 ? ASM(SHL32rr, d, cx) : ASM(SHL64rr, d, cx);
	} else if (is_signed(op->getStamp())) {
		sz <= 4 ? ASM(SAR32rr, d, cx) : ASM(SAR64rr, d, cx);
	} else {
		sz <= 4 ? ASM(SHR32rr, d, cx) : ASM(SHR64rr, d, cx);
	}
	res.set_modified();
	return true;
}

bool NautilusCompiler::compile_compare(IRInstRef inst) {
	const auto* op = this->adaptor->get_op(inst)->dynCast<ir::CompareOperation>();
	IRValueRef lhs_ref{op->getLeftInput()->getIdentifier().getId()};
	IRValueRef rhs_ref{op->getRightInput()->getIdentifier().getId()};

	Type input_type = op->getLeftInput()->getStamp();

	auto [lhs_vr, lhs] = this->val_ref_single(lhs_ref);
	auto [rhs_vr, rhs] = this->val_ref_single(rhs_ref);
	auto [res_vr, res] = this->result_ref_single(IRValueRef{inst.id});

	AsmReg l = lhs.load_to_reg();
	AsmReg r = rhs.load_to_reg();
	AsmReg d = res.alloc_reg();

	// Zero out result first (SETCC only sets low byte)
	ASM(XOR32rr, d, d);

	if (is_float(input_type)) {
		if (input_type == Type::f32) {
			ASM(SSE_UCOMISSrr, l, r);
		} else {
			ASM(SSE_UCOMISDrr, l, r);
		}
	} else {
		uint32_t sz = gp_size(input_type);
		sz <= 4 ? ASM(CMP32rr, l, r) : ASM(CMP64rr, l, r);
	}

	using Cmp = ir::CompareOperation::Comparator;
	bool use_signed = is_signed(input_type) || is_float(input_type);

	switch (op->getComparator()) {
		case Cmp::EQ:
			ASM(SETZ8r, d);
			break;
		case Cmp::NE:
			ASM(SETNZ8r, d);
			break;
		case Cmp::LT:
			use_signed ? ASM(SETL8r, d) : ASM(SETC8r, d);
			break;
		case Cmp::LE:
			use_signed ? ASM(SETLE8r, d) : ASM(SETBE8r, d);
			break;
		case Cmp::GT:
			use_signed ? ASM(SETG8r, d) : ASM(SETA8r, d);
			break;
		case Cmp::GE:
			use_signed ? ASM(SETGE8r, d) : ASM(SETNC8r, d);
			break;
	}
	res.set_modified();
	return true;
}

// ─── Memory ───────────────────────────────────────────────────────────────────

bool NautilusCompiler::compile_load(IRInstRef inst) {
	const auto* op = this->adaptor->get_op(inst)->dynCast<ir::LoadOperation>();
	IRValueRef addr_ref{op->getAddress()->getIdentifier().getId()};
	Type stamp = op->getStamp();

	auto [addr_vr, addr] = this->val_ref_single(addr_ref);
	auto [res_vr, res] = this->result_ref_single(IRValueRef{inst.id});

	AsmReg a = addr.load_to_reg();
	AsmReg d = res.alloc_reg();

	if (is_float(stamp)) {
		if (stamp == Type::f32) {
			ASM(SSE_MOVSSrm, d, FE_MEM(a, 0, 0, 0));
		} else {
			ASM(SSE_MOVSDrm, d, FE_MEM(a, 0, 0, 0));
		}
	} else {
		uint32_t sz = gp_size(stamp);
		switch (sz) {
			case 1:
				is_signed(stamp) ? ASM(MOVSXr32m8, d, FE_MEM(a, 0, 0, 0)) : ASM(MOVZXr32m8, d, FE_MEM(a, 0, 0, 0));
				break;
			case 2:
				is_signed(stamp) ? ASM(MOVSXr32m16, d, FE_MEM(a, 0, 0, 0)) : ASM(MOVZXr32m16, d, FE_MEM(a, 0, 0, 0));
				break;
			case 4:
				ASM(MOV32rm, d, FE_MEM(a, 0, 0, 0));
				break;
			default:
				ASM(MOV64rm, d, FE_MEM(a, 0, 0, 0));
				break;
		}
	}
	res.set_modified();
	return true;
}

bool NautilusCompiler::compile_store(IRInstRef inst) {
	const auto* op = this->adaptor->get_op(inst)->dynCast<ir::StoreOperation>();
	IRValueRef val_ref_id{op->getValue()->getIdentifier().getId()};
	IRValueRef addr_ref{op->getAddress()->getIdentifier().getId()};
	Type stamp = op->getValue()->getStamp();

	auto [val_vr, val_part] = this->val_ref_single(val_ref_id);
	auto [addr_vr, addr_part] = this->val_ref_single(addr_ref);

	AsmReg v = val_part.load_to_reg();
	AsmReg a = addr_part.load_to_reg();

	if (is_float(stamp)) {
		if (stamp == Type::f32) {
			ASM(SSE_MOVSSmr, FE_MEM(a, 0, 0, 0), v);
		} else {
			ASM(SSE_MOVSDmr, FE_MEM(a, 0, 0, 0), v);
		}
	} else {
		uint32_t sz = gp_size(stamp);
		switch (sz) {
			case 1:
				ASM(MOV8mr, FE_MEM(a, 0, 0, 0), v);
				break;
			case 2:
				ASM(MOV16mr, FE_MEM(a, 0, 0, 0), v);
				break;
			case 4:
				ASM(MOV32mr, FE_MEM(a, 0, 0, 0), v);
				break;
			default:
				ASM(MOV64mr, FE_MEM(a, 0, 0, 0), v);
				break;
		}
	}
	return true;
}

bool NautilusCompiler::compile_alloca(IRInstRef inst) {
	const auto* op = this->adaptor->get_op(inst)->dynCast<ir::AllocaOperation>();
	auto [res_vr, res] = this->result_ref_single(IRValueRef{inst.id});
	uint32_t sz = static_cast<uint32_t>(op->getSize());
	// alloca_fixed takes ValuePart& - ValuePartRef inherits ValuePart
	this->alloca_fixed(sz, 8, static_cast<Base::ValuePart&>(res));
	return true;
}

// ─── Type conversion ──────────────────────────────────────────────────────────

bool NautilusCompiler::compile_cast(IRInstRef inst) {
	const auto* op = this->adaptor->get_op(inst)->dynCast<ir::CastOperation>();
	IRValueRef input_ref{op->getInput()->getIdentifier().getId()};
	Type from_type = op->getInput()->getStamp();
	Type to_type = op->getStamp();

	auto [in_vr, in_part] = this->val_ref_single(input_ref);
	auto [res_vr, res] = this->result_ref_single(IRValueRef{inst.id});

	AsmReg src = in_part.load_to_reg();
	AsmReg dst = res.alloc_reg();

	if (from_type == to_type) {
		// No-op cast
		if (gp_size(from_type) <= 4) {
			ASM(MOV32rr, dst, src);
		} else {
			ASM(MOV64rr, dst, src);
		}
	} else if (!is_float(from_type) && !is_float(to_type)) {
		// Int → Int
		uint32_t from_sz = gp_size(from_type);
		uint32_t to_sz = gp_size(to_type);
		if (to_sz >= from_sz) {
			// Widening
			if (is_signed(from_type)) {
				if (from_sz == 1)
					ASM(MOVSXr64r8, dst, src);
				else if (from_sz == 2)
					ASM(MOVSXr64r16, dst, src);
				else
					ASM(MOVSXr64r32, dst, src);
			} else {
				if (from_sz <= 4) {
					ASM(MOV32rr, dst, src);
				} else {
					ASM(MOV64rr, dst, src);
				}
			}
		} else {
			// Narrowing: just move (upper bits are irrelevant)
			ASM(MOV32rr, dst, src);
		}
	} else if (!is_float(from_type) && is_float(to_type)) {
		// Int → Float
		if (to_type == Type::f32) {
			gp_size(from_type) <= 4 ? ASM(SSE_CVTSI2SS32rr, dst, src) : ASM(SSE_CVTSI2SS64rr, dst, src);
		} else {
			gp_size(from_type) <= 4 ? ASM(SSE_CVTSI2SD32rr, dst, src) : ASM(SSE_CVTSI2SD64rr, dst, src);
		}
	} else if (is_float(from_type) && !is_float(to_type)) {
		// Float → Int (truncate toward zero)
		if (from_type == Type::f32) {
			gp_size(to_type) <= 4 ? ASM(SSE_CVTTSS2SI32rr, dst, src) : ASM(SSE_CVTTSS2SI64rr, dst, src);
		} else {
			gp_size(to_type) <= 4 ? ASM(SSE_CVTTSD2SI32rr, dst, src) : ASM(SSE_CVTTSD2SI64rr, dst, src);
		}
	} else {
		// Float → Float
		if (from_type == Type::f32 && to_type == Type::f64) {
			ASM(SSE_CVTSS2SDrr, dst, src);
		} else {
			ASM(SSE_CVTSD2SSrr, dst, src);
		}
	}
	res.set_modified();
	return true;
}

// ─── Select ───────────────────────────────────────────────────────────────────

bool NautilusCompiler::compile_select(IRInstRef inst) {
	const auto* op = this->adaptor->get_op(inst)->dynCast<ir::SelectOperation>();
	IRValueRef cond_ref{op->getCondition()->getIdentifier().getId()};
	IRValueRef true_ref{op->getTrueValue()->getIdentifier().getId()};
	IRValueRef false_ref{op->getFalseValue()->getIdentifier().getId()};
	Type stamp = op->getStamp();

	auto [cond_vr, cond] = this->val_ref_single(cond_ref);
	auto [true_vr, true_part] = this->val_ref_single(true_ref);
	auto [false_vr, false_part] = this->val_ref_single(false_ref);
	auto [res_vr, res] = this->result_ref_single(IRValueRef{inst.id});

	AsmReg cond_reg = cond.load_to_reg();
	ASM(TEST32rr, cond_reg, cond_reg);

	AsmReg t = true_part.load_to_reg();
	AsmReg f = false_part.load_to_reg();
	AsmReg d = res.alloc_try_reuse(true_part);

	// Move false into dst, then CMOVNZ to true if condition is non-zero
	if (d != f) {
		if (gp_size(stamp) <= 4) {
			ASM(MOV32rr, d, f);
		} else {
			ASM(MOV64rr, d, f);
		}
	}
	if (gp_size(stamp) <= 4) {
		ASM(CMOVNZ32rr, d, t);
	} else {
		ASM(CMOVNZ64rr, d, t);
	}
	res.set_modified();
	return true;
}

// ─── Control flow ─────────────────────────────────────────────────────────────

bool NautilusCompiler::compile_if(IRInstRef inst) {
	const auto* op = this->adaptor->get_op(inst)->dynCast<ir::IfOperation>();

	const ir::BasicBlock* true_blk = op->getTrueBlockInvocation().getBlock();
	const ir::BasicBlock* false_blk = op->getFalseBlockInvocation().getBlock();
	const auto& bb_list = this->adaptor->blocks_vec();

	uint32_t true_idx = ~0u;
	uint32_t false_idx = ~0u;
	for (uint32_t i = 0; i < static_cast<uint32_t>(bb_list.size()); ++i) {
		if (bb_list[i].get() == true_blk)
			true_idx = i;
		if (bb_list[i].get() == false_blk)
			false_idx = i;
	}

	IRValueRef cond_ref{const_cast<ir::IfOperation*>(op)->getBooleanValue()->getIdentifier().getId()};
	auto [cond_vr, cond] = this->val_ref_single(cond_ref);
	AsmReg cond_reg = cond.load_to_reg();

	ASM(TEST32rr, cond_reg, cond_reg);
	this->generate_cond_branch(Jump::jne, static_cast<IRBlockRef>(true_idx), static_cast<IRBlockRef>(false_idx));
	return true;
}

bool NautilusCompiler::compile_branch(IRInstRef inst) {
	const auto* op = this->adaptor->get_op(inst)->dynCast<ir::BranchOperation>();
	const ir::BasicBlock* tgt_blk = op->getNextBlockInvocation().getBlock();
	const auto& bb_list = this->adaptor->blocks_vec();

	uint32_t tgt_idx = ~0u;
	for (uint32_t i = 0; i < static_cast<uint32_t>(bb_list.size()); ++i) {
		if (bb_list[i].get() == tgt_blk) {
			tgt_idx = i;
			break;
		}
	}
	this->generate_uncond_branch(static_cast<IRBlockRef>(tgt_idx));
	return true;
}

bool NautilusCompiler::compile_return(IRInstRef inst) {
	const auto* op = this->adaptor->get_op(inst)->dynCast<ir::ReturnOperation>();

	Base::RetBuilder rb{*this->derived(), *this->cur_cc_assigner()};
	if (op->hasReturnValue()) {
		rb.add(IRValueRef{op->getReturnValue()->getIdentifier().getId()});
	}
	rb.ret();
	return true;
}

// ─── External function call ───────────────────────────────────────────────────

bool NautilusCompiler::compile_proxy_call(IRInstRef inst) {
	const auto* op_base = this->adaptor->get_op(inst);
	const auto* op = op_base->dynCast<ir::ProxyCallOperation>();
	has_calls_ = true;

	const std::string& sym_name = op->getFunctionSymbol();
	// getFunctionPtr() is non-const, use const_cast
	void* fn_ptr = const_cast<ir::ProxyCallOperation*>(op)->getFunctionPtr();

	// Register external symbol address
	external_symbol_addresses[sym_name] = fn_ptr;

	// Create or reuse symbol reference for this external function
	::tpde::SymRef sym =
	    this->assembler.sym_add_undef(sym_name, ::tpde::Assembler::SymBinding::GLOBAL);

	// Build arguments list
	const auto& input_args = op->getInputArguments();
	::tpde::util::SmallVector<Base::CallArg, 8> arguments;
	arguments.reserve(static_cast<uint32_t>(input_args.size()));
	for (const ir::Operation* arg : input_args) {
		arguments.push_back(Base::CallArg{IRValueRef{arg->getIdentifier().getId()}});
	}

	// Emit call
	if (op->getStamp() != nautilus::Type::v) {
		auto res_vr = this->result_ref(IRValueRef{inst.id});
		this->generate_call(sym, std::span<Base::CallArg>{arguments.data(), arguments.size()}, &res_vr);
	} else {
		this->generate_call(sym, std::span<Base::CallArg>{arguments.data(), arguments.size()}, nullptr);
	}
	return true;
}

} // namespace nautilus::compiler::tpde

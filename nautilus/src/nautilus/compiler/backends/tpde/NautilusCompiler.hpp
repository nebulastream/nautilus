#pragma once

#include "NautilusIRAdaptor.hpp"
#include "nautilus/tracing/Types.hpp"
#include "tpde/Assembler.hpp"
#include "tpde/base.hpp"
#include "tpde/x64/CompilerX64.hpp"
#include <optional>
#include <unordered_map>

namespace nautilus::compiler::tpde {

/// CRTP compiler class that extends CompilerX64 with Nautilus-specific
/// instruction selection. Each Nautilus IR OperationType maps to one or more
/// x64 instructions via a per-type compile_* method.
struct NautilusCompiler : ::tpde::x64::CompilerX64<NautilusIRAdaptor, NautilusCompiler> {
	using Base = ::tpde::x64::CompilerX64<NautilusIRAdaptor, NautilusCompiler>;
	using AsmReg = Base::AsmReg;

	/// Tracks whether the current function contains any call instructions.
	bool has_calls_{false};

	/// External symbol → address map built during compile_inst for ProxyCallOp.
	std::unordered_map<std::string, void*> external_symbol_addresses;

	explicit NautilusCompiler(NautilusIRAdaptor* adaptor) : Base(adaptor) {}

	// ── Compiler concept requirements ─────────────────────────────────────────

	bool cur_func_may_emit_calls() const { return has_calls_; }

	::tpde::SymRef cur_personality_func() const { return {}; }

	struct NautilusValueParts {
		uint32_t size;
		::tpde::RegBank bank;

		[[nodiscard]] uint32_t count() const { return 1; }

		[[nodiscard]] uint32_t size_bytes(uint32_t /*part*/) const { return size; }

		[[nodiscard]] ::tpde::RegBank reg_bank(uint32_t /*part*/) const { return bank; }
	};

	NautilusValueParts val_parts(IRValueRef val) {
		const ir::Operation* op = this->adaptor->get_op(val);
		return type_to_value_parts(op->getStamp());
	}

	struct ValRefSpecial {
		uint8_t mode; // must be first member, >= 4
	};
	static_assert(offsetof(ValRefSpecial, mode) == 0);
	static_assert(std::is_standard_layout_v<ValRefSpecial>);

	std::optional<ValRefSpecial> val_ref_special(IRValueRef /*val*/) {
		return std::nullopt;
	}

	ValuePart val_part_ref_special(ValRefSpecial& /*vrs*/, uint32_t /*part*/) {
		TPDE_UNREACHABLE("val_part_ref_special called on Nautilus compiler");
	}

	bool try_force_fixed_assignment(IRValueRef /*val*/) const { return false; }

	void define_func_idx(IRFuncRef /*func*/, uint32_t /*idx*/) {}

	// ── Core instruction compilation ──────────────────────────────────────────

	[[nodiscard]] bool compile_inst(IRInstRef inst, InstRange remaining);

private:
	// ── Per-operation compilers ───────────────────────────────────────────────
	bool compile_const_int(IRInstRef inst);
	bool compile_const_bool(IRInstRef inst);
	bool compile_const_float(IRInstRef inst);
	bool compile_const_ptr(IRInstRef inst);

	bool compile_add(IRInstRef inst);
	bool compile_sub(IRInstRef inst);
	bool compile_mul(IRInstRef inst);
	bool compile_div(IRInstRef inst);
	bool compile_mod(IRInstRef inst);

	bool compile_and(IRInstRef inst);
	bool compile_or(IRInstRef inst);
	bool compile_not(IRInstRef inst);
	bool compile_negate(IRInstRef inst);
	bool compile_binary_comp(IRInstRef inst);
	bool compile_shift(IRInstRef inst);
	bool compile_compare(IRInstRef inst);

	bool compile_load(IRInstRef inst);
	bool compile_store(IRInstRef inst);
	bool compile_alloca(IRInstRef inst);

	bool compile_cast(IRInstRef inst);
	bool compile_select(IRInstRef inst);

	bool compile_if(IRInstRef inst);
	bool compile_branch(IRInstRef inst);
	bool compile_return(IRInstRef inst);

	bool compile_proxy_call(IRInstRef inst);

	// ── Helpers ───────────────────────────────────────────────────────────────

	static NautilusValueParts type_to_value_parts(nautilus::Type t);

	/// Return the byte-width of a Nautilus type.
	static uint32_t type_bytes(nautilus::Type t);

	/// Return the x64 size prefix (1/2/4/8) for a type.
	static uint32_t gp_size(nautilus::Type t);

	/// True if the type is a signed integer.
	static bool is_signed(nautilus::Type t);

	/// True if the type is a floating-point type.
	static bool is_float(nautilus::Type t);
};

} // namespace nautilus::compiler::tpde

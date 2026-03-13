#pragma once

#include "nautilus/compiler/ir/IRGraph.hpp"
#include "nautilus/compiler/ir/blocks/BasicBlock.hpp"
#include "nautilus/compiler/ir/blocks/BasicBlockArgument.hpp"
#include "nautilus/compiler/ir/blocks/BasicBlockInvocation.hpp"
#include "nautilus/compiler/ir/operations/BranchOperation.hpp"
#include "nautilus/compiler/ir/operations/FunctionOperation.hpp"
#include "nautilus/compiler/ir/operations/IfOperation.hpp"
#include "nautilus/compiler/ir/operations/Operation.hpp"
#include "nautilus/compiler/ir/operations/ReturnOperation.hpp"
#include "tpde/IRAdaptor.hpp"
#include "tpde/ValLocalIdx.hpp"
#include "tpde/base.hpp"
#include <cassert>
#include <memory>
#include <ranges>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace nautilus::compiler::tpde {

// ─── Reference types ──────────────────────────────────────────────────────────
// Each fits in ≤ 8 bytes as required by the IRAdaptor concept.

/// References a single IR operation (value or instruction) by its dense index.
struct IRValueRef {
	uint32_t id{~0u};
	bool operator==(const IRValueRef& o) const { return id == o.id; }
	bool operator!=(const IRValueRef& o) const { return id != o.id; }
};

/// References an instruction (same index space as values).
struct IRInstRef {
	uint32_t id{~0u};
	bool operator==(const IRInstRef& o) const { return id == o.id; }
	bool operator!=(const IRInstRef& o) const { return id != o.id; }
	explicit operator IRValueRef() const { return {id}; }
};

/// References a basic block by its index in the function's block list.
enum class IRBlockRef : uint32_t {
};

/// Single function per IRGraph — always 0.
enum class IRFuncRef : uint32_t {
};

// ─── PHIRef helper ────────────────────────────────────────────────────────────

struct NautilusPHIRef {
	const ir::BasicBlockArgument* arg;
	/// parallel arrays: incoming_vals[i] is the value from incoming_blocks[i]
	std::vector<IRValueRef> incoming_vals;
	std::vector<IRBlockRef> incoming_blocks;

	[[nodiscard]] uint32_t incoming_count() const {
		return static_cast<uint32_t>(incoming_vals.size());
	}

	[[nodiscard]] IRValueRef incoming_val_for_slot(uint32_t slot) const {
		assert(slot < incoming_count());
		return incoming_vals[slot];
	}

	[[nodiscard]] IRBlockRef incoming_block_for_slot(uint32_t slot) const {
		assert(slot < incoming_count());
		return incoming_blocks[slot];
	}

	[[nodiscard]] IRValueRef incoming_val_for_block(IRBlockRef blk) const {
		for (uint32_t i = 0; i < incoming_count(); ++i) {
			if (incoming_blocks[i] == blk)
				return incoming_vals[i];
		}
		return {~0u};
	}
};

// ─── Main adaptor ─────────────────────────────────────────────────────────────

class NautilusIRAdaptor {
public:
	// ── TPDE concept type aliases ────────────────────────────────────────────
	using IRValueRef = tpde::IRValueRef;
	using IRInstRef = tpde::IRInstRef;
	using IRBlockRef = tpde::IRBlockRef;
	using IRFuncRef = tpde::IRFuncRef;

	static constexpr IRValueRef INVALID_VALUE_REF{~0u};
	static constexpr IRBlockRef INVALID_BLOCK_REF = static_cast<IRBlockRef>(~0u);
	static constexpr IRFuncRef INVALID_FUNC_REF = static_cast<IRFuncRef>(~0u);

	static constexpr bool TPDE_PROVIDES_HIGHEST_VAL_IDX = true;
	static constexpr bool TPDE_LIVENESS_VISIT_ARGS = true;

	explicit NautilusIRAdaptor(std::shared_ptr<ir::IRGraph> ir);

	// ── Module-level ─────────────────────────────────────────────────────────
	[[nodiscard]] uint32_t func_count() const { return 1; }

	[[nodiscard]] auto funcs() const {
		return std::views::single(static_cast<IRFuncRef>(0));
	}

	[[nodiscard]] auto funcs_to_compile() const { return funcs(); }

	[[nodiscard]] std::string_view func_link_name(IRFuncRef) const {
		return func_->getName();
	}

	[[nodiscard]] static bool func_extern(IRFuncRef) { return false; }

	[[nodiscard]] static bool func_only_local(IRFuncRef) { return true; }

	[[nodiscard]] static bool func_has_weak_linkage(IRFuncRef) { return false; }

	// ── Current-function state (set by switch_func) ───────────────────────────
	[[nodiscard]] static bool cur_needs_unwind_info() { return false; }

	[[nodiscard]] static bool cur_is_vararg() { return false; }

	[[nodiscard]] uint32_t cur_highest_val_idx() const {
		return highest_val_idx_;
	}

	[[nodiscard]] auto cur_args() const {
		return std::views::iota(uint32_t{0}, static_cast<uint32_t>(arg_val_ids_.size())) |
		       std::views::transform([this](uint32_t i) { return IRValueRef{arg_val_ids_[i]}; });
	}

	[[nodiscard]] static bool cur_arg_is_byval(uint32_t) { return false; }

	[[nodiscard]] static uint32_t cur_arg_byval_size(uint32_t) { return 0; }

	[[nodiscard]] static uint32_t cur_arg_byval_align(uint32_t) { return 0; }

	[[nodiscard]] static bool cur_arg_is_sret(uint32_t) { return false; }

	[[nodiscard]] auto cur_static_allocas() const {
		return std::views::all(alloca_val_ids_) |
		       std::views::transform([](uint32_t id) { return IRValueRef{id}; });
	}

	[[nodiscard]] static bool cur_has_dynamic_alloca() { return false; }

	[[nodiscard]] IRBlockRef cur_entry_block() const {
		return static_cast<IRBlockRef>(0);
	}

	[[nodiscard]] auto cur_blocks() const {
		return std::views::iota(uint32_t{0}, static_cast<uint32_t>(blocks_.size())) |
		       std::views::transform([](uint32_t i) { return static_cast<IRBlockRef>(i); });
	}

	// ── Block methods ─────────────────────────────────────────────────────────
	[[nodiscard]] auto block_succs(IRBlockRef blk) const {
		return std::views::all(block_succs_[static_cast<uint32_t>(blk)]) |
		       std::views::transform([](uint32_t b) { return static_cast<IRBlockRef>(b); });
	}

	[[nodiscard]] auto block_insts(IRBlockRef blk) const {
		return std::views::all(block_insts_[static_cast<uint32_t>(blk)]) |
		       std::views::transform([](uint32_t id) { return IRInstRef{id}; });
	}

	[[nodiscard]] auto block_phis(IRBlockRef blk) const {
		return std::views::all(block_phis_[static_cast<uint32_t>(blk)]) |
		       std::views::transform([](uint32_t id) { return IRValueRef{id}; });
	}

	[[nodiscard]] uint32_t block_info(IRBlockRef blk) const {
		return block_info_[static_cast<uint32_t>(blk)][0];
	}

	void block_set_info(IRBlockRef blk, uint32_t v) {
		block_info_[static_cast<uint32_t>(blk)][0] = v;
	}

	[[nodiscard]] uint32_t block_info2(IRBlockRef blk) const {
		return block_info_[static_cast<uint32_t>(blk)][1];
	}

	void block_set_info2(IRBlockRef blk, uint32_t v) {
		block_info_[static_cast<uint32_t>(blk)][1] = v;
	}

	[[nodiscard]] std::string_view block_fmt_ref(IRBlockRef blk) const {
		return block_names_[static_cast<uint32_t>(blk)];
	}

	// ── Value methods ─────────────────────────────────────────────────────────
	[[nodiscard]] ::tpde::ValLocalIdx val_local_idx(IRValueRef val) const {
		auto it = val_local_idx_map_.find(val.id);
		assert(it != val_local_idx_map_.end());
		return ::tpde::ValLocalIdx(it->second);
	}

	[[nodiscard]] bool val_ignore_in_liveness_analysis(IRValueRef val) const {
		auto it = op_map_.find(val.id);
		if (it == op_map_.end())
			return false;
		using OT = ir::Operation::OperationType;
		auto t = it->second->getOperationType();
		return t == OT::ConstIntOp || t == OT::ConstBooleanOp || t == OT::ConstFloatOp ||
		       t == OT::ConstPtrOp;
	}

	[[nodiscard]] bool val_is_phi(IRValueRef val) const {
		auto it = op_map_.find(val.id);
		if (it == op_map_.end())
			return false;
		return it->second->getOperationType() == ir::Operation::OperationType::BasicBlockArgument;
	}

	[[nodiscard]] NautilusPHIRef val_as_phi(IRValueRef val) const {
		auto it = phi_map_.find(val.id);
		assert(it != phi_map_.end());
		return it->second;
	}

	[[nodiscard]] uint32_t val_alloca_size(IRValueRef val) const;

	[[nodiscard]] uint32_t val_alloca_align(IRValueRef /*val*/) const {
		return 8; // align allocas to 8 bytes
	}

	[[nodiscard]] std::string_view value_fmt_ref(IRValueRef val) const {
		return val_name(val.id);
	}

	// ── Instruction methods ───────────────────────────────────────────────────
	[[nodiscard]] auto inst_operands(IRInstRef inst) const {
		return std::views::all(inst_operands_[inst.id]) |
		       std::views::transform([](uint32_t id) { return IRValueRef{id}; });
	}

	[[nodiscard]] auto inst_results(IRInstRef inst) const {
		bool has_result = inst_has_result_[inst.id];
		return std::views::single(IRValueRef{inst.id}) | std::views::drop(!has_result ? 1 : 0);
	}

	[[nodiscard]] static bool inst_fused(IRInstRef) { return false; }

	[[nodiscard]] std::string_view inst_fmt_ref(IRInstRef inst) const {
		return val_name(inst.id);
	}

	// ── Lifecycle ─────────────────────────────────────────────────────────────
	void start_compile() {}

	void end_compile() {}

	bool switch_func(IRFuncRef /*func*/) {
		// Already prepared in constructor — nothing to re-do.
		return true;
	}

	void reset() {}

	// ── Non-concept helpers used by NautilusCompiler ──────────────────────────
	[[nodiscard]] const ir::Operation* get_op(IRValueRef val) const {
		auto it = op_map_.find(val.id);
		assert(it != op_map_.end());
		return it->second;
	}

	[[nodiscard]] const ir::Operation* get_op(IRInstRef inst) const {
		return get_op(IRValueRef{inst.id});
	}

	[[nodiscard]] const ir::FunctionOperation* func_op() const { return func_; }

	[[nodiscard]] const std::vector<std::unique_ptr<ir::BasicBlock>>& blocks_vec() const {
		return func_->getBasicBlocks();
	}

private:
	void build(const ir::FunctionOperation& func);
	void compute_phi_incoming(const ir::FunctionOperation& func);
	[[nodiscard]] std::string_view val_name(uint32_t id) const;

	std::shared_ptr<ir::IRGraph> ir_;
	const ir::FunctionOperation* func_{nullptr};

	// Operation pointer lookup by dense id
	std::unordered_map<uint32_t, const ir::Operation*> op_map_;

	// Dense local index for liveness (0-based within function)
	std::unordered_map<uint32_t, uint32_t> val_local_idx_map_;
	uint32_t highest_val_idx_{0};

	// Argument dense ids (in order)
	std::vector<uint32_t> arg_val_ids_;

	// Alloca dense ids
	std::vector<uint32_t> alloca_val_ids_;

	// Per-block data
	std::vector<const ir::BasicBlock*> blocks_;
	std::vector<std::string> block_names_;
	std::vector<std::array<uint32_t, 2>> block_info_;
	std::vector<std::vector<uint32_t>> block_succs_;
	std::vector<std::vector<uint32_t>> block_phis_;
	std::vector<std::vector<uint32_t>> block_insts_;

	// Per-value data (indexed by dense id)
	std::vector<std::vector<uint32_t>> inst_operands_;
	std::vector<bool> inst_has_result_;
	std::vector<std::string> val_names_;

	// PHI incoming map
	std::unordered_map<uint32_t, NautilusPHIRef> phi_map_;
};

} // namespace nautilus::compiler::tpde

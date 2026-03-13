#include "NautilusIRAdaptor.hpp"

#include "nautilus/compiler/ir/IRGraph.hpp"
#include "nautilus/compiler/ir/blocks/BasicBlockArgument.hpp"
#include "nautilus/compiler/ir/blocks/BasicBlockInvocation.hpp"
#include "nautilus/compiler/ir/operations/AllocaOperation.hpp"
#include "nautilus/compiler/ir/operations/BranchOperation.hpp"
#include "nautilus/compiler/ir/operations/FunctionOperation.hpp"
#include "nautilus/compiler/ir/operations/IfOperation.hpp"
#include "nautilus/compiler/ir/operations/Operation.hpp"
#include "nautilus/compiler/ir/operations/ReturnOperation.hpp"
#include <sstream>

namespace nautilus::compiler::tpde {

using OT = ir::Operation::OperationType;

NautilusIRAdaptor::NautilusIRAdaptor(std::shared_ptr<ir::IRGraph> ir) : ir_(std::move(ir)) {
	func_ = &ir_->getRootOperation();
	build(*func_);
	compute_phi_incoming(*func_);
}

void NautilusIRAdaptor::build(const ir::FunctionOperation& func) {
	const auto& bb_list = func.getBasicBlocks();
	const uint32_t num_blocks = static_cast<uint32_t>(bb_list.size());

	blocks_.reserve(num_blocks);
	block_names_.reserve(num_blocks);
	block_info_.assign(num_blocks, {0u, 0u});
	block_succs_.resize(num_blocks);
	block_phis_.resize(num_blocks);
	block_insts_.resize(num_blocks);

	// First pass: assign a dense id to every Operation (arguments + body ops).
	// Ordering: function-level args first, then per-block operations in order.
	uint32_t next_id = 0;

	// Function arguments (BasicBlockArguments of entry block) become the
	// "cur_args()" range.  They are stored as the entry block's argument list.
	const ir::BasicBlock& entry_block = *bb_list[0];
	for (auto& arg : entry_block.getArguments()) {
		uint32_t id = arg->getIdentifier().getId();
		// Ensure the val_names_ and inst_* vectors are large enough.
		auto grow = [&](uint32_t idx) {
			if (idx >= static_cast<uint32_t>(val_names_.size())) {
				val_names_.resize(idx + 1);
				inst_operands_.resize(idx + 1);
				inst_has_result_.resize(idx + 1, false);
			}
		};
		grow(id);
		op_map_[id] = arg.get();
		val_local_idx_map_[id] = next_id++;
		arg_val_ids_.push_back(id);
		std::ostringstream ss;
		ss << "arg_" << id;
		val_names_[id] = ss.str();
		inst_has_result_[id] = true;
	}

	// Now iterate all blocks and all operations.
	for (uint32_t bi = 0; bi < num_blocks; ++bi) {
		const ir::BasicBlock* blk = bb_list[bi].get();
		blocks_.push_back(blk);
		block_names_.push_back(blk->getIdentifier());

		// Record phi values (BasicBlockArguments of non-entry blocks).
		// Entry block args are function parameters, not phi nodes.
		if (bi > 0) {
			for (auto& arg : blk->getArguments()) {
				uint32_t id = arg->getIdentifier().getId();
				auto grow = [&](uint32_t idx) {
					if (idx >= static_cast<uint32_t>(val_names_.size())) {
						val_names_.resize(idx + 1);
						inst_operands_.resize(idx + 1);
						inst_has_result_.resize(idx + 1, false);
					}
				};
				grow(id);
				op_map_[id] = arg.get();
				val_local_idx_map_[id] = next_id++;
				block_phis_[bi].push_back(id);
				std::ostringstream ss;
				ss << "phi_" << id;
				val_names_[id] = ss.str();
				inst_has_result_[id] = true;
			}
		}

		// Regular instructions.
		for (auto& op_ptr : blk->getOperations()) {
			const ir::Operation* op = op_ptr.get();
			uint32_t id = op->getIdentifier().getId();

			// Grow auxiliary tables.
			if (id >= static_cast<uint32_t>(val_names_.size())) {
				val_names_.resize(id + 1);
				inst_operands_.resize(id + 1);
				inst_has_result_.resize(id + 1, false);
			}

			op_map_[id] = op;

			// Operands
			for (const ir::Operation* inp : op->getInputs()) {
				inst_operands_[id].push_back(inp->getIdentifier().getId());
			}

			// Does this operation produce a result value?
			bool produces_result = true;
			switch (op->getOperationType()) {
				case OT::StoreOp:
				case OT::BranchOp:
				case OT::IfOp:
				case OT::ReturnOp:
				case OT::BlockInvocation:
				case OT::LoopOp:
				case OT::MLIR_YIELD:
					produces_result = false;
					break;
				default:
					break;
			}
			inst_has_result_[id] = produces_result;

			// Allocas: track for cur_static_allocas.
			if (op->getOperationType() == OT::AllocaOp) {
				alloca_val_ids_.push_back(id);
			}

			// Skip FunctionOp, BlockInvocation, LoopOp, MLIR_YIELD as instructions —
			// they are structural, not emitted by compile_inst.
			switch (op->getOperationType()) {
				case OT::FunctionOp:
				case OT::BlockInvocation:
				case OT::LoopOp:
				case OT::MLIR_YIELD:
					break;
				default:
					block_insts_[bi].push_back(id);
					val_local_idx_map_[id] = next_id++;
					break;
			}

			std::ostringstream ss;
			ss << op->getIdentifier().toString();
			val_names_[id] = ss.str();
		}

		// Compute successors from the terminator.
		ir::Operation* term = const_cast<ir::BasicBlock*>(blk)->getTerminatorOp();
		if (term == nullptr)
			continue;

		switch (term->getOperationType()) {
			case OT::BranchOp: {
				auto* br = term->dynCast<ir::BranchOperation>();
				const ir::BasicBlock* tgt = br->getNextBlockInvocation().getBlock();
				// Find index of tgt
				for (uint32_t j = 0; j < num_blocks; ++j) {
					if (bb_list[j].get() == tgt) {
						block_succs_[bi].push_back(j);
						break;
					}
				}
				break;
			}
			case OT::IfOp: {
				auto* ifop = term->dynCast<ir::IfOperation>();
				const ir::BasicBlock* true_blk = ifop->getTrueBlockInvocation().getBlock();
				const ir::BasicBlock* false_blk = ifop->getFalseBlockInvocation().getBlock();
				for (uint32_t j = 0; j < num_blocks; ++j) {
					if (bb_list[j].get() == true_blk)
						block_succs_[bi].push_back(j);
				}
				for (uint32_t j = 0; j < num_blocks; ++j) {
					if (bb_list[j].get() == false_blk)
						block_succs_[bi].push_back(j);
				}
				break;
			}
			default:
				// ReturnOp → no successors
				break;
		}
	}

	highest_val_idx_ = next_id > 0 ? next_id - 1 : 0;
}

void NautilusIRAdaptor::compute_phi_incoming(const ir::FunctionOperation& func) {
	const auto& bb_list = func.getBasicBlocks();
	const uint32_t num_blocks = static_cast<uint32_t>(bb_list.size());

	// For each non-entry block with phi arguments, collect incoming
	// (value, block) pairs from every predecessor's BlockInvocation.
	for (uint32_t bi = 1; bi < num_blocks; ++bi) {
		const ir::BasicBlock* blk = bb_list[bi].get();
		const auto& args = blk->getArguments();
		if (args.empty())
			continue;

		// Initialize empty PHIRef for each argument.
		for (auto& arg : args) {
			uint32_t phi_id = arg->getIdentifier().getId();
			NautilusPHIRef ref;
			ref.arg = arg.get();
			phi_map_[phi_id] = std::move(ref);
		}

		// Walk predecessors and extract their BlockInvocations.
		for (const ir::BasicBlock* pred : const_cast<ir::BasicBlock*>(blk)->getPredecessors()) {
			// Find block index of pred
			uint32_t pred_idx = ~0u;
			for (uint32_t j = 0; j < num_blocks; ++j) {
				if (bb_list[j].get() == pred) {
					pred_idx = j;
					break;
				}
			}
			assert(pred_idx != ~0u);
			IRBlockRef pred_ref = static_cast<IRBlockRef>(pred_idx);

			// Find the BasicBlockInvocation in pred that targets blk.
			ir::Operation* term = const_cast<ir::BasicBlock*>(pred)->getTerminatorOp();
			if (term == nullptr)
				continue;

			auto process_invocation = [&](const ir::BasicBlockInvocation& inv) {
				if (inv.getBlock() != blk)
					return;
				const auto& inv_args = inv.getArguments();
				for (uint32_t ai = 0; ai < static_cast<uint32_t>(args.size()); ++ai) {
					if (ai >= inv_args.size())
						break;
					uint32_t phi_id = args[ai]->getIdentifier().getId();
					uint32_t val_id = inv_args[ai]->getIdentifier().getId();
					auto& phi_ref = phi_map_.at(phi_id);
					phi_ref.incoming_vals.push_back(IRValueRef{val_id});
					phi_ref.incoming_blocks.push_back(pred_ref);
				}
			};

			switch (term->getOperationType()) {
				case OT::BranchOp: {
					auto* br = term->dynCast<ir::BranchOperation>();
					process_invocation(br->getNextBlockInvocation());
					break;
				}
				case OT::IfOp: {
					auto* ifop = term->dynCast<ir::IfOperation>();
					process_invocation(ifop->getTrueBlockInvocation());
					process_invocation(ifop->getFalseBlockInvocation());
					break;
				}
				default:
					break;
			}
		}
	}
}

uint32_t NautilusIRAdaptor::val_alloca_size(IRValueRef val) const {
	auto it = op_map_.find(val.id);
	assert(it != op_map_.end());
	auto* alloca = it->second->dynCast<ir::AllocaOperation>();
	assert(alloca != nullptr);
	return static_cast<uint32_t>(alloca->getSize());
}

std::string_view NautilusIRAdaptor::val_name(uint32_t id) const {
	if (id < static_cast<uint32_t>(val_names_.size()))
		return val_names_[id];
	return "?";
}

static_assert(::tpde::IRAdaptor<NautilusIRAdaptor>);

} // namespace nautilus::compiler::tpde

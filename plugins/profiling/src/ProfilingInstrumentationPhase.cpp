#include "ProfilingInstrumentationPhase.hpp"
#include "nautilus/common/FunctionAttributes.hpp"
#include "nautilus/compiler/ir/IRGraph.hpp"
#include "nautilus/compiler/ir/blocks/BasicBlock.hpp"
#include "nautilus/compiler/ir/operations/ConstPtrOperation.hpp"
#include "nautilus/compiler/ir/operations/Operation.hpp"
#include "nautilus/compiler/ir/operations/ProxyCallOperation.hpp"
#include "nautilus/profiling/profiler.hpp"
#include <array>

namespace nautilus::profiling {

using namespace nautilus::compiler::ir;

// C-style symbol names (no ::) so the C++ backend can emit valid source.
static constexpr const char* RUNTIME_START_SYMBOL = "nautilus_profiling_runtimeStart";
static constexpr const char* RUNTIME_STOP_SYMBOL = "nautilus_profiling_runtimeStop";

ProfilingIRPass::ProfilingIRPass() : profiler_(std::make_shared<Profiler>()) {
}

std::string ProfilingIRPass::getName() const {
	return "profilingInstrumentation";
}

OperationIdentifier ProfilingIRPass::nextIdentifier() {
	return OperationIdentifier(nextId_++);
}

void ProfilingIRPass::apply(IRGraph& ir) {
	// Seed the id counter above any existing id so newly minted operations
	// never collide with ids already in use.
	uint32_t maxId = 0;
	for (auto* func : ir.getFunctionOperations()) {
		for (auto* block : func->getBasicBlocks()) {
			for (auto* op : block->getOperations()) {
				maxId = std::max(maxId, op->getIdentifier().getId());
			}
			for (auto* arg : block->getArguments()) {
				maxId = std::max(maxId, arg->getIdentifier().getId());
			}
		}
	}
	nextId_ = maxId + 1;

	for (auto* func : ir.getFunctionOperations()) {
		auto* region = profiler_->getOrCreateRegion(func->getName());
		instrumentFunction(ir, *func, region);
	}
}

void ProfilingIRPass::insertProxyCall(IRGraph& ir, BasicBlock& block, Operation* before, const char* symbol,
                                      void* functionPtr, Operation* regionPtr) {
	auto& arena = ir.getArena();
	std::array<Operation*, 1> inputs {regionPtr};
	auto* call = arena.create<ProxyCallOperation>(arena, symbol, symbol, functionPtr, nextIdentifier(),
	                                              std::span<Operation* const>(inputs.data(), inputs.size()), Type::v,
	                                              FunctionAttributes {});
	block.addOperationBefore(before, call);
}

void ProfilingIRPass::instrumentFunction(IRGraph& ir, FunctionOperation& func, ProfileRegion* region) {
	auto& blocks = func.getBasicBlocks();
	if (blocks.empty()) {
		return;
	}

	auto& arena = ir.getArena();
	auto* voidRegion = static_cast<void*>(region);

	// Entry: materialize a constant pointer to the region, then emit the
	// start call before the first existing op.
	auto* entryBlock = blocks[0];
	auto* firstOp = entryBlock->getOperationAt(0);
	auto* entryConstPtr = arena.create<ConstPtrOperation>(arena, nextIdentifier(), voidRegion);
	entryBlock->addOperationBefore(firstOp, entryConstPtr);
	insertProxyCall(ir, *entryBlock, firstOp, RUNTIME_START_SYMBOL,
	                reinterpret_cast<void*>(&ProfileRegion::runtimeStart), entryConstPtr);

	// Each return block needs its own ConstPtrOperation — IR values are
	// block-scoped (SSA), so the entry-block constant cannot be referenced
	// from another block.
	// Limitation: functions that never return (infinite loops, exceptions
	// thrown out of invoke()) will fire runtimeStart but no runtimeStop.
	for (auto* block : blocks) {
		auto* terminator = block->getTerminatorOp();
		if (terminator->getOperationType() != Operation::OperationType::ReturnOp) {
			continue;
		}
		Operation* regionPtr = entryConstPtr;
		if (block != entryBlock) {
			regionPtr = arena.create<ConstPtrOperation>(arena, nextIdentifier(), voidRegion);
			block->addOperationBefore(terminator, regionPtr);
		}
		insertProxyCall(ir, *block, terminator, RUNTIME_STOP_SYMBOL,
		                reinterpret_cast<void*>(&ProfileRegion::runtimeStop), regionPtr);
	}
}

} // namespace nautilus::profiling

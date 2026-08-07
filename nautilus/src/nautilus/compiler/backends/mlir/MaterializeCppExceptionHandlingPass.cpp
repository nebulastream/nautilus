#include "nautilus/compiler/backends/mlir/MaterializeCppExceptionHandlingPass.hpp"
#include "nautilus/compiler/ir/IRGraph.hpp"
#include "nautilus/compiler/ir/operations/FunctionOperation.hpp"
#include <fmt/format.h>
#include <iterator>
#include <llvm/ADT/SmallVector.h>
#include <mlir/Dialect/LLVMIR/LLVMDialect.h>
#include <mlir/IR/Builders.h>
#include <mlir/IR/SymbolTable.h>
#include <mlir/IR/Verifier.h>
#include <unordered_map>

namespace nautilus::compiler::mlir {

std::string cleanupDestructorSymbol(void* functionPtr) {
	return fmt::format("__nautilus_cleanup_dtor_{:x}", reinterpret_cast<uintptr_t>(functionPtr));
}

namespace {

::mlir::LLVM::LLVMFuncOp getOrCreatePersonality(::mlir::ModuleOp module) {
	constexpr auto symbol = "__gxx_personality_v0";
	if (auto function = module.lookupSymbol<::mlir::LLVM::LLVMFuncOp>(symbol)) {
		return function;
	}
	::mlir::OpBuilder builder(module.getContext());
	builder.setInsertionPointToStart(module.getBody());
	auto type = ::mlir::LLVM::LLVMFunctionType::get(builder.getI32Type(), {}, true);
	return ::mlir::LLVM::LLVMFuncOp::create(builder, module.getLoc(), symbol, type);
}

::mlir::LogicalResult materializeFunction(::mlir::ModuleOp module, ::mlir::LLVM::LLVMFuncOp function,
                                          const ir::FunctionOperation& irFunction) {
	if (!irFunction.hasExceptionRegion() || irFunction.getExceptionRegion().pads.empty()) {
		return ::mlir::success();
	}

	std::unordered_map<AllocaIndex, ::mlir::Value> allocaSlots;
	llvm::SmallVector<::mlir::LLVM::CallOp> markedCalls;
	function.walk([&](::mlir::Operation* operation) {
		if (auto alloca = llvm::dyn_cast<::mlir::LLVM::AllocaOp>(operation)) {
			if (auto index = operation->getAttrOfType<::mlir::IntegerAttr>(ALLOCA_INDEX_ATTR)) {
				allocaSlots.emplace(static_cast<AllocaIndex>(index.getUInt()), alloca.getResult());
			}
		} else if (auto call = llvm::dyn_cast<::mlir::LLVM::CallOp>(operation)) {
			if (operation->hasAttr(CLEANUP_PAD_ATTR)) {
				markedCalls.push_back(call);
			}
		}
	});
	if (markedCalls.empty()) {
		function.emitError("logical exception region has cleanup pads but no marked LLVM calls");
		return ::mlir::failure();
	}

	getOrCreatePersonality(module);
	function.setPersonality("__gxx_personality_v0");
	::mlir::OpBuilder builder(module.getContext());
	auto pointerType = ::mlir::LLVM::LLVMPointerType::get(module.getContext());
	auto exceptionType =
	    ::mlir::LLVM::LLVMStructType::getLiteral(module.getContext(), {pointerType, builder.getI32Type()});
	std::vector<::mlir::Block*> cleanupBlocks(irFunction.getExceptionRegion().pads.size(), nullptr);

	for (const auto& pad : irFunction.getExceptionRegion().pads) {
		auto* cleanupBlock = new ::mlir::Block();
		function.getBody().push_back(cleanupBlock);
		cleanupBlocks[pad.id] = cleanupBlock;
		builder.setInsertionPointToStart(cleanupBlock);
		auto landingpad =
		    ::mlir::LLVM::LandingpadOp::create(builder, function.getLoc(), exceptionType, true, ::mlir::ValueRange {});
		for (auto position = pad.active.rbegin(); position != pad.active.rend(); ++position) {
			auto slot = allocaSlots.find(*position);
			if (slot == allocaSlots.end()) {
				function.emitError() << "cleanup pad " << pad.id << " references missing alloca " << *position;
				return ::mlir::failure();
			}
			const auto& destructor = irFunction.getAllocaSpecs().at(*position).destructor;
			if (!destructor.has_value()) {
				function.emitError() << "cleanup pad " << pad.id << " references alloca without destructor";
				return ::mlir::failure();
			}
			::mlir::LLVM::CallOp::create(builder, function.getLoc(), ::mlir::TypeRange {},
			                             cleanupDestructorSymbol(destructor->functionPtr),
			                             ::mlir::ValueRange {slot->second});
		}
		::mlir::LLVM::ResumeOp::create(builder, function.getLoc(), landingpad.getResult());
	}

	for (auto call : markedCalls) {
		auto padAttr = call->getAttrOfType<::mlir::IntegerAttr>(CLEANUP_PAD_ATTR);
		const auto pad = static_cast<size_t>(padAttr.getUInt());
		if (pad >= cleanupBlocks.size() || cleanupBlocks[pad] == nullptr) {
			call.emitError("references an unknown Nautilus cleanup pad");
			return ::mlir::failure();
		}
		auto* callBlock = call->getBlock();
		auto* normalBlock = callBlock->splitBlock(std::next(call->getIterator()));
		builder.setInsertionPoint(call);
		auto invoke = ::mlir::LLVM::InvokeOp::create(builder, call.getLoc(), call.getCalleeFunctionType(),
		                                             call.getCalleeAttr(), call.getCalleeOperands(), normalBlock,
		                                             ::mlir::ValueRange {}, cleanupBlocks[pad], ::mlir::ValueRange {});
		call->replaceAllUsesWith(invoke->getResults());
		call.erase();
	}
	return ::mlir::success();
}

} // namespace

::mlir::LogicalResult materializeCppExceptionHandling(::mlir::ModuleOp module, const ir::IRGraph& ir) {
	for (const auto* irFunction : ir.getFunctionOperations()) {
		if (irFunction == nullptr || !irFunction->hasExceptionRegion() ||
		    irFunction->getExceptionRegion().pads.empty()) {
			continue;
		}
		auto function = module.lookupSymbol<::mlir::LLVM::LLVMFuncOp>(irFunction->getName());
		if (!function) {
			module.emitError() << "missing LLVM function for exception region '" << irFunction->getName() << "'";
			return ::mlir::failure();
		}
		if (::mlir::failed(materializeFunction(module, function, *irFunction))) {
			return ::mlir::failure();
		}
	}
	return ::mlir::verify(module);
}

} // namespace nautilus::compiler::mlir

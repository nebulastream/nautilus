// MLIR intrinsic plugin that rewrites the three profile runtime calls to
// inline llvm.readcyclecounter() + a direct call to the pre-timed _ticks
// variant. Effect: in JIT'd Nautilus code a region event costs a single
// cycle-counter read plus one call, instead of the default path's
// clock_gettime vDSO entry.
//
// Matching is by function pointer — the address that ProxyCallOperation
// recorded at trace time must equal the address of the C-ABI entry point
// here (same TU chain), which is true because the plugin is linked into
// every Nautilus program that uses the profile plugin.

// Pulls in MLIRBackendIntrinsic (for the plugin base) which transitively
// brings MLIRLoweringProvider and its dependencies, including
// ProxyCallOperation. The ProxyCallOperation header itself is missing
// `#pragma once` (pre-existing), so we rely on the indirect include to
// avoid a redefinition error.
#include "nautilus/compiler/backends/mlir/intrinsics/MLIRBackendIntrinsic.hpp"
#include "nautilus/profile/MLIR.hpp"
#include "nautilus/profile/profile_runtime.h"
#include <cstdint>
#include <mlir/Dialect/LLVMIR/LLVMDialect.h>
#include <mlir/Dialect/LLVMIR/LLVMTypes.h>
#include <mutex>

namespace nautilus::profile {

namespace {

using namespace nautilus::compiler::mlir;
using nautilus::compiler::ir::ProxyCallOperation;

// Emits the prologue common to every profiler intrinsic:
//   %ticks = llvm.call_intrinsic "llvm.readcyclecounter"() : () -> i64
// Returns the i64 value.
::mlir::Value emitReadCycleCounter(::mlir::OpBuilder& builder) {
	auto loc = builder.getUnknownLoc();
	auto i64 = builder.getI64Type();
	// Uses the single-result build: (resultType, StringAttr intrin, ValueRange args).
	auto op = builder.create<::mlir::LLVM::CallIntrinsicOp>(
	    loc, ::mlir::Type {i64}, builder.getStringAttr("llvm.readcyclecounter"), ::mlir::ValueRange {});
	return op.getResult(0);
}

// Bakes `ticksFnAddr` as an i64 constant in the JIT IR, casts to a function
// pointer, and emits an indirect LLVM call to it with the trace's arguments
// followed by the cycle-counter value. Used by each of the three
// intrinsics below.
template <typename Fn>
bool lowerWithTicks(::mlir::OpBuilder& builder, const ProxyCallOperation* call, MLIRLoweringProvider::ValueFrame& frame,
                    Fn* ticksFnAddr) {
	auto loc = builder.getUnknownLoc();
	auto i64 = builder.getI64Type();
	auto voidTy = ::mlir::LLVM::LLVMVoidType::get(builder.getContext());
	auto ptrTy = ::mlir::LLVM::LLVMPointerType::get(builder.getContext());

	// 1. Read cycle counter inline.
	auto ticks = emitReadCycleCounter(builder);

	// 2. Gather arguments from the traced call, in the original order, and
	//    append the ticks value as the final argument.
	llvm::SmallVector<::mlir::Value, 4> args;
	llvm::SmallVector<::mlir::Type, 4> argTypes;
	for (const auto& input : call->getInputArguments()) {
		auto v = frame.getValue(input->getIdentifier());
		args.push_back(v);
		argTypes.push_back(v.getType());
	}
	args.push_back(ticks);
	argTypes.push_back(i64);

	// 3. Bake the runtime entry point's address as a constant function pointer.
	auto addrAttr = builder.getI64IntegerAttr(reinterpret_cast<int64_t>(ticksFnAddr));
	auto addrConst = builder.create<::mlir::LLVM::ConstantOp>(loc, i64, addrAttr);
	auto fnPtr = builder.create<::mlir::LLVM::IntToPtrOp>(loc, ptrTy, ::mlir::ValueRange {addrConst});

	// 4. Indirect call through the pointer, with inputs + ticks. Matches
	//    how MLIRLoweringProvider::visitIndirectCall emits LLVM::CallOp:
	//    the callee pointer is the first operand, args follow.
	auto fnType = ::mlir::LLVM::LLVMFunctionType::get(voidTy, argTypes);
	llvm::SmallVector<::mlir::Value, 5> allOperands;
	allOperands.push_back(fnPtr);
	allOperands.append(args.begin(), args.end());
	builder.create<::mlir::LLVM::CallOp>(loc, fnType, allOperands);
	return true;
}

class NautilusProfileIntrinsicPlugin : public MLIRIntrinsicPlugin {
public:
	void registerIntrinsics(MLIRIntrinsicManager& manager) override {
		manager.addIntrinsic(reinterpret_cast<void*>(&__nautilus_profile_begin),
		                     [](std::unique_ptr<::mlir::OpBuilder>& builder, const ProxyCallOperation* call,
		                        MLIRLoweringProvider::ValueFrame& frame) -> bool {
			                     return lowerWithTicks(*builder, call, frame, &__nautilus_profile_begin_ticks);
		                     });
		manager.addIntrinsic(reinterpret_cast<void*>(&__nautilus_profile_end),
		                     [](std::unique_ptr<::mlir::OpBuilder>& builder, const ProxyCallOperation* call,
		                        MLIRLoweringProvider::ValueFrame& frame) -> bool {
			                     return lowerWithTicks(*builder, call, frame, &__nautilus_profile_end_ticks);
		                     });
		manager.addIntrinsic(reinterpret_cast<void*>(&__nautilus_profile_counter_i64),
		                     [](std::unique_ptr<::mlir::OpBuilder>& builder, const ProxyCallOperation* call,
		                        MLIRLoweringProvider::ValueFrame& frame) -> bool {
			                     return lowerWithTicks(*builder, call, frame, &__nautilus_profile_counter_i64_ticks);
		                     });
	}
	~NautilusProfileIntrinsicPlugin() override = default;
};

} // namespace

void registerMLIRIntrinsics() {
	// Idempotent: adding the plugin twice is harmless — getIntrinsic returns
	// the first-registered handler. A mutex guards concurrent first-touch.
	static std::once_flag flag;
	std::call_once(flag, [] {
		MLIRIntrinsicPluginRegistry::instance().addPlugin(std::make_shared<NautilusProfileIntrinsicPlugin>());
	});
}

} // namespace nautilus::profile

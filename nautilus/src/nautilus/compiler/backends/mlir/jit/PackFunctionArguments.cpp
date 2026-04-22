// Ported verbatim from llvm-project llvmorg-21.1.2:
//   mlir/lib/ExecutionEngine/ExecutionEngine.cpp
// See https://llvm.org/LICENSE.txt (Apache-2.0 with LLVM exception).

#include "nautilus/compiler/backends/mlir/jit/PackFunctionArguments.hpp"
#include <llvm/ADT/APInt.h>
#include <llvm/ADT/DenseSet.h>
#include <llvm/ADT/STLExtras.h>
#include <llvm/ADT/SmallVector.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Value.h>

namespace nautilus::compiler::mlir::detail {

std::string makePackedFunctionName(llvm::StringRef name) {
	return "_mlir_" + name.str();
}

// For each function in the LLVM module, define an interface function that wraps
// all the arguments of the original function and all its results into an i8**
// pointer to provide a unified invocation interface.
void packFunctionArguments(llvm::Module* module) {
	auto& ctx = module->getContext();
	llvm::IRBuilder<> builder(ctx);
	llvm::DenseSet<llvm::Function*> interfaceFunctions;
	for (auto& func : module->getFunctionList()) {
		if (func.isDeclaration()) {
			continue;
		}
		if (interfaceFunctions.count(&func)) {
			continue;
		}

		// Given a function `foo(<...>)`, define the interface function
		// `mlir_foo(i8**)`.
		auto* newType = llvm::FunctionType::get(builder.getVoidTy(), builder.getPtrTy(),
		                                        /*isVarArg=*/false);
		auto newName = makePackedFunctionName(func.getName());
		auto funcCst = module->getOrInsertFunction(newName, newType);
		auto* interfaceFunc = llvm::cast<llvm::Function>(funcCst.getCallee());
		interfaceFunctions.insert(interfaceFunc);

		// Extract the arguments from the type-erased argument list and cast them
		// to the proper types.
		auto* bb = llvm::BasicBlock::Create(ctx);
		bb->insertInto(interfaceFunc);
		builder.SetInsertPoint(bb);
		llvm::Value* argList = interfaceFunc->arg_begin();
		llvm::SmallVector<llvm::Value*, 8> args;
		args.reserve(llvm::size(func.args()));
		for (auto [index, arg] : llvm::enumerate(func.args())) {
			llvm::Value* argIndex = llvm::Constant::getIntegerValue(builder.getInt64Ty(), llvm::APInt(64, index));
			llvm::Value* argPtrPtr = builder.CreateGEP(builder.getPtrTy(), argList, argIndex);
			llvm::Value* argPtr = builder.CreateLoad(builder.getPtrTy(), argPtrPtr);
			llvm::Type* argTy = arg.getType();
			llvm::Value* load = builder.CreateLoad(argTy, argPtr);
			args.push_back(load);
		}

		// Call the implementation function with the extracted arguments.
		llvm::Value* result = builder.CreateCall(&func, args);

		// Assuming the result is one value, potentially of type `void`.
		if (!result->getType()->isVoidTy()) {
			llvm::Value* retIndex =
			    llvm::Constant::getIntegerValue(builder.getInt64Ty(), llvm::APInt(64, llvm::size(func.args())));
			llvm::Value* retPtrPtr = builder.CreateGEP(builder.getPtrTy(), argList, retIndex);
			llvm::Value* retPtr = builder.CreateLoad(builder.getPtrTy(), retPtrPtr);
			builder.CreateStore(result, retPtr);
		}

		// The interface function returns void.
		builder.CreateRetVoid();
	}
}

} // namespace nautilus::compiler::mlir::detail


#include "MetalKernelCompiler.hpp"
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <llvm/Bitcode/BitcodeWriter.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/Support/raw_ostream.h>
#include <mlir/Conversion/ArithToLLVM/ArithToLLVM.h>
#include <mlir/Conversion/ControlFlowToLLVM/ControlFlowToLLVM.h>
#include <mlir/Conversion/FuncToLLVM/ConvertFuncToLLVMPass.h>
#include <mlir/Conversion/MathToLLVM/MathToLLVM.h>
#include <mlir/Conversion/ReconcileUnrealizedCasts/ReconcileUnrealizedCasts.h>
#include <mlir/Dialect/Func/IR/FuncOps.h>
#include <mlir/Dialect/LLVMIR/LLVMDialect.h>
#include <mlir/IR/Builders.h>
#include <mlir/IR/BuiltinOps.h>
#include <mlir/IR/IRMapping.h>
#include <mlir/Pass/PassManager.h>
#include <mlir/Target/LLVMIR/Dialect/Builtin/BuiltinToLLVMIRTranslation.h>
#include <mlir/Target/LLVMIR/Dialect/LLVMIR/LLVMToLLVMIRTranslation.h>
#include <mlir/Target/LLVMIR/Export.h>

namespace nautilus::gpu {

/// Names of the GPU intrinsic stub functions as they appear in MLIR after lowering.
/// These are matched by suffix since the mangled name may have a prefix.
static const std::vector<std::pair<std::string, int>> kThreadIdxIntrinsics = {
    {"gpu_threadIdx_x", 0},
    {"gpu_threadIdx_y", 1},
    {"gpu_threadIdx_z", 2},
};
static const std::vector<std::pair<std::string, int>> kBlockIdxIntrinsics = {
    {"gpu_blockIdx_x", 0},
    {"gpu_blockIdx_y", 1},
    {"gpu_blockIdx_z", 2},
};
static const std::vector<std::pair<std::string, int>> kBlockDimIntrinsics = {
    {"gpu_blockDim_x", 0},
    {"gpu_blockDim_y", 1},
    {"gpu_blockDim_z", 2},
};
static const std::vector<std::pair<std::string, int>> kGridDimIntrinsics = {
    {"gpu_gridDim_x", 0},
    {"gpu_gridDim_y", 1},
    {"gpu_gridDim_z", 2},
};

/// Check if a function name (possibly mangled) ends with a given suffix.
static bool nameEndsWith(llvm::StringRef name, const std::string& suffix) {
	return name.contains(suffix);
}

/// Post-process LLVM IR module for Metal/AIR target.
///
/// This transforms the kernel function to be Metal-compatible:
/// 1. Adds extra parameters for thread indices (threadIdx, blockIdx, blockDim, gridDim)
/// 2. Replaces calls to GPU intrinsic stubs with uses of those parameters
/// 3. Marks the function as an AIR kernel via metadata
/// 4. Sets address space 1 (device) on pointer arguments
/// 5. Sets the target triple and data layout for AIR
static void postProcessLLVMForAIR(llvm::Module& module) {
	// AIR target triple and data layout for Apple Silicon
	module.setTargetTriple("air64-apple-macosx15.0.0");
	module.setDataLayout("e-p:64:64:64-i1:8:8-i8:8:8-i16:16:16-i32:32:32-i64:64:64-f32:32:32-f64:64:64-"
	                     "v16:16:16-v24:32:32-v32:32:32-v48:64:64-v64:64:64-v96:128:128-v128:128:128-"
	                     "v192:256:256-v256:256:256-v512:512:512-v1024:1024:1024-n8:16:32");

	auto& ctx = module.getContext();

	// Find the kernel function (the non-declaration function in the module)
	llvm::Function* kernelFn = nullptr;
	for (auto& fn : module) {
		if (!fn.isDeclaration()) {
			kernelFn = &fn;
			break;
		}
	}
	if (!kernelFn) {
		return;
	}

	// Collect all GPU intrinsic calls and which parameters we need to add
	bool needsThreadIdx = false;
	bool needsBlockIdx = false;
	bool needsBlockDim = false;
	bool needsGridDim = false;
	bool needsSyncThreads = false;

	for (auto& bb : *kernelFn) {
		for (auto& inst : bb) {
			if (auto* call = llvm::dyn_cast<llvm::CallInst>(&inst)) {
				auto* callee = call->getCalledFunction();
				if (!callee) {
					continue;
				}
				auto calleeName = callee->getName();
				for (auto& [name, _] : kThreadIdxIntrinsics) {
					if (nameEndsWith(calleeName, name)) {
						needsThreadIdx = true;
					}
				}
				for (auto& [name, _] : kBlockIdxIntrinsics) {
					if (nameEndsWith(calleeName, name)) {
						needsBlockIdx = true;
					}
				}
				for (auto& [name, _] : kBlockDimIntrinsics) {
					if (nameEndsWith(calleeName, name)) {
						needsBlockDim = true;
					}
				}
				for (auto& [name, _] : kGridDimIntrinsics) {
					if (nameEndsWith(calleeName, name)) {
						needsGridDim = true;
					}
				}
				if (nameEndsWith(calleeName, "gpu_syncThreads")) {
					needsSyncThreads = true;
				}
			}
		}
	}

	// Build new function type with extra i32 parameters for thread indices.
	// Metal passes these as kernel parameters annotated with metadata.
	// Order: original args, then [threadIdx.x/y/z], [blockIdx.x/y/z], [blockDim.x/y/z], [gridDim.x/y/z]
	auto* origFnTy = kernelFn->getFunctionType();
	std::vector<llvm::Type*> newParamTypes(origFnTy->params().begin(), origFnTy->params().end());

	auto* i32Ty = llvm::Type::getInt32Ty(ctx);

	// Track the parameter indices for each thread index group
	int threadIdxParamStart = -1;
	int blockIdxParamStart = -1;
	int blockDimParamStart = -1;
	int gridDimParamStart = -1;

	if (needsThreadIdx) {
		threadIdxParamStart = static_cast<int>(newParamTypes.size());
		newParamTypes.push_back(i32Ty); // threadIdx.x
		newParamTypes.push_back(i32Ty); // threadIdx.y
		newParamTypes.push_back(i32Ty); // threadIdx.z
	}
	if (needsBlockIdx) {
		blockIdxParamStart = static_cast<int>(newParamTypes.size());
		newParamTypes.push_back(i32Ty); // blockIdx.x
		newParamTypes.push_back(i32Ty); // blockIdx.y
		newParamTypes.push_back(i32Ty); // blockIdx.z
	}
	if (needsBlockDim) {
		blockDimParamStart = static_cast<int>(newParamTypes.size());
		newParamTypes.push_back(i32Ty); // blockDim.x
		newParamTypes.push_back(i32Ty); // blockDim.y
		newParamTypes.push_back(i32Ty); // blockDim.z
	}
	if (needsGridDim) {
		gridDimParamStart = static_cast<int>(newParamTypes.size());
		newParamTypes.push_back(i32Ty); // gridDim.x
		newParamTypes.push_back(i32Ty); // gridDim.y
		newParamTypes.push_back(i32Ty); // gridDim.z
	}

	// Create new function with extended parameter list
	auto* newFnTy = llvm::FunctionType::get(origFnTy->getReturnType(), newParamTypes, false);
	auto* newFn = llvm::Function::Create(newFnTy, kernelFn->getLinkage(), kernelFn->getName() + "_metal", &module);

	// Map old arguments to new arguments
	llvm::ValueToValueMapTy vmap;
	auto newArgIt = newFn->arg_begin();
	for (auto& oldArg : kernelFn->args()) {
		newArgIt->setName(oldArg.getName());
		vmap[&oldArg] = &*newArgIt;
		++newArgIt;
	}

	// Name the extra thread index parameters
	auto nameParam = [&](int start, const char* prefix) {
		if (start < 0) {
			return;
		}
		auto it = newFn->arg_begin() + start;
		(it + 0)->setName(llvm::Twine(prefix) + ".x");
		(it + 1)->setName(llvm::Twine(prefix) + ".y");
		(it + 2)->setName(llvm::Twine(prefix) + ".z");
	};
	nameParam(threadIdxParamStart, "threadIdx");
	nameParam(blockIdxParamStart, "blockIdx");
	nameParam(blockDimParamStart, "blockDim");
	nameParam(gridDimParamStart, "gridDim");

	// Clone the function body
	llvm::SmallVector<llvm::ReturnInst*, 4> returns;
	llvm::CloneFunctionInto(newFn, kernelFn, vmap, llvm::CloneFunctionChangeType::LocalChangesOnly, returns);

	// Replace GPU intrinsic calls with parameter references
	std::vector<llvm::CallInst*> toErase;
	for (auto& bb : *newFn) {
		for (auto& inst : bb) {
			auto* call = llvm::dyn_cast<llvm::CallInst>(&inst);
			if (!call || !call->getCalledFunction()) {
				continue;
			}
			auto calleeName = call->getCalledFunction()->getName();

			auto tryReplace = [&](const std::vector<std::pair<std::string, int>>& intrinsics, int paramStart) {
				if (paramStart < 0) {
					return false;
				}
				for (auto& [name, dim] : intrinsics) {
					if (nameEndsWith(calleeName, name)) {
						auto* param = newFn->getArg(paramStart + dim);
						call->replaceAllUsesWith(param);
						toErase.push_back(call);
						return true;
					}
				}
				return false;
			};

			if (tryReplace(kThreadIdxIntrinsics, threadIdxParamStart)) {
				continue;
			}
			if (tryReplace(kBlockIdxIntrinsics, blockIdxParamStart)) {
				continue;
			}
			if (tryReplace(kBlockDimIntrinsics, blockDimParamStart)) {
				continue;
			}
			if (tryReplace(kGridDimIntrinsics, gridDimParamStart)) {
				continue;
			}

			// syncThreads -> air.wg.barrier with appropriate flags
			if (nameEndsWith(calleeName, "gpu_syncThreads")) {
				// Metal threadgroup barrier: air.wg.barrier(mem_flags)
				// mem_flags = 2 for mem_threadgroup
				auto* barrierFn =
				    module
				        .getOrInsertFunction(
				            "air.wg.barrier",
				            llvm::FunctionType::get(llvm::Type::getVoidTy(ctx), {llvm::Type::getInt32Ty(ctx)}, false))
				        .getCallee();
				llvm::IRBuilder<> builder(call);
				builder.CreateCall(llvm::cast<llvm::Function>(barrierFn),
				                   {llvm::ConstantInt::get(llvm::Type::getInt32Ty(ctx), 2)});
				toErase.push_back(call);
				continue;
			}
		}
	}

	// Erase replaced calls
	for (auto* call : toErase) {
		call->eraseFromParent();
	}

	// Remove declarations for the GPU stub functions
	std::vector<llvm::Function*> declsToRemove;
	for (auto& fn : module) {
		if (fn.isDeclaration()) {
			auto fnName = fn.getName();
			if (fnName.contains("gpu_threadIdx") || fnName.contains("gpu_blockIdx") ||
			    fnName.contains("gpu_blockDim") || fnName.contains("gpu_gridDim") ||
			    fnName.contains("gpu_syncThreads") || fnName.contains("gpu_set_grid") ||
			    fnName.contains("gpu_set_block")) {
				declsToRemove.push_back(&fn);
			}
		}
	}
	for (auto* fn : declsToRemove) {
		fn->eraseFromParent();
	}

	// Remove the old kernel function and rename new one
	auto kernelName = kernelFn->getName().str();
	kernelFn->eraseFromParent();
	newFn->setName(kernelName);

	// Add AIR kernel metadata.
	// The metal compiler recognizes functions with "air.kernel" metadata as compute kernels.
	auto* mdString = llvm::MDString::get(ctx, "air.kernel");
	auto* mdNode = llvm::MDNode::get(ctx, {mdString});
	newFn->setMetadata("air.kernel", mdNode);

	// Mark the kernel entry point in module-level metadata
	auto* kernelsMD = module.getOrInsertNamedMetadata("air.kernel");
	auto* kernelRef = llvm::ValueAsMetadata::get(newFn);
	kernelsMD->addOperand(llvm::MDNode::get(ctx, {kernelRef, mdString}));

	// Set pointer args to address space 1 (device) via metadata.
	// Note: actual address space conversion would require rewriting the function type
	// with addrspace(1) pointers. For xcrun metal -x ir, we annotate via metadata instead.
	auto* argMD = module.getOrInsertNamedMetadata("air.arg_type_info");
	for (unsigned i = 0; i < origFnTy->getNumParams(); ++i) {
		if (origFnTy->getParamType(i)->isPointerTy()) {
			auto* idxMD = llvm::ConstantAsMetadata::get(llvm::ConstantInt::get(i32Ty, i));
			auto* asMD = llvm::MDString::get(ctx, "air.buffer");
			auto* rwMD = llvm::MDString::get(ctx, "air.read_write");
			argMD->addOperand(llvm::MDNode::get(ctx, {idxMD, asMD, rwMD}));
		}
	}

	// Mark thread index parameters with AIR metadata
	auto* threadPosMD = module.getOrInsertNamedMetadata("air.thread_position_info");
	auto addThreadParamMD = [&](int paramStart, const char* airAttr) {
		if (paramStart < 0) {
			return;
		}
		for (int dim = 0; dim < 3; ++dim) {
			auto* idxMD = llvm::ConstantAsMetadata::get(llvm::ConstantInt::get(i32Ty, paramStart + dim));
			auto* attrMD = llvm::MDString::get(ctx, airAttr);
			auto* dimMD = llvm::ConstantAsMetadata::get(llvm::ConstantInt::get(i32Ty, dim));
			threadPosMD->addOperand(llvm::MDNode::get(ctx, {idxMD, attrMD, dimMD}));
		}
	};
	addThreadParamMD(threadIdxParamStart, "air.thread_position_in_threadgroup");
	addThreadParamMD(blockIdxParamStart, "air.threadgroup_position_in_grid");
	addThreadParamMD(blockDimParamStart, "air.threads_per_threadgroup");
	addThreadParamMD(gridDimParamStart, "air.threadgroups_per_grid");

	// Suppress needsSyncThreads warning
	(void) needsSyncThreads;
}

bool MetalKernelCompiler::runCommand(const std::string& command) {
	int result = std::system(command.c_str());
	return result == 0;
}

std::vector<uint8_t> MetalKernelCompiler::compile(mlir::func::FuncOp kernelFunc, mlir::MLIRContext& context) {
	// 1. Clone kernel into a fresh MLIR module
	auto moduleOp = mlir::ModuleOp::create(kernelFunc.getLoc());
	mlir::IRMapping mapping;
	moduleOp.push_back(kernelFunc.clone(mapping));

	// 2. Run standard MLIR lowering passes (same as CPU pipeline)
	mlir::PassManager pm(&context);
	pm.addPass(mlir::createConvertMathToLLVMPass());
	pm.addPass(mlir::createConvertFuncToLLVMPass());
	pm.addPass(mlir::createConvertControlFlowToLLVMPass());
	pm.addPass(mlir::createArithToLLVMConversionPass());
	pm.addPass(mlir::createReconcileUnrealizedCastsPass());
	if (mlir::failed(pm.run(moduleOp))) {
		return {};
	}

	// 3. Translate MLIR to LLVM IR
	mlir::registerBuiltinDialectTranslation(context);
	mlir::registerLLVMDialectTranslation(context);
	llvm::LLVMContext llvmCtx;
	auto llvmModule = mlir::translateModuleToLLVMIR(moduleOp, llvmCtx);
	if (!llvmModule) {
		return {};
	}

	// 4. Post-process LLVM IR for Metal/AIR
	postProcessLLVMForAIR(*llvmModule);

	// 5. Write LLVM bitcode to temp file
	auto tmpDir = std::filesystem::temp_directory_path();
	auto bcPath = (tmpDir / "nautilus_kernel.bc").string();
	auto airPath = (tmpDir / "nautilus_kernel.air").string();
	auto metallibPath = (tmpDir / "nautilus_kernel.metallib").string();

	{
		std::error_code ec;
		llvm::raw_fd_ostream bcFile(bcPath, ec);
		if (ec) {
			return {};
		}
		llvm::WriteBitcodeToFile(*llvmModule, bcFile);
	}

	// 6. Compile LLVM bitcode -> AIR using xcrun metal
	auto metalCmd = "xcrun metal -c -x ir " + bcPath + " -o " + airPath + " 2>/dev/null";
	if (!runCommand(metalCmd)) {
		// Fallback: try writing as textual IR instead of bitcode
		auto llPath = (tmpDir / "nautilus_kernel.ll").string();
		{
			std::error_code ec;
			llvm::raw_fd_ostream llFile(llPath, ec);
			if (ec) {
				return {};
			}
			llvmModule->print(llFile, nullptr);
		}
		metalCmd = "xcrun metal -c -x ir " + llPath + " -o " + airPath + " 2>/dev/null";
		if (!runCommand(metalCmd)) {
			return {};
		}
	}

	// 7. Package AIR -> metallib using xcrun metallib
	auto metallibCmd = "xcrun metallib " + airPath + " -o " + metallibPath + " 2>/dev/null";
	if (!runCommand(metallibCmd)) {
		return {};
	}

	// 8. Read metallib bytes
	std::ifstream metallibFile(metallibPath, std::ios::binary | std::ios::ate);
	if (!metallibFile.is_open()) {
		return {};
	}
	auto size = metallibFile.tellg();
	metallibFile.seekg(0, std::ios::beg);
	std::vector<uint8_t> metallib(size);
	metallibFile.read(reinterpret_cast<char*>(metallib.data()), size);

	// 9. Clean up temp files
	std::filesystem::remove(bcPath);
	std::filesystem::remove(airPath);
	std::filesystem::remove(metallibPath);
	// Also try removing .ll fallback file
	std::filesystem::remove((tmpDir / "nautilus_kernel.ll").string());

	return metallib;
}

} // namespace nautilus::gpu

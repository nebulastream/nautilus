
#include "MLIRGPUPass.hpp"
#include "GPUKernelCompiler.hpp"
#include "GPURuntimeLib.hpp"
#include <mlir/Dialect/Arith/IR/Arith.h>
#include <mlir/Dialect/Func/IR/FuncOps.h>
#include <mlir/Dialect/LLVMIR/LLVMDialect.h>
#include <mlir/IR/Builders.h>
#include <mlir/IR/BuiltinOps.h>
#include <mlir/IR/IRMapping.h>

#ifdef ENABLE_GPU_CUDA
#include "cuda/CUDAKernelCompiler.hpp"
#endif
#ifdef ENABLE_GPU_METAL
#include "metal/MetalKernelCompiler.hpp"
#endif

namespace nautilus::gpu {

MLIRGPUPass::MLIRGPUPass() {
	// Select platform-specific kernel compiler.
	// Priority: CUDA on Linux, Metal on macOS.
#ifdef ENABLE_GPU_CUDA
	kernelCompiler_ = std::make_unique<CUDAKernelCompiler>();
#elif defined(ENABLE_GPU_METAL)
	kernelCompiler_ = std::make_unique<MetalKernelCompiler>();
#endif
}

MLIRGPUPass::~MLIRGPUPass() = default;

/// Check if a function symbol name contains a given substring (handles mangled names).
static bool symbolContains(llvm::StringRef name, llvm::StringRef sub) {
	return name.contains(sub);
}

/// Represents a detected GPU kernel launch pattern in the host function.
struct LaunchSite {
	mlir::func::CallOp setGridCall;
	mlir::func::CallOp setBlockCall;
	mlir::func::CallOp kernelCall;
	mlir::func::FuncOp kernelFunc;
};

/// Find all GPU kernel launch patterns in the host function.
/// Pattern: call @*gpu_set_grid*(...) → call @*gpu_set_block*(...) → call @kernel(...)
static std::vector<LaunchSite> findLaunchSites(mlir::func::FuncOp hostFunc, mlir::ModuleOp module) {
	std::vector<LaunchSite> sites;

	// Collect all call ops in order
	llvm::SmallVector<mlir::func::CallOp> callOps;
	hostFunc.walk([&](mlir::func::CallOp call) { callOps.push_back(call); });

	for (size_t i = 0; i + 2 < callOps.size(); ++i) {
		auto call0 = callOps[i];
		auto call1 = callOps[i + 1];
		auto call2 = callOps[i + 2];

		// Check pattern: set_grid, set_block, kernel_call
		if (!symbolContains(call0.getCallee(), "gpu_set_grid")) {
			continue;
		}
		if (!symbolContains(call1.getCallee(), "gpu_set_block")) {
			continue;
		}

		// The third call should be to a function that is defined (has body) in the module
		auto kernelName = call2.getCallee();
		auto kernelFunc = module.lookupSymbol<mlir::func::FuncOp>(kernelName);
		if (!kernelFunc || kernelFunc.isDeclaration()) {
			continue;
		}

		sites.push_back({call0, call1, call2, kernelFunc});
		i += 2; // Skip past this pattern
	}

	return sites;
}

/// Declare a GPU runtime function as an external function in the MLIR module.
/// Returns the function symbol and records it for JIT proxy function registration.
static mlir::func::FuncOp declareRuntimeFunction(mlir::OpBuilder& builder, mlir::ModuleOp module,
                                                 const std::string& name, mlir::FunctionType fnType) {
	// Check if already declared
	if (auto existing = module.lookupSymbol<mlir::func::FuncOp>(name)) {
		return existing;
	}

	mlir::OpBuilder::InsertionGuard guard(builder);
	builder.setInsertionPointToStart(module.getBody());

	auto funcOp = builder.create<mlir::func::FuncOp>(module.getLoc(), name, fnType);
	funcOp.setPrivate();
	return funcOp;
}

/// Create an LLVM global constant for a byte array (e.g. compiled kernel data).
static void createGlobalBytes(mlir::OpBuilder& builder, mlir::ModuleOp module, const std::string& name,
                              const std::vector<uint8_t>& data) {
	mlir::OpBuilder::InsertionGuard guard(builder);
	builder.setInsertionPointToStart(module.getBody());

	auto i8Type = builder.getI8Type();
	auto arrayType = mlir::LLVM::LLVMArrayType::get(i8Type, data.size());
	auto loc = module.getLoc();

	auto globalOp = builder.create<mlir::LLVM::GlobalOp>(loc, arrayType, /*isConstant=*/true,
	                                                     mlir::LLVM::Linkage::Internal, name, mlir::Attribute());

	// Build the initializer region
	auto& region = globalOp.getInitializerRegion();
	auto* block = builder.createBlock(&region);
	builder.setInsertionPointToStart(block);

	// Create dense elements attribute for the byte data
	auto tensorType = mlir::RankedTensorType::get({static_cast<int64_t>(data.size())}, i8Type);
	auto dataAttr = mlir::DenseElementsAttr::get(tensorType, llvm::ArrayRef(data));
	auto constVal = builder.create<mlir::LLVM::ConstantOp>(loc, arrayType, dataAttr);
	builder.create<mlir::LLVM::ReturnOp>(loc, constVal.getResult());
}

/// Create an LLVM global constant for a null-terminated C string.
static void createGlobalString(mlir::OpBuilder& builder, mlir::ModuleOp module, const std::string& name,
                               const std::string& value) {
	std::vector<uint8_t> bytes(value.begin(), value.end());
	bytes.push_back(0); // null terminator
	createGlobalBytes(builder, module, name, bytes);
}

/// Rewrite a GPU kernel launch site in the host function.
/// Replaces: call @set_grid + call @set_block + call @kernel
/// With: load_module + get_kernel + pack_args + launch + synchronize
static void rewriteLaunchSite(mlir::OpBuilder& builder, mlir::ModuleOp module, LaunchSite& site,
                              const std::vector<uint8_t>& kernelData, int launchIdx,
                              std::vector<std::string>& proxySymbols, std::vector<void*>& proxyAddresses) {
	auto* ctx = builder.getContext();
	auto loc = site.kernelCall.getLoc();
	auto ptrType = mlir::LLVM::LLVMPointerType::get(ctx);
	auto i32Type = builder.getI32Type();
	auto i64Type = builder.getI64Type();

	// Create unique global names for this launch site
	std::string dataGlobalName = "_gpu_kernel_data_" + std::to_string(launchIdx);
	std::string nameGlobalName = "_gpu_kernel_name_" + std::to_string(launchIdx);
	std::string kernelName = site.kernelFunc.getName().str();

	// Create global constants for kernel data and name
	createGlobalBytes(builder, module, dataGlobalName, kernelData);
	createGlobalString(builder, module, nameGlobalName, kernelName);

	// Declare GPU runtime functions
	auto loadModuleFnType = mlir::FunctionType::get(ctx, {ptrType, i64Type}, {ptrType});
	auto getKernelFnType = mlir::FunctionType::get(ctx, {ptrType, ptrType}, {ptrType});
	auto launchFnType = mlir::FunctionType::get(
	    ctx, {ptrType, i32Type, i32Type, i32Type, i32Type, i32Type, i32Type, ptrType, i64Type}, {});
	auto syncFnType = mlir::FunctionType::get(ctx, {}, {});

	auto loadModuleFn = declareRuntimeFunction(builder, module, "_nrt_gpu_load_module", loadModuleFnType);
	auto getKernelFn = declareRuntimeFunction(builder, module, "_nrt_gpu_get_kernel", getKernelFnType);
	auto launchFn = declareRuntimeFunction(builder, module, "_nrt_gpu_launch_kernel", launchFnType);
	auto syncFn = declareRuntimeFunction(builder, module, "_nrt_gpu_synchronize", syncFnType);

	// Register proxy function symbols for JIT linking (only once per function name)
	auto registerProxy = [&](const std::string& symName, void* addr) {
		for (const auto& existing : proxySymbols) {
			if (existing == symName) {
				return;
			}
		}
		proxySymbols.push_back(symName);
		proxyAddresses.push_back(addr);
	};
	registerProxy("_nrt_gpu_load_module", reinterpret_cast<void*>(&runtime::gpuLoadModule));
	registerProxy("_nrt_gpu_get_kernel", reinterpret_cast<void*>(&runtime::gpuGetKernel));
	registerProxy("_nrt_gpu_launch_kernel", reinterpret_cast<void*>(&runtime::gpuLaunchKernel));
	registerProxy("_nrt_gpu_synchronize", reinterpret_cast<void*>(&runtime::gpuSynchronize));

	// Capture operands from the original calls before we erase them
	auto gridX = site.setGridCall.getOperands()[0];
	auto gridY = site.setGridCall.getOperands()[1];
	auto gridZ = site.setGridCall.getOperands()[2];
	auto blockX = site.setBlockCall.getOperands()[0];
	auto blockY = site.setBlockCall.getOperands()[1];
	auto blockZ = site.setBlockCall.getOperands()[2];
	auto kernelArgs = site.kernelCall.getOperands();

	// Position builder right before the set_grid call (replacing the whole sequence)
	builder.setInsertionPoint(site.setGridCall);

	// 1. Get pointer to kernel data global
	auto dataAddr = builder.create<mlir::LLVM::AddressOfOp>(loc, ptrType, dataGlobalName);
	auto dataSizeConst = builder.create<mlir::arith::ConstantOp>(
	    loc, i64Type, builder.getI64IntegerAttr(static_cast<int64_t>(kernelData.size())));

	// 2. Load module
	auto gpuModule = builder.create<mlir::func::CallOp>(loc, loadModuleFn, mlir::ValueRange {dataAddr, dataSizeConst});

	// 3. Get kernel
	auto nameAddr = builder.create<mlir::LLVM::AddressOfOp>(loc, ptrType, nameGlobalName);
	auto gpuKernel =
	    builder.create<mlir::func::CallOp>(loc, getKernelFn, mlir::ValueRange {gpuModule.getResult(0), nameAddr});

	// 4. Pack kernel arguments into void** array
	auto numArgs = kernelArgs.size();
	auto numArgsConst =
	    builder.create<mlir::arith::ConstantOp>(loc, i64Type, builder.getI64IntegerAttr(static_cast<int64_t>(numArgs)));
	auto oneConst = builder.create<mlir::arith::ConstantOp>(loc, i64Type, builder.getI64IntegerAttr(1));

	// Allocate array of pointers: void* args[numArgs]
	auto argsArray = builder.create<mlir::LLVM::AllocaOp>(loc, ptrType, ptrType, numArgsConst);

	// For each kernel argument, allocate space for it and store a pointer in the array
	for (size_t i = 0; i < numArgs; ++i) {
		auto arg = kernelArgs[i];
		auto argType = arg.getType();

		// Allocate space for the argument value
		auto argSlot = builder.create<mlir::LLVM::AllocaOp>(loc, ptrType, argType, oneConst);

		// Store the argument value into the slot
		builder.create<mlir::LLVM::StoreOp>(loc, arg, argSlot);

		// Store the pointer to the slot into args[i]
		auto idxConst =
		    builder.create<mlir::arith::ConstantOp>(loc, i64Type, builder.getI64IntegerAttr(static_cast<int64_t>(i)));
		// Compute the address of args[i] = argsArray + i * sizeof(ptr)
		// Use GEP with !llvm.ptr element type
		auto gepIndices = mlir::ValueRange {idxConst};
		auto slot = builder.create<mlir::LLVM::GEPOp>(loc, ptrType, ptrType, argsArray, gepIndices);
		builder.create<mlir::LLVM::StoreOp>(loc, argSlot, slot);
	}

	// 5. Launch kernel
	auto sharedMemConst = builder.create<mlir::arith::ConstantOp>(loc, i64Type, builder.getI64IntegerAttr(0));
	builder.create<mlir::func::CallOp>(loc, launchFn,
	                                   mlir::ValueRange {gpuKernel.getResult(0), gridX, gridY, gridZ, blockX, blockY,
	                                                     blockZ, argsArray, sharedMemConst});

	// 6. Synchronize
	builder.create<mlir::func::CallOp>(loc, syncFn, mlir::ValueRange {});

	// 7. Erase the original calls (in reverse order to maintain iterator validity)
	site.kernelCall.erase();
	site.setBlockCall.erase();
	site.setGridCall.erase();
}

void MLIRGPUPass::run(mlir::OwningOpRef<mlir::ModuleOp>& module, const compiler::DumpHandler& /*dumpHandler*/,
                      const engine::Options& /*options*/) {
	if (!kernelCompiler_) {
		return; // No GPU backend available -- CPU fallback mode
	}

	auto moduleOp = module.get();
	auto* ctx = moduleOp.getContext();

	// Find the host function (the one with llvm.emit_c_interface attribute, typically named "execute")
	mlir::func::FuncOp hostFunc;
	for (auto funcOp : moduleOp.getOps<mlir::func::FuncOp>()) {
		if (funcOp->hasAttr("llvm.emit_c_interface")) {
			hostFunc = funcOp;
			break;
		}
	}
	if (!hostFunc) {
		return; // No host function found
	}

	// Detect GPU launch patterns
	auto launchSites = findLaunchSites(hostFunc, moduleOp);
	if (launchSites.empty()) {
		return; // No GPU launches detected
	}

	mlir::OpBuilder builder(ctx);

	// Process each launch site
	int launchIdx = 0;
	for (auto& site : launchSites) {
		// Compile the kernel function
		auto kernelData = kernelCompiler_->compile(site.kernelFunc, *ctx);
		if (kernelData.empty()) {
			continue; // Compilation failed, skip this launch (will cause runtime error)
		}

		// Rewrite the host code
		rewriteLaunchSite(builder, moduleOp, site, kernelData, launchIdx, proxySymbols_, proxyAddresses_);
		launchIdx++;
	}

	// Remove extracted kernel functions from the host module
	for (auto& site : launchSites) {
		if (site.kernelFunc && !site.kernelFunc->use_empty()) {
			continue; // Still referenced, don't remove
		}
		if (site.kernelFunc) {
			site.kernelFunc.erase();
		}
	}

	// Clean up GPU stub function declarations that are no longer needed
	std::vector<mlir::func::FuncOp> stubsToRemove;
	for (auto funcOp : moduleOp.getOps<mlir::func::FuncOp>()) {
		if (!funcOp.isDeclaration()) {
			continue;
		}
		auto name = funcOp.getName();
		if (symbolContains(name, "gpu_set_grid") || symbolContains(name, "gpu_set_block") ||
		    symbolContains(name, "gpu_threadIdx") || symbolContains(name, "gpu_blockIdx") ||
		    symbolContains(name, "gpu_blockDim") || symbolContains(name, "gpu_gridDim") ||
		    symbolContains(name, "gpu_syncThreads")) {
			if (funcOp.symbolKnownUseEmpty(moduleOp)) {
				stubsToRemove.push_back(funcOp);
			}
		}
	}
	for (auto func : stubsToRemove) {
		func.erase();
	}
}

} // namespace nautilus::gpu

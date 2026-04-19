
#include "MetalLoweringProvider.hpp"
#include <cassert>
#include <regex>
#include <string>
#include <utility>

namespace nautilus::compiler::metal {

// ============================================================================
// MetalLoweringProvider
// ============================================================================

/// Post-process generated MSL device code to add 'device' address space
/// to pointer casts in load/store operations.
static std::string fixMetalDeviceCode(const std::string& code) {
	std::regex ptrCastRe(R"(\(\((\w+)\*\)\()");
	return std::regex_replace(code, ptrCastRe, "((device $1*)(");
}

MetalLoweringResult MetalLoweringProvider::lower(std::shared_ptr<ir::IRGraph> ir, const engine::Options& options) {
	MetalLoweringResult result;
	// Device code (MSL)
	auto deviceCtx = DeviceContext(ir);
	result.deviceCode = fixMetalDeviceCode(deviceCtx.process().str());
	// Host code (C++ with Metal API)
	auto hostCtx = HostContext(std::move(ir));
	auto bufferSize = options.getOptionOrDefault<int>("gpu.metal.bufferSize", 4096);
	result.hostCode = hostCtx.process(bufferSize).str();
	return result;
}

// ============================================================================
// DeviceContext — generates .metal MSL kernel code
// ============================================================================

MetalLoweringProvider::DeviceContext::DeviceContext(std::shared_ptr<ir::IRGraph> irGraph) {
	ir = std::move(irGraph);
	registerGPUIntrinsics();
}

void MetalLoweringProvider::DeviceContext::registerGPUIntrinsics() {
	auto regDevice = [&](void* ptr, const std::string& expr) {
		gpu::registerNullaryIntrinsic(gpuIntrinsics, ptr, expr, "uint");
		deviceIntrinsics.insert(ptr);
	};
	regDevice(reinterpret_cast<void*>(nautilus_gpu_thread_idx_x), "nautilus_threadIdx.x");
	regDevice(reinterpret_cast<void*>(nautilus_gpu_thread_idx_y), "nautilus_threadIdx.y");
	regDevice(reinterpret_cast<void*>(nautilus_gpu_thread_idx_z), "nautilus_threadIdx.z");
	regDevice(reinterpret_cast<void*>(nautilus_gpu_block_idx_x), "nautilus_blockIdx.x");
	regDevice(reinterpret_cast<void*>(nautilus_gpu_block_idx_y), "nautilus_blockIdx.y");
	regDevice(reinterpret_cast<void*>(nautilus_gpu_block_idx_z), "nautilus_blockIdx.z");
	regDevice(reinterpret_cast<void*>(nautilus_gpu_block_dim_x), "nautilus_blockDim.x");
	regDevice(reinterpret_cast<void*>(nautilus_gpu_block_dim_y), "nautilus_blockDim.y");
	regDevice(reinterpret_cast<void*>(nautilus_gpu_block_dim_z), "nautilus_blockDim.z");
	regDevice(reinterpret_cast<void*>(nautilus_gpu_grid_dim_x), "nautilus_gridDim.x");
	regDevice(reinterpret_cast<void*>(nautilus_gpu_grid_dim_y), "nautilus_gridDim.y");
	regDevice(reinterpret_cast<void*>(nautilus_gpu_grid_dim_z), "nautilus_gridDim.z");
	auto syncPtr = reinterpret_cast<void*>(nautilus_gpu_sync_threads);
	gpu::registerVoidIntrinsic(gpuIntrinsics, syncPtr, "threadgroup_barrier(mem_flags::mem_threadgroup)");
	deviceIntrinsics.insert(syncPtr);

	// Launch config intrinsics are host-side only — not registered here.
}

std::string MetalLoweringProvider::DeviceContext::getType(const Type& stamp) {
	switch (stamp) {
	case Type::v:
		return "void";
	case Type::b:
		return "bool";
	case Type::i8:
		return "char";
	case Type::i16:
		return "short";
	case Type::i32:
		return "int";
	case Type::i64:
		return "long";
	case Type::ui8:
		return "uchar";
	case Type::ui16:
		return "ushort";
	case Type::ui32:
		return "uint";
	case Type::ui64:
		return "ulong";
	case Type::f32:
		return "float";
	case Type::f64:
		return "float"; // Metal lacks native double
	case Type::ptr:
		return "device uchar*";
	}
	assert(false);
	return "unknown";
}

MetalLoweringProvider::DeviceContext::Code MetalLoweringProvider::DeviceContext::process() {
	Code code;
	code << "#include <metal_stdlib>\nusing namespace metal;\n\n";

	const auto& functionOperations = ir->getFunctionOperations();
	classifyKernelFunctions();

	auto emitFunction = [&](const ir::FunctionOperation& func) {
		resetFunctionState();
		gpu::RegisterFrame rootFrame;
		const auto& functionBasicBlock = func.getFunctionBasicBlock();

		bool isKernel = kernelFunctions.contains(func.getName());
		std::vector<std::string> arguments;
		for (auto i = 0ull; i < functionBasicBlock.getArguments().size(); i++) {
			auto argument = functionBasicBlock.getArguments()[i];
			auto var = getVariable(argument->getIdentifier());
			rootFrame.setValue(argument->getIdentifier(), var);
			if (isKernel) {
				if (argument->getStamp() == Type::ptr) {
					arguments.emplace_back("device uchar* " + var + " [[buffer(" + std::to_string(i) + ")]]");
				} else {
					// Scalar args must be passed as constant references in Metal kernels
					arguments.emplace_back("constant " + getType(argument->getStamp()) + "& " + var + " [[buffer(" +
					                       std::to_string(i) + ")]]");
				}
			} else {
				arguments.emplace_back(getType(argument->getStamp()) + " " + var);
			}
		}
		processBlock(&functionBasicBlock, rootFrame);

		if (isKernel) {
			code << "kernel void " << func.getName() << "(\n";
			for (size_t i = 0; i < arguments.size(); i++) {
				code << "    " << arguments[i] << ",\n";
			}
			code << "    uint3 nautilus_threadIdx [[thread_position_in_threadgroup]],\n";
			code << "    uint3 nautilus_blockIdx [[threadgroup_position_in_grid]],\n";
			code << "    uint3 nautilus_blockDim [[threads_per_threadgroup]],\n";
			code << "    uint3 nautilus_gridDim [[threadgroups_per_grid]]\n";
		} else {
			code << getType(func.getOutputArg()) << " " << func.getName() << "(";
			for (size_t i = 0; i < arguments.size(); i++) {
				if (i != 0)
					code << ",";
				code << arguments[i] << " ";
			}
		}
		code << ") {\n" << blockArguments.str();
		if (blocks.size() > 1) {
			// Multi-block: emit while(true)/switch dispatch for Metal (no goto/labels).
			code << "int __pc = 0;\nwhile (true) {\nswitch (__pc) {\n";
			for (size_t bi = 0; bi < blocks.size(); bi++) {
				auto blockStr = blocks[bi].str();
				// Strip the "Block_X:\n" label from the beginning of each block
				auto nlPos = blockStr.find('\n');
				if (nlPos != std::string::npos) {
					blockStr = blockStr.substr(nlPos + 1);
				}
				// Replace "goto Block_X;" with "__pc = X; continue;"
				// We use continue (not break) to restart the while loop.
				std::regex gotoRe(R"(goto (Block_\w+);)");
				std::smatch m;
				std::string transformed;
				std::string remaining = blockStr;
				while (std::regex_search(remaining, m, gotoRe)) {
					transformed += m.prefix().str();
					// Extract the block index from the label name
					auto targetLabel = m[1].str();
					// Find which case index this label corresponds to
					for (size_t ti = 0; ti < blocks.size(); ti++) {
						auto blockHead = blocks[ti].str();
						if (blockHead.find(targetLabel + ":\n") == 0) {
							transformed += "__pc = " + std::to_string(ti) + "; continue;";
							break;
						}
					}
					remaining = m.suffix().str();
				}
				transformed += remaining;
				code << "case " << bi << ": {\n" << transformed << "}\n";
			}
			code << "}\n}\n"; // close switch + while
		} else {
			// Single block: just strip the label
			for (auto& block : blocks) {
				auto blockStr = block.str();
				auto nlPos = blockStr.find('\n');
				if (nlPos != std::string::npos) {
					blockStr = blockStr.substr(nlPos + 1);
				}
				code << blockStr << "\n";
			}
		}
		code << "}\n\n";
	};

	// Emit non-root functions that are kernels (use device intrinsics)
	for (size_t i = 1; i < functionOperations.size(); i++) {
		if (kernelFunctions.contains(functionOperations[i]->getName())) {
			emitFunction(*functionOperations[i]);
		}
	}
	// Emit root only if it's a kernel
	if (!functionOperations.empty()) {
		const auto& rootFunc = *functionOperations[0];
		if (kernelFunctions.contains(rootFunc.getName())) {
			emitFunction(rootFunc);
		}
	}

	return code;
}

void MetalLoweringProvider::DeviceContext::processOperation(ir::Operation* opt, short blockIndex,
                                                            gpu::RegisterFrame& frame) {
	dispatchOperation(opt, blockIndex, frame);
}

void MetalLoweringProvider::DeviceContext::processGPUOperation(ir::Operation* opt,
                                                               short blockIndex, gpu::RegisterFrame& frame) {
	using OT = ir::Operation::OperationType;
	if (opt->getOperationType() == OT::ProxyCallOp) {
		processProxyCall(ir::as<ir::ProxyCallOperation>(opt), blockIndex, frame);
		return;
	}
	throw NotImplementedException("Operation is not implemented for Metal device backend");
}

void MetalLoweringProvider::DeviceContext::processProxyCall(ir::ProxyCallOperation* opt, short blockIndex,
                                                            gpu::RegisterFrame& frame) {
	auto it = gpuIntrinsics.find(opt->getFunctionPtr());
	if (it != gpuIntrinsics.end()) {
		if (it->second(opt, blockIndex, frame, blockArguments, blocks, getVariable)) {
			return;
		}
	}
	// Internal function call (device helper)
	bool isInternalFunction = ir->getFunctionOperation(opt->getFunctionName()) != nullptr;
	if (isInternalFunction) {
		std::stringstream args;
		for (size_t i = 0; i < opt->getInputArguments().size(); i++) {
			if (i != 0)
				args << ",";
			args << frame.getValue(opt->getInputArguments()[i]->getIdentifier());
		}
		if (opt->getStamp() != Type::v) {
			auto resultVar = getVariable(opt->getIdentifier());
			if (!frame.contains(opt->getIdentifier())) {
				blockArguments << getType(opt->getStamp()) << " " << resultVar << ";\n";
				frame.setValue(opt->getIdentifier(), resultVar);
			}
			blocks[blockIndex] << resultVar << " = ";
		}
		blocks[blockIndex] << opt->getFunctionName() << "(" << args.str() << ");\n";
		return;
	}
	throw NotImplementedException("External function calls are not supported in Metal kernels: " +
	                              opt->getFunctionName());
}

// ============================================================================
// HostContext — generates C++ host code with Metal API dispatch
// ============================================================================

MetalLoweringProvider::HostContext::HostContext(std::shared_ptr<ir::IRGraph> irGraph) {
	ir = std::move(irGraph);
	registerHostIntrinsics();
}

void MetalLoweringProvider::HostContext::registerHostIntrinsics() {
	// Device intrinsics classification (same as DeviceContext)
	deviceIntrinsics.insert(reinterpret_cast<void*>(nautilus_gpu_thread_idx_x));
	deviceIntrinsics.insert(reinterpret_cast<void*>(nautilus_gpu_thread_idx_y));
	deviceIntrinsics.insert(reinterpret_cast<void*>(nautilus_gpu_thread_idx_z));
	deviceIntrinsics.insert(reinterpret_cast<void*>(nautilus_gpu_block_idx_x));
	deviceIntrinsics.insert(reinterpret_cast<void*>(nautilus_gpu_block_idx_y));
	deviceIntrinsics.insert(reinterpret_cast<void*>(nautilus_gpu_block_idx_z));
	deviceIntrinsics.insert(reinterpret_cast<void*>(nautilus_gpu_block_dim_x));
	deviceIntrinsics.insert(reinterpret_cast<void*>(nautilus_gpu_block_dim_y));
	deviceIntrinsics.insert(reinterpret_cast<void*>(nautilus_gpu_block_dim_z));
	deviceIntrinsics.insert(reinterpret_cast<void*>(nautilus_gpu_grid_dim_x));
	deviceIntrinsics.insert(reinterpret_cast<void*>(nautilus_gpu_grid_dim_y));
	deviceIntrinsics.insert(reinterpret_cast<void*>(nautilus_gpu_grid_dim_z));
	deviceIntrinsics.insert(reinterpret_cast<void*>(nautilus_gpu_sync_threads));

	// Launch config intrinsics: capture grid/block variable names
	gpu::registerLaunchConfigIntrinsic(gpuIntrinsics, reinterpret_cast<void*>(nautilus_gpu_set_grid), pendingGridX,
	                                   pendingGridY, pendingGridZ, hasLaunchConfig);
	gpu::registerLaunchConfigIntrinsic(gpuIntrinsics, reinterpret_cast<void*>(nautilus_gpu_set_block), pendingBlockX,
	                                   pendingBlockY, pendingBlockZ, hasLaunchConfig);
}

std::string MetalLoweringProvider::HostContext::getType(const Type& stamp) {
	// Host code uses standard C++ types
	switch (stamp) {
	case Type::v:
		return "void";
	case Type::b:
		return "bool";
	case Type::i8:
		return "int8_t";
	case Type::i16:
		return "int16_t";
	case Type::i32:
		return "int32_t";
	case Type::i64:
		return "int64_t";
	case Type::ui8:
		return "uint8_t";
	case Type::ui16:
		return "uint16_t";
	case Type::ui32:
		return "uint32_t";
	case Type::ui64:
		return "uint64_t";
	case Type::f32:
		return "float";
	case Type::f64:
		return "double";
	case Type::ptr:
		return "uint8_t*";
	}
	assert(false);
	return "unknown";
}

MetalLoweringProvider::HostContext::Code MetalLoweringProvider::HostContext::process(int bufferSize) {
	Code code;
	code << "#include <cstdint>\n";
	code << "#include <cstring>\n";
	code << "#include <Metal/Metal.h>\n\n";
	code << "#define NAUTILUS_BUFFER_SIZE " << bufferSize << "\n\n";

	const auto& functionOperations = ir->getFunctionOperations();
	classifyKernelFunctions();

	if (functionOperations.empty()) {
		return code;
	}

	// Find the host function (non-kernel). It may not be at index 0 if the
	// function order varies depending on the tracing/IR construction.
	const ir::FunctionOperation* hostFunc = nullptr;
	for (const auto& func : functionOperations) {
		if (!kernelFunctions.contains(func->getName())) {
			hostFunc = func;
			break;
		}
	}
	// If all functions are kernels, host dispatch is fully external.
	if (!hostFunc) {
		return code;
	}
	const auto& rootFunc = *hostFunc;

	// Generate the host function body
	resetFunctionState();
	gpu::RegisterFrame rootFrame;
	std::vector<std::string> arguments;
	const auto& functionBasicBlock = rootFunc.getFunctionBasicBlock();
	for (auto i = 0ull; i < functionBasicBlock.getArguments().size(); i++) {
		auto argument = functionBasicBlock.getArguments()[i];
		auto var = getVariable(argument->getIdentifier());
		rootFrame.setValue(argument->getIdentifier(), var);
		arguments.emplace_back(getType(argument->getStamp()) + " " + var);
	}
	processBlock(&functionBasicBlock, rootFrame);

	auto retType = getType(rootFunc.getOutputArg());
	code << "extern \"C\" " << retType << " execute(";
	for (size_t i = 0; i < arguments.size(); i++) {
		if (i != 0)
			code << ",";
		code << arguments[i] << " ";
	}
	code << "){\n";
	code << blockArguments.str();
	if (!functions.str().empty())
		code << functions.str();
	for (auto& block : blocks)
		code << block.str() << "\n";
	code << "}\n";

	return code;
}

void MetalLoweringProvider::HostContext::processOperation(ir::Operation* opt, short blockIndex,
                                                          gpu::RegisterFrame& frame) {
	dispatchOperation(opt, blockIndex, frame);
}

void MetalLoweringProvider::HostContext::processGPUOperation(ir::Operation* opt,
                                                             short blockIndex, gpu::RegisterFrame& frame) {
	using OT = ir::Operation::OperationType;
	if (opt->getOperationType() == OT::ProxyCallOp) {
		processProxyCall(ir::as<ir::ProxyCallOperation>(opt), blockIndex, frame);
		return;
	}
	throw NotImplementedException("Operation is not implemented for Metal host backend");
}

void MetalLoweringProvider::HostContext::processProxyCall(ir::ProxyCallOperation* opt, short blockIndex,
                                                          gpu::RegisterFrame& frame) {
	// Check launch config intrinsics (setGrid/setBlock)
	auto it = gpuIntrinsics.find(opt->getFunctionPtr());
	if (it != gpuIntrinsics.end()) {
		if (it->second(opt, blockIndex, frame, blockArguments, blocks, getVariable)) {
			return;
		}
	}

	// Collect arguments
	std::stringstream args;
	for (size_t i = 0; i < opt->getInputArguments().size(); i++) {
		if (i != 0)
			args << ",";
		args << frame.getValue(opt->getInputArguments()[i]->getIdentifier());
	}

	bool isInternalFunction = ir->getFunctionOperation(opt->getFunctionName()) != nullptr;
	bool isKernelCall = isInternalFunction && kernelFunctions.contains(opt->getFunctionName());

	if (isKernelCall) {
		// Emit Metal API kernel dispatch
		auto kernelName = opt->getFunctionName();
		auto argList = opt->getInputArguments();

		blocks[blockIndex] << "// Metal kernel dispatch: " << kernelName << "\n";
		blocks[blockIndex] << "{\n";
		blocks[blockIndex] << "    id<MTLDevice> device = MTLCreateSystemDefaultDevice();\n";
		blocks[blockIndex] << "    NSError* error = nil;\n";
		blocks[blockIndex] << "    NSURL* libURL = [NSURL fileURLWithPath:@\"__METALLIB_PATH__\"];\n";
		blocks[blockIndex] << "    id<MTLLibrary> library = [device newLibraryWithURL:libURL error:&error];\n";
		blocks[blockIndex] << "    id<MTLFunction> kernelFunc = [library newFunctionWithName:@\"" << kernelName
		                   << "\"];\n";
		blocks[blockIndex]
		    << "    id<MTLComputePipelineState> pipeline = [device newComputePipelineStateWithFunction:kernelFunc "
		       "error:&error];\n";
		blocks[blockIndex] << "    id<MTLCommandQueue> queue = [device newCommandQueue];\n";
		blocks[blockIndex] << "    id<MTLCommandBuffer> cmdBuf = [queue commandBuffer];\n";
		blocks[blockIndex] << "    id<MTLComputeCommandEncoder> encoder = [cmdBuf computeCommandEncoder];\n";
		blocks[blockIndex] << "    [encoder setComputePipelineState:pipeline];\n";

		// Track pointer buffer indices for copy-back after execution
		std::vector<std::pair<size_t, std::string>> ptrBuffers;

		// Set buffer arguments
		for (size_t i = 0; i < argList.size(); i++) {
			auto argVar = frame.getValue(argList[i]->getIdentifier());
			auto argStamp = argList[i]->getStamp();
			if (argStamp == Type::ptr) {
				blocks[blockIndex] << "    id<MTLBuffer> buf_" << i << " = [device newBufferWithBytes:(void*)" << argVar
				                   << " length:NAUTILUS_BUFFER_SIZE options:MTLResourceStorageModeShared];\n";
				blocks[blockIndex] << "    [encoder setBuffer:buf_" << i << " offset:0 atIndex:" << i << "];\n";
				ptrBuffers.emplace_back(i, argVar);
			} else {
				blocks[blockIndex] << "    [encoder setBytes:&" << argVar << " length:sizeof(" << getType(argStamp)
				                   << ") atIndex:" << i << "];\n";
			}
		}

		// Dispatch with grid/block dims
		if (hasLaunchConfig) {
			blocks[blockIndex] << "    MTLSize grid = MTLSizeMake(" << pendingGridX << "," << pendingGridY << ","
			                   << pendingGridZ << ");\n";
			blocks[blockIndex] << "    MTLSize block = MTLSizeMake(" << pendingBlockX << "," << pendingBlockY << ","
			                   << pendingBlockZ << ");\n";
		} else {
			blocks[blockIndex] << "    MTLSize grid = MTLSizeMake(1,1,1);\n";
			blocks[blockIndex] << "    MTLSize block = MTLSizeMake(256,1,1);\n";
		}
		blocks[blockIndex] << "    [encoder dispatchThreadgroups:grid threadsPerThreadgroup:block];\n";
		blocks[blockIndex] << "    [encoder endEncoding];\n";
		blocks[blockIndex] << "    [cmdBuf commit];\n";
		blocks[blockIndex] << "    [cmdBuf waitUntilCompleted];\n";

		// Copy back results from GPU buffers to host memory
		for (auto& [bufIdx, argVar] : ptrBuffers) {
			blocks[blockIndex] << "    memcpy((void*)" << argVar << ", [buf_" << bufIdx
			                   << " contents], NAUTILUS_BUFFER_SIZE);\n";
		}

		blocks[blockIndex] << "}\n";
		hasLaunchConfig = false;
		return;
	}

	// Regular internal function call
	if (isInternalFunction) {
		if (opt->getStamp() != Type::v) {
			auto resultVar = getVariable(opt->getIdentifier());
			if (!frame.contains(opt->getIdentifier())) {
				blockArguments << getType(opt->getStamp()) << " " << resultVar << ";\n";
				frame.setValue(opt->getIdentifier(), resultVar);
			}
			blocks[blockIndex] << resultVar << " = ";
		}
		blocks[blockIndex] << opt->getFunctionName() << "(" << args.str() << ");\n";
		return;
	}

	// External function call via function pointer
	auto returnTypeStr = getType(opt->getStamp());
	std::stringstream argTypes;
	for (size_t i = 0; i < opt->getInputArguments().size(); i++) {
		if (i != 0)
			argTypes << ",";
		argTypes << getType(opt->getInputArguments()[i]->getStamp());
	}
	if (!functionNames.contains(opt->getFunctionSymbol())) {
		functions << "auto f_" << opt->getFunctionSymbol() << " = (" << returnTypeStr << "(*)(" << argTypes.str()
		          << "))" << opt->getFunctionPtr() << ";\n";
		functionNames.emplace(opt->getFunctionSymbol());
	}
	if (opt->getStamp() != Type::v) {
		auto resultVar = getVariable(opt->getIdentifier());
		if (!frame.contains(opt->getIdentifier())) {
			blockArguments << getType(opt->getStamp()) << " " << resultVar << ";\n";
			frame.setValue(opt->getIdentifier(), resultVar);
		}
		blocks[blockIndex] << resultVar << " = ";
	}
	blocks[blockIndex] << "f_" << opt->getFunctionSymbol() << "(" << args.str() << ");\n";
}

} // namespace nautilus::compiler::metal

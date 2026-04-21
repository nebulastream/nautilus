
#include "CUDALoweringProvider.hpp"
#include <cassert>
#include <string>
#include <utility>

namespace nautilus::compiler::cuda {

CUDALoweringProvider::LoweringContext::LoweringContext(std::shared_ptr<ir::IRGraph> irGraph,
                                                       const engine::Options& options)
    : options(options) {
	ir = std::move(irGraph);
	registerGPUIntrinsics();
}

void CUDALoweringProvider::LoweringContext::registerGPUIntrinsics() {
	// Device intrinsics: these mark a function as a GPU kernel
	auto regDevice = [&](void* ptr, const std::string& expr) {
		gpu::registerNullaryIntrinsic(gpuIntrinsics, ptr, expr);
		deviceIntrinsics.insert(ptr);
	};
	regDevice(reinterpret_cast<void*>(nautilus_gpu_thread_idx_x), "threadIdx.x");
	regDevice(reinterpret_cast<void*>(nautilus_gpu_thread_idx_y), "threadIdx.y");
	regDevice(reinterpret_cast<void*>(nautilus_gpu_thread_idx_z), "threadIdx.z");
	regDevice(reinterpret_cast<void*>(nautilus_gpu_block_idx_x), "blockIdx.x");
	regDevice(reinterpret_cast<void*>(nautilus_gpu_block_idx_y), "blockIdx.y");
	regDevice(reinterpret_cast<void*>(nautilus_gpu_block_idx_z), "blockIdx.z");
	regDevice(reinterpret_cast<void*>(nautilus_gpu_block_dim_x), "blockDim.x");
	regDevice(reinterpret_cast<void*>(nautilus_gpu_block_dim_y), "blockDim.y");
	regDevice(reinterpret_cast<void*>(nautilus_gpu_block_dim_z), "blockDim.z");
	regDevice(reinterpret_cast<void*>(nautilus_gpu_grid_dim_x), "gridDim.x");
	regDevice(reinterpret_cast<void*>(nautilus_gpu_grid_dim_y), "gridDim.y");
	regDevice(reinterpret_cast<void*>(nautilus_gpu_grid_dim_z), "gridDim.z");
	auto syncPtr = reinterpret_cast<void*>(nautilus_gpu_sync_threads);
	gpu::registerVoidIntrinsic(gpuIntrinsics, syncPtr, "__syncthreads()");
	deviceIntrinsics.insert(syncPtr);

	// Launch config intrinsics: host-side, do NOT mark function as kernel
	gpu::registerLaunchConfigIntrinsic(gpuIntrinsics, reinterpret_cast<void*>(nautilus_gpu_set_grid), pendingGridX,
	                                   pendingGridY, pendingGridZ, hasLaunchConfig);
	gpu::registerLaunchConfigIntrinsic(gpuIntrinsics, reinterpret_cast<void*>(nautilus_gpu_set_block), pendingBlockX,
	                                   pendingBlockY, pendingBlockZ, hasLaunchConfig);
}

std::string CUDALoweringProvider::lower(std::shared_ptr<ir::IRGraph> ir, const engine::Options& options) {
	auto ctx = LoweringContext(std::move(ir), options);
	return ctx.process().str();
}

std::string CUDALoweringProvider::LoweringContext::getType(const Type& stamp) {
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

CUDALoweringProvider::LoweringContext::Code CUDALoweringProvider::LoweringContext::process() {
	Code pipelineCode;
	pipelineCode << "\n#include <cstdint>\n#include <cuda_runtime.h>\n\n";

	const auto& functionOperations = ir->getFunctionOperations();
	auto gridX = options.getOptionOrDefault<int>("gpu.gridDimX", 1);
	auto gridY = options.getOptionOrDefault<int>("gpu.gridDimY", 1);
	auto gridZ = options.getOptionOrDefault<int>("gpu.gridDimZ", 1);
	auto blockX = options.getOptionOrDefault<int>("gpu.blockDimX", 256);
	auto blockY = options.getOptionOrDefault<int>("gpu.blockDimY", 1);
	auto blockZ = options.getOptionOrDefault<int>("gpu.blockDimZ", 1);

	classifyKernelFunctions();

	auto emitFunction = [&](const ir::FunctionOperation& func, const std::string& nameOverride = "") {
		resetFunctionState();
		gpu::RegisterFrame rootFrame;
		std::vector<std::string> arguments;
		const auto& functionBasicBlock = func.getFunctionBasicBlock();

		for (auto i = 0ull; i < functionBasicBlock.getArguments().size(); i++) {
			auto argument = functionBasicBlock.getArguments()[i];
			auto var = getVariable(argument->getIdentifier());
			rootFrame.setValue(argument->getIdentifier(), var);
			arguments.emplace_back(getType(argument->getStamp()) + " " + var);
		}
		processBlock(&functionBasicBlock, rootFrame);

		bool isKernel = kernelFunctions.contains(func.getName());
		auto funcName = nameOverride.empty() ? func.getName() : nameOverride;
		if (isKernel) {
			pipelineCode << "__global__ void " << funcName << "(";
		} else {
			pipelineCode << "__device__ " << getType(func.getOutputArg()) << " " << funcName << "(";
		}
		for (size_t i = 0; i < arguments.size(); i++) {
			if (i != 0)
				pipelineCode << ",";
			pipelineCode << arguments[i] << " ";
		}
		pipelineCode << "){\n" << blockArguments.str();
		if (!functions.str().empty())
			pipelineCode << functions.str();
		for (auto& block : blocks)
			pipelineCode << block.str() << "\n";
		pipelineCode << "}\n\n";
	};

	// Emit non-root functions
	for (size_t i = 1; i < functionOperations.size(); i++) {
		emitFunction(*functionOperations[i]);
	}

	// Emit root function
	if (!functionOperations.empty()) {
		const auto& rootFunc = *functionOperations[0];
		bool rootIsKernel = kernelFunctions.contains(rootFunc.getName());

		if (rootIsKernel) {
			auto kernelName = rootFunc.getName() + "_kernel";
			emitFunction(rootFunc, kernelName);

			const auto& rootBlock = rootFunc.getFunctionBasicBlock();
			pipelineCode << "extern \"C\" void execute(";
			for (size_t i = 0; i < rootBlock.getArguments().size(); i++) {
				if (i != 0)
					pipelineCode << ",";
				pipelineCode << getType(rootBlock.getArguments()[i]->getStamp()) << " arg_" << i;
			}
			pipelineCode << "){\n";
			pipelineCode << "    dim3 grid(" << gridX << "," << gridY << "," << gridZ << ");\n";
			pipelineCode << "    dim3 block(" << blockX << "," << blockY << "," << blockZ << ");\n";
			pipelineCode << "    " << kernelName << "<<<grid, block>>>(";
			for (size_t i = 0; i < rootBlock.getArguments().size(); i++) {
				if (i != 0)
					pipelineCode << ",";
				pipelineCode << "arg_" << i;
			}
			pipelineCode << ");\n    cudaDeviceSynchronize();\n}\n";
		} else {
			// Root is host code
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

			pipelineCode << "extern \"C\" " << getType(rootFunc.getOutputArg()) << " execute(";
			for (size_t i = 0; i < arguments.size(); i++) {
				if (i != 0)
					pipelineCode << ",";
				pipelineCode << arguments[i] << " ";
			}
			pipelineCode << "){\n" << blockArguments.str();
			if (!functions.str().empty())
				pipelineCode << functions.str();
			for (auto& block : blocks)
				pipelineCode << block.str() << "\n";
			pipelineCode << "}\n";
		}
	}
	return pipelineCode;
}

void CUDALoweringProvider::LoweringContext::processOperation(ir::Operation* opt,
                                                             short blockIndex, gpu::RegisterFrame& frame) {
	dispatchOperation(opt, blockIndex, frame);
}

void CUDALoweringProvider::LoweringContext::processGPUOperation(ir::Operation* opt,
                                                                short blockIndex, gpu::RegisterFrame& frame) {
	using OT = ir::Operation::OperationType;
	switch (opt->getOperationType()) {
	case OT::ProxyCallOp:
		processProxyCall(ir::as<ir::ProxyCallOperation>(opt), blockIndex, frame);
		return;
	case OT::IndirectCallOp:
		processIndirectCall(ir::as<ir::IndirectCallOperation>(opt), blockIndex, frame);
		return;
	case OT::FunctionAddressOfOp:
		processFunctionAddressOf(ir::as<ir::FunctionAddressOfOperation>(opt), blockIndex, frame);
		return;
	default:
		throw NotImplementedException("Operation is not implemented for CUDA backend");
	}
}

void CUDALoweringProvider::LoweringContext::processProxyCall(ir::ProxyCallOperation* opt, short blockIndex,
                                                             gpu::RegisterFrame& frame) {
	// Check GPU intrinsics
	auto it = gpuIntrinsics.find(opt->getFunctionPtr());
	if (it != gpuIntrinsics.end()) {
		if (it->second(opt, blockIndex, frame, blockArguments, blocks, getVariable)) {
			return;
		}
	}

	// Collect arguments
	std::stringstream argTypes, args;
	for (size_t i = 0; i < opt->getInputArguments().size(); i++) {
		if (i != 0) {
			argTypes << ",";
			args << ",";
		}
		args << frame.getValue(opt->getInputArguments()[i]->getIdentifier());
		argTypes << getType(opt->getInputArguments()[i]->getStamp());
	}

	bool isInternalFunction = ir->getFunctionOperation(opt->getFunctionName()) != nullptr;
	bool isKernelCall = isInternalFunction && kernelFunctions.contains(opt->getFunctionName());

	if (isKernelCall) {
		if (hasLaunchConfig) {
			// Use dynamic launch config from setGrid/setBlock intrinsics
			blocks[blockIndex] << opt->getFunctionName() << "<<<dim3(" << pendingGridX << "," << pendingGridY << ","
			                   << pendingGridZ << "),dim3(" << pendingBlockX << "," << pendingBlockY << ","
			                   << pendingBlockZ << ")>>>(" << args.str() << ");\n";
		} else {
			// Fallback: use static config from engine::Options
			auto gx = options.getOptionOrDefault<int>("gpu.gridDimX", 1);
			auto gy = options.getOptionOrDefault<int>("gpu.gridDimY", 1);
			auto gz = options.getOptionOrDefault<int>("gpu.gridDimZ", 1);
			auto bx = options.getOptionOrDefault<int>("gpu.blockDimX", 256);
			auto by = options.getOptionOrDefault<int>("gpu.blockDimY", 1);
			auto bz = options.getOptionOrDefault<int>("gpu.blockDimZ", 1);
			blocks[blockIndex] << opt->getFunctionName() << "<<<dim3(" << gx << "," << gy << "," << gz << "),dim3("
			                   << bx << "," << by << "," << bz << ")>>>(" << args.str() << ");\n";
		}
		blocks[blockIndex] << "cudaDeviceSynchronize();\n";
		hasLaunchConfig = false; // consumed
		return;
	}

	auto returnTypeStr = getType(opt->getStamp());
	if (!isInternalFunction && !functionNames.contains(opt->getFunctionSymbol())) {
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
	if (isInternalFunction) {
		blocks[blockIndex] << opt->getFunctionName() << "(" << args.str() << ");\n";
	} else {
		blocks[blockIndex] << "f_" << opt->getFunctionSymbol() << "(" << args.str() << ");\n";
	}
}

void CUDALoweringProvider::LoweringContext::processIndirectCall(ir::IndirectCallOperation* opt, short blockIndex,
                                                                gpu::RegisterFrame& frame) {
	auto returnTypeStr = getType(opt->getStamp());
	std::stringstream argTypes, args;
	auto inputArguments = opt->getInputArguments();
	for (size_t i = 0; i < inputArguments.size(); i++) {
		if (i != 0) {
			argTypes << ",";
			args << ",";
		}
		args << frame.getValue(inputArguments[i]->getIdentifier());
		argTypes << getType(inputArguments[i]->getStamp());
	}
	auto fnPtrVar = frame.getValue(opt->getFunctionPtrOperand()->getIdentifier());
	if (opt->getStamp() != Type::v) {
		auto resultVar = getVariable(opt->getIdentifier());
		if (!frame.contains(opt->getIdentifier())) {
			blockArguments << getType(opt->getStamp()) << " " << resultVar << ";\n";
			frame.setValue(opt->getIdentifier(), resultVar);
		}
		blocks[blockIndex] << resultVar << " = ((" << returnTypeStr << "(*)(" << argTypes.str() << "))" << fnPtrVar
		                   << ")(" << args.str() << ");\n";
	} else {
		blocks[blockIndex] << "((" << returnTypeStr << "(*)(" << argTypes.str() << "))" << fnPtrVar << ")("
		                   << args.str() << ");\n";
	}
}

void CUDALoweringProvider::LoweringContext::processFunctionAddressOf(ir::FunctionAddressOfOperation* funcAddrOp,
                                                                     short blockIndex, gpu::RegisterFrame& frame) {
	auto resultVar = getVariable(funcAddrOp->getIdentifier());
	if (!frame.contains(funcAddrOp->getIdentifier())) {
		blockArguments << "uint8_t* " << resultVar << ";\n";
		frame.setValue(funcAddrOp->getIdentifier(), resultVar);
	}
	bool isInternalFunction = ir->getFunctionOperation(funcAddrOp->getFunctionName()) != nullptr;
	if (isInternalFunction) {
		blocks[blockIndex] << resultVar << " = (uint8_t*)&" << funcAddrOp->getFunctionName() << ";\n";
	} else {
		if (!functionNames.contains(funcAddrOp->getFunctionSymbol())) {
			functions << "auto f_" << funcAddrOp->getFunctionSymbol() << " = (void*)" << funcAddrOp->getFunctionPtr()
			          << ";\n";
			functionNames.emplace(funcAddrOp->getFunctionSymbol());
		}
		blocks[blockIndex] << resultVar << " = (uint8_t*)f_" << funcAddrOp->getFunctionSymbol() << ";\n";
	}
}

} // namespace nautilus::compiler::cuda

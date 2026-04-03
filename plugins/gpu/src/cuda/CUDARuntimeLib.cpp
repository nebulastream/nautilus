
#include "../GPURuntimeLib.hpp"
#include <cuda.h>
#include <mutex>
#include <stdexcept>
#include <string>

namespace nautilus::gpu::runtime {

/// Singleton managing CUDA context and device.
struct CUDAContext {
	CUdevice device;
	CUcontext context;

	static CUDAContext& instance() {
		static CUDAContext ctx;
		return ctx;
	}

private:
	CUDAContext() {
		CUresult err = cuInit(0);
		if (err != CUDA_SUCCESS) {
			throw std::runtime_error("CUDA: cuInit failed with error " + std::to_string(err));
		}
		err = cuDeviceGet(&device, 0);
		if (err != CUDA_SUCCESS) {
			throw std::runtime_error("CUDA: cuDeviceGet failed with error " + std::to_string(err));
		}
		err = cuCtxCreate(&context, 0, device);
		if (err != CUDA_SUCCESS) {
			throw std::runtime_error("CUDA: cuCtxCreate failed with error " + std::to_string(err));
		}
	}
};

static const char* getCUDAErrorString(CUresult err) {
	const char* str = nullptr;
	cuGetErrorString(err, &str);
	return str ? str : "unknown error";
}

void* gpuLoadModule(const void* kernelData, size_t kernelSize) {
	(void) kernelSize; // PTX is null-terminated, size not needed by cuModuleLoadData
	auto& ctx = CUDAContext::instance();
	(void) ctx; // Ensure CUDA context is initialized

	CUmodule module;
	CUresult err = cuModuleLoadData(&module, kernelData);
	if (err != CUDA_SUCCESS) {
		throw std::runtime_error(std::string("CUDA: cuModuleLoadData failed: ") + getCUDAErrorString(err));
	}
	return reinterpret_cast<void*>(module);
}

void* gpuGetKernel(void* module, const char* kernelName) {
	CUfunction function;
	CUresult err = cuModuleGetFunction(&function, reinterpret_cast<CUmodule>(module), kernelName);
	if (err != CUDA_SUCCESS) {
		throw std::runtime_error(std::string("CUDA: cuModuleGetFunction failed for '") + kernelName +
		                         "': " + getCUDAErrorString(err));
	}
	return reinterpret_cast<void*>(function);
}

void gpuLaunchKernel(void* kernel, uint32_t gridX, uint32_t gridY, uint32_t gridZ, uint32_t blockX, uint32_t blockY,
                     uint32_t blockZ, void** args, size_t sharedMem) {
	CUresult err = cuLaunchKernel(reinterpret_cast<CUfunction>(kernel), gridX, gridY, gridZ, blockX, blockY, blockZ,
	                              static_cast<unsigned int>(sharedMem), /*stream=*/nullptr, args, /*extra=*/nullptr);
	if (err != CUDA_SUCCESS) {
		throw std::runtime_error(std::string("CUDA: cuLaunchKernel failed: ") + getCUDAErrorString(err));
	}
}

void gpuSynchronize() {
	CUresult err = cuCtxSynchronize();
	if (err != CUDA_SUCCESS) {
		throw std::runtime_error(std::string("CUDA: cuCtxSynchronize failed: ") + getCUDAErrorString(err));
	}
}

void* gpuMalloc(size_t size) {
	CUDAContext::instance(); // Ensure context
	CUdeviceptr ptr;
	CUresult err = cuMemAlloc(&ptr, size);
	if (err != CUDA_SUCCESS) {
		throw std::runtime_error(std::string("CUDA: cuMemAlloc failed: ") + getCUDAErrorString(err));
	}
	return reinterpret_cast<void*>(ptr);
}

void gpuFree(void* ptr) {
	if (ptr) {
		CUresult err = cuMemFree(reinterpret_cast<CUdeviceptr>(ptr));
		if (err != CUDA_SUCCESS) {
			throw std::runtime_error(std::string("CUDA: cuMemFree failed: ") + getCUDAErrorString(err));
		}
	}
}

void gpuMemcpyHtoD(void* dst, const void* src, size_t size) {
	CUresult err = cuMemcpyHtoD(reinterpret_cast<CUdeviceptr>(dst), src, size);
	if (err != CUDA_SUCCESS) {
		throw std::runtime_error(std::string("CUDA: cuMemcpyHtoD failed: ") + getCUDAErrorString(err));
	}
}

void gpuMemcpyDtoH(void* dst, const void* src, size_t size) {
	CUresult err = cuMemcpyDtoH(dst, reinterpret_cast<CUdeviceptr>(src), size);
	if (err != CUDA_SUCCESS) {
		throw std::runtime_error(std::string("CUDA: cuMemcpyDtoH failed: ") + getCUDAErrorString(err));
	}
}

} // namespace nautilus::gpu::runtime

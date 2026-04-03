
#include "../GPURuntimeLib.hpp"

#import <Foundation/Foundation.h>
#import <Metal/Metal.h>

#include <cstring>
#include <mutex>
#include <stdexcept>

namespace nautilus::gpu::runtime {

/// Singleton managing Metal device and command queue.
struct MetalContext {
	id<MTLDevice> device;
	id<MTLCommandQueue> commandQueue;
	id<MTLCommandBuffer> lastCommandBuffer;

	static MetalContext& instance() {
		static MetalContext ctx;
		return ctx;
	}

private:
	MetalContext() {
		device = MTLCreateSystemDefaultDevice();
		if (!device) {
			throw std::runtime_error("Metal: no GPU device found");
		}
		commandQueue = [device newCommandQueue];
		if (!commandQueue) {
			throw std::runtime_error("Metal: failed to create command queue");
		}
		lastCommandBuffer = nil;
	}
};

void* gpuLoadModule(const void* kernelData, size_t kernelSize) {
	auto& ctx = MetalContext::instance();

	// kernelData is a pre-compiled metallib binary (from MetalKernelCompiler AIR pipeline)
	dispatch_data_t data = dispatch_data_create(kernelData, kernelSize, dispatch_get_main_queue(),
	                                            DISPATCH_DATA_DESTRUCTOR_DEFAULT);
	NSError* error = nil;
	id<MTLLibrary> library = [ctx.device newLibraryWithData:data error:&error];
	if (!library) {
		NSString* desc = error ? [error localizedDescription] : @"unknown error";
		throw std::runtime_error(std::string("Metal: failed to load metallib: ") + [desc UTF8String]);
	}

	// Transfer ownership to caller (retained by ARC in the id cast)
	return (__bridge_retained void*) library;
}

void* gpuGetKernel(void* module, const char* kernelName) {
	auto& ctx = MetalContext::instance();
	id<MTLLibrary> library = (__bridge id<MTLLibrary>) module;

	NSString* name = [NSString stringWithUTF8String:kernelName];
	id<MTLFunction> function = [library newFunctionWithName:name];
	if (!function) {
		throw std::runtime_error(std::string("Metal: kernel function not found: ") + kernelName);
	}

	NSError* error = nil;
	id<MTLComputePipelineState> pipeline = [ctx.device newComputePipelineStateWithFunction:function error:&error];
	if (!pipeline) {
		NSString* desc = error ? [error localizedDescription] : @"unknown error";
		throw std::runtime_error(std::string("Metal: failed to create pipeline: ") + [desc UTF8String]);
	}

	return (__bridge_retained void*) pipeline;
}

void gpuLaunchKernel(void* kernel, uint32_t gridX, uint32_t gridY, uint32_t gridZ, uint32_t blockX, uint32_t blockY,
                     uint32_t blockZ, void** args, size_t sharedMem) {
	auto& ctx = MetalContext::instance();
	id<MTLComputePipelineState> pipeline = (__bridge id<MTLComputePipelineState>) kernel;

	id<MTLCommandBuffer> commandBuffer = [ctx.commandQueue commandBuffer];
	id<MTLComputeCommandEncoder> encoder = [commandBuffer computeCommandEncoder];
	[encoder setComputePipelineState:pipeline];

	// Set buffer arguments. Each args[i] is a void* pointing to a MTLBuffer*
	// (the host code stores pointers to the kernel arguments, which are themselves MTLBuffer pointers).
	// We need to figure out how many arguments to bind. The pipeline reflection could give us this,
	// but for simplicity we iterate until we hit a null sentinel or use the buffer bindings from
	// the pipeline's reflection data.
	//
	// Convention: args is a void** where each entry is a pointer to a value.
	// For buffer arguments, the value itself is a MTLBuffer* (from gpuMalloc).
	// We bind them sequentially as buffer(0), buffer(1), etc.
	if (args) {
		// Determine argument count from pipeline reflection
		NSUInteger argCount = 0;
		for (MTLArgument* arg in pipeline.computeFunction.stageInputAttributes) {
			argCount++;
		}
		// Fallback: just iterate through args until we have bound all buffer arguments
		// The kernel compiler knows how many buffer args there are; the host code packs them.
		// We use the pipeline's maxTotalThreadsPerThreadgroup to avoid reading garbage.
		// Actually, we iterate through the argument table from reflection:
		NSArray<MTLArgument*>* arguments = nil;
		// Use the simple approach: args contains pointers to MTLBuffer* values
		// Bind until we hit a null or reach the thread-index parameter boundary
		for (NSUInteger i = 0; args[i] != nullptr; ++i) {
			// args[i] is a pointer to the argument value.
			// For buffer args, *(void**)args[i] is the MTLBuffer*.
			void* argVal = *(void**) args[i];
			id<MTLBuffer> buffer = (__bridge id<MTLBuffer>) argVal;
			[encoder setBuffer:buffer offset:0 atIndex:i];
		}
	}

	if (sharedMem > 0) {
		// Set threadgroup memory length for dynamic shared memory
		[encoder setThreadgroupMemoryLength:sharedMem atIndex:0];
	}

	MTLSize gridSize = MTLSizeMake(gridX, gridY, gridZ);
	MTLSize blockSize = MTLSizeMake(blockX, blockY, blockZ);
	[encoder dispatchThreadgroups:gridSize threadsPerThreadgroup:blockSize];
	[encoder endEncoding];
	[commandBuffer commit];

	ctx.lastCommandBuffer = commandBuffer;
}

void gpuSynchronize() {
	auto& ctx = MetalContext::instance();
	if (ctx.lastCommandBuffer) {
		[ctx.lastCommandBuffer waitUntilCompleted];
		ctx.lastCommandBuffer = nil;
	}
}

void* gpuMalloc(size_t size) {
	auto& ctx = MetalContext::instance();
	// Use shared storage mode for unified memory on Apple Silicon.
	// CPU and GPU can both access this memory without explicit copies.
	id<MTLBuffer> buffer = [ctx.device newBufferWithLength:size options:MTLResourceStorageModeShared];
	if (!buffer) {
		throw std::runtime_error("Metal: failed to allocate buffer of size " + std::to_string(size));
	}
	return (__bridge_retained void*) buffer;
}

void gpuFree(void* ptr) {
	if (ptr) {
		// Release the retained MTLBuffer
		id<MTLBuffer> buffer = (__bridge_transfer id<MTLBuffer>) ptr;
		(void) buffer; // ARC releases it
	}
}

void gpuMemcpyHtoD(void* dst, const void* src, size_t size) {
	// dst is a MTLBuffer* (from gpuMalloc)
	id<MTLBuffer> buffer = (__bridge id<MTLBuffer>) dst;
	std::memcpy(buffer.contents, src, size);
}

void gpuMemcpyDtoH(void* dst, const void* src, size_t size) {
	// src is a MTLBuffer* (from gpuMalloc)
	id<MTLBuffer> buffer = (__bridge id<MTLBuffer>) src;
	std::memcpy(dst, buffer.contents, size);
}

} // namespace nautilus::gpu::runtime

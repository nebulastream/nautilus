#include <cstdint>
#include <cstring>
#include <Metal/Metal.h>

#define NAUTILUS_BUFFER_SIZE 4096

extern "C" void execute(uint8_t* var_$1 ,uint8_t* var_$2 ,uint32_t var_$3 ){
uint32_t var_$4;
uint32_t var_$6;
uint32_t var_$7;
uint32_t var_$8;
uint32_t var_$10;
uint32_t var_$11;
Block_0:
var_$4 = (uint32_t)1;
var_$6 = (uint32_t)1;
var_$7 = (uint32_t)1;
var_$8 = (uint32_t)256;
var_$10 = (uint32_t)1;
var_$11 = (uint32_t)1;
// Metal kernel dispatch: classify
{
    id<MTLDevice> device = MTLCreateSystemDefaultDevice();
    NSError* error = nil;
    NSURL* libURL = [NSURL fileURLWithPath:@"__METALLIB_PATH__"];
    id<MTLLibrary> library = [device newLibraryWithURL:libURL error:&error];
    id<MTLFunction> kernelFunc = [library newFunctionWithName:@"classify"];
    id<MTLComputePipelineState> pipeline = [device newComputePipelineStateWithFunction:kernelFunc error:&error];
    id<MTLCommandQueue> queue = [device newCommandQueue];
    id<MTLCommandBuffer> cmdBuf = [queue commandBuffer];
    id<MTLComputeCommandEncoder> encoder = [cmdBuf computeCommandEncoder];
    [encoder setComputePipelineState:pipeline];
    id<MTLBuffer> buf_0 = [device newBufferWithBytes:(void*)var_$1 length:NAUTILUS_BUFFER_SIZE options:MTLResourceStorageModeShared];
    [encoder setBuffer:buf_0 offset:0 atIndex:0];
    id<MTLBuffer> buf_1 = [device newBufferWithBytes:(void*)var_$2 length:NAUTILUS_BUFFER_SIZE options:MTLResourceStorageModeShared];
    [encoder setBuffer:buf_1 offset:0 atIndex:1];
    [encoder setBytes:&var_$3 length:sizeof(uint32_t) atIndex:2];
    MTLSize grid = MTLSizeMake(var_$4,var_$6,var_$7);
    MTLSize block = MTLSizeMake(var_$8,var_$10,var_$11);
    [encoder dispatchThreadgroups:grid threadsPerThreadgroup:block];
    [encoder endEncoding];
    [cmdBuf commit];
    [cmdBuf waitUntilCompleted];
    memcpy((void*)var_$1, [buf_0 contents], NAUTILUS_BUFFER_SIZE);
    memcpy((void*)var_$2, [buf_1 contents], NAUTILUS_BUFFER_SIZE);
}
return;

}

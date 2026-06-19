#include <cstdint>
#include <Metal/Metal.h>

extern "C" uint64_t nautilus_gpu_buffer_bytes(void*);

extern "C" void execute(uint8_t* var_$1 ,uint8_t* var_$2 ,uint8_t* var_$3 ,uint32_t var_$4 ){
uint32_t var_$5;
uint32_t var_$6;
uint32_t var_$7;
uint32_t var_$8;
uint32_t var_$11;
uint32_t var_$12;
uint32_t var_$13;
uint32_t var_$15;
uint32_t var_$16;
Block_0:
var_$5 = (uint32_t)255;
var_$6 = var_$4+var_$5;
var_$7 = (uint32_t)256;
var_$8 = var_$6/var_$7;
var_$11 = (uint32_t)1;
var_$12 = (uint32_t)1;
var_$13 = (uint32_t)256;
var_$15 = (uint32_t)1;
var_$16 = (uint32_t)1;
// Metal kernel dispatch: vecAddBounded
{
    static id<MTLDevice> device = MTLCreateSystemDefaultDevice();
    static id<MTLCommandQueue> queue = [device newCommandQueue];
    NSError* error = nil;
    static id<MTLLibrary> library = [device newLibraryWithURL:[NSURL fileURLWithPath:@"__METALLIB_PATH__"] error:&error];
    static id<MTLComputePipelineState> pipeline = [device newComputePipelineStateWithFunction:[library newFunctionWithName:@"vecAddBounded"] error:&error];
    id<MTLCommandBuffer> cmdBuf = [queue commandBuffer];
    id<MTLComputeCommandEncoder> encoder = [cmdBuf computeCommandEncoder];
    [encoder setComputePipelineState:pipeline];
    id<MTLBuffer> buf_0 = [device newBufferWithBytesNoCopy:(void*)var_$1 length:nautilus_gpu_buffer_bytes((void*)var_$1) options:MTLResourceStorageModeShared deallocator:nil];
    [encoder setBuffer:buf_0 offset:0 atIndex:0];
    id<MTLBuffer> buf_1 = [device newBufferWithBytesNoCopy:(void*)var_$2 length:nautilus_gpu_buffer_bytes((void*)var_$2) options:MTLResourceStorageModeShared deallocator:nil];
    [encoder setBuffer:buf_1 offset:0 atIndex:1];
    id<MTLBuffer> buf_2 = [device newBufferWithBytesNoCopy:(void*)var_$3 length:nautilus_gpu_buffer_bytes((void*)var_$3) options:MTLResourceStorageModeShared deallocator:nil];
    [encoder setBuffer:buf_2 offset:0 atIndex:2];
    [encoder setBytes:&var_$4 length:sizeof(uint32_t) atIndex:3];
    MTLSize grid = MTLSizeMake(var_$8,var_$11,var_$12);
    MTLSize block = MTLSizeMake(var_$13,var_$15,var_$16);
    [encoder dispatchThreadgroups:grid threadsPerThreadgroup:block];
    [encoder endEncoding];
    [cmdBuf commit];
    [cmdBuf waitUntilCompleted];
}
return;

}

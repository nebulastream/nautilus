
#include <cstdint>
#include <cuda_runtime.h>

__global__ void execute_kernel(){
Block_0:
__syncthreads();
return;

}

extern "C" void execute(){
    dim3 grid(1,1,1);
    dim3 block(256,1,1);
    execute_kernel<<<grid, block>>>();
    cudaDeviceSynchronize();
}

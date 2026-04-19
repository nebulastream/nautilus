
#include <cstdint>
#include <cuda_runtime.h>

__global__ void execute_kernel(){
uint32_t var_$1;
uint32_t var_$2;
uint32_t var_$3;
uint32_t var_$4;
uint32_t var_$5;
Block_0:
var_$1 = blockIdx.x;
var_$2 = blockDim.x;
var_$3 = var_$1*var_$2;
var_$4 = threadIdx.x;
var_$5 = var_$3+var_$4;
return var_$5;

}

extern "C" void execute(){
    dim3 grid(1,1,1);
    dim3 block(256,1,1);
    execute_kernel<<<grid, block>>>();
    cudaDeviceSynchronize();
}

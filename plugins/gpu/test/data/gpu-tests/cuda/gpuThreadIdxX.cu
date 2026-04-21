
#include <cstdint>
#include <cuda_runtime.h>

__global__ void execute_kernel(uint32_t var_$1 ){
uint32_t var_$2;
uint32_t var_$3;
Block_0:
var_$2 = threadIdx.x;
var_$3 = var_$2+var_$1;
return var_$3;

}

extern "C" void execute(uint32_t arg_0){
    dim3 grid(1,1,1);
    dim3 block(256,1,1);
    execute_kernel<<<grid, block>>>(arg_0);
    cudaDeviceSynchronize();
}

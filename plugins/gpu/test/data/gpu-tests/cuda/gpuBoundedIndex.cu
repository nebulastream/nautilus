
#include <cstdint>
#include <cuda_runtime.h>

__global__ void execute_kernel(uint32_t var_$1 ){
uint32_t var_$2;
uint32_t var_$3;
uint32_t var_$4;
uint32_t var_$5;
uint32_t var_$6;
bool var_$7;
uint32_t var_$10;
Block_0:
var_$2 = blockIdx.x;
var_$3 = blockDim.x;
var_$4 = var_$2*var_$3;
var_$5 = threadIdx.x;
var_$6 = var_$4+var_$5;
var_$7 = var_$6 < var_$1;
if (var_$7){
{
uint32_t temp_0 = var_$6;
var_$6 = temp_0;
}
goto Block_1;
}else{
{
}
goto Block_2;}

Block_1:
{
uint32_t temp_0 = var_$6;
var_$6 = temp_0;
}
goto Block_3;

Block_3:
return var_$6;

Block_2:
var_$10 = (uint32_t)0;
{
uint32_t temp_0 = var_$10;
var_$6 = temp_0;
}
goto Block_3;

}

extern "C" void execute(uint32_t arg_0){
    dim3 grid(1,1,1);
    dim3 block(256,1,1);
    execute_kernel<<<grid, block>>>(arg_0);
    cudaDeviceSynchronize();
}

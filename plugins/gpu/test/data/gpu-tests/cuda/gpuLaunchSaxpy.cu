
#include <cstdint>
#include <cuda_runtime.h>

__global__ void saxpy(uint8_t* var_$1 ,uint8_t* var_$2 ,float var_$3 ){
uint32_t var_$6;
uint32_t var_$7;
uint32_t var_$8;
uint32_t var_$9;
uint32_t var_$10;
int32_t var_$11;
uint64_t var_$15;
uint64_t var_$16;
uint64_t var_$17;
uint8_t* var_$18;
float var_$21;
float var_$22;
int32_t var_$23;
uint64_t var_$27;
uint64_t var_$28;
uint64_t var_$29;
uint8_t* var_$30;
float var_$33;
float var_$34;
int32_t var_$35;
uint64_t var_$39;
uint64_t var_$40;
uint64_t var_$41;
uint8_t* var_$42;
Block_0:
var_$6 = blockIdx.x;
var_$7 = blockDim.x;
var_$8 = var_$6*var_$7;
var_$9 = threadIdx.x;
var_$10 = var_$8+var_$9;
var_$11 = (int32_t)var_$10;
var_$15 = (uint64_t)4;
var_$16 = (uint64_t)var_$11;
var_$17 = var_$16*var_$15;
var_$18 = var_$1+var_$17;
var_$21 = *((float*)(var_$18));
var_$22 = var_$3*var_$21;
var_$23 = (int32_t)var_$10;
var_$27 = (uint64_t)4;
var_$28 = (uint64_t)var_$23;
var_$29 = var_$28*var_$27;
var_$30 = var_$2+var_$29;
var_$33 = *((float*)(var_$30));
var_$34 = var_$22+var_$33;
var_$35 = (int32_t)var_$10;
var_$39 = (uint64_t)4;
var_$40 = (uint64_t)var_$35;
var_$41 = var_$40*var_$39;
var_$42 = var_$2+var_$41;
*((float*)(var_$42)) = var_$34;
return;

}

extern "C" void execute(uint8_t* var_$1 ,uint8_t* var_$2 ,float var_$3 ){
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
saxpy<<<dim3(var_$4,var_$6,var_$7),dim3(var_$8,var_$10,var_$11)>>>(var_$1,var_$2,var_$3);
cudaDeviceSynchronize();
return;

}

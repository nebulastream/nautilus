
#include <cstdint>
#include <cuda_runtime.h>

__global__ void saxpy(uint8_t* var_$1 ,uint8_t* var_$2 ,float var_$3 ){
uint32_t var_$4;
uint32_t var_$5;
uint32_t var_$6;
uint32_t var_$7;
uint32_t var_$8;
int32_t var_$9;
uint64_t var_$13;
uint64_t var_$14;
uint64_t var_$15;
uint8_t* var_$16;
float var_$19;
float var_$20;
int32_t var_$21;
uint64_t var_$25;
uint64_t var_$26;
uint64_t var_$27;
uint8_t* var_$28;
float var_$31;
float var_$32;
int32_t var_$33;
uint64_t var_$37;
uint64_t var_$38;
uint64_t var_$39;
uint8_t* var_$40;
Block_0:
var_$4 = blockIdx.x;
var_$5 = blockDim.x;
var_$6 = var_$4*var_$5;
var_$7 = threadIdx.x;
var_$8 = var_$6+var_$7;
var_$9 = (int32_t)var_$8;
var_$13 = (uint64_t)4;
var_$14 = (uint64_t)var_$9;
var_$15 = var_$14*var_$13;
var_$16 = var_$1+var_$15;
var_$19 = *((float*)(var_$16));
var_$20 = var_$3*var_$19;
var_$21 = (int32_t)var_$8;
var_$25 = (uint64_t)4;
var_$26 = (uint64_t)var_$21;
var_$27 = var_$26*var_$25;
var_$28 = var_$2+var_$27;
var_$31 = *((float*)(var_$28));
var_$32 = var_$20+var_$31;
var_$33 = (int32_t)var_$8;
var_$37 = (uint64_t)4;
var_$38 = (uint64_t)var_$33;
var_$39 = var_$38*var_$37;
var_$40 = var_$2+var_$39;
*((float*)(var_$40)) = var_$32;
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

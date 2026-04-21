
#include <cstdint>
#include <cuda_runtime.h>

__global__ void vecScale(uint8_t* var_$1 ,uint8_t* var_$2 ,uint32_t var_$3 ){
uint32_t var_$6;
int32_t var_$7;
uint64_t var_$11;
uint64_t var_$12;
uint64_t var_$13;
uint8_t* var_$14;
uint32_t var_$18;
uint32_t var_$19;
int32_t var_$20;
uint64_t var_$24;
uint64_t var_$25;
uint64_t var_$26;
uint8_t* var_$27;
Block_0:
var_$6 = threadIdx.x;
var_$7 = (int32_t)var_$6;
var_$11 = (uint64_t)4;
var_$12 = (uint64_t)var_$7;
var_$13 = var_$12*var_$11;
var_$14 = var_$1+var_$13;
var_$18 = *((uint32_t*)(var_$14));
var_$19 = var_$18*var_$3;
var_$20 = (int32_t)var_$6;
var_$24 = (uint64_t)4;
var_$25 = (uint64_t)var_$20;
var_$26 = var_$25*var_$24;
var_$27 = var_$2+var_$26;
*((uint32_t*)(var_$27)) = var_$19;
return;

}

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
vecScale<<<dim3(var_$4,var_$6,var_$7),dim3(var_$8,var_$10,var_$11)>>>(var_$1,var_$2,var_$3);
cudaDeviceSynchronize();
return;

}

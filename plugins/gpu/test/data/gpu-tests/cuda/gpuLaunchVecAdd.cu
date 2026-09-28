
#include <cstdint>
#include <cuda_runtime.h>

__global__ void vecAdd(uint8_t* var_$1 ,uint8_t* var_$2 ,uint8_t* var_$3 ){
uint32_t var_$7;
uint64_t var_$8;
uint64_t var_$9;
uint64_t var_$10;
uint8_t* var_$11;
uint64_t var_$12;
uint64_t var_$13;
uint64_t var_$14;
uint8_t* var_$15;
uint32_t var_$16;
uint32_t var_$17;
uint32_t var_$18;
uint64_t var_$19;
uint64_t var_$20;
uint64_t var_$21;
uint8_t* var_$22;
Block_0:
var_$7 = threadIdx.x;
var_$8 = (uint64_t)4;
var_$9 = (uint64_t)var_$7;
var_$10 = var_$9*var_$8;
var_$11 = var_$1+var_$10;
var_$12 = (uint64_t)4;
var_$13 = (uint64_t)var_$7;
var_$14 = var_$13*var_$12;
var_$15 = var_$2+var_$14;
var_$16 = *((uint32_t*)(var_$11));
var_$17 = *((uint32_t*)(var_$15));
var_$18 = var_$16+var_$17;
var_$19 = (uint64_t)4;
var_$20 = (uint64_t)var_$7;
var_$21 = var_$20*var_$19;
var_$22 = var_$3+var_$21;
*((uint32_t*)(var_$22)) = var_$18;
return;

}

extern "C" void execute(uint8_t* var_$1 ,uint8_t* var_$2 ,uint8_t* var_$3 ){
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
vecAdd<<<dim3(var_$4,var_$6,var_$7),dim3(var_$8,var_$10,var_$11)>>>(var_$1,var_$2,var_$3);
cudaDeviceSynchronize();
return;

}

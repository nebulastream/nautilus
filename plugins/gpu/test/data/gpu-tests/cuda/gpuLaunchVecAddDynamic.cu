
#include <cstdint>
#include <cuda_runtime.h>

__global__ void vecAdd(uint8_t* var_$1 ,uint8_t* var_$2 ,uint8_t* var_$3 ){
uint32_t var_$7;
int32_t var_$8;
uint64_t var_$12;
uint64_t var_$13;
uint64_t var_$14;
uint8_t* var_$15;
int32_t var_$18;
uint64_t var_$22;
uint64_t var_$23;
uint64_t var_$24;
uint8_t* var_$25;
uint32_t var_$28;
uint32_t var_$29;
uint32_t var_$30;
int32_t var_$31;
uint64_t var_$35;
uint64_t var_$36;
uint64_t var_$37;
uint8_t* var_$38;
Block_0:
var_$7 = threadIdx.x;
var_$8 = (int32_t)var_$7;
var_$12 = (uint64_t)4;
var_$13 = (uint64_t)var_$8;
var_$14 = var_$13*var_$12;
var_$15 = var_$1+var_$14;
var_$18 = (int32_t)var_$7;
var_$22 = (uint64_t)4;
var_$23 = (uint64_t)var_$18;
var_$24 = var_$23*var_$22;
var_$25 = var_$2+var_$24;
var_$28 = *((uint32_t*)(var_$15));
var_$29 = *((uint32_t*)(var_$25));
var_$30 = var_$28+var_$29;
var_$31 = (int32_t)var_$7;
var_$35 = (uint64_t)4;
var_$36 = (uint64_t)var_$31;
var_$37 = var_$36*var_$35;
var_$38 = var_$3+var_$37;
*((uint32_t*)(var_$38)) = var_$30;
return;

}

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
vecAdd<<<dim3(var_$8,var_$11,var_$12),dim3(var_$13,var_$15,var_$16)>>>(var_$1,var_$2,var_$3);
cudaDeviceSynchronize();
return;

}

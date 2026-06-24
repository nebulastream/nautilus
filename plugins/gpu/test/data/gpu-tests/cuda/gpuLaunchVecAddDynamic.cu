
#include <cstdint>
#include <cuda_runtime.h>

__global__ void vecAdd(uint8_t* var_$1 ,uint8_t* var_$2 ,uint8_t* var_$3 ){
uint32_t var_$4;
int32_t var_$5;
uint64_t var_$9;
uint64_t var_$10;
uint64_t var_$11;
uint8_t* var_$12;
int32_t var_$15;
uint64_t var_$19;
uint64_t var_$20;
uint64_t var_$21;
uint8_t* var_$22;
uint32_t var_$25;
uint32_t var_$26;
uint32_t var_$27;
int32_t var_$28;
uint64_t var_$32;
uint64_t var_$33;
uint64_t var_$34;
uint8_t* var_$35;
Block_0:
var_$4 = threadIdx.x;
var_$5 = (int32_t)var_$4;
var_$9 = (uint64_t)4;
var_$10 = (uint64_t)var_$5;
var_$11 = var_$10*var_$9;
var_$12 = var_$1+var_$11;
var_$15 = (int32_t)var_$4;
var_$19 = (uint64_t)4;
var_$20 = (uint64_t)var_$15;
var_$21 = var_$20*var_$19;
var_$22 = var_$2+var_$21;
var_$25 = *((uint32_t*)(var_$12));
var_$26 = *((uint32_t*)(var_$22));
var_$27 = var_$25+var_$26;
var_$28 = (int32_t)var_$4;
var_$32 = (uint64_t)4;
var_$33 = (uint64_t)var_$28;
var_$34 = var_$33*var_$32;
var_$35 = var_$3+var_$34;
*((uint32_t*)(var_$35)) = var_$27;
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

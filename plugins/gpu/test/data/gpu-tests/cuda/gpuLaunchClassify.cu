
#include <cstdint>
#include <cuda_runtime.h>

__device__ void execute(uint8_t* var_$1 ,uint8_t* var_$2 ,uint32_t var_$3 ){
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
classify<<<dim3(var_$4,var_$6,var_$7),dim3(var_$8,var_$10,var_$11)>>>(var_$1,var_$2,var_$3);
cudaDeviceSynchronize();
return;

}

__global__ void classify_kernel(uint8_t* var_$1 ,uint8_t* var_$2 ,uint32_t var_$3 ){
uint32_t var_$6;
uint32_t var_$7;
uint32_t var_$8;
uint32_t var_$9;
uint32_t var_$10;
bool var_$11;
uint8_t* var_$5;
uint8_t* var_$4;
int32_t var_$13;
uint64_t var_$17;
uint64_t var_$18;
uint64_t var_$19;
uint8_t* var_$20;
uint32_t var_$23;
uint32_t var_$24;
bool var_$25;
uint32_t var_$27;
int32_t var_$28;
uint64_t var_$32;
uint64_t var_$33;
uint64_t var_$34;
uint8_t* var_$35;
int32_t var_$46;
uint64_t var_$50;
uint64_t var_$51;
uint64_t var_$52;
uint8_t* var_$53;
uint32_t var_$56;
uint32_t var_$57;
bool var_$58;
uint32_t var_$60;
int32_t var_$61;
uint64_t var_$65;
uint64_t var_$66;
uint64_t var_$67;
uint8_t* var_$68;
int32_t var_$76;
uint64_t var_$80;
uint64_t var_$81;
uint64_t var_$82;
uint8_t* var_$83;
uint32_t var_$86;
uint32_t var_$87;
bool var_$88;
uint32_t var_$90;
int32_t var_$91;
uint64_t var_$95;
uint64_t var_$96;
uint64_t var_$97;
uint8_t* var_$98;
uint32_t var_$106;
int32_t var_$107;
uint64_t var_$111;
uint64_t var_$112;
uint64_t var_$113;
uint8_t* var_$114;
Block_0:
var_$6 = blockIdx.x;
var_$7 = blockDim.x;
var_$8 = var_$6*var_$7;
var_$9 = threadIdx.x;
var_$10 = var_$8+var_$9;
var_$11 = var_$10 < var_$3;
if (var_$11){
{
uint8_t* temp_0 = var_$2;
uint32_t temp_1 = var_$10;
uint8_t* temp_2 = var_$1;
var_$5 = temp_0;
var_$10 = temp_1;
var_$4 = temp_2;
}
goto Block_1;
}else{
{
}
goto Block_2;}

Block_1:
var_$13 = (int32_t)var_$10;
var_$17 = (uint64_t)4;
var_$18 = (uint64_t)var_$13;
var_$19 = var_$18*var_$17;
var_$20 = var_$4+var_$19;
var_$23 = (uint32_t)200;
var_$24 = *((uint32_t*)(var_$20));
var_$25 = var_$24 > var_$23;
if (var_$25){
{
uint8_t* temp_0 = var_$5;
uint32_t temp_1 = var_$10;
var_$5 = temp_0;
var_$10 = temp_1;
}
goto Block_3;
}else{
{
uint8_t* temp_0 = var_$5;
uint32_t temp_1 = var_$10;
uint8_t* temp_2 = var_$4;
var_$5 = temp_0;
var_$10 = temp_1;
var_$4 = temp_2;
}
goto Block_4;}

Block_3:
var_$27 = (uint32_t)3;
var_$28 = (int32_t)var_$10;
var_$32 = (uint64_t)4;
var_$33 = (uint64_t)var_$28;
var_$34 = var_$33*var_$32;
var_$35 = var_$5+var_$34;
*((uint32_t*)(var_$35)) = var_$27;
{
}
goto Block_9;

Block_9:
return;

Block_4:
var_$46 = (int32_t)var_$10;
var_$50 = (uint64_t)4;
var_$51 = (uint64_t)var_$46;
var_$52 = var_$51*var_$50;
var_$53 = var_$4+var_$52;
var_$56 = (uint32_t)100;
var_$57 = *((uint32_t*)(var_$53));
var_$58 = var_$57 > var_$56;
if (var_$58){
{
uint8_t* temp_0 = var_$5;
uint32_t temp_1 = var_$10;
var_$5 = temp_0;
var_$10 = temp_1;
}
goto Block_5;
}else{
{
uint8_t* temp_0 = var_$5;
uint32_t temp_1 = var_$10;
uint8_t* temp_2 = var_$4;
var_$5 = temp_0;
var_$10 = temp_1;
var_$4 = temp_2;
}
goto Block_6;}

Block_5:
var_$60 = (uint32_t)2;
var_$61 = (int32_t)var_$10;
var_$65 = (uint64_t)4;
var_$66 = (uint64_t)var_$61;
var_$67 = var_$66*var_$65;
var_$68 = var_$5+var_$67;
*((uint32_t*)(var_$68)) = var_$60;
{
}
goto Block_9;

Block_6:
var_$76 = (int32_t)var_$10;
var_$80 = (uint64_t)4;
var_$81 = (uint64_t)var_$76;
var_$82 = var_$81*var_$80;
var_$83 = var_$4+var_$82;
var_$86 = (uint32_t)0;
var_$87 = *((uint32_t*)(var_$83));
var_$88 = var_$87 > var_$86;
if (var_$88){
{
uint8_t* temp_0 = var_$5;
uint32_t temp_1 = var_$10;
var_$5 = temp_0;
var_$10 = temp_1;
}
goto Block_7;
}else{
{
uint8_t* temp_0 = var_$5;
uint32_t temp_1 = var_$10;
var_$5 = temp_0;
var_$10 = temp_1;
}
goto Block_8;}

Block_7:
var_$90 = (uint32_t)1;
var_$91 = (int32_t)var_$10;
var_$95 = (uint64_t)4;
var_$96 = (uint64_t)var_$91;
var_$97 = var_$96*var_$95;
var_$98 = var_$5+var_$97;
*((uint32_t*)(var_$98)) = var_$90;
{
}
goto Block_9;

Block_8:
var_$106 = (uint32_t)0;
var_$107 = (int32_t)var_$10;
var_$111 = (uint64_t)4;
var_$112 = (uint64_t)var_$107;
var_$113 = var_$112*var_$111;
var_$114 = var_$5+var_$113;
*((uint32_t*)(var_$114)) = var_$106;
{
}
goto Block_9;

Block_2:
{
}
goto Block_9;

}

extern "C" void execute(uint8_t* arg_0,uint8_t* arg_1,uint32_t arg_2){
    dim3 grid(1,1,1);
    dim3 block(256,1,1);
    classify_kernel<<<grid, block>>>(arg_0,arg_1,arg_2);
    cudaDeviceSynchronize();
}

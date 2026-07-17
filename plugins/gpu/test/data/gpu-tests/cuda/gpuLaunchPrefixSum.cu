
#include <cstdint>
#include <cuda_runtime.h>

__global__ void prefixSum(uint8_t* var_$1 ,uint8_t* var_$2 ,uint32_t var_$3 ){
uint32_t var_$4;
uint32_t var_$5;
uint32_t var_$6;
uint32_t var_$7;
uint32_t var_$8;
bool var_$9;
uint32_t var_$11;
uint32_t var_$12;
bool var_$13;
int32_t var_$15;
uint64_t var_$19;
uint64_t var_$20;
uint64_t var_$21;
uint8_t* var_$22;
uint32_t var_$25;
uint32_t var_$26;
uint32_t var_$27;
uint32_t var_$28;
int32_t var_$36;
uint64_t var_$40;
uint64_t var_$41;
uint64_t var_$42;
uint8_t* var_$43;
Block_0:
var_$4 = blockIdx.x;
var_$5 = blockDim.x;
var_$6 = var_$4*var_$5;
var_$7 = threadIdx.x;
var_$8 = var_$6+var_$7;
var_$9 = var_$8 < var_$3;
if (var_$9){
{
uint8_t* temp_0 = var_$2;
uint32_t temp_1 = var_$8;
uint8_t* temp_2 = var_$1;
var_$2 = temp_0;
var_$8 = temp_1;
var_$1 = temp_2;
}
goto Block_1;
}else{
{
}
goto Block_2;}

Block_1:
var_$11 = (uint32_t)0;
var_$12 = (uint32_t)0;
{
uint8_t* temp_0 = var_$2;
uint32_t temp_1 = var_$8;
uint32_t temp_2 = var_$11;
uint32_t temp_3 = var_$12;
uint8_t* temp_4 = var_$1;
var_$2 = temp_0;
var_$8 = temp_1;
var_$11 = temp_2;
var_$12 = temp_3;
var_$1 = temp_4;
}
goto Block_5;

Block_5:
var_$13 = var_$12 <= var_$8;
if (var_$13){
{
uint8_t* temp_0 = var_$2;
uint32_t temp_1 = var_$8;
uint32_t temp_2 = var_$12;
uint32_t temp_3 = var_$11;
uint8_t* temp_4 = var_$1;
var_$2 = temp_0;
var_$8 = temp_1;
var_$12 = temp_2;
var_$11 = temp_3;
var_$1 = temp_4;
}
goto Block_3;
}else{
{
uint8_t* temp_0 = var_$2;
uint32_t temp_1 = var_$8;
uint32_t temp_2 = var_$11;
var_$2 = temp_0;
var_$8 = temp_1;
var_$11 = temp_2;
}
goto Block_4;}

Block_3:
var_$15 = (int32_t)var_$12;
var_$19 = (uint64_t)4;
var_$20 = (uint64_t)var_$15;
var_$21 = var_$20*var_$19;
var_$22 = var_$1+var_$21;
var_$25 = *((uint32_t*)(var_$22));
var_$26 = var_$11+var_$25;
var_$27 = (uint32_t)1;
var_$28 = var_$12+var_$27;
{
uint8_t* temp_0 = var_$2;
uint32_t temp_1 = var_$8;
uint32_t temp_2 = var_$26;
uint32_t temp_3 = var_$28;
uint8_t* temp_4 = var_$1;
var_$2 = temp_0;
var_$8 = temp_1;
var_$11 = temp_2;
var_$12 = temp_3;
var_$1 = temp_4;
}
goto Block_5;

Block_4:
var_$36 = (int32_t)var_$8;
var_$40 = (uint64_t)4;
var_$41 = (uint64_t)var_$36;
var_$42 = var_$41*var_$40;
var_$43 = var_$2+var_$42;
*((uint32_t*)(var_$43)) = var_$11;
{
}
goto Block_6;

Block_6:
return;

Block_2:
{
}
goto Block_6;

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
prefixSum<<<dim3(var_$4,var_$6,var_$7),dim3(var_$8,var_$10,var_$11)>>>(var_$1,var_$2,var_$3);
cudaDeviceSynchronize();
return;

}

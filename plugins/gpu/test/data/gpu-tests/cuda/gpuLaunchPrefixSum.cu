
#include <cstdint>
#include <cuda_runtime.h>

__global__ void prefixSum(uint8_t* var_$1 ,uint8_t* var_$2 ,uint32_t var_$3 ){
uint32_t var_$6;
uint32_t var_$7;
uint32_t var_$8;
uint32_t var_$9;
uint32_t var_$10;
bool var_$11;
uint8_t* var_$5;
uint8_t* var_$4;
uint32_t var_$13;
uint32_t var_$14;
bool var_$15;
uint64_t var_$17;
uint64_t var_$18;
uint64_t var_$19;
uint8_t* var_$20;
uint32_t var_$21;
uint32_t var_$22;
uint32_t var_$23;
uint32_t var_$24;
uint64_t var_$31;
uint64_t var_$32;
uint64_t var_$33;
uint8_t* var_$34;
Block_0:
var_$6 = blockIdx.x;
var_$7 = blockDim.x;
var_$8 = var_$6*var_$7;
var_$9 = threadIdx.x;
var_$10 = var_$8+var_$9;
var_$11 = var_$10 < var_$3;
if (var_$11){
{
uint32_t temp_0 = var_$10;
uint8_t* temp_1 = var_$2;
uint8_t* temp_2 = var_$1;
var_$10 = temp_0;
var_$5 = temp_1;
var_$4 = temp_2;
}
goto Block_1;
}else{
{
}
goto Block_2;}

Block_1:
var_$13 = (uint32_t)0;
var_$14 = (uint32_t)0;
{
uint32_t temp_0 = var_$10;
uint8_t* temp_1 = var_$5;
uint32_t temp_2 = var_$13;
uint32_t temp_3 = var_$14;
uint8_t* temp_4 = var_$4;
var_$10 = temp_0;
var_$5 = temp_1;
var_$13 = temp_2;
var_$14 = temp_3;
var_$4 = temp_4;
}
goto Block_5;

Block_5:
var_$15 = var_$14 <= var_$10;
if (var_$15){
{
uint32_t temp_0 = var_$10;
uint8_t* temp_1 = var_$5;
uint32_t temp_2 = var_$14;
uint8_t* temp_3 = var_$4;
uint32_t temp_4 = var_$13;
var_$10 = temp_0;
var_$5 = temp_1;
var_$14 = temp_2;
var_$4 = temp_3;
var_$13 = temp_4;
}
goto Block_3;
}else{
{
uint32_t temp_0 = var_$10;
uint8_t* temp_1 = var_$5;
uint32_t temp_2 = var_$13;
var_$10 = temp_0;
var_$5 = temp_1;
var_$13 = temp_2;
}
goto Block_4;}

Block_3:
var_$17 = (uint64_t)4;
var_$18 = (uint64_t)var_$14;
var_$19 = var_$18*var_$17;
var_$20 = var_$4+var_$19;
var_$21 = *((uint32_t*)(var_$20));
var_$22 = var_$13+var_$21;
var_$23 = (uint32_t)1;
var_$24 = var_$14+var_$23;
{
uint32_t temp_0 = var_$10;
uint8_t* temp_1 = var_$5;
uint32_t temp_2 = var_$22;
uint32_t temp_3 = var_$24;
uint8_t* temp_4 = var_$4;
var_$10 = temp_0;
var_$5 = temp_1;
var_$13 = temp_2;
var_$14 = temp_3;
var_$4 = temp_4;
}
goto Block_5;

Block_4:
var_$31 = (uint64_t)4;
var_$32 = (uint64_t)var_$10;
var_$33 = var_$32*var_$31;
var_$34 = var_$5+var_$33;
*((uint32_t*)(var_$34)) = var_$13;
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

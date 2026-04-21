
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
int32_t var_$17;
uint64_t var_$21;
uint64_t var_$22;
uint64_t var_$23;
uint8_t* var_$24;
uint32_t var_$27;
uint32_t var_$28;
uint32_t var_$29;
uint32_t var_$30;
int32_t var_$38;
uint64_t var_$42;
uint64_t var_$43;
uint64_t var_$44;
uint8_t* var_$45;
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
var_$13 = (uint32_t)0;
var_$14 = (uint32_t)0;
{
uint8_t* temp_0 = var_$5;
uint32_t temp_1 = var_$10;
uint32_t temp_2 = var_$13;
uint32_t temp_3 = var_$14;
uint8_t* temp_4 = var_$4;
var_$5 = temp_0;
var_$10 = temp_1;
var_$13 = temp_2;
var_$14 = temp_3;
var_$4 = temp_4;
}
goto Block_5;

Block_5:
var_$15 = var_$14 <= var_$10;
if (var_$15){
{
uint8_t* temp_0 = var_$5;
uint32_t temp_1 = var_$10;
uint32_t temp_2 = var_$14;
uint32_t temp_3 = var_$13;
uint8_t* temp_4 = var_$4;
var_$5 = temp_0;
var_$10 = temp_1;
var_$14 = temp_2;
var_$13 = temp_3;
var_$4 = temp_4;
}
goto Block_3;
}else{
{
uint8_t* temp_0 = var_$5;
uint32_t temp_1 = var_$10;
uint32_t temp_2 = var_$13;
var_$5 = temp_0;
var_$10 = temp_1;
var_$13 = temp_2;
}
goto Block_4;}

Block_3:
var_$17 = (int32_t)var_$14;
var_$21 = (uint64_t)4;
var_$22 = (uint64_t)var_$17;
var_$23 = var_$22*var_$21;
var_$24 = var_$4+var_$23;
var_$27 = *((uint32_t*)(var_$24));
var_$28 = var_$13+var_$27;
var_$29 = (uint32_t)1;
var_$30 = var_$14+var_$29;
{
uint8_t* temp_0 = var_$5;
uint32_t temp_1 = var_$10;
uint32_t temp_2 = var_$28;
uint32_t temp_3 = var_$30;
uint8_t* temp_4 = var_$4;
var_$5 = temp_0;
var_$10 = temp_1;
var_$13 = temp_2;
var_$14 = temp_3;
var_$4 = temp_4;
}
goto Block_5;

Block_4:
var_$38 = (int32_t)var_$10;
var_$42 = (uint64_t)4;
var_$43 = (uint64_t)var_$38;
var_$44 = var_$43*var_$42;
var_$45 = var_$5+var_$44;
*((uint32_t*)(var_$45)) = var_$13;
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

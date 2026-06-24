
#include <cstdint>
#include <cuda_runtime.h>

__global__ void classify(uint8_t* var_$1 ,uint8_t* var_$2 ,uint32_t var_$3 ){
uint32_t var_$4;
uint32_t var_$5;
uint32_t var_$6;
uint32_t var_$7;
uint32_t var_$8;
bool var_$9;
int32_t var_$11;
uint64_t var_$15;
uint64_t var_$16;
uint64_t var_$17;
uint8_t* var_$18;
uint32_t var_$21;
uint32_t var_$22;
bool var_$23;
uint32_t var_$25;
int32_t var_$26;
uint64_t var_$30;
uint64_t var_$31;
uint64_t var_$32;
uint8_t* var_$33;
int32_t var_$44;
uint64_t var_$48;
uint64_t var_$49;
uint64_t var_$50;
uint8_t* var_$51;
uint32_t var_$54;
uint32_t var_$55;
bool var_$56;
uint32_t var_$58;
int32_t var_$59;
uint64_t var_$63;
uint64_t var_$64;
uint64_t var_$65;
uint8_t* var_$66;
int32_t var_$74;
uint64_t var_$78;
uint64_t var_$79;
uint64_t var_$80;
uint8_t* var_$81;
uint32_t var_$84;
uint32_t var_$85;
bool var_$86;
uint32_t var_$88;
int32_t var_$89;
uint64_t var_$93;
uint64_t var_$94;
uint64_t var_$95;
uint8_t* var_$96;
uint32_t var_$104;
int32_t var_$105;
uint64_t var_$109;
uint64_t var_$110;
uint64_t var_$111;
uint8_t* var_$112;
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
var_$11 = (int32_t)var_$8;
var_$15 = (uint64_t)4;
var_$16 = (uint64_t)var_$11;
var_$17 = var_$16*var_$15;
var_$18 = var_$1+var_$17;
var_$21 = (uint32_t)200;
var_$22 = *((uint32_t*)(var_$18));
var_$23 = var_$22 > var_$21;
if (var_$23){
{
uint8_t* temp_0 = var_$2;
uint32_t temp_1 = var_$8;
var_$2 = temp_0;
var_$8 = temp_1;
}
goto Block_3;
}else{
{
uint8_t* temp_0 = var_$2;
uint32_t temp_1 = var_$8;
uint8_t* temp_2 = var_$1;
var_$2 = temp_0;
var_$8 = temp_1;
var_$1 = temp_2;
}
goto Block_4;}

Block_3:
var_$25 = (uint32_t)3;
var_$26 = (int32_t)var_$8;
var_$30 = (uint64_t)4;
var_$31 = (uint64_t)var_$26;
var_$32 = var_$31*var_$30;
var_$33 = var_$2+var_$32;
*((uint32_t*)(var_$33)) = var_$25;
{
}
goto Block_9;

Block_9:
return;

Block_4:
var_$44 = (int32_t)var_$8;
var_$48 = (uint64_t)4;
var_$49 = (uint64_t)var_$44;
var_$50 = var_$49*var_$48;
var_$51 = var_$1+var_$50;
var_$54 = (uint32_t)100;
var_$55 = *((uint32_t*)(var_$51));
var_$56 = var_$55 > var_$54;
if (var_$56){
{
uint8_t* temp_0 = var_$2;
uint32_t temp_1 = var_$8;
var_$2 = temp_0;
var_$8 = temp_1;
}
goto Block_5;
}else{
{
uint8_t* temp_0 = var_$2;
uint32_t temp_1 = var_$8;
uint8_t* temp_2 = var_$1;
var_$2 = temp_0;
var_$8 = temp_1;
var_$1 = temp_2;
}
goto Block_6;}

Block_5:
var_$58 = (uint32_t)2;
var_$59 = (int32_t)var_$8;
var_$63 = (uint64_t)4;
var_$64 = (uint64_t)var_$59;
var_$65 = var_$64*var_$63;
var_$66 = var_$2+var_$65;
*((uint32_t*)(var_$66)) = var_$58;
{
}
goto Block_9;

Block_6:
var_$74 = (int32_t)var_$8;
var_$78 = (uint64_t)4;
var_$79 = (uint64_t)var_$74;
var_$80 = var_$79*var_$78;
var_$81 = var_$1+var_$80;
var_$84 = (uint32_t)0;
var_$85 = *((uint32_t*)(var_$81));
var_$86 = var_$85 > var_$84;
if (var_$86){
{
uint8_t* temp_0 = var_$2;
uint32_t temp_1 = var_$8;
var_$2 = temp_0;
var_$8 = temp_1;
}
goto Block_7;
}else{
{
uint8_t* temp_0 = var_$2;
uint32_t temp_1 = var_$8;
var_$2 = temp_0;
var_$8 = temp_1;
}
goto Block_8;}

Block_7:
var_$88 = (uint32_t)1;
var_$89 = (int32_t)var_$8;
var_$93 = (uint64_t)4;
var_$94 = (uint64_t)var_$89;
var_$95 = var_$94*var_$93;
var_$96 = var_$2+var_$95;
*((uint32_t*)(var_$96)) = var_$88;
{
}
goto Block_9;

Block_8:
var_$104 = (uint32_t)0;
var_$105 = (int32_t)var_$8;
var_$109 = (uint64_t)4;
var_$110 = (uint64_t)var_$105;
var_$111 = var_$110*var_$109;
var_$112 = var_$2+var_$111;
*((uint32_t*)(var_$112)) = var_$104;
{
}
goto Block_9;

Block_2:
{
}
goto Block_9;

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
classify<<<dim3(var_$4,var_$6,var_$7),dim3(var_$8,var_$10,var_$11)>>>(var_$1,var_$2,var_$3);
cudaDeviceSynchronize();
return;

}

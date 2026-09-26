
#include <cstdint>
#include <cuda_runtime.h>

__global__ void classify(uint8_t* var_$1 ,uint8_t* var_$2 ,uint32_t var_$3 ){
uint32_t var_$6;
uint32_t var_$7;
uint32_t var_$8;
uint32_t var_$9;
uint32_t var_$10;
bool var_$11;
uint8_t* var_$5;
uint8_t* var_$4;
uint64_t var_$16;
uint64_t var_$17;
uint64_t var_$18;
uint8_t* var_$19;
uint32_t var_$22;
uint32_t var_$23;
bool var_$24;
uint32_t var_$26;
uint64_t var_$30;
uint64_t var_$31;
uint64_t var_$32;
uint8_t* var_$33;
uint64_t var_$47;
uint64_t var_$48;
uint64_t var_$49;
uint8_t* var_$50;
uint32_t var_$53;
uint32_t var_$54;
bool var_$55;
uint32_t var_$57;
uint64_t var_$61;
uint64_t var_$62;
uint64_t var_$63;
uint8_t* var_$64;
uint64_t var_$75;
uint64_t var_$76;
uint64_t var_$77;
uint8_t* var_$78;
uint32_t var_$81;
uint32_t var_$82;
bool var_$83;
uint32_t var_$85;
uint64_t var_$89;
uint64_t var_$90;
uint64_t var_$91;
uint8_t* var_$92;
uint32_t var_$100;
uint64_t var_$104;
uint64_t var_$105;
uint64_t var_$106;
uint8_t* var_$107;
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
var_$16 = (uint64_t)4;
var_$17 = (uint64_t)var_$10;
var_$18 = var_$17*var_$16;
var_$19 = var_$4+var_$18;
var_$22 = (uint32_t)200;
var_$23 = *((uint32_t*)(var_$19));
var_$24 = var_$23 > var_$22;
if (var_$24){
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
var_$26 = (uint32_t)3;
var_$30 = (uint64_t)4;
var_$31 = (uint64_t)var_$10;
var_$32 = var_$31*var_$30;
var_$33 = var_$5+var_$32;
*((uint32_t*)(var_$33)) = var_$26;
{
}
goto Block_9;

Block_9:
return;

Block_4:
var_$47 = (uint64_t)4;
var_$48 = (uint64_t)var_$10;
var_$49 = var_$48*var_$47;
var_$50 = var_$4+var_$49;
var_$53 = (uint32_t)100;
var_$54 = *((uint32_t*)(var_$50));
var_$55 = var_$54 > var_$53;
if (var_$55){
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
var_$57 = (uint32_t)2;
var_$61 = (uint64_t)4;
var_$62 = (uint64_t)var_$10;
var_$63 = var_$62*var_$61;
var_$64 = var_$5+var_$63;
*((uint32_t*)(var_$64)) = var_$57;
{
}
goto Block_9;

Block_6:
var_$75 = (uint64_t)4;
var_$76 = (uint64_t)var_$10;
var_$77 = var_$76*var_$75;
var_$78 = var_$4+var_$77;
var_$81 = (uint32_t)0;
var_$82 = *((uint32_t*)(var_$78));
var_$83 = var_$82 > var_$81;
if (var_$83){
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
var_$85 = (uint32_t)1;
var_$89 = (uint64_t)4;
var_$90 = (uint64_t)var_$10;
var_$91 = var_$90*var_$89;
var_$92 = var_$5+var_$91;
*((uint32_t*)(var_$92)) = var_$85;
{
}
goto Block_9;

Block_8:
var_$100 = (uint32_t)0;
var_$104 = (uint64_t)4;
var_$105 = (uint64_t)var_$10;
var_$106 = var_$105*var_$104;
var_$107 = var_$5+var_$106;
*((uint32_t*)(var_$107)) = var_$100;
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

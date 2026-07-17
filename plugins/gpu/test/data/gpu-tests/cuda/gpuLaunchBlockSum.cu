
#include <cstdint>
#include <cuda_runtime.h>

__global__ void blockSum(uint8_t* var_$1 ,uint8_t* var_$2 ,uint32_t var_$3 ){
uint64_t var_$4;
uint64_t var_$5;
__shared__ unsigned char nautilus_shared_6[1024];
uint8_t* var_$6;
uint32_t var_$7;
uint32_t var_$8;
uint32_t var_$9;
uint32_t var_$10;
uint32_t var_$11;
uint32_t var_$12;
bool var_$13;
int32_t var_$15;
uint64_t var_$19;
uint64_t var_$20;
uint64_t var_$21;
uint8_t* var_$22;
uint32_t var_$25;
int32_t var_$27;
uint64_t var_$31;
uint64_t var_$32;
uint64_t var_$33;
uint8_t* var_$34;
uint32_t var_$40;
uint32_t var_$41;
uint32_t var_$42;
uint32_t var_$43;
bool var_$44;
bool var_$46;
int32_t var_$48;
uint64_t var_$52;
uint64_t var_$53;
uint64_t var_$54;
uint8_t* var_$55;
uint32_t var_$58;
int32_t var_$59;
uint64_t var_$63;
uint64_t var_$64;
uint64_t var_$65;
uint8_t* var_$66;
uint32_t var_$69;
uint32_t var_$70;
uint32_t var_$71;
int32_t var_$72;
uint64_t var_$76;
uint64_t var_$77;
uint64_t var_$78;
uint8_t* var_$79;
uint32_t var_$85;
uint32_t var_$86;
uint32_t var_$87;
uint32_t var_$94;
bool var_$95;
int32_t var_$97;
uint64_t var_$101;
uint64_t var_$102;
uint64_t var_$103;
uint8_t* var_$104;
uint32_t var_$107;
uint32_t var_$109;
int32_t var_$110;
uint64_t var_$114;
uint64_t var_$115;
uint64_t var_$116;
uint8_t* var_$117;
Block_0:
var_$4 = (uint64_t)1024;
var_$5 = (uint64_t)4;
var_$6 = nautilus_shared_6;
var_$7 = threadIdx.x;
var_$8 = blockIdx.x;
var_$9 = blockDim.x;
var_$10 = var_$8*var_$9;
var_$11 = var_$10+var_$7;
var_$12 = (uint32_t)0;
var_$13 = var_$11 < var_$3;
if (var_$13){
{
uint8_t* temp_0 = var_$2;
uint8_t* temp_1 = var_$6;
uint32_t temp_2 = var_$7;
uint8_t* temp_3 = var_$1;
uint32_t temp_4 = var_$11;
var_$2 = temp_0;
var_$6 = temp_1;
var_$7 = temp_2;
var_$1 = temp_3;
var_$11 = temp_4;
}
goto Block_1;
}else{
{
uint8_t* temp_0 = var_$2;
uint8_t* temp_1 = var_$6;
uint32_t temp_2 = var_$7;
uint32_t temp_3 = var_$12;
var_$2 = temp_0;
var_$6 = temp_1;
var_$7 = temp_2;
var_$12 = temp_3;
}
goto Block_2;}

Block_1:
var_$15 = (int32_t)var_$11;
var_$19 = (uint64_t)4;
var_$20 = (uint64_t)var_$15;
var_$21 = var_$20*var_$19;
var_$22 = var_$1+var_$21;
var_$25 = *((uint32_t*)(var_$22));
{
uint8_t* temp_0 = var_$2;
uint8_t* temp_1 = var_$6;
uint32_t temp_2 = var_$7;
uint32_t temp_3 = var_$25;
var_$2 = temp_0;
var_$6 = temp_1;
var_$7 = temp_2;
var_$12 = temp_3;
}
goto Block_8;

Block_8:
var_$27 = (int32_t)var_$7;
var_$31 = (uint64_t)4;
var_$32 = (uint64_t)var_$27;
var_$33 = var_$32*var_$31;
var_$34 = var_$6+var_$33;
*((uint32_t*)(var_$34)) = var_$12;
__syncthreads();
var_$40 = blockDim.x;
var_$41 = (uint32_t)2;
var_$42 = var_$40/var_$41;
var_$43 = (uint32_t)0;
{
uint8_t* temp_0 = var_$2;
uint8_t* temp_1 = var_$6;
uint32_t temp_2 = var_$7;
uint32_t temp_3 = var_$42;
uint32_t temp_4 = var_$43;
var_$2 = temp_0;
var_$6 = temp_1;
var_$7 = temp_2;
var_$42 = temp_3;
var_$43 = temp_4;
}
goto Block_7;

Block_7:
var_$44 = var_$42 > var_$43;
if (var_$44){
{
uint8_t* temp_0 = var_$2;
uint8_t* temp_1 = var_$6;
uint32_t temp_2 = var_$7;
uint32_t temp_3 = var_$42;
var_$2 = temp_0;
var_$6 = temp_1;
var_$7 = temp_2;
var_$42 = temp_3;
}
goto Block_3;
}else{
{
uint8_t* temp_0 = var_$2;
uint8_t* temp_1 = var_$6;
uint32_t temp_2 = var_$7;
var_$2 = temp_0;
var_$6 = temp_1;
var_$7 = temp_2;
}
goto Block_4;}

Block_3:
var_$46 = var_$7 < var_$42;
if (var_$46){
{
uint8_t* temp_0 = var_$2;
uint8_t* temp_1 = var_$6;
uint32_t temp_2 = var_$7;
uint32_t temp_3 = var_$42;
var_$2 = temp_0;
var_$6 = temp_1;
var_$7 = temp_2;
var_$42 = temp_3;
}
goto Block_5;
}else{
{
uint8_t* temp_0 = var_$2;
uint8_t* temp_1 = var_$6;
uint32_t temp_2 = var_$7;
uint32_t temp_3 = var_$42;
var_$2 = temp_0;
var_$6 = temp_1;
var_$7 = temp_2;
var_$42 = temp_3;
}
goto Block_6;}

Block_5:
var_$48 = (int32_t)var_$7;
var_$52 = (uint64_t)4;
var_$53 = (uint64_t)var_$48;
var_$54 = var_$53*var_$52;
var_$55 = var_$6+var_$54;
var_$58 = var_$7+var_$42;
var_$59 = (int32_t)var_$58;
var_$63 = (uint64_t)4;
var_$64 = (uint64_t)var_$59;
var_$65 = var_$64*var_$63;
var_$66 = var_$6+var_$65;
var_$69 = *((uint32_t*)(var_$55));
var_$70 = *((uint32_t*)(var_$66));
var_$71 = var_$69+var_$70;
var_$72 = (int32_t)var_$7;
var_$76 = (uint64_t)4;
var_$77 = (uint64_t)var_$72;
var_$78 = var_$77*var_$76;
var_$79 = var_$6+var_$78;
*((uint32_t*)(var_$79)) = var_$71;
{
uint8_t* temp_0 = var_$2;
uint8_t* temp_1 = var_$6;
uint32_t temp_2 = var_$7;
uint32_t temp_3 = var_$42;
var_$2 = temp_0;
var_$6 = temp_1;
var_$7 = temp_2;
var_$42 = temp_3;
}
goto Block_11;

Block_11:
__syncthreads();
var_$85 = (uint32_t)2;
var_$86 = var_$42/var_$85;
var_$87 = (uint32_t)0;
{
uint8_t* temp_0 = var_$2;
uint8_t* temp_1 = var_$6;
uint32_t temp_2 = var_$7;
uint32_t temp_3 = var_$86;
uint32_t temp_4 = var_$87;
var_$2 = temp_0;
var_$6 = temp_1;
var_$7 = temp_2;
var_$42 = temp_3;
var_$43 = temp_4;
}
goto Block_7;

Block_6:
{
uint8_t* temp_0 = var_$2;
uint8_t* temp_1 = var_$6;
uint32_t temp_2 = var_$7;
uint32_t temp_3 = var_$42;
var_$2 = temp_0;
var_$6 = temp_1;
var_$7 = temp_2;
var_$42 = temp_3;
}
goto Block_11;

Block_4:
var_$94 = (uint32_t)0;
var_$95 = var_$7 == var_$94;
if (var_$95){
{
uint8_t* temp_0 = var_$2;
uint8_t* temp_1 = var_$6;
var_$2 = temp_0;
var_$6 = temp_1;
}
goto Block_9;
}else{
{
}
goto Block_10;}

Block_9:
var_$97 = (int32_t)0;
var_$101 = (uint64_t)4;
var_$102 = (uint64_t)var_$97;
var_$103 = var_$102*var_$101;
var_$104 = var_$6+var_$103;
var_$107 = *((uint32_t*)(var_$104));
var_$109 = blockIdx.x;
var_$110 = (int32_t)var_$109;
var_$114 = (uint64_t)4;
var_$115 = (uint64_t)var_$110;
var_$116 = var_$115*var_$114;
var_$117 = var_$2+var_$116;
*((uint32_t*)(var_$117)) = var_$107;
{
}
goto Block_12;

Block_12:
return;

Block_10:
{
}
goto Block_12;

Block_2:
{
uint8_t* temp_0 = var_$2;
uint8_t* temp_1 = var_$6;
uint32_t temp_2 = var_$7;
uint32_t temp_3 = var_$12;
var_$2 = temp_0;
var_$6 = temp_1;
var_$7 = temp_2;
var_$12 = temp_3;
}
goto Block_8;

}

extern "C" void execute(uint8_t* var_$1 ,uint8_t* var_$2 ,uint32_t var_$3 ){
uint32_t var_$4;
uint32_t var_$5;
uint32_t var_$6;
uint32_t var_$7;
uint32_t var_$10;
uint32_t var_$11;
uint32_t var_$12;
uint32_t var_$14;
uint32_t var_$15;
Block_0:
var_$4 = (uint32_t)255;
var_$5 = var_$3+var_$4;
var_$6 = (uint32_t)256;
var_$7 = var_$5/var_$6;
var_$10 = (uint32_t)1;
var_$11 = (uint32_t)1;
var_$12 = (uint32_t)256;
var_$14 = (uint32_t)1;
var_$15 = (uint32_t)1;
blockSum<<<dim3(var_$7,var_$10,var_$11),dim3(var_$12,var_$14,var_$15)>>>(var_$1,var_$2,var_$3);
cudaDeviceSynchronize();
return;

}

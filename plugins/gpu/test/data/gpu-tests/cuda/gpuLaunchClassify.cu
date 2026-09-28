
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
uint64_t var_$13;
uint64_t var_$14;
uint64_t var_$15;
uint8_t* var_$16;
uint32_t var_$17;
uint32_t var_$18;
bool var_$19;
uint32_t var_$21;
uint64_t var_$22;
uint64_t var_$23;
uint64_t var_$24;
uint8_t* var_$25;
uint64_t var_$33;
uint64_t var_$34;
uint64_t var_$35;
uint8_t* var_$36;
uint32_t var_$37;
uint32_t var_$38;
bool var_$39;
uint32_t var_$41;
uint64_t var_$42;
uint64_t var_$43;
uint64_t var_$44;
uint8_t* var_$45;
uint64_t var_$50;
uint64_t var_$51;
uint64_t var_$52;
uint8_t* var_$53;
uint32_t var_$54;
uint32_t var_$55;
bool var_$56;
uint32_t var_$58;
uint64_t var_$59;
uint64_t var_$60;
uint64_t var_$61;
uint8_t* var_$62;
uint32_t var_$67;
uint64_t var_$68;
uint64_t var_$69;
uint64_t var_$70;
uint8_t* var_$71;
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
var_$13 = (uint64_t)4;
var_$14 = (uint64_t)var_$10;
var_$15 = var_$14*var_$13;
var_$16 = var_$4+var_$15;
var_$17 = (uint32_t)200;
var_$18 = *((uint32_t*)(var_$16));
var_$19 = var_$18 > var_$17;
if (var_$19){
{
uint32_t temp_0 = var_$10;
uint8_t* temp_1 = var_$5;
var_$10 = temp_0;
var_$5 = temp_1;
}
goto Block_3;
}else{
{
uint32_t temp_0 = var_$10;
uint8_t* temp_1 = var_$5;
uint8_t* temp_2 = var_$4;
var_$10 = temp_0;
var_$5 = temp_1;
var_$4 = temp_2;
}
goto Block_4;}

Block_3:
var_$21 = (uint32_t)3;
var_$22 = (uint64_t)4;
var_$23 = (uint64_t)var_$10;
var_$24 = var_$23*var_$22;
var_$25 = var_$5+var_$24;
*((uint32_t*)(var_$25)) = var_$21;
{
}
goto Block_9;

Block_9:
return;

Block_4:
var_$33 = (uint64_t)4;
var_$34 = (uint64_t)var_$10;
var_$35 = var_$34*var_$33;
var_$36 = var_$4+var_$35;
var_$37 = (uint32_t)100;
var_$38 = *((uint32_t*)(var_$36));
var_$39 = var_$38 > var_$37;
if (var_$39){
{
uint32_t temp_0 = var_$10;
uint8_t* temp_1 = var_$5;
var_$10 = temp_0;
var_$5 = temp_1;
}
goto Block_5;
}else{
{
uint32_t temp_0 = var_$10;
uint8_t* temp_1 = var_$5;
uint8_t* temp_2 = var_$4;
var_$10 = temp_0;
var_$5 = temp_1;
var_$4 = temp_2;
}
goto Block_6;}

Block_5:
var_$41 = (uint32_t)2;
var_$42 = (uint64_t)4;
var_$43 = (uint64_t)var_$10;
var_$44 = var_$43*var_$42;
var_$45 = var_$5+var_$44;
*((uint32_t*)(var_$45)) = var_$41;
{
}
goto Block_9;

Block_6:
var_$50 = (uint64_t)4;
var_$51 = (uint64_t)var_$10;
var_$52 = var_$51*var_$50;
var_$53 = var_$4+var_$52;
var_$54 = (uint32_t)0;
var_$55 = *((uint32_t*)(var_$53));
var_$56 = var_$55 > var_$54;
if (var_$56){
{
uint32_t temp_0 = var_$10;
uint8_t* temp_1 = var_$5;
var_$10 = temp_0;
var_$5 = temp_1;
}
goto Block_7;
}else{
{
uint32_t temp_0 = var_$10;
uint8_t* temp_1 = var_$5;
var_$10 = temp_0;
var_$5 = temp_1;
}
goto Block_8;}

Block_7:
var_$58 = (uint32_t)1;
var_$59 = (uint64_t)4;
var_$60 = (uint64_t)var_$10;
var_$61 = var_$60*var_$59;
var_$62 = var_$5+var_$61;
*((uint32_t*)(var_$62)) = var_$58;
{
}
goto Block_9;

Block_8:
var_$67 = (uint32_t)0;
var_$68 = (uint64_t)4;
var_$69 = (uint64_t)var_$10;
var_$70 = var_$69*var_$68;
var_$71 = var_$5+var_$70;
*((uint32_t*)(var_$71)) = var_$67;
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

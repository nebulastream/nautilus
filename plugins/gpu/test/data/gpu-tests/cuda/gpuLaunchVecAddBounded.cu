
#include <cstdint>
#include <cuda_runtime.h>

__global__ void vecAddBounded(uint8_t* var_$1 ,uint8_t* var_$2 ,uint8_t* var_$3 ,uint32_t var_$4 ){
uint32_t var_$8;
uint32_t var_$9;
uint32_t var_$10;
uint32_t var_$11;
uint32_t var_$12;
bool var_$13;
uint8_t* var_$7;
uint8_t* var_$6;
uint8_t* var_$5;
int32_t var_$15;
uint64_t var_$19;
uint64_t var_$20;
uint64_t var_$21;
uint8_t* var_$22;
int32_t var_$25;
uint64_t var_$29;
uint64_t var_$30;
uint64_t var_$31;
uint8_t* var_$32;
uint32_t var_$35;
uint32_t var_$36;
uint32_t var_$37;
int32_t var_$38;
uint64_t var_$42;
uint64_t var_$43;
uint64_t var_$44;
uint8_t* var_$45;
Block_0:
var_$8 = blockIdx.x;
var_$9 = blockDim.x;
var_$10 = var_$8*var_$9;
var_$11 = threadIdx.x;
var_$12 = var_$10+var_$11;
var_$13 = var_$12 < var_$4;
if (var_$13){
{
uint8_t* temp_0 = var_$3;
uint32_t temp_1 = var_$12;
uint8_t* temp_2 = var_$2;
uint8_t* temp_3 = var_$1;
var_$7 = temp_0;
var_$12 = temp_1;
var_$6 = temp_2;
var_$5 = temp_3;
}
goto Block_1;
}else{
{
}
goto Block_2;}

Block_1:
var_$15 = (int32_t)var_$12;
var_$19 = (uint64_t)4;
var_$20 = (uint64_t)var_$15;
var_$21 = var_$20*var_$19;
var_$22 = var_$5+var_$21;
var_$25 = (int32_t)var_$12;
var_$29 = (uint64_t)4;
var_$30 = (uint64_t)var_$25;
var_$31 = var_$30*var_$29;
var_$32 = var_$6+var_$31;
var_$35 = *((uint32_t*)(var_$22));
var_$36 = *((uint32_t*)(var_$32));
var_$37 = var_$35+var_$36;
var_$38 = (int32_t)var_$12;
var_$42 = (uint64_t)4;
var_$43 = (uint64_t)var_$38;
var_$44 = var_$43*var_$42;
var_$45 = var_$7+var_$44;
*((uint32_t*)(var_$45)) = var_$37;
{
}
goto Block_3;

Block_3:
return;

Block_2:
{
}
goto Block_3;

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
vecAddBounded<<<dim3(var_$8,var_$11,var_$12),dim3(var_$13,var_$15,var_$16)>>>(var_$1,var_$2,var_$3,var_$4);
cudaDeviceSynchronize();
return;

}

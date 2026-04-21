#include <metal_stdlib>
using namespace metal;

kernel void vecAddBounded(
    device uchar* var_$1 [[buffer(0)]],
    device uchar* var_$2 [[buffer(1)]],
    device uchar* var_$3 [[buffer(2)]],
    constant uint& var_$4 [[buffer(3)]],
    uint3 nautilus_threadIdx [[thread_position_in_threadgroup]],
    uint3 nautilus_blockIdx [[threadgroup_position_in_grid]],
    uint3 nautilus_blockDim [[threads_per_threadgroup]],
    uint3 nautilus_gridDim [[threadgroups_per_grid]]
) {
uint var_$8;
uint var_$9;
uint var_$10;
uint var_$11;
uint var_$12;
bool var_$13;
device uchar* var_$7;
device uchar* var_$6;
device uchar* var_$5;
int var_$15;
ulong var_$19;
ulong var_$20;
ulong var_$21;
device uchar* var_$22;
int var_$25;
ulong var_$29;
ulong var_$30;
ulong var_$31;
device uchar* var_$32;
uint var_$35;
uint var_$36;
uint var_$37;
int var_$38;
ulong var_$42;
ulong var_$43;
ulong var_$44;
device uchar* var_$45;
int __pc = 0;
while (true) {
switch (__pc) {
case 0: {
var_$8 = nautilus_blockIdx.x;
var_$9 = nautilus_blockDim.x;
var_$10 = var_$8*var_$9;
var_$11 = nautilus_threadIdx.x;
var_$12 = var_$10+var_$11;
var_$13 = var_$12 < var_$4;
if (var_$13){
{
device uchar* temp_0 = var_$3;
uint temp_1 = var_$12;
device uchar* temp_2 = var_$2;
device uchar* temp_3 = var_$1;
var_$7 = temp_0;
var_$12 = temp_1;
var_$6 = temp_2;
var_$5 = temp_3;
}
__pc = 1; continue;
}else{
{
}
__pc = 3; continue;}
}
case 1: {
var_$15 = (int)var_$12;
var_$19 = (ulong)4;
var_$20 = (ulong)var_$15;
var_$21 = var_$20*var_$19;
var_$22 = var_$5+var_$21;
var_$25 = (int)var_$12;
var_$29 = (ulong)4;
var_$30 = (ulong)var_$25;
var_$31 = var_$30*var_$29;
var_$32 = var_$6+var_$31;
var_$35 = *((device uint*)(var_$22));
var_$36 = *((device uint*)(var_$32));
var_$37 = var_$35+var_$36;
var_$38 = (int)var_$12;
var_$42 = (ulong)4;
var_$43 = (ulong)var_$38;
var_$44 = var_$43*var_$42;
var_$45 = var_$7+var_$44;
*((device uint*)(var_$45)) = var_$37;
{
}
__pc = 2; continue;
}
case 2: {
return;
}
case 3: {
{
}
__pc = 2; continue;
}
}
}
}


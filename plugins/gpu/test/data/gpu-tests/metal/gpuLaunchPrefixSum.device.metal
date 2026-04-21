#include <metal_stdlib>
using namespace metal;

kernel void prefixSum(
    device uchar* var_$1 [[buffer(0)]],
    device uchar* var_$2 [[buffer(1)]],
    constant uint& var_$3 [[buffer(2)]],
    uint3 nautilus_threadIdx [[thread_position_in_threadgroup]],
    uint3 nautilus_blockIdx [[threadgroup_position_in_grid]],
    uint3 nautilus_blockDim [[threads_per_threadgroup]],
    uint3 nautilus_gridDim [[threadgroups_per_grid]]
) {
uint var_$6;
uint var_$7;
uint var_$8;
uint var_$9;
uint var_$10;
bool var_$11;
device uchar* var_$5;
device uchar* var_$4;
uint var_$13;
uint var_$14;
bool var_$15;
int var_$17;
ulong var_$21;
ulong var_$22;
ulong var_$23;
device uchar* var_$24;
uint var_$27;
uint var_$28;
uint var_$29;
uint var_$30;
int var_$38;
ulong var_$42;
ulong var_$43;
ulong var_$44;
device uchar* var_$45;
int __pc = 0;
while (true) {
switch (__pc) {
case 0: {
var_$6 = nautilus_blockIdx.x;
var_$7 = nautilus_blockDim.x;
var_$8 = var_$6*var_$7;
var_$9 = nautilus_threadIdx.x;
var_$10 = var_$8+var_$9;
var_$11 = var_$10 < var_$3;
if (var_$11){
{
device uchar* temp_0 = var_$2;
uint temp_1 = var_$10;
device uchar* temp_2 = var_$1;
var_$5 = temp_0;
var_$10 = temp_1;
var_$4 = temp_2;
}
__pc = 1; continue;
}else{
{
}
__pc = 6; continue;}
}
case 1: {
var_$13 = (uint)0;
var_$14 = (uint)0;
{
device uchar* temp_0 = var_$5;
uint temp_1 = var_$10;
uint temp_2 = var_$13;
uint temp_3 = var_$14;
device uchar* temp_4 = var_$4;
var_$5 = temp_0;
var_$10 = temp_1;
var_$13 = temp_2;
var_$14 = temp_3;
var_$4 = temp_4;
}
__pc = 2; continue;
}
case 2: {
var_$15 = var_$14 <= var_$10;
if (var_$15){
{
device uchar* temp_0 = var_$5;
uint temp_1 = var_$10;
uint temp_2 = var_$14;
uint temp_3 = var_$13;
device uchar* temp_4 = var_$4;
var_$5 = temp_0;
var_$10 = temp_1;
var_$14 = temp_2;
var_$13 = temp_3;
var_$4 = temp_4;
}
__pc = 3; continue;
}else{
{
device uchar* temp_0 = var_$5;
uint temp_1 = var_$10;
uint temp_2 = var_$13;
var_$5 = temp_0;
var_$10 = temp_1;
var_$13 = temp_2;
}
__pc = 4; continue;}
}
case 3: {
var_$17 = (int)var_$14;
var_$21 = (ulong)4;
var_$22 = (ulong)var_$17;
var_$23 = var_$22*var_$21;
var_$24 = var_$4+var_$23;
var_$27 = *((device uint*)(var_$24));
var_$28 = var_$13+var_$27;
var_$29 = (uint)1;
var_$30 = var_$14+var_$29;
{
device uchar* temp_0 = var_$5;
uint temp_1 = var_$10;
uint temp_2 = var_$28;
uint temp_3 = var_$30;
device uchar* temp_4 = var_$4;
var_$5 = temp_0;
var_$10 = temp_1;
var_$13 = temp_2;
var_$14 = temp_3;
var_$4 = temp_4;
}
__pc = 2; continue;
}
case 4: {
var_$38 = (int)var_$10;
var_$42 = (ulong)4;
var_$43 = (ulong)var_$38;
var_$44 = var_$43*var_$42;
var_$45 = var_$5+var_$44;
*((device uint*)(var_$45)) = var_$13;
{
}
__pc = 5; continue;
}
case 5: {
return;
}
case 6: {
{
}
__pc = 5; continue;
}
}
}
}


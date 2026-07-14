#include <metal_stdlib>
using namespace metal;

kernel void prefixSum(
    device uchar* var_$1 [[buffer(0)]],
    device uchar* var_$2 [[buffer(1)]],
    constant uint& var_$3_arg [[buffer(2)]],
    uint3 nautilus_threadIdx [[thread_position_in_threadgroup]],
    uint3 nautilus_blockIdx [[threadgroup_position_in_grid]],
    uint3 nautilus_blockDim [[threads_per_threadgroup]],
    uint3 nautilus_gridDim [[threadgroups_per_grid]]
) {
uint var_$3;
uint var_$4;
uint var_$5;
uint var_$6;
uint var_$7;
uint var_$8;
bool var_$9;
uint var_$11;
uint var_$12;
bool var_$13;
int var_$15;
ulong var_$19;
ulong var_$20;
ulong var_$21;
device uchar* var_$22;
uint var_$25;
uint var_$26;
uint var_$27;
uint var_$28;
int var_$36;
ulong var_$40;
ulong var_$41;
ulong var_$42;
device uchar* var_$43;
var_$3 = var_$3_arg;
int __pc = 0;
while (true) {
switch (__pc) {
case 0: {
var_$4 = nautilus_blockIdx.x;
var_$5 = nautilus_blockDim.x;
var_$6 = var_$4*var_$5;
var_$7 = nautilus_threadIdx.x;
var_$8 = var_$6+var_$7;
var_$9 = var_$8 < var_$3;
if (var_$9){
{
device uchar* temp_0 = var_$2;
uint temp_1 = var_$8;
device uchar* temp_2 = var_$1;
var_$2 = temp_0;
var_$8 = temp_1;
var_$1 = temp_2;
}
__pc = 1; continue;
}else{
{
}
__pc = 6; continue;}
}
case 1: {
var_$11 = (uint)0;
var_$12 = (uint)0;
{
device uchar* temp_0 = var_$2;
uint temp_1 = var_$8;
uint temp_2 = var_$11;
uint temp_3 = var_$12;
device uchar* temp_4 = var_$1;
var_$2 = temp_0;
var_$8 = temp_1;
var_$11 = temp_2;
var_$12 = temp_3;
var_$1 = temp_4;
}
__pc = 2; continue;
}
case 2: {
var_$13 = var_$12 <= var_$8;
if (var_$13){
{
device uchar* temp_0 = var_$2;
uint temp_1 = var_$8;
uint temp_2 = var_$12;
uint temp_3 = var_$11;
device uchar* temp_4 = var_$1;
var_$2 = temp_0;
var_$8 = temp_1;
var_$12 = temp_2;
var_$11 = temp_3;
var_$1 = temp_4;
}
__pc = 3; continue;
}else{
{
device uchar* temp_0 = var_$2;
uint temp_1 = var_$8;
uint temp_2 = var_$11;
var_$2 = temp_0;
var_$8 = temp_1;
var_$11 = temp_2;
}
__pc = 4; continue;}
}
case 3: {
var_$15 = (int)var_$12;
var_$19 = (ulong)4;
var_$20 = (ulong)var_$15;
var_$21 = var_$20*var_$19;
var_$22 = var_$1+var_$21;
var_$25 = *((device uint*)(var_$22));
var_$26 = var_$11+var_$25;
var_$27 = (uint)1;
var_$28 = var_$12+var_$27;
{
device uchar* temp_0 = var_$2;
uint temp_1 = var_$8;
uint temp_2 = var_$26;
uint temp_3 = var_$28;
device uchar* temp_4 = var_$1;
var_$2 = temp_0;
var_$8 = temp_1;
var_$11 = temp_2;
var_$12 = temp_3;
var_$1 = temp_4;
}
__pc = 2; continue;
}
case 4: {
var_$36 = (int)var_$8;
var_$40 = (ulong)4;
var_$41 = (ulong)var_$36;
var_$42 = var_$41*var_$40;
var_$43 = var_$2+var_$42;
*((device uint*)(var_$43)) = var_$11;
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


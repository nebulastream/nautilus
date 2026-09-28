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
ulong var_$17;
ulong var_$18;
ulong var_$19;
device uchar* var_$20;
uint var_$21;
uint var_$22;
uint var_$23;
uint var_$24;
ulong var_$32;
ulong var_$33;
ulong var_$34;
device uchar* var_$35;
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
uint temp_0 = var_$10;
device uchar* temp_1 = var_$2;
device uchar* temp_2 = var_$1;
var_$10 = temp_0;
var_$5 = temp_1;
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
uint temp_0 = var_$13;
uint temp_1 = var_$10;
device uchar* temp_2 = var_$5;
uint temp_3 = var_$14;
device uchar* temp_4 = var_$4;
var_$13 = temp_0;
var_$10 = temp_1;
var_$5 = temp_2;
var_$14 = temp_3;
var_$4 = temp_4;
}
__pc = 2; continue;
}
case 2: {
var_$15 = var_$14 <= var_$10;
if (var_$15){
{
uint temp_0 = var_$10;
device uchar* temp_1 = var_$5;
uint temp_2 = var_$14;
device uchar* temp_3 = var_$4;
uint temp_4 = var_$13;
var_$10 = temp_0;
var_$5 = temp_1;
var_$14 = temp_2;
var_$4 = temp_3;
var_$13 = temp_4;
}
__pc = 3; continue;
}else{
{
uint temp_0 = var_$13;
uint temp_1 = var_$10;
device uchar* temp_2 = var_$5;
var_$13 = temp_0;
var_$10 = temp_1;
var_$5 = temp_2;
}
__pc = 4; continue;}
}
case 3: {
var_$17 = (ulong)4;
var_$18 = (ulong)var_$14;
var_$19 = var_$18*var_$17;
var_$20 = var_$4+var_$19;
var_$21 = *((device uint*)(var_$20));
var_$22 = var_$13+var_$21;
var_$23 = (uint)1;
var_$24 = var_$14+var_$23;
{
uint temp_0 = var_$22;
uint temp_1 = var_$10;
device uchar* temp_2 = var_$5;
uint temp_3 = var_$24;
device uchar* temp_4 = var_$4;
var_$13 = temp_0;
var_$10 = temp_1;
var_$5 = temp_2;
var_$14 = temp_3;
var_$4 = temp_4;
}
__pc = 2; continue;
}
case 4: {
var_$32 = (ulong)4;
var_$33 = (ulong)var_$10;
var_$34 = var_$33*var_$32;
var_$35 = var_$5+var_$34;
*((device uint*)(var_$35)) = var_$13;
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


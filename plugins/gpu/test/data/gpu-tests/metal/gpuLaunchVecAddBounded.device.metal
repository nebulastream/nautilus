#include <metal_stdlib>
using namespace metal;

kernel void vecAddBounded(
    device uchar* var_$1 [[buffer(0)]],
    device uchar* var_$2 [[buffer(1)]],
    device uchar* var_$3 [[buffer(2)]],
    constant uint& var_$4_arg [[buffer(3)]],
    uint3 nautilus_threadIdx [[thread_position_in_threadgroup]],
    uint3 nautilus_blockIdx [[threadgroup_position_in_grid]],
    uint3 nautilus_blockDim [[threads_per_threadgroup]],
    uint3 nautilus_gridDim [[threadgroups_per_grid]]
) {
uint var_$4;
uint var_$5;
uint var_$6;
uint var_$7;
uint var_$8;
uint var_$9;
bool var_$10;
int var_$12;
ulong var_$16;
ulong var_$17;
ulong var_$18;
device uchar* var_$19;
int var_$22;
ulong var_$26;
ulong var_$27;
ulong var_$28;
device uchar* var_$29;
uint var_$32;
uint var_$33;
uint var_$34;
int var_$35;
ulong var_$39;
ulong var_$40;
ulong var_$41;
device uchar* var_$42;
var_$4 = var_$4_arg;
int __pc = 0;
while (true) {
switch (__pc) {
case 0: {
var_$5 = nautilus_blockIdx.x;
var_$6 = nautilus_blockDim.x;
var_$7 = var_$5*var_$6;
var_$8 = nautilus_threadIdx.x;
var_$9 = var_$7+var_$8;
var_$10 = var_$9 < var_$4;
if (var_$10){
{
device uchar* temp_0 = var_$3;
uint temp_1 = var_$9;
device uchar* temp_2 = var_$2;
device uchar* temp_3 = var_$1;
var_$3 = temp_0;
var_$9 = temp_1;
var_$2 = temp_2;
var_$1 = temp_3;
}
__pc = 1; continue;
}else{
{
}
__pc = 3; continue;}
}
case 1: {
var_$12 = (int)var_$9;
var_$16 = (ulong)4;
var_$17 = (ulong)var_$12;
var_$18 = var_$17*var_$16;
var_$19 = var_$1+var_$18;
var_$22 = (int)var_$9;
var_$26 = (ulong)4;
var_$27 = (ulong)var_$22;
var_$28 = var_$27*var_$26;
var_$29 = var_$2+var_$28;
var_$32 = *((device uint*)(var_$19));
var_$33 = *((device uint*)(var_$29));
var_$34 = var_$32+var_$33;
var_$35 = (int)var_$9;
var_$39 = (ulong)4;
var_$40 = (ulong)var_$35;
var_$41 = var_$40*var_$39;
var_$42 = var_$3+var_$41;
*((device uint*)(var_$42)) = var_$34;
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


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
device uchar* var_$5;
device uchar* var_$6;
device uchar* var_$7;
ulong var_$15;
ulong var_$16;
ulong var_$17;
device uchar* var_$18;
ulong var_$19;
ulong var_$20;
ulong var_$21;
device uchar* var_$22;
uint var_$23;
uint var_$24;
uint var_$25;
ulong var_$26;
ulong var_$27;
ulong var_$28;
device uchar* var_$29;
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
uint temp_0 = var_$12;
device uchar* temp_1 = var_$1;
device uchar* temp_2 = var_$2;
device uchar* temp_3 = var_$3;
var_$12 = temp_0;
var_$5 = temp_1;
var_$6 = temp_2;
var_$7 = temp_3;
}
__pc = 1; continue;
}else{
{
}
__pc = 3; continue;}
}
case 1: {
var_$15 = (ulong)4;
var_$16 = (ulong)var_$12;
var_$17 = var_$16*var_$15;
var_$18 = var_$5+var_$17;
var_$19 = (ulong)4;
var_$20 = (ulong)var_$12;
var_$21 = var_$20*var_$19;
var_$22 = var_$6+var_$21;
var_$23 = *((device uint*)(var_$18));
var_$24 = *((device uint*)(var_$22));
var_$25 = var_$23+var_$24;
var_$26 = (ulong)4;
var_$27 = (ulong)var_$12;
var_$28 = var_$27*var_$26;
var_$29 = var_$7+var_$28;
*((device uint*)(var_$29)) = var_$25;
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


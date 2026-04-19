#include <metal_stdlib>
using namespace metal;

kernel void execute(
    constant uint& var_$1 [[buffer(0)]],
    uint3 nautilus_threadIdx [[thread_position_in_threadgroup]],
    uint3 nautilus_blockIdx [[threadgroup_position_in_grid]],
    uint3 nautilus_blockDim [[threads_per_threadgroup]],
    uint3 nautilus_gridDim [[threadgroups_per_grid]]
) {
uint var_$2;
uint var_$3;
uint var_$4;
uint var_$5;
uint var_$6;
bool var_$7;
uint var_$10;
int __pc = 0;
while (true) {
switch (__pc) {
case 0: {
var_$2 = nautilus_blockIdx.x;
var_$3 = nautilus_blockDim.x;
var_$4 = var_$2*var_$3;
var_$5 = nautilus_threadIdx.x;
var_$6 = var_$4+var_$5;
var_$7 = var_$6 < var_$1;
if (var_$7){
{
uint temp_0 = var_$6;
var_$6 = temp_0;
}
__pc = 1; continue;
}else{
{
}
__pc = 3; continue;}
}
case 1: {
{
uint temp_0 = var_$6;
var_$6 = temp_0;
}
__pc = 2; continue;
}
case 2: {
return var_$6;
}
case 3: {
var_$10 = (uint)0;
{
uint temp_0 = var_$10;
var_$6 = temp_0;
}
__pc = 2; continue;
}
}
}
}


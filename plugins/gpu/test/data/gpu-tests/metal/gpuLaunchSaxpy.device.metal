#include <metal_stdlib>
using namespace metal;

kernel void saxpy(
    device uchar* var_$1 [[buffer(0)]],
    device uchar* var_$2 [[buffer(1)]],
    constant float& var_$3 [[buffer(2)]],
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
ulong var_$14;
ulong var_$15;
ulong var_$16;
device uchar* var_$17;
float var_$20;
float var_$21;
ulong var_$25;
ulong var_$26;
ulong var_$27;
device uchar* var_$28;
float var_$31;
float var_$32;
ulong var_$36;
ulong var_$37;
ulong var_$38;
device uchar* var_$39;
var_$6 = nautilus_blockIdx.x;
var_$7 = nautilus_blockDim.x;
var_$8 = var_$6*var_$7;
var_$9 = nautilus_threadIdx.x;
var_$10 = var_$8+var_$9;
var_$14 = (ulong)4;
var_$15 = (ulong)var_$10;
var_$16 = var_$15*var_$14;
var_$17 = var_$1+var_$16;
var_$20 = *((device float*)(var_$17));
var_$21 = var_$3*var_$20;
var_$25 = (ulong)4;
var_$26 = (ulong)var_$10;
var_$27 = var_$26*var_$25;
var_$28 = var_$2+var_$27;
var_$31 = *((device float*)(var_$28));
var_$32 = var_$21+var_$31;
var_$36 = (ulong)4;
var_$37 = (ulong)var_$10;
var_$38 = var_$37*var_$36;
var_$39 = var_$2+var_$38;
*((device float*)(var_$39)) = var_$32;
return;

}


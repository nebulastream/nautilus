#include <metal_stdlib>
using namespace metal;

kernel void vecScale(
    device uchar* var_$1 [[buffer(0)]],
    device uchar* var_$2 [[buffer(1)]],
    constant uint& var_$3 [[buffer(2)]],
    uint3 nautilus_threadIdx [[thread_position_in_threadgroup]],
    uint3 nautilus_blockIdx [[threadgroup_position_in_grid]],
    uint3 nautilus_blockDim [[threads_per_threadgroup]],
    uint3 nautilus_gridDim [[threadgroups_per_grid]]
) {
uint var_$6;
ulong var_$7;
ulong var_$8;
ulong var_$9;
device uchar* var_$10;
uint var_$11;
uint var_$12;
ulong var_$13;
ulong var_$14;
ulong var_$15;
device uchar* var_$16;
var_$6 = nautilus_threadIdx.x;
var_$7 = (ulong)4;
var_$8 = (ulong)var_$6;
var_$9 = var_$8*var_$7;
var_$10 = var_$1+var_$9;
var_$11 = *((device uint*)(var_$10));
var_$12 = var_$11*var_$3;
var_$13 = (ulong)4;
var_$14 = (ulong)var_$6;
var_$15 = var_$14*var_$13;
var_$16 = var_$2+var_$15;
*((device uint*)(var_$16)) = var_$12;
return;

}


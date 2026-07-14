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
uint var_$4;
int var_$5;
ulong var_$9;
ulong var_$10;
ulong var_$11;
device uchar* var_$12;
uint var_$16;
uint var_$17;
int var_$18;
ulong var_$22;
ulong var_$23;
ulong var_$24;
device uchar* var_$25;
var_$4 = nautilus_threadIdx.x;
var_$5 = (int)var_$4;
var_$9 = (ulong)4;
var_$10 = (ulong)var_$5;
var_$11 = var_$10*var_$9;
var_$12 = var_$1+var_$11;
var_$16 = *((device uint*)(var_$12));
var_$17 = var_$16*var_$3;
var_$18 = (int)var_$4;
var_$22 = (ulong)4;
var_$23 = (ulong)var_$18;
var_$24 = var_$23*var_$22;
var_$25 = var_$2+var_$24;
*((device uint*)(var_$25)) = var_$17;
return;

}


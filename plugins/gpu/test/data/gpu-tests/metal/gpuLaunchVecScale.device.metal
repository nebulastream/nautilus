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
int var_$7;
ulong var_$11;
ulong var_$12;
ulong var_$13;
device uchar* var_$14;
uint var_$18;
uint var_$19;
int var_$20;
ulong var_$24;
ulong var_$25;
ulong var_$26;
device uchar* var_$27;
var_$6 = nautilus_threadIdx.x;
var_$7 = (int)var_$6;
var_$11 = (ulong)4;
var_$12 = (ulong)var_$7;
var_$13 = var_$12*var_$11;
var_$14 = var_$1+var_$13;
var_$18 = *((device uint*)(var_$14));
var_$19 = var_$18*var_$3;
var_$20 = (int)var_$6;
var_$24 = (ulong)4;
var_$25 = (ulong)var_$20;
var_$26 = var_$25*var_$24;
var_$27 = var_$2+var_$26;
*((device uint*)(var_$27)) = var_$19;
return;

}


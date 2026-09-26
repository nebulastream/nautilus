#include <metal_stdlib>
using namespace metal;

kernel void vecAdd(
    device uchar* var_$1 [[buffer(0)]],
    device uchar* var_$2 [[buffer(1)]],
    device uchar* var_$3 [[buffer(2)]],
    uint3 nautilus_threadIdx [[thread_position_in_threadgroup]],
    uint3 nautilus_blockIdx [[threadgroup_position_in_grid]],
    uint3 nautilus_blockDim [[threads_per_threadgroup]],
    uint3 nautilus_gridDim [[threadgroups_per_grid]]
) {
uint var_$7;
ulong var_$11;
ulong var_$12;
ulong var_$13;
device uchar* var_$14;
ulong var_$20;
ulong var_$21;
ulong var_$22;
device uchar* var_$23;
uint var_$26;
uint var_$27;
uint var_$28;
ulong var_$32;
ulong var_$33;
ulong var_$34;
device uchar* var_$35;
var_$7 = nautilus_threadIdx.x;
var_$11 = (ulong)4;
var_$12 = (ulong)var_$7;
var_$13 = var_$12*var_$11;
var_$14 = var_$1+var_$13;
var_$20 = (ulong)4;
var_$21 = (ulong)var_$7;
var_$22 = var_$21*var_$20;
var_$23 = var_$2+var_$22;
var_$26 = *((device uint*)(var_$14));
var_$27 = *((device uint*)(var_$23));
var_$28 = var_$26+var_$27;
var_$32 = (ulong)4;
var_$33 = (ulong)var_$7;
var_$34 = var_$33*var_$32;
var_$35 = var_$3+var_$34;
*((device uint*)(var_$35)) = var_$28;
return;

}


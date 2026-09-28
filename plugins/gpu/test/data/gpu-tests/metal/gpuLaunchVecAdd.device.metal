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
ulong var_$8;
ulong var_$9;
ulong var_$10;
device uchar* var_$11;
ulong var_$12;
ulong var_$13;
ulong var_$14;
device uchar* var_$15;
uint var_$16;
uint var_$17;
uint var_$18;
ulong var_$19;
ulong var_$20;
ulong var_$21;
device uchar* var_$22;
var_$7 = nautilus_threadIdx.x;
var_$8 = (ulong)4;
var_$9 = (ulong)var_$7;
var_$10 = var_$9*var_$8;
var_$11 = var_$1+var_$10;
var_$12 = (ulong)4;
var_$13 = (ulong)var_$7;
var_$14 = var_$13*var_$12;
var_$15 = var_$2+var_$14;
var_$16 = *((device uint*)(var_$11));
var_$17 = *((device uint*)(var_$15));
var_$18 = var_$16+var_$17;
var_$19 = (ulong)4;
var_$20 = (ulong)var_$7;
var_$21 = var_$20*var_$19;
var_$22 = var_$3+var_$21;
*((device uint*)(var_$22)) = var_$18;
return;

}


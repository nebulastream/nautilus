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
int var_$8;
ulong var_$12;
ulong var_$13;
ulong var_$14;
device uchar* var_$15;
int var_$18;
ulong var_$22;
ulong var_$23;
ulong var_$24;
device uchar* var_$25;
uint var_$28;
uint var_$29;
uint var_$30;
int var_$31;
ulong var_$35;
ulong var_$36;
ulong var_$37;
device uchar* var_$38;
var_$7 = nautilus_threadIdx.x;
var_$8 = (int)var_$7;
var_$12 = (ulong)4;
var_$13 = (ulong)var_$8;
var_$14 = var_$13*var_$12;
var_$15 = var_$1+var_$14;
var_$18 = (int)var_$7;
var_$22 = (ulong)4;
var_$23 = (ulong)var_$18;
var_$24 = var_$23*var_$22;
var_$25 = var_$2+var_$24;
var_$28 = *((device uint*)(var_$15));
var_$29 = *((device uint*)(var_$25));
var_$30 = var_$28+var_$29;
var_$31 = (int)var_$7;
var_$35 = (ulong)4;
var_$36 = (ulong)var_$31;
var_$37 = var_$36*var_$35;
var_$38 = var_$3+var_$37;
*((device uint*)(var_$38)) = var_$30;
return;

}


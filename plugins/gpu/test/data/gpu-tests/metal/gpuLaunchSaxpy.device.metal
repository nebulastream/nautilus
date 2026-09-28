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
ulong var_$11;
ulong var_$12;
ulong var_$13;
device uchar* var_$14;
float var_$15;
float var_$16;
ulong var_$17;
ulong var_$18;
ulong var_$19;
device uchar* var_$20;
float var_$21;
float var_$22;
ulong var_$23;
ulong var_$24;
ulong var_$25;
device uchar* var_$26;
var_$6 = nautilus_blockIdx.x;
var_$7 = nautilus_blockDim.x;
var_$8 = var_$6*var_$7;
var_$9 = nautilus_threadIdx.x;
var_$10 = var_$8+var_$9;
var_$11 = (ulong)4;
var_$12 = (ulong)var_$10;
var_$13 = var_$12*var_$11;
var_$14 = var_$1+var_$13;
var_$15 = *((device float*)(var_$14));
var_$16 = var_$3*var_$15;
var_$17 = (ulong)4;
var_$18 = (ulong)var_$10;
var_$19 = var_$18*var_$17;
var_$20 = var_$2+var_$19;
var_$21 = *((device float*)(var_$20));
var_$22 = var_$16+var_$21;
var_$23 = (ulong)4;
var_$24 = (ulong)var_$10;
var_$25 = var_$24*var_$23;
var_$26 = var_$2+var_$25;
*((device float*)(var_$26)) = var_$22;
return;

}


#include <metal_stdlib>
using namespace metal;

kernel void saxpy(
    device uchar* var_$1 [[buffer(0)]],
    device uchar* var_$2 [[buffer(1)]],
    constant float& var_$3_arg [[buffer(2)]],
    uint3 nautilus_threadIdx [[thread_position_in_threadgroup]],
    uint3 nautilus_blockIdx [[threadgroup_position_in_grid]],
    uint3 nautilus_blockDim [[threads_per_threadgroup]],
    uint3 nautilus_gridDim [[threadgroups_per_grid]]
) {
float var_$3;
uint var_$4;
uint var_$5;
uint var_$6;
uint var_$7;
uint var_$8;
int var_$9;
ulong var_$13;
ulong var_$14;
ulong var_$15;
device uchar* var_$16;
float var_$19;
float var_$20;
int var_$21;
ulong var_$25;
ulong var_$26;
ulong var_$27;
device uchar* var_$28;
float var_$31;
float var_$32;
int var_$33;
ulong var_$37;
ulong var_$38;
ulong var_$39;
device uchar* var_$40;
var_$3 = var_$3_arg;
var_$4 = nautilus_blockIdx.x;
var_$5 = nautilus_blockDim.x;
var_$6 = var_$4*var_$5;
var_$7 = nautilus_threadIdx.x;
var_$8 = var_$6+var_$7;
var_$9 = (int)var_$8;
var_$13 = (ulong)4;
var_$14 = (ulong)var_$9;
var_$15 = var_$14*var_$13;
var_$16 = var_$1+var_$15;
var_$19 = *((device float*)(var_$16));
var_$20 = var_$3*var_$19;
var_$21 = (int)var_$8;
var_$25 = (ulong)4;
var_$26 = (ulong)var_$21;
var_$27 = var_$26*var_$25;
var_$28 = var_$2+var_$27;
var_$31 = *((device float*)(var_$28));
var_$32 = var_$20+var_$31;
var_$33 = (int)var_$8;
var_$37 = (ulong)4;
var_$38 = (ulong)var_$33;
var_$39 = var_$38*var_$37;
var_$40 = var_$2+var_$39;
*((device float*)(var_$40)) = var_$32;
return;

}


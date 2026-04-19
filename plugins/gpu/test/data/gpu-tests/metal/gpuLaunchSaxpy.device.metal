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
int var_$11;
ulong var_$15;
ulong var_$16;
ulong var_$17;
device uchar* var_$18;
float var_$21;
float var_$22;
int var_$23;
ulong var_$27;
ulong var_$28;
ulong var_$29;
device uchar* var_$30;
float var_$33;
float var_$34;
int var_$35;
ulong var_$39;
ulong var_$40;
ulong var_$41;
device uchar* var_$42;
var_$6 = nautilus_blockIdx.x;
var_$7 = nautilus_blockDim.x;
var_$8 = var_$6*var_$7;
var_$9 = nautilus_threadIdx.x;
var_$10 = var_$8+var_$9;
var_$11 = (int)var_$10;
var_$15 = (ulong)4;
var_$16 = (ulong)var_$11;
var_$17 = var_$16*var_$15;
var_$18 = var_$1+var_$17;
var_$21 = *((device float*)(var_$18));
var_$22 = var_$3*var_$21;
var_$23 = (int)var_$10;
var_$27 = (ulong)4;
var_$28 = (ulong)var_$23;
var_$29 = var_$28*var_$27;
var_$30 = var_$2+var_$29;
var_$33 = *((device float*)(var_$30));
var_$34 = var_$22+var_$33;
var_$35 = (int)var_$10;
var_$39 = (ulong)4;
var_$40 = (ulong)var_$35;
var_$41 = var_$40*var_$39;
var_$42 = var_$2+var_$41;
*((device float*)(var_$42)) = var_$34;
return;

}


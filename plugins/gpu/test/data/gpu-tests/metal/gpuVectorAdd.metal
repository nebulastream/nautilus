#include <metal_stdlib>
using namespace metal;

kernel void execute(
    device uchar* var_$1 [[buffer(0)]],
    device uchar* var_$2 [[buffer(1)]],
    device uchar* var_$3 [[buffer(2)]],
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
int var_$15;
ulong var_$19;
ulong var_$20;
ulong var_$21;
device uchar* var_$22;
uint var_$25;
uint var_$26;
uint var_$27;
int var_$28;
ulong var_$32;
ulong var_$33;
ulong var_$34;
device uchar* var_$35;
var_$4 = nautilus_threadIdx.x;
var_$5 = (int)var_$4;
var_$9 = (ulong)4;
var_$10 = (ulong)var_$5;
var_$11 = var_$10*var_$9;
var_$12 = var_$1+var_$11;
var_$15 = (int)var_$4;
var_$19 = (ulong)4;
var_$20 = (ulong)var_$15;
var_$21 = var_$20*var_$19;
var_$22 = var_$2+var_$21;
var_$25 = *((device uint*)(var_$12));
var_$26 = *((device uint*)(var_$22));
var_$27 = var_$25+var_$26;
var_$28 = (int)var_$4;
var_$32 = (ulong)4;
var_$33 = (ulong)var_$28;
var_$34 = var_$33*var_$32;
var_$35 = var_$3+var_$34;
*((device uint*)(var_$35)) = var_$27;
threadgroup_barrier(mem_flags::mem_threadgroup);
return;

}


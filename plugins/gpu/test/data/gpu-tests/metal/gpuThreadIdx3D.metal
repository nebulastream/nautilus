#include <metal_stdlib>
using namespace metal;

kernel void execute(
    uint3 nautilus_threadIdx [[thread_position_in_threadgroup]],
    uint3 nautilus_blockIdx [[threadgroup_position_in_grid]],
    uint3 nautilus_blockDim [[threads_per_threadgroup]],
    uint3 nautilus_gridDim [[threadgroups_per_grid]]
) {
uint var_$1;
uint var_$2;
uint var_$3;
uint var_$4;
uint var_$5;
var_$1 = nautilus_threadIdx.x;
var_$2 = nautilus_threadIdx.y;
var_$3 = var_$1+var_$2;
var_$4 = nautilus_threadIdx.z;
var_$5 = var_$3+var_$4;
return var_$5;

}


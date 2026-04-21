#include <metal_stdlib>
using namespace metal;

kernel void execute(
    constant uint& var_$1 [[buffer(0)]],
    uint3 nautilus_threadIdx [[thread_position_in_threadgroup]],
    uint3 nautilus_blockIdx [[threadgroup_position_in_grid]],
    uint3 nautilus_blockDim [[threads_per_threadgroup]],
    uint3 nautilus_gridDim [[threadgroups_per_grid]]
) {
uint var_$2;
uint var_$3;
var_$2 = nautilus_gridDim.x;
var_$3 = var_$2+var_$1;
return var_$3;

}


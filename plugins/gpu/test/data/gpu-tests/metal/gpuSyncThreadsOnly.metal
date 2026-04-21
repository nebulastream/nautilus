#include <metal_stdlib>
using namespace metal;

kernel void execute(
    uint3 nautilus_threadIdx [[thread_position_in_threadgroup]],
    uint3 nautilus_blockIdx [[threadgroup_position_in_grid]],
    uint3 nautilus_blockDim [[threads_per_threadgroup]],
    uint3 nautilus_gridDim [[threadgroups_per_grid]]
) {
threadgroup_barrier(mem_flags::mem_threadgroup);
return;

}


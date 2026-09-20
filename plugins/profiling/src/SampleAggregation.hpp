#pragma once

#include "nautilus/profiling/sample_report.hpp"
#include <perfcpp/sample/result.hpp>

namespace nautilus::profiling::detail {

/// Resolves every sampled instruction pointer and aggregates the result.
///
/// Shared by the single-threaded and threaded samplers so both attribute
/// samples the same way -- the JIT symbol registry first, ELF symbols second,
/// aggregated by module and name (see SampleReport).
[[nodiscard]] SampleReport aggregate(const ::perf::SampleResult& samples);

} // namespace nautilus::profiling::detail

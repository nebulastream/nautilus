#pragma once

#include "nautilus/profiling/measurement.hpp"
#include <perfcpp/counter/result.hpp>
#include <string>
#include <vector>

namespace nautilus::profiling::detail {

/// Copies a perf-cpp counter result into Measurement entries.
///
/// The copy is not incidental: `CounterResult` holds `std::string_view`s into
/// the process-wide `CounterDefinition`, and a Measurement outlives the counter
/// that produced it.
inline std::vector<Measurement::Entry> toEntries(const ::perf::CounterResult& result) {
	std::vector<Measurement::Entry> entries;
	entries.reserve(result.size());
	for (const auto& [name, value] : result) {
		entries.push_back({std::string(name), value});
	}
	return entries;
}

} // namespace nautilus::profiling::detail

#pragma once

#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace nautilus::profiling {

/// Counter values collected over one measurement window.
///
/// A window is whatever sat between `CounterSet::start()` and
/// `CounterSet::stop()` -- compiled nautilus functions, `invoke()`d runtime
/// calls, plain host code, any mixture -- on one thread or summed across a
/// pool.
///
/// Values are `double` rather than an integer type because that is what the
/// perf subsystem hands back once a counter has been scaled for multiplexing
/// (when more events are requested than the PMU has slots for, the kernel
/// time-slices them and reports an extrapolated value), and because derived
/// metrics like cycles-per-instruction are not integral.
class Measurement {
public:
	/// One event or derived metric and its value.
	struct Entry {
		std::string name;
		double value = 0.0;
	};

	Measurement() = default;
	explicit Measurement(std::vector<Entry> entries);

	/// Every event and metric collected.
	[[nodiscard]] const std::vector<Entry>& entries() const noexcept;

	/// The value of one event or metric by name, or nullopt if it was not
	/// collected.
	[[nodiscard]] std::optional<double> get(std::string_view event) const noexcept;

	[[nodiscard]] bool empty() const noexcept;

	/// A human-readable two-column table. Intended for a log line or a demo's
	/// stdout, not for parsing.
	[[nodiscard]] std::string table() const;

	/// One `event,value` row per entry, with a header.
	[[nodiscard]] std::string csv() const;

private:
	std::vector<Entry> entries_;
};

} // namespace nautilus::profiling

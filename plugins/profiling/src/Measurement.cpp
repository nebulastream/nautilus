#include "nautilus/profiling/measurement.hpp"
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <sstream>
#include <utility>

namespace nautilus::profiling {

namespace {

/// Formats a counter value for the human-readable table.
///
/// Counter values span many orders of magnitude -- a ratio like
/// cycles-per-instruction sits near 1, an instruction count on a real workload
/// runs into the billions -- so a single format string reads badly at one end or
/// the other. Values below 1000 keep three decimals; everything above is
/// rendered as a whole number, which is what a counter actually is once the
/// kernel's multiplexing scale factor has been applied.
std::string formatValue(double value) {
	char buffer[64];
	if (std::abs(value) < 1000.0 && value != std::floor(value)) {
		std::snprintf(buffer, sizeof(buffer), "%.3f", value);
	} else {
		std::snprintf(buffer, sizeof(buffer), "%.0f", value);
	}
	return buffer;
}

} // namespace

Measurement::Measurement(std::vector<Entry> entries) : entries_(std::move(entries)) {
}

const std::vector<Measurement::Entry>& Measurement::entries() const noexcept {
	return entries_;
}

std::optional<double> Measurement::get(std::string_view event) const noexcept {
	const auto it =
	    std::find_if(entries_.begin(), entries_.end(), [event](const Entry& entry) { return entry.name == event; });
	if (it == entries_.end()) {
		return std::nullopt;
	}
	return it->value;
}

bool Measurement::empty() const noexcept {
	return entries_.empty();
}

std::string Measurement::table() const {
	if (entries_.empty()) {
		return {};
	}

	// Size both columns to their widest cell so the table stays aligned whatever
	// magnitudes the run produced.
	size_t nameWidth = std::string_view("event").size();
	size_t valueWidth = std::string_view("value").size();
	for (const auto& entry : entries_) {
		nameWidth = std::max(nameWidth, entry.name.size());
		valueWidth = std::max(valueWidth, formatValue(entry.value).size());
	}

	std::ostringstream out;
	const auto row = [&out, nameWidth, valueWidth](const std::string& name, const std::string& value) {
		out << name << std::string(nameWidth - name.size(), ' ') << "  " << std::string(valueWidth - value.size(), ' ')
		    << value << '\n';
	};

	row("event", "value");
	for (const auto& entry : entries_) {
		row(entry.name, formatValue(entry.value));
	}
	return out.str();
}

std::string Measurement::csv() const {
	std::ostringstream out;
	out << "event,value\n";
	for (const auto& entry : entries_) {
		out << entry.name << ',' << formatValue(entry.value) << '\n';
	}
	return out.str();
}

} // namespace nautilus::profiling

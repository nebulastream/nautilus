
#include "nautilus/CompilationStatistics.hpp"
#include "nautilus/exceptions/RuntimeException.hpp"
#include <algorithm>
#include <fmt/format.h>
#include <string_view>

namespace nautilus::compiler {

namespace {

/// True when @p key names a duration rendered as milliseconds. We match a
/// trailing ".ms" segment or the sentinel word "totalMs" / "Ms" at the end
/// of a dot-separated key, so users can both write
/// @c "tracing.ms" and @c "irPasses.totalMs" and get the same `X.XX ms`
/// rendering.
bool isDurationKey(std::string_view key) {
	if (key.size() >= 3 && key.substr(key.size() - 3) == ".ms") {
		return true;
	}
	if (key.size() >= 2 && key.substr(key.size() - 2) == "Ms") {
		return true;
	}
	return false;
}

/// True when @p key names a byte count; rendered in human-friendly units.
bool isByteKey(std::string_view key) {
	return key.size() >= 6 && key.substr(key.size() - 6) == ".bytes";
}

std::string formatInteger(int64_t v) {
	// Thousands separator — readable for large counts / byte sizes.
	std::string raw = std::to_string(v);
	std::size_t start = (!raw.empty() && raw.front() == '-') ? 1U : 0U;
	for (std::ptrdiff_t i = static_cast<std::ptrdiff_t>(raw.size()) - 3; i > static_cast<std::ptrdiff_t>(start);
	     i -= 3) {
		raw.insert(static_cast<std::size_t>(i), ",");
	}
	return raw;
}

std::string formatBytes(int64_t bytes) {
	constexpr double KiB = 1024.0;
	constexpr double MiB = 1024.0 * 1024.0;
	constexpr double GiB = 1024.0 * 1024.0 * 1024.0;
	double absValue = static_cast<double>(bytes < 0 ? -bytes : bytes);
	if (absValue >= GiB) {
		return fmt::format("{:.2f} GiB", static_cast<double>(bytes) / GiB);
	}
	if (absValue >= MiB) {
		return fmt::format("{:.2f} MiB", static_cast<double>(bytes) / MiB);
	}
	if (absValue >= KiB) {
		return fmt::format("{:.2f} KiB", static_cast<double>(bytes) / KiB);
	}
	return fmt::format("{} B", formatInteger(bytes));
}

/// Group = segment before the first '.' in the key. Keys without a '.'
/// land in the @c "general" group.
std::pair<std::string_view, std::string_view> splitGroup(std::string_view key) {
	auto pos = key.find('.');
	if (pos == std::string_view::npos) {
		return {"general", key};
	}
	return {key.substr(0, pos), key.substr(pos + 1)};
}

/// Phase totals (keys like @c "*.totalMs" or containing "total") are
/// emphasised by being emitted first within their group.
bool isTotalSubkey(std::string_view subkey) {
	return subkey == "totalMs" || subkey == "total.ms" || subkey == "total";
}

} // namespace

void CompilationStatistics::set(std::string key, StatValue value) {
	auto it = index_.find(key);
	if (it != index_.end()) {
		entries_[it->second].second = std::move(value);
		return;
	}
	index_.emplace(key, entries_.size());
	entries_.emplace_back(std::move(key), std::move(value));
}

void CompilationStatistics::add(const std::string& key, int64_t delta) {
	auto it = index_.find(key);
	if (it == index_.end()) {
		index_.emplace(key, entries_.size());
		entries_.emplace_back(key, StatValue {delta});
		return;
	}
	auto& slot = entries_[it->second].second;
	if (!std::holds_alternative<int64_t>(slot)) {
		throw RuntimeException("CompilationStatistics::add on non-integer key '" + key + "'");
	}
	std::get<int64_t>(slot) += delta;
}

void CompilationStatistics::recordTimingMs(const std::string& key, std::chrono::steady_clock::time_point start) {
	auto elapsed = std::chrono::duration<double, std::milli>(std::chrono::steady_clock::now() - start).count();
	set(key, StatValue {elapsed});
}

void CompilationStatistics::merge(const std::string& prefix, const CompilationStatistics& other) {
	for (const auto& [k, v] : other.entries_) {
		set(prefix + "." + k, v);
	}
}

bool CompilationStatistics::contains(const std::string& key) const {
	return index_.contains(key);
}

const StatValue* CompilationStatistics::find(const std::string& key) const {
	auto it = index_.find(key);
	if (it == index_.end()) {
		return nullptr;
	}
	return &entries_[it->second].second;
}

std::string CompilationStatistics::formatValue(std::string_view key, const StatValue& value) {
	if (std::holds_alternative<double>(value)) {
		double d = std::get<double>(value);
		if (isDurationKey(key)) {
			return fmt::format("{:.2f} ms", d);
		}
		return fmt::format("{:.4f}", d);
	}
	if (std::holds_alternative<int64_t>(value)) {
		int64_t v = std::get<int64_t>(value);
		if (isByteKey(key)) {
			return formatBytes(v);
		}
		return formatInteger(v);
	}
	return std::get<std::string>(value);
}

std::string CompilationStatistics::toString() const {
	std::string out;
	for (const auto& [k, v] : entries_) {
		out += fmt::format("{} = {}\n", k, formatValue(k, v));
	}
	return out;
}

std::string CompilationStatistics::formatReport(std::string_view compilationId, std::string_view backendName) const {
	// Collect entries into insertion-ordered groups. Preserving the
	// encounter order keeps the report layout predictable for tests and
	// humans alike (e.g. `tracing` before `ir` before `mlir`).
	std::vector<std::string_view> groupOrder;
	std::unordered_map<std::string_view, std::vector<Entry>> groups;
	for (const auto& entry : entries_) {
		auto [group, subkey] = splitGroup(entry.first);
		if (!groups.contains(group)) {
			groupOrder.push_back(group);
		}
		groups[group].emplace_back(std::string(subkey), entry.second);
	}

	// Inside each group: total* subkeys first, then insertion order.
	for (auto& [_, list] : groups) {
		std::stable_partition(list.begin(), list.end(),
		                      [](const Entry& e) { return isTotalSubkey(e.first); });
	}

	// Column width per group for aligned `key = value`.
	auto columnWidth = [](const std::vector<Entry>& list) {
		std::size_t w = 0;
		for (const auto& e : list) {
			w = std::max(w, e.first.size());
		}
		return w;
	};

	constexpr std::string_view HR = "───────────────────────────────────────────────────────────────────";
	std::string out;
	out += fmt::format("┌─ nautilus compilation statistics ─────────────────────────────────\n");
	out += fmt::format("│  id:      {}\n", compilationId);
	if (!backendName.empty()) {
		out += fmt::format("│  backend: {}\n", backendName);
	}
	out += fmt::format("├{}\n", HR);

	for (auto group : groupOrder) {
		const auto& list = groups[group];
		out += fmt::format("│  {}\n", group);
		auto width = columnWidth(list);
		for (const auto& [sub, val] : list) {
			out += fmt::format("│    {:<{}} = {}\n", sub, width, formatValue(sub, val));
		}
	}
	out += fmt::format("└{}", HR);
	return out;
}

ScopedTimer::ScopedTimer(CompilationStatistics* stats, std::string key)
    : stats_(stats), key_(std::move(key)), start_(std::chrono::steady_clock::now()) {
}

ScopedTimer::~ScopedTimer() {
	if (stats_ != nullptr) {
		stats_->recordTimingMs(key_, start_);
	}
}

} // namespace nautilus::compiler


#include "nautilus/CompilationStatistics.hpp"
#include "nautilus/exceptions/RuntimeException.hpp"
#include <algorithm>
#include <fmt/format.h>
#include <functional>
#include <string_view>
#include <type_traits>
#include <variant>

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

void appendJsonString(std::string& out, std::string_view text) {
	out += '"';
	for (const char c : text) {
		switch (c) {
		case '"':
			out += "\\\"";
			break;
		case '\\':
			out += "\\\\";
			break;
		case '\n':
			out += "\\n";
			break;
		case '\t':
			out += "\\t";
			break;
		default:
			if (static_cast<unsigned char>(c) < 0x20) {
				out += fmt::format("\\u{:04x}", static_cast<unsigned>(c));
			} else {
				out += c;
			}
		}
	}
	out += '"';
}

} // namespace

std::size_t CompilationStatistics::indexOf(std::string_view key, std::size_t hash) const {
	if (table_.empty()) {
		return NOT_FOUND;
	}
	const std::size_t mask = table_.size() - 1;
	for (std::size_t slot = hash & mask;; slot = (slot + 1) & mask) {
		const auto position = table_[slot];
		if (position == 0) {
			return NOT_FOUND;
		}
		if (hashes_[position - 1] == static_cast<uint32_t>(hash) && entries_[position - 1].first == key) {
			return position - 1;
		}
	}
}

StatValue& CompilationStatistics::append(std::string_view key, std::size_t hash, StatValue value) const {
	if (entries_.empty()) {
		// most compiles record between 40 and 60 entries: one allocation each instead of a series of regrowths
		entries_.reserve(64);
		hashes_.reserve(64);
		table_.assign(128, 0);
	} else if ((entries_.size() + 1) * 2 > table_.size()) {
		table_.assign(table_.size() * 2, 0);
		const std::size_t mask = table_.size() - 1;
		for (std::size_t i = 0; i < entries_.size(); ++i) {
			std::size_t slot = hashes_[i] & mask;
			while (table_[slot] != 0) {
				slot = (slot + 1) & mask;
			}
			table_[slot] = static_cast<uint32_t>(i + 1);
		}
	}
	const std::size_t mask = table_.size() - 1;
	std::size_t slot = hash & mask;
	while (table_[slot] != 0) {
		slot = (slot + 1) & mask;
	}
	entries_.emplace_back(std::string(key), std::move(value));
	hashes_.push_back(static_cast<uint32_t>(hash));
	table_[slot] = static_cast<uint32_t>(entries_.size());
	return entries_.back().second;
}

void CompilationStatistics::set(std::string_view key, StatValue value) {
	const auto hash = std::hash<std::string_view> {}(key);
	if (const auto index = indexOf(key, hash); index != NOT_FOUND) {
		entries_[index].second = std::move(value);
		return;
	}
	append(key, hash, std::move(value));
}

void CompilationStatistics::add(std::string_view key, int64_t delta) {
	const auto hash = std::hash<std::string_view> {}(key);
	const auto index = indexOf(key, hash);
	if (index == NOT_FOUND) {
		append(key, hash, StatValue {delta});
		return;
	}
	auto& slot = entries_[index].second;
	if (!std::holds_alternative<int64_t>(slot)) {
		throw RuntimeException("CompilationStatistics::add on non-integer key '" + std::string(key) + "'");
	}
	std::get<int64_t>(slot) += delta;
}

void CompilationStatistics::recordTimingMs(std::string_view key, std::chrono::steady_clock::time_point start) {
	auto elapsed = std::chrono::duration<double, std::milli>(std::chrono::steady_clock::now() - start).count();
	set(key, StatValue {elapsed});
}

void CompilationStatistics::merge(const std::string& prefix, const CompilationStatistics& other) {
	other.expandDeferred();
	for (const auto& [k, v] : other.entries_) {
		set(prefix + "." + k, v);
	}
}

void CompilationStatistics::defer(std::function<void(CompilationStatistics&)> fill) {
	deferred_.push_back({entries_.size(), std::move(fill)});
	hasDeferred_.store(true, std::memory_order_release);
}

void CompilationStatistics::expandDeferredSlow() const {
	const std::lock_guard<std::mutex> lock(expandMutex_);
	if (!hasDeferred_.load(std::memory_order_acquire)) {
		return; // another reader expanded them
	}
	// Each group lands where it was deferred: the positions refer to the entries of the writer, so the entries of the
	// groups before this one shift it. A key that was set directly as well keeps its direct value.
	auto groups = std::move(deferred_);
	deferred_.clear();
	std::vector<Entry> merged;
	merged.reserve(64);
	std::size_t copied = 0;
	for (auto& group : groups) {
		for (; copied < group.position && copied < entries_.size(); ++copied) {
			merged.push_back(std::move(entries_[copied]));
		}
		CompilationStatistics scratch;
		group.fill(scratch);
		for (auto& entry : scratch.entries_) {
			merged.push_back(std::move(entry));
		}
	}
	for (; copied < entries_.size(); ++copied) {
		merged.push_back(std::move(entries_[copied]));
	}
	// rebuild the index over the merged order, dropping a duplicate key (the first one, a direct set, wins)
	entries_.clear();
	hashes_.clear();
	table_.clear();
	for (auto& entry : merged) {
		const auto hash = std::hash<std::string_view> {}(entry.first);
		if (indexOf(entry.first, hash) == NOT_FOUND) {
			append(entry.first, hash, std::move(entry.second));
		}
	}
	hasDeferred_.store(false, std::memory_order_release);
}

bool CompilationStatistics::contains(std::string_view key) const {
	expandDeferred();
	return indexOf(key, std::hash<std::string_view> {}(key)) != NOT_FOUND;
}

const StatValue* CompilationStatistics::find(std::string_view key) const {
	expandDeferred();
	const auto index = indexOf(key, std::hash<std::string_view> {}(key));
	return index == NOT_FOUND ? nullptr : &entries_[index].second;
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
	expandDeferred();
	std::string out;
	for (const auto& [k, v] : entries_) {
		out += fmt::format("{} = {}\n", k, formatValue(k, v));
	}
	return out;
}

std::string CompilationStatistics::toJson() const {
	expandDeferred();
	std::string out = "{";
	for (const auto& [k, v] : entries_) {
		if (out.size() > 1) {
			out += ',';
		}
		appendJsonString(out, k);
		out += ':';
		std::visit(
		    [&out](const auto& value) {
			    using T = std::decay_t<decltype(value)>;
			    if constexpr (std::is_same_v<T, std::string>) {
				    appendJsonString(out, value);
			    } else {
				    out += fmt::format("{}", value);
			    }
		    },
		    v);
	}
	out += '}';
	return out;
}

std::string CompilationStatistics::formatReport(std::string_view compilationId, std::string_view backendName) const {
	expandDeferred();
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
		std::stable_partition(list.begin(), list.end(), [](const Entry& e) { return isTotalSubkey(e.first); });
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


#pragma once

#include <chrono>
#include <cstdint>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <variant>
#include <vector>

namespace nautilus::compiler {

/**
 * @brief Tagged value stored in a @ref CompilationStatistics map.
 *
 * Counters use @c int64_t, durations use @c double (milliseconds), and any
 * free-form text (backend name, compilation id, …) uses @c std::string.
 */
using StatValue = std::variant<int64_t, double, std::string>;

/**
 * @brief General-purpose string-map for metrics collected throughout the
 * compilation pipeline (tracing, IR passes, backend lowering).
 *
 * The map is insertion-ordered so reports are reproducible. Keys use
 * dot-separated prefixes to denote groupings (e.g. @c "irPasses.totalMs",
 * @c "mlir.pipeline.ms"); @ref formatReport groups and aligns entries by the
 * leading segment.
 *
 * Not thread-safe: a single @c CompilationStatistics object belongs to one
 * compilation and is written by the owning pipeline; concurrent compilations
 * use independent instances.
 */
class CompilationStatistics {
public:
	CompilationStatistics() = default;

	/// Overwrite (or create) the entry for @p key.
	void set(std::string key, StatValue value);

	/// Increment @p key's integer counter by @p delta. Creates the entry if
	/// absent. Throws if @p key is present but holds a non-integer value.
	void add(const std::string& key, int64_t delta);

	/// Record ``now - start`` in milliseconds under @p key. Uses a
	/// monotonic steady clock.
	void recordTimingMs(const std::string& key, std::chrono::steady_clock::time_point start);

	/// Copy every entry from @p other into this map, prefixing each key with
	/// @p prefix + '.'. Useful for nesting backend-local stats under a
	/// pipeline-wide namespace.
	void merge(const std::string& prefix, const CompilationStatistics& other);

	[[nodiscard]] bool contains(const std::string& key) const;
	[[nodiscard]] const StatValue* find(const std::string& key) const;
	[[nodiscard]] bool empty() const noexcept {
		return entries_.empty();
	}
	[[nodiscard]] std::size_t size() const noexcept {
		return entries_.size();
	}

	using Entry = std::pair<std::string, StatValue>;
	[[nodiscard]] auto begin() const {
		return entries_.begin();
	}
	[[nodiscard]] auto end() const {
		return entries_.end();
	}

	/// One `key = value` per line, values rendered as @ref formatValue.
	[[nodiscard]] std::string toString() const;

	/// Multiline, grouped, column-aligned report suitable for user-facing
	/// log output. Entries are grouped by the substring preceding the first
	/// `.` in the key; entries without a `.` land in the @c general group.
	/// Phase totals (keys ending in @c totalMs or @c total.ms) are emitted
	/// first within each group.
	[[nodiscard]] std::string formatReport(std::string_view compilationId, std::string_view backendName = {}) const;

	/// Format a single @ref StatValue: durations as "X.XX ms", byte counts
	/// as "X B"/"X.Y KiB"/"X.Y MiB", integer counters with thousands
	/// separators, strings verbatim.
	[[nodiscard]] static std::string formatValue(std::string_view key, const StatValue& value);

private:
	// Insertion-ordered vector keeps the report stable; the map indexes it
	// for O(1) lookup / overwrite.
	std::vector<Entry> entries_;
	std::unordered_map<std::string, std::size_t> index_;
};

/**
 * @brief RAII helper that records the time elapsed between construction and
 * destruction into a @ref CompilationStatistics under the supplied key.
 *
 * A null @c stats pointer is allowed and the timer becomes a no-op, matching
 * the nullable-pointer pattern used elsewhere for optional instrumentation.
 */
class ScopedTimer {
public:
	ScopedTimer(CompilationStatistics* stats, std::string key);
	~ScopedTimer();

	ScopedTimer(const ScopedTimer&) = delete;
	ScopedTimer& operator=(const ScopedTimer&) = delete;
	ScopedTimer(ScopedTimer&&) = delete;
	ScopedTimer& operator=(ScopedTimer&&) = delete;

private:
	CompilationStatistics* stats_;
	std::string key_;
	std::chrono::steady_clock::time_point start_;
};

} // namespace nautilus::compiler

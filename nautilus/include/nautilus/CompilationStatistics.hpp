
#pragma once

#include <atomic>
#include <chrono>
#include <cstdint>
#include <functional>
#include <mutex>
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
 * Not thread-safe while a compilation writes it: a single @c CompilationStatistics
 * object belongs to one compilation and is written by the owning pipeline;
 * concurrent compilations use independent instances. Once the compilation is
 * done any number of threads may read it.
 */
class CompilationStatistics {
public:
	CompilationStatistics() = default;
	CompilationStatistics(const CompilationStatistics&) = delete;
	CompilationStatistics& operator=(const CompilationStatistics&) = delete;

	/// Registers a group of entries that a stage keeps as plain data: @p fill adds them with @ref set when the
	/// statistics are first read (@ref find, iteration, a report), at the position in the insertion order this call
	/// has. Most compiles never read their statistics, and building a few dozen keyed entries is a sizeable part of
	/// the time of a small one. @p fill must not depend on anything that changes after this call.
	void defer(std::function<void(CompilationStatistics&)> fill);

	/// Overwrite (or create) the entry for @p key.
	void set(std::string_view key, StatValue value);

	/// Increment @p key's integer counter by @p delta. Creates the entry if
	/// absent. Throws if @p key is present but holds a non-integer value.
	void add(std::string_view key, int64_t delta);

	/// Record ``now - start`` in milliseconds under @p key. Uses a
	/// monotonic steady clock.
	void recordTimingMs(std::string_view key, std::chrono::steady_clock::time_point start);

	/// Copy every entry from @p other into this map, prefixing each key with
	/// @p prefix + '.'. Useful for nesting backend-local stats under a
	/// pipeline-wide namespace.
	void merge(const std::string& prefix, const CompilationStatistics& other);

	[[nodiscard]] bool contains(std::string_view key) const;
	/// Whether @p key was set directly, without expanding the deferred groups (so the writer of the statistics can ask
	/// during a compilation without paying for them). A key of a deferred group is not found.
	[[nodiscard]] bool containsDirect(std::string_view key) const {
		return indexOf(key, std::hash<std::string_view> {}(key)) != NOT_FOUND;
	}
	[[nodiscard]] const StatValue* find(std::string_view key) const;
	[[nodiscard]] bool empty() const noexcept {
		expandDeferred();
		return entries_.empty();
	}
	[[nodiscard]] std::size_t size() const noexcept {
		expandDeferred();
		return entries_.size();
	}

	using Entry = std::pair<std::string, StatValue>;
	[[nodiscard]] auto begin() const {
		expandDeferred();
		return entries_.begin();
	}
	[[nodiscard]] auto end() const {
		expandDeferred();
		return entries_.end();
	}

	/// One `key = value` per line, values rendered as @ref formatValue.
	[[nodiscard]] std::string toString() const;

	/// One flat JSON object with every entry in insertion order: integers and
	/// durations as JSON numbers, text as JSON strings. Single line, so it can
	/// be appended to a JSON-lines file.
	[[nodiscard]] std::string toJson() const;

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
	static constexpr std::size_t NOT_FOUND = ~std::size_t {0};

	/// The index of @p key in @c entries_, or @c NOT_FOUND.
	[[nodiscard]] std::size_t indexOf(std::string_view key, std::size_t hash) const;
	/// Appends a new entry for @p key (which must be absent) and enters it into the index.
	StatValue& append(std::string_view key, std::size_t hash, StatValue value) const;

	/// Adds the entries of the deferred groups, once, from whichever thread reads first.
	void expandDeferred() const {
		if (hasDeferred_.load(std::memory_order_acquire)) {
			expandDeferredSlow();
		}
	}
	void expandDeferredSlow() const;

	// Insertion-ordered vector keeps the report stable. Every compile records a few dozen entries, so the index is a
	// flat open-addressing table of positions into the vector, not a node-based map that allocates per entry. The
	// containers are mutable for the lazy expansion of the deferred groups, which a reader performs.
	mutable std::vector<Entry> entries_;
	mutable std::vector<uint32_t> hashes_;
	mutable std::vector<uint32_t> table_; // position + 1 per slot, 0 when empty; a power-of-two size

	struct Deferred {
		std::size_t position;
		std::function<void(CompilationStatistics&)> fill;
	};
	mutable std::vector<Deferred> deferred_;
	mutable std::atomic<bool> hasDeferred_ {false};
	mutable std::mutex expandMutex_;
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

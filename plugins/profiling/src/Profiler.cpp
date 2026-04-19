#include "nautilus/profiling/profiler.hpp"
#include "ProfilingInstrumentationPhase.hpp"
#include "nautilus/Engine.hpp"
#include <algorithm>
#include <array>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <sstream>
#include <unordered_map>

#ifdef __linux__
#include <csignal>
#include <linux/perf_event.h>
#include <sys/ioctl.h>
#include <sys/syscall.h>
#include <sys/wait.h>
#include <unistd.h>
#endif

#ifdef __APPLE__
#include <csignal>
#include <sys/wait.h>
#include <unistd.h>
#endif

namespace nautilus::profiling {

// --- Thread-local entry state ---
// Keyed by ProfileRegion pointer so multiple threads can profile the same
// region concurrently without data races on entry timestamps/counters.

struct ThreadEntryState {
	uint64_t entry_ns = 0;
	std::array<uint64_t, MAX_HW_COUNTERS> entry_counters = {};
};

static thread_local std::unordered_map<ProfileRegion*, ThreadEntryState> tl_entry_state;

// --- Timing ---

static uint64_t nowNs() {
	struct timespec ts {};
	clock_gettime(CLOCK_MONOTONIC, &ts);
	return static_cast<uint64_t>(ts.tv_sec) * 1000000000ULL + static_cast<uint64_t>(ts.tv_nsec);
}

// --- Linux perf_event_open helpers ---

#ifdef __linux__
static int perfEventOpen(uint32_t type, uint64_t config) {
	struct perf_event_attr pe {};
	pe.type = type;
	pe.size = sizeof(pe);
	pe.config = config;
	pe.disabled = 1;
	pe.exclude_kernel = 1;
	pe.exclude_hv = 1;
	int fd = static_cast<int>(syscall(SYS_perf_event_open, &pe, 0, -1, -1, 0));
	if (fd >= 0) {
		ioctl(fd, PERF_EVENT_IOC_RESET, 0);
		ioctl(fd, PERF_EVENT_IOC_ENABLE, 0);
	}
	return fd;
}

static uint64_t readCounter(int fd) {
	uint64_t value = 0;
	if (fd >= 0) {
		auto n = read(fd, &value, sizeof(value));
		(void) n;
	}
	return value;
}

static uint64_t hwCounterConfig(HwCounter c) {
	switch (c) {
	case HwCounter::CPU_CYCLES:
		return PERF_COUNT_HW_CPU_CYCLES;
	case HwCounter::INSTRUCTIONS:
		return PERF_COUNT_HW_INSTRUCTIONS;
	case HwCounter::CACHE_MISSES:
		return PERF_COUNT_HW_CACHE_MISSES;
	case HwCounter::BRANCH_MISSES:
		return PERF_COUNT_HW_BRANCH_MISSES;
	}
	return PERF_COUNT_HW_CPU_CYCLES;
}
#endif

static const char* counterName(HwCounter c) {
	switch (c) {
	case HwCounter::CPU_CYCLES:
		return "cycles";
	case HwCounter::INSTRUCTIONS:
		return "instructions";
	case HwCounter::CACHE_MISSES:
		return "cache-misses";
	case HwCounter::BRANCH_MISSES:
		return "branch-misses";
	}
	return "unknown";
}

// --- ProfileRegion ---

ProfileRegion::ProfileRegion(std::string name) : name_(std::move(name)) {
}

ProfileRegion::~ProfileRegion() {
#ifdef __linux__
	for (int i = 0; i < MAX_HW_COUNTERS; ++i) {
		if (counter_fds_[i] >= 0) {
			close(counter_fds_[i]);
		}
	}
#endif
}

void ProfileRegion::enableCounter(HwCounter counter) {
	counters_enabled_[static_cast<int>(counter)] = true;
}

void ProfileRegion::enableIntervalRecording(size_t max_intervals) {
	record_intervals_ = true;
	max_intervals_ = max_intervals;
	intervals_.reserve(std::min(max_intervals, static_cast<size_t>(8192)));
}

void ProfileRegion::openCounters() {
#ifdef __linux__
	if (counters_opened_) {
		return;
	}
	counters_opened_ = true;
	for (int i = 0; i < MAX_HW_COUNTERS; ++i) {
		if (counters_enabled_[i]) {
			counter_fds_[i] = perfEventOpen(PERF_TYPE_HARDWARE, hwCounterConfig(static_cast<HwCounter>(i)));
		}
	}
#endif
}

void ProfileRegion::runtimeStart(ProfileRegion* region) {
	if (!region->active_.load(std::memory_order_relaxed)) {
		return;
	}

	if (!region->counters_opened_) {
		region->openCounters();
	}

	auto& entry = tl_entry_state[region];

#ifdef __linux__
	for (int i = 0; i < MAX_HW_COUNTERS; ++i) {
		if (region->counter_fds_[i] >= 0) {
			entry.entry_counters[i] = readCounter(region->counter_fds_[i]);
		}
	}
#endif

	entry.entry_ns = nowNs();
}

void ProfileRegion::runtimeStop(ProfileRegion* region) {
	if (!region->active_.load(std::memory_order_relaxed)) {
		return;
	}

	auto exit_ns = nowNs();
	auto& entry = tl_entry_state[region];
	auto duration = exit_ns - entry.entry_ns;

	region->call_count_.fetch_add(1, std::memory_order_relaxed);
	region->total_ns_.fetch_add(duration, std::memory_order_relaxed);

	// Atomic min update.
	auto cur_min = region->min_ns_.load(std::memory_order_relaxed);
	while (duration < cur_min && !region->min_ns_.compare_exchange_weak(cur_min, duration, std::memory_order_relaxed)) {
	}
	// Atomic max update.
	auto cur_max = region->max_ns_.load(std::memory_order_relaxed);
	while (duration > cur_max && !region->max_ns_.compare_exchange_weak(cur_max, duration, std::memory_order_relaxed)) {
	}

#ifdef __linux__
	for (int i = 0; i < MAX_HW_COUNTERS; ++i) {
		if (region->counter_fds_[i] >= 0) {
			auto delta = readCounter(region->counter_fds_[i]) - entry.entry_counters[i];
			region->total_counter_[i].fetch_add(delta, std::memory_order_relaxed);
		}
	}
#endif

	// Record interval for perf correlation (mutex-protected).
	if (region->record_intervals_) {
		std::lock_guard lock(region->intervals_mutex_);
		if (region->intervals_.size() < region->max_intervals_) {
			region->intervals_.push_back({entry.entry_ns, exit_ns});
		}
	}
}

void ProfileRegion::reset() {
	call_count_.store(0, std::memory_order_relaxed);
	total_ns_.store(0, std::memory_order_relaxed);
	min_ns_.store(UINT64_MAX, std::memory_order_relaxed);
	max_ns_.store(0, std::memory_order_relaxed);
	for (int i = 0; i < MAX_HW_COUNTERS; ++i) {
		total_counter_[i].store(0, std::memory_order_relaxed);
	}
	std::lock_guard lock(intervals_mutex_);
	intervals_.clear();
}

void ProfileRegion::setActive(bool active) {
	active_.store(active, std::memory_order_relaxed);
}

bool ProfileRegion::isActive() const {
	return active_.load(std::memory_order_relaxed);
}

const std::string& ProfileRegion::name() const noexcept {
	return name_;
}

uint64_t ProfileRegion::callCount() const {
	return call_count_.load(std::memory_order_relaxed);
}

uint64_t ProfileRegion::totalDurationNs() const {
	return total_ns_.load(std::memory_order_relaxed);
}

uint64_t ProfileRegion::minDurationNs() const {
	auto v = min_ns_.load(std::memory_order_relaxed);
	return v == UINT64_MAX ? 0 : v;
}

uint64_t ProfileRegion::maxDurationNs() const {
	return max_ns_.load(std::memory_order_relaxed);
}

double ProfileRegion::avgDurationNs() const {
	auto count = callCount();
	return count > 0 ? static_cast<double>(totalDurationNs()) / count : 0.0;
}

uint64_t ProfileRegion::counterTotal(HwCounter counter) const {
	return total_counter_[static_cast<int>(counter)].load(std::memory_order_relaxed);
}

double ProfileRegion::counterAvg(HwCounter counter) const {
	auto count = callCount();
	return count > 0 ? static_cast<double>(counterTotal(counter)) / count : 0.0;
}

const std::vector<RegionInterval>& ProfileRegion::getIntervals() const {
	return intervals_;
}

void ProfileRegion::report() const {
	std::cout << std::left << std::setw(20) << name_ << std::right << std::setw(10) << callCount() << std::setw(14)
	          << std::fixed << std::setprecision(1) << static_cast<double>(totalDurationNs()) / 1000.0 << std::setw(12)
	          << std::fixed << std::setprecision(0) << avgDurationNs() << std::setw(12) << minDurationNs()
	          << std::setw(12) << maxDurationNs();

	for (int i = 0; i < MAX_HW_COUNTERS; ++i) {
		if (counters_enabled_[i]) {
			auto total = total_counter_[i].load(std::memory_order_relaxed);
			auto count = callCount();
			double avg = count > 0 ? static_cast<double>(total) / count : 0.0;
			std::cout << "  " << counterName(static_cast<HwCounter>(i)) << "=" << total << "(avg:" << std::fixed
			          << std::setprecision(0) << avg << ")";
		}
	}
	std::cout << std::endl;
}

// --- macOS sample command conversion ---

#ifdef __APPLE__
// Converts macOS `sample` command output into perf-script-like format
// that correlateWithPerf() can process.
//
// The `sample` command produces an aggregated call tree with per-node
// sample counts — it does NOT record per-sample timestamps.  To enable
// timestamp-based correlation with ProfileRegion intervals we generate
// synthetic timestamps distributed evenly across [sessionStartNs,
// sessionEndNs].  For a repetitive workload (tight pipeline loop) this
// attributes samples to regions roughly in proportion to each region's
// share of total execution time.
static void convertSampleOutput(const std::string& samplePath, const std::string& outputPath, uint64_t sessionStartNs,
                                uint64_t sessionEndNs) {
	std::ifstream in(samplePath);
	if (!in.is_open()) {
		return;
	}

	// --- Phase 1: parse the call tree into (stack, count) pairs. ---
	struct StackSample {
		std::vector<std::string> frames;
		uint64_t count;
	};
	std::vector<StackSample> allStacks;

	// Current root-to-leaf path being built while walking the tree.
	struct PathNode {
		std::string name;
		size_t depth;
		uint64_t count;
	};
	std::vector<PathNode> path;
	bool inCallGraph = false;

	// Helper: emit the current path as a leaf sample.
	auto emitLeaf = [&]() {
		if (path.empty()) {
			return;
		}
		StackSample s;
		s.count = path.back().count;
		for (auto& node : path) {
			s.frames.push_back(node.name);
		}
		allStacks.push_back(std::move(s));
	};

	std::string line;
	while (std::getline(in, line)) {
		if (line.find("Call graph:") != std::string::npos) {
			inCallGraph = true;
			continue;
		}
		if (!inCallGraph) {
			continue;
		}
		if (line.empty() || line.find("Total number") != std::string::npos) {
			emitLeaf();
			path.clear();
			if (line.find("Total number") != std::string::npos) {
				break;
			}
			continue;
		}

		// Count leading whitespace / tree-drawing characters.
		size_t indent = 0;
		while (indent < line.size() && (line[indent] == ' ' || line[indent] == '\t' || line[indent] == '+' ||
		                                line[indent] == '!' || line[indent] == '|' || line[indent] == ':')) {
			indent++;
		}
		if (indent >= line.size()) {
			continue;
		}

		// First token after indent is the sample count.
		auto rest = line.substr(indent);
		auto spacePos = rest.find(' ');
		if (spacePos == std::string::npos) {
			continue;
		}
		uint64_t count = 0;
		try {
			count = std::stoull(rest.substr(0, spacePos));
		} catch (...) {
			continue;
		}

		// Extract function name.
		rest = rest.substr(spacePos + 1);
		auto nameStart = rest.find_first_not_of(' ');
		if (nameStart == std::string::npos) {
			continue;
		}
		rest = rest.substr(nameStart);
		auto parenPos = rest.find("  (in");
		std::string funcName;
		if (parenPos != std::string::npos) {
			funcName = rest.substr(0, parenPos);
		} else {
			auto endPos = rest.find_first_of(" \t");
			funcName = endPos != std::string::npos ? rest.substr(0, endPos) : rest;
		}

		// Filter out thread headers, unknown symbols, and dispatch queue annotations.
		if (funcName.empty() || funcName[0] == '?' || funcName.starts_with("Thread_") ||
		    funcName.starts_with("DispatchQueue_")) {
			continue;
		}

		size_t depth = indent / 2;

		// If the new node is at the same or shallower depth we are at a
		// sibling (or uncle) in the tree → the previous path ended at a leaf.
		if (!path.empty() && path.back().depth >= depth) {
			emitLeaf();
			while (!path.empty() && path.back().depth >= depth) {
				path.pop_back();
			}
		}
		path.push_back({funcName, depth, count});
	}
	// Flush any remaining path.
	emitLeaf();

	// --- Phase 2: emit perf-script format with synthetic timestamps. ---

	uint64_t totalSamples = 0;
	for (auto& s : allStacks) {
		totalSamples += s.count;
	}
	if (totalSamples == 0 || sessionStartNs >= sessionEndNs) {
		return;
	}

	uint64_t timeRange = sessionEndNs - sessionStartNs;
	uint64_t sampleIdx = 0;

	std::ofstream out(outputPath);
	for (auto& s : allStacks) {
		for (uint64_t i = 0; i < s.count; ++i) {
			uint64_t ts = sessionStartNs + (timeRange * sampleIdx) / totalSamples;
			uint64_t secs = ts / 1000000000ULL;
			uint64_t nanos = ts % 1000000000ULL;
			out << "nautilus 1 [000] " << secs << "." << std::setw(9) << std::setfill('0') << nanos << ": cpu-clock:\n";
			// Emit frames leaf-to-root (perf script convention: deepest first).
			for (auto it = s.frames.rbegin(); it != s.frames.rend(); ++it) {
				out << "\t0 " << *it << " (nautilus)\n";
			}
			out << "\n";
			sampleIdx++;
		}
	}
}
#endif

// --- Profiler ---

Profiler::~Profiler() {
#if defined(__linux__) || defined(__APPLE__)
	if (profilerPid_ > 0) {
		kill(profilerPid_, SIGINT);
		int status = 0;
		waitpid(profilerPid_, &status, 0);
		profilerPid_ = -1;
	}
	if (!profilerDataDir_.empty()) {
		std::filesystem::remove_all(profilerDataDir_);
		profilerDataDir_.clear();
	}
#endif
}

ProfileRegion* Profiler::getOrCreateRegion(const std::string& name) {
	std::lock_guard lock(mutex_);
	// Check if a region with this name already exists.
	for (auto* region : regions_) {
		if (region->name() == name) {
			return region;
		}
	}
	// Create a new region owned by this profiler.
	std::unique_ptr<ProfileRegion> region(new ProfileRegion(name));
	auto* ptr = region.get();
	ownedRegions_.push_back(std::move(region));
	regions_.push_back(ptr);
	return ptr;
}

void Profiler::start() {
	std::lock_guard lock(mutex_);

	if (state_ == State::IDLE) {
		// Fresh start: reset all regions and enable interval recording.
		for (auto* region : regions_) {
			region->reset();
			region->enableIntervalRecording();
		}
		combinedStacks_.clear();

		// Fork system profiler.
#if defined(__linux__) || defined(__APPLE__)
		auto tmpDir = std::filesystem::temp_directory_path() / "nautilus_profiler";
		std::filesystem::create_directories(tmpDir);
		profilerDataDir_ = tmpDir.string();

		pid_t pid = fork();
		if (pid == 0) {
			auto pidStr = std::to_string(getppid());
#ifdef __linux__
			auto dataPath = (tmpDir / "perf.data").string();
			execlp("perf", "perf", "record", "-g", "-k", "1", "-p", pidStr.c_str(), "-o", dataPath.c_str(), nullptr);
#else // __APPLE__
			auto outPath = (tmpDir / "sample_output.txt").string();
			execlp("sample", "sample", pidStr.c_str(), "-wait", "-mayDie", "-f", outPath.c_str(), nullptr);
#endif
			_exit(127);
		} else if (pid > 0) {
			profilerPid_ = pid;
			usleep(100000); // 100ms for profiler to attach
		} else {
			profilerPid_ = -1;
		}
#endif
	}
	// Both IDLE->RUNNING and PAUSED->RUNNING: activate all regions.
	for (auto* region : regions_) {
		region->setActive(true);
	}
	state_ = State::RUNNING;
}

void Profiler::pause() {
	std::lock_guard lock(mutex_);
	if (state_ != State::RUNNING) {
		return;
	}
	for (auto* region : regions_) {
		region->setActive(false);
	}
	state_ = State::PAUSED;
}

void Profiler::stop() {
	std::lock_guard lock(mutex_);
	for (auto* region : regions_) {
		region->setActive(false);
	}

#if defined(__linux__) || defined(__APPLE__)
	if (profilerPid_ > 0) {
		kill(profilerPid_, SIGINT);
		int status = 0;
		waitpid(profilerPid_, &status, 0);

		// The profiler is OK if it exited normally (and wasn't our 127 fallback
		// from a failed execlp), or if it was terminated by the SIGINT we sent.
		bool profilerOk =
		    (WIFEXITED(status) && WEXITSTATUS(status) != 127) || (WIFSIGNALED(status) && WTERMSIG(status) == SIGINT);
		profilerPid_ = -1;

		if (!profilerOk) {
			std::cerr << "nautilus-profiling: system profiler was unavailable" << std::endl;
			if (!profilerDataDir_.empty()) {
				std::filesystem::remove_all(profilerDataDir_);
				profilerDataDir_.clear();
			}
		} else {
			auto dir = profilerDataDir_;
			auto scriptPath = dir + "/profiler_script.txt";
			auto combinedPath = dir + "/combined.folded";

#ifdef __linux__
			auto dataPath = dir + "/perf.data";
			std::string cmd = "perf script -i " + dataPath + " > " + scriptPath + " 2>/dev/null";
			int ret = std::system(cmd.c_str());
#else // __APPLE__
			auto samplePath = dir + "/sample_output.txt";
			uint64_t sessionStart = UINT64_MAX, sessionEnd = 0;
			for (auto* region : regions_) {
				for (const auto& iv : region->getIntervals()) {
					if (iv.start_ns < sessionStart) {
						sessionStart = iv.start_ns;
					}
					if (iv.stop_ns > sessionEnd) {
						sessionEnd = iv.stop_ns;
					}
				}
			}
			convertSampleOutput(samplePath, scriptPath, sessionStart, sessionEnd);
			int ret = 0;
#endif

			if (ret == 0) {
				correlateWithPerf(scriptPath, combinedPath);
				std::ifstream f(combinedPath);
				if (f.is_open()) {
					combinedStacks_.assign(std::istreambuf_iterator<char>(f), std::istreambuf_iterator<char>());
				}
			}

			std::filesystem::remove_all(dir);
			profilerDataDir_.clear();
		}
	}
#endif

	state_ = State::IDLE;
}

void Profiler::report() const {
	std::lock_guard lock(mutex_);
	if (regions_.empty()) {
		return;
	}

	std::vector<ProfileRegion*> sorted(regions_.begin(), regions_.end());
	std::sort(sorted.begin(), sorted.end(), [](const ProfileRegion* a, const ProfileRegion* b) {
		return a->totalDurationNs() > b->totalDurationNs();
	});

	std::cout << std::left << std::setw(20) << "Region" << std::right << std::setw(10) << "Calls" << std::setw(14)
	          << "Total(us)" << std::setw(12) << "Avg(ns)" << std::setw(12) << "Min(ns)" << std::setw(12) << "Max(ns)"
	          << std::endl;
	std::cout << std::string(80, '-') << std::endl;
	for (auto* region : sorted) {
		if (region->callCount() > 0) {
			region->report();
		}
	}
}

void Profiler::exportChromeTrace(const std::string& path) const {
	std::lock_guard lock(mutex_);

	// Helper to write a JSON-escaped string.
	auto writeJsonStr = [](std::ofstream& out, const std::string& s) {
		for (char c : s) {
			if (c == '"') {
				out << "\\\"";
			} else if (c == '\\') {
				out << "\\\\";
			} else {
				out << c;
			}
		}
	};

	// --- Parse the combined folded stacks into per-region leaf breakdowns. ---
	struct LeafEntry {
		std::string name;
		uint64_t count = 0;
	};
	// region name -> list of (leaf function, sample count)
	std::map<std::string, std::vector<LeafEntry>> regionLeaves;

	if (!combinedStacks_.empty()) {
		std::istringstream ss(combinedStacks_);
		std::string line;
		while (std::getline(ss, line)) {
			if (line.empty()) {
				continue;
			}
			// Format: "region;frame1;...;leaf count"
			auto lastSpace = line.rfind(' ');
			if (lastSpace == std::string::npos || lastSpace == 0) {
				continue;
			}
			uint64_t count = 0;
			try {
				count = std::stoull(line.substr(lastSpace + 1));
			} catch (...) {
				continue;
			}
			auto stackPart = line.substr(0, lastSpace);

			// Region is the first frame.
			auto firstSemi = stackPart.find(';');
			std::string regionName = firstSemi != std::string::npos ? stackPart.substr(0, firstSemi) : stackPart;

			// Leaf is the last frame.
			auto lastSemi = stackPart.rfind(';');
			std::string leaf = lastSemi != std::string::npos ? stackPart.substr(lastSemi + 1) : regionName;

			// Merge into existing entry or add new.
			auto& leaves = regionLeaves[regionName];
			bool found = false;
			for (auto& entry : leaves) {
				if (entry.name == leaf) {
					entry.count += count;
					found = true;
					break;
				}
			}
			if (!found) {
				leaves.push_back({leaf, count});
			}
		}

		// Sort leaves by count descending within each region.
		for (auto& [_, leaves] : regionLeaves) {
			std::sort(leaves.begin(), leaves.end(),
			          [](const LeafEntry& a, const LeafEntry& b) { return a.count > b.count; });
		}
	}

	// --- Emit Chrome Trace JSON. ---
	// Level 1 (region spans): use actual ProfileRegion duration.
	// Level 2 (native function spans): distribute duration proportionally
	// to sample counts within each region.  Chrome Trace nests events on
	// the same tid that fall inside a parent's time range.

	std::ofstream out(path);
	out << "[\n";

	bool first = true;

	// Helper: emit level-2 native sub-spans for a region, distributing
	// dur_us across leaves proportionally to their sample counts.
	auto emitLeafSpans = [&](const std::vector<LeafEntry>& leaves, double ts_us, double dur_us) {
		uint64_t totalSamples = 0;
		for (auto& leaf : leaves) {
			totalSamples += leaf.count;
		}
		if (totalSamples == 0) {
			return;
		}
		double leaf_ts = ts_us;
		for (auto& leaf : leaves) {
			double leaf_dur = dur_us * static_cast<double>(leaf.count) / static_cast<double>(totalSamples);
			out << ",\n  {\"name\": \"";
			writeJsonStr(out, leaf.name);
			out << "\", \"cat\": \"native\", \"ph\": \"X\", " << "\"ts\": " << std::fixed << std::setprecision(1)
			    << leaf_ts << ", \"dur\": " << leaf_dur << ", \"pid\": 1, \"tid\": 1, "
			    << "\"args\": {\"samples\": " << leaf.count << "}}";
			leaf_ts += leaf_dur;
		}
	};

	uint64_t ts_offset_us = 0;

	for (auto* region : regions_) {
		if (region->callCount() == 0) {
			continue;
		}
		auto dur_us = static_cast<double>(region->totalDurationNs()) / 1000.0;
		auto ts_us = static_cast<double>(ts_offset_us);

		if (!first) {
			out << ",\n";
		}
		first = false;
		out << "  {\"name\": \"";
		writeJsonStr(out, region->name());
		out << "\", \"cat\": \"region\", \"ph\": \"X\", " << "\"ts\": " << std::fixed << std::setprecision(1) << ts_us
		    << ", \"dur\": " << dur_us << ", \"pid\": 1, \"tid\": 1, "
		    << "\"args\": {\"calls\": " << region->callCount() << ", \"avg_ns\": " << std::setprecision(0)
		    << region->avgDurationNs() << "}}";

		if (auto it = regionLeaves.find(region->name()); it != regionLeaves.end()) {
			emitLeafSpans(it->second, ts_us, dur_us);
		}

		ts_offset_us += static_cast<uint64_t>(dur_us);
	}

	// Unattributed samples (if any) as a single span at the end. Synthetic
	// duration of 1us per sample keeps the visualiser happy when there's no
	// corresponding region-level timing to anchor on.
	if (auto unattr = regionLeaves.find("[unattributed]"); unattr != regionLeaves.end()) {
		uint64_t totalSamples = 0;
		for (auto& leaf : unattr->second) {
			totalSamples += leaf.count;
		}
		if (totalSamples > 0) {
			double dur_us = static_cast<double>(totalSamples);
			double ts_us = static_cast<double>(ts_offset_us);
			out << ",\n  {\"name\": \"[unattributed]\", \"cat\": \"region\", \"ph\": \"X\", "
			    << "\"ts\": " << std::fixed << std::setprecision(1) << ts_us << ", \"dur\": " << dur_us
			    << ", \"pid\": 1, \"tid\": 1, " << "\"args\": {\"samples\": " << totalSamples << "}}";
			emitLeafSpans(unattr->second, ts_us, dur_us);
		}
	}

	out << "\n]\n";
}

const std::vector<ProfileRegion*>& Profiler::getRegions() const {
	return regions_;
}

Profiler::State Profiler::getState() const {
	return state_;
}

std::shared_ptr<Profiler> Profiler::enableForEngine(const engine::NautilusEngine& engine) {
	auto pass = std::make_unique<ProfilingIRPass>();
	auto profiler = pass->getProfiler();
	engine.addIRPass(std::move(pass));
	return profiler;
}

// --- Perf correlation ---

void Profiler::correlateWithPerf(const std::string& perfScriptPath, const std::string& outputPath) {
	struct TaggedInterval {
		uint64_t start_ns;
		uint64_t stop_ns;
		const std::string* region_name;
	};
	std::vector<TaggedInterval> all_intervals;

	for (auto* region : regions_) {
		for (const auto& iv : region->getIntervals()) {
			all_intervals.push_back({iv.start_ns, iv.stop_ns, &region->name()});
		}
	}

	std::sort(all_intervals.begin(), all_intervals.end(),
	          [](const TaggedInterval& a, const TaggedInterval& b) { return a.start_ns < b.start_ns; });

	std::ifstream perfFile(perfScriptPath);
	if (!perfFile.is_open()) {
		std::cerr << "nautilus-profiling: cannot open profiler output: " << perfScriptPath << std::endl;
		return;
	}

	std::map<std::string, uint64_t> foldedStacks;

	std::string line;
	uint64_t sample_ts_ns = 0;
	std::vector<std::string> stack_frames;
	bool in_stack = false;

	auto flushSample = [&]() {
		if (stack_frames.empty() || sample_ts_ns == 0) {
			stack_frames.clear();
			return;
		}

		const std::string* region_name = nullptr;
		auto it = std::upper_bound(all_intervals.begin(), all_intervals.end(), sample_ts_ns,
		                           [](uint64_t ts, const TaggedInterval& iv) { return ts < iv.start_ns; });
		if (it != all_intervals.begin()) {
			--it;
			if (sample_ts_ns >= it->start_ns && sample_ts_ns <= it->stop_ns) {
				region_name = it->region_name;
			}
		}

		std::string folded;
		if (region_name) {
			folded = *region_name;
		} else {
			folded = "[unattributed]";
		}
		for (auto rit = stack_frames.rbegin(); rit != stack_frames.rend(); ++rit) {
			folded += ";";
			folded += *rit;
		}
		foldedStacks[folded]++;

		stack_frames.clear();
	};

	while (std::getline(perfFile, line)) {
		if (line.empty()) {
			flushSample();
			in_stack = false;
			continue;
		}

		if (line[0] != ' ' && line[0] != '\t') {
			flushSample();
			in_stack = true;

			auto colon1 = line.find(':');
			if (colon1 == std::string::npos) {
				continue;
			}
			auto ts_end = colon1;
			auto ts_start = ts_end;
			while (ts_start > 0 && (std::isdigit(line[ts_start - 1]) || line[ts_start - 1] == '.')) {
				ts_start--;
			}
			if (ts_start >= ts_end) {
				continue;
			}
			std::string ts_str = line.substr(ts_start, ts_end - ts_start);
			auto dot = ts_str.find('.');
			if (dot == std::string::npos) {
				continue;
			}
			uint64_t secs = std::stoull(ts_str.substr(0, dot));
			std::string frac = ts_str.substr(dot + 1);
			while (frac.size() < 9) {
				frac += '0';
			}
			frac = frac.substr(0, 9);
			uint64_t nanos = std::stoull(frac);
			sample_ts_ns = secs * 1000000000ULL + nanos;
		} else if (in_stack) {
			std::string trimmed = line;
			auto first_nonspace = trimmed.find_first_not_of(" \t");
			if (first_nonspace != std::string::npos) {
				trimmed = trimmed.substr(first_nonspace);
			}
			auto space = trimmed.find(' ');
			if (space != std::string::npos) {
				trimmed = trimmed.substr(space + 1);
			}
			auto end = trimmed.find_first_of(" (");
			if (end != std::string::npos) {
				trimmed = trimmed.substr(0, end);
			}
			auto plus = trimmed.find('+');
			if (plus != std::string::npos) {
				trimmed = trimmed.substr(0, plus);
			}
			if (!trimmed.empty() && trimmed != "[unknown]") {
				stack_frames.push_back(trimmed);
			}
		}
	}
	flushSample();

	std::ofstream out(outputPath);
	for (const auto& [stack, count] : foldedStacks) {
		out << stack << " " << count << "\n";
	}
}

} // namespace nautilus::profiling

// P0 latency corpus (docs/low-latency-plan.md): compiles scalable functions with
// every available backend and reports the per-stage split of the compile, the
// operations it produced and the heap allocations it made.
//
//   nautilus-latency-corpus [repetitions [function [N [backend]]]] > results.jsonl
//
// One JSON object per (function, size, backend) on stdout; a table on stderr.

#include "nautilus/CompilationStatistics.hpp"
#include "nautilus/Engine.hpp"
#include "nautilus/config.hpp"
#include "nautilus/function.hpp"
#include "nautilus/region.hpp"
#include "nautilus/static.hpp"
#include <algorithm>
#include <atomic>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <functional>
#include <new>
#include <string>
#include <vector>

// ---- allocation counting ----------------------------------------------------
static std::atomic<uint64_t> allocationCount {0};
static std::atomic<uint64_t> allocationBytes {0};

void* operator new(std::size_t size) {
	allocationCount.fetch_add(1, std::memory_order_relaxed);
	allocationBytes.fetch_add(size, std::memory_order_relaxed);
	if (void* p = std::malloc(size == 0 ? 1 : size)) {
		return p;
	}
	throw std::bad_alloc();
}
void* operator new[](std::size_t size) {
	return operator new(size);
}
void* operator new(std::size_t size, std::align_val_t align) {
	allocationCount.fetch_add(1, std::memory_order_relaxed);
	allocationBytes.fetch_add(size, std::memory_order_relaxed);
	auto a = static_cast<std::size_t>(align);
	if (void* p = std::aligned_alloc(a, (size + a - 1) / a * a)) {
		return p;
	}
	throw std::bad_alloc();
}
void* operator new[](std::size_t size, std::align_val_t align) {
	return operator new(size, align);
}
void operator delete(void* p) noexcept {
	std::free(p);
}
void operator delete[](void* p) noexcept {
	std::free(p);
}
void operator delete(void* p, std::size_t) noexcept {
	std::free(p);
}
void operator delete[](void* p, std::size_t) noexcept {
	std::free(p);
}
void operator delete(void* p, std::align_val_t) noexcept {
	std::free(p);
}
void operator delete[](void* p, std::align_val_t) noexcept {
	std::free(p);
}
void operator delete(void* p, std::size_t, std::align_val_t) noexcept {
	std::free(p);
}
void operator delete[](void* p, std::size_t, std::align_val_t) noexcept {
	std::free(p);
}

using namespace nautilus;

// ---- corpus -----------------------------------------------------------------
// N is read at trace time, so one function covers every size.
static int N = 0;

extern "C" int64_t corpusRuntimeCall(int64_t a, int64_t b) noexcept {
	return a ^ (b + 1);
}

// Straight-line arithmetic, unrolled by static_val: one call site, N iterations.
static val<int64_t> straight(val<int64_t> x) {
	val<int64_t> r = x;
	for (static_val<int> i = 0; i < N; i++) {
		r = r * 3 + i;
	}
	return r;
}

// N independent ifs in a row: every if adds a traced path (symbolic replay).
static val<int64_t> chainedIf(val<int64_t> x) {
	val<int64_t> r = 0;
	for (static_val<int> i = 0; i < N; i++) {
		if (x > i) {
			r = r + 1;
		}
	}
	return r;
}

// The same, each if inside its own region(): replay bounded to the region.
static val<int64_t> chainedIfRegion(val<int64_t> x) {
	val<int64_t> r = 0;
	for (static_val<int> i = 0; i < N; i++) {
		region([&]() {
			if (x > i) {
				r = r + 1;
			}
		});
	}
	return r;
}

// A val loop whose body is N unrolled operations.
static val<int64_t> loopBody(val<int64_t> n) {
	val<int64_t> r = 1;
	for (val<int64_t> j = 0; j < n; j = j + 1) {
		for (static_val<int> i = 0; i < N; i++) {
			r = r + j * i;
		}
	}
	return r;
}

// N runtime calls.
static val<int64_t> calls(val<int64_t> x) {
	val<int64_t> r = x;
	for (static_val<int> i = 0; i < N; i++) {
		r = invoke(corpusRuntimeCall, r, val<int64_t>(i));
	}
	return r;
}

// 1024 distinct call sites (no static_val): stresses tag creation / the tag trie.
#define NAUTILUS_SITE r = r * 3 + 1;
#define NAUTILUS_SITE4 NAUTILUS_SITE NAUTILUS_SITE NAUTILUS_SITE NAUTILUS_SITE
#define NAUTILUS_SITE16 NAUTILUS_SITE4 NAUTILUS_SITE4 NAUTILUS_SITE4 NAUTILUS_SITE4
#define NAUTILUS_SITE64 NAUTILUS_SITE16 NAUTILUS_SITE16 NAUTILUS_SITE16 NAUTILUS_SITE16
#define NAUTILUS_SITE256 NAUTILUS_SITE64 NAUTILUS_SITE64 NAUTILUS_SITE64 NAUTILUS_SITE64
static val<int64_t> distinctSites1024(val<int64_t> x) {
	val<int64_t> r = x;
	NAUTILUS_SITE256 NAUTILUS_SITE256 NAUTILUS_SITE256 NAUTILUS_SITE256 return r;
}

struct CorpusEntry {
	const char* name;
	val<int64_t> (*fn)(val<int64_t>);
	std::vector<int> sizes;
};

// ---- driver -----------------------------------------------------------------
static double statMs(const compiler::CompilationStatistics& s, const char* key) {
	if (auto* v = s.find(key)) {
		if (auto* d = std::get_if<double>(v)) {
			return *d;
		}
	}
	return -1;
}

static int64_t statInt(const compiler::CompilationStatistics& s, const char* key) {
	if (auto* v = s.find(key)) {
		if (auto* i = std::get_if<int64_t>(v)) {
			return *i;
		}
	}
	return -1;
}

int main(int argc, char** argv) {
	int repetitions = argc > 1 ? std::atoi(argv[1]) : 5;
	std::string onlyFunction = argc > 2 ? argv[2] : "";
	int onlySize = argc > 3 ? std::atoi(argv[3]) : 0;
	std::string onlyBackend = argc > 4 ? argv[4] : "";

	std::vector<std::string> backends;
#ifdef ENABLE_BC_BACKEND
	backends.emplace_back("bc");
#endif
#ifdef ENABLE_TBC_BACKEND
	backends.emplace_back("tbc");
#endif
#ifdef ENABLE_ASMJIT_BACKEND
	backends.emplace_back("asmjit");
#endif
#ifdef ENABLE_MLIR_BACKEND
	backends.emplace_back("mlir");
#endif

	std::vector<CorpusEntry> corpus = {
	    {"straight", straight, {100, 1000, 10000}},
	    {"chainedIf", chainedIf, {10, 100, 300}},
	    {"chainedIfRegion", chainedIfRegion, {10, 100, 300}},
	    {"loopBody", loopBody, {100, 1000, 10000}},
	    {"calls", calls, {100, 1000, 5000}},
	    {"distinctSites1024", distinctSites1024, {1}},
	};

	std::fprintf(stderr, "%-18s %6s %-7s %9s %9s %9s %9s %9s %8s %9s %8s\n", "function", "N", "backend", "total_ms",
	             "trace_ms", "irgen_ms", "passes_ms", "backend_ms", "ir_ops", "allocs", "ns/op");
	for (auto& entry : corpus) {
		if (!onlyFunction.empty() && onlyFunction != entry.name) {
			continue;
		}
		for (int size : entry.sizes) {
			if (onlySize != 0 && onlySize != size) {
				continue;
			}
			N = size;
			for (auto& backend : backends) {
				if (!onlyBackend.empty() && onlyBackend != backend) {
					continue;
				}
				engine::Options options;
				options.setOption("engine.backend", backend);
				if (backend == "mlir") {
					options.setOption("mlir.eager_compilation", true);
				}
				std::vector<double> totals, traces, irgens, passes, backendMs;
				std::vector<uint64_t> allocs;
				int64_t irOps = -1, baselineOps = -1;
				std::string lastStats;
				for (int rep = 0; rep < repetitions; rep++) {
					engine::NautilusEngine engine(options);
					auto before = allocationCount.load();
					auto start = std::chrono::steady_clock::now();
					auto fn = engine.registerFunction(entry.fn);
					auto wall =
					    std::chrono::duration<double, std::milli>(std::chrono::steady_clock::now() - start).count();
					allocs.push_back(allocationCount.load() - before);
					auto stats = fn.getStatistics();
					if (!stats) {
						std::fprintf(stderr, "no statistics for %s\n", entry.name);
						return 1;
					}
					totals.push_back(wall);
					traces.push_back(statMs(*stats, "tracing.ms"));
					irgens.push_back(statMs(*stats, "irGeneration.ms"));
					passes.push_back(statMs(*stats, "irPasses.totalMs"));
					backendMs.push_back(statMs(*stats, "backend.totalMs"));
					irOps = statInt(*stats, "ir.operations");
					baselineOps = statInt(*stats, "irPasses.baseline.operations");
					lastStats = stats->toJson();
					// Make sure the code runs (and is not a stale handle).
					volatile auto result = fn(int64_t {3});
					(void) result;
				}
				auto median = [](std::vector<double> v) {
					std::sort(v.begin(), v.end());
					return v[v.size() / 2];
				};
				std::sort(allocs.begin(), allocs.end());
				double total = median(totals);
				auto opsForRate = baselineOps > 0 ? baselineOps : irOps;
				std::fprintf(stderr, "%-18s %6d %-7s %9.3f %9.3f %9.3f %9.3f %9.3f %8lld %9llu %8.0f\n", entry.name,
				             size, backend.c_str(), total, median(traces), median(irgens), median(passes),
				             median(backendMs), static_cast<long long>(opsForRate),
				             static_cast<unsigned long long>(allocs[allocs.size() / 2]),
				             opsForRate > 0 ? total * 1e6 / static_cast<double>(opsForRate) : 0.0);
				std::printf("{\"function\":\"%s\",\"n\":%d,\"backend\":\"%s\",\"total_ms\":%.4f,\"tracing_ms\":%.4f,"
				            "\"irgen_ms\":%.4f,\"passes_ms\":%.4f,\"backend_ms\":%.4f,\"ir_ops_before_passes\":%lld,"
				            "\"ir_ops\":%lld,\"allocations\":%llu,\"stats\":%s}\n",
				            entry.name, size, backend.c_str(), total, median(traces), median(irgens), median(passes),
				            median(backendMs), static_cast<long long>(baselineOps), static_cast<long long>(irOps),
				            static_cast<unsigned long long>(allocs[allocs.size() / 2]), lastStats.c_str());
				std::fflush(stdout);
			}
		}
	}
	return 0;
}

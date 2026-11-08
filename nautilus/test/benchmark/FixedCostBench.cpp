// Fixed cost per compile: how long registerFunction takes for tiny kernels, and where the time goes outside the
// operations themselves. Unlike nautilus-compile-bench (a fresh engine per repetition, per-operation numbers), this
// harness registers the same small kernel many times on ONE engine, the pattern of a query pipeline made of many small
// functions, and reports the median wall time of registerFunction next to the median of every duration statistic of
// the compile, so the time that no stage accounts for (`unaccounted`: engine, module and bookkeeping) is visible.
//
//   nautilus-fixed-cost-bench [repetitions [kernel [backend]]]
//
//   kernel   add, fib, sum, internalCallLoop, or "" / "all"
//   backend  tbc, asmjit, mlir, cpp, or "" / "all" (whatever is built); default tbc and asmjit
//
// One TSV block per (backend, kernel): wall_us, then one row per statistics key holding a duration (in us). The
// first repetition is a warm-up and not counted. NAUTILUS_OPTIONS layers options on top, as for any engine.

#include "CompileKernels.hpp"
#include "nautilus/CompilationStatistics.hpp"
#include "nautilus/Engine.hpp"
#include "nautilus/config.hpp"
#include "nautilus/function.hpp"
#include <algorithm>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <functional>
#include <map>
#include <string>
#include <vector>

using namespace nautilus;
using namespace compile_kernels;

static double median(std::vector<double> v) {
	std::sort(v.begin(), v.end());
	return v.empty() ? 0 : v[v.size() / 2];
}

static bool isDurationKey(const std::string& key) {
	auto ends = [&](const char* suffix) {
		const std::string s(suffix);
		return key.size() >= s.size() && key.compare(key.size() - s.size(), s.size(), s) == 0;
	};
	return ends(".ms") || ends("Ms");
}

template <typename F>
static void run(const std::string& kernel, const std::string& backend, int reps, F f) {
	engine::Options options;
	options.setOption("engine.backend", backend);
	if (backend == "mlir") {
		options.setOption("mlir.eager_compilation", true);
	}
	engine::NautilusEngine engine(options);
	std::vector<double> wall;
	std::map<std::string, std::vector<double>> stages;
	for (int rep = -1; rep < reps; rep++) {
		const auto start = std::chrono::steady_clock::now();
		auto fn = f(engine);
		const auto elapsed =
		    std::chrono::duration<double, std::micro>(std::chrono::steady_clock::now() - start).count();
		if (rep < 0) {
			continue;
		}
		wall.push_back(elapsed);
		if (auto stats = fn.getStatistics()) {
			for (const auto& [key, value] : *stats) {
				if (const auto* d = std::get_if<double>(&value); d != nullptr && isDurationKey(key)) {
					stages[key].push_back(*d * 1e3);
				}
			}
		}
	}
	const double total = stages.count("compilation.totalMs") ? median(stages["compilation.totalMs"]) : 0;
	std::printf("%s\t%s\twall_us\t%.1f\n", backend.c_str(), kernel.c_str(), median(wall));
	std::printf("%s\t%s\tunaccounted_us\t%.1f\n", backend.c_str(), kernel.c_str(), median(wall) - total);
	for (auto& [key, values] : stages) {
		std::printf("%s\t%s\t%s\t%.1f\n", backend.c_str(), kernel.c_str(), key.c_str(), median(values));
	}
	std::fflush(stdout);
}

int main(int argc, char** argv) {
	const int reps = argc > 1 ? std::atoi(argv[1]) : 200;
	std::string only = argc > 2 ? argv[2] : "";
	std::string onlyBackend = argc > 3 ? argv[3] : "";
	if (only == "all") {
		only.clear();
	}
	std::vector<std::string> backends;
#ifdef ENABLE_ASMJIT_BACKEND
	backends.emplace_back("asmjit");
#endif
#ifdef ENABLE_TBC_BACKEND
	backends.emplace_back("tbc");
#endif
#ifdef ENABLE_MLIR_BACKEND
	backends.emplace_back("mlir");
#endif
#ifdef ENABLE_C_BACKEND
	backends.emplace_back("cpp");
#endif
	for (const auto& backend : backends) {
		if (onlyBackend.empty() ? (backend == "mlir" || backend == "cpp")
		                        : (onlyBackend != "all" && onlyBackend != backend)) {
			continue;
		}
		const int n = (backend == "cpp") ? std::max(3, reps / 20) : (backend == "mlir" ? std::max(5, reps / 4) : reps);
		if (only.empty() || only == "add") {
			run("add", backend, n, [](engine::NautilusEngine& e) { return e.registerFunction(addExpression); });
		}
		if (only.empty() || only == "fib") {
			run("fib", backend, n, [](engine::NautilusEngine& e) { return e.registerFunction(fib); });
		}
		if (only.empty() || only == "sum") {
			run("sum", backend, n, [](engine::NautilusEngine& e) { return e.registerFunction(sumArr); });
		}
		if (only.empty() || only == "internalCallLoop") {
			run("internalCallLoop", backend, n,
			    [](engine::NautilusEngine& e) { return e.registerFunction(internalCallLoop); });
		}
	}
	return 0;
}

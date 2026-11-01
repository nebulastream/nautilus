// Compile-time harness: the cost per stage and per operation of compiling small and large kernels, on one binary, with
// the 0.3 pipeline and with the direct NIR tracer (`tracing.nirTracer`), so that the two can be compared without
// rebuilding. It never calls the compiled function. See the `perf-investigation` skill and
// docs/plans/nautilus-0.4/10-observability.md for the statistics keys.
//
//   nautilus-compile-bench [repetitions [kernel [backend [mode]]]]
//
//   kernel   a name from the table below, or "" / "all"
//   backend  tbc, asmjit, cpp, mlir (whatever is built), or "all"; default tbc. The NIR mode runs on tbc only (the
//            `nirtbc` chain) until the backends read NIR; the other backends get the 0.3 rows
//   mode     "03" (nirTracer=false), "nir" (nirTracer=true), "" / "both"
//
// One TSV row per (backend, kernel, N, mode) on stdout; medians over the repetitions. Columns:
//   trace_ms     tracing.ms (on the NIR path this includes writing the pre-SSA module)
//   conv_ms      NIR: conversion.ms; 0.3: irGeneration.ms + irPasses.totalMs (the work between tracer and backend)
//   backend_ms   backend.totalMs
//   trace_ops    tracing.operations (exact, same on both modes), out_ops conversion.instructions (NIR) or
//                ir.operations after the passes (0.3)
//   trace_us/op, conv_us/op, total_us/op   per traced operation (front end = trace + conv; total adds the backend)
//
// The statistics need the pipeline to record them, which the engine does for every compilation; the NIR path only
// converts when statistics are requested, so the harness always asks (Engine::getStatistics).

#include "CompileKernels.hpp"
#include "nautilus/CompilationStatistics.hpp"
#include "nautilus/Engine.hpp"
#include "nautilus/config.hpp"
#include "nautilus/function.hpp"
#include "nautilus/nautilus_function.hpp"
#include "nautilus/region.hpp"
#include "nautilus/static.hpp"
#include <algorithm>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <functional>
#include <string>
#include <vector>

using namespace nautilus;
using namespace compile_kernels;

static double median(std::vector<double> v) {
	std::sort(v.begin(), v.end());
	return v.empty() ? 0 : v[v.size() / 2];
}

static double statMs(const compiler::CompilationStatistics& s, const char* key) {
	if (auto* v = s.find(key)) {
		if (auto* d = std::get_if<double>(v)) {
			return *d;
		}
	}
	return 0;
}

static int64_t statInt(const compiler::CompilationStatistics& s, const char* key) {
	if (auto* v = s.find(key)) {
		if (auto* i = std::get_if<int64_t>(v)) {
			return *i;
		}
	}
	return 0;
}

template <typename F>
static void runKernel(const char* kernel, int n, const std::string& backend, bool nir, int reps, F f) {
	engine::Options options;
	// Until the backends read NIR, the NIR tracer's chain ends in the `nirtbc` pseudo-backend, which compiles the SSA
	// NIR with the tbc backend; the 0.3 column of tbc is the like-for-like backend stage.
	options.setOption("engine.backend", nir ? std::string("nirtbc") : backend);
	options.setOption("tracing.nirTracer", nir);
	if (nir) {
		options.setOption("tracing.nirChain", true);
	}
	if (backend == "tbc") {
		options.setOption("tbc.mode", std::string("interp"));
	}
	if (backend == "mlir") {
		options.setOption("mlir.eager_compilation", true);
	}
	std::vector<double> trace, conv, back, wall;
	int64_t traceOps = 0, outOps = 0;
	for (int rep = -1; rep < reps; rep++) { // repetition -1 is a warm-up
		engine::NautilusEngine engine(options);
		const auto start = std::chrono::steady_clock::now();
		auto fn = f(engine);
		const auto elapsed =
		    std::chrono::duration<double, std::milli>(std::chrono::steady_clock::now() - start).count();
		auto stats = fn.getStatistics();
		if (!stats) {
			std::fprintf(stderr, "no statistics for %s\n", kernel);
			std::exit(1);
		}
		if (rep < 0) {
			continue;
		}
		wall.push_back(elapsed);
		trace.push_back(statMs(*stats, "tracing.ms"));
		if (nir) {
			conv.push_back(statMs(*stats, "conversion.ms"));
			outOps = statInt(*stats, "conversion.instructions");
		} else {
			conv.push_back(statMs(*stats, "irGeneration.ms") + statMs(*stats, "irPasses.totalMs"));
			outOps = statInt(*stats, "ir.operations");
		}
		back.push_back(statMs(*stats, "backend.totalMs"));
		traceOps = statInt(*stats, "tracing.operations");
	}
	const double t = median(trace), c = median(conv), b = median(back);
	const double ops = traceOps > 0 ? static_cast<double>(traceOps) : 1.0;
	std::printf("%s\t%s\t%d\t%s\t%.4f\t%.4f\t%.4f\t%.4f\t%lld\t%lld\t%.4f\t%.4f\t%.4f\n", backend.c_str(), kernel, n,
	            nir ? "nir" : "03", t, c, b, median(wall), static_cast<long long>(traceOps),
	            static_cast<long long>(outOps), t * 1e3 / ops, c * 1e3 / ops, (t + c + b) * 1e3 / ops);
	std::fflush(stdout);
}

int main(int argc, char** argv) {
	const int reps = argc > 1 ? std::atoi(argv[1]) : 15;
	std::string only = argc > 2 ? argv[2] : "";
	std::string onlyBackend = argc > 3 ? argv[3] : "tbc";
	std::string mode = argc > 4 ? argv[4] : "both";
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
#ifdef ENABLE_C_BACKEND
	backends.emplace_back("cpp");
#endif
#ifdef ENABLE_MLIR_BACKEND
	backends.emplace_back("mlir");
#endif

	// kernels with a signature registerFunction accepts are registered by a lambda so that each is a distinct type
	struct Entry {
		const char* name;
		std::vector<int> sizes;
		std::function<void(const std::string&, int, bool, int)> run;
	};
	auto entry = [](const char* name, std::vector<int> sizes, auto kernel) {
		return Entry {name, std::move(sizes), [name, kernel](const std::string& backend, int n, bool nir, int reps) {
			              kernelSize = n;
			              runKernel(name, n, backend, nir, reps,
			                        [&](engine::NautilusEngine& e) { return e.registerFunction(kernel); });
		              }};
	};
	std::vector<Entry> corpus;
	corpus.push_back(entry("add", {0}, addExpression));
	corpus.push_back(entry("fib", {0}, fib));
	corpus.push_back(entry("sum", {0}, sumArr));
	corpus.push_back(entry("internalCallLoop", {0}, internalCallLoop));
	corpus.push_back(entry("straight", {500, 5000}, straight));
	corpus.push_back(entry("calls", {500, 5000}, calls));
	corpus.push_back(entry("loopBody", {100, 1000}, loopBody));
	corpus.push_back(entry("chainedIf", {10, 100, 300}, chainedIf));
	corpus.push_back(entry("chainedIfElse", {10, 100, 300}, chainedIfElse));
	corpus.push_back(entry("chainedIfRegion", {10, 100, 300}, chainedIfRegion));

	std::printf("backend\tkernel\tn\tmode\ttrace_ms\tconv_ms\tbackend_ms\twall_ms\ttrace_ops\tout_ops\ttrace_us/op\t"
	            "conv_us/op\ttotal_us/op\n");
	for (const auto& backend : backends) {
		if (!onlyBackend.empty() && onlyBackend != "all" && onlyBackend != backend) {
			continue;
		}
		for (const auto& e : corpus) {
			if (!only.empty() && only != e.name) {
				continue;
			}
			for (int n : e.sizes) {
				for (bool nir : {false, true}) {
					if ((mode == "03" && nir) || (mode == "nir" && !nir)) {
						continue;
					}
					if (nir && backend != "tbc") {
						continue; // only the tbc chain reads NIR so far
					}
					e.run(backend, n, nir, reps);
				}
			}
		}
	}
	return 0;
}

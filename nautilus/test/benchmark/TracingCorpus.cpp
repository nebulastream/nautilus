// Traces one function of the tracing corpus a number of times and does nothing else, so that a profiler (callgrind)
// sees the tracer alone: `nautilus-tracing-corpus [function[:N] [reps [mode]]]`; mode `nir` drives the direct NIR
// tracer, `ir` and `nirconv` add the IR generation of 0.3 and the conversion to SSA of 0.4 to the respective tracer.
#include "CompileKernels.hpp"
#include "DistinctSitesFunctions.hpp"
#include "NestedIfBenchmarks.hpp"
#include "TracingUtil.hpp"
#include "nautilus/CompilableFunction.hpp"
#include "nautilus/common/Arena.hpp"
#include "nautilus/conversion/Conversion.hpp"
#include "nautilus/tracing/ExecutionTrace.hpp"
#include "nautilus/tracing/TraceContext.hpp"
#include "nautilus/tracing/nir/NirTracer.hpp"
#include "nautilus/tracing/phases/TraceToIRConversionPhase.hpp"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <functional>
#include <list>
#include <map>
#include <string>

using namespace nautilus;
using namespace nautilus::engine;

int main(int argc, char** argv) {
	std::map<std::string, std::function<void()>> corpus {
	    {"distinctSites1024", engine::details::createFunctionWrapper(distinctSites1024)},
	    {"chainedIf100", engine::details::createFunctionWrapper(chainedIf100)},
	};
	// the kernels of the compile-time harness: `name` or `name:N` (the size of the kernel, default 500)
	using namespace compile_kernels;
	auto kernel = [&](const char* name, auto fn) {
		corpus[name] = engine::details::createFunctionWrapper(fn);
	};
	kernel("add", addExpression);
	kernel("fib", fib);
	kernel("sum", sumArr);
	kernel("internalCallLoop", internalCallLoop);
	kernel("straight", straight);
	kernel("calls", calls);
	kernel("chainedIf", chainedIf);
	kernel("chainedIfElse", chainedIfElse);
	kernel("chainedIfRegion", chainedIfRegion);
	kernel("loopBody", loopBody);
	std::string name = argc > 1 ? argv[1] : "distinctSites1024";
	kernelSize = 500;
	if (const auto colon = name.find(':'); colon != std::string::npos) {
		kernelSize = std::atoi(name.c_str() + colon + 1);
		name.resize(colon);
	}
	int reps = argc > 2 ? std::atoi(argv[2]) : 1;
	auto it = corpus.find(name);
	if (it == corpus.end()) {
		std::fprintf(stderr, "unknown function '%s'\n", name.c_str());
		return 1;
	}
	// mode: "" the 0.3 tracer; "ir" the 0.3 tracer and its IR generation; "nir" the NIR tracer (including the write-out
	// to a module); "nirconv" the NIR tracer and the conversion to SSA
	const std::string mode = argc > 3 ? argv[3] : "";
	const bool direct = mode == "nir" || mode == "nirconv";
	common::ArenaPool irPool;
	uint64_t operations = 0, snapshots = 0;
	for (int i = 0; i < reps; ++i) {
		common::Arena arena;
		std::list<compiler::CompilableFunction> functions;
		functions.emplace_back("execute", it->second);
		if (direct) {
			auto result = tracing::nirtrace::NirTracer::Trace(functions, engine::Options {}, arena);
			operations = result.statistics.operations;
			snapshots = result.statistics.snapshots;
			if (mode == "nirconv") {
				auto ssa = conversion::convert(result.module);
				if (ssa.insts.empty()) {
					return 2;
				}
			}
		} else {
			auto module = tracing::TraceContext::Trace(functions, engine::Options {}, arena);
			operations = module->statistics.operations;
			snapshots = module->statistics.snapshots;
			if (mode == "ir") {
				auto graph = tracing::TraceToIRConversionPhase().apply(std::move(module), irPool, "bench");
				if (!graph) {
					return 2;
				}
			}
		}
	}
	std::printf("%s: %llu operations, %llu snapshots\n", name.c_str(), static_cast<unsigned long long>(operations),
	            static_cast<unsigned long long>(snapshots));
	return 0;
}

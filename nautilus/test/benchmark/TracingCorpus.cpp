// Traces one function of the tracing corpus a number of times and does nothing else, so that a profiler (callgrind)
// sees the tracer alone: `nautilus-tracing-corpus [function[:N] [reps [nir]]]`; `nir` drives the direct NIR tracer.
#include "CompileKernels.hpp"
#include "DistinctSitesFunctions.hpp"
#include "NestedIfBenchmarks.hpp"
#include "TracingUtil.hpp"
#include "nautilus/CompilableFunction.hpp"
#include "nautilus/common/Arena.hpp"
#include "nautilus/tracing/ExecutionTrace.hpp"
#include "nautilus/tracing/TraceContext.hpp"
#include "nautilus/tracing/nir/NirTracer.hpp"
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
	const bool direct = argc > 3 && std::strcmp(argv[3], "nir") == 0;
	uint64_t operations = 0, snapshots = 0;
	for (int i = 0; i < reps; ++i) {
		common::Arena arena;
		std::list<compiler::CompilableFunction> functions;
		functions.emplace_back("execute", it->second);
		if (direct) {
			auto result = tracing::nirtrace::NirTracer::Trace(functions, engine::Options {}, arena);
			operations = result.statistics.operations;
			snapshots = result.statistics.snapshots;
		} else {
			auto module = tracing::TraceContext::Trace(functions, engine::Options {}, arena);
			operations = module->statistics.operations;
			snapshots = module->statistics.snapshots;
		}
	}
	std::printf("%s: %llu operations, %llu snapshots\n", name.c_str(), static_cast<unsigned long long>(operations),
	            static_cast<unsigned long long>(snapshots));
	return 0;
}

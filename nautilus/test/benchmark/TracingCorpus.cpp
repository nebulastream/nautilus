// Traces one function of the tracing corpus a number of times and does nothing else, so that a profiler (callgrind)
// sees the tracer alone: `nautilus-tracing-corpus [function [reps]]`.
#include "DistinctSitesFunctions.hpp"
#include "NestedIfBenchmarks.hpp"
#include "TracingUtil.hpp"
#include "nautilus/CompilableFunction.hpp"
#include "nautilus/common/Arena.hpp"
#include "nautilus/tracing/ExecutionTrace.hpp"
#include "nautilus/tracing/TraceContext.hpp"
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
	std::string name = argc > 1 ? argv[1] : "distinctSites1024";
	int reps = argc > 2 ? std::atoi(argv[2]) : 1;
	auto it = corpus.find(name);
	if (it == corpus.end()) {
		std::fprintf(stderr, "unknown function '%s'\n", name.c_str());
		return 1;
	}
	uint64_t operations = 0, snapshots = 0;
	for (int i = 0; i < reps; ++i) {
		common::Arena arena;
		std::list<compiler::CompilableFunction> functions;
		functions.emplace_back("execute", it->second);
		auto module = tracing::TraceContext::Trace(functions, engine::Options {}, arena);
		operations = module->statistics.operations;
		snapshots = module->statistics.snapshots;
	}
	std::printf("%s: %llu operations, %llu snapshots\n", name.c_str(), static_cast<unsigned long long>(operations),
	            static_cast<unsigned long long>(snapshots));
	return 0;
}

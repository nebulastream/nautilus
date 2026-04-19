#include "common/GPUFunctions.hpp"
#include "nautilus/CompilableFunction.hpp"
#include "nautilus/Engine.hpp"
#include "nautilus/config.hpp"
#include "nautilus/tracing/ExceptionBasedTraceContext.hpp"
#include "nautilus/tracing/ExecutionTrace.hpp"
#include "nautilus/tracing/LazyTraceContext.hpp"
#include "nautilus/tracing/phases/SSACreationPhase.hpp"
#include "nautilus/tracing/phases/SSAVerifier.hpp"
#include "nautilus/tracing/phases/TraceToIRConversionPhase.hpp"
#include <catch2/catch_all.hpp>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <unistd.h>

namespace nautilus::log::options {
bool getLogAddresses();
void setLogAddresses(bool);
} // namespace nautilus::log::options

namespace nautilus::engine {

static bool checkTestFile(std::string actual, const std::string& category, const std::string& group,
                          const std::string& name) {
	auto groupDir = std::string(GPU_TEST_DATA_FOLDER) + category + "/" + group + "/";
	if (!std::filesystem::exists(groupDir)) {
		std::filesystem::create_directories(groupDir);
	}
	std::string filePath = groupDir + name + ".trace";
	if (!std::filesystem::exists(filePath)) {
		std::cerr << "File does not exist: " << filePath << " Initializing with current trace. Please Rerun.\n";
		std::ofstream file {filePath};
		file << actual;
		return false;
	}
	std::ifstream file(filePath);
	if (!file.is_open()) {
		std::cerr << "Unable to open file " << filePath << std::endl;
		return false;
	}
	std::stringstream expect;
	expect << file.rdbuf();
	if (expect.str() == actual) {
		return true;
	}
	char tmpName[] = "/tmp/actual_trace_XXXXXX";
	int tmpFd = mkstemp(tmpName);
	close(tmpFd);
	std::ofstream tmpfile {tmpName};
	tmpfile << actual;
	std::cerr << "Trace mismatch: (exp vs act) " << filePath << " " << tmpName << std::endl;
	std::cerr << "=== Expected (" << filePath << ") ===\n"
	          << expect.str() << "\n=== Actual ===\n"
	          << actual << "\n=== End ===\n";
	return false;
}

using TraceFn = std::unique_ptr<tracing::TraceModule> (*)(std::list<compiler::CompilableFunction>&,
                                                          const engine::Options&);

static auto traceContexts = std::vector<std::tuple<std::string, TraceFn>> {
    {"ExceptionBasedTraceContext", tracing::ExceptionBasedTraceContext::Trace},
    {"LazyTraceContext", tracing::LazyTraceContext::Trace},
};

static void runTraceTests(const std::string& category,
                          std::vector<std::tuple<std::string, std::function<void()>>>& tests) {
	nautilus::log::options::setLogAddresses(false);
	for (auto& [ctxName, traceFn] : traceContexts) {
		DYNAMIC_SECTION(ctxName) {
			for (auto& [name, func] : tests) {
				DYNAMIC_SECTION(name) {
					auto rootFunction = compiler::CompilableFunction("execute", func);
					std::list<compiler::CompilableFunction> functionsToTrace;
					functionsToTrace.push_back(rootFunction);
					auto executionTrace = traceFn(functionsToTrace, engine::Options());
					DYNAMIC_SECTION("tracing") {
						REQUIRE(checkTestFile(executionTrace.get()->toString(), category, "tracing", name));
					}
					auto ssaCreationPhase = tracing::SSACreationPhase();
					auto afterSSA =
					    ssaCreationPhase.apply(std::shared_ptr<tracing::TraceModule>(std::move(executionTrace)));
					DYNAMIC_SECTION("after_ssa") {
						REQUIRE(checkTestFile(afterSSA.get()->toString(), category, "after_ssa", name));
					}
					DYNAMIC_SECTION("ssa_verify") {
						for (const auto& fnName : afterSSA->getFunctionNames()) {
							auto ssaResult = tracing::VerifySSA(*afterSSA->getFunction(fnName));
							if (!ssaResult.valid) {
								for (const auto& error : ssaResult.errors) {
									FAIL(error);
								}
							}
						}
					}
					DYNAMIC_SECTION("ir") {
						auto irGenerationPhase = tracing::TraceToIRConversionPhase();
						[[maybe_unused]] auto ir = irGenerationPhase.apply(std::move(afterSSA));
						REQUIRE(checkTestFile(ir.get()->toString(), category, "ir", name));
					}
				}
			}
		}
	}
}

TEST_CASE("GPU Trace Test") {
	auto tests = std::vector<std::tuple<std::string, std::function<void()>>> {
	    {"gpuThreadIdxX", details::createFunctionWrapper(gpuThreadIdxX)},
	    {"gpuBlockIdxX", details::createFunctionWrapper(gpuBlockIdxX)},
	    {"gpuBlockDimX", details::createFunctionWrapper(gpuBlockDimX)},
	    {"gpuGridDimX", details::createFunctionWrapper(gpuGridDimX)},
	    {"gpuGlobalThreadIdx", details::createFunctionWrapper(gpuGlobalThreadIdx)},
	    {"gpuThreadIdx3D", details::createFunctionWrapper(gpuThreadIdx3D)},
	    {"gpuSyncThreadsOnly", details::createFunctionWrapper(gpuSyncThreadsOnly)},
	    {"gpuBoundedIndex", details::createFunctionWrapper(gpuBoundedIndex)},
	    {"gpuVectorAdd", details::createFunctionWrapper(gpuVectorAdd)},
	    {"gpuLaunchVecAdd", details::createFunctionWrapper(gpuLaunchVecAdd)},
	    {"gpuLaunchVecAddDynamic", details::createFunctionWrapper(gpuLaunchVecAddDynamic)},
	    {"gpuLaunchSaxpy", details::createFunctionWrapper(gpuLaunchSaxpy)},
	    {"gpuLaunchVecScale", details::createFunctionWrapper(gpuLaunchVecScale)},
	    {"gpuLaunchVecAddBounded", details::createFunctionWrapper(gpuLaunchVecAddBounded)},
	    {"gpuLaunchClassify", details::createFunctionWrapper(gpuLaunchClassify)},
	    {"gpuLaunchPrefixSum", details::createFunctionWrapper(gpuLaunchPrefixSum)},
	};
	runTraceTests("gpu-tests", tests);
}

} // namespace nautilus::engine


#include "ValueTypeFunctions.hpp"
#include "nautilus/CompilableFunction.hpp"
#include "nautilus/Engine.hpp"
#include "nautilus/tracing/ExceptionBasedTraceContext.hpp"
#include "nautilus/tracing/LazyTraceContext.hpp"
#include "nautilus/tracing/phases/SSACreationPhase.hpp"
#include "nautilus/tracing/phases/SSAVerifier.hpp"
#include "nautilus/tracing/phases/TraceToIRConversionPhase.hpp"
#include <catch2/catch_all.hpp>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <unistd.h>

namespace nautilus::log::options {
bool getLogAddresses();
void setLogAddresses(bool);
} // namespace nautilus::log::options

namespace nautilus::engine {

bool checkValueTypeTestFile(std::string actual, const std::string& category, const std::string& group,
                             const std::string& name) {
	auto groupDir = std::string(STD_PLUGIN_TEST_DATA_FOLDER) + category + "/" + group + "/";
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

static auto valueTypeTraceContexts = std::vector<std::tuple<std::string, TraceFn>> {
    {"ExceptionBasedTraceContext", tracing::ExceptionBasedTraceContext::Trace},
    {"LazyTraceContext", tracing::LazyTraceContext::Trace},
};

void runValueTypeTraceTests(const std::string& category,
                             std::vector<std::tuple<std::string, std::function<void()>>>& tests) {
	nautilus::log::options::setLogAddresses(false);
	for (auto& [ctxName, traceFn] : valueTypeTraceContexts) {
		DYNAMIC_SECTION(ctxName) {
			for (auto& [name, func] : tests) {
				DYNAMIC_SECTION(name) {
					auto rootFunction = compiler::CompilableFunction("execute", func);
					std::list<compiler::CompilableFunction> functionsToTrace;
					functionsToTrace.push_back(rootFunction);

					auto executionTrace = traceFn(functionsToTrace, engine::Options());
					DYNAMIC_SECTION("tracing") {
						REQUIRE(checkValueTypeTestFile(executionTrace.get()->toString(), category, "tracing", name));
					}
					auto ssaCreationPhase = tracing::SSACreationPhase();
					auto afterSSA =
					    ssaCreationPhase.apply(std::shared_ptr<tracing::TraceModule>(std::move(executionTrace)));
					DYNAMIC_SECTION("after_ssa") {
						REQUIRE(checkValueTypeTestFile(afterSSA.get()->toString(), category, "after_ssa", name));
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
						REQUIRE(checkValueTypeTestFile(ir.get()->toString(), category, "ir", name));
					}
				}
			}
		}
	}
}

TEST_CASE("Value Trace Test") {
	auto tests = std::vector<std::tuple<std::string, std::function<void()>>> {
	    // default constructor
	    {"constructAndAccess", details::createFunctionWrapper(constructAndAccess)},
	    {"constructSetBothFields", details::createFunctionWrapper(constructSetBothFields)},
	    {"constructNonTrivialDefault", details::createFunctionWrapper(constructNonTrivialDefault)},
	    // parameterised constructor
	    {"constructWithArgs", details::createFunctionWrapper(constructWithArgs)},
	    // copy constructor and copy assignment
	    {"copyConstruct", details::createFunctionWrapper(copyConstruct)},
	    {"copyAssign", details::createFunctionWrapper(copyAssign)},
	    {"copyConstructNonTrivial", details::createFunctionWrapper(copyConstructNonTrivial)},
	    // destructor
	    {"nonTrivialDestructor", details::createFunctionWrapper(nonTrivialDestructor)},
	    // loops
	    {"modifyInLoop", details::createFunctionWrapper(modifyInLoop)},
	    {"copyInLoop", details::createFunctionWrapper(copyInLoop)},
	    {"constructAndCall", details::createFunctionWrapper(constructAndCall)},
	    // struct in conditional return paths
	    {"structInConditionalReturn", details::createFunctionWrapper(structInConditionalReturn)},
	    {"structInBothBranches", details::createFunctionWrapper(structInBothBranches)},
	    // mixed-alignment struct tests
	    {"mixedAlignSetAll", details::createFunctionWrapper(mixedAlignSetAll)},
	    {"mixedAlignNoClobber", details::createFunctionWrapper(mixedAlignNoClobber)},
	    {"reversePaddingAccess", details::createFunctionWrapper(reversePaddingAccess)},
	    {"doubleAndByteAccess", details::createFunctionWrapper(doubleAndByteAccess)},
	    {"mixedAlignModifyInLoop", details::createFunctionWrapper(mixedAlignModifyInLoop)},
	    {"mixedAlignConditionalReturn", details::createFunctionWrapper(mixedAlignConditionalReturn)},
	    // multi-struct, multi-loop tests
	    {"multiStructMultiLoop", details::createFunctionWrapper(multiStructMultiLoop)},
	    {"twoStructsSameLoop", details::createFunctionWrapper(twoStructsSameLoop)},
	    {"outerAndInnerStructLoop", details::createFunctionWrapper(outerAndInnerStructLoop)},
	    {"multiStructConditionalLoop", details::createFunctionWrapper(multiStructConditionalLoop)},
	    {"structInNestedConditionalReturn", details::createFunctionWrapper(structInNestedConditionalReturn)},
	    // nested function + struct tests
	    {"modifyStructInLoopWithNestedCall", details::createFunctionWrapper(modifyStructInLoopWithNestedCall)},
	    {"constructStructInLoopWithNestedCall", details::createFunctionWrapper(constructStructInLoopWithNestedCall)},
	    {"multipleNestedCallsInLoop", details::createFunctionWrapper(multipleNestedCallsInLoop)},
	    // alloca merge bug: different-sized structs with non-overlapping lifetimes
	    {"allocaMergeBug", details::createFunctionWrapper(allocaMergeBug)},
	    {"allocaMergeInLoop", details::createFunctionWrapper(allocaMergeInLoop)},
	};
	runValueTypeTraceTests("value-tracing-tests", tests);
}

} // namespace nautilus::engine

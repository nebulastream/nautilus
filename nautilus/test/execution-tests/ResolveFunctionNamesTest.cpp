#include "ExecutionTest.hpp"
#include <catch2/catch_test_macros.hpp>
#include <cstdint>
#include <list>
#include <nautilus/CompilableFunction.hpp>
#include <nautilus/Engine.hpp>
#include <nautilus/function.hpp>
#include <nautilus/region.hpp>
#include <nautilus/tracing/TraceContext.hpp>
#include <string>
#include <vector>

namespace nautilus { namespace {

#ifdef ENABLE_TRACING

// Not noexcept: invoked through the exception-handling call path.
int32_t resolveNamesAddOne(int32_t x) {
	return x + 1;
}

// noexcept: invoked through the plain call path.
int32_t resolveNamesTimesTwo(int32_t x) noexcept {
	return x * 2;
}

val<int32_t> callsInAndOutsideRegion(val<int32_t> x) {
	val<int32_t> result = invoke(resolveNamesAddOne, x);
	region("inner", [&]() { result = invoke(resolveNamesTimesTwo, invoke(resolveNamesAddOne, result)); });
	return result;
}

/// The (function name, mangled name) of every call recorded while tracing callsInAndOutsideRegion.
std::vector<std::pair<std::string, std::string>> tracedCallNames(bool resolveFunctionNames) {
	common::Arena arena;
	std::list<compiler::CompilableFunction> functions;
	functions.emplace_back("execute", [] {
		val<int32_t> x = 3;
		auto result = callsInAndOutsideRegion(x);
		(void) result;
	});
	engine::Options options;
	options.setOption("engine.resolveFunctionNames", resolveFunctionNames);
	auto module = tracing::TraceContext::Trace(functions, options, arena);
	std::vector<std::pair<std::string, std::string>> names;
	for (const auto* block : module->getFunction("execute")->getBlocks()) {
		for (const auto* operation : block->operations) {
			if (operation->op == tracing::Op::CALL || operation->op == tracing::Op::CALL_WITH_EXCEPTION_HANDLING) {
				const auto* call = std::get<tracing::FunctionCall*>(operation->input[0]);
				names.emplace_back(call->functionName, call->mangledName);
			}
		}
	}
	return names;
}

TEST_CASE("engine.resolveFunctionNames=false names callees by address, inside regions too") {
	const auto unresolved = tracedCallNames(false);
	REQUIRE(unresolved.size() == 3);
	for (const auto& [functionName, mangledName] : unresolved) {
		CHECK(functionName.find("resolveNames") == std::string::npos);
		CHECK(mangledName.find("resolveNames") == std::string::npos);
		CHECK(functionName == mangledName);
		CHECK(functionName.rfind("0x", 0) == 0);
	}
	// Both calls of resolveNamesAddOne -- one outside, one inside the region -- get the same name.
	CHECK(unresolved[0].first == unresolved[1].first);
	CHECK(unresolved[0].first != unresolved[2].first);
}

TEST_CASE("engine.resolveFunctionNames=false does not change the generated code") {
	for (const auto& backend : testing::availableBackends()) {
		DYNAMIC_SECTION(backend) {
			auto engine = testing::makeEngine(
			    backend, [](engine::Options& options) { options.setOption("engine.resolveFunctionNames", false); });
			auto function = engine.registerFunction(callsInAndOutsideRegion);
			REQUIRE(function(3) == 10);
			REQUIRE(function(-1) == 2);
		}
	}
}

#endif

}} // namespace nautilus

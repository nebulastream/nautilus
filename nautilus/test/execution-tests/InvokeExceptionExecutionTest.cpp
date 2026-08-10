#include <catch2/catch_test_macros.hpp>
#include <cstdint>
#include <list>
#include <nautilus/CompilableFunction.hpp>
#include <nautilus/Engine.hpp>
#include <nautilus/function.hpp>
#include <nautilus/tracing/ExceptionBasedTraceContext.hpp>
#include <nautilus/tracing/LazyTraceContext.hpp>
#include <nautilus/val_std.hpp>
#include <stdexcept>

namespace nautilus { namespace {

thread_local uint32_t destructorCalls = 0;
thread_local int32_t destructorValues[4] = {};

struct ExceptionResult {
	int32_t value = 0;
	~ExceptionResult() noexcept {
		destructorValues[destructorCalls++] = value;
	}
};

void writeResult(ExceptionResult* result, int32_t value) noexcept {
	result->value = value;
}

void throwWhileWriting(ExceptionResult*, int32_t) {
	throw std::runtime_error("invoke failed");
}

void throwWithoutStruct() {
	throw std::runtime_error("invoke failed without cleanup");
}

val<int32_t> invokeNoexceptWithStruct() {
	val<ExceptionResult> result;
	invoke(writeResult, &result, val<int32_t> {42});
	return result.get(&ExceptionResult::value);
}

val<int32_t> invokeThrowingWithStruct() {
	val<ExceptionResult> result;
	invoke(throwWhileWriting, &result, val<int32_t> {42});
	return result.get(&ExceptionResult::value);
}

val<int32_t> invokeThrowingWithTwoStructs() {
	val<ExceptionResult> first;
	invoke(writeResult, &first, val<int32_t> {1});
	val<ExceptionResult> second;
	invoke(writeResult, &second, val<int32_t> {2});
	invoke(throwWhileWriting, &second, val<int32_t> {42});
	return 0;
}

val<int32_t> invokeThrowingWithoutStruct() {
	invoke(throwWithoutStruct);
	return 42;
}

engine::NautilusEngine makeMlirEngine(const std::string& traceMode) {
	engine::Options options;
	options.setOption("engine.Compilation", true);
	options.setOption("engine.backend", std::string("mlir"));
	options.setOption("engine.compilationStrategy", std::string("legacy"));
	options.setOption("engine.traceMode", traceMode);
	options.setOption("mlir.enableMultithreading", false);
	return engine::NautilusEngine {options};
}

engine::NautilusEngine makeInterpreterEngine() {
	engine::Options options;
	options.setOption("engine.Compilation", false);
	return engine::NautilusEngine {options};
}

TEST_CASE("MLIR invokes unwind live val<Struct> destructors") {
	for (const auto& traceMode : {std::string("exceptionBasedTracing"), std::string("lazyTracing")}) {
		DYNAMIC_SECTION(traceMode) {
			auto engine = makeMlirEngine(traceMode);
			auto function = engine.registerFunction(invokeThrowingWithStruct);
			destructorCalls = 0;
			REQUIRE_THROWS_AS(function(), std::runtime_error);
			REQUIRE(destructorCalls == 1);
		}
	}
}

TEST_CASE("noexcept MLIR invokes retain the direct call path") {
	auto engine = makeMlirEngine("lazyTracing");
	auto function = engine.registerFunction(invokeNoexceptWithStruct);
	destructorCalls = 0;
	REQUIRE(function() == 42);
	REQUIRE(destructorCalls == 1);
}

TEST_CASE("MLIR exceptional cleanups run in reverse construction order") {
	auto engine = makeMlirEngine("lazyTracing");
	auto function = engine.registerFunction(invokeThrowingWithTwoStructs);
	destructorCalls = 0;
	REQUIRE_THROWS_AS(function(), std::runtime_error);
	REQUIRE(destructorCalls == 2);
	REQUIRE(destructorValues[0] == 2);
	REQUIRE(destructorValues[1] == 1);
}

TEST_CASE("interpreter invokes use native C++ exception unwinding") {
	auto engine = makeInterpreterEngine();
	auto function = engine.registerFunction(invokeThrowingWithStruct);
	destructorCalls = 0;
	REQUIRE_THROWS_AS(function(), std::runtime_error);
	REQUIRE(destructorCalls == 1);
}

TEST_CASE("throwing invokes without live structs retain exception handling") {
	using TraceFn = std::unique_ptr<tracing::TraceModule> (*)(std::list<compiler::CompilableFunction>&,
	                                                          const engine::Options&, common::Arena&);
	for (const auto& [traceMode, traceFn] :
	     {std::pair {"exceptionBasedTracing", TraceFn {tracing::ExceptionBasedTraceContext::Trace}},
	      std::pair {"lazyTracing", TraceFn {tracing::LazyTraceContext::Trace}}}) {
		DYNAMIC_SECTION(traceMode) {
			common::Arena arena;
			std::list<compiler::CompilableFunction> functions;
			functions.emplace_back("execute", [] { (void) invokeThrowingWithoutStruct(); });
			auto module = traceFn(functions, engine::Options {}, arena);
			auto* trace = module->getFunction("execute");

			auto exceptionCalls = size_t {0};
			for (const auto* block : trace->getBlocks()) {
				for (const auto* operation : block->operations) {
					if (operation->op == tracing::Op::CALL_WITH_EXCEPTION_HANDLING) {
						const auto* call = std::get<tracing::FunctionCall*>(operation->input[0]);
						REQUIRE(call->destructors.empty());
						++exceptionCalls;
					}
				}
			}
			REQUIRE(exceptionCalls == 1);

			auto engine = makeMlirEngine(traceMode);
			auto function = engine.registerFunction(invokeThrowingWithoutStruct);
			REQUIRE_THROWS_AS(function(), std::runtime_error);
		}
	}
}

}} // namespace nautilus

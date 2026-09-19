#include "ExecutionTest.hpp"
#include "nautilus/Engine.hpp"
#include "nautilus/config.hpp"
#include "nautilus/val.hpp"
#include "nautilus/val_std.hpp"
#include <catch2/catch_all.hpp>
#include <stdexcept>

// BCInterpreter::execute() picks a loop variant per basic block based on
// CodeBlock::hasPendingCheck, nested inside the call/switch dispatch-mode
// branch; the threaded path handles the check as its own computed-goto
// label. All three need to actually take the pending-exception branch, not
// just agree on ordinary (non-throwing) results the way BCDispatchModeTest
// checks.
#if defined(ENABLE_TRACING) && defined(ENABLE_BC_BACKEND)

namespace nautilus::engine {

namespace {

thread_local uint32_t destructorCalls = 0;

struct ExceptionResult {
	int32_t value = 0;
	~ExceptionResult() noexcept {
		++destructorCalls;
	}
};

void throwWhileWriting(ExceptionResult*, int32_t) {
	throw std::runtime_error("invoke failed");
}

val<int32_t> invokeThrowingWithStruct() {
	val<ExceptionResult> result;
	invoke(throwWhileWriting, &result, val<int32_t> {42});
	return result.get(&ExceptionResult::value);
}

engine::NautilusEngine makeBcEngine(const std::string& traceMode, const std::string& dispatch) {
	engine::Options options;
	options.setOption("engine.Compilation", true);
	options.setOption("engine.backend", std::string("bc"));
	options.setOption("engine.compilationStrategy", std::string("legacy"));
	options.setOption("engine.traceMode", traceMode);
	options.setOption("bc.dispatch", dispatch);
	return engine::NautilusEngine {options};
}

} // namespace

TEST_CASE("BC backend unwinds destructors under every dispatch mode") {
	for (const auto& dispatch : {std::string("call"), std::string("switch"), std::string("threaded")}) {
		DYNAMIC_SECTION(dispatch) {
			auto engine = makeBcEngine("lazyTracing", dispatch);
			auto function = engine.registerFunction(invokeThrowingWithStruct);
			destructorCalls = 0;
			REQUIRE_THROWS_AS(function(), std::runtime_error);
			REQUIRE(destructorCalls == 1);
		}
	}
}

} // namespace nautilus::engine

#endif

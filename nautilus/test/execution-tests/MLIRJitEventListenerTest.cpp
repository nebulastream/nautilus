#include "nautilus/config.hpp"

#if defined(ENABLE_TRACING) && defined(ENABLE_MLIR_BACKEND)

#include "ExecutionTest.hpp"
#include "MLIRJitEventListenerTestFixture.hpp"
#include "nautilus/Engine.hpp"
#include <catch2/catch_all.hpp>

namespace nautilus::engine {

namespace {

val<int32_t> listenerAddOne(val<int32_t> x) {
	return x + 1;
}

} // namespace

TEST_CASE("MLIR JIT event listener receives object-loaded notifications") {
	nautilus::testing::CountingJITEventListenerHandle handle;

	Options options;
	options.setOption("engine.backend", std::string("mlir"));
	options.addMLIRJitEventListener(handle.listener());

	{
		NautilusEngine engine(options);
		auto fn = engine.registerFunction(listenerAddOne);
		REQUIRE(fn(41) == 42);
	}

	REQUIRE(handle.numObjectsLoaded() > 0);
}

TEST_CASE("MLIR JIT event listener: multiple listeners all receive events") {
	nautilus::testing::CountingJITEventListenerHandle first;
	nautilus::testing::CountingJITEventListenerHandle second;

	Options options;
	options.setOption("engine.backend", std::string("mlir"));
	options.addMLIRJitEventListener(first.listener());
	options.addMLIRJitEventListener(second.listener());

	{
		NautilusEngine engine(options);
		auto fn = engine.registerFunction(listenerAddOne);
		REQUIRE(fn(0) == 1);
	}

	REQUIRE(first.numObjectsLoaded() > 0);
	REQUIRE(second.numObjectsLoaded() == first.numObjectsLoaded());
}

TEST_CASE("MLIR JIT event listener: null entries are ignored") {
	Options options;
	options.setOption("engine.backend", std::string("mlir"));
	options.addMLIRJitEventListener(nullptr);

	NautilusEngine engine(options);
	auto fn = engine.registerFunction(listenerAddOne);
	REQUIRE(fn(7) == 8);
}

} // namespace nautilus::engine

#endif // ENABLE_TRACING && ENABLE_MLIR_BACKEND

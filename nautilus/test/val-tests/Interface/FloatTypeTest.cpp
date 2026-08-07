
#include "ExecutionTest.hpp"
#include "nautilus/Engine.hpp"
#include "nautilus/val.hpp"
#include <catch2/catch_all.hpp>
#include <cmath>
#include <limits>

namespace nautilus::engine {

template <typename T>
val<T> floatAdd(val<T> a, val<T> b) {
	return a + b;
}

template <typename T>
val<T> floatSub(val<T> a, val<T> b) {
	return a - b;
}

template <typename T>
val<T> floatMul(val<T> a, val<T> b) {
	return a * b;
}

template <typename T>
val<T> floatDiv(val<T> a, val<T> b) {
	return a / b;
}

template <typename T>
val<bool> floatEq(val<T> a, val<T> b) {
	return a == b;
}

template <typename T>
val<bool> floatNe(val<T> a, val<T> b) {
	return a != b;
}

template <typename T>
val<bool> floatLt(val<T> a, val<T> b) {
	return a < b;
}

template <typename T>
val<bool> floatGt(val<T> a, val<T> b) {
	return a > b;
}

template <typename T>
val<bool> floatLe(val<T> a, val<T> b) {
	return a <= b;
}

template <typename T>
val<bool> floatGe(val<T> a, val<T> b) {
	return a >= b;
}

template <typename T>
void floatTest(NautilusEngine& eng) {
	SECTION("arithmetic") {
		SECTION("add") {
			auto f = eng.registerFunction(floatAdd<T>);
			REQUIRE(f(static_cast<T>(5.0), static_cast<T>(3.0)) == static_cast<T>(8.0));
			REQUIRE(f(static_cast<T>(0.0), static_cast<T>(0.0)) == static_cast<T>(0.0));
		}
		SECTION("sub") {
			auto f = eng.registerFunction(floatSub<T>);
			REQUIRE(f(static_cast<T>(5.0), static_cast<T>(3.0)) == static_cast<T>(2.0));
			REQUIRE(f(static_cast<T>(5.0), static_cast<T>(5.0)) == static_cast<T>(0.0));
		}
		SECTION("mul") {
			auto f = eng.registerFunction(floatMul<T>);
			REQUIRE(f(static_cast<T>(5.0), static_cast<T>(2.0)) == static_cast<T>(10.0));
			REQUIRE(f(static_cast<T>(5.0), static_cast<T>(0.0)) == static_cast<T>(0.0));
		}
		SECTION("div") {
			auto f = eng.registerFunction(floatDiv<T>);
			REQUIRE(f(static_cast<T>(6.0), static_cast<T>(2.0)) == static_cast<T>(3.0));
			REQUIRE(f(static_cast<T>(10.0), static_cast<T>(1.0)) == static_cast<T>(10.0));
		}
	}
	SECTION("comparison") {
		SECTION("eq") {
			auto f = eng.registerFunction(floatEq<T>);
			REQUIRE(f(static_cast<T>(42.0), static_cast<T>(42.0)) == true);
			REQUIRE(f(static_cast<T>(42.0), static_cast<T>(1.0)) == false);
		}
		SECTION("ne") {
			auto f = eng.registerFunction(floatNe<T>);
			REQUIRE(f(static_cast<T>(42.0), static_cast<T>(1.0)) == true);
			REQUIRE(f(static_cast<T>(42.0), static_cast<T>(42.0)) == false);
		}
		SECTION("lt") {
			auto f = eng.registerFunction(floatLt<T>);
			REQUIRE(f(static_cast<T>(1.0), static_cast<T>(42.0)) == true);
			REQUIRE(f(static_cast<T>(42.0), static_cast<T>(1.0)) == false);
			REQUIRE(f(static_cast<T>(42.0), static_cast<T>(42.0)) == false);
		}
		SECTION("gt") {
			auto f = eng.registerFunction(floatGt<T>);
			REQUIRE(f(static_cast<T>(42.0), static_cast<T>(1.0)) == true);
			REQUIRE(f(static_cast<T>(1.0), static_cast<T>(42.0)) == false);
			REQUIRE(f(static_cast<T>(42.0), static_cast<T>(42.0)) == false);
		}
		SECTION("le") {
			auto f = eng.registerFunction(floatLe<T>);
			REQUIRE(f(static_cast<T>(42.0), static_cast<T>(42.0)) == true);
			REQUIRE(f(static_cast<T>(1.0), static_cast<T>(42.0)) == true);
			REQUIRE(f(static_cast<T>(42.0), static_cast<T>(1.0)) == false);
		}
		SECTION("ge") {
			auto f = eng.registerFunction(floatGe<T>);
			REQUIRE(f(static_cast<T>(42.0), static_cast<T>(42.0)) == true);
			REQUIRE(f(static_cast<T>(42.0), static_cast<T>(1.0)) == true);
			REQUIRE(f(static_cast<T>(1.0), static_cast<T>(42.0)) == false);
		}
	}
	SECTION("special values") {
		SECTION("positive infinity") {
			auto f = eng.registerFunction(floatAdd<T>);
			auto inf = std::numeric_limits<T>::infinity();
			auto result = f(inf, static_cast<T>(1.0));
			REQUIRE(std::isinf(result));
			REQUIRE(result > static_cast<T>(0));
		}
		SECTION("NaN comparison is always false") {
			auto f = eng.registerFunction(floatEq<T>);
			auto nan = std::numeric_limits<T>::quiet_NaN();
			REQUIRE(f(nan, nan) == false);
		}
		SECTION("NaN ne is always true") {
			auto f = eng.registerFunction(floatNe<T>);
			auto nan = std::numeric_limits<T>::quiet_NaN();
			REQUIRE(f(nan, nan) == true);
		}
		SECTION("negative zero equals zero") {
			auto f = eng.registerFunction(floatEq<T>);
			REQUIRE(f(static_cast<T>(-0.0), static_cast<T>(0.0)) == true);
		}
	}
}

TEMPLATE_TEST_CASE("Float Interface Interpreter Test", "[value][interface][template]", float, double) {
	auto eng = nautilus::testing::makeEngine("interpreter");
	floatTest<TestType>(eng);
}

#ifdef ENABLE_TRACING
TEMPLATE_TEST_CASE("Float Interface Compiler Test", "[value][interface][template]", float, double) {
	const std::vector<std::string> traceModes = {"exceptionBasedTracing", "lazyTracing"};
	for (const auto& backend : nautilus::testing::availableBackends()) {
		for (const auto& traceMode : traceModes) {
			DYNAMIC_SECTION(backend + "_" + traceMode) {
				auto eng = nautilus::testing::makeEngine(
				    backend, [&](engine::Options& options) { options.setOption("engine.traceMode", traceMode); });
				floatTest<TestType>(eng);
			}
		}
	}
}
#endif

} // namespace nautilus::engine

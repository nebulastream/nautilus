
#include "ExecutionTest.hpp"
#include "nautilus/Engine.hpp"
#include "nautilus/val.hpp"
#include <catch2/catch_all.hpp>
#include <limits>

namespace nautilus::engine {

template <typename T>
val<T> integerAdd(val<T> a, val<T> b) {
	return a + b;
}

template <typename T>
val<T> integerSub(val<T> a, val<T> b) {
	return a - b;
}

template <typename T>
val<T> integerMul(val<T> a, val<T> b) {
	return a * b;
}

template <typename T>
val<T> integerDiv(val<T> a, val<T> b) {
	return a / b;
}

template <typename T>
val<T> integerMod(val<T> a, val<T> b) {
	return a % b;
}

template <typename T>
val<bool> integerEq(val<T> a, val<T> b) {
	return a == b;
}

template <typename T>
val<bool> integerNe(val<T> a, val<T> b) {
	return a != b;
}

template <typename T>
val<bool> integerLt(val<T> a, val<T> b) {
	return a < b;
}

template <typename T>
val<bool> integerGt(val<T> a, val<T> b) {
	return a > b;
}

template <typename T>
val<bool> integerLe(val<T> a, val<T> b) {
	return a <= b;
}

template <typename T>
val<bool> integerGe(val<T> a, val<T> b) {
	return a >= b;
}

template <typename T>
val<T> integerBitAnd(val<T> a, val<T> b) {
	return a & b;
}

template <typename T>
val<T> integerBitOr(val<T> a, val<T> b) {
	return a | b;
}

template <typename T>
val<T> integerBitXor(val<T> a, val<T> b) {
	return a ^ b;
}

template <typename T>
val<T> integerShl(val<T> a, val<T> b) {
	return a << b;
}

template <typename T>
val<T> integerShr(val<T> a, val<T> b) {
	return a >> b;
}

template <typename T>
void integerTest(NautilusEngine& eng) {
	SECTION("arithmetic") {
		SECTION("add") {
			auto f = eng.registerFunction(integerAdd<T>);
			REQUIRE(f(static_cast<T>(2), static_cast<T>(3)) == static_cast<T>(5));
			REQUIRE(f(static_cast<T>(0), static_cast<T>(0)) == static_cast<T>(0));
			REQUIRE(f(static_cast<T>(10), static_cast<T>(5)) == static_cast<T>(15));
		}
		SECTION("sub") {
			auto f = eng.registerFunction(integerSub<T>);
			REQUIRE(f(static_cast<T>(10), static_cast<T>(3)) == static_cast<T>(7));
			REQUIRE(f(static_cast<T>(5), static_cast<T>(5)) == static_cast<T>(0));
		}
		SECTION("mul") {
			auto f = eng.registerFunction(integerMul<T>);
			REQUIRE(f(static_cast<T>(3), static_cast<T>(4)) == static_cast<T>(12));
			REQUIRE(f(static_cast<T>(5), static_cast<T>(0)) == static_cast<T>(0));
			REQUIRE(f(static_cast<T>(1), static_cast<T>(42)) == static_cast<T>(42));
		}
		SECTION("div") {
			auto f = eng.registerFunction(integerDiv<T>);
			REQUIRE(f(static_cast<T>(12), static_cast<T>(4)) == static_cast<T>(3));
			REQUIRE(f(static_cast<T>(10), static_cast<T>(1)) == static_cast<T>(10));
			REQUIRE(f(static_cast<T>(7), static_cast<T>(2)) == static_cast<T>(3));
		}
		SECTION("mod") {
			auto f = eng.registerFunction(integerMod<T>);
			REQUIRE(f(static_cast<T>(13), static_cast<T>(5)) == static_cast<T>(3));
			REQUIRE(f(static_cast<T>(10), static_cast<T>(5)) == static_cast<T>(0));
			REQUIRE(f(static_cast<T>(3), static_cast<T>(10)) == static_cast<T>(3));
		}
	}
	SECTION("comparison") {
		SECTION("eq") {
			auto f = eng.registerFunction(integerEq<T>);
			REQUIRE(f(static_cast<T>(42), static_cast<T>(42)) == true);
			REQUIRE(f(static_cast<T>(42), static_cast<T>(1)) == false);
		}
		SECTION("ne") {
			auto f = eng.registerFunction(integerNe<T>);
			REQUIRE(f(static_cast<T>(42), static_cast<T>(1)) == true);
			REQUIRE(f(static_cast<T>(42), static_cast<T>(42)) == false);
		}
		SECTION("lt") {
			auto f = eng.registerFunction(integerLt<T>);
			REQUIRE(f(static_cast<T>(1), static_cast<T>(42)) == true);
			REQUIRE(f(static_cast<T>(42), static_cast<T>(1)) == false);
			REQUIRE(f(static_cast<T>(42), static_cast<T>(42)) == false);
		}
		SECTION("gt") {
			auto f = eng.registerFunction(integerGt<T>);
			REQUIRE(f(static_cast<T>(42), static_cast<T>(1)) == true);
			REQUIRE(f(static_cast<T>(1), static_cast<T>(42)) == false);
			REQUIRE(f(static_cast<T>(42), static_cast<T>(42)) == false);
		}
		SECTION("le") {
			auto f = eng.registerFunction(integerLe<T>);
			REQUIRE(f(static_cast<T>(42), static_cast<T>(42)) == true);
			REQUIRE(f(static_cast<T>(1), static_cast<T>(42)) == true);
			REQUIRE(f(static_cast<T>(42), static_cast<T>(1)) == false);
		}
		SECTION("ge") {
			auto f = eng.registerFunction(integerGe<T>);
			REQUIRE(f(static_cast<T>(42), static_cast<T>(42)) == true);
			REQUIRE(f(static_cast<T>(42), static_cast<T>(1)) == true);
			REQUIRE(f(static_cast<T>(1), static_cast<T>(42)) == false);
		}
	}
	SECTION("bitwise") {
		SECTION("and") {
			auto f = eng.registerFunction(integerBitAnd<T>);
			REQUIRE(f(static_cast<T>(42), static_cast<T>(42)) == static_cast<T>(42));
			REQUIRE(f(static_cast<T>(0), static_cast<T>(42)) == static_cast<T>(0));
		}
		SECTION("or") {
			auto f = eng.registerFunction(integerBitOr<T>);
			REQUIRE(f(static_cast<T>(42), static_cast<T>(0)) == static_cast<T>(42));
			REQUIRE(f(static_cast<T>(0), static_cast<T>(0)) == static_cast<T>(0));
		}
		SECTION("xor") {
			auto f = eng.registerFunction(integerBitXor<T>);
			REQUIRE(f(static_cast<T>(42), static_cast<T>(42)) == static_cast<T>(0));
			REQUIRE(f(static_cast<T>(42), static_cast<T>(0)) == static_cast<T>(42));
		}
		SECTION("shl") {
			auto f = eng.registerFunction(integerShl<T>);
			REQUIRE(f(static_cast<T>(1), static_cast<T>(2)) == static_cast<T>(4));
			REQUIRE(f(static_cast<T>(8), static_cast<T>(1)) == static_cast<T>(16));
		}
		SECTION("shr") {
			auto f = eng.registerFunction(integerShr<T>);
			REQUIRE(f(static_cast<T>(8), static_cast<T>(2)) == static_cast<T>(2));
			REQUIRE(f(static_cast<T>(16), static_cast<T>(1)) == static_cast<T>(8));
		}
	}
}

TEMPLATE_TEST_CASE("Integer Interface Interpreter Test", "[value][interface][template]", int8_t, int16_t, int32_t,
                   int64_t, uint8_t, uint16_t, uint32_t, uint64_t) {
	auto eng = nautilus::testing::makeEngine("interpreter");
	integerTest<TestType>(eng);
}

#ifdef ENABLE_TRACING
TEMPLATE_TEST_CASE("Integer Interface Compiler Test", "[value][interface][template]", int8_t, int16_t, int32_t, int64_t,
                   uint8_t, uint16_t, uint32_t, uint64_t) {
	const std::vector<std::string> traceModes = {"exceptionBasedTracing", "lazyTracing"};
	for (const auto& backend : nautilus::testing::availableBackends()) {
		for (const auto& traceMode : traceModes) {
			DYNAMIC_SECTION(backend + "_" + traceMode) {
				auto eng = nautilus::testing::makeEngine(
				    backend, [&](engine::Options& options) { options.setOption("engine.traceMode", traceMode); });
				integerTest<TestType>(eng);
			}
		}
	}
}
#endif

} // namespace nautilus::engine

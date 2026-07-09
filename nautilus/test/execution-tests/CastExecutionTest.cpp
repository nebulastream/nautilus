#include "CastFunctions.hpp"
#include "ExecutionTest.hpp"
#include "nautilus/Engine.hpp"
#include "nautilus/val_concepts.hpp"
#include <catch2/catch_all.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
namespace nautilus::engine {

template <typename BaseType>
void createCastTest(engine::NautilusEngine& engine, std::string name, BaseType min, BaseType max) {
	DYNAMIC_SECTION(name) {
		BaseType zero = 0;
		SECTION("to_i8") {
			auto f = engine.registerFunction(staticCastExpression<BaseType, int8_t>);
			REQUIRE(f(zero) == static_cast<int8_t>(zero));
			REQUIRE(f(min) == static_cast<int8_t>(min));
			REQUIRE(f(max) == static_cast<int8_t>(max));
		}
		SECTION("to_i16") {
			auto f = engine.registerFunction(staticCastExpression<BaseType, int16_t>);
			REQUIRE(f(zero) == static_cast<int16_t>(zero));
			REQUIRE(f(min) == static_cast<int16_t>(min));
			REQUIRE(f(max) == static_cast<int16_t>(max));
		}
		SECTION("to_i32") {
			auto f = engine.registerFunction(staticCastExpression<BaseType, int32_t>);
			REQUIRE(f(zero) == static_cast<int32_t>(zero));
			REQUIRE(f(min) == static_cast<int32_t>(min));
			REQUIRE(f(max) == static_cast<int32_t>(max));
		}
		SECTION("to_i64") {
			auto f = engine.registerFunction(staticCastExpression<BaseType, int64_t>);
			REQUIRE(f(zero) == static_cast<int64_t>(zero));
			REQUIRE(f(min) == static_cast<int64_t>(min));
			REQUIRE(f(max) == static_cast<int64_t>(max));
		}
		SECTION("to_ui8") {
			auto f = engine.registerFunction(staticCastExpression<BaseType, uint8_t>);
			REQUIRE(f(zero) == static_cast<uint8_t>(zero));
			auto res = f(min);
			auto ref = static_cast<unsigned char>(min);
			REQUIRE(ref == res);
			auto res2 = f(max);
			auto ref2 = static_cast<unsigned char>(max);
			REQUIRE(ref2 == res2);
		}

		SECTION("to_ui16") {
			auto f = engine.registerFunction(staticCastExpression<BaseType, uint16_t>);
			REQUIRE(f(zero) == static_cast<uint16_t>(zero));
			[[maybe_unused]] auto castValue = static_cast<uint16_t>(min);
			REQUIRE(f(min) == static_cast<uint16_t>(min));
			REQUIRE(f(max) == static_cast<uint16_t>(max));
		}
		SECTION("to_ui32") {
			auto f = engine.registerFunction(staticCastExpression<BaseType, uint32_t>);
			REQUIRE(f(zero) == static_cast<uint32_t>(zero));
			REQUIRE(f(min) == static_cast<uint32_t>(min));
			REQUIRE(f(max) == static_cast<uint32_t>(max));
		}
		SECTION("to_ui64") {
			auto f = engine.registerFunction(staticCastExpression<BaseType, uint64_t>);
			REQUIRE(f(zero) == static_cast<uint64_t>(zero));
			REQUIRE(f(min) == static_cast<uint64_t>(min));
			REQUIRE(f(max) == static_cast<uint64_t>(max));
		}
		SECTION("to_float") {
			auto f = engine.registerFunction(staticCastExpression<BaseType, float>);
			REQUIRE(f(zero) == static_cast<float>(zero));
			REQUIRE(f(min) == static_cast<float>(min));
			REQUIRE(f(max) == static_cast<float>(max));
		}
		SECTION("to_double") {
			auto f = engine.registerFunction(staticCastExpression<BaseType, double>);
			REQUIRE(f(zero) == static_cast<double>(zero));
			REQUIRE(f(min) == static_cast<double>(min));
			REQUIRE(f(max) == static_cast<double>(max));
		}
		/*SECTION("to_bool") {
		    auto f = engine.registerFunction(staticCastExpression<BaseType, bool>);
		    REQUIRE(f(zero) == static_cast<bool>(zero));
		    REQUIRE(f(BaseType(1)) == static_cast<bool>(BaseType(1)));
		    REQUIRE(f(min) == static_cast<bool>(min));
		    REQUIRE(f(max) == static_cast<bool>(max));
		}*/
	}
}

void castTest(engine::NautilusEngine& engine) {
	createCastTest<int8_t>(engine, "cast_from_i8", INT8_MIN, INT8_MAX);
	createCastTest<int16_t>(engine, "cast_from_i16", INT16_MIN, INT16_MAX);
	createCastTest<int32_t>(engine, "cast_from_i32", INT32_MIN, INT32_MAX);
	createCastTest<int64_t>(engine, "cast_from_i64", INT64_MIN, INT64_MAX);
	createCastTest<uint8_t>(engine, "cast_from_ui8", 0, UINT8_MAX);
	createCastTest<uint16_t>(engine, "cast_from_ui16", 0, UINT16_MAX);
	createCastTest<uint32_t>(engine, "cast_from_ui32", 0, UINT32_MAX);
	createCastTest<uint64_t>(engine, "cast_from_ui64", 0, UINT64_MAX);
	createCastTest<bool>(engine, "cast_from_bool", true, false);
	// createCastTest<float>(engine, "cast_from_float",
	// std::numeric_limits<float>::min(), std::numeric_limits<float>::max());
	// createCastTest<double>(engine, "cast_from_double",
	// std::numeric_limits<double>::min(), std::numeric_limits<double>::max());
}

template <typename OutType>
void createPtrToArithSection(engine::NautilusEngine& engine, std::string name) {
	DYNAMIC_SECTION(name) {
		auto f = engine.registerFunction(staticCastExpression<void*, OutType>);
		int32_t x = 42;
		void* ptr = &x;
		REQUIRE(f(ptr) == static_cast<OutType>(reinterpret_cast<uintptr_t>(ptr)));
	}
}

void ptrCastTest(engine::NautilusEngine& engine) {
	createPtrToArithSection<int8_t>(engine, "ptr_to_i8");
	createPtrToArithSection<int16_t>(engine, "ptr_to_i16");
	createPtrToArithSection<int32_t>(engine, "ptr_to_i32");
	createPtrToArithSection<int64_t>(engine, "ptr_to_i64");
	createPtrToArithSection<uint8_t>(engine, "ptr_to_ui8");
	createPtrToArithSection<uint16_t>(engine, "ptr_to_ui16");
	createPtrToArithSection<uint32_t>(engine, "ptr_to_ui32");
	createPtrToArithSection<uint64_t>(engine, "ptr_to_ui64");
	createPtrToArithSection<float>(engine, "ptr_to_float");
	createPtrToArithSection<double>(engine, "ptr_to_double");

	SECTION("i64_to_ptr") {
		auto f = engine.registerFunction(staticCastExpression<int64_t, void*>);
		int32_t x = 42;
		auto intVal = static_cast<int64_t>(reinterpret_cast<uintptr_t>(&x));
		REQUIRE(f(intVal) == reinterpret_cast<void*>(static_cast<uintptr_t>(intVal)));
	}
	SECTION("ui64_to_ptr") {
		auto f = engine.registerFunction(staticCastExpression<uint64_t, void*>);
		int32_t x = 42;
		auto intVal = static_cast<uint64_t>(reinterpret_cast<uintptr_t>(&x));
		REQUIRE(f(intVal) == reinterpret_cast<void*>(static_cast<uintptr_t>(intVal)));
	}
	SECTION("ptr_roundtrip_i64") {
		auto ptrToInt = engine.registerFunction(staticCastExpression<void*, int64_t>);
		auto intToPtr = engine.registerFunction(staticCastExpression<int64_t, void*>);
		int32_t x = 42;
		void* original = &x;
		auto asInt = ptrToInt(original);
		auto backToPtr = intToPtr(asInt);
		REQUIRE(backToPtr == original);
	}
}

// Regression for the AsmJit narrowing-cast lowering. A narrowing integer cast
// must drop the stale high bits *before* the value reaches a full-width
// consumer. The x64 backend previously only sign/zero-extended from the source
// width and skipped the narrow-to-destination step, so a value like
// (int8_t)200 stayed 200 in its 64-bit register instead of -56. Widening casts
// and re-narrowing casts read sub-registers and therefore masked the bug; a
// signed comparison reads the full register and exposes it. The ARM backend
// already performed the narrowing, so this also pins the two backends to the
// same observable behavior.
template <typename In, typename Narrow>
val<int32_t> narrowCastThenSignTest(val<In> x) {
	val<Narrow> n = static_cast<val<Narrow>>(x);
	if (n > val<Narrow>(0)) {
		return val<int32_t>(1);
	}
	return val<int32_t>(0);
}

template <typename In, typename Narrow>
void checkNarrowCastSign(engine::NautilusEngine& engine, std::string name, std::initializer_list<In> inputs) {
	DYNAMIC_SECTION(name) {
		auto f = engine.registerFunction(narrowCastThenSignTest<In, Narrow>);
		for (In in : inputs) {
			const int32_t reference = (static_cast<Narrow>(in) > static_cast<Narrow>(0)) ? 1 : 0;
			REQUIRE(f(in) == reference);
		}
	}
}

void narrowCastSignTest(engine::NautilusEngine& engine) {
	// Inputs chosen so the narrowed value differs in sign from the source.
	checkNarrowCastSign<int32_t, int8_t>(engine, "i32_to_i8", {0, 100, 200, 384, -200});
	checkNarrowCastSign<int32_t, int16_t>(engine, "i32_to_i16", {0, 1000, 40000, 100000, -40000});
	checkNarrowCastSign<int64_t, int8_t>(engine, "i64_to_i8", {0, 127, 128, 255, 256});
}

// Regression for the AsmJit x64 float→uint64 lowering (issue #328). The
// backend emitted the signed cvttss2si/cvttsd2si for every integer
// destination, so inputs in [2^63, 2^64) overflowed to the "integer
// indefinite" value 2^63 instead of converting. All inputs stay below 2^64:
// converting larger values to uint64_t is undefined behavior, so the native
// reference would be undefined too.
template <typename In>
void checkFloatToUi64(engine::NautilusEngine& engine, std::string name, std::initializer_list<In> inputs) {
	DYNAMIC_SECTION(name) {
		auto f = engine.registerFunction(staticCastExpression<In, uint64_t>);
		for (In in : inputs) {
			REQUIRE(f(in) == static_cast<uint64_t>(in));
		}
	}
}

void floatToUnsignedCastTest(engine::NautilusEngine& engine) {
	// Values straddling the 2^63 boundary where the signed conversion overflows.
	checkFloatToUi64<float>(engine, "f32_to_ui64",
	                        {0.0f, 1.5f, 4294967296.0f /*2^32*/, 4611686018427387904.0f /*2^62*/,
	                         9223372036854775808.0f /*2^63*/, 16500484545456129080.0f /*issue #328 fuzz value*/,
	                         18446742974197923840.0f /*largest f32 < 2^64*/});
	checkFloatToUi64<double>(engine, "f64_to_ui64",
	                         {0.0, 1.5, 4294967296.0, 4611686018427387904.0, 9223372036854775808.0,
	                          16500484545456129080.0, 18446744073709549568.0 /*largest f64 < 2^64*/});
}

TEST_CASE("Cast Interpreter Test") {
	auto engine = nautilus::testing::makeEngine("interpreter");
	castTest(engine);
	ptrCastTest(engine);
	narrowCastSignTest(engine);
	floatToUnsignedCastTest(engine);
}

#ifdef ENABLE_TRACING
TEST_CASE("Cast Compiler Test") {
	nautilus::testing::forEachBackendWithTraceMode([](engine::NautilusEngine& engine) { castTest(engine); });
}

TEST_CASE("Pointer Cast Compiler Test") {
	nautilus::testing::forEachBackendWithTraceMode([](engine::NautilusEngine& engine) { ptrCastTest(engine); });
}

TEST_CASE("Narrowing Cast Sign Compiler Test") {
	nautilus::testing::forEachBackendWithTraceMode([](engine::NautilusEngine& engine) { narrowCastSignTest(engine); });
}

TEST_CASE("Float To Unsigned Cast Compiler Test") {
	nautilus::testing::forEachBackendWithTraceMode(
	    [](engine::NautilusEngine& engine) { floatToUnsignedCastTest(engine); });
}
#endif
} // namespace nautilus::engine

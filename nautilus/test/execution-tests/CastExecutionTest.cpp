#include "CastFunctions.hpp"
#include "nautilus/Engine.hpp"
#include "nautilus/val_concepts.hpp"
#include <catch2/catch_all.hpp>

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
			REQUIRE(f(min) == static_cast<uint8_t>(min));
			REQUIRE(f(max) == static_cast<uint8_t>(max));
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

void addTest(engine::NautilusEngine& engine) {
	createCastTest<int8_t>(engine, "cast_from_i8", INT8_MIN, INT8_MAX);
	createCastTest<int16_t>(engine, "cast_from_i16", INT16_MIN, INT16_MAX);
	createCastTest<int32_t>(engine, "cast_from_i32", INT32_MIN, INT32_MAX);
	createCastTest<int64_t>(engine, "cast_from_i64", INT64_MIN, INT64_MAX);
	createCastTest<uint8_t>(engine, "cast_from_ui8", 0, UINT8_MAX);
	createCastTest<uint16_t>(engine, "cast_from_ui16", 0, UINT16_MAX);
	createCastTest<uint32_t>(engine, "cast_from_ui32", 0, UINT32_MAX);
	createCastTest<uint64_t>(engine, "cast_from_ui64", 0, UINT64_MAX);
	createCastTest<bool>(engine, "cast_from_bool", true, false);
	createCastTest<float>(engine, "cast_from_float", std::numeric_limits<float>::min(), std::numeric_limits<float>::max());
	createCastTest<double>(engine, "cast_from_double", std::numeric_limits<double>::min(), std::numeric_limits<double>::max());
}

TEST_CASE("Cast Interpreter Test") {
	engine::Options options;
	options.setOption("engine.Compilation", false);
	auto engine = engine::NautilusEngine(options);
	addTest(engine);
}

#ifdef ENABLE_TRACING
TEST_CASE("Cast Compiler Test") {
	std::vector<std::string> backends = {};
#ifdef ENABLE_MLIR_BACKEND
	backends.emplace_back("mlir");
#endif
#ifdef ENABLE_C_BACKEND
	backends.emplace_back("cpp");
#endif
#ifdef ENABLE_BC_BACKEND
	backends.emplace_back("bc");
#endif
#ifdef ENABLE_ASMJIT_BACKEND
	backends.emplace_back("asmjit");
#endif
	for (auto& backend : backends) {
		DYNAMIC_SECTION(backend) {
			engine::Options options;
			options.setOption("engine.backend", backend);
			auto engine = engine::NautilusEngine(options);
			addTest(engine);
		}
	}
}
#endif
} // namespace nautilus::engine

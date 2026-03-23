#include "VectorFunctions.hpp"
#include "nautilus/Engine.hpp"
#include <catch2/catch_all.hpp>
#include <cmath>
#include <cstring>

namespace nautilus::engine {

// Number of lanes for each type on the current hardware
static constexpr size_t FLOAT_LANES = Vector<float>::lanes;
static constexpr size_t INT_LANES = Vector<int32_t>::lanes;
static constexpr size_t DOUBLE_LANES = Vector<double>::lanes;

void vectorTests(engine::NautilusEngine& engine) {

	SECTION("vector_add float") {
		auto f = engine.registerFunction(vectorAddFloat);
		alignas(64) float a[FLOAT_LANES];
		alignas(64) float b[FLOAT_LANES];
		alignas(64) float c[FLOAT_LANES] = {};
		for (size_t i = 0; i < FLOAT_LANES; i++) {
			a[i] = static_cast<float>(i + 1);
			b[i] = static_cast<float>(i + 5);
		}
		f(a, b, c);
		for (size_t i = 0; i < FLOAT_LANES; i++) {
			REQUIRE(c[i] == a[i] + b[i]);
		}
	}

	SECTION("vector_sub float") {
		auto f = engine.registerFunction(vectorSubFloat);
		alignas(64) float a[FLOAT_LANES];
		alignas(64) float b[FLOAT_LANES];
		alignas(64) float c[FLOAT_LANES] = {};
		for (size_t i = 0; i < FLOAT_LANES; i++) {
			a[i] = static_cast<float>((i + 1) * 10);
			b[i] = static_cast<float>(i + 1);
		}
		f(a, b, c);
		for (size_t i = 0; i < FLOAT_LANES; i++) {
			REQUIRE(c[i] == a[i] - b[i]);
		}
	}

	SECTION("vector_mul float") {
		auto f = engine.registerFunction(vectorMulFloat);
		alignas(64) float a[FLOAT_LANES];
		alignas(64) float b[FLOAT_LANES];
		alignas(64) float c[FLOAT_LANES] = {};
		for (size_t i = 0; i < FLOAT_LANES; i++) {
			a[i] = static_cast<float>(i + 2);
			b[i] = 10.0f;
		}
		f(a, b, c);
		for (size_t i = 0; i < FLOAT_LANES; i++) {
			REQUIRE(c[i] == a[i] * b[i]);
		}
	}

	SECTION("vector_div float") {
		auto f = engine.registerFunction(vectorDivFloat);
		alignas(64) float a[FLOAT_LANES];
		alignas(64) float b[FLOAT_LANES];
		alignas(64) float c[FLOAT_LANES] = {};
		for (size_t i = 0; i < FLOAT_LANES; i++) {
			a[i] = static_cast<float>((i + 1) * 10);
			b[i] = static_cast<float>(i + 1);
		}
		f(a, b, c);
		for (size_t i = 0; i < FLOAT_LANES; i++) {
			REQUIRE(c[i] == 10.0f);
		}
	}

	SECTION("vector_neg float") {
		auto f = engine.registerFunction(vectorNegFloat);
		alignas(64) float a[FLOAT_LANES];
		alignas(64) float c[FLOAT_LANES] = {};
		for (size_t i = 0; i < FLOAT_LANES; i++) {
			a[i] = static_cast<float>(i + 1) * ((i % 2 == 0) ? 1.0f : -1.0f);
		}
		f(a, c);
		for (size_t i = 0; i < FLOAT_LANES; i++) {
			REQUIRE(c[i] == -a[i]);
		}
	}

	SECTION("vector_min float") {
		auto f = engine.registerFunction(vectorMinFloat);
		alignas(64) float a[FLOAT_LANES];
		alignas(64) float b[FLOAT_LANES];
		alignas(64) float c[FLOAT_LANES] = {};
		for (size_t i = 0; i < FLOAT_LANES; i++) {
			a[i] = static_cast<float>(i);
			b[i] = static_cast<float>(FLOAT_LANES - 1 - i);
		}
		f(a, b, c);
		for (size_t i = 0; i < FLOAT_LANES; i++) {
			REQUIRE(c[i] == std::min(a[i], b[i]));
		}
	}

	SECTION("vector_max float") {
		auto f = engine.registerFunction(vectorMaxFloat);
		alignas(64) float a[FLOAT_LANES];
		alignas(64) float b[FLOAT_LANES];
		alignas(64) float c[FLOAT_LANES] = {};
		for (size_t i = 0; i < FLOAT_LANES; i++) {
			a[i] = static_cast<float>(i);
			b[i] = static_cast<float>(FLOAT_LANES - 1 - i);
		}
		f(a, b, c);
		for (size_t i = 0; i < FLOAT_LANES; i++) {
			REQUIRE(c[i] == std::max(a[i], b[i]));
		}
	}

	SECTION("vector_reduce_add float") {
		auto f = engine.registerFunction(vectorReduceAddFloat);
		alignas(64) float a[FLOAT_LANES];
		float expected = 0.0f;
		for (size_t i = 0; i < FLOAT_LANES; i++) {
			a[i] = static_cast<float>(i + 1);
			expected += a[i];
		}
		auto result = f(a);
		REQUIRE(result == expected);
	}

	SECTION("vector_fma float") {
		auto f = engine.registerFunction(vectorFmaFloat);
		alignas(64) float a[FLOAT_LANES];
		alignas(64) float b[FLOAT_LANES];
		alignas(64) float c[FLOAT_LANES];
		alignas(64) float d[FLOAT_LANES] = {};
		for (size_t i = 0; i < FLOAT_LANES; i++) {
			a[i] = static_cast<float>(i + 1);
			b[i] = static_cast<float>(i + 5);
			c[i] = 10.0f;
		}
		f(a, b, c, d);
		for (size_t i = 0; i < FLOAT_LANES; i++) {
			REQUIRE(d[i] == std::fma(a[i], b[i], c[i]));
		}
	}

	SECTION("vector_add int32") {
		auto f = engine.registerFunction(vectorAddInt);
		alignas(64) int32_t a[INT_LANES];
		alignas(64) int32_t b[INT_LANES];
		alignas(64) int32_t c[INT_LANES] = {};
		for (size_t i = 0; i < INT_LANES; i++) {
			a[i] = static_cast<int32_t>(i + 1);
			b[i] = static_cast<int32_t>((i + 1) * 10);
		}
		f(a, b, c);
		for (size_t i = 0; i < INT_LANES; i++) {
			REQUIRE(c[i] == a[i] + b[i]);
		}
	}

	SECTION("vector_mul int32") {
		auto f = engine.registerFunction(vectorMulInt);
		alignas(64) int32_t a[INT_LANES];
		alignas(64) int32_t b[INT_LANES];
		alignas(64) int32_t c[INT_LANES] = {};
		for (size_t i = 0; i < INT_LANES; i++) {
			a[i] = static_cast<int32_t>(i + 2);
			b[i] = 10;
		}
		f(a, b, c);
		for (size_t i = 0; i < INT_LANES; i++) {
			REQUIRE(c[i] == a[i] * b[i]);
		}
	}

	SECTION("vector_reduce_add int32") {
		auto f = engine.registerFunction(vectorReduceAddInt);
		alignas(64) int32_t a[INT_LANES];
		int32_t expected = 0;
		for (size_t i = 0; i < INT_LANES; i++) {
			a[i] = static_cast<int32_t>(i + 1);
			expected += a[i];
		}
		auto result = f(a);
		REQUIRE(result == expected);
	}

	// ================================================================
	// Operator overload tests
	// ================================================================

	SECTION("operator+ float") {
		auto f = engine.registerFunction(vectorOperatorAddFloat);
		alignas(64) float a[FLOAT_LANES];
		alignas(64) float b[FLOAT_LANES];
		alignas(64) float c[FLOAT_LANES] = {};
		for (size_t i = 0; i < FLOAT_LANES; i++) {
			a[i] = static_cast<float>(i + 1);
			b[i] = static_cast<float>(i + 5);
		}
		f(a, b, c);
		for (size_t i = 0; i < FLOAT_LANES; i++) {
			REQUIRE(c[i] == a[i] + b[i]);
		}
	}

	SECTION("operator* and operator- float (a*b - c)") {
		auto f = engine.registerFunction(vectorOperatorMulSubFloat);
		alignas(64) float a[FLOAT_LANES];
		alignas(64) float b[FLOAT_LANES];
		alignas(64) float c[FLOAT_LANES];
		alignas(64) float d[FLOAT_LANES] = {};
		for (size_t i = 0; i < FLOAT_LANES; i++) {
			a[i] = static_cast<float>(i + 2);
			b[i] = 3.0f;
			c[i] = 1.0f;
		}
		f(a, b, c, d);
		for (size_t i = 0; i < FLOAT_LANES; i++) {
			REQUIRE(d[i] == a[i] * b[i] - c[i]);
		}
	}

	SECTION("unary operator- float") {
		auto f = engine.registerFunction(vectorOperatorNegFloat);
		alignas(64) float a[FLOAT_LANES];
		alignas(64) float c[FLOAT_LANES] = {};
		for (size_t i = 0; i < FLOAT_LANES; i++) {
			a[i] = static_cast<float>(i + 1);
		}
		f(a, c);
		for (size_t i = 0; i < FLOAT_LANES; i++) {
			REQUIRE(c[i] == -a[i]);
		}
	}

	// ================================================================
	// Reduce min/max tests
	// ================================================================

	SECTION("vector_reduce_min float") {
		auto f = engine.registerFunction(vectorReduceMinFloat);
		alignas(64) float a[FLOAT_LANES];
		for (size_t i = 0; i < FLOAT_LANES; i++) {
			a[i] = static_cast<float>(FLOAT_LANES - i);
		}
		a[FLOAT_LANES / 2] = -42.0f; // ensure min is in the middle
		auto result = f(a);
		REQUIRE(result == -42.0f);
	}

	SECTION("vector_reduce_max float") {
		auto f = engine.registerFunction(vectorReduceMaxFloat);
		alignas(64) float a[FLOAT_LANES];
		for (size_t i = 0; i < FLOAT_LANES; i++) {
			a[i] = static_cast<float>(i);
		}
		a[FLOAT_LANES / 2] = 999.0f;
		auto result = f(a);
		REQUIRE(result == 999.0f);
	}

	SECTION("vector_reduce_min int32") {
		auto f = engine.registerFunction(vectorReduceMinInt);
		alignas(64) int32_t a[INT_LANES];
		for (size_t i = 0; i < INT_LANES; i++) {
			a[i] = static_cast<int32_t>(100 - i);
		}
		a[0] = -5;
		auto result = f(a);
		REQUIRE(result == -5);
	}

	SECTION("vector_reduce_max int32") {
		auto f = engine.registerFunction(vectorReduceMaxInt);
		alignas(64) int32_t a[INT_LANES];
		for (size_t i = 0; i < INT_LANES; i++) {
			a[i] = static_cast<int32_t>(i);
		}
		a[INT_LANES - 1] = 500;
		auto result = f(a);
		REQUIRE(result == 500);
	}

	// ================================================================
	// Comparison tests
	// ================================================================

	SECTION("vector_lt float") {
		auto f = engine.registerFunction(vectorLtFloat);
		alignas(64) float a[FLOAT_LANES];
		alignas(64) float b[FLOAT_LANES];
		alignas(64) float c[FLOAT_LANES] = {};
		for (size_t i = 0; i < FLOAT_LANES; i++) {
			a[i] = static_cast<float>(i);
			b[i] = static_cast<float>(FLOAT_LANES / 2);
		}
		f(a, b, c);
		for (size_t i = 0; i < FLOAT_LANES; i++) {
			uint32_t ci;
			std::memcpy(&ci, &c[i], sizeof(float));
			if (a[i] < b[i]) {
				REQUIRE(ci == 0xFFFFFFFF);
			} else {
				REQUIRE(ci == 0x00000000);
			}
		}
	}

	// ================================================================
	// Blend test
	// ================================================================

	SECTION("vector_blend float") {
		auto f = engine.registerFunction(vectorBlendFloat);
		alignas(64) float a[FLOAT_LANES];
		alignas(64) float b[FLOAT_LANES];
		alignas(64) float mask[FLOAT_LANES];
		alignas(64) float c[FLOAT_LANES] = {};
		for (size_t i = 0; i < FLOAT_LANES; i++) {
			a[i] = 10.0f;
			b[i] = 20.0f;
			// Alternate mask: odd lanes select a, even lanes select b
			uint32_t m = (i % 2 == 1) ? 0xFFFFFFFF : 0x00000000;
			std::memcpy(&mask[i], &m, sizeof(float));
		}
		f(a, b, mask, c);
		for (size_t i = 0; i < FLOAT_LANES; i++) {
			if (i % 2 == 1) {
				REQUIRE(c[i] == 10.0f);
			} else {
				REQUIRE(c[i] == 20.0f);
			}
		}
	}

	// ================================================================
	// Bitwise test
	// ================================================================

	SECTION("vector_and int32") {
		auto f = engine.registerFunction(vectorAndInt);
		alignas(64) int32_t a[INT_LANES];
		alignas(64) int32_t b[INT_LANES];
		alignas(64) int32_t c[INT_LANES] = {};
		for (size_t i = 0; i < INT_LANES; i++) {
			a[i] = 0xFF00FF00;
			b[i] = static_cast<int32_t>(0xFFFF0000);
		}
		f(a, b, c);
		for (size_t i = 0; i < INT_LANES; i++) {
			REQUIRE(c[i] == (int32_t) (0xFF00FF00 & 0xFFFF0000));
		}
	}

	// ================================================================
	// Double tests
	// ================================================================

	SECTION("vector_add double") {
		auto f = engine.registerFunction(vectorAddDouble);
		alignas(64) double a[DOUBLE_LANES];
		alignas(64) double b[DOUBLE_LANES];
		alignas(64) double c[DOUBLE_LANES] = {};
		for (size_t i = 0; i < DOUBLE_LANES; i++) {
			a[i] = static_cast<double>(i) + 1.5;
			b[i] = static_cast<double>(i) + 3.5;
		}
		f(a, b, c);
		for (size_t i = 0; i < DOUBLE_LANES; i++) {
			REQUIRE(c[i] == a[i] + b[i]);
		}
	}
}

TEST_CASE("Vector Interpreter Test") {
	engine::Options options;
	options.setOption("engine.Compilation", false);
	auto engine = engine::NautilusEngine(options);
	vectorTests(engine);
}

#ifdef ENABLE_TRACING
TEST_CASE("Vector Compiler Test") {
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
	for (auto& backend : backends) {
		DYNAMIC_SECTION(backend) {
			engine::Options options;
			options.setOption("engine.backend", backend);
			auto engine = engine::NautilusEngine(options);
			vectorTests(engine);
		}
	}
}
#endif

} // namespace nautilus::engine

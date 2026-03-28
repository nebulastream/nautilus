#include "VectorFunctions.hpp"
#include "nautilus/Engine.hpp"
#include <algorithm>
#include <catch2/catch_all.hpp>
#include <cmath>
#include <cstring>

namespace nautilus::engine {

static constexpr size_t FL = val<vec<float>>::lanes;
static constexpr size_t IL = val<vec<int32_t>>::lanes;
static constexpr size_t DL = val<vec<double>>::lanes;

void vectorTests(engine::NautilusEngine& engine) {

	// ================================================================
	// Float arithmetic
	// ================================================================

	SECTION("add float") {
		auto f = engine.registerFunction(vectorAddFloat);
		alignas(64) float a[FL], b[FL], c[FL] = {};
		for (size_t i = 0; i < FL; i++) {
			a[i] = static_cast<float>(i + 1);
			b[i] = static_cast<float>(i + 5);
		}
		f(a, b, c);
		for (size_t i = 0; i < FL; i++)
			REQUIRE(c[i] == a[i] + b[i]);
	}

	SECTION("sub float") {
		auto f = engine.registerFunction(vectorSubFloat);
		alignas(64) float a[FL], b[FL], c[FL] = {};
		for (size_t i = 0; i < FL; i++) {
			a[i] = static_cast<float>((i + 1) * 10);
			b[i] = static_cast<float>(i + 1);
		}
		f(a, b, c);
		for (size_t i = 0; i < FL; i++)
			REQUIRE(c[i] == a[i] - b[i]);
	}

	SECTION("mul float") {
		auto f = engine.registerFunction(vectorMulFloat);
		alignas(64) float a[FL], b[FL], c[FL] = {};
		for (size_t i = 0; i < FL; i++) {
			a[i] = static_cast<float>(i + 2);
			b[i] = 10.0f;
		}
		f(a, b, c);
		for (size_t i = 0; i < FL; i++)
			REQUIRE(c[i] == a[i] * b[i]);
	}

	SECTION("div float") {
		auto f = engine.registerFunction(vectorDivFloat);
		alignas(64) float a[FL], b[FL], c[FL] = {};
		for (size_t i = 0; i < FL; i++) {
			a[i] = static_cast<float>((i + 1) * 10);
			b[i] = static_cast<float>(i + 1);
		}
		f(a, b, c);
		for (size_t i = 0; i < FL; i++)
			REQUIRE(c[i] == 10.0f);
	}

	SECTION("neg float") {
		auto f = engine.registerFunction(vectorNegFloat);
		alignas(64) float a[FL], c[FL] = {};
		for (size_t i = 0; i < FL; i++)
			a[i] = static_cast<float>(i + 1) * ((i % 2 == 0) ? 1.0f : -1.0f);
		f(a, c);
		for (size_t i = 0; i < FL; i++)
			REQUIRE(c[i] == -a[i]);
	}

	SECTION("abs float") {
		auto f = engine.registerFunction(vectorAbsFloat);
		alignas(64) float a[FL], c[FL] = {};
		for (size_t i = 0; i < FL; i++)
			a[i] = static_cast<float>(i + 1) * ((i % 2 == 0) ? 1.0f : -1.0f);
		f(a, c);
		for (size_t i = 0; i < FL; i++)
			REQUIRE(c[i] == std::abs(a[i]));
	}

	SECTION("min float") {
		auto f = engine.registerFunction(vectorMinFloat);
		alignas(64) float a[FL], b[FL], c[FL] = {};
		for (size_t i = 0; i < FL; i++) {
			a[i] = static_cast<float>(i);
			b[i] = static_cast<float>(FL - 1 - i);
		}
		f(a, b, c);
		for (size_t i = 0; i < FL; i++)
			REQUIRE(c[i] == std::min(a[i], b[i]));
	}

	SECTION("max float") {
		auto f = engine.registerFunction(vectorMaxFloat);
		alignas(64) float a[FL], b[FL], c[FL] = {};
		for (size_t i = 0; i < FL; i++) {
			a[i] = static_cast<float>(i);
			b[i] = static_cast<float>(FL - 1 - i);
		}
		f(a, b, c);
		for (size_t i = 0; i < FL; i++)
			REQUIRE(c[i] == std::max(a[i], b[i]));
	}

	SECTION("fma float") {
		auto f = engine.registerFunction(vectorFmaFloat);
		alignas(64) float a[FL], b[FL], c[FL], d[FL] = {};
		for (size_t i = 0; i < FL; i++) {
			a[i] = static_cast<float>(i + 1);
			b[i] = static_cast<float>(i + 5);
			c[i] = 10.0f;
		}
		f(a, b, c, d);
		for (size_t i = 0; i < FL; i++)
			REQUIRE(d[i] == std::fma(a[i], b[i], c[i]));
	}

	SECTION("chained a*b - c float") {
		auto f = engine.registerFunction(vectorMulSubFloat);
		alignas(64) float a[FL], b[FL], c[FL], d[FL] = {};
		for (size_t i = 0; i < FL; i++) {
			a[i] = static_cast<float>(i + 2);
			b[i] = 3.0f;
			c[i] = 1.0f;
		}
		f(a, b, c, d);
		for (size_t i = 0; i < FL; i++)
			REQUIRE(d[i] == a[i] * b[i] - c[i]);
	}

	// ================================================================
	// Compound assignment
	// ================================================================

	SECTION("compound += float") {
		auto f = engine.registerFunction(vectorCompoundAddFloat);
		alignas(64) float a[FL], b[FL], c[FL] = {};
		for (size_t i = 0; i < FL; i++) {
			a[i] = static_cast<float>(i + 1);
			b[i] = static_cast<float>(i + 5);
		}
		f(a, b, c);
		for (size_t i = 0; i < FL; i++)
			REQUIRE(c[i] == a[i] + b[i]);
	}

	SECTION("compound *= float") {
		auto f = engine.registerFunction(vectorCompoundMulFloat);
		alignas(64) float a[FL], b[FL], c[FL] = {};
		for (size_t i = 0; i < FL; i++) {
			a[i] = static_cast<float>(i + 2);
			b[i] = 10.0f;
		}
		f(a, b, c);
		for (size_t i = 0; i < FL; i++)
			REQUIRE(c[i] == a[i] * b[i]);
	}

	// ================================================================
	// Float reductions
	// ================================================================

	SECTION("reduce_add float") {
		auto f = engine.registerFunction(vectorReduceAddFloat);
		alignas(64) float a[FL];
		float expected = 0.0f;
		for (size_t i = 0; i < FL; i++) {
			a[i] = static_cast<float>(i + 1);
			expected += a[i];
		}
		REQUIRE(f(a) == expected);
	}

	SECTION("reduce_min float") {
		auto f = engine.registerFunction(vectorReduceMinFloat);
		alignas(64) float a[FL];
		for (size_t i = 0; i < FL; i++)
			a[i] = static_cast<float>(FL - i);
		a[FL / 2] = -42.0f;
		REQUIRE(f(a) == -42.0f);
	}

	SECTION("reduce_max float") {
		auto f = engine.registerFunction(vectorReduceMaxFloat);
		alignas(64) float a[FL];
		for (size_t i = 0; i < FL; i++)
			a[i] = static_cast<float>(i);
		a[FL / 2] = 999.0f;
		REQUIRE(f(a) == 999.0f);
	}

	// ================================================================
	// Comparisons
	// ================================================================

	SECTION("lt float (operator<)") {
		auto f = engine.registerFunction(vectorLtFloat);
		alignas(64) float a[FL], b[FL], c[FL] = {};
		for (size_t i = 0; i < FL; i++) {
			a[i] = static_cast<float>(i);
			b[i] = static_cast<float>(FL / 2);
		}
		f(a, b, c);
		for (size_t i = 0; i < FL; i++) {
			uint32_t ci;
			std::memcpy(&ci, &c[i], sizeof(float));
			REQUIRE(ci == ((a[i] < b[i]) ? 0xFFFFFFFF : 0x00000000));
		}
	}

	SECTION("ge float (operator>=)") {
		auto f = engine.registerFunction(vectorGeFloat);
		alignas(64) float a[FL], b[FL], c[FL] = {};
		for (size_t i = 0; i < FL; i++) {
			a[i] = static_cast<float>(i);
			b[i] = static_cast<float>(FL / 2);
		}
		f(a, b, c);
		for (size_t i = 0; i < FL; i++) {
			uint32_t ci;
			std::memcpy(&ci, &c[i], sizeof(float));
			REQUIRE(ci == ((a[i] >= b[i]) ? 0xFFFFFFFF : 0x00000000));
		}
	}

	SECTION("eq float (operator==)") {
		auto f = engine.registerFunction(vectorEqFloat);
		alignas(64) float a[FL], b[FL], c[FL] = {};
		for (size_t i = 0; i < FL; i++) {
			a[i] = static_cast<float>(i);
			b[i] = static_cast<float>(i % 2 == 0 ? i : i + 1);
		}
		f(a, b, c);
		for (size_t i = 0; i < FL; i++) {
			uint32_t ci;
			std::memcpy(&ci, &c[i], sizeof(float));
			REQUIRE(ci == ((a[i] == b[i]) ? 0xFFFFFFFF : 0x00000000));
		}
	}

	SECTION("ne float (operator!=)") {
		auto f = engine.registerFunction(vectorNeFloat);
		alignas(64) float a[FL], b[FL], c[FL] = {};
		for (size_t i = 0; i < FL; i++) {
			a[i] = static_cast<float>(i);
			b[i] = static_cast<float>(i % 2 == 0 ? i : i + 1);
		}
		f(a, b, c);
		for (size_t i = 0; i < FL; i++) {
			uint32_t ci;
			std::memcpy(&ci, &c[i], sizeof(float));
			REQUIRE(ci == ((a[i] != b[i]) ? 0xFFFFFFFF : 0x00000000));
		}
	}

	SECTION("le float (operator<=)") {
		auto f = engine.registerFunction(vectorLeFloat);
		alignas(64) float a[FL], b[FL], c[FL] = {};
		for (size_t i = 0; i < FL; i++) {
			a[i] = static_cast<float>(i);
			b[i] = static_cast<float>(FL / 2);
		}
		f(a, b, c);
		for (size_t i = 0; i < FL; i++) {
			uint32_t ci;
			std::memcpy(&ci, &c[i], sizeof(float));
			REQUIRE(ci == ((a[i] <= b[i]) ? 0xFFFFFFFF : 0x00000000));
		}
	}

	SECTION("gt float (operator>)") {
		auto f = engine.registerFunction(vectorGtFloat);
		alignas(64) float a[FL], b[FL], c[FL] = {};
		for (size_t i = 0; i < FL; i++) {
			a[i] = static_cast<float>(i);
			b[i] = static_cast<float>(FL / 2);
		}
		f(a, b, c);
		for (size_t i = 0; i < FL; i++) {
			uint32_t ci;
			std::memcpy(&ci, &c[i], sizeof(float));
			REQUIRE(ci == ((a[i] > b[i]) ? 0xFFFFFFFF : 0x00000000));
		}
	}

	SECTION("eq int32 (operator==)") {
		auto f = engine.registerFunction(vectorEqInt);
		alignas(64) int32_t a[IL], b[IL], c[IL] = {};
		for (size_t i = 0; i < IL; i++) {
			a[i] = static_cast<int32_t>(i);
			b[i] = static_cast<int32_t>(i % 2 == 0 ? i : i + 1);
		}
		f(a, b, c);
		for (size_t i = 0; i < IL; i++) {
			uint32_t ci;
			std::memcpy(&ci, &c[i], sizeof(int32_t));
			REQUIRE(ci == ((a[i] == b[i]) ? 0xFFFFFFFF : 0x00000000));
		}
	}

	SECTION("ne int32 (operator!=)") {
		auto f = engine.registerFunction(vectorNeInt);
		alignas(64) int32_t a[IL], b[IL], c[IL] = {};
		for (size_t i = 0; i < IL; i++) {
			a[i] = static_cast<int32_t>(i);
			b[i] = static_cast<int32_t>(i % 2 == 0 ? i : i + 1);
		}
		f(a, b, c);
		for (size_t i = 0; i < IL; i++) {
			uint32_t ci;
			std::memcpy(&ci, &c[i], sizeof(int32_t));
			REQUIRE(ci == ((a[i] != b[i]) ? 0xFFFFFFFF : 0x00000000));
		}
	}

	SECTION("lt int32 (operator<)") {
		auto f = engine.registerFunction(vectorLtInt);
		alignas(64) int32_t a[IL], b[IL], c[IL] = {};
		for (size_t i = 0; i < IL; i++) {
			a[i] = static_cast<int32_t>(i);
			b[i] = static_cast<int32_t>(IL / 2);
		}
		f(a, b, c);
		for (size_t i = 0; i < IL; i++) {
			uint32_t ci;
			std::memcpy(&ci, &c[i], sizeof(int32_t));
			REQUIRE(ci == ((a[i] < b[i]) ? 0xFFFFFFFF : 0x00000000));
		}
	}

	SECTION("gt int32 (operator>)") {
		auto f = engine.registerFunction(vectorGtInt);
		alignas(64) int32_t a[IL], b[IL], c[IL] = {};
		for (size_t i = 0; i < IL; i++) {
			a[i] = static_cast<int32_t>(i);
			b[i] = static_cast<int32_t>(IL / 2);
		}
		f(a, b, c);
		for (size_t i = 0; i < IL; i++) {
			uint32_t ci;
			std::memcpy(&ci, &c[i], sizeof(int32_t));
			REQUIRE(ci == ((a[i] > b[i]) ? 0xFFFFFFFF : 0x00000000));
		}
	}

	// ================================================================
	// Blend — float and int
	// ================================================================

	SECTION("blend float") {
		auto f = engine.registerFunction(vectorBlendFloat);
		alignas(64) float a[FL], b[FL], mask[FL], c[FL] = {};
		for (size_t i = 0; i < FL; i++) {
			a[i] = 10.0f;
			b[i] = 20.0f;
			uint32_t m = (i % 2 == 1) ? 0xFFFFFFFF : 0x00000000;
			std::memcpy(&mask[i], &m, sizeof(float));
		}
		f(a, b, mask, c);
		for (size_t i = 0; i < FL; i++)
			REQUIRE(c[i] == ((i % 2 == 1) ? 10.0f : 20.0f));
	}

	SECTION("blend int32") {
		auto f = engine.registerFunction(vectorBlendInt);
		alignas(64) int32_t a[IL], b[IL], mask[IL], c[IL] = {};
		for (size_t i = 0; i < IL; i++) {
			a[i] = 100;
			b[i] = 200;
			mask[i] = (i % 2 == 0) ? (int32_t) 0xFFFFFFFF : 0;
		}
		f(a, b, mask, c);
		for (size_t i = 0; i < IL; i++)
			REQUIRE(c[i] == ((i % 2 == 0) ? 100 : 200));
	}

	// ================================================================
	// Bitwise — int and float
	// ================================================================

	SECTION("and int32 (operator&)") {
		auto f = engine.registerFunction(vectorAndInt);
		alignas(64) int32_t a[IL], b[IL], c[IL] = {};
		for (size_t i = 0; i < IL; i++) {
			a[i] = (int32_t) 0xFF00FF00;
			b[i] = (int32_t) 0xFFFF0000;
		}
		f(a, b, c);
		for (size_t i = 0; i < IL; i++)
			REQUIRE(c[i] == (int32_t) (0xFF00FF00 & 0xFFFF0000));
	}

	SECTION("or int32 (operator|)") {
		auto f = engine.registerFunction(vectorOrInt);
		alignas(64) int32_t a[IL], b[IL], c[IL] = {};
		for (size_t i = 0; i < IL; i++) {
			a[i] = (int32_t) 0x0F0F0000;
			b[i] = (int32_t) 0x00F0F0F0;
		}
		f(a, b, c);
		for (size_t i = 0; i < IL; i++)
			REQUIRE(c[i] == (int32_t) (0x0F0F0000 | 0x00F0F0F0));
	}

	SECTION("xor int32 (operator^)") {
		auto f = engine.registerFunction(vectorXorInt);
		alignas(64) int32_t a[IL], b[IL], c[IL] = {};
		for (size_t i = 0; i < IL; i++) {
			a[i] = (int32_t) 0xFF00FF00;
			b[i] = (int32_t) 0xFFFF0000;
		}
		f(a, b, c);
		for (size_t i = 0; i < IL; i++)
			REQUIRE(c[i] == (int32_t) (0xFF00FF00 ^ 0xFFFF0000));
	}

	SECTION("and float (operator&)") {
		auto f = engine.registerFunction(vectorAndFloat);
		alignas(64) float a[FL], b[FL], c[FL] = {};
		for (size_t i = 0; i < FL; i++) {
			uint32_t ai = 0xFF00FF00, bi = 0xFFFF0000;
			std::memcpy(&a[i], &ai, sizeof(float));
			std::memcpy(&b[i], &bi, sizeof(float));
		}
		f(a, b, c);
		for (size_t i = 0; i < FL; i++) {
			uint32_t ci;
			std::memcpy(&ci, &c[i], sizeof(float));
			REQUIRE(ci == (0xFF00FF00u & 0xFFFF0000u));
		}
	}

	SECTION("or float (operator|)") {
		auto f = engine.registerFunction(vectorOrFloat);
		alignas(64) float a[FL], b[FL], c[FL] = {};
		for (size_t i = 0; i < FL; i++) {
			uint32_t ai = 0x0F0F0000, bi = 0x00F0F0F0;
			std::memcpy(&a[i], &ai, sizeof(float));
			std::memcpy(&b[i], &bi, sizeof(float));
		}
		f(a, b, c);
		for (size_t i = 0; i < FL; i++) {
			uint32_t ci;
			std::memcpy(&ci, &c[i], sizeof(float));
			REQUIRE(ci == (0x0F0F0000u | 0x00F0F0F0u));
		}
	}

	// ================================================================
	// Compare → Blend chain (realistic SIMD pattern)
	// ================================================================

	SECTION("compare-then-blend float (vector max via cmp+blend)") {
		auto f = engine.registerFunction(vectorCmpBlendFloat);
		alignas(64) float a[FL], b[FL], c[FL] = {};
		for (size_t i = 0; i < FL; i++) {
			a[i] = static_cast<float>(i);
			b[i] = static_cast<float>(FL - 1 - i);
		}
		f(a, b, c);
		for (size_t i = 0; i < FL; i++)
			REQUIRE(c[i] == std::max(a[i], b[i]));
	}

	// ================================================================
	// Integer arithmetic + reductions
	// ================================================================

	SECTION("add int32") {
		auto f = engine.registerFunction(vectorAddInt);
		alignas(64) int32_t a[IL], b[IL], c[IL] = {};
		for (size_t i = 0; i < IL; i++) {
			a[i] = static_cast<int32_t>(i + 1);
			b[i] = static_cast<int32_t>((i + 1) * 10);
		}
		f(a, b, c);
		for (size_t i = 0; i < IL; i++)
			REQUIRE(c[i] == a[i] + b[i]);
	}

	SECTION("mul int32") {
		auto f = engine.registerFunction(vectorMulInt);
		alignas(64) int32_t a[IL], b[IL], c[IL] = {};
		for (size_t i = 0; i < IL; i++) {
			a[i] = static_cast<int32_t>(i + 2);
			b[i] = 10;
		}
		f(a, b, c);
		for (size_t i = 0; i < IL; i++)
			REQUIRE(c[i] == a[i] * b[i]);
	}

	SECTION("reduce_add int32") {
		auto f = engine.registerFunction(vectorReduceAddInt);
		alignas(64) int32_t a[IL];
		int32_t expected = 0;
		for (size_t i = 0; i < IL; i++) {
			a[i] = static_cast<int32_t>(i + 1);
			expected += a[i];
		}
		REQUIRE(f(a) == expected);
	}

	SECTION("reduce_min int32") {
		auto f = engine.registerFunction(vectorReduceMinInt);
		alignas(64) int32_t a[IL];
		for (size_t i = 0; i < IL; i++)
			a[i] = static_cast<int32_t>(100 - i);
		a[0] = -5;
		REQUIRE(f(a) == -5);
	}

	SECTION("reduce_max int32") {
		auto f = engine.registerFunction(vectorReduceMaxInt);
		alignas(64) int32_t a[IL];
		for (size_t i = 0; i < IL; i++)
			a[i] = static_cast<int32_t>(i);
		a[IL - 1] = 500;
		REQUIRE(f(a) == 500);
	}

	// ================================================================
	// Double
	// ================================================================

	SECTION("add double") {
		auto f = engine.registerFunction(vectorAddDouble);
		alignas(64) double a[DL], b[DL], c[DL] = {};
		for (size_t i = 0; i < DL; i++) {
			a[i] = static_cast<double>(i) + 1.5;
			b[i] = static_cast<double>(i) + 3.5;
		}
		f(a, b, c);
		for (size_t i = 0; i < DL; i++)
			REQUIRE(c[i] == a[i] + b[i]);
	}

	// ================================================================
	// Vector<T> alias Load / Store
	// ================================================================

	SECTION("Vector<T> Load/Store round-trip") {
		auto f = engine.registerFunction(vectorAliasLoadStore);
		alignas(64) float a[FL], c[FL] = {};
		for (size_t i = 0; i < FL; i++)
			a[i] = static_cast<float>(i * 3 + 7);
		f(a, c);
		for (size_t i = 0; i < FL; i++)
			REQUIRE(c[i] == a[i]);
	}

	// ================================================================
	// vector_load<T> convenience function
	// ================================================================

	SECTION("vector_load convenience") {
		auto f = engine.registerFunction(vectorConvenienceLoad);
		alignas(64) float a[FL], b[FL], c[FL] = {};
		for (size_t i = 0; i < FL; i++) {
			a[i] = static_cast<float>(i + 1);
			b[i] = static_cast<float>(i + 5);
		}
		f(a, b, c);
		for (size_t i = 0; i < FL; i++)
			REQUIRE(c[i] == a[i] + b[i]);
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

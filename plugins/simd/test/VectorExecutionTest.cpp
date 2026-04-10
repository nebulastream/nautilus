#include "ExecutionTest.hpp"
#include "VectorFunctions.hpp"
#include "nautilus/Engine.hpp"
#include "nautilus/simd/plugin.hpp"
#include <algorithm>
#include <catch2/catch_all.hpp>
#include <cmath>
#include <cstring>

namespace nautilus::engine {

// Lane counts are now determined at runtime via CPU feature detection.
// Use max_lanes for stack array sizing (safe upper bound).
static const size_t FL = vec<float>::Lanes();
static const size_t IL = vec<int32_t>::Lanes();
static const size_t DL = vec<double>::Lanes();
static constexpr size_t FL_MAX = vec<float>::max_lanes;
static constexpr size_t IL_MAX = vec<int32_t>::max_lanes;
static constexpr size_t DL_MAX = vec<double>::max_lanes;

void vectorTests(engine::NautilusEngine& engine) {

	// ================================================================
	// Float arithmetic
	// ================================================================

	SECTION("add float") {
		auto f = engine.registerFunction(vectorAddFloat);
		alignas(64) float a[FL_MAX], b[FL_MAX], c[FL_MAX] = {};
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
		alignas(64) float a[FL_MAX], b[FL_MAX], c[FL_MAX] = {};
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
		alignas(64) float a[FL_MAX], b[FL_MAX], c[FL_MAX] = {};
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
		alignas(64) float a[FL_MAX], b[FL_MAX], c[FL_MAX] = {};
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
		alignas(64) float a[FL_MAX], c[FL_MAX] = {};
		for (size_t i = 0; i < FL; i++)
			a[i] = static_cast<float>(i + 1) * ((i % 2 == 0) ? 1.0f : -1.0f);
		f(a, c);
		for (size_t i = 0; i < FL; i++)
			REQUIRE(c[i] == -a[i]);
	}

	SECTION("abs float") {
		auto f = engine.registerFunction(vectorAbsFloat);
		alignas(64) float a[FL_MAX], c[FL_MAX] = {};
		for (size_t i = 0; i < FL; i++)
			a[i] = static_cast<float>(i + 1) * ((i % 2 == 0) ? 1.0f : -1.0f);
		f(a, c);
		for (size_t i = 0; i < FL; i++)
			REQUIRE(c[i] == std::abs(a[i]));
	}

	SECTION("min float") {
		auto f = engine.registerFunction(vectorMinFloat);
		alignas(64) float a[FL_MAX], b[FL_MAX], c[FL_MAX] = {};
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
		alignas(64) float a[FL_MAX], b[FL_MAX], c[FL_MAX] = {};
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
		alignas(64) float a[FL_MAX], b[FL_MAX], c[FL_MAX], d[FL_MAX] = {};
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
		alignas(64) float a[FL_MAX], b[FL_MAX], c[FL_MAX], d[FL_MAX] = {};
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
		alignas(64) float a[FL_MAX], b[FL_MAX], c[FL_MAX] = {};
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
		alignas(64) float a[FL_MAX], b[FL_MAX], c[FL_MAX] = {};
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
		alignas(64) float a[FL_MAX];
		float expected = 0.0f;
		for (size_t i = 0; i < FL; i++) {
			a[i] = static_cast<float>(i + 1);
			expected += a[i];
		}
		REQUIRE(f(a) == expected);
	}

	SECTION("reduce_min float") {
		auto f = engine.registerFunction(vectorReduceMinFloat);
		alignas(64) float a[FL_MAX];
		for (size_t i = 0; i < FL; i++)
			a[i] = static_cast<float>(FL - i);
		a[FL / 2] = -42.0f;
		REQUIRE(f(a) == -42.0f);
	}

	SECTION("reduce_max float") {
		auto f = engine.registerFunction(vectorReduceMaxFloat);
		alignas(64) float a[FL_MAX];
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
		alignas(64) float a[FL_MAX], b[FL_MAX], c[FL_MAX] = {};
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
		alignas(64) float a[FL_MAX], b[FL_MAX], c[FL_MAX] = {};
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
		alignas(64) float a[FL_MAX], b[FL_MAX], c[FL_MAX] = {};
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
		alignas(64) float a[FL_MAX], b[FL_MAX], c[FL_MAX] = {};
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
		alignas(64) float a[FL_MAX], b[FL_MAX], c[FL_MAX] = {};
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
		alignas(64) float a[FL_MAX], b[FL_MAX], c[FL_MAX] = {};
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
		alignas(64) int32_t a[IL_MAX], b[IL_MAX], c[IL_MAX] = {};
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
		alignas(64) int32_t a[IL_MAX], b[IL_MAX], c[IL_MAX] = {};
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
		alignas(64) int32_t a[IL_MAX], b[IL_MAX], c[IL_MAX] = {};
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
		alignas(64) int32_t a[IL_MAX], b[IL_MAX], c[IL_MAX] = {};
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
		alignas(64) float a[FL_MAX], b[FL_MAX], mask[FL_MAX], c[FL_MAX] = {};
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
		alignas(64) int32_t a[IL_MAX], b[IL_MAX], mask[IL_MAX], c[IL_MAX] = {};
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
		alignas(64) int32_t a[IL_MAX], b[IL_MAX], c[IL_MAX] = {};
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
		alignas(64) int32_t a[IL_MAX], b[IL_MAX], c[IL_MAX] = {};
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
		alignas(64) int32_t a[IL_MAX], b[IL_MAX], c[IL_MAX] = {};
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
		alignas(64) float a[FL_MAX], b[FL_MAX], c[FL_MAX] = {};
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
		alignas(64) float a[FL_MAX], b[FL_MAX], c[FL_MAX] = {};
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
		alignas(64) float a[FL_MAX], b[FL_MAX], c[FL_MAX] = {};
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
		alignas(64) int32_t a[IL_MAX], b[IL_MAX], c[IL_MAX] = {};
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
		alignas(64) int32_t a[IL_MAX], b[IL_MAX], c[IL_MAX] = {};
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
		alignas(64) int32_t a[IL_MAX];
		int32_t expected = 0;
		for (size_t i = 0; i < IL; i++) {
			a[i] = static_cast<int32_t>(i + 1);
			expected += a[i];
		}
		REQUIRE(f(a) == expected);
	}

	SECTION("reduce_min int32") {
		auto f = engine.registerFunction(vectorReduceMinInt);
		alignas(64) int32_t a[IL_MAX];
		for (size_t i = 0; i < IL; i++)
			a[i] = static_cast<int32_t>(100 - i);
		a[0] = -5;
		REQUIRE(f(a) == -5);
	}

	SECTION("reduce_max int32") {
		auto f = engine.registerFunction(vectorReduceMaxInt);
		alignas(64) int32_t a[IL_MAX];
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
		alignas(64) double a[DL_MAX], b[DL_MAX], c[DL_MAX] = {};
		for (size_t i = 0; i < DL; i++) {
			a[i] = static_cast<double>(i) + 1.5;
			b[i] = static_cast<double>(i) + 3.5;
		}
		f(a, b, c);
		for (size_t i = 0; i < DL; i++)
			REQUIRE(c[i] == a[i] + b[i]);
	}

	// ================================================================
	// Realistic composite use cases
	// ================================================================

	SECTION("dot product float") {
		auto f = engine.registerFunction(vectorDotProductFloat);
		alignas(64) float a[FL_MAX], b[FL_MAX];
		float expected = 0.0f;
		for (size_t i = 0; i < FL; i++) {
			a[i] = static_cast<float>(i + 1);
			b[i] = static_cast<float>(i + 2);
			expected += a[i] * b[i];
		}
		REQUIRE(f(a, b) == expected);
	}

	SECTION("dot product int32") {
		auto f = engine.registerFunction(vectorDotProductInt);
		alignas(64) int32_t a[IL_MAX], b[IL_MAX];
		int32_t expected = 0;
		for (size_t i = 0; i < IL; i++) {
			a[i] = static_cast<int32_t>(i + 1);
			b[i] = static_cast<int32_t>(i + 2);
			expected += a[i] * b[i];
		}
		REQUIRE(f(a, b) == expected);
	}

	SECTION("squared norm float") {
		auto f = engine.registerFunction(vectorSquaredNormFloat);
		alignas(64) float a[FL_MAX];
		float expected = 0.0f;
		for (size_t i = 0; i < FL; i++) {
			a[i] = static_cast<float>(i + 1);
			expected += a[i] * a[i];
		}
		REQUIRE(f(a) == expected);
	}

	SECTION("saxpy float (FMA-based)") {
		auto f = engine.registerFunction(vectorSaxpyFloat);
		alignas(64) float x[FL_MAX], y[FL_MAX], alpha_vec[FL_MAX];
		float alpha = 2.5f;
		for (size_t i = 0; i < FL; i++) {
			x[i] = static_cast<float>(i + 1);
			y[i] = static_cast<float>(i * 10);
			alpha_vec[i] = alpha;
		}
		float expected[FL_MAX];
		for (size_t i = 0; i < FL; i++)
			expected[i] = std::fma(alpha, x[i], y[i]);
		f(x, y, alpha_vec);
		for (size_t i = 0; i < FL; i++)
			REQUIRE(y[i] == expected[i]);
	}

	SECTION("clamp float") {
		auto f = engine.registerFunction(vectorClampFloat);
		alignas(64) float a[FL_MAX], lo[FL_MAX], hi[FL_MAX], c[FL_MAX] = {};
		for (size_t i = 0; i < FL; i++) {
			a[i] = static_cast<float>(i) * 3.0f - 5.0f;
			lo[i] = 0.0f;
			hi[i] = 10.0f;
		}
		f(a, lo, hi, c);
		for (size_t i = 0; i < FL; i++)
			REQUIRE(c[i] == std::max(0.0f, std::min(a[i], 10.0f)));
	}

	SECTION("distance squared float") {
		auto f = engine.registerFunction(vectorDistanceSquaredFloat);
		alignas(64) float a[FL_MAX], b[FL_MAX];
		float expected = 0.0f;
		for (size_t i = 0; i < FL; i++) {
			a[i] = static_cast<float>(i + 1);
			b[i] = static_cast<float>((i + 1) * 2);
			float d = a[i] - b[i];
			expected += d * d;
		}
		REQUIRE(f(a, b) == expected);
	}

	SECTION("polynomial evaluation (Horner)") {
		auto f = engine.registerFunction(vectorPolyEvalFloat);
		alignas(64) float x[FL_MAX], c0[FL_MAX], c1[FL_MAX], c2[FL_MAX], c3[FL_MAX], out[FL_MAX] = {};
		for (size_t i = 0; i < FL; i++) {
			x[i] = static_cast<float>(i) * 0.5f;
			c0[i] = 1.0f;
			c1[i] = 2.0f;
			c2[i] = 3.0f;
			c3[i] = 4.0f;
		}
		f(x, c0, c1, c2, c3, out);
		for (size_t i = 0; i < FL; i++) {
			float xi = x[i];
			float expected = std::fma(std::fma(std::fma(4.0f, xi, 3.0f), xi, 2.0f), xi, 1.0f);
			REQUIRE(out[i] == expected);
		}
	}

	SECTION("conditional negate float") {
		auto f = engine.registerFunction(vectorConditionalNegateFloat);
		alignas(64) float a[FL_MAX], mask[FL_MAX], c[FL_MAX] = {};
		for (size_t i = 0; i < FL; i++) {
			a[i] = static_cast<float>(i + 1);
			uint32_t m = (i % 2 == 0) ? 0xFFFFFFFF : 0x00000000;
			std::memcpy(&mask[i], &m, sizeof(float));
		}
		f(a, mask, c);
		for (size_t i = 0; i < FL; i++) {
			float expected = (i % 2 == 0) ? -a[i] : a[i];
			REQUIRE(c[i] == expected);
		}
	}

	SECTION("sum of absolute differences float") {
		auto f = engine.registerFunction(vectorSadFloat);
		alignas(64) float a[FL_MAX], b[FL_MAX];
		float expected = 0.0f;
		for (size_t i = 0; i < FL; i++) {
			a[i] = static_cast<float>(i + 1);
			b[i] = static_cast<float>((i + 1) * 2);
			expected += std::abs(a[i] - b[i]);
		}
		REQUIRE(f(a, b) == expected);
	}

	SECTION("weighted sum parts") {
		auto f = engine.registerFunction(vectorWeightedSumParts);
		alignas(64) float a[FL_MAX], w[FL_MAX];
		float numer = 0.0f, denom = 0.0f;
		float numer_out = 0.0f, denom_out = 0.0f;
		for (size_t i = 0; i < FL; i++) {
			a[i] = static_cast<float>(i + 1);
			w[i] = static_cast<float>(FL - i);
			numer += a[i] * w[i];
			denom += w[i];
		}
		f(a, w, &numer_out, &denom_out);
		REQUIRE(numer_out == numer);
		REQUIRE(denom_out == denom);
	}

	SECTION("lerp float") {
		auto f = engine.registerFunction(vectorLerpFloat);
		alignas(64) float a[FL_MAX], b[FL_MAX], t[FL_MAX], c[FL_MAX] = {};
		for (size_t i = 0; i < FL; i++) {
			a[i] = 0.0f;
			b[i] = static_cast<float>((i + 1) * 10);
			t[i] = 0.25f;
		}
		f(a, b, t, c);
		for (size_t i = 0; i < FL; i++) {
			float expected = std::fma(0.25f, b[i] - a[i], a[i]);
			REQUIRE(c[i] == expected);
		}
	}

	SECTION("range float (max - min)") {
		auto f = engine.registerFunction(vectorRangeFloat);
		alignas(64) float a[FL_MAX];
		for (size_t i = 0; i < FL; i++)
			a[i] = static_cast<float>(i * 3);
		float expected = a[FL - 1] - a[0];
		REQUIRE(f(a) == expected);
	}

	SECTION("relu float") {
		auto f = engine.registerFunction(vectorReluFloat);
		alignas(64) float a[FL_MAX], zeros[FL_MAX], c[FL_MAX] = {};
		for (size_t i = 0; i < FL; i++) {
			a[i] = static_cast<float>(i) - static_cast<float>(FL) / 2.0f;
			zeros[i] = 0.0f;
		}
		f(a, zeros, c);
		for (size_t i = 0; i < FL; i++)
			REQUIRE(c[i] == std::max(0.0f, a[i]));
	}

	SECTION("multi-accumulate a*b + c*d") {
		auto f = engine.registerFunction(vectorMulAccFloat);
		alignas(64) float a[FL_MAX], b[FL_MAX], c[FL_MAX], d[FL_MAX], out[FL_MAX] = {};
		for (size_t i = 0; i < FL; i++) {
			a[i] = static_cast<float>(i + 1);
			b[i] = 2.0f;
			c[i] = static_cast<float>(i + 1);
			d[i] = 3.0f;
		}
		f(a, b, c, d, out);
		for (size_t i = 0; i < FL; i++)
			REQUIRE(out[i] == a[i] * b[i] + c[i] * d[i]);
	}

	SECTION("saturate float [0, max]") {
		auto f = engine.registerFunction(vectorSaturateFloat);
		alignas(64) float a[FL_MAX], max_arr[FL_MAX], c[FL_MAX] = {};
		for (size_t i = 0; i < FL; i++) {
			a[i] = static_cast<float>(i) * 5.0f - 10.0f;
			max_arr[i] = 8.0f;
		}
		f(a, max_arr, c);
		for (size_t i = 0; i < FL; i++) {
			float expected = std::min(std::max(0.0f, a[i]), 8.0f);
			REQUIRE(c[i] == expected);
		}
	}

	// ================================================================
	// Broadcast — scalar to all lanes
	// ================================================================

	SECTION("broadcast float") {
		auto f = engine.registerFunction(vectorBroadcastFloat);
		alignas(64) float out[FL_MAX] = {};
		float scalar = 42.5f;
		f(scalar, out);
		for (size_t i = 0; i < FL; i++)
			REQUIRE(out[i] == 42.5f);
	}

	SECTION("broadcast int32") {
		auto f = engine.registerFunction(vectorBroadcastInt);
		alignas(64) int32_t out[IL_MAX] = {};
		int32_t scalar = 7;
		f(scalar, out);
		for (size_t i = 0; i < IL; i++)
			REQUIRE(out[i] == 7);
	}

	SECTION("broadcast double") {
		auto f = engine.registerFunction(vectorBroadcastDouble);
		alignas(64) double out[DL_MAX] = {};
		double scalar = 3.14;
		f(scalar, out);
		for (size_t i = 0; i < DL; i++)
			REQUIRE(out[i] == 3.14);
	}

	SECTION("broadcast + multiply float") {
		auto f = engine.registerFunction(vectorBroadcastMulFloat);
		alignas(64) float a[FL_MAX], out[FL_MAX] = {};
		float scalar = 3.0f;
		for (size_t i = 0; i < FL; i++)
			a[i] = static_cast<float>(i + 1);
		f(a, scalar, out);
		for (size_t i = 0; i < FL; i++)
			REQUIRE(out[i] == a[i] * scalar);
	}

	// ================================================================
	// Gather — indexed load
	// ================================================================

	SECTION("gather float") {
		auto f = engine.registerFunction(vectorGatherFloat);
		float base[] = {10.0f, 20.0f,  30.0f,  40.0f,  50.0f,  60.0f,  70.0f,  80.0f,
		                90.0f, 100.0f, 110.0f, 120.0f, 130.0f, 140.0f, 150.0f, 160.0f};
		alignas(64) int32_t indices[FL_MAX];
		alignas(64) float out[FL_MAX] = {};
		for (size_t i = 0; i < FL; i++)
			indices[i] = static_cast<int32_t>(FL - 1 - i); // reverse order
		f(base, indices, out);
		for (size_t i = 0; i < FL; i++)
			REQUIRE(out[i] == base[FL - 1 - i]);
	}

	SECTION("gather int32") {
		auto f = engine.registerFunction(vectorGatherInt);
		int32_t base[] = {100, 200, 300, 400, 500, 600, 700, 800, 900, 1000, 1100, 1200, 1300, 1400, 1500, 1600};
		alignas(64) int32_t indices[IL_MAX];
		alignas(64) int32_t out[IL_MAX] = {};
		for (size_t i = 0; i < IL; i++)
			indices[i] = static_cast<int32_t>(i * 2 % IL); // stride-2 access
		f(base, indices, out);
		for (size_t i = 0; i < IL; i++)
			REQUIRE(out[i] == base[i * 2 % IL]);
	}

	SECTION("gather + reduce float") {
		auto f = engine.registerFunction(vectorGatherReduceFloat);
		float base[] = {10.0f, 20.0f,  30.0f,  40.0f,  50.0f,  60.0f,  70.0f,  80.0f,
		                90.0f, 100.0f, 110.0f, 120.0f, 130.0f, 140.0f, 150.0f, 160.0f};
		alignas(64) int32_t indices[FL_MAX];
		float expected = 0.0f;
		for (size_t i = 0; i < FL; i++) {
			indices[i] = static_cast<int32_t>(i);
			expected += base[i];
		}
		REQUIRE(f(base, indices) == expected);
	}

	// ================================================================
	// Scatter — indexed store
	// ================================================================

	SECTION("scatter float") {
		auto f = engine.registerFunction(vectorScatterFloat);
		alignas(64) float src[FL_MAX];
		float base[16] = {};
		alignas(64) int32_t indices[FL_MAX];
		for (size_t i = 0; i < FL; i++) {
			src[i] = static_cast<float>((i + 1) * 10);
			indices[i] = static_cast<int32_t>(FL - 1 - i); // reverse order
		}
		f(src, base, indices);
		for (size_t i = 0; i < FL; i++)
			REQUIRE(base[FL - 1 - i] == src[i]);
	}

	SECTION("scatter int32") {
		auto f = engine.registerFunction(vectorScatterInt);
		alignas(64) int32_t src[IL_MAX];
		int32_t base[16] = {};
		alignas(64) int32_t indices[IL_MAX];
		for (size_t i = 0; i < IL; i++) {
			src[i] = static_cast<int32_t>((i + 1) * 100);
			indices[i] = static_cast<int32_t>(IL - 1 - i);
		}
		f(src, base, indices);
		for (size_t i = 0; i < IL; i++)
			REQUIRE(base[IL - 1 - i] == src[i]);
	}

	// ================================================================
	// Extract — get single lane
	// ================================================================

	SECTION("extract float") {
		auto f = engine.registerFunction(vectorExtractFloat);
		alignas(64) float a[FL_MAX];
		for (size_t i = 0; i < FL; i++)
			a[i] = static_cast<float>((i + 1) * 11);
		for (size_t i = 0; i < FL; i++)
			REQUIRE(f(a, static_cast<int32_t>(i)) == a[i]);
	}

	SECTION("extract int32") {
		auto f = engine.registerFunction(vectorExtractInt);
		alignas(64) int32_t a[IL_MAX];
		for (size_t i = 0; i < IL; i++)
			a[i] = static_cast<int32_t>((i + 1) * 7);
		for (size_t i = 0; i < IL; i++)
			REQUIRE(f(a, static_cast<int32_t>(i)) == a[i]);
	}

	// ================================================================
	// Insert — replace single lane
	// ================================================================

	SECTION("insert float") {
		auto f = engine.registerFunction(vectorInsertFloat);
		alignas(64) float a[FL_MAX], out[FL_MAX] = {};
		for (size_t i = 0; i < FL; i++)
			a[i] = static_cast<float>(i + 1);
		int32_t idx = 1;
		float value = 999.0f;
		f(a, value, idx, out);
		for (size_t i = 0; i < FL; i++) {
			if (i == 1)
				REQUIRE(out[i] == 999.0f);
			else
				REQUIRE(out[i] == a[i]);
		}
	}

	SECTION("insert int32") {
		auto f = engine.registerFunction(vectorInsertInt);
		alignas(64) int32_t a[IL_MAX], out[IL_MAX] = {};
		for (size_t i = 0; i < IL; i++)
			a[i] = static_cast<int32_t>(i + 1);
		int32_t idx = 0;
		int32_t value = -42;
		f(a, value, idx, out);
		for (size_t i = 0; i < IL; i++) {
			if (i == 0)
				REQUIRE(out[i] == -42);
			else
				REQUIRE(out[i] == a[i]);
		}
	}

	// ================================================================
	// Shifts — integer left/right shift
	// ================================================================

	SECTION("shl int32") {
		auto f = engine.registerFunction(vectorShlInt);
		alignas(64) int32_t a[IL_MAX], b[IL_MAX], c[IL_MAX] = {};
		for (size_t i = 0; i < IL; i++) {
			a[i] = static_cast<int32_t>(i + 1);
			b[i] = 2; // shift left by 2 = multiply by 4
		}
		f(a, b, c);
		for (size_t i = 0; i < IL; i++)
			REQUIRE(c[i] == (a[i] << 2));
	}

	SECTION("shr int32") {
		auto f = engine.registerFunction(vectorShrInt);
		alignas(64) int32_t a[IL_MAX], b[IL_MAX], c[IL_MAX] = {};
		for (size_t i = 0; i < IL; i++) {
			a[i] = static_cast<int32_t>((i + 1) * 16);
			b[i] = 3; // shift right by 3 = divide by 8
		}
		f(a, b, c);
		for (size_t i = 0; i < IL; i++)
			REQUIRE(c[i] == (a[i] >> 3));
	}

	SECTION("shl int64") {
		auto f = engine.registerFunction(vectorShlInt64);
		const size_t I64L = vec<int64_t>::Lanes();
		static constexpr size_t I64L_MAX = vec<int64_t>::max_lanes;
		alignas(64) int64_t a[I64L_MAX], b[I64L_MAX], c[I64L_MAX] = {};
		for (size_t i = 0; i < I64L; i++) {
			a[i] = static_cast<int64_t>(i + 1);
			b[i] = 4;
		}
		f(a, b, c);
		for (size_t i = 0; i < I64L; i++)
			REQUIRE(c[i] == (a[i] << 4));
	}

	SECTION("compound shift int32 (shl then shr)") {
		auto f = engine.registerFunction(vectorCompoundShiftInt);
		alignas(64) int32_t a[IL_MAX], out[IL_MAX] = {};
		for (size_t i = 0; i < IL; i++)
			a[i] = static_cast<int32_t>(i + 1);
		f(a, out);
		for (size_t i = 0; i < IL; i++)
			REQUIRE(out[i] == ((a[i] << 4) >> 4));
	}

	// ================================================================
	// Vector<T> alias Load / Store
	// ================================================================

	SECTION("Vector<T> Load/Store round-trip") {
		auto f = engine.registerFunction(vectorAliasLoadStore);
		alignas(64) float a[FL_MAX], c[FL_MAX] = {};
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
		alignas(64) float a[FL_MAX], b[FL_MAX], c[FL_MAX] = {};
		for (size_t i = 0; i < FL; i++) {
			a[i] = static_cast<float>(i + 1);
			b[i] = static_cast<float>(i + 5);
		}
		f(a, b, c);
		for (size_t i = 0; i < FL; i++)
			REQUIRE(c[i] == a[i] + b[i]);
	}
}

// ============================================================================
// Unaligned-buffer tests
//
// Regression tests for the MLIR vector intrinsic alignment bug: the load/store
// intrinsics must use element-level alignment (not natural vector alignment)
// so the generated code works with buffers that are not aligned to the full
// SIMD register width. std::vector::data() and malloc'd memory typically only
// guarantee alignof(T), not 64-byte alignment required by AVX-512.
//
// These tests deliberately offset the data pointer by one element so the
// buffers are guaranteed to be misaligned w.r.t. any vector width > sizeof(T).
// ============================================================================

void unalignedVectorTests(engine::NautilusEngine& engine) {

	// Float: load from unaligned pointer, add, store to unaligned pointer.
	SECTION("unaligned add float") {
		auto f = engine.registerFunction(vectorAddFloat);
		// Allocate extra element so we can offset by 1 to force misalignment.
		alignas(64) float a_aligned[FL_MAX + 1], b_aligned[FL_MAX + 1], c_aligned[FL_MAX + 1];
		float* a = a_aligned + 1; // misaligned by sizeof(float) = 4 bytes
		float* b = b_aligned + 1;
		float* c = c_aligned + 1;
		std::memset(c, 0, FL * sizeof(float));
		for (size_t i = 0; i < FL; i++) {
			a[i] = static_cast<float>(i + 1);
			b[i] = static_cast<float>(i + 5);
		}
		f(a, b, c);
		for (size_t i = 0; i < FL; i++) {
			REQUIRE(c[i] == a[i] + b[i]);
		}
	}

	// Int32: load from unaligned pointer, add, store to unaligned pointer.
	SECTION("unaligned add int") {
		auto f = engine.registerFunction(vectorAddInt);
		alignas(64) int32_t a_aligned[IL_MAX + 1], b_aligned[IL_MAX + 1], c_aligned[IL_MAX + 1];
		int32_t* a = a_aligned + 1;
		int32_t* b = b_aligned + 1;
		int32_t* c = c_aligned + 1;
		std::memset(c, 0, IL * sizeof(int32_t));
		for (size_t i = 0; i < IL; i++) {
			a[i] = static_cast<int32_t>(i + 1);
			b[i] = static_cast<int32_t>(i + 10);
		}
		f(a, b, c);
		for (size_t i = 0; i < IL; i++) {
			REQUIRE(c[i] == a[i] + b[i]);
		}
	}

	// Float: mul + sub chain with unaligned pointers.
	SECTION("unaligned mul-sub float") {
		auto f = engine.registerFunction(vectorMulSubFloat);
		alignas(64) float a_al[FL_MAX + 1], b_al[FL_MAX + 1], c_al[FL_MAX + 1], d_al[FL_MAX + 1];
		float* a = a_al + 1;
		float* b = b_al + 1;
		float* c = c_al + 1;
		float* d = d_al + 1;
		std::memset(d, 0, FL * sizeof(float));
		for (size_t i = 0; i < FL; i++) {
			a[i] = static_cast<float>(i + 1);
			b[i] = 2.0f;
			c[i] = static_cast<float>(i);
		}
		f(a, b, c, d);
		for (size_t i = 0; i < FL; i++) {
			REQUIRE(d[i] == a[i] * b[i] - c[i]);
		}
	}

	// Float: reduce-add from unaligned pointer.
	SECTION("unaligned reduce-add float") {
		auto f = engine.registerFunction(vectorReduceAddFloat);
		alignas(64) float a_aligned[FL_MAX + 1];
		float* a = a_aligned + 1;
		float expected = 0.0f;
		for (size_t i = 0; i < FL; i++) {
			a[i] = static_cast<float>(i + 1);
			expected += a[i];
		}
		REQUIRE(f(a) == expected);
	}

	// Double: add with unaligned pointers.
	SECTION("unaligned add double") {
		auto f = engine.registerFunction(vectorAddDouble);
		alignas(64) double a_aligned[DL_MAX + 1], b_aligned[DL_MAX + 1], c_aligned[DL_MAX + 1];
		double* a = a_aligned + 1;
		double* b = b_aligned + 1;
		double* c = c_aligned + 1;
		std::memset(c, 0, DL * sizeof(double));
		for (size_t i = 0; i < DL; i++) {
			a[i] = static_cast<double>(i + 1);
			b[i] = static_cast<double>(i + 100);
		}
		f(a, b, c);
		for (size_t i = 0; i < DL; i++) {
			REQUIRE(c[i] == a[i] + b[i]);
		}
	}

	// Int32: dot product from unaligned pointers (mul + reduce-add chain).
	SECTION("unaligned dot-product int") {
		auto f = engine.registerFunction(vectorDotProductInt);
		alignas(64) int32_t a_aligned[IL_MAX + 1], b_aligned[IL_MAX + 1];
		int32_t* a = a_aligned + 1;
		int32_t* b = b_aligned + 1;
		int32_t expected = 0;
		for (size_t i = 0; i < IL; i++) {
			a[i] = static_cast<int32_t>(i + 1);
			b[i] = 2;
			expected += a[i] * b[i];
		}
		REQUIRE(f(a, b) == expected);
	}
}

TEST_CASE("Vector Interpreter Test") {
	auto engine = nautilus::testing::makeEngine("interpreter");
	vectorTests(engine);
}

#ifdef ENABLE_TRACING
TEST_CASE("Vector Compiler Test") {
	nautilus::testing::forEachBackend([](engine::NautilusEngine& engine) { vectorTests(engine); },
	                                  /*include_interpreter=*/false);
}

// Regression test for the MLIR vector intrinsic alignment bug.  Only runs on
// the MLIR backend with intrinsics enabled, because without the intrinsic
// plugin, vector ops fall back to scalar invoke() calls where alignment is
// irrelevant. Plugin registration is handled by nautilus/simd/plugin.hpp.
#ifdef ENABLE_MLIR_BACKEND
TEST_CASE("Vector Unaligned Buffer Test") {
	auto engine = nautilus::testing::makeEngine(
	    "mlir", [](engine::Options& options) { options.setOption("mlir.enableIntrinsics", true); });
	unalignedVectorTests(engine);
}
#endif // ENABLE_MLIR_BACKEND
#endif // ENABLE_TRACING

} // namespace nautilus::engine

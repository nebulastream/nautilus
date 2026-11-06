// `val<T> OP literal` and `val<T> OP static_val<L>` trace the right operand as one constant of the type the operator
// converts it to, converted in C++, instead of a constant of its own type and a CAST (val_arith.hpp, literal_operand).
// These tests compare, on every backend, the folded forms with the old forms (`val<L>(K)`, which traces the constant
// and the CAST) and with plain C++, for the literals where that conversion is not the identity: out of range of the
// common type, of the other signedness, wider than the value, floating point, bool. Whether the conversion is done in
// C++ or traced does not change a value, so each expression is evaluated in six forms that all must agree.

#include "ExecutionTest.hpp"
#include "nautilus/Engine.hpp"
#include "nautilus/static.hpp"
#include "nautilus/val.hpp"
#include "nautilus/val_ptr.hpp"
#include <array>
#include <catch2/catch_all.hpp>
#include <cstdint>
#include <limits>
#include <string>
#include <type_traits>
#include <typeinfo>
#include <vector>

// The reference expressions compare and combine mixed signedness on purpose.
#if defined(__clang__)
#pragma clang diagnostic ignored "-Wsign-compare"
#pragma clang diagnostic ignored "-Wtautological-compare"
#pragma clang diagnostic ignored "-Wtautological-constant-compare"
#pragma clang diagnostic ignored "-Wtautological-unsigned-zero-compare"
#pragma clang diagnostic ignored "-Wimplicit-int-float-conversion"
#elif defined(__GNUC__)
#pragma GCC diagnostic ignored "-Wsign-compare"
#pragma GCC diagnostic ignored "-Wbool-compare"
#pragma GCC diagnostic ignored "-Wtype-limits"
#endif

namespace nautilus::engine {

namespace {

/// Signed integer addition, subtraction and multiplication wrap in traced code. The reference computes them in the
/// unsigned type, since signed overflow is undefined behavior in C++.
template <typename A, typename B, typename Fn>
auto wrapping(A a, B b, Fn fn) {
	using C = decltype(a + b);
	if constexpr (std::is_integral_v<C> && std::is_signed_v<C>) {
		using U = std::make_unsigned_t<C>;
		return static_cast<C>(fn(static_cast<U>(a), static_cast<U>(b)));
	} else {
		return fn(a, b);
	}
}

// Each operation applies to val operands and, as `native`, to raw ones.
#define LITERAL_OP(NAME, EXPR, NATIVE)                                                                                 \
	struct NAME {                                                                                                      \
		static constexpr const char* name = #NAME;                                                                     \
		template <typename A, typename B>                                                                              \
		static auto apply(A&& a, B&& b) {                                                                              \
			return std::forward<A>(a) EXPR std::forward<B>(b);                                                         \
		}                                                                                                              \
		template <typename A, typename B>                                                                              \
		static auto native(A a, B b) {                                                                                 \
			return NATIVE;                                                                                             \
		}                                                                                                              \
	}

LITERAL_OP(Add, +, wrapping(a, b, [](auto p, auto q) { return p + q; }));
LITERAL_OP(Sub, -, wrapping(a, b, [](auto p, auto q) { return p - q; }));
LITERAL_OP(Mul, *, wrapping(a, b, [](auto p, auto q) { return p * q; }));
LITERAL_OP(Div, /, a / b);
LITERAL_OP(Mod, %, a % b);
LITERAL_OP(Shl, <<, a << b);
LITERAL_OP(Shr, >>, a >> b);
LITERAL_OP(And, &, a& b);
LITERAL_OP(Or, |, a | b);
LITERAL_OP(Xor, ^, a ^ b);
LITERAL_OP(Eq, ==, a == b);
LITERAL_OP(Ne, !=, a != b);
LITERAL_OP(Lt, <, a < b);
LITERAL_OP(Le, <=, a <= b);
LITERAL_OP(Gt, >, a > b);
LITERAL_OP(Ge, >=, a >= b);

template <typename... Ops>
struct OpList {
	static constexpr size_t size = sizeof...(Ops);
};

using Basic = OpList<Add, Sub, Mul, Eq, Ne, Lt, Le, Gt, Ge>;
using Bits = OpList<And, Or, Xor>;
using Divide = OpList<Div>;
using DivideAndModulo = OpList<Div, Mod>;
using Shifts = OpList<Shl, Shr>;
using ShiftLeft = OpList<Shl>;
using ShiftRight = OpList<Shr>;

// The forms of one expression: with the literal on the right, then on the left. A form is a slot of the result array.
enum Form : unsigned {
	RightLiteral = 0,
	RightVal = 1,
	LeftLiteral = 2,
	LeftVal = 3,
	RightStatic = 4,
	LeftStatic = 5,
	FormCount = 6
};
constexpr unsigned RightForms = 1u << RightLiteral | 1u << RightVal | 1u << RightStatic;
constexpr unsigned LeftForms = 1u << LeftLiteral | 1u << LeftVal | 1u << LeftStatic;

template <typename T, typename L>
using ResultElement = std::conditional_t<std::is_floating_point_v<T> || std::is_floating_point_v<L>, double, int64_t>;

template <typename Out, typename R>
void store(val<Out*>& out, val<bool*>& outBool, size_t slot, val<R> result) {
	if constexpr (std::is_same_v<R, bool>) {
		outBool[slot] = result;
	} else {
		out[slot] = static_cast<val<Out>>(result);
	}
}

template <typename Out, typename R>
void storeNative(Out* out, bool* outBool, size_t slot, R result) {
	if constexpr (std::is_same_v<R, bool>) {
		outBool[slot] = result;
	} else {
		out[slot] = static_cast<Out>(result);
	}
}

template <typename Op, typename T, typename L, L K, unsigned Forms, typename Out>
void emitOp(val<T>& x, val<Out*>& out, val<bool*>& outBool, size_t base) {
	if constexpr (Forms & (1u << RightLiteral)) {
		store(out, outBool, base + RightLiteral, Op::apply(x, K));
	}
	if constexpr (Forms & (1u << RightVal)) {
		store(out, outBool, base + RightVal, Op::apply(x, val<L>(K)));
	}
	if constexpr (Forms & (1u << LeftLiteral)) {
		store(out, outBool, base + LeftLiteral, Op::apply(K, x));
	}
	if constexpr (Forms & (1u << LeftVal)) {
		store(out, outBool, base + LeftVal, Op::apply(val<L>(K), x));
	}
	if constexpr (std::is_integral_v<L>) {
		if constexpr (Forms & (1u << RightStatic)) {
			static_val<L> s(K);
			store(out, outBool, base + RightStatic, Op::apply(x, s));
		}
		if constexpr (Forms & (1u << LeftStatic)) {
			static_val<L> s(K);
			store(out, outBool, base + LeftStatic, Op::apply(s, x));
		}
	}
}

template <typename Op, typename T, typename L, L K, unsigned Forms, typename Out>
void emitNativeOp(T x, Out* out, bool* outBool, size_t base) {
	if constexpr (Forms & (1u << RightLiteral)) {
		storeNative(out, outBool, base + RightLiteral, Op::native(x, K));
	}
	if constexpr (Forms & (1u << RightVal)) {
		storeNative(out, outBool, base + RightVal, Op::native(x, K));
	}
	if constexpr (Forms & (1u << LeftLiteral)) {
		storeNative(out, outBool, base + LeftLiteral, Op::native(K, x));
	}
	if constexpr (Forms & (1u << LeftVal)) {
		storeNative(out, outBool, base + LeftVal, Op::native(K, x));
	}
	if constexpr (std::is_integral_v<L>) {
		if constexpr (Forms & (1u << RightStatic)) {
			storeNative(out, outBool, base + RightStatic, Op::native(x, K));
		}
		if constexpr (Forms & (1u << LeftStatic)) {
			storeNative(out, outBool, base + LeftStatic, Op::native(K, x));
		}
	}
}

template <typename T, typename L, L K, typename Ops, unsigned Forms>
struct Kernel;

template <typename T, typename L, L K, typename... Ops, unsigned Forms>
struct Kernel<T, L, K, OpList<Ops...>, Forms> {
	using Out = ResultElement<T, L>;
	static constexpr size_t slots = sizeof...(Ops) * FormCount;

	static void traced(val<T> x, val<Out*> out, val<bool*> outBool) {
		size_t base = 0;
		((emitOp<Ops, T, L, K, Forms, Out>(x, out, outBool, base), base += FormCount), ...);
	}

	static void native(T x, Out* out, bool* outBool) {
		size_t base = 0;
		((emitNativeOp<Ops, T, L, K, Forms, Out>(x, out, outBool, base), base += FormCount), ...);
	}

	static std::string describe(size_t slot) {
		static const char* const formNames[FormCount] = {"x OP literal",       "x OP val<L>(literal)",
		                                                 "literal OP x",       "val<L>(literal) OP x",
		                                                 "x OP static_val<L>", "static_val<L> OP x"};
		std::string names[] = {Ops::name...};
		return names[slot / FormCount] + " " + formNames[slot % FormCount];
	}
};

enum class Inputs { Any, NonZero, SmallNonNegative };

template <typename T>
std::vector<T> inputValues(Inputs kind) {
	std::vector<T> values;
	auto add = [&](auto v) {
		if constexpr (std::is_integral_v<T>) {
			// only values that fit T
			if (v < std::numeric_limits<T>::min() || v > std::numeric_limits<T>::max()) {
				return;
			}
		}
		if (kind == Inputs::NonZero && (static_cast<T>(v) == T(0) || static_cast<T>(v) == T(-1))) {
			return;
		}
		values.push_back(static_cast<T>(v));
	};
	if (kind == Inputs::SmallNonNegative) {
		for (int v : {0, 1, 2, 3, 5, 7}) {
			add(v);
		}
		return values;
	}
	if constexpr (std::is_floating_point_v<T>) {
		for (double v : {-3.5, -1.0, 0.5, 1.0, 2.5, 16777217.0, 1.0e10, -1.0e10}) {
			add(v);
		}
		if (kind == Inputs::Any) {
			add(0.0);
			add(-0.0);
		}
	} else {
		for (long long v :
		     {(long long) std::numeric_limits<T>::min(), (long long) std::numeric_limits<T>::min() + 1, -128LL, -3LL,
		      -2LL, 2LL, 3LL, 7LL, 100LL, 127LL, 255LL, 256LL, 300LL, 65535LL, 65536LL, 2147483647LL,
		      (long long) std::numeric_limits<T>::max() - 1, (long long) std::numeric_limits<T>::max()}) {
			add(v);
		}
		if constexpr (std::is_unsigned_v<T>) {
			// long long cannot hold the top of a 64-bit unsigned type
			if constexpr (sizeof(T) == 8) {
				values.push_back(std::numeric_limits<T>::max() - 1);
				values.push_back(T(1) << 63);
			}
		}
		if (kind == Inputs::Any) {
			add(0);
			add(1);
			add(-1);
		}
	}
	return values;
}

/// Compiles the six forms of every operation of `Ops` applied to `val<T>` and the literal `K` of type `L` and compares
/// them with plain C++ for each input.
template <typename T, typename L, L K, typename Ops, unsigned Forms = RightForms | LeftForms>
void checkLiteral(engine::NautilusEngine& engine, Inputs inputs = Inputs::Any) {
	using Kern = Kernel<T, L, K, Ops, Forms>;
	using Out = typename Kern::Out;
	auto f = engine.registerFunction(&Kern::traced);
	for (T x : inputValues<T>(inputs)) {
		std::array<Out, Kern::slots> got {};
		std::array<Out, Kern::slots> want {};
		std::array<bool, Kern::slots> gotBool {};
		std::array<bool, Kern::slots> wantBool {};
		Kern::native(x, want.data(), wantBool.data());
		f(x, got.data(), gotBool.data());
		for (size_t slot = 0; slot < Kern::slots; ++slot) {
			INFO(Kern::describe(slot) << " with x = " << +x << ", literal = " << +K << " (" << typeid(L).name()
			                          << ", x is " << typeid(T).name() << ")");
			CHECK(got[slot] == want[slot]);
			CHECK(gotBool[slot] == wantBool[slot]);
		}
	}
}

constexpr uint64_t U64Max = std::numeric_limits<uint64_t>::max();

} // namespace

// A literal of another type than the value: the operator converts both to the common type. The literal is converted in
// C++ where that is exact for every value of its type.
TEST_CASE("Literal operands: integral literals of every width and signedness") {
	nautilus::testing::forEachBackend([](engine::NautilusEngine& engine) {
		SECTION("int8_t") {
			// 300 does not fit the value type, and neither does -1 fit an unsigned one
			checkLiteral<int8_t, int, 300, Basic>(engine);
			checkLiteral<int8_t, int, -1, Basic>(engine);
			checkLiteral<int8_t, uint8_t, 255, Basic>(engine);
			checkLiteral<int8_t, uint32_t, 4000000000u, Basic>(engine);
			checkLiteral<int8_t, int64_t, -5000000000LL, Basic>(engine);
			checkLiteral<int8_t, int, 0x0f0, Bits>(engine);
		}
		SECTION("uint8_t") {
			checkLiteral<uint8_t, int, -1, Basic>(engine);
			checkLiteral<uint8_t, int, 511, Basic>(engine);
			checkLiteral<uint8_t, int8_t, -128, Basic>(engine);
			checkLiteral<uint8_t, uint64_t, U64Max, Basic>(engine);
			checkLiteral<uint8_t, int, 0x1ff, Bits>(engine);
		}
		SECTION("int16_t and uint16_t") {
			checkLiteral<int16_t, uint16_t, 65535, Basic>(engine);
			checkLiteral<int16_t, int, -70000, Basic>(engine);
			checkLiteral<uint16_t, int16_t, -2, Basic>(engine);
			checkLiteral<uint16_t, uint32_t, 4294967295u, Basic>(engine);
			checkLiteral<uint16_t, int, -70000, Bits>(engine);
		}
		SECTION("int32_t") {
			// int32_t against uint32_t: the value is converted to the literal's unsigned type
			checkLiteral<int32_t, uint32_t, 4000000000u, Basic>(engine);
			checkLiteral<int32_t, uint32_t, 1u, Basic>(engine);
			checkLiteral<int32_t, int8_t, -128, Basic>(engine);
			checkLiteral<int32_t, uint8_t, 255, Basic>(engine);
			checkLiteral<int32_t, int64_t, 5000000000LL, Basic>(engine);
			checkLiteral<int32_t, uint64_t, U64Max, Basic>(engine);
			checkLiteral<int32_t, int, -1, Bits>(engine);
		}
		SECTION("uint32_t") {
			// `u < -1` converts -1 to 4294967295
			checkLiteral<uint32_t, int, -1, Basic>(engine);
			checkLiteral<uint32_t, int, -5, Basic>(engine);
			checkLiteral<uint32_t, int8_t, -128, Basic>(engine);
			checkLiteral<uint32_t, int16_t, -32768, Basic>(engine);
			checkLiteral<uint32_t, int64_t, -1, Basic>(engine);
			checkLiteral<uint32_t, int64_t, 5000000000LL, Basic>(engine);
			checkLiteral<uint32_t, int, -1, Bits>(engine);
		}
		SECTION("int64_t") {
			checkLiteral<int64_t, int, 3, Basic>(engine);
			checkLiteral<int64_t, int, -1, Basic>(engine);
			checkLiteral<int64_t, uint32_t, 4294967295u, Basic>(engine);
			checkLiteral<int64_t, uint64_t, U64Max, Basic>(engine);
			checkLiteral<int64_t, uint64_t, 9223372036854775808ull, Basic>(engine);
			checkLiteral<int64_t, int8_t, -128, Basic>(engine);
			checkLiteral<int64_t, uint64_t, 0x00ff00ff00ff00ffull, Bits>(engine);
		}
		SECTION("uint64_t") {
			checkLiteral<uint64_t, int, -1, Basic>(engine);
			checkLiteral<uint64_t, int, 7, Basic>(engine);
			checkLiteral<uint64_t, int64_t, -5000000000LL, Basic>(engine);
			checkLiteral<uint64_t, int8_t, -128, Basic>(engine);
			checkLiteral<uint64_t, uint32_t, 4294967295u, Basic>(engine);
			checkLiteral<uint64_t, int, -1, Bits>(engine);
		}
		SECTION("bool literals") {
			checkLiteral<int32_t, bool, true, Basic>(engine);
			checkLiteral<uint64_t, bool, true, Basic>(engine);
			checkLiteral<int8_t, bool, false, Basic>(engine);
			checkLiteral<int32_t, bool, true, Bits>(engine);
		}
	});
}

TEST_CASE("Literal operands: division and modulo") {
	nautilus::testing::forEachBackend([](engine::NautilusEngine& engine) {
		// the divisor is never 0, the dividend never -1 together with the smallest value
		checkLiteral<int32_t, int, 7, DivideAndModulo>(engine, Inputs::NonZero);
		checkLiteral<int32_t, int, -3, DivideAndModulo>(engine, Inputs::NonZero);
		checkLiteral<int8_t, int, 300, DivideAndModulo>(engine, Inputs::NonZero);
		checkLiteral<uint32_t, int, -3, DivideAndModulo>(engine, Inputs::NonZero);
		checkLiteral<uint32_t, int64_t, -3, DivideAndModulo>(engine, Inputs::NonZero);
		checkLiteral<uint64_t, int, -7, DivideAndModulo>(engine, Inputs::NonZero);
		checkLiteral<int64_t, uint32_t, 4000000000u, DivideAndModulo>(engine, Inputs::NonZero);
		checkLiteral<int64_t, uint64_t, 9223372036854775809ull, DivideAndModulo>(engine, Inputs::NonZero);
		checkLiteral<uint8_t, int8_t, -128, DivideAndModulo>(engine, Inputs::NonZero);
	});
}

TEST_CASE("Literal operands: shifts") {
	nautilus::testing::forEachBackend([](engine::NautilusEngine& engine) {
		// the shifted value is the val, with literal shift amounts of other types than the value. Shifts to the left
		// stay small: 0.3's tbc does not wrap an int32_t shifted out of 32 bits, with a literal or with a val alike.
		checkLiteral<int32_t, int, 3, ShiftRight, RightForms>(engine);
		checkLiteral<int32_t, int, 3, ShiftLeft, RightForms>(engine, Inputs::SmallNonNegative);
		checkLiteral<int32_t, uint8_t, 31, ShiftRight, RightForms>(engine);
		checkLiteral<int32_t, uint8_t, 31, ShiftLeft, RightForms>(engine, Inputs::SmallNonNegative);
		checkLiteral<uint32_t, int8_t, 1, ShiftRight, RightForms>(engine);
		checkLiteral<uint32_t, int8_t, 1, ShiftLeft, RightForms>(engine, Inputs::SmallNonNegative);
		checkLiteral<int64_t, int, 63, ShiftRight, RightForms>(engine);
		checkLiteral<int64_t, int, 63, ShiftLeft, RightForms>(engine, Inputs::SmallNonNegative);
		checkLiteral<int64_t, uint32_t, 40, ShiftRight, RightForms>(engine);
		checkLiteral<int64_t, uint32_t, 40, ShiftLeft, RightForms>(engine, Inputs::SmallNonNegative);
		checkLiteral<uint64_t, uint8_t, 7, ShiftRight, RightForms>(engine);
		checkLiteral<uint64_t, uint8_t, 7, ShiftLeft, RightForms>(engine, Inputs::SmallNonNegative);
		checkLiteral<int8_t, int, 7, ShiftRight, RightForms>(engine);
		checkLiteral<int8_t, int, 7, ShiftLeft, RightForms>(engine, Inputs::SmallNonNegative);
		// (a shift amount wider than the shifted value is not covered: the 0.3 tracer records such a shift with both
		// operands converted to the wider type and a result of the narrower one, which the NIR verifier rejects, with
		// a literal or with a val alike)
		// ... and the val is the shift amount
		checkLiteral<int32_t, int, 3, Shifts, LeftForms>(engine, Inputs::SmallNonNegative);
		checkLiteral<uint8_t, uint8_t, 255, Shifts, LeftForms>(engine, Inputs::SmallNonNegative);
		checkLiteral<int8_t, int, -5, Shifts, LeftForms>(engine, Inputs::SmallNonNegative);
		checkLiteral<uint32_t, int, 1000, Shifts, LeftForms>(engine, Inputs::SmallNonNegative);
		checkLiteral<int64_t, uint64_t, 0x8000000000000000ull, Shifts, LeftForms>(engine, Inputs::SmallNonNegative);
	});
}

// An integer that a floating-point type cannot represent keeps its CAST; the others are converted in C++. Both must
// give the value C++ gives.
TEST_CASE("Literal operands: floating point") {
	nautilus::testing::forEachBackend([](engine::NautilusEngine& engine) {
		SECTION("float") {
			checkLiteral<float, int, 3, Basic>(engine);
			checkLiteral<float, int, 16777217, Basic>(engine);
			checkLiteral<float, int, -2147483647 - 1, Basic>(engine);
			checkLiteral<float, int16_t, -3, Basic>(engine);
			checkLiteral<float, uint8_t, 200, Basic>(engine);
			checkLiteral<float, uint32_t, 4294967295u, Basic>(engine);
			checkLiteral<float, int64_t, 9007199254740993LL, Basic>(engine);
			checkLiteral<float, uint64_t, U64Max, Basic>(engine);
			checkLiteral<float, bool, true, Basic>(engine);
			checkLiteral<float, double, 1.5, Basic>(engine);
			checkLiteral<float, double, 0.1, Basic>(engine);
			checkLiteral<float, double, 1.0e30, Basic>(engine);
			checkLiteral<float, int, 7, Divide>(engine, Inputs::NonZero);
			checkLiteral<float, double, 0.1, Divide>(engine, Inputs::NonZero);
		}
		SECTION("double") {
			checkLiteral<double, int, 3, Basic>(engine);
			checkLiteral<double, uint32_t, 4000000000u, Basic>(engine);
			checkLiteral<double, int64_t, 9007199254740993LL, Basic>(engine);
			checkLiteral<double, int64_t, -9007199254740993LL, Basic>(engine);
			checkLiteral<double, uint64_t, U64Max, Basic>(engine);
			checkLiteral<double, uint64_t, 9223372036854775809ull, Basic>(engine);
			checkLiteral<double, float, 0.1f, Basic>(engine);
			checkLiteral<double, float, -16777217.0f, Basic>(engine);
			checkLiteral<double, bool, true, Basic>(engine);
			checkLiteral<double, uint8_t, 255, Divide>(engine, Inputs::NonZero);
			checkLiteral<double, float, 0.1f, Divide>(engine, Inputs::NonZero);
		}
		SECTION("an integral value against a floating-point literal") {
			checkLiteral<int32_t, double, 2.5, Basic>(engine);
			checkLiteral<int32_t, float, 0.1f, Basic>(engine);
			checkLiteral<int64_t, float, 0.5f, Basic>(engine);
			checkLiteral<int64_t, double, 1.0e10, Basic>(engine);
			checkLiteral<uint8_t, double, 0.5, Basic>(engine);
			checkLiteral<uint64_t, double, 3.0, Basic>(engine);
			checkLiteral<int8_t, float, -2.5f, Divide>(engine, Inputs::NonZero);
		}
	});
}

// The folding is an option; the unfolded traces must compute the same values.
TEST_CASE("Literal operands: without static constant folding") {
	nautilus::testing::forEachBackend(
	    [](engine::NautilusEngine& engine) {
		    checkLiteral<int8_t, int, 300, Basic>(engine);
		    checkLiteral<uint32_t, int, -1, Basic>(engine);
		    checkLiteral<int64_t, uint64_t, U64Max, Basic>(engine);
		    checkLiteral<uint64_t, int8_t, -128, Basic>(engine);
		    checkLiteral<float, int, 16777217, Basic>(engine);
		    checkLiteral<double, uint64_t, U64Max, Basic>(engine);
		    checkLiteral<int32_t, uint8_t, 31, ShiftRight, RightForms>(engine);
		    checkLiteral<int32_t, uint8_t, 31, ShiftLeft, RightForms>(engine, Inputs::SmallNonNegative);
	    },
	    true, [](engine::Options& options) { options.setOption("engine.foldStaticConstants", false); });
}

// A static_val that changes between the uses (a loop counter) is the operand of each use with its current value.
namespace {
val<int64_t> accumulateWithStaticCounter(val<int64_t> x) {
	val<int64_t> acc = x;
	for (static_val<int32_t> i = -3; i < 5; i++) {
		acc = (acc * 3 + i) ^ (acc >> 1);
		acc = acc - i;
		if (acc > i) {
			acc = acc + 1;
		}
	}
	return acc;
}

val<uint32_t> accumulateUnsignedWithStaticCounter(val<uint32_t> x) {
	val<uint32_t> acc = x;
	for (static_val<int32_t> i = -3; i < 3; i++) {
		acc = acc * 7u + i;
		if (acc < i) {
			acc = acc + 1u;
		}
	}
	return acc;
}

int64_t accumulateNative(int64_t x) {
	int64_t acc = x;
	for (int32_t i = -3; i < 5; i++) {
		acc = static_cast<int64_t>((static_cast<uint64_t>(acc) * 3 + static_cast<uint64_t>(static_cast<int64_t>(i)))) ^
		      (acc >> 1);
		acc = static_cast<int64_t>(static_cast<uint64_t>(acc) - static_cast<uint64_t>(static_cast<int64_t>(i)));
		if (acc > i) {
			acc = acc + 1;
		}
	}
	return acc;
}

uint32_t accumulateUnsignedNative(uint32_t x) {
	uint32_t acc = x;
	for (int32_t i = -3; i < 3; i++) {
		acc = acc * 7u + static_cast<uint32_t>(i);
		if (acc < static_cast<uint32_t>(i)) {
			acc = acc + 1u;
		}
	}
	return acc;
}
} // namespace

TEST_CASE("Literal operands: static_val loop counters") {
	nautilus::testing::forEachBackend([](engine::NautilusEngine& engine) {
		auto f = engine.registerFunction(accumulateWithStaticCounter);
		auto g = engine.registerFunction(accumulateUnsignedWithStaticCounter);
		for (int64_t x : {int64_t(0), int64_t(1), int64_t(-1), int64_t(1234567890123), int64_t(-987654321)}) {
			INFO("x = " << x);
			CHECK(f(x) == accumulateNative(x));
		}
		for (uint32_t x : {0u, 1u, 7u, 0xffffffffu, 0x80000000u}) {
			INFO("x = " << x);
			CHECK(g(x) == accumulateUnsignedNative(x));
		}
	});
}

} // namespace nautilus::engine

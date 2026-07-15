#include "Ast.hpp"
#include "Types.hpp"
#include <array>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <limits>
#include <string>
#include <vector>

// Seed corpus generator for the differential AST fuzzer (issue #359).
//
// The fuzzer's generator (Ast.hpp's generateNode/generatePtrNode, driven by
// ByteReader.hpp) decodes every AST/argument/buffer value straight out of
// raw fuzzer-buffer bytes, so libFuzzer's mutator starts from nothing:
// semantically interesting constants (INT_MIN, NaN, +-0.0, ...) are only
// ever reached by chance. This tool builds a small corpus of hand-picked
// byte streams that decode -- via the *exact same* ByteReader consumption
// order the harness uses -- to programs that exercise boundary constants and
// the specific bug shapes documented in fuzz/README.md's "Known findings"
// (signed-zero negation, narrow-integer wraparound, cast range clamping).
//
// This is an encode-only mirror of the decode-only ByteReader/generateNode
// pair: it never calls generate<T>() itself (that would consume bytes it
// hasn't produced yet), it instead builds the byte stream a *given* small
// AST would decode from, by replaying generateNode's own control-flow
// decisions (which byte ranges pick "leaf vs internal", which table index
// picks a Kind, ...) in reverse. Kind indices are looked up by scanning the
// real INT_KINDS/FLOAT_KINDS/BOOL_KINDS/ALL_TYPES/ROOT_TYPES tables from
// Ast.hpp/Types.hpp directly (never hand-copied), so a reordering or
// resizing of those tables in the generator is automatically picked up here
// too -- see the "Regenerating" section in README.md.
//
// Usage: nautilus-fuzz-seedgen <output-dir>
// Writes one file per seed, named seed-<NNN>-<description>.bin.

namespace nautilus::fuzz::seedgen {

/// Encode-only counterpart of ByteReader: appends bytes chosen so that
/// feeding the resulting buffer through ByteReader (as generateNode does)
/// reproduces the intended decision at each step.
class Writer {
public:
	std::vector<uint8_t> buf;

	void byte(uint8_t b) {
		buf.push_back(b);
	}

	/// Raw little-endian encoding of a trivially-copyable value, matching
	/// ByteReader::consume<T>().
	template <typename T>
	void raw(T v) {
		uint8_t bytes[sizeof(T)];
		std::memcpy(bytes, &v, sizeof(T));
		for (uint8_t b : bytes) {
			buf.push_back(b);
		}
	}

	/// Every table index this generator needs (Kind-table / TypeId-table
	/// selection) is decoded by the generator as a single `reader.byte() % n`
	/// -- NOT the (separate, currently unused by generateNode/checkOne)
	/// multi-byte ByteReader::pick() helper. Picking the raw byte == idx
	/// (idx < n <= 256, true for every table indexed here) makes
	/// `byte % n == idx`.
	void selectIndex(uint32_t idx) {
		byte(static_cast<uint8_t>(idx));
	}

	/// generateNode's leading `sel` byte, forced down the Const-leaf path:
	/// `(sel & 0x3) == 0` selects "leaf", `sel & 0x4` clear selects Const
	/// (not Param), and (for the loop leaves) `sel & 0x6 != 0x6` avoids the
	/// LoopIndex/LoopAcc branch -- moot here since every seed AST is built at
	/// loopDepth == 0, but keeping the byte fully explicit either way.
	void constLeafSelector() {
		byte(0x00);
	}

	/// generateNode's leading `sel` byte, forced down the "internal node"
	/// path: any value with `sel & 0x3 != 0` clears the leaf bias, and
	/// `sel & 0x18 != 0x18` avoids the (breakDepth > 0 only, so moot at
	/// breakDepth == 0) LoopBreak/LoopContinue branch.
	void internalSelector() {
		byte(0x01);
	}

	/// Index of `k` within a Kind table (INT_KINDS/FLOAT_KINDS/BOOL_KINDS),
	/// looked up by value so a reordering of the table in Ast.hpp can never
	/// silently desync this generator from it.
	template <typename Table>
	static uint32_t indexOf(Kind k, const Table& table) {
		for (uint32_t i = 0; i < table.size(); ++i) {
			if (table[i] == k) {
				return i;
			}
		}
		std::fprintf(stderr, "SeedGen: Kind not found in table (Ast.hpp table changed?)\n");
		std::abort();
	}

	template <typename Table>
	static uint32_t indexOf(TypeId t, const Table& table) {
		for (uint32_t i = 0; i < table.size(); ++i) {
			if (table[i] == t) {
				return i;
			}
		}
		std::fprintf(stderr, "SeedGen: TypeId not found in table (Types.hpp table changed?)\n");
		std::abort();
	}
};

// The Kind tables live in Ast.hpp's `detail` namespace as C-array
// `inline constexpr Kind[]`. Wrap them as std::array views once here so
// Writer::indexOf's generic lookup works uniformly (and so a future resize
// of a C array is still picked up automatically via sizeof).
template <typename T, size_t N>
constexpr std::array<T, N> toArray(const T (&src)[N]) {
	std::array<T, N> out {};
	for (size_t i = 0; i < N; ++i) {
		out[i] = src[i];
	}
	return out;
}

inline const auto kIntKinds = toArray(detail::INT_KINDS);
inline const auto kFloatKinds = toArray(detail::FLOAT_KINDS);
inline const auto kBoolKinds = toArray(detail::BOOL_KINDS);
inline const auto kAllTypes = toArray(ALL_TYPES);
inline const auto kRootTypes = toArray(ROOT_TYPES);

/// Which Kind table generateNode<T> consults, mirroring its own
/// if-constexpr dispatch (Ast.hpp).
template <typename T>
const auto& kindTableFor() {
	if constexpr (std::is_same_v<T, bool>) {
		return kBoolKinds;
	} else if constexpr (std::is_floating_point_v<T>) {
		return kFloatKinds;
	} else {
		return kIntKinds;
	}
}

/// Append a Kind::Const leaf holding `v` (generateNode's leaf branch).
template <typename T>
void writeConst(Writer& w, T v) {
	w.constLeafSelector();
	w.raw<T>(v);
}

/// Append an internal node of `kind` with `arity` already-encoded children,
/// where `childWriter(i)` appends child `i`'s own bytes. Mirrors
/// generateNode's non-leaf branch for every Kind this file uses (Neg, Add,
/// Sub, Mul, Div, Eq, Cast): none of them hit the Load/Store/Call/Loop/
/// isPtrCompare special-cased kid layouts, so the shared "childCount kids in
/// order" path applies uniformly.
template <typename T, typename ChildFn>
void writeNode(Writer& w, Kind kind, int arity, ChildFn&& childWriter) {
	w.internalSelector();
	w.selectIndex(Writer::indexOf(kind, kindTableFor<T>()));
	for (int i = 0; i < arity; ++i) {
		childWriter(i);
	}
}

template <typename T>
void writeNeg(Writer& w, T v) {
	writeNode<T>(w, Kind::Neg, 1, [&](int) { writeConst<T>(w, v); });
}

template <typename T>
void writeBinary(Writer& w, Kind kind, T a, T b) {
	writeNode<T>(w, kind, 2, [&](int i) { writeConst<T>(w, i == 0 ? a : b); });
}

/// Cast(Const(v)) with an explicit target TypeId -- generateNode's Cast case
/// consumes the target-type byte right after the kind-selection byte, before
/// any children.
template <typename T>
void writeCast(Writer& w, T v, TypeId target) {
	w.internalSelector();
	w.selectIndex(Writer::indexOf(Kind::Cast, kindTableFor<T>()));
	w.selectIndex(Writer::indexOf(target, kAllTypes));
	writeConst<T>(w, v);
}

/// Full input: root-type byte, shape byte (arity3, the historically default
/// shape -- shape index 2 lands on "arity3" in both the bool 5-shape menu and
/// the 7-shape menu every other domain uses, see checkOneTyped in
/// Harness.hpp), then the AST bytes `body` produces. Args/initial-buffer
/// bytes are deliberately omitted: every seed AST here is Const-only (no
/// Kind::Param, no Load/Store), so ByteReader's past-end zero-padding makes
/// omitting them exactly equivalent to writing explicit zeros, and omitting
/// them keeps every seed file minimal.
template <typename T, typename BodyFn>
std::vector<uint8_t> buildInput(TypeId rootType, BodyFn&& body) {
	Writer w;
	w.selectIndex(Writer::indexOf(rootType, kRootTypes));
	constexpr uint8_t arity3ShapeByte = 2;
	w.byte(arity3ShapeByte);
	body(w);
	return w.buf;
}

struct Seed {
	std::string name;
	std::vector<uint8_t> bytes;
};

template <typename T>
TypeId typeIdOf();
template <>
TypeId typeIdOf<int8_t>() {
	return TypeId::I8;
}
template <>
TypeId typeIdOf<int16_t>() {
	return TypeId::I16;
}
template <>
TypeId typeIdOf<int32_t>() {
	return TypeId::I32;
}
template <>
TypeId typeIdOf<int64_t>() {
	return TypeId::I64;
}
template <>
TypeId typeIdOf<uint8_t>() {
	return TypeId::U8;
}
template <>
TypeId typeIdOf<uint16_t>() {
	return TypeId::U16;
}
template <>
TypeId typeIdOf<uint32_t>() {
	return TypeId::U32;
}
template <>
TypeId typeIdOf<uint64_t>() {
	return TypeId::U64;
}
template <>
TypeId typeIdOf<float>() {
	return TypeId::F32;
}
template <>
TypeId typeIdOf<double>() {
	return TypeId::F64;
}

/// Per-integer-width boundary constants: 0, 1, -1 (signed only), min, max,
/// and the smallest/largest power of two that fits.
template <typename T>
std::vector<T> intBoundaryValues() {
	std::vector<T> vals {T(0), T(1)};
	if constexpr (std::is_signed_v<T>) {
		vals.push_back(T(-1));
	}
	vals.push_back(std::numeric_limits<T>::min());
	vals.push_back(std::numeric_limits<T>::max());
	// Highest power of two representable in T (sign bit boundary for signed
	// types, top bit for unsigned).
	using U = std::make_unsigned_t<T>;
	constexpr int bits = sizeof(T) * 8;
	const U hiBit = U(1) << (bits - (std::is_signed_v<T> ? 2 : 1));
	vals.push_back(static_cast<T>(hiBit));
	return vals;
}

template <typename T>
std::vector<T> floatBoundaryValues() {
	return {
	    T(0.0),
	    -T(0.0),
	    T(1.0),
	    -T(1.0),
	    std::numeric_limits<T>::quiet_NaN(),
	    std::numeric_limits<T>::infinity(),
	    -std::numeric_limits<T>::infinity(),
	    std::numeric_limits<T>::denorm_min(), // smallest subnormal
	    std::numeric_limits<T>::epsilon(),
	    std::numeric_limits<T>::min(), // smallest positive normal
	    std::numeric_limits<T>::max(),
	    -std::numeric_limits<T>::max(),
	};
}

template <typename T>
void addLeafSeeds(std::vector<Seed>& seeds) {
	const TypeId ty = typeIdOf<T>();
	std::vector<T> values;
	if constexpr (std::is_floating_point_v<T>) {
		values = floatBoundaryValues<T>();
	} else {
		values = intBoundaryValues<T>();
	}
	for (T v : values) {
		auto bytes = buildInput<T>(ty, [&](Writer& w) { writeConst<T>(w, v); });
		seeds.push_back({std::string("const-") + typeSuffix<T>() + "-" + formatValue<T>(v), std::move(bytes)});
	}
}

/// Bool domain has no width/sign boundary values, just its two states -- a
/// small seed pair so the mutator has a starting point for the bool domain
/// too, mirroring addLeafSeeds<T> above for the other ten types.
void addBoolLeafSeeds(std::vector<Seed>& seeds) {
	for (bool v : {false, true}) {
		auto bytes = buildInput<bool>(TypeId::Bool, [&](Writer& w) { writeConst<bool>(w, v); });
		seeds.push_back({std::string("const-bool-") + (v ? "true" : "false"), std::move(bytes)});
	}
}

/// Signed-only: Neg(MIN) -- wrappingNeg(MIN) == MIN (two's-complement
/// negation overflow), the integer-domain twin of the float signed-zero
/// negation bug documented in README.md "Known findings".
template <typename T>
void addNegMinSeed(std::vector<Seed>& seeds) {
	if constexpr (std::is_signed_v<T> && !std::is_floating_point_v<T>) {
		const TypeId ty = typeIdOf<T>();
		auto bytes = buildInput<T>(ty, [&](Writer& w) { writeNeg<T>(w, std::numeric_limits<T>::min()); });
		seeds.push_back({std::string("neg-min-") + typeSuffix<T>(), std::move(bytes)});
	}
}

/// Signed-only: Div(MIN, -1). EvalNative.hpp's safeDivisor forces divisors
/// away from both 0 and -1 (the one divisor for which MIN/divisor traps), so
/// this decodes to (native and traced) MIN / 1 == MIN -- exercising that
/// exact steer-away-from-(-1) branch instead of undefined behavior.
template <typename T>
void addDivMinByMinusOneSeed(std::vector<Seed>& seeds) {
	if constexpr (std::is_signed_v<T> && !std::is_floating_point_v<T>) {
		const TypeId ty = typeIdOf<T>();
		auto bytes =
		    buildInput<T>(ty, [&](Writer& w) { writeBinary<T>(w, Kind::Div, std::numeric_limits<T>::min(), T(-1)); });
		seeds.push_back({std::string("div-min-by-neg1-") + typeSuffix<T>(), std::move(bytes)});
	}
}

/// Integer domain: Add(MAX, 1) -- one-past-max wraparound (defined modulo
/// 2^n, see README.md "Soundness model"), the narrow-int
/// wrap-then-compare shape from the issue: wrapping straight into the type's
/// own MIN for signed types, or 0 for unsigned.
template <typename T>
void addAddOverflowSeed(std::vector<Seed>& seeds) {
	if constexpr (!std::is_floating_point_v<T>) {
		const TypeId ty = typeIdOf<T>();
		auto bytes =
		    buildInput<T>(ty, [&](Writer& w) { writeBinary<T>(w, Kind::Add, std::numeric_limits<T>::max(), T(1)); });
		seeds.push_back({std::string("add-overflow-") + typeSuffix<T>(), std::move(bytes)});
	}
}

/// Integer domain: Eq(Add(MAX, 1), 0) -- wrap-then-compare, forcing the
/// wrapped result through a comparison instead of just a return value.
template <typename T>
void addWrapThenCompareSeed(std::vector<Seed>& seeds) {
	if constexpr (!std::is_floating_point_v<T>) {
		const TypeId ty = typeIdOf<T>();
		auto bytes = buildInput<T>(ty, [&](Writer& w) {
			writeNode<T>(w, Kind::Eq, 2, [&](int i) {
				if (i == 0) {
					writeBinary<T>(w, Kind::Add, std::numeric_limits<T>::max(), T(1));
				} else {
					writeConst<T>(w, T(0));
				}
			});
		});
		seeds.push_back({std::string("wrap-then-compare-") + typeSuffix<T>(), std::move(bytes)});
	}
}

/// Float domain: Div(0.0, 0.0) == NaN, Div(1.0, 0.0) == +inf,
/// Div(-1.0, 0.0) == -inf -- IEEE-754-defined division-by-zero results,
/// distinct enough (payload/sign) that byte-for-byte parity across every
/// backend is worth its own seed each.
template <typename T>
void addFloatDivSeeds(std::vector<Seed>& seeds) {
	if constexpr (std::is_floating_point_v<T>) {
		const TypeId ty = typeIdOf<T>();
		auto zeroByZero = buildInput<T>(ty, [&](Writer& w) { writeBinary<T>(w, Kind::Div, T(0.0), T(0.0)); });
		seeds.push_back({std::string("div-0-by-0-") + typeSuffix<T>(), std::move(zeroByZero)});

		auto oneByZero = buildInput<T>(ty, [&](Writer& w) { writeBinary<T>(w, Kind::Div, T(1.0), T(0.0)); });
		seeds.push_back({std::string("div-1-by-0-") + typeSuffix<T>(), std::move(oneByZero)});

		auto negOneByZero = buildInput<T>(ty, [&](Writer& w) { writeBinary<T>(w, Kind::Div, T(-1.0), T(0.0)); });
		seeds.push_back({std::string("div-neg1-by-0-") + typeSuffix<T>(), std::move(negOneByZero)});
	}
}

/// Float domain: Neg(0.0) -- the exact signed-zero-negation shape documented
/// in README.md "Known findings" (`val<T>::operator-()`'s cancellation bug):
/// a correct IEEE-754 negation must flip +0.0 to -0.0, which a
/// subtraction-based implementation gets wrong.
template <typename T>
void addNegZeroSeed(std::vector<Seed>& seeds) {
	if constexpr (std::is_floating_point_v<T>) {
		const TypeId ty = typeIdOf<T>();
		auto bytes = buildInput<T>(ty, [&](Writer& w) { writeNeg<T>(w, T(0.0)); });
		seeds.push_back({std::string("neg-zero-") + typeSuffix<T>(), std::move(bytes)});
	}
}

/// Cast at range limits: (To)(From)MAX and (To)(From)MIN for a narrowing
/// integer cast, and float->int casts of the boundary/NaN/inf values that
/// hiLimitExclusive/loLimitInclusive (Types.hpp) specifically clamp.
template <typename From, typename To>
void addCastBoundarySeeds(std::vector<Seed>& seeds) {
	const TypeId fromTy = typeIdOf<From>();
	const TypeId toTy = typeIdOf<To>();
	const std::string suffix = std::string(typeSuffix<From>()) + "-to-" + typeSuffix<To>();

	auto castMax =
	    buildInput<From>(fromTy, [&](Writer& w) { writeCast<From>(w, std::numeric_limits<From>::max(), toTy); });
	seeds.push_back({"cast-max-" + suffix, std::move(castMax)});

	auto castMin =
	    buildInput<From>(fromTy, [&](Writer& w) { writeCast<From>(w, std::numeric_limits<From>::lowest(), toTy); });
	seeds.push_back({"cast-min-" + suffix, std::move(castMin)});

	if constexpr (std::is_floating_point_v<From> && !std::is_floating_point_v<To>) {
		auto castNan = buildInput<From>(
		    fromTy, [&](Writer& w) { writeCast<From>(w, std::numeric_limits<From>::quiet_NaN(), toTy); });
		seeds.push_back({"cast-nan-" + suffix, std::move(castNan)});

		auto castInf = buildInput<From>(
		    fromTy, [&](Writer& w) { writeCast<From>(w, std::numeric_limits<From>::infinity(), toTy); });
		seeds.push_back({"cast-inf-" + suffix, std::move(castInf)});
	}
}

std::vector<Seed> buildAllSeeds() {
	std::vector<Seed> seeds;

	addLeafSeeds<int8_t>(seeds);
	addLeafSeeds<int16_t>(seeds);
	addLeafSeeds<int32_t>(seeds);
	addLeafSeeds<int64_t>(seeds);
	addLeafSeeds<uint8_t>(seeds);
	addLeafSeeds<uint16_t>(seeds);
	addLeafSeeds<uint32_t>(seeds);
	addLeafSeeds<uint64_t>(seeds);
	addLeafSeeds<float>(seeds);
	addLeafSeeds<double>(seeds);
	addBoolLeafSeeds(seeds);

	addNegMinSeed<int8_t>(seeds);
	addNegMinSeed<int16_t>(seeds);
	addNegMinSeed<int32_t>(seeds);
	addNegMinSeed<int64_t>(seeds);

	addDivMinByMinusOneSeed<int8_t>(seeds);
	addDivMinByMinusOneSeed<int16_t>(seeds);
	addDivMinByMinusOneSeed<int32_t>(seeds);
	addDivMinByMinusOneSeed<int64_t>(seeds);

	addAddOverflowSeed<int8_t>(seeds);
	addAddOverflowSeed<int16_t>(seeds);
	addAddOverflowSeed<int32_t>(seeds);
	addAddOverflowSeed<int64_t>(seeds);
	addAddOverflowSeed<uint8_t>(seeds);
	addAddOverflowSeed<uint16_t>(seeds);
	addAddOverflowSeed<uint32_t>(seeds);
	addAddOverflowSeed<uint64_t>(seeds);

	addWrapThenCompareSeed<int8_t>(seeds);
	addWrapThenCompareSeed<uint32_t>(seeds);

	addFloatDivSeeds<float>(seeds);
	addFloatDivSeeds<double>(seeds);

	addNegZeroSeed<float>(seeds);
	addNegZeroSeed<double>(seeds);

	addCastBoundarySeeds<int64_t, int8_t>(seeds);
	addCastBoundarySeeds<int32_t, uint8_t>(seeds);
	addCastBoundarySeeds<double, int32_t>(seeds);
	addCastBoundarySeeds<float, int64_t>(seeds);
	addCastBoundarySeeds<double, uint64_t>(seeds);

	return seeds;
}

} // namespace nautilus::fuzz::seedgen

int main(int argc, char** argv) {
	if (argc != 2) {
		std::fprintf(stderr, "usage: %s <output-dir>\n", argv[0]);
		return 1;
	}
	const std::filesystem::path outDir(argv[1]);
	std::filesystem::create_directories(outDir);

	const auto seeds = nautilus::fuzz::seedgen::buildAllSeeds();
	int index = 0;
	for (const auto& seed : seeds) {
		char prefix[16];
		std::snprintf(prefix, sizeof(prefix), "seed-%03d-", index++);
		const std::filesystem::path path = outDir / (prefix + seed.name + ".bin");
		std::ofstream out(path, std::ios::binary);
		out.write(reinterpret_cast<const char*>(seed.bytes.data()), static_cast<std::streamsize>(seed.bytes.size()));
	}
	std::printf("wrote %zu seed files to %s\n", seeds.size(), outDir.string().c_str());
	return 0;
}

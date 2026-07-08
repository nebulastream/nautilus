
#include "nautilus/compiler/backends/tbc/TBCThunkCall.hpp"
#include <cstring>
#include <type_traits>
#include <utility>

// ABI gate for the reordered typed-thunk call. The reorder trick (integer
// args first, fp args after, each class keeping its relative order) requires
// the two argument classes to be register-assigned independently, which holds
// for SysV x86-64 and AAPCS64 (incl. Apple's arm64 variant) but not e.g. for
// Windows x64, whose four argument slots are shared between classes.
#if (defined(__x86_64__) || defined(__aarch64__)) && !defined(_WIN32)
#define TBC_THUNKCALL_SUPPORTED 1
#else
#define TBC_THUNKCALL_SUPPORTED 0
#endif

namespace nautilus::compiler::tbc {

namespace {

// Spill-safety invariants of the reorder trick (see TBCCode.hpp): AArch64 has
// 8 integer + 8 fp argument registers, so nothing spills at all; SysV x86-64
// has 6 integer + 8 SSE registers, so integer args 7-8 spill into stack slots
// that SysV rounds to 8 bytes (layout-compatible with a uint64_t parameter),
// and fp args must never spill because a reordered fp stack arg could land in
// a different slot than in the callee's real signature.
static_assert(kThunkMaxIntArgs <= 8, "integer args beyond 8 would spill on AArch64 with packed layout (Apple)");
static_assert(kThunkMaxFpArgs <= 8, "fp args must stay in registers on both supported ABIs");

#if TBC_THUNKCALL_SUPPORTED

template <size_t>
using IntParam = uint64_t;

template <uint32_t Bits, size_t K>
using FpParam = std::conditional_t<((Bits >> K) & 1u) != 0, double, float>;

template <class T>
T loadFp(uint64_t slot) {
	T value;
	std::memcpy(&value, &slot, sizeof(T));
	return value;
}

/// One pre-compiled caller per signature shape: `Ret` return, `sizeof...(I)`
/// leading uint64_t (integer-class) parameters, and one f32/f64 parameter per
/// fp position `F` (kind selected by bit `F` of `FpBits`). Calling `target`
/// through the shape's function-pointer type is what makes the compiler emit
/// the native argument-register moves; the cast is ABI-sound under the
/// reorder/caps rules above (the codebase already relies on the same
/// reasoning for the escaping-function-pointer trampolines).
template <class Ret, uint32_t FpBits, class IntSeq, class FpSeq>
struct Thunk;

template <class Ret, uint32_t FpBits, size_t... I, size_t... F>
struct Thunk<Ret, FpBits, std::index_sequence<I...>, std::index_sequence<F...>> {
	static uint64_t call(void* target, [[maybe_unused]] const uint64_t* intArgs,
	                     [[maybe_unused]] const uint64_t* fpArgs) {
		using Fn = Ret (*)(IntParam<I>..., FpParam<FpBits, F>...);
		const auto fn = reinterpret_cast<Fn>(target);
		if constexpr (std::is_void_v<Ret>) {
			fn(intArgs[I]..., loadFp<FpParam<FpBits, F>>(fpArgs[F])...);
			return 0;
		} else if constexpr (std::is_floating_point_v<Ret>) {
			const Ret result = fn(intArgs[I]..., loadFp<FpParam<FpBits, F>>(fpArgs[F])...);
			uint64_t slot = 0;
			std::memcpy(&slot, &result, sizeof(result));
			return slot;
		} else {
			// Integer-class returns travel as uint64_t; the marshaller
			// truncates to the declared width (upper bits of a narrow return
			// register are unspecified).
			return static_cast<uint64_t>(fn(intArgs[I]..., loadFp<FpParam<FpBits, F>>(fpArgs[F])...));
		}
	}
};

/// Fp-pattern rows are flattened per fp-arg count: patterns with NF fp args
/// occupy indices [2^NF - 1, 2^(NF+1) - 1) keyed by their f32/f64 bitmask.
constexpr std::array<size_t, kThunkMaxFpArgs + 1> kFpPatternOffset = [] {
	std::array<size_t, kThunkMaxFpArgs + 1> offsets {};
	for (size_t n = 0; n <= kThunkMaxFpArgs; ++n) {
		offsets[n] = (size_t {1} << n) - 1;
	}
	return offsets;
}();
constexpr size_t kFpPatternCount = (size_t {1} << (kThunkMaxFpArgs + 1)) - 1;

using FpPatternRow = std::array<ExtThunkFn, kFpPatternCount>;
using RetTable = std::array<FpPatternRow, kThunkMaxIntArgs + 1>;

template <class Ret, size_t NumInt, size_t NumFp>
constexpr void fillFpPatterns(FpPatternRow& row) {
	[&]<size_t... Bits>(std::index_sequence<Bits...>) {
		((row[kFpPatternOffset[NumFp] + Bits] =
		      &Thunk<Ret, static_cast<uint32_t>(Bits), std::make_index_sequence<NumInt>,
		             std::make_index_sequence<NumFp>>::call),
		 ...);
	}(std::make_index_sequence<(size_t {1} << NumFp)>());
}

template <class Ret, size_t NumInt>
constexpr FpPatternRow makeFpPatternRow() {
	FpPatternRow row {};
	[&]<size_t... NumFp>(std::index_sequence<NumFp...>) {
		(fillFpPatterns<Ret, NumInt, NumFp>(row), ...);
	}(std::make_index_sequence<kThunkMaxFpArgs + 1>());
	return row;
}

template <class Ret>
constexpr RetTable makeRetTable() {
	return [&]<size_t... NumInt>(std::index_sequence<NumInt...>) {
		return RetTable {makeFpPatternRow<Ret, NumInt>()...};
	}(std::make_index_sequence<kThunkMaxIntArgs + 1>());
}

/// Return kinds share thunks: void, all integer-class (uint64_t), f32, f64.
enum class RetKind : uint8_t { Void = 0, Int, F32, F64 };

const std::array<RetTable, 4> kThunkTable {makeRetTable<void>(), makeRetTable<uint64_t>(), makeRetTable<float>(),
                                           makeRetTable<double>()};

#endif // TBC_THUNKCALL_SUPPORTED

} // namespace

bool thunkCallsSupported() {
	return TBC_THUNKCALL_SUPPORTED != 0;
}

void resolveExtCallThunk(CallSite& site) {
	site.ext = ResolvedThunk {};
#if TBC_THUNKCALL_SUPPORTED
	size_t numInt = 0;
	size_t numFp = 0;
	uint32_t fpBits = 0;
	for (size_t i = 0; i < site.argTypes.size(); ++i) {
		const uint16_t reg = site.argRegs[i];
		switch (site.argTypes[i]) {
		case Type::b:
		case Type::ui8:
		case Type::ui16:
		case Type::ui32:
		case Type::i64:
		case Type::ui64:
		case Type::ptr:
			// Slots already hold these zero-extended (or bit-exact).
			if (numInt == kThunkMaxIntArgs) {
				return;
			}
			site.ext.intLoads[numInt++] = {reg, ArgLoad::Raw};
			break;
		case Type::i8:
			if (numInt == kThunkMaxIntArgs) {
				return;
			}
			site.ext.intLoads[numInt++] = {reg, ArgLoad::SExt8};
			break;
		case Type::i16:
			if (numInt == kThunkMaxIntArgs) {
				return;
			}
			site.ext.intLoads[numInt++] = {reg, ArgLoad::SExt16};
			break;
		case Type::i32:
			if (numInt == kThunkMaxIntArgs) {
				return;
			}
			site.ext.intLoads[numInt++] = {reg, ArgLoad::SExt32};
			break;
		case Type::f32:
			if (numFp == kThunkMaxFpArgs) {
				return;
			}
			site.ext.fpRegs[numFp++] = reg;
			break;
		case Type::f64:
			if (numFp == kThunkMaxFpArgs) {
				return;
			}
			fpBits |= 1u << numFp;
			site.ext.fpRegs[numFp++] = reg;
			break;
		default:
			return;
		}
	}
	RetKind retKind;
	switch (site.returnType) {
	case Type::v:
		retKind = RetKind::Void;
		break;
	case Type::b:
	case Type::i8:
	case Type::i16:
	case Type::i32:
	case Type::i64:
	case Type::ui8:
	case Type::ui16:
	case Type::ui32:
	case Type::ui64:
	case Type::ptr:
		retKind = RetKind::Int;
		break;
	case Type::f32:
		retKind = RetKind::F32;
		break;
	case Type::f64:
		retKind = RetKind::F64;
		break;
	default:
		return;
	}
	site.ext.numInt = static_cast<uint8_t>(numInt);
	site.ext.numFp = static_cast<uint8_t>(numFp);
	site.ext.fn = kThunkTable[static_cast<size_t>(retKind)][numInt][kFpPatternOffset[numFp] + fpBits];
#endif
}

} // namespace nautilus::compiler::tbc

#pragma once

#include <cstdint>
#include <type_traits>

namespace nautilus::fuzz {

/// Number of native helper functions a Kind::Call node can target (its imm
/// is drawn modulo this).
inline constexpr int NUM_CALLEES = 2;

/// Pure native helpers exposed to generated programs through Kind::Call.
/// The native oracle calls them directly; the traced kernel calls the exact
/// same instantiation through nautilus::invoke(). Because both legs execute
/// the identical native code, the differential surface is exclusively the
/// backend's call lowering: argument/return marshalling, narrow-integer ABI
/// extension, float register passing. Every helper must be fully defined for
/// all inputs (unsigned-wraparound arithmetic; IEEE 754 for floats).
template <typename T>
T calleeMix(T a, T b) {
	if constexpr (std::is_floating_point_v<T>) {
		return a * T(0.5) + b;
	} else {
		using U = std::make_unsigned_t<T>;
		return static_cast<T>(static_cast<U>(static_cast<U>(a) * U(3)) + static_cast<U>(b));
	}
}

template <typename T>
T calleeMin(T a, T b) {
	// NaN-safe by identity: `NaN < b` is false, so b is returned. Both the
	// oracle and the compiled kernel run this same code, so the results match
	// by construction whatever the operands are.
	return a < b ? a : b;
}

inline const char* calleeName(uint64_t imm) {
	return imm % NUM_CALLEES == 0 ? "mix" : "min";
}

} // namespace nautilus::fuzz

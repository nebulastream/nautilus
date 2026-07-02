#pragma once

#include "nautilus/compiler/backends/tbc/TBCCode.hpp"
#include <cstring>
#include <type_traits>

namespace nautilus::compiler::tbc {

// Value-op semantics shared by all three dispatch skins (tail-call, computed
// goto, switch). Every body is a tiny force-inlinable function
// `void(const Instr&, uint64_t*)`; the skins add the per-opcode dispatch glue.
//
// Register slots are untyped 64 bits. Writers normalize: bools are stored as
// 0/1, narrow integers are zero-extended, floats/pointers keep their bit
// pattern (same invariant as the bc backend's writeReg). Readers use the exact
// IR-typed width, so the low bytes are always authoritative.

template <class T>
inline T readReg(const uint64_t* fp, uint16_t r) {
	T value;
	std::memcpy(&value, &fp[r], sizeof(T));
	return value;
}

template <class T>
inline void writeReg(uint64_t* fp, uint16_t r, T value) {
	if constexpr (std::is_same_v<T, bool>) {
		fp[r] = value ? 1u : 0u;
	} else if constexpr (std::is_integral_v<T>) {
		fp[r] = static_cast<uint64_t>(static_cast<std::make_unsigned_t<T>>(value));
	} else {
		fp[r] = 0;
		std::memcpy(&fp[r], &value, sizeof(T));
	}
}

inline void opNop(const Instr&, uint64_t*) {
}

inline void opMov(const Instr& i, uint64_t* fp) {
	fp[i.a] = fp[i.b];
}

template <class T>
inline void opMovImm(const Instr& i, uint64_t* fp) {
	writeReg<T>(fp, i.a, static_cast<T>(static_cast<int16_t>(i.c)));
}

template <class T>
inline void opAdd(const Instr& i, uint64_t* fp) {
	writeReg<T>(fp, i.a, static_cast<T>(readReg<T>(fp, i.b) + readReg<T>(fp, i.c)));
}

template <class T>
inline void opSub(const Instr& i, uint64_t* fp) {
	writeReg<T>(fp, i.a, static_cast<T>(readReg<T>(fp, i.b) - readReg<T>(fp, i.c)));
}

template <class T>
inline void opMul(const Instr& i, uint64_t* fp) {
	writeReg<T>(fp, i.a, static_cast<T>(readReg<T>(fp, i.b) * readReg<T>(fp, i.c)));
}

template <class T>
inline void opDiv(const Instr& i, uint64_t* fp) {
	writeReg<T>(fp, i.a, static_cast<T>(readReg<T>(fp, i.b) / readReg<T>(fp, i.c)));
}

template <class T>
inline void opMod(const Instr& i, uint64_t* fp) {
	writeReg<T>(fp, i.a, static_cast<T>(readReg<T>(fp, i.b) % readReg<T>(fp, i.c)));
}

template <class T>
inline void opAddImm(const Instr& i, uint64_t* fp) {
	writeReg<T>(fp, i.a, static_cast<T>(readReg<T>(fp, i.b) + static_cast<T>(static_cast<int16_t>(i.c))));
}

template <class T>
inline void opSubImm(const Instr& i, uint64_t* fp) {
	writeReg<T>(fp, i.a, static_cast<T>(readReg<T>(fp, i.b) - static_cast<T>(static_cast<int16_t>(i.c))));
}

template <class T>
inline void opMulImm(const Instr& i, uint64_t* fp) {
	writeReg<T>(fp, i.a, static_cast<T>(readReg<T>(fp, i.b) * static_cast<T>(static_cast<int16_t>(i.c))));
}

template <class T>
inline void opEq(const Instr& i, uint64_t* fp) {
	writeReg<bool>(fp, i.a, readReg<T>(fp, i.b) == readReg<T>(fp, i.c));
}

template <class T>
inline void opNe(const Instr& i, uint64_t* fp) {
	writeReg<bool>(fp, i.a, readReg<T>(fp, i.b) != readReg<T>(fp, i.c));
}

template <class T>
inline void opLt(const Instr& i, uint64_t* fp) {
	writeReg<bool>(fp, i.a, readReg<T>(fp, i.b) < readReg<T>(fp, i.c));
}

template <class T>
inline void opLe(const Instr& i, uint64_t* fp) {
	writeReg<bool>(fp, i.a, readReg<T>(fp, i.b) <= readReg<T>(fp, i.c));
}

template <class T>
inline void opGt(const Instr& i, uint64_t* fp) {
	writeReg<bool>(fp, i.a, readReg<T>(fp, i.b) > readReg<T>(fp, i.c));
}

template <class T>
inline void opGe(const Instr& i, uint64_t* fp) {
	writeReg<bool>(fp, i.a, readReg<T>(fp, i.b) >= readReg<T>(fp, i.c));
}

inline void opAnd(const Instr& i, uint64_t* fp) {
	writeReg<bool>(fp, i.a, readReg<bool>(fp, i.b) && readReg<bool>(fp, i.c));
}

inline void opOr(const Instr& i, uint64_t* fp) {
	writeReg<bool>(fp, i.a, readReg<bool>(fp, i.b) || readReg<bool>(fp, i.c));
}

inline void opNot(const Instr& i, uint64_t* fp) {
	writeReg<bool>(fp, i.a, !readReg<bool>(fp, i.b));
}

template <class T>
inline void opLoad(const Instr& i, uint64_t* fp) {
	T value;
	std::memcpy(&value, reinterpret_cast<const void*>(fp[i.b]), sizeof(T));
	writeReg<T>(fp, i.a, value);
}

template <class T>
inline void opStore(const Instr& i, uint64_t* fp) {
	T value = readReg<T>(fp, i.b);
	std::memcpy(reinterpret_cast<void*>(fp[i.a]), &value, sizeof(T));
}

template <class T>
inline void opBand(const Instr& i, uint64_t* fp) {
	writeReg<T>(fp, i.a, static_cast<T>(readReg<T>(fp, i.b) & readReg<T>(fp, i.c)));
}

template <class T>
inline void opBor(const Instr& i, uint64_t* fp) {
	writeReg<T>(fp, i.a, static_cast<T>(readReg<T>(fp, i.b) | readReg<T>(fp, i.c)));
}

template <class T>
inline void opBxor(const Instr& i, uint64_t* fp) {
	writeReg<T>(fp, i.a, static_cast<T>(readReg<T>(fp, i.b) ^ readReg<T>(fp, i.c)));
}

template <class T>
inline void opShl(const Instr& i, uint64_t* fp) {
	writeReg<T>(fp, i.a, static_cast<T>(readReg<T>(fp, i.b) << readReg<T>(fp, i.c)));
}

template <class T>
inline void opShr(const Instr& i, uint64_t* fp) {
	writeReg<T>(fp, i.a, static_cast<T>(readReg<T>(fp, i.b) >> readReg<T>(fp, i.c)));
}

inline void opBnot(const Instr& i, uint64_t* fp) {
	writeReg<int64_t>(fp, i.a, ~readReg<int64_t>(fp, i.b));
}

template <class T>
inline void opNeg(const Instr& i, uint64_t* fp) {
	writeReg<T>(fp, i.a, -readReg<T>(fp, i.b));
}

template <class S, class D>
inline void opCast(const Instr& i, uint64_t* fp) {
	writeReg<D>(fp, i.a, static_cast<D>(readReg<S>(fp, i.b)));
}

} // namespace nautilus::compiler::tbc

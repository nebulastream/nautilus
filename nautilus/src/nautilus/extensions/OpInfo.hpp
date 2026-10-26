#pragma once

#include "nautilus/nir/Inst.hpp"
#include "nautilus/nir/Types.hpp"
#include <cstdint>
#include <functional>
#include <string>
#include <vector>

namespace nautilus::nir {
class Module;
}

namespace nautilus::ext {

/// What a field a, b or c of an extension instruction holds (13-nir-spec.md §5.11.1).
enum class FieldKind : uint8_t { Unused, Value, Const, Imm, List };
/// What a word of the `extra` payload holds.
enum class WordKind : uint8_t { Value, Const, Imm, Pointer, Type };

/// The operand shape: the core verifier, canonicalizer, loader, printer and parser use only this, never plugin code.
/// A `list` payload is `[n, w0 ... wn-1]` with `n = prefix.size() + k * group.size()` for some k >= 0 (k = 0 without a
/// group).
struct OperandShape {
	FieldKind a = FieldKind::Unused, b = FieldKind::Unused, c = FieldKind::Unused;
	std::vector<WordKind> prefix;
	std::vector<WordKind> group;
};

enum OpTrait : uint8_t { TraitPure = 1, TraitCommutative = 2, TraitIdempotent = 4 };
enum class EffectClass : uint8_t { None, Read, Write, ReadWrite };

/// Operand and result constraints, checked by the core verifier before the `verify` hook.
struct Signature {
	/// Number of value operands (`value` fields and `value` payload words, constants included).
	uint8_t operands = 0;
	/// The result type, or Void for none; `resultFromOperand` takes the type of that operand instead (255: unused).
	nir::TypeId result = nir::TypeId::Void;
	uint8_t resultFromOperand = 255;
};

/// An extension operation. Extension operations never throw and are never terminators (13 §5.11).
struct OpInfo {
	std::string name; // "simd.add"
	uint32_t version = 1;
	uint32_t minCompatibleVersion = 1;
	Signature sig;
	OperandShape shape;
	uint8_t traits = 0;
	EffectClass effects = EffectClass::None;
	/// Plugin check run after the core checks; returns an error message, empty if the instruction is valid.
	std::function<std::string(const nir::Module&, const nir::Inst&)> verify;
	bool portable = true;
	/// The fallback is mandatory: a native function in the pointer convention (04 §3.3), or an expansion applied by
	/// legalization. Target-restricted operations (targets != 0) may omit both.
	void* nativeFallback = nullptr;
	bool hasExpansion = false;
	/// Bit set of targets the operation is valid on; 0 means all.
	uint32_t targets = 0;
};

/// An intrinsic: a function a plugin registers by name, with a native fallback (13 §4.6.1). Its identity does not depend
/// on any backend; backend lowerings attach by name.
struct IntrinsicInfo {
	std::string name; // "builtin.prefetch"
	uint32_t version = 1;
	uint32_t minCompatibleVersion = 1;
	/// The NIR signature as type words (13 §4.5.1): the result first, then the parameters.
	std::vector<uint32_t> signature;
	/// Attribute bits (13 §4.6.2).
	uint32_t attrs = 0;
	/// The native function that defines the semantics.
	void* fallback = nullptr;
	/// Calls the fallback and captures an exception it throws; required unless the attributes have `noUnwind`.
	void* captureThunk = nullptr;
};

} // namespace nautilus::ext

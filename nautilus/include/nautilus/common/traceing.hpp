
#pragma once

#include "nautilus/common/TypedValueRef.hpp"
#include "nautilus/common/Types.hpp"
#include "nautilus/val_concepts.hpp"
#include <any>
#include <array>
#include <cstdint>
#include <iosfwd>
#include <vector>

namespace nautilus::tracing {

std::string getFunctionName(void* fnptr);

enum Op : uint8_t {
	JMP,
	CMP,
	RETURN,
	ASSIGN,
	CONST,
	CAST,
	FREE,
	CALL,
	// Memory
	LOAD,
	STORE,
	// Logical
	EQ,
	NEQ,
	LT,
	LTE,
	GT,
	GTE,
	NOT,
	AND,
	OR,
	// Arithmetic
	ADD,
	MUL,
	DIV,
	SUB,
	MOD,
	// Binary
	LSH,
	RSH,
	BOR,
	BXOR,
	BAND,
	NEGATE,
};

template <is_compatible_val_type T>
constexpr Type to_type();

/**
 * Returns the nautilus type for a C++ type.
 * @tparam T
 * @return Type
 */
template <is_compatible_val_type T>
constexpr Type to_type() {
	using type = std::remove_cvref_t<T>;
	if constexpr (std::is_same_v<type, bool>) {
		return Type::b;
	} else if constexpr (std::is_same_v<type, int8_t> || (std::is_same_v<type, char>) ) {
		return Type::i8;
	} else if constexpr (std::is_same_v<type, int16_t>) {
		return Type::i16;
	} else if constexpr (std::is_same_v<type, int32_t>) {
		return Type::i32;
	} else if constexpr (std::is_same_v<type, int64_t>) {
		return Type::i64;
	} else if constexpr (std::is_same_v<type, uint8_t>) {
		return Type::ui8;
	} else if constexpr (std::is_same_v<type, uint16_t>) {
		return Type::ui16;
	} else if constexpr (std::is_same_v<type, uint32_t>) {
		return Type::ui32;
	} else if constexpr (std::is_same_v<type, uint64_t> || std::is_same_v<type, size_t>) {
		return Type::ui64;
	} else if constexpr (std::is_same_v<type, float>) {
		return Type::f32;
	} else if constexpr (std::is_same_v<type, double>) {
		return Type::f64;
	} else if constexpr (std::is_pointer_v<type>) {
		return Type::ptr;
	} else {
		return Type::v;
	}
}

[[maybe_unused, nodiscard]] bool inTracer();

[[maybe_unused]] value_ref traceBinaryOp(Op operation, Type resultType, value_ref leftState, value_ref rightState);

template <Op op, typename T>
[[maybe_unused]] value_ref traceBinaryOp(value_ref leftState, value_ref rightState) {
	auto type = to_type<T>();
	return traceBinaryOp(op, type, leftState, rightState);
}

template <Op op, typename T>
[[maybe_unused]] value_ref traceUnaryOp(value_ref inputState);

[[maybe_unused]] bool traceBool(value_ref state);

[[maybe_unused]] void traceStore(value_ref target, value_ref src, Type valueType);

[[maybe_unused]] value_ref traceLoad(value_ref src, Type resultType);

[[maybe_unused]] value_ref traceCopy(value_ref state);

value_ref registerFunctionArgument(Type type, size_t index);

void traceAssignment(value_ref target, value_ref source, Type resultType);

value_ref traceConstant(Type type, std::any&& value);

template <typename T>
value_ref traceConstant(T value) {
	if (!inTracer())
		return {0, to_type<T>()};
	return traceConstant(to_type<T>(), std::any(value));
}

void traceReturnOperation(Type type, value_ref ref);

void traceValueDestruction(value_ref ref);

value_ref traceCast(value_ref state, Type resultType);

value_ref traceCall(void* fptn, Type resultType, const std::vector<tracing::value_ref>& arguments);

std::ostream& operator<<(std::ostream& os, const Op& operation);

void pushStaticVal(void* ptr);
void popStaticVal();
void allocateValRef(ValueRef ref);
void freeValRef(ValueRef ref);
} // namespace nautilus::tracing

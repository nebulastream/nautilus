
#include "ConstantTracker.hpp"
#include <limits>
#include <type_traits>

namespace nautilus::tracing {

ConstantTracker::Entry& ConstantTracker::ensure(ValueRef ref) {
	if (entries.size() <= ref) {
		entries.resize(static_cast<size_t>(ref) + 1);
	}
	return entries[ref];
}

void ConstantTracker::setConst(ValueRef ref, const ConstantLiteral& literal) {
	auto& entry = ensure(ref);
	// Runtime is a final state - never promote back to Const.
	if (entry.state == State::Runtime) {
		return;
	}
	entry.state = State::Const;
	entry.literal = literal;
}

void ConstantTracker::setRuntime(ValueRef ref) {
	ensure(ref).state = State::Runtime;
}

std::optional<ConstantLiteral> ConstantTracker::get(ValueRef ref) const {
	if (ref < entries.size() && entries[ref].state == State::Const) {
		return entries[ref].literal;
	}
	return std::nullopt;
}

std::optional<bool> ConstantTracker::tryFoldBool(ValueRef ref) const {
	if (auto literal = get(ref)) {
		if (const bool* b = std::get_if<bool>(&*literal)) {
			return *b;
		}
	}
	return std::nullopt;
}

void ConstantTracker::clear() {
	entries.clear();
}

namespace {

/// True if a folded value of C++ type T may be reported for a nautilus
/// resultType - i.e. the literal alternative matches the recorded type.
template <typename T>
bool matchesType(Type resultType) {
	return TypeResolver<T>::to_type() == resultType;
}

template <typename T>
std::optional<ConstantLiteral> foldBinaryTyped(Op op, Type resultType, T a, T b) {
	if constexpr (std::is_same_v<T, bool>) {
		if (resultType != Type::b) {
			return std::nullopt;
		}
		switch (op) {
		case AND:
			return ConstantLiteral(a && b);
		case OR:
			return ConstantLiteral(a || b);
		case EQ:
			return ConstantLiteral(a == b);
		case NEQ:
			return ConstantLiteral(a != b);
		default:
			return std::nullopt;
		}
	} else if constexpr (std::is_same_v<T, void*>) {
		if (resultType != Type::b) {
			return std::nullopt;
		}
		switch (op) {
		case EQ:
			return ConstantLiteral(a == b);
		case NEQ:
			return ConstantLiteral(a != b);
		default:
			return std::nullopt;
		}
	} else if constexpr (std::is_integral_v<T>) {
		using U = std::make_unsigned_t<T>;
		constexpr int bits = static_cast<int>(sizeof(T) * 8);
		switch (op) {
		case EQ:
		case NEQ:
		case LT:
		case LTE:
		case GT:
		case GTE: {
			if (resultType != Type::b) {
				return std::nullopt;
			}
			switch (op) {
			case EQ:
				return ConstantLiteral(a == b);
			case NEQ:
				return ConstantLiteral(a != b);
			case LT:
				return ConstantLiteral(a < b);
			case LTE:
				return ConstantLiteral(a <= b);
			case GT:
				return ConstantLiteral(a > b);
			default:
				return ConstantLiteral(a >= b);
			}
		}
		case ADD:
		case SUB:
		case MUL: {
			if (!matchesType<T>(resultType)) {
				return std::nullopt;
			}
			// Compute on the unsigned equivalent so a signed overflow wraps
			// (two's complement) instead of being UB in the tracer itself.
			U ua = static_cast<U>(a);
			U ub = static_cast<U>(b);
			U result = op == ADD ? static_cast<U>(ua + ub) : op == SUB ? static_cast<U>(ua - ub) : static_cast<U>(ua * ub);
			return ConstantLiteral(static_cast<T>(result));
		}
		case DIV:
		case MOD: {
			if (!matchesType<T>(resultType) || b == 0) {
				return std::nullopt;
			}
			if constexpr (std::is_signed_v<T>) {
				// INT_MIN / -1 overflows; never fold a trapping division.
				if (a == std::numeric_limits<T>::min() && b == static_cast<T>(-1)) {
					return std::nullopt;
				}
			}
			return ConstantLiteral(static_cast<T>(op == DIV ? a / b : a % b));
		}
		case BAND:
		case BOR:
		case BXOR: {
			if (!matchesType<T>(resultType)) {
				return std::nullopt;
			}
			U ua = static_cast<U>(a);
			U ub = static_cast<U>(b);
			U result = op == BAND ? static_cast<U>(ua & ub) : op == BOR ? static_cast<U>(ua | ub) : static_cast<U>(ua ^ ub);
			return ConstantLiteral(static_cast<T>(result));
		}
		case LSH:
		case RSH: {
			if (!matchesType<T>(resultType) || b < 0 || static_cast<int64_t>(b) >= bits) {
				return std::nullopt;
			}
			if (op == LSH) {
				return ConstantLiteral(static_cast<T>(static_cast<U>(a) << b));
			}
			return ConstantLiteral(static_cast<T>(a >> b));
		}
		default:
			return std::nullopt;
		}
	} else {
		// float / double
		switch (op) {
		case EQ:
		case NEQ:
		case LT:
		case LTE:
		case GT:
		case GTE: {
			if (resultType != Type::b) {
				return std::nullopt;
			}
			switch (op) {
			case EQ:
				return ConstantLiteral(a == b);
			case NEQ:
				return ConstantLiteral(a != b);
			case LT:
				return ConstantLiteral(a < b);
			case LTE:
				return ConstantLiteral(a <= b);
			case GT:
				return ConstantLiteral(a > b);
			default:
				return ConstantLiteral(a >= b);
			}
		}
		case ADD:
		case SUB:
		case MUL:
		case DIV: {
			if (!matchesType<T>(resultType)) {
				return std::nullopt;
			}
			switch (op) {
			case ADD:
				return ConstantLiteral(a + b);
			case SUB:
				return ConstantLiteral(a - b);
			case MUL:
				return ConstantLiteral(a * b);
			default:
				return ConstantLiteral(a / b);
			}
		}
		default:
			return std::nullopt;
		}
	}
}

/// Converts a literal to the C++ type behind the given nautilus type.
/// Floating-point to integer conversions are refused: an out-of-range or NaN
/// conversion is UB and the trace-time value must never differ from what the
/// compiled code would compute.
template <typename From>
std::optional<ConstantLiteral> castTyped(Type resultType, From value) {
	if constexpr (std::is_same_v<From, void*>) {
		return std::nullopt;
	} else {
		constexpr bool fromFloat = std::is_floating_point_v<From>;
		switch (resultType) {
		case Type::b:
			if constexpr (fromFloat) {
				return std::nullopt;
			} else {
				return ConstantLiteral(value != static_cast<From>(0));
			}
		case Type::i8:
			if constexpr (fromFloat) {
				return std::nullopt;
			} else {
				return ConstantLiteral(static_cast<int8_t>(value));
			}
		case Type::i16:
			if constexpr (fromFloat) {
				return std::nullopt;
			} else {
				return ConstantLiteral(static_cast<int16_t>(value));
			}
		case Type::i32:
			if constexpr (fromFloat) {
				return std::nullopt;
			} else {
				return ConstantLiteral(static_cast<int32_t>(value));
			}
		case Type::i64:
			if constexpr (fromFloat) {
				return std::nullopt;
			} else {
				return ConstantLiteral(static_cast<int64_t>(value));
			}
		case Type::ui8:
			if constexpr (fromFloat) {
				return std::nullopt;
			} else {
				return ConstantLiteral(static_cast<uint8_t>(value));
			}
		case Type::ui16:
			if constexpr (fromFloat) {
				return std::nullopt;
			} else {
				return ConstantLiteral(static_cast<uint16_t>(value));
			}
		case Type::ui32:
			if constexpr (fromFloat) {
				return std::nullopt;
			} else {
				return ConstantLiteral(static_cast<uint32_t>(value));
			}
		case Type::ui64:
			if constexpr (fromFloat) {
				return std::nullopt;
			} else {
				return ConstantLiteral(static_cast<uint64_t>(value));
			}
		case Type::f32:
			return ConstantLiteral(static_cast<float>(value));
		case Type::f64:
			return ConstantLiteral(static_cast<double>(value));
		default:
			return std::nullopt;
		}
	}
}

template <typename T>
std::optional<ConstantLiteral> foldUnaryTyped(Op op, Type resultType, T value) {
	switch (op) {
	case NOT:
		if constexpr (std::is_same_v<T, bool>) {
			if (resultType == Type::b) {
				return ConstantLiteral(!value);
			}
		}
		return std::nullopt;
	case NEGATE:
		// NEGATE is the bitwise complement (see val_arith.hpp neg()).
		if constexpr (std::is_integral_v<T> && !std::is_same_v<T, bool>) {
			if (matchesType<T>(resultType)) {
				using U = std::make_unsigned_t<T>;
				return ConstantLiteral(static_cast<T>(~static_cast<U>(value)));
			}
		}
		return std::nullopt;
	case CAST:
		return castTyped<T>(resultType, value);
	default:
		return std::nullopt;
	}
}

} // namespace

std::optional<ConstantLiteral> ConstantTracker::foldBinary(Op op, Type resultType, const ConstantLiteral& left,
                                                           const ConstantLiteral& right) {
	if (left.index() != right.index()) {
		return std::nullopt;
	}
	return std::visit(
	    [&](auto a) -> std::optional<ConstantLiteral> {
		    using T = decltype(a);
		    return foldBinaryTyped<T>(op, resultType, a, std::get<T>(right));
	    },
	    left);
}

std::optional<ConstantLiteral> ConstantTracker::foldUnary(Op op, Type resultType, const ConstantLiteral& input) {
	return std::visit([&](auto v) -> std::optional<ConstantLiteral> { return foldUnaryTyped(op, resultType, v); },
	                  input);
}

} // namespace nautilus::tracing


#pragma once

#include "nautilus/tracing/Operations.hpp"
#include "nautilus/tracing/TypedValueRef.hpp"
#include "nautilus/tracing/Types.hpp"
#include <optional>
#include <vector>

namespace nautilus::tracing {

/**
 * @brief Tracks which traced value refs are trace-time constants so that
 * branches on constant conditions can be pruned during tracing.
 *
 * Lattice per ValueRef: Unknown -> { Const(literal) | Runtime } -> Runtime,
 * monotone toward Runtime and never back. The map persists across exploration
 * iterations: refs are unique across the whole trace, every ref has a single
 * defining operation, and assignment targets (the only refs whose value can
 * differ per path) are always killed to Runtime - so a stale entry can only be
 * more conservative than the truth on the current path.
 */
class ConstantTracker {
public:
	void setConst(ValueRef ref, const ConstantLiteral& literal);
	void setRuntime(ValueRef ref);

	/// Returns the literal if the ref is a known trace-time constant.
	std::optional<ConstantLiteral> get(ValueRef ref) const;

	/// Returns the boolean value of the ref if it is a known constant bool.
	std::optional<bool> tryFoldBool(ValueRef ref) const;

	void clear();

	/// Folds a binary operation on two constant literals. Returns nullopt for
	/// unfoldable ops, mismatched literal types, results whose type does not
	/// match @p resultType, and folds that would trap or be UB (division by
	/// zero, signed-overflowing division, out-of-range shifts).
	static std::optional<ConstantLiteral> foldBinary(Op op, Type resultType, const ConstantLiteral& left,
	                                                 const ConstantLiteral& right);

	/// Folds a unary operation (NOT, NEGATE, CAST) on a constant literal.
	static std::optional<ConstantLiteral> foldUnary(Op op, Type resultType, const ConstantLiteral& input);

private:
	enum class State : uint8_t { Unknown, Runtime, Const };
	struct Entry {
		State state = State::Unknown;
		ConstantLiteral literal;
	};

	Entry& ensure(ValueRef ref);

	std::vector<Entry> entries;
};

} // namespace nautilus::tracing

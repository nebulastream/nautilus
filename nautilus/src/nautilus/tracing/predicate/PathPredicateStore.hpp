
#pragma once

#include "nautilus/tracing/Operations.hpp"
#include "nautilus/tracing/TypedValueRef.hpp"
#include <cstdint>
#include <optional>
#include <unordered_map>
#include <vector>

namespace nautilus::tracing {

/**
 * @brief Lightweight path-predicate store used by ScopedTraceContext to prune
 * branches whose outcome is implied by earlier branches on the same value.
 *
 * Each entry encodes a single comparison of a traced value against an integer
 * literal: `var <cmp_op> literal`.  Predicates are pushed when the symbolic
 * executor commits to a branch direction and popped when that branch is
 * abandoned.  `evaluate()` returns:
 *
 *   - `std::nullopt`  — the candidate is independent of the active predicates;
 *                       both arms must be explored.
 *   - `true`          — the candidate is entailed by the active predicates;
 *                       the false arm is dead.
 *   - `false`         — the candidate is contradicted by the active
 *                       predicates; the true arm is dead.
 *
 * Reasoning is restricted to single-variable equality / ordering comparisons
 * against integer literals.  This is enough to handle the
 * `pathExplosion_constraintBlind_*` family and other shapes where a later
 * branch tests the same variable against a literal already constrained by a
 * dominator.  More general SMT-style reasoning is intentionally out of scope.
 */
class PathPredicateStore {
public:
	struct Predicate {
		ValueRef var;
		Op cmpOp;     // EQ, NEQ, LT, LTE, GT, GTE
		int64_t lit;  // integer literal we are comparing against
		bool isValid; // false when we could not derive a literal predicate from the CMP op
	};

	/// Push a predicate onto the active stack.  Invalid predicates (isValid == false)
	/// are still pushed but never entail anything; this keeps push / pop balanced
	/// with branch enter / leave even when we cannot derive a useful predicate.
	void push(const Predicate& predicate);

	/// Pop the most recently pushed predicate.
	void pop();

	/// Drop every active predicate in one shot.  Cheaper than calling pop() in
	/// a loop because it clears the per-var index in a single sweep.  Used by
	/// the tracer's resume() to reset state between symbolic-execution
	/// iterations.
	void clear() noexcept;

	/// Number of active predicates.  Useful in tests.
	std::size_t size() const noexcept;

	/// Evaluates whether @p candidate is implied (true), contradicted (false),
	/// or independent (nullopt) by the currently active predicates.
	std::optional<bool> evaluate(const Predicate& candidate) const;

	/// Returns true if the candidate predicate is well-formed (was derived from
	/// a supported CMP op with an integer literal operand).
	static bool isWellFormed(const Predicate& p) noexcept {
		return p.isValid;
	}

private:
	// LIFO of predicates pushed along the current path.  Invalid predicates
	// are still stored here so push/pop stays balanced with the tracer's
	// branch-enter / branch-leave callbacks; they never produce verdicts.
	std::vector<Predicate> active_;

	// Inverted index: var -> indices into `active_` (only for valid entries).
	// Built incrementally in push() / pop() so evaluate() can walk only the
	// predicates that mention the same var as the candidate.  Without this
	// index, evaluate() is O(active_.size()); with it, it's O(predicates that
	// reference candidate.var).
	std::unordered_map<ValueRef, std::vector<std::size_t>> byVar_;
};

} // namespace nautilus::tracing

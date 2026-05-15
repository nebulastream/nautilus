
#include "nautilus/tracing/predicate/PathPredicateStore.hpp"

namespace nautilus::tracing {

namespace {

// Returns true if x <cmpOp> lit is satisfiable for at least one x that also satisfies
// active <cmpOp> activeLit.  Returns false if the candidate is contradicted by the active.
// The third returned value is true if the candidate is *entailed* by the active.
struct Implication {
	bool entailed;
	bool contradicted;
};

constexpr Implication implies(Op activeOp, int64_t activeLit, Op candOp, int64_t candLit) noexcept {
	switch (activeOp) {
	case EQ: {
		// x == activeLit fixes x.  Evaluate the candidate by direct substitution.
		switch (candOp) {
		case EQ:
			return {activeLit == candLit, activeLit != candLit};
		case NEQ:
			return {activeLit != candLit, activeLit == candLit};
		case LT:
			return {activeLit < candLit, !(activeLit < candLit)};
		case LTE:
			return {activeLit <= candLit, !(activeLit <= candLit)};
		case GT:
			return {activeLit > candLit, !(activeLit > candLit)};
		case GTE:
			return {activeLit >= candLit, !(activeLit >= candLit)};
		default:
			return {false, false};
		}
	}
	case NEQ: {
		// x != activeLit is too weak to entail / contradict most candidates,
		// except for the candidate (== activeLit) which it contradicts and
		// (!= activeLit) which it entails.
		if (candOp == EQ && candLit == activeLit) {
			return {false, true};
		}
		if (candOp == NEQ && candLit == activeLit) {
			return {true, false};
		}
		return {false, false};
	}
	case LT: {
		// x < activeLit
		switch (candOp) {
		case LT:
			return {candLit >= activeLit, false};
		case LTE:
			return {candLit >= activeLit - 1, false};
		case GTE:
			return {false, candLit >= activeLit};
		case GT:
			return {false, candLit >= activeLit - 1};
		case EQ:
			return {false, candLit >= activeLit};
		case NEQ:
			return {candLit >= activeLit, false};
		default:
			return {false, false};
		}
	}
	case LTE: {
		// x <= activeLit
		switch (candOp) {
		case LT:
			return {candLit > activeLit, false};
		case LTE:
			return {candLit >= activeLit, false};
		case GTE:
			return {false, candLit > activeLit};
		case GT:
			return {false, candLit >= activeLit};
		case EQ:
			return {false, candLit > activeLit};
		case NEQ:
			return {candLit > activeLit, false};
		default:
			return {false, false};
		}
	}
	case GT: {
		// x > activeLit
		switch (candOp) {
		case GT:
			return {candLit <= activeLit, false};
		case GTE:
			return {candLit <= activeLit + 1, false};
		case LTE:
			return {false, candLit <= activeLit};
		case LT:
			return {false, candLit <= activeLit + 1};
		case EQ:
			return {false, candLit <= activeLit};
		case NEQ:
			return {candLit <= activeLit, false};
		default:
			return {false, false};
		}
	}
	case GTE: {
		// x >= activeLit
		switch (candOp) {
		case GT:
			return {candLit < activeLit, false};
		case GTE:
			return {candLit <= activeLit, false};
		case LTE:
			return {false, candLit < activeLit};
		case LT:
			return {false, candLit <= activeLit};
		case EQ:
			return {false, candLit < activeLit};
		case NEQ:
			return {candLit < activeLit, false};
		default:
			return {false, false};
		}
	}
	default:
		return {false, false};
	}
}

} // namespace

void PathPredicateStore::push(const Predicate& predicate) {
	if (predicate.isValid) {
		// Only index valid predicates: invalid ones never produce verdicts
		// and only exist for push/pop balance.
		byVar_[predicate.var].push_back(active_.size());
	}
	active_.push_back(predicate);
}

void PathPredicateStore::pop() {
	if (active_.empty()) {
		return;
	}
	const auto& back = active_.back();
	if (back.isValid) {
		auto it = byVar_.find(back.var);
		if (it != byVar_.end()) {
			if (!it->second.empty()) {
				it->second.pop_back();
			}
			// Reclaim empty buckets so the map doesn't leak per-iteration.
			if (it->second.empty()) {
				byVar_.erase(it);
			}
		}
	}
	active_.pop_back();
}

void PathPredicateStore::clear() noexcept {
	active_.clear();
	byVar_.clear();
}

std::size_t PathPredicateStore::size() const noexcept {
	return active_.size();
}

std::optional<bool> PathPredicateStore::evaluate(const Predicate& candidate) const {
	if (!candidate.isValid) {
		return std::nullopt;
	}
	// Per-var index: skip directly to predicates that mention candidate.var.
	// O(predicates for this var) instead of O(total active predicates).
	auto bucketIt = byVar_.find(candidate.var);
	if (bucketIt == byVar_.end()) {
		return std::nullopt;
	}
	const auto& indices = bucketIt->second;
	// Walk most-recent-first so a tighter dominator overrides an older one.
	for (auto rit = indices.rbegin(); rit != indices.rend(); ++rit) {
		const auto& p = active_[*rit];
		auto [entailed, contradicted] = implies(p.cmpOp, p.lit, candidate.cmpOp, candidate.lit);
		if (entailed) {
			return true;
		}
		if (contradicted) {
			return false;
		}
	}
	return std::nullopt;
}

} // namespace nautilus::tracing

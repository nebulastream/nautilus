
#include "nautilus/tracing/Operations.hpp"
#include "nautilus/tracing/predicate/PathPredicateStore.hpp"
#include <catch2/catch_all.hpp>

using nautilus::tracing::Op;
using nautilus::tracing::PathPredicateStore;

namespace {

PathPredicateStore::Predicate make(uint32_t var, Op op, int64_t lit) {
	return {var, op, lit, true};
}

PathPredicateStore::Predicate invalid() {
	return {0, Op::EQ, 0, false};
}

} // namespace

TEST_CASE("PathPredicateStore: empty store is neutral", "[scoped-tracing]") {
	PathPredicateStore store;
	REQUIRE(store.size() == 0);
	REQUIRE_FALSE(store.evaluate(make(1, Op::EQ, 0)).has_value());
}

TEST_CASE("PathPredicateStore: x == c entails x == c, contradicts x == c'", "[scoped-tracing]") {
	PathPredicateStore store;
	store.push(make(1, Op::EQ, 5));
	REQUIRE(store.size() == 1);

	auto sameEq = store.evaluate(make(1, Op::EQ, 5));
	REQUIRE(sameEq.has_value());
	REQUIRE(*sameEq == true);

	auto differentEq = store.evaluate(make(1, Op::EQ, 6));
	REQUIRE(differentEq.has_value());
	REQUIRE(*differentEq == false);

	// Unrelated variable should not be affected.
	REQUIRE_FALSE(store.evaluate(make(2, Op::EQ, 5)).has_value());
}

TEST_CASE("PathPredicateStore: x == c entails ordering comparisons", "[scoped-tracing]") {
	PathPredicateStore store;
	store.push(make(1, Op::EQ, 5));

	// x == 5  =>  x < 10 is true,  x < 5 is false
	REQUIRE(*store.evaluate(make(1, Op::LT, 10)) == true);
	REQUIRE(*store.evaluate(make(1, Op::LT, 5)) == false);
	REQUIRE(*store.evaluate(make(1, Op::LTE, 5)) == true);
	REQUIRE(*store.evaluate(make(1, Op::GT, 4)) == true);
	REQUIRE(*store.evaluate(make(1, Op::GT, 5)) == false);
	REQUIRE(*store.evaluate(make(1, Op::GTE, 5)) == true);
}

TEST_CASE("PathPredicateStore: x < c entails / contradicts neighbouring constraints", "[scoped-tracing]") {
	PathPredicateStore store;
	store.push(make(1, Op::LT, 10));

	// x < 10  =>  x < 11 is implied (well, not strictly — entailment requires that the candidate is implied for ALL satisfying x)
	REQUIRE(*store.evaluate(make(1, Op::LT, 11)) == true);
	REQUIRE(*store.evaluate(make(1, Op::LT, 10)) == true);
	REQUIRE(*store.evaluate(make(1, Op::LTE, 9)) == true);

	// x < 10  =>  x >= 10  is contradicted
	REQUIRE(*store.evaluate(make(1, Op::GTE, 10)) == false);
	REQUIRE(*store.evaluate(make(1, Op::GTE, 11)) == false);

	// x < 10  is independent of  x < 5  (could be either way).
	REQUIRE_FALSE(store.evaluate(make(1, Op::LT, 5)).has_value());
}

TEST_CASE("PathPredicateStore: x != c is too weak for most ordering constraints", "[scoped-tracing]") {
	PathPredicateStore store;
	store.push(make(1, Op::NEQ, 5));

	// x != 5  =>  x == 5  is contradicted
	REQUIRE(*store.evaluate(make(1, Op::EQ, 5)) == false);

	// x != 5  =>  x != 5  is entailed
	REQUIRE(*store.evaluate(make(1, Op::NEQ, 5)) == true);

	// x != 5  is independent of  x < 10
	REQUIRE_FALSE(store.evaluate(make(1, Op::LT, 10)).has_value());
}

TEST_CASE("PathPredicateStore: invalid predicate is never used", "[scoped-tracing]") {
	PathPredicateStore store;
	store.push(invalid());
	REQUIRE(store.size() == 1);

	// Active is invalid -> never entails / contradicts.
	REQUIRE_FALSE(store.evaluate(make(1, Op::EQ, 5)).has_value());

	// Candidate is invalid -> never reasoned about.
	store.push(make(1, Op::EQ, 5));
	REQUIRE_FALSE(store.evaluate(invalid()).has_value());
}

TEST_CASE("PathPredicateStore: push / pop balances the active set", "[scoped-tracing]") {
	PathPredicateStore store;
	store.push(make(1, Op::EQ, 5));
	store.push(make(2, Op::LT, 100));
	REQUIRE(store.size() == 2);
	store.pop();
	REQUIRE(store.size() == 1);
	// After pop, only x == 5 is active; x < 100 no longer applies.
	REQUIRE_FALSE(store.evaluate(make(2, Op::LT, 100)).has_value());
	REQUIRE(*store.evaluate(make(1, Op::EQ, 5)) == true);
	store.pop();
	REQUIRE(store.size() == 0);
	// Extra pops are a no-op.
	store.pop();
	REQUIRE(store.size() == 0);
}

TEST_CASE("PathPredicateStore: most recent predicate wins on conflict", "[scoped-tracing]") {
	// Domain pushes (x == 1) then (x == 2). Logically inconsistent in
	// practice — the tracer would not push both along a single path — but
	// the store walks newest-first and reports the first reachable verdict.
	PathPredicateStore store;
	store.push(make(1, Op::EQ, 1));
	store.push(make(1, Op::EQ, 2));
	auto verdict = store.evaluate(make(1, Op::EQ, 2));
	REQUIRE(verdict.has_value());
	REQUIRE(*verdict == true);
}

#pragma once

#include <nautilus/Engine.hpp>

namespace nautilus::engine {

val<bool> boolNot(val<bool> x) {
	return !x;
}

val<bool> boolAnd(val<bool> a, val<bool> b) {
	return a && b;
}

val<bool> boolOr(val<bool> a, val<bool> b) {
	return a || b;
}

val<bool> boolAssignment(val<bool> x) {
	auto z = x;
	return z;
}

val<bool> boolAssignmentOr(val<bool> x, val<bool> y) {
	auto z = x;
	x = y;
	return z || y;
}

val<bool> boolConst(val<bool> x) {
	return x && true;
}

val<bool> boolEquals(val<bool> x, val<bool> z) {
	return x == z;
}

val<bool> boolNotEquals(val<bool> x, val<bool> z) {
	return x != z;
}

val<bool> boolIfElse(val<bool> x, val<bool> z) {
	if (x && z) {
		return true;
	} else {
		return false;
	}
}

val<bool> eval(val<bool> x, val<bool> z) {
	return x == z;
}

val<bool> boolNestedFunction(val<bool> x, val<bool> z) {
	if (eval(x, z)) {
		return true;
	} else {
		return false;
	}
}

template <typename T>
val<bool> operatorBool(val<T> x) {
	if (x) {
		return true;
	} else {
		return false;
	}
}

// Pointer null check in the explicit, symbolic form. `p != nullptr` stays a
// val<bool> data value feeding a single condition branch. val<T*> deliberately
// has no implicit `operator bool()`: that conversion materializes the null test
// into a separate control-flow branch which can be split from the dereference it
// guards, leaving an unchecked load that LLVM miscompiles at -O3.
val<bool> ptrIsNonNull(val<int32_t*> p) {
	if (p != nullptr) {
		return true;
	} else {
		return false;
	}
}

// Comparison operators with raw bool (testing NEW operators added to val_bool.hpp)
val<bool> boolEqualsMixed(val<bool> x, val<bool> y) {
	// Convert y to raw bool to test mixed operations
	bool rawY = y;
	return x == rawY;
}

val<bool> boolNotEqualsMixed(val<bool> x, val<bool> y) {
	// Convert x to raw bool to test mixed operations
	bool rawX = x;
	return rawX != y;
}

// Combined operations test using comparison and logical operators
val<bool> boolComplexOps(val<bool> a, val<bool> b, val<bool> c) {
	// Test: (a == b) && (b != c)
	auto eqResult = a == b;
	auto neqResult = b != c;
	return eqResult && neqResult;
}

// Test probability getter/setter
val<bool> boolProbabilityTest(val<bool> x) {
	x.setIsTrueProbability(0.9);
	// Just return the value to ensure probability doesn't affect logic
	return x;
}

// --- Regression tests for bc backend register-slot zero-extension ---
//
// These exercise the case where a `bool` value is produced by a runtime
// `invoke()` (a ProxyCall in the IR) and then returned from the JIT'd
// function. On the bc backend the bool value is written into a 64-bit
// register slot. If only the low byte is written, stale upper bytes from
// previous register use leak through the int64_t function-return path
// and flip a `false` result to `true`. To make any such bug observable,
// we first stash a non-zero pointer into the register file via another
// invoke before producing the final bool result.

inline bool returnsFalseRuntime() {
	return false;
}

inline bool returnsTrueRuntime() {
	return true;
}

inline void* returnsDirtyPointerRuntime() {
	static int sentinel = 0xCAFEBABE;
	return &sentinel;
}

inline void* returnsOtherDirtyPointerRuntime() {
	static long other[2] = {0x1234567, 0x89ABCDE};
	return &other;
}

inline bool comparePointersEqualRuntime(void* a, void* b) {
	return a == b;
}

// Bug repro 1: invoke a runtime function returning bool=false directly,
// after first dirtying register slots with a non-zero pointer return.
val<bool> invokeReturnsFalseAfterDirtyReg() {
	auto dirty = invoke(returnsDirtyPointerRuntime);
	(void) dirty;
	return invoke(returnsFalseRuntime);
}

// Bug repro 2: same shape but for true — guards the happy path.
val<bool> invokeReturnsTrueAfterDirtyReg() {
	auto dirty = invoke(returnsDirtyPointerRuntime);
	(void) dirty;
	return invoke(returnsTrueRuntime);
}

// Bug repro 3: bool produced by an invoke comparing two distinct
// non-null pointers — legitimately returns false, with the proxy-call
// return register having previously held non-zero pointer values.
val<bool> invokePointerCmpFalse() {
	auto a = invoke(returnsDirtyPointerRuntime);
	auto b = invoke(returnsOtherDirtyPointerRuntime);
	return invoke(comparePointersEqualRuntime, a, b);
}

// --- Narrow integer return regression tests ---
//
// Mirror the bool repro for every narrow integer width that goes
// through writeReg<T>() in the bc backend. The dirty-pointer invoke
// ensures the register slot reused for the narrow return previously
// held non-zero upper bytes, so any partial-width write would surface
// as a wrong value (especially for zero / negative values where the
// upper bits matter for sign- or zero-extension).

inline int8_t returnsI8(int8_t v) {
	return v;
}
inline int16_t returnsI16(int16_t v) {
	return v;
}
inline int32_t returnsI32(int32_t v) {
	return v;
}
inline int64_t returnsI64(int64_t v) {
	return v;
}
inline uint8_t returnsU8(uint8_t v) {
	return v;
}
inline uint16_t returnsU16(uint16_t v) {
	return v;
}
inline uint32_t returnsU32(uint32_t v) {
	return v;
}
inline uint64_t returnsU64(uint64_t v) {
	return v;
}

val<int8_t> invokeReturnsI8AfterDirtyReg(val<int8_t> v) {
	auto dirty = invoke(returnsDirtyPointerRuntime);
	(void) dirty;
	return invoke(returnsI8, v);
}
val<int16_t> invokeReturnsI16AfterDirtyReg(val<int16_t> v) {
	auto dirty = invoke(returnsDirtyPointerRuntime);
	(void) dirty;
	return invoke(returnsI16, v);
}
val<int32_t> invokeReturnsI32AfterDirtyReg(val<int32_t> v) {
	auto dirty = invoke(returnsDirtyPointerRuntime);
	(void) dirty;
	return invoke(returnsI32, v);
}
val<int64_t> invokeReturnsI64AfterDirtyReg(val<int64_t> v) {
	auto dirty = invoke(returnsDirtyPointerRuntime);
	(void) dirty;
	return invoke(returnsI64, v);
}
val<uint8_t> invokeReturnsU8AfterDirtyReg(val<uint8_t> v) {
	auto dirty = invoke(returnsDirtyPointerRuntime);
	(void) dirty;
	return invoke(returnsU8, v);
}
val<uint16_t> invokeReturnsU16AfterDirtyReg(val<uint16_t> v) {
	auto dirty = invoke(returnsDirtyPointerRuntime);
	(void) dirty;
	return invoke(returnsU16, v);
}
val<uint32_t> invokeReturnsU32AfterDirtyReg(val<uint32_t> v) {
	auto dirty = invoke(returnsDirtyPointerRuntime);
	(void) dirty;
	return invoke(returnsU32, v);
}
val<uint64_t> invokeReturnsU64AfterDirtyReg(val<uint64_t> v) {
	auto dirty = invoke(returnsDirtyPointerRuntime);
	(void) dirty;
	return invoke(returnsU64, v);
}

// Cast-after-invoke variants: write a narrow value via invoke, then
// cast it to int64. This exercises readReg<NarrowType> sitting on top
// of a slot whose upper bytes were dirtied by the previous proxy call.
val<int64_t> invokeI8ThenCastToI64(val<int8_t> v) {
	auto dirty = invoke(returnsDirtyPointerRuntime);
	(void) dirty;
	auto narrow = invoke(returnsI8, v);
	return static_cast<val<int64_t>>(narrow);
}
val<int64_t> invokeI16ThenCastToI64(val<int16_t> v) {
	auto dirty = invoke(returnsDirtyPointerRuntime);
	(void) dirty;
	auto narrow = invoke(returnsI16, v);
	return static_cast<val<int64_t>>(narrow);
}
val<int64_t> invokeI32ThenCastToI64(val<int32_t> v) {
	auto dirty = invoke(returnsDirtyPointerRuntime);
	(void) dirty;
	auto narrow = invoke(returnsI32, v);
	return static_cast<val<int64_t>>(narrow);
}
val<uint64_t> invokeU8ThenCastToU64(val<uint8_t> v) {
	auto dirty = invoke(returnsDirtyPointerRuntime);
	(void) dirty;
	auto narrow = invoke(returnsU8, v);
	return static_cast<val<uint64_t>>(narrow);
}
val<uint64_t> invokeU16ThenCastToU64(val<uint16_t> v) {
	auto dirty = invoke(returnsDirtyPointerRuntime);
	(void) dirty;
	auto narrow = invoke(returnsU16, v);
	return static_cast<val<uint64_t>>(narrow);
}
val<uint64_t> invokeU32ThenCastToU64(val<uint32_t> v) {
	auto dirty = invoke(returnsDirtyPointerRuntime);
	(void) dirty;
	auto narrow = invoke(returnsU32, v);
	return static_cast<val<uint64_t>>(narrow);
}

// --- Pointer null-guard loop regression -------------------------------------
//
// Walks a singly linked list, reassigning the cursor inside the loop. This is
// the shape that crashed in NebulaStream's chained-hash-map probe: the loop
// condition tests a pointer that is overwritten every iteration
// (`node = node->next`, which can become null). Written with the explicit
// `node != nullptr`, the null test stays a symbolic val<bool> feeding the loop
// condition, so the guard dominates every iteration's load. (The implicit
// pointer `operator bool()` was removed precisely because, combined with the
// eager &&/|| operators, it could split the per-iteration null guard from the
// load and let LLVM treat the dereference as unchecked at -O3.) This kernel must
// return the correct sum and never dereference a null cursor -- in BOTH bool
// configs and at the engine's full optimization level.
struct ListNode {
	int32_t value;
	ListNode* next;
};

inline int32_t listNodeValue(ListNode* n) {
	return n->value;
}

inline ListNode* listNodeNext(ListNode* n) {
	return n->next;
}

val<int32_t> sumLinkedList(val<ListNode*> head) {
	val<int32_t> sum = 0;
	val<ListNode*> node = head;
	while (node != nullptr) {
		sum += invoke(listNodeValue, node);
		node = invoke(listNodeNext, node);
	}
	return sum;
}

#ifdef ENABLE_SHORT_CIRCUIT_BOOL
// --- Short-circuit evaluation regression tests ---
//
// These exist to prove that under ENABLE_SHORT_CIRCUIT_BOOL the right-hand
// side of && / || really is not evaluated at runtime when the left-hand side
// determines the result. The shape of these tests is fragile by design: they
// would crash, divide by zero, dereference null, or leave a counter at the
// wrong value if the JIT lowered && / || to an eager binary op rather than a
// branch. They are gated on the cmake option because the same code is *not*
// safe to run under the eager-AND/OR build (the divide and the deref live
// directly in the trace, unguarded).

inline int64_t shortCircuitSideEffectCounter = 0;

inline bool incrementCounterAndReturnTrue() {
	shortCircuitSideEffectCounter++;
	return true;
}

inline bool incrementCounterAndReturnFalse() {
	shortCircuitSideEffectCounter++;
	return false;
}

inline int32_t divideTenBy(int32_t x) {
	return 10 / x;
}

inline int32_t loadInt(int32_t* p) {
	return *p;
}

// `false && rhs` must not invoke rhs. The counter stays at its pre-call
// value when `a` is false, and bumps by exactly one when `a` is true.
val<bool> shortCircuitAndRhsCounter(val<bool> a) {
	return a && invoke(incrementCounterAndReturnTrue);
}

// Symmetric case for ||: `true || rhs` must not invoke rhs.
val<bool> shortCircuitOrRhsCounter(val<bool> a) {
	return a || invoke(incrementCounterAndReturnFalse);
}

// Divide-by-zero guard. Without short-circuit semantics calling this with
// x == 0 would compute 10 / 0 in the trace and fault. With short-circuit
// the divide is nested under the `x != 0` branch.
val<int32_t> shortCircuitGuardDivByZero(val<int32_t> x) {
	if ((x != 0) && (invoke(divideTenBy, x) > 1)) {
		return invoke(divideTenBy, x);
	}
	return val<int32_t>(0);
}

// Null-pointer deref guard. Same idea: the load is only emitted on the path
// where `p != nullptr` is known to be true.
val<int32_t> shortCircuitGuardNullDeref(val<int32_t*> p) {
	if ((p != nullptr) && (invoke(loadInt, p) == 42)) {
		return val<int32_t>(1);
	}
	return val<int32_t>(0);
}
#endif

} // namespace nautilus::engine

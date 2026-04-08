#pragma once

#include <nautilus/Engine.hpp>
#include <nautilus/function.hpp>
#include <nautilus/inline.hpp>
#include <nautilus/val.hpp>
#include <nautilus/val_func.hpp>
#include <nautilus/val_ptr.hpp>

namespace nautilus::engine {

// ---------------------------------------------------------------------------
// Plain runtime helper functions used as callback targets in tests
// ---------------------------------------------------------------------------

NAUTILUS_INLINE int32_t addFn(int32_t x, int32_t y) {
	return x + y;
}

NAUTILUS_INLINE int32_t mulFn(int32_t x, int32_t y) {
	return x * y;
}

NAUTILUS_INLINE int32_t negFn(int32_t x) {
	return -x;
}

NAUTILUS_INLINE void voidFn(int32_t, int32_t) {
}

// A runtime function that calls a typed function pointer passed from traced code
NAUTILUS_INLINE int32_t applyBinaryFn(int32_t (*fn)(int32_t, int32_t), int32_t x, int32_t y) {
	return fn(x, y);
}

// A runtime function that accepts an opaque callback as void*
NAUTILUS_INLINE void registerCb(void* fn) {
	(void) fn;
}

// A runtime function that returns a function pointer
NAUTILUS_INLINE int32_t (*getAddFn())(int32_t, int32_t) {
	return addFn;
}

// ---------------------------------------------------------------------------
// Traced functions — use val<> parameters and call through fn-ptr values
// ---------------------------------------------------------------------------

/// Checks whether a function pointer is null.
val<bool> fnPtrIsNull(val<int32_t (*)(int32_t, int32_t)> fn) {
	return fn == nullptr;
}

/// Checks whether a function pointer is non-null.
val<bool> fnPtrNotNull(val<int32_t (*)(int32_t, int32_t)> fn) {
	return fn != nullptr;
}

/// Returns true when two identical function pointers compare equal.
val<bool> fnPtrEquals(val<int32_t (*)(int32_t, int32_t)> a, val<int32_t (*)(int32_t, int32_t)> b) {
	return a == b;
}

/// Returns true when two different function pointers compare not-equal.
val<bool> fnPtrNotEquals(val<int32_t (*)(int32_t, int32_t)> a, val<int32_t (*)(int32_t, int32_t)> b) {
	return a != b;
}

/// Calls through a typed function pointer and returns the result.
val<int32_t> callThroughFnPtr(val<int32_t (*)(int32_t, int32_t)> fn, val<int32_t> x, val<int32_t> y) {
	return fn(x, y);
}

/// Calls through a void function pointer (no return value).
val<int32_t> callVoidFnPtr(val<void (*)(int32_t, int32_t)> fn, val<int32_t> x, val<int32_t> y) {
	fn(x, y);
	return x;
}

/// Converts a typed fn-ptr val to val<void*> and passes it to a runtime function.
val<int32_t> fnPtrToVoidPtr(val<int32_t (*)(int32_t, int32_t)> fn, val<int32_t> x) {
	val<void*> vp = fn;
	invoke(registerCb, vp);
	return x;
}

/// Passes a val<FuncPtr> to a runtime function that takes the exact typed fn-ptr.
val<int32_t> fnPtrAsTypedArg(val<int32_t (*)(int32_t, int32_t)> fn, val<int32_t> x, val<int32_t> y) {
	return invoke(applyBinaryFn, fn, x, y);
}

/// Copies a function pointer and calls through the copy.
val<int32_t> fnPtrCopyAndCall(val<int32_t (*)(int32_t, int32_t)> fn, val<int32_t> x, val<int32_t> y) {
	val<int32_t (*)(int32_t, int32_t)> copy = fn;
	return copy(x, y);
}

/// Assigns a new function pointer, then calls through it.
val<int32_t> fnPtrAssignAndCall(val<int32_t (*)(int32_t, int32_t)> fn, val<int32_t (*)(int32_t, int32_t)> fn2,
                                val<int32_t> x, val<int32_t> y) {
	fn = fn2;
	return fn(x, y);
}

/// Selects one of two function pointers based on a flag, then calls it.
val<int32_t> selectFnPtr(val<bool> useAdd, val<int32_t> x, val<int32_t> y) {
	val<int32_t (*)(int32_t, int32_t)> fn = addFn;
	if (useAdd) {
		fn = addFn;
	} else {
		fn = mulFn;
	}
	return fn(x, y);
}

/// Calls through a function pointer in a counted loop; accumulates the results.
val<int32_t> fnPtrInLoop(val<int32_t (*)(int32_t, int32_t)> fn, val<int32_t> x, val<int32_t> n) {
	val<int32_t> sum = 0;
	for (val<int32_t> i = 0; i < n; i = i + 1) {
		sum = fn(sum, x);
	}
	return sum;
}

/// Returns 0 when the fn-ptr is null; otherwise calls through it.
val<int32_t> fnPtrNullBranch(val<int32_t (*)(int32_t, int32_t)> fn, val<int32_t> x, val<int32_t> y) {
	if (fn == nullptr) {
		return val<int32_t>(0);
	}
	return fn(x, y);
}

/// Wraps a non-capturing lambda via make_fn_val and calls through it.
val<int32_t> fnPtrFromLambda(val<int32_t> x) {
	auto fn = make_fn_val(+[](int32_t v) { return v * 3; });
	return fn(x);
}

/// Converts a fn-ptr to void* then back to the typed fn-ptr, then calls it.
val<int32_t> fnPtrRoundtripVoidPtr(val<int32_t (*)(int32_t, int32_t)> fn, val<int32_t> x, val<int32_t> y) {
	val<void*> vp = fn;
	val<int32_t (*)(int32_t, int32_t)> recovered = static_cast<val<int32_t (*)(int32_t, int32_t)>>(vp);
	return recovered(x, y);
}

/// Creates a val<fn-ptr> from an inline constant (addFn) and calls through it.
val<int32_t> fnPtrInlineConst(val<int32_t> x, val<int32_t> y) {
	val<int32_t (*)(int32_t, int32_t)> fn = addFn;
	return fn(x, y);
}

/// Creates a val<fn-ptr> from an inline constant (negFn, unary) and calls through it.
val<int32_t> fnPtrInlineConstUnary(val<int32_t> x) {
	val<int32_t (*)(int32_t)> fn = negFn;
	return fn(x);
}

} // namespace nautilus::engine

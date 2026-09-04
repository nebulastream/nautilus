#pragma once

#include "nautilus/function.hpp"
#include "nautilus/val.hpp"
#include "nautilus/val_std.hpp"
#include <stdexcept>

// ---------------------------------------------------------------------------
// Exception-handling test functions shared by the trace, execution and LLVM IR
// suites. They exercise the traced exception paths: a potentially-throwing
// call with and without live destructor-bearing val<T> objects, reverse-order
// cleanup, and noexcept calls that stay on the direct path.
// ---------------------------------------------------------------------------

namespace nautilus::testing {

struct GoldenExceptionCleanup {
	~GoldenExceptionCleanup() noexcept {
	}
};

void goldenWriteCleanup(GoldenExceptionCleanup*, int32_t) noexcept {
}

void goldenThrowWithoutCleanup() {
	throw std::runtime_error("golden exception without cleanup");
}

void goldenThrowWithCleanup(GoldenExceptionCleanup*) {
	throw std::runtime_error("golden exception with cleanup");
}

// Pad-less potentially-throwing call: nothing to run on unwind.
val<int32_t> exceptionCallWithoutCleanup() {
	invoke(goldenThrowWithoutCleanup);
	return 42;
}

// One live destructor-bearing val<T>: the call needs a landing pad.
val<int32_t> exceptionCallWithCleanup() {
	val<GoldenExceptionCleanup> cleanup;
	invoke(goldenThrowWithCleanup, &cleanup);
	return 42;
}

// Two live objects: destructors must run in reverse construction order.
val<int32_t> exceptionCallWithTwoCleanups() {
	val<GoldenExceptionCleanup> first;
	invoke(goldenWriteCleanup, &first, val<int32_t> {1});
	val<GoldenExceptionCleanup> second;
	invoke(goldenWriteCleanup, &second, val<int32_t> {2});
	invoke(goldenThrowWithCleanup, &second);
	return 0;
}

// noexcept call with a live object: direct call, no exception machinery; the
// object is destroyed on the normal path.
val<int32_t> exceptionCallNoexceptWithCleanup() {
	val<GoldenExceptionCleanup> cleanup;
	invoke(goldenWriteCleanup, &cleanup, val<int32_t> {3});
	return 42;
}

} // namespace nautilus::testing

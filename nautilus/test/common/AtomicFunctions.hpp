#pragma once

#include <nautilus/Engine.hpp>
#include <nautilus/std/atomic.h>

namespace nautilus::engine {

// ============================================================================
// Atomic Load / Store
// ============================================================================

val<int32_t> atomicLoadI32(val<int32_t*> ptr) {
	return atomic_load(ptr);
}

void atomicStoreI32(val<int32_t*> ptr, val<int32_t> value) {
	atomic_store(ptr, value);
}

val<int64_t> atomicLoadI64(val<int64_t*> ptr) {
	return atomic_load(ptr);
}

void atomicStoreI64(val<int64_t*> ptr, val<int64_t> value) {
	atomic_store(ptr, value);
}

// ============================================================================
// Atomic Fetch-Add / Fetch-Sub
// ============================================================================

val<int32_t> atomicFetchAddI32(val<int32_t*> ptr, val<int32_t> value) {
	return atomic_fetch_add(ptr, value);
}

val<int64_t> atomicFetchAddI64(val<int64_t*> ptr, val<int64_t> value) {
	return atomic_fetch_add(ptr, value);
}

val<int32_t> atomicFetchSubI32(val<int32_t*> ptr, val<int32_t> value) {
	return atomic_fetch_sub(ptr, value);
}

// ============================================================================
// Atomic Exchange
// ============================================================================

val<int32_t> atomicExchangeI32(val<int32_t*> ptr, val<int32_t> desired) {
	return atomic_exchange(ptr, desired);
}

val<int64_t> atomicExchangeI64(val<int64_t*> ptr, val<int64_t> desired) {
	return atomic_exchange(ptr, desired);
}

// ============================================================================
// Atomic Compare-and-Exchange
// ============================================================================

val<bool> atomicCasI32(val<int32_t*> ptr, val<int32_t*> expected, val<int32_t> desired) {
	return atomic_compare_exchange(ptr, expected, desired);
}

val<bool> atomicCasI64(val<int64_t*> ptr, val<int64_t*> expected, val<int64_t> desired) {
	return atomic_compare_exchange(ptr, expected, desired);
}

// ============================================================================
// Composite: Atomic Add (load + add + store via fetch_add)
// ============================================================================

val<int32_t> atomicAccumulate(val<int32_t*> ptr, val<int32_t> count) {
	val<int32_t> old = atomic_fetch_add(ptr, count);
	return old + count;
}

} // namespace nautilus::engine

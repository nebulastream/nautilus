#pragma once

#include <nautilus/val_ptr.hpp>

namespace nautilus {

// ============================================================================
// Atomic Load
// ============================================================================

val<int32_t> atomic_load(val<int32_t*> ptr);
val<int64_t> atomic_load(val<int64_t*> ptr);
val<uint32_t> atomic_load(val<uint32_t*> ptr);
val<uint64_t> atomic_load(val<uint64_t*> ptr);

// ============================================================================
// Atomic Store
// ============================================================================

void atomic_store(val<int32_t*> ptr, val<int32_t> value);
void atomic_store(val<int64_t*> ptr, val<int64_t> value);
void atomic_store(val<uint32_t*> ptr, val<uint32_t> value);
void atomic_store(val<uint64_t*> ptr, val<uint64_t> value);

// ============================================================================
// Atomic Exchange
// ============================================================================

val<int32_t> atomic_exchange(val<int32_t*> ptr, val<int32_t> desired);
val<int64_t> atomic_exchange(val<int64_t*> ptr, val<int64_t> desired);
val<uint32_t> atomic_exchange(val<uint32_t*> ptr, val<uint32_t> desired);
val<uint64_t> atomic_exchange(val<uint64_t*> ptr, val<uint64_t> desired);

// ============================================================================
// Atomic Compare-and-Exchange
// Returns true if the exchange was performed (i.e., *ptr was equal to expected).
// On failure, *expected is updated with the current value of *ptr.
// ============================================================================

val<bool> atomic_compare_exchange(val<int32_t*> ptr, val<int32_t*> expected, val<int32_t> desired);
val<bool> atomic_compare_exchange(val<int64_t*> ptr, val<int64_t*> expected, val<int64_t> desired);
val<bool> atomic_compare_exchange(val<uint32_t*> ptr, val<uint32_t*> expected, val<uint32_t> desired);
val<bool> atomic_compare_exchange(val<uint64_t*> ptr, val<uint64_t*> expected, val<uint64_t> desired);

// ============================================================================
// Atomic Fetch-Add (returns the old value)
// ============================================================================

val<int32_t> atomic_fetch_add(val<int32_t*> ptr, val<int32_t> value);
val<int64_t> atomic_fetch_add(val<int64_t*> ptr, val<int64_t> value);
val<uint32_t> atomic_fetch_add(val<uint32_t*> ptr, val<uint32_t> value);
val<uint64_t> atomic_fetch_add(val<uint64_t*> ptr, val<uint64_t> value);

// ============================================================================
// Atomic Fetch-Sub (returns the old value)
// ============================================================================

val<int32_t> atomic_fetch_sub(val<int32_t*> ptr, val<int32_t> value);
val<int64_t> atomic_fetch_sub(val<int64_t*> ptr, val<int64_t> value);
val<uint32_t> atomic_fetch_sub(val<uint32_t*> ptr, val<uint32_t> value);
val<uint64_t> atomic_fetch_sub(val<uint64_t*> ptr, val<uint64_t> value);

} // namespace nautilus

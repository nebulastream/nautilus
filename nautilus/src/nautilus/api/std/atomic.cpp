
#include <nautilus/function.hpp>
#include <nautilus/std/atomic.h>

namespace nautilus {

// ============================================================================
// Runtime Functions — Atomic Load (seq_cst)
// ============================================================================

static int32_t nautilus_atomic_load_i32(int32_t* ptr) {
	return __atomic_load_n(ptr, __ATOMIC_SEQ_CST);
}

static int64_t nautilus_atomic_load_i64(int64_t* ptr) {
	return __atomic_load_n(ptr, __ATOMIC_SEQ_CST);
}

static uint32_t nautilus_atomic_load_u32(uint32_t* ptr) {
	return __atomic_load_n(ptr, __ATOMIC_SEQ_CST);
}

static uint64_t nautilus_atomic_load_u64(uint64_t* ptr) {
	return __atomic_load_n(ptr, __ATOMIC_SEQ_CST);
}

// ============================================================================
// Runtime Functions — Atomic Store (seq_cst)
// ============================================================================

static void nautilus_atomic_store_i32(int32_t* ptr, int32_t value) {
	__atomic_store_n(ptr, value, __ATOMIC_SEQ_CST);
}

static void nautilus_atomic_store_i64(int64_t* ptr, int64_t value) {
	__atomic_store_n(ptr, value, __ATOMIC_SEQ_CST);
}

static void nautilus_atomic_store_u32(uint32_t* ptr, uint32_t value) {
	__atomic_store_n(ptr, value, __ATOMIC_SEQ_CST);
}

static void nautilus_atomic_store_u64(uint64_t* ptr, uint64_t value) {
	__atomic_store_n(ptr, value, __ATOMIC_SEQ_CST);
}

// ============================================================================
// Runtime Functions — Atomic Exchange (seq_cst)
// ============================================================================

static int32_t nautilus_atomic_exchange_i32(int32_t* ptr, int32_t desired) {
	return __atomic_exchange_n(ptr, desired, __ATOMIC_SEQ_CST);
}

static int64_t nautilus_atomic_exchange_i64(int64_t* ptr, int64_t desired) {
	return __atomic_exchange_n(ptr, desired, __ATOMIC_SEQ_CST);
}

static uint32_t nautilus_atomic_exchange_u32(uint32_t* ptr, uint32_t desired) {
	return __atomic_exchange_n(ptr, desired, __ATOMIC_SEQ_CST);
}

static uint64_t nautilus_atomic_exchange_u64(uint64_t* ptr, uint64_t desired) {
	return __atomic_exchange_n(ptr, desired, __ATOMIC_SEQ_CST);
}

// ============================================================================
// Runtime Functions — Atomic Compare-and-Exchange (seq_cst)
// ============================================================================

static bool nautilus_atomic_cas_i32(int32_t* ptr, int32_t* expected, int32_t desired) {
	return __atomic_compare_exchange_n(ptr, expected, desired, false, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST);
}

static bool nautilus_atomic_cas_i64(int64_t* ptr, int64_t* expected, int64_t desired) {
	return __atomic_compare_exchange_n(ptr, expected, desired, false, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST);
}

static bool nautilus_atomic_cas_u32(uint32_t* ptr, uint32_t* expected, uint32_t desired) {
	return __atomic_compare_exchange_n(ptr, expected, desired, false, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST);
}

static bool nautilus_atomic_cas_u64(uint64_t* ptr, uint64_t* expected, uint64_t desired) {
	return __atomic_compare_exchange_n(ptr, expected, desired, false, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST);
}

// ============================================================================
// Runtime Functions — Atomic Fetch-Add (seq_cst)
// ============================================================================

static int32_t nautilus_atomic_fetch_add_i32(int32_t* ptr, int32_t value) {
	return __atomic_fetch_add(ptr, value, __ATOMIC_SEQ_CST);
}

static int64_t nautilus_atomic_fetch_add_i64(int64_t* ptr, int64_t value) {
	return __atomic_fetch_add(ptr, value, __ATOMIC_SEQ_CST);
}

static uint32_t nautilus_atomic_fetch_add_u32(uint32_t* ptr, uint32_t value) {
	return __atomic_fetch_add(ptr, value, __ATOMIC_SEQ_CST);
}

static uint64_t nautilus_atomic_fetch_add_u64(uint64_t* ptr, uint64_t value) {
	return __atomic_fetch_add(ptr, value, __ATOMIC_SEQ_CST);
}

// ============================================================================
// Runtime Functions — Atomic Fetch-Sub (seq_cst)
// ============================================================================

static int32_t nautilus_atomic_fetch_sub_i32(int32_t* ptr, int32_t value) {
	return __atomic_fetch_sub(ptr, value, __ATOMIC_SEQ_CST);
}

static int64_t nautilus_atomic_fetch_sub_i64(int64_t* ptr, int64_t value) {
	return __atomic_fetch_sub(ptr, value, __ATOMIC_SEQ_CST);
}

static uint32_t nautilus_atomic_fetch_sub_u32(uint32_t* ptr, uint32_t value) {
	return __atomic_fetch_sub(ptr, value, __ATOMIC_SEQ_CST);
}

static uint64_t nautilus_atomic_fetch_sub_u64(uint64_t* ptr, uint64_t value) {
	return __atomic_fetch_sub(ptr, value, __ATOMIC_SEQ_CST);
}

// ============================================================================
// Public API — invoke() wrappers
// ============================================================================

// Atomic Load
val<int32_t> atomic_load(val<int32_t*> ptr) {
	return invoke(nautilus_atomic_load_i32, ptr);
}
val<int64_t> atomic_load(val<int64_t*> ptr) {
	return invoke(nautilus_atomic_load_i64, ptr);
}
val<uint32_t> atomic_load(val<uint32_t*> ptr) {
	return invoke(nautilus_atomic_load_u32, ptr);
}
val<uint64_t> atomic_load(val<uint64_t*> ptr) {
	return invoke(nautilus_atomic_load_u64, ptr);
}

// Atomic Store
void atomic_store(val<int32_t*> ptr, val<int32_t> value) {
	invoke(nautilus_atomic_store_i32, ptr, value);
}
void atomic_store(val<int64_t*> ptr, val<int64_t> value) {
	invoke(nautilus_atomic_store_i64, ptr, value);
}
void atomic_store(val<uint32_t*> ptr, val<uint32_t> value) {
	invoke(nautilus_atomic_store_u32, ptr, value);
}
void atomic_store(val<uint64_t*> ptr, val<uint64_t> value) {
	invoke(nautilus_atomic_store_u64, ptr, value);
}

// Atomic Exchange
val<int32_t> atomic_exchange(val<int32_t*> ptr, val<int32_t> desired) {
	return invoke(nautilus_atomic_exchange_i32, ptr, desired);
}
val<int64_t> atomic_exchange(val<int64_t*> ptr, val<int64_t> desired) {
	return invoke(nautilus_atomic_exchange_i64, ptr, desired);
}
val<uint32_t> atomic_exchange(val<uint32_t*> ptr, val<uint32_t> desired) {
	return invoke(nautilus_atomic_exchange_u32, ptr, desired);
}
val<uint64_t> atomic_exchange(val<uint64_t*> ptr, val<uint64_t> desired) {
	return invoke(nautilus_atomic_exchange_u64, ptr, desired);
}

// Atomic Compare-and-Exchange
val<bool> atomic_compare_exchange(val<int32_t*> ptr, val<int32_t*> expected, val<int32_t> desired) {
	return invoke(nautilus_atomic_cas_i32, ptr, expected, desired);
}
val<bool> atomic_compare_exchange(val<int64_t*> ptr, val<int64_t*> expected, val<int64_t> desired) {
	return invoke(nautilus_atomic_cas_i64, ptr, expected, desired);
}
val<bool> atomic_compare_exchange(val<uint32_t*> ptr, val<uint32_t*> expected, val<uint32_t> desired) {
	return invoke(nautilus_atomic_cas_u32, ptr, expected, desired);
}
val<bool> atomic_compare_exchange(val<uint64_t*> ptr, val<uint64_t*> expected, val<uint64_t> desired) {
	return invoke(nautilus_atomic_cas_u64, ptr, expected, desired);
}

// Atomic Fetch-Add
val<int32_t> atomic_fetch_add(val<int32_t*> ptr, val<int32_t> value) {
	return invoke(nautilus_atomic_fetch_add_i32, ptr, value);
}
val<int64_t> atomic_fetch_add(val<int64_t*> ptr, val<int64_t> value) {
	return invoke(nautilus_atomic_fetch_add_i64, ptr, value);
}
val<uint32_t> atomic_fetch_add(val<uint32_t*> ptr, val<uint32_t> value) {
	return invoke(nautilus_atomic_fetch_add_u32, ptr, value);
}
val<uint64_t> atomic_fetch_add(val<uint64_t*> ptr, val<uint64_t> value) {
	return invoke(nautilus_atomic_fetch_add_u64, ptr, value);
}

// Atomic Fetch-Sub
val<int32_t> atomic_fetch_sub(val<int32_t*> ptr, val<int32_t> value) {
	return invoke(nautilus_atomic_fetch_sub_i32, ptr, value);
}
val<int64_t> atomic_fetch_sub(val<int64_t*> ptr, val<int64_t> value) {
	return invoke(nautilus_atomic_fetch_sub_i64, ptr, value);
}
val<uint32_t> atomic_fetch_sub(val<uint32_t*> ptr, val<uint32_t> value) {
	return invoke(nautilus_atomic_fetch_sub_u32, ptr, value);
}
val<uint64_t> atomic_fetch_sub(val<uint64_t*> ptr, val<uint64_t> value) {
	return invoke(nautilus_atomic_fetch_sub_u64, ptr, value);
}

// ============================================================================
// Function pointer accessors for intrinsic registration
// ============================================================================

void* getAtomicLoadI32Ptr() {
	return reinterpret_cast<void*>(nautilus_atomic_load_i32);
}
void* getAtomicLoadI64Ptr() {
	return reinterpret_cast<void*>(nautilus_atomic_load_i64);
}
void* getAtomicLoadU32Ptr() {
	return reinterpret_cast<void*>(nautilus_atomic_load_u32);
}
void* getAtomicLoadU64Ptr() {
	return reinterpret_cast<void*>(nautilus_atomic_load_u64);
}
void* getAtomicStoreI32Ptr() {
	return reinterpret_cast<void*>(nautilus_atomic_store_i32);
}
void* getAtomicStoreI64Ptr() {
	return reinterpret_cast<void*>(nautilus_atomic_store_i64);
}
void* getAtomicStoreU32Ptr() {
	return reinterpret_cast<void*>(nautilus_atomic_store_u32);
}
void* getAtomicStoreU64Ptr() {
	return reinterpret_cast<void*>(nautilus_atomic_store_u64);
}
void* getAtomicExchangeI32Ptr() {
	return reinterpret_cast<void*>(nautilus_atomic_exchange_i32);
}
void* getAtomicExchangeI64Ptr() {
	return reinterpret_cast<void*>(nautilus_atomic_exchange_i64);
}
void* getAtomicExchangeU32Ptr() {
	return reinterpret_cast<void*>(nautilus_atomic_exchange_u32);
}
void* getAtomicExchangeU64Ptr() {
	return reinterpret_cast<void*>(nautilus_atomic_exchange_u64);
}
void* getAtomicCasI32Ptr() {
	return reinterpret_cast<void*>(nautilus_atomic_cas_i32);
}
void* getAtomicCasI64Ptr() {
	return reinterpret_cast<void*>(nautilus_atomic_cas_i64);
}
void* getAtomicCasU32Ptr() {
	return reinterpret_cast<void*>(nautilus_atomic_cas_u32);
}
void* getAtomicCasU64Ptr() {
	return reinterpret_cast<void*>(nautilus_atomic_cas_u64);
}
void* getAtomicFetchAddI32Ptr() {
	return reinterpret_cast<void*>(nautilus_atomic_fetch_add_i32);
}
void* getAtomicFetchAddI64Ptr() {
	return reinterpret_cast<void*>(nautilus_atomic_fetch_add_i64);
}
void* getAtomicFetchAddU32Ptr() {
	return reinterpret_cast<void*>(nautilus_atomic_fetch_add_u32);
}
void* getAtomicFetchAddU64Ptr() {
	return reinterpret_cast<void*>(nautilus_atomic_fetch_add_u64);
}
void* getAtomicFetchSubI32Ptr() {
	return reinterpret_cast<void*>(nautilus_atomic_fetch_sub_i32);
}
void* getAtomicFetchSubI64Ptr() {
	return reinterpret_cast<void*>(nautilus_atomic_fetch_sub_i64);
}
void* getAtomicFetchSubU32Ptr() {
	return reinterpret_cast<void*>(nautilus_atomic_fetch_sub_u32);
}
void* getAtomicFetchSubU64Ptr() {
	return reinterpret_cast<void*>(nautilus_atomic_fetch_sub_u64);
}

} // namespace nautilus

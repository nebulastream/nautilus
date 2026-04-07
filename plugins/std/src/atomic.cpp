#include <atomic>
#include <cstdint>
#include <nautilus/function.hpp>
#include <nautilus/std/atomic.h>

// =====================================================================================
// Named extern "C" thunks. The MLIR backend intrinsic registry (see
// MLIRAtomicIntrinsics.cpp) keys on the addresses of these thunks to lower the matching
// runtime call to a real LLVM atomic instruction. Outside of the MLIR backend (cpp/bc/
// asmjit/interpreter) the calls fall through to the standard library implementations.
// =====================================================================================

extern "C" {

#define NAUTILUS_ATOMIC_LOAD_THUNK(suffix, T, order_enum)                                                              \
	T nautilus_atomic_load_##T##_##suffix(std::atomic<T>* p) {                                                         \
		return p->load(order_enum);                                                                                    \
	}
#define NAUTILUS_ATOMIC_STORE_THUNK(suffix, T, order_enum)                                                             \
	void nautilus_atomic_store_##T##_##suffix(std::atomic<T>* p, T v) {                                                \
		p->store(v, order_enum);                                                                                       \
	}
#define NAUTILUS_ATOMIC_RMW_THUNK(op, suffix, T, order_enum)                                                           \
	T nautilus_atomic_##op##_##T##_##suffix(std::atomic<T>* p, T v) {                                                  \
		return p->op(v, order_enum);                                                                                   \
	}
#define NAUTILUS_ATOMIC_EXCHANGE_THUNK(suffix, T, order_enum)                                                          \
	T nautilus_atomic_exchange_##T##_##suffix(std::atomic<T>* p, T v) {                                                \
		return p->exchange(v, order_enum);                                                                             \
	}
#define NAUTILUS_ATOMIC_CMPXCHG_THUNK(T)                                                                               \
	bool nautilus_atomic_cmpxchg_strong_##T##_seqcst(std::atomic<T>* p, T* expected, T desired) {                      \
		return p->compare_exchange_strong(*expected, desired);                                                         \
	}

// Per-type expansion: load (seqcst, acquire, relaxed), store (seqcst, release, relaxed),
// exchange + fetch_{add,sub,and,or,xor} (seqcst, relaxed), cmpxchg (seqcst).
#define NAUTILUS_ATOMIC_PER_TYPE(T)                                                                                    \
	NAUTILUS_ATOMIC_LOAD_THUNK(seqcst, T, std::memory_order_seq_cst)                                                   \
	NAUTILUS_ATOMIC_LOAD_THUNK(acquire, T, std::memory_order_acquire)                                                  \
	NAUTILUS_ATOMIC_LOAD_THUNK(relaxed, T, std::memory_order_relaxed)                                                  \
	NAUTILUS_ATOMIC_STORE_THUNK(seqcst, T, std::memory_order_seq_cst)                                                  \
	NAUTILUS_ATOMIC_STORE_THUNK(release, T, std::memory_order_release)                                                 \
	NAUTILUS_ATOMIC_STORE_THUNK(relaxed, T, std::memory_order_relaxed)                                                 \
	NAUTILUS_ATOMIC_EXCHANGE_THUNK(seqcst, T, std::memory_order_seq_cst)                                               \
	NAUTILUS_ATOMIC_EXCHANGE_THUNK(relaxed, T, std::memory_order_relaxed)                                              \
	NAUTILUS_ATOMIC_RMW_THUNK(fetch_add, seqcst, T, std::memory_order_seq_cst)                                         \
	NAUTILUS_ATOMIC_RMW_THUNK(fetch_add, relaxed, T, std::memory_order_relaxed)                                        \
	NAUTILUS_ATOMIC_RMW_THUNK(fetch_sub, seqcst, T, std::memory_order_seq_cst)                                         \
	NAUTILUS_ATOMIC_RMW_THUNK(fetch_sub, relaxed, T, std::memory_order_relaxed)                                        \
	NAUTILUS_ATOMIC_RMW_THUNK(fetch_and, seqcst, T, std::memory_order_seq_cst)                                         \
	NAUTILUS_ATOMIC_RMW_THUNK(fetch_and, relaxed, T, std::memory_order_relaxed)                                        \
	NAUTILUS_ATOMIC_RMW_THUNK(fetch_or, seqcst, T, std::memory_order_seq_cst)                                          \
	NAUTILUS_ATOMIC_RMW_THUNK(fetch_or, relaxed, T, std::memory_order_relaxed)                                         \
	NAUTILUS_ATOMIC_RMW_THUNK(fetch_xor, seqcst, T, std::memory_order_seq_cst)                                         \
	NAUTILUS_ATOMIC_RMW_THUNK(fetch_xor, relaxed, T, std::memory_order_relaxed)                                        \
	NAUTILUS_ATOMIC_CMPXCHG_THUNK(T)

NAUTILUS_ATOMIC_PER_TYPE(int32_t)
NAUTILUS_ATOMIC_PER_TYPE(int64_t)
NAUTILUS_ATOMIC_PER_TYPE(uint32_t)
NAUTILUS_ATOMIC_PER_TYPE(uint64_t)

#undef NAUTILUS_ATOMIC_LOAD_THUNK
#undef NAUTILUS_ATOMIC_STORE_THUNK
#undef NAUTILUS_ATOMIC_RMW_THUNK
#undef NAUTILUS_ATOMIC_EXCHANGE_THUNK
#undef NAUTILUS_ATOMIC_CMPXCHG_THUNK
#undef NAUTILUS_ATOMIC_PER_TYPE

// =====================================================================================
// atomic_ref-based thunks: operate on a raw T* by reinterpreting the storage as an
// atomic via std::atomic_ref<T>. Generates the same LLVM atomic instructions as the
// std::atomic<T>* thunks above and is registered with the same MLIR lowerings.
// =====================================================================================
#define NAUTILUS_ATOMIC_REF_THUNKS(T)                                                                                  \
	T nautilus_atomic_ref_load_##T##_seqcst(T* p) {                                                                    \
		return std::atomic_ref<T>(*p).load(std::memory_order_seq_cst);                                                 \
	}                                                                                                                  \
	T nautilus_atomic_ref_load_##T##_acquire(T* p) {                                                                   \
		return std::atomic_ref<T>(*p).load(std::memory_order_acquire);                                                 \
	}                                                                                                                  \
	T nautilus_atomic_ref_load_##T##_relaxed(T* p) {                                                                   \
		return std::atomic_ref<T>(*p).load(std::memory_order_relaxed);                                                 \
	}                                                                                                                  \
	void nautilus_atomic_ref_store_##T##_seqcst(T* p, T v) {                                                           \
		std::atomic_ref<T>(*p).store(v, std::memory_order_seq_cst);                                                    \
	}                                                                                                                  \
	void nautilus_atomic_ref_store_##T##_release(T* p, T v) {                                                          \
		std::atomic_ref<T>(*p).store(v, std::memory_order_release);                                                    \
	}                                                                                                                  \
	void nautilus_atomic_ref_store_##T##_relaxed(T* p, T v) {                                                          \
		std::atomic_ref<T>(*p).store(v, std::memory_order_relaxed);                                                    \
	}                                                                                                                  \
	T nautilus_atomic_ref_exchange_##T##_seqcst(T* p, T v) {                                                           \
		return std::atomic_ref<T>(*p).exchange(v, std::memory_order_seq_cst);                                          \
	}                                                                                                                  \
	T nautilus_atomic_ref_exchange_##T##_relaxed(T* p, T v) {                                                          \
		return std::atomic_ref<T>(*p).exchange(v, std::memory_order_relaxed);                                          \
	}                                                                                                                  \
	T nautilus_atomic_ref_fetch_add_##T##_seqcst(T* p, T v) {                                                          \
		return std::atomic_ref<T>(*p).fetch_add(v, std::memory_order_seq_cst);                                         \
	}                                                                                                                  \
	T nautilus_atomic_ref_fetch_add_##T##_relaxed(T* p, T v) {                                                         \
		return std::atomic_ref<T>(*p).fetch_add(v, std::memory_order_relaxed);                                         \
	}                                                                                                                  \
	T nautilus_atomic_ref_fetch_sub_##T##_seqcst(T* p, T v) {                                                          \
		return std::atomic_ref<T>(*p).fetch_sub(v, std::memory_order_seq_cst);                                         \
	}                                                                                                                  \
	T nautilus_atomic_ref_fetch_sub_##T##_relaxed(T* p, T v) {                                                         \
		return std::atomic_ref<T>(*p).fetch_sub(v, std::memory_order_relaxed);                                         \
	}                                                                                                                  \
	T nautilus_atomic_ref_fetch_and_##T##_seqcst(T* p, T v) {                                                          \
		return std::atomic_ref<T>(*p).fetch_and(v, std::memory_order_seq_cst);                                         \
	}                                                                                                                  \
	T nautilus_atomic_ref_fetch_and_##T##_relaxed(T* p, T v) {                                                         \
		return std::atomic_ref<T>(*p).fetch_and(v, std::memory_order_relaxed);                                         \
	}                                                                                                                  \
	T nautilus_atomic_ref_fetch_or_##T##_seqcst(T* p, T v) {                                                           \
		return std::atomic_ref<T>(*p).fetch_or(v, std::memory_order_seq_cst);                                          \
	}                                                                                                                  \
	T nautilus_atomic_ref_fetch_or_##T##_relaxed(T* p, T v) {                                                          \
		return std::atomic_ref<T>(*p).fetch_or(v, std::memory_order_relaxed);                                          \
	}                                                                                                                  \
	T nautilus_atomic_ref_fetch_xor_##T##_seqcst(T* p, T v) {                                                          \
		return std::atomic_ref<T>(*p).fetch_xor(v, std::memory_order_seq_cst);                                         \
	}                                                                                                                  \
	T nautilus_atomic_ref_fetch_xor_##T##_relaxed(T* p, T v) {                                                         \
		return std::atomic_ref<T>(*p).fetch_xor(v, std::memory_order_relaxed);                                         \
	}                                                                                                                  \
	bool nautilus_atomic_ref_cmpxchg_strong_##T##_seqcst(T* p, T* expected, T desired) {                               \
		return std::atomic_ref<T>(*p).compare_exchange_strong(*expected, desired);                                     \
	}

NAUTILUS_ATOMIC_REF_THUNKS(int32_t)
NAUTILUS_ATOMIC_REF_THUNKS(int64_t)
NAUTILUS_ATOMIC_REF_THUNKS(uint32_t)
NAUTILUS_ATOMIC_REF_THUNKS(uint64_t)
#undef NAUTILUS_ATOMIC_REF_THUNKS

void nautilus_atomic_thread_fence_seqcst() {
	std::atomic_thread_fence(std::memory_order_seq_cst);
}
void nautilus_atomic_thread_fence_acquire() {
	std::atomic_thread_fence(std::memory_order_acquire);
}
void nautilus_atomic_thread_fence_release() {
	std::atomic_thread_fence(std::memory_order_release);
}
void nautilus_atomic_thread_fence_acq_rel() {
	std::atomic_thread_fence(std::memory_order_acq_rel);
}
void nautilus_atomic_thread_fence_relaxed() {
	std::atomic_thread_fence(std::memory_order_relaxed);
}

} // extern "C"

namespace nautilus {

// =====================================================================================
// invoke() wrappers — one per typed/ordered thunk above. Following the cstdlib.cpp style.
// =====================================================================================

#define NAUTILUS_DEF_LOAD(T, suffix)                                                                                   \
	val<T> atomic_load_##suffix(val<std::atomic<T>*> obj) {                                                            \
		return invoke<>(&nautilus_atomic_load_##T##_##suffix, obj);                                                    \
	}
#define NAUTILUS_DEF_STORE(T, suffix)                                                                                  \
	void atomic_store_##suffix(val<std::atomic<T>*> obj, val<T> v) {                                                   \
		invoke<>(&nautilus_atomic_store_##T##_##suffix, obj, v);                                                       \
	}
#define NAUTILUS_DEF_RMW(op, T, suffix)                                                                                \
	val<T> atomic_##op##_##suffix(val<std::atomic<T>*> obj, val<T> v) {                                                \
		return invoke<>(&nautilus_atomic_##op##_##T##_##suffix, obj, v);                                               \
	}
#define NAUTILUS_DEF_EXCHANGE(T, suffix)                                                                               \
	val<T> atomic_exchange_##suffix(val<std::atomic<T>*> obj, val<T> v) {                                              \
		return invoke<>(&nautilus_atomic_exchange_##T##_##suffix, obj, v);                                             \
	}
#define NAUTILUS_DEF_CMPXCHG(T)                                                                                        \
	val<bool> atomic_compare_exchange_strong(val<std::atomic<T>*> obj, val<T*> expected, val<T> desired) {             \
		return invoke<>(&nautilus_atomic_cmpxchg_strong_##T##_seqcst, obj, expected, desired);                         \
	}

#define NAUTILUS_DEF_PER_TYPE(T)                                                                                       \
	NAUTILUS_DEF_LOAD(T, seqcst)                                                                                       \
	NAUTILUS_DEF_LOAD(T, acquire)                                                                                      \
	NAUTILUS_DEF_LOAD(T, relaxed)                                                                                      \
	NAUTILUS_DEF_STORE(T, seqcst)                                                                                      \
	NAUTILUS_DEF_STORE(T, release)                                                                                     \
	NAUTILUS_DEF_STORE(T, relaxed)                                                                                     \
	NAUTILUS_DEF_EXCHANGE(T, seqcst)                                                                                   \
	NAUTILUS_DEF_EXCHANGE(T, relaxed)                                                                                  \
	NAUTILUS_DEF_RMW(fetch_add, T, seqcst)                                                                             \
	NAUTILUS_DEF_RMW(fetch_add, T, relaxed)                                                                            \
	NAUTILUS_DEF_RMW(fetch_sub, T, seqcst)                                                                             \
	NAUTILUS_DEF_RMW(fetch_sub, T, relaxed)                                                                            \
	NAUTILUS_DEF_RMW(fetch_and, T, seqcst)                                                                             \
	NAUTILUS_DEF_RMW(fetch_and, T, relaxed)                                                                            \
	NAUTILUS_DEF_RMW(fetch_or, T, seqcst)                                                                              \
	NAUTILUS_DEF_RMW(fetch_or, T, relaxed)                                                                             \
	NAUTILUS_DEF_RMW(fetch_xor, T, seqcst)                                                                             \
	NAUTILUS_DEF_RMW(fetch_xor, T, relaxed)                                                                            \
	NAUTILUS_DEF_CMPXCHG(T)

NAUTILUS_DEF_PER_TYPE(int32_t)
NAUTILUS_DEF_PER_TYPE(int64_t)
NAUTILUS_DEF_PER_TYPE(uint32_t)
NAUTILUS_DEF_PER_TYPE(uint64_t)

#undef NAUTILUS_DEF_LOAD
#undef NAUTILUS_DEF_STORE
#undef NAUTILUS_DEF_RMW
#undef NAUTILUS_DEF_EXCHANGE
#undef NAUTILUS_DEF_CMPXCHG
#undef NAUTILUS_DEF_PER_TYPE

// ---- atomic_ref invoke wrappers ----
#define NAUTILUS_DEF_REF(T)                                                                                            \
	val<T> atomic_ref_load_seqcst(val<T*> ptr) {                                                                       \
		return invoke<>(&nautilus_atomic_ref_load_##T##_seqcst, ptr);                                                  \
	}                                                                                                                  \
	val<T> atomic_ref_load_acquire(val<T*> ptr) {                                                                      \
		return invoke<>(&nautilus_atomic_ref_load_##T##_acquire, ptr);                                                 \
	}                                                                                                                  \
	val<T> atomic_ref_load_relaxed(val<T*> ptr) {                                                                      \
		return invoke<>(&nautilus_atomic_ref_load_##T##_relaxed, ptr);                                                 \
	}                                                                                                                  \
	void atomic_ref_store_seqcst(val<T*> ptr, val<T> v) {                                                              \
		invoke<>(&nautilus_atomic_ref_store_##T##_seqcst, ptr, v);                                                     \
	}                                                                                                                  \
	void atomic_ref_store_release(val<T*> ptr, val<T> v) {                                                             \
		invoke<>(&nautilus_atomic_ref_store_##T##_release, ptr, v);                                                    \
	}                                                                                                                  \
	void atomic_ref_store_relaxed(val<T*> ptr, val<T> v) {                                                             \
		invoke<>(&nautilus_atomic_ref_store_##T##_relaxed, ptr, v);                                                    \
	}                                                                                                                  \
	val<T> atomic_ref_exchange_seqcst(val<T*> ptr, val<T> v) {                                                         \
		return invoke<>(&nautilus_atomic_ref_exchange_##T##_seqcst, ptr, v);                                           \
	}                                                                                                                  \
	val<T> atomic_ref_exchange_relaxed(val<T*> ptr, val<T> v) {                                                        \
		return invoke<>(&nautilus_atomic_ref_exchange_##T##_relaxed, ptr, v);                                          \
	}                                                                                                                  \
	val<T> atomic_ref_fetch_add_seqcst(val<T*> ptr, val<T> v) {                                                        \
		return invoke<>(&nautilus_atomic_ref_fetch_add_##T##_seqcst, ptr, v);                                          \
	}                                                                                                                  \
	val<T> atomic_ref_fetch_add_relaxed(val<T*> ptr, val<T> v) {                                                       \
		return invoke<>(&nautilus_atomic_ref_fetch_add_##T##_relaxed, ptr, v);                                         \
	}                                                                                                                  \
	val<T> atomic_ref_fetch_sub_seqcst(val<T*> ptr, val<T> v) {                                                        \
		return invoke<>(&nautilus_atomic_ref_fetch_sub_##T##_seqcst, ptr, v);                                          \
	}                                                                                                                  \
	val<T> atomic_ref_fetch_sub_relaxed(val<T*> ptr, val<T> v) {                                                       \
		return invoke<>(&nautilus_atomic_ref_fetch_sub_##T##_relaxed, ptr, v);                                         \
	}                                                                                                                  \
	val<T> atomic_ref_fetch_and_seqcst(val<T*> ptr, val<T> v) {                                                        \
		return invoke<>(&nautilus_atomic_ref_fetch_and_##T##_seqcst, ptr, v);                                          \
	}                                                                                                                  \
	val<T> atomic_ref_fetch_and_relaxed(val<T*> ptr, val<T> v) {                                                       \
		return invoke<>(&nautilus_atomic_ref_fetch_and_##T##_relaxed, ptr, v);                                         \
	}                                                                                                                  \
	val<T> atomic_ref_fetch_or_seqcst(val<T*> ptr, val<T> v) {                                                         \
		return invoke<>(&nautilus_atomic_ref_fetch_or_##T##_seqcst, ptr, v);                                           \
	}                                                                                                                  \
	val<T> atomic_ref_fetch_or_relaxed(val<T*> ptr, val<T> v) {                                                        \
		return invoke<>(&nautilus_atomic_ref_fetch_or_##T##_relaxed, ptr, v);                                          \
	}                                                                                                                  \
	val<T> atomic_ref_fetch_xor_seqcst(val<T*> ptr, val<T> v) {                                                        \
		return invoke<>(&nautilus_atomic_ref_fetch_xor_##T##_seqcst, ptr, v);                                          \
	}                                                                                                                  \
	val<T> atomic_ref_fetch_xor_relaxed(val<T*> ptr, val<T> v) {                                                       \
		return invoke<>(&nautilus_atomic_ref_fetch_xor_##T##_relaxed, ptr, v);                                         \
	}                                                                                                                  \
	val<bool> atomic_ref_compare_exchange_strong(val<T*> ptr, val<T*> expected, val<T> desired) {                      \
		return invoke<>(&nautilus_atomic_ref_cmpxchg_strong_##T##_seqcst, ptr, expected, desired);                     \
	}

NAUTILUS_DEF_REF(int32_t)
NAUTILUS_DEF_REF(int64_t)
NAUTILUS_DEF_REF(uint32_t)
NAUTILUS_DEF_REF(uint64_t)
#undef NAUTILUS_DEF_REF

void atomic_thread_fence_seqcst() {
	invoke<>(&nautilus_atomic_thread_fence_seqcst);
}
void atomic_thread_fence_acquire() {
	invoke<>(&nautilus_atomic_thread_fence_acquire);
}
void atomic_thread_fence_release() {
	invoke<>(&nautilus_atomic_thread_fence_release);
}
void atomic_thread_fence_acq_rel() {
	invoke<>(&nautilus_atomic_thread_fence_acq_rel);
}
void atomic_thread_fence_relaxed() {
	invoke<>(&nautilus_atomic_thread_fence_relaxed);
}

} // namespace nautilus

#pragma once

#include "nautilus/val_ptr.hpp"
#include <atomic>
#include <cstdint>

namespace nautilus {

// =====================================================================================
// Free atomic functions on raw std::atomic<T>* — these wrap named extern "C" thunks
// defined in atomic.cpp so the function pointer is stable and can be matched by the
// MLIR backend intrinsic registry to lower the call directly to LLVM atomic ops.
// =====================================================================================

// Memory order is exposed as a non-type template parameter (compile-time constant) so
// that each ordering instantiates a distinct call-site that the MLIR intrinsic table
// can hook independently.

// ---- atomic_load ----
val<int32_t> atomic_load_seqcst(val<std::atomic<int32_t>*> obj);
val<int32_t> atomic_load_acquire(val<std::atomic<int32_t>*> obj);
val<int32_t> atomic_load_relaxed(val<std::atomic<int32_t>*> obj);
val<int64_t> atomic_load_seqcst(val<std::atomic<int64_t>*> obj);
val<int64_t> atomic_load_acquire(val<std::atomic<int64_t>*> obj);
val<int64_t> atomic_load_relaxed(val<std::atomic<int64_t>*> obj);
val<uint32_t> atomic_load_seqcst(val<std::atomic<uint32_t>*> obj);
val<uint32_t> atomic_load_acquire(val<std::atomic<uint32_t>*> obj);
val<uint32_t> atomic_load_relaxed(val<std::atomic<uint32_t>*> obj);
val<uint64_t> atomic_load_seqcst(val<std::atomic<uint64_t>*> obj);
val<uint64_t> atomic_load_acquire(val<std::atomic<uint64_t>*> obj);
val<uint64_t> atomic_load_relaxed(val<std::atomic<uint64_t>*> obj);

// ---- atomic_store ----
void atomic_store_seqcst(val<std::atomic<int32_t>*> obj, val<int32_t> v);
void atomic_store_release(val<std::atomic<int32_t>*> obj, val<int32_t> v);
void atomic_store_relaxed(val<std::atomic<int32_t>*> obj, val<int32_t> v);
void atomic_store_seqcst(val<std::atomic<int64_t>*> obj, val<int64_t> v);
void atomic_store_release(val<std::atomic<int64_t>*> obj, val<int64_t> v);
void atomic_store_relaxed(val<std::atomic<int64_t>*> obj, val<int64_t> v);
void atomic_store_seqcst(val<std::atomic<uint32_t>*> obj, val<uint32_t> v);
void atomic_store_release(val<std::atomic<uint32_t>*> obj, val<uint32_t> v);
void atomic_store_relaxed(val<std::atomic<uint32_t>*> obj, val<uint32_t> v);
void atomic_store_seqcst(val<std::atomic<uint64_t>*> obj, val<uint64_t> v);
void atomic_store_release(val<std::atomic<uint64_t>*> obj, val<uint64_t> v);
void atomic_store_relaxed(val<std::atomic<uint64_t>*> obj, val<uint64_t> v);

#define NAUTILUS_ATOMIC_RMW_DECL(name)                                                                                 \
	val<int32_t> name##_seqcst(val<std::atomic<int32_t>*> obj, val<int32_t> v);                                        \
	val<int32_t> name##_relaxed(val<std::atomic<int32_t>*> obj, val<int32_t> v);                                       \
	val<int64_t> name##_seqcst(val<std::atomic<int64_t>*> obj, val<int64_t> v);                                        \
	val<int64_t> name##_relaxed(val<std::atomic<int64_t>*> obj, val<int64_t> v);                                       \
	val<uint32_t> name##_seqcst(val<std::atomic<uint32_t>*> obj, val<uint32_t> v);                                     \
	val<uint32_t> name##_relaxed(val<std::atomic<uint32_t>*> obj, val<uint32_t> v);                                    \
	val<uint64_t> name##_seqcst(val<std::atomic<uint64_t>*> obj, val<uint64_t> v);                                     \
	val<uint64_t> name##_relaxed(val<std::atomic<uint64_t>*> obj, val<uint64_t> v);

NAUTILUS_ATOMIC_RMW_DECL(atomic_exchange)
NAUTILUS_ATOMIC_RMW_DECL(atomic_fetch_add)
NAUTILUS_ATOMIC_RMW_DECL(atomic_fetch_sub)
NAUTILUS_ATOMIC_RMW_DECL(atomic_fetch_and)
NAUTILUS_ATOMIC_RMW_DECL(atomic_fetch_or)
NAUTILUS_ATOMIC_RMW_DECL(atomic_fetch_xor)
#undef NAUTILUS_ATOMIC_RMW_DECL

// ---- atomic_compare_exchange_strong (runtime only — not lowered to LLVM cmpxchg) ----
val<bool> atomic_compare_exchange_strong(val<std::atomic<int32_t>*> obj, val<int32_t*> expected, val<int32_t> desired);
val<bool> atomic_compare_exchange_strong(val<std::atomic<int64_t>*> obj, val<int64_t*> expected, val<int64_t> desired);
val<bool> atomic_compare_exchange_strong(val<std::atomic<uint32_t>*> obj, val<uint32_t*> expected,
                                         val<uint32_t> desired);
val<bool> atomic_compare_exchange_strong(val<std::atomic<uint64_t>*> obj, val<uint64_t*> expected,
                                         val<uint64_t> desired);

// =====================================================================================
// Atomic operations directly on a val<T*> — treat the pointed-to memory as atomic
// without requiring a std::atomic wrapper (uses std::atomic_ref under the hood). The
// MLIR backend lowers these to the same LLVM atomic instructions as the std::atomic
// overloads above.
// =====================================================================================

#define NAUTILUS_ATOMIC_REF_DECL(T)                                                                                    \
	val<T> atomic_ref_load_seqcst(val<T*> ptr);                                                                        \
	val<T> atomic_ref_load_acquire(val<T*> ptr);                                                                       \
	val<T> atomic_ref_load_relaxed(val<T*> ptr);                                                                       \
	void atomic_ref_store_seqcst(val<T*> ptr, val<T> v);                                                               \
	void atomic_ref_store_release(val<T*> ptr, val<T> v);                                                              \
	void atomic_ref_store_relaxed(val<T*> ptr, val<T> v);                                                              \
	val<T> atomic_ref_exchange_seqcst(val<T*> ptr, val<T> v);                                                          \
	val<T> atomic_ref_exchange_relaxed(val<T*> ptr, val<T> v);                                                         \
	val<T> atomic_ref_fetch_add_seqcst(val<T*> ptr, val<T> v);                                                         \
	val<T> atomic_ref_fetch_add_relaxed(val<T*> ptr, val<T> v);                                                        \
	val<T> atomic_ref_fetch_sub_seqcst(val<T*> ptr, val<T> v);                                                         \
	val<T> atomic_ref_fetch_sub_relaxed(val<T*> ptr, val<T> v);                                                        \
	val<T> atomic_ref_fetch_and_seqcst(val<T*> ptr, val<T> v);                                                         \
	val<T> atomic_ref_fetch_and_relaxed(val<T*> ptr, val<T> v);                                                        \
	val<T> atomic_ref_fetch_or_seqcst(val<T*> ptr, val<T> v);                                                          \
	val<T> atomic_ref_fetch_or_relaxed(val<T*> ptr, val<T> v);                                                         \
	val<T> atomic_ref_fetch_xor_seqcst(val<T*> ptr, val<T> v);                                                         \
	val<T> atomic_ref_fetch_xor_relaxed(val<T*> ptr, val<T> v);                                                        \
	val<bool> atomic_ref_compare_exchange_strong(val<T*> ptr, val<T*> expected, val<T> desired);
NAUTILUS_ATOMIC_REF_DECL(int32_t)
NAUTILUS_ATOMIC_REF_DECL(int64_t)
NAUTILUS_ATOMIC_REF_DECL(uint32_t)
NAUTILUS_ATOMIC_REF_DECL(uint64_t)
#undef NAUTILUS_ATOMIC_REF_DECL

// Ordering-templated dispatchers operating directly on a val<T*>.
template <std::memory_order Order = std::memory_order_seq_cst, class T>
val<T> atomic_load(val<T*> ptr) {
	if constexpr (Order == std::memory_order_relaxed) {
		return atomic_ref_load_relaxed(ptr);
	} else if constexpr (Order == std::memory_order_acquire || Order == std::memory_order_consume) {
		return atomic_ref_load_acquire(ptr);
	} else {
		return atomic_ref_load_seqcst(ptr);
	}
}

template <std::memory_order Order = std::memory_order_seq_cst, class T>
void atomic_store(val<T*> ptr, val<T> desired) {
	if constexpr (Order == std::memory_order_relaxed) {
		atomic_ref_store_relaxed(ptr, desired);
	} else if constexpr (Order == std::memory_order_release) {
		atomic_ref_store_release(ptr, desired);
	} else {
		atomic_ref_store_seqcst(ptr, desired);
	}
}

#define NAUTILUS_ATOMIC_REF_RMW_TEMPLATE(name)                                                                         \
	template <std::memory_order Order = std::memory_order_seq_cst, class T>                                            \
	val<T> atomic_##name(val<T*> ptr, val<T> arg) {                                                                    \
		if constexpr (Order == std::memory_order_relaxed) {                                                            \
			return atomic_ref_##name##_relaxed(ptr, arg);                                                              \
		} else {                                                                                                       \
			return atomic_ref_##name##_seqcst(ptr, arg);                                                               \
		}                                                                                                              \
	}

NAUTILUS_ATOMIC_REF_RMW_TEMPLATE(exchange)
NAUTILUS_ATOMIC_REF_RMW_TEMPLATE(fetch_add)
NAUTILUS_ATOMIC_REF_RMW_TEMPLATE(fetch_sub)
NAUTILUS_ATOMIC_REF_RMW_TEMPLATE(fetch_and)
NAUTILUS_ATOMIC_REF_RMW_TEMPLATE(fetch_or)
NAUTILUS_ATOMIC_REF_RMW_TEMPLATE(fetch_xor)
#undef NAUTILUS_ATOMIC_REF_RMW_TEMPLATE

template <class T>
val<bool> atomic_compare_exchange_strong(val<T*> ptr, val<T*> expected, val<T> desired) {
	return atomic_ref_compare_exchange_strong(ptr, expected, desired);
}

// ---- atomic_thread_fence ----
void atomic_thread_fence_seqcst();
void atomic_thread_fence_acquire();
void atomic_thread_fence_release();
void atomic_thread_fence_acq_rel();
void atomic_thread_fence_relaxed();

// Convenience: ordering-templated dispatchers (default seq_cst). The body is a static
// dispatch on the order so call sites read like `atomic_load<acquire>(p)`.
template <std::memory_order Order = std::memory_order_seq_cst, class T>
val<T> atomic_load(val<std::atomic<T>*> obj) {
	if constexpr (Order == std::memory_order_relaxed) {
		return atomic_load_relaxed(obj);
	} else if constexpr (Order == std::memory_order_acquire || Order == std::memory_order_consume) {
		return atomic_load_acquire(obj);
	} else {
		return atomic_load_seqcst(obj);
	}
}

template <std::memory_order Order = std::memory_order_seq_cst, class T>
void atomic_store(val<std::atomic<T>*> obj, val<T> desired) {
	if constexpr (Order == std::memory_order_relaxed) {
		atomic_store_relaxed(obj, desired);
	} else if constexpr (Order == std::memory_order_release) {
		atomic_store_release(obj, desired);
	} else {
		atomic_store_seqcst(obj, desired);
	}
}

#define NAUTILUS_ATOMIC_RMW_TEMPLATE(name)                                                                             \
	template <std::memory_order Order = std::memory_order_seq_cst, class T>                                            \
	val<T> name(val<std::atomic<T>*> obj, val<T> arg) {                                                                \
		if constexpr (Order == std::memory_order_relaxed) {                                                            \
			return name##_relaxed(obj, arg);                                                                           \
		} else {                                                                                                       \
			return name##_seqcst(obj, arg);                                                                            \
		}                                                                                                              \
	}

NAUTILUS_ATOMIC_RMW_TEMPLATE(atomic_exchange)
NAUTILUS_ATOMIC_RMW_TEMPLATE(atomic_fetch_add)
NAUTILUS_ATOMIC_RMW_TEMPLATE(atomic_fetch_sub)
NAUTILUS_ATOMIC_RMW_TEMPLATE(atomic_fetch_and)
NAUTILUS_ATOMIC_RMW_TEMPLATE(atomic_fetch_or)
NAUTILUS_ATOMIC_RMW_TEMPLATE(atomic_fetch_xor)
#undef NAUTILUS_ATOMIC_RMW_TEMPLATE

template <std::memory_order Order = std::memory_order_seq_cst>
void atomic_thread_fence() {
	if constexpr (Order == std::memory_order_relaxed) {
		atomic_thread_fence_relaxed();
	} else if constexpr (Order == std::memory_order_acquire || Order == std::memory_order_consume) {
		atomic_thread_fence_acquire();
	} else if constexpr (Order == std::memory_order_release) {
		atomic_thread_fence_release();
	} else if constexpr (Order == std::memory_order_acq_rel) {
		atomic_thread_fence_acq_rel();
	} else {
		atomic_thread_fence_seqcst();
	}
}

// =====================================================================================
// val<std::atomic<T>> wrapper class.
//
// Stores an owning val<std::atomic<T>*>. All member functions delegate to the named
// free thunks above so the call-site function pointer is stable across templates and
// translation units.
// =====================================================================================
template <class T>
class val<std::atomic<T>> {
	using base_type = std::atomic<T>;

public:
	val() : data_ptr(nullptr), owning_(true) {
		data_ptr = invoke(+[]() -> base_type* { return new base_type(); });
	}

	explicit val(val<T> initial) : data_ptr(nullptr), owning_(true) {
		data_ptr = invoke(+[](T v) -> base_type* { return new base_type(v); }, initial);
	}

	// Non-owning view: wrap an existing val<std::atomic<T>*> (e.g. one passed in as a
	// function argument or returned from another traced call). The wrapper does not
	// take ownership and will not delete the underlying atomic on destruction.
	explicit val(val<base_type*> ptr) : data_ptr(ptr), owning_(false) {
	}

	val(const val&) = delete;
	val& operator=(const val&) = delete;

	~val() {
		if (!owning_) {
			return;
		}
		invoke(
		    +[](base_type* p) {
			    if (p != nullptr) {
				    delete p;
			    }
		    },
		    data_ptr);
	}

	val<base_type*> get() const {
		return data_ptr;
	}

	template <std::memory_order Order = std::memory_order_seq_cst>
	val<T> load() const {
		return atomic_load<Order, T>(data_ptr);
	}

	template <std::memory_order Order = std::memory_order_seq_cst>
	void store(val<T> v) {
		atomic_store<Order, T>(data_ptr, v);
	}

	template <std::memory_order Order = std::memory_order_seq_cst>
	val<T> exchange(val<T> v) {
		return atomic_exchange<Order, T>(data_ptr, v);
	}

	template <std::memory_order Order = std::memory_order_seq_cst>
	val<T> fetch_add(val<T> v) {
		return atomic_fetch_add<Order, T>(data_ptr, v);
	}

	template <std::memory_order Order = std::memory_order_seq_cst>
	val<T> fetch_sub(val<T> v) {
		return atomic_fetch_sub<Order, T>(data_ptr, v);
	}

	template <std::memory_order Order = std::memory_order_seq_cst>
	val<T> fetch_and(val<T> v) {
		return atomic_fetch_and<Order, T>(data_ptr, v);
	}

	template <std::memory_order Order = std::memory_order_seq_cst>
	val<T> fetch_or(val<T> v) {
		return atomic_fetch_or<Order, T>(data_ptr, v);
	}

	template <std::memory_order Order = std::memory_order_seq_cst>
	val<T> fetch_xor(val<T> v) {
		return atomic_fetch_xor<Order, T>(data_ptr, v);
	}

	val<bool> compare_exchange_strong(val<T*> expected, val<T> desired) {
		return atomic_compare_exchange_strong(data_ptr, expected, desired);
	}

private:
	val<base_type*> data_ptr;
	bool owning_;
};

} // namespace nautilus

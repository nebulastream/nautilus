#pragma once

#include "nautilus/Engine.hpp"
#include "nautilus/val.hpp"
#include "nautilus/val_ptr.hpp"
#include <cstddef>
#include <cstdint>
#include <utility>

namespace nautilus::gpu {

namespace detail {
// Backend-specific unified-memory primitives + the runtime size table.
// Defined in gpu.cpp; selected at build time by the active GPU backend
// (Metal: page-aligned host memory; CUDA: cudaMallocManaged).
void* allocUnifiedBytes(size_t bytes);               // allocate page-rounded unified memory
void freeUnifiedBytes(void* ptr);                    // free memory from allocUnifiedBytes
void registerBuffer(void* ptr, uint64_t allocBytes); // record ptr -> page-rounded size
void unregisterBuffer(void* ptr);                    // drop a size-table entry
size_t pageRoundUp(size_t bytes);                    // round up to the system page size
} // namespace detail

/// A handle over a unified, GPU-accessible buffer.
///
/// `Array<T>` plays two roles from a single type:
///   1. **Host handle** (outside tracing) — returned by allocUnified/copy/wrap;
///      carries the real pointer, element count, and ownership. Converts to
///      `T*` so it can be passed straight to a compiled host function.
///   2. **Traced kernel argument** — built from a single function-argument value
///      ref by `engine::details::arg_factory<Array<T>>`, so it occupies exactly
///      one IR/ABI slot (`raw_type == T*`) and lowers identically to `val<T*>`.
///      Indexable like a pointer inside kernels.
template <class T>
class Array {
public:
	using raw_type = T*;

	/// Build from a pointer value (the arg-factory hook and the implicit
	/// conversion used when a host `val<T*>` is forwarded into a kernel that
	/// declares an `Array<T>` parameter).
	Array(const val<T*>& ptr) : ptr_(rebuild(ptr)) {
	}

	// Copy/move/assign rebuild the underlying val<T*> from the source's value +
	// tracing state directly, instead of letting val<T*>'s copy constructor run
	// (which emits a traceCopy operation). Those copies happen at a single
	// source location inside the implicitly-generated Array copy constructor, so
	// their tags collide and distinct buffer arguments get deduplicated into one
	// — the cause of `b` aliasing `a`. Reconstructing keeps each argument's ref.
	Array(const Array& o) : ptr_(rebuild(o.ptr_)), hostPtr_(o.hostPtr_), count_(o.count_), owns_(o.owns_) {
	}
	Array(Array&& o) noexcept : ptr_(rebuild(o.ptr_)), hostPtr_(o.hostPtr_), count_(o.count_), owns_(o.owns_) {
	}
	Array& operator=(const Array& o) {
		ptr_ = rebuild(o.ptr_);
		hostPtr_ = o.hostPtr_;
		count_ = o.count_;
		owns_ = o.owns_;
		return *this;
	}
	Array& operator=(Array&& o) noexcept {
		return *this = o;
	}

	/// Adopt a concrete unified allocation as a host handle. Used by the
	/// allocUnified/copy/wrap free functions.
	static Array<T> adopt(T* ptr, size_t count, bool owns) {
		Array<T> a {val<T*>(ptr)};
		a.hostPtr_ = ptr;
		a.count_ = count;
		a.owns_ = owns;
		return a;
	}

	/// Pointer-like indexing inside kernels (forwards to the underlying val<T*>).
	///
	/// Force-inlined: the tracer identifies each traced operation by its runtime
	/// call stack, so this wrapper must not introduce its own stack frame —
	/// otherwise distinct accesses (e.g. `a[i]` and `b[i]`) would share a tag
	/// and be deduplicated into one. Inlining makes every call site unique, the
	/// same way direct `val<T*>` indexing is.
	template <class I>
	[[gnu::always_inline]] inline auto operator[](I&& index) {
		return ptr_[std::forward<I>(index)];
	}

	/// Usable wherever a `val<T*>` is expected (pointer arithmetic, intrinsics).
	[[gnu::always_inline]] inline operator val<T*>() const {
		return ptr_;
	}

#ifdef ENABLE_TRACING
	/// Lets call sites (getArgumentReferences / StateResolver) treat an Array
	/// argument as its underlying pointer.
	tracing::TypedValueRef getState() const {
		return ptr_.getState();
	}
#endif

	// --- Host-handle accessors (valid outside tracing) ---
	T* data() const {
		return hostPtr_;
	}
	size_t size() const {
		return count_;
	}
	size_t bytes() const {
		return count_ * sizeof(T);
	}
	bool owns() const {
		return owns_;
	}
	/// Decay to the raw pointer for the compiled host-function ABI.
	operator T*() const {
		return hostPtr_;
	}

private:
	// Rebuild a val<T*> carrying the same value and tracing state as `src`
	// without emitting a traceCopy (uses the value+ref constructor directly).
	static val<T*> rebuild(const val<T*>& src) {
		auto value = details::RawValueResolver<T*>::getRawValue(src);
#ifdef ENABLE_TRACING
		return val<T*>(value, src.getState());
#else
		return val<T*>(value);
#endif
	}

	val<T*> ptr_;
	T* hostPtr_ = nullptr;
	size_t count_ = 0;
	bool owns_ = false;
};

/// Allocate a new unified buffer of `count` elements (owning).
template <class T>
Array<T> allocUnified(size_t count) {
	auto* p = static_cast<T*>(detail::allocUnifiedBytes(count * sizeof(T)));
	detail::registerBuffer(p, detail::pageRoundUp(count * sizeof(T)));
	return Array<T>::adopt(p, count, /*owns=*/true);
}

/// Allocate a new unified buffer and copy `count` elements from `src` into it
/// (owning). This is the one boundary copy for arbitrary host input.
template <class T>
Array<T> copy(const T* src, size_t count) {
	auto a = allocUnified<T>(count);
	for (size_t i = 0; i < count; ++i) {
		a.data()[i] = src[i];
	}
	return a;
}

/// Wrap a pointer the caller guarantees is already unified-compatible
/// (page-aligned on Metal, cudaMallocManaged on CUDA). No allocation, no copy,
/// non-owning. Misuse (incompatible memory) is a caller error.
template <class T>
Array<T> wrap(T* ptr, size_t count) {
	detail::registerBuffer(ptr, detail::pageRoundUp(count * sizeof(T)));
	return Array<T>::adopt(ptr, count, /*owns=*/false);
}

/// Free a unified buffer. Frees the underlying memory iff the Array owns it,
/// and always drops the size-table entry.
template <class T>
void freeUnified(Array<T>& a) {
	auto* p = a.data();
	if (p == nullptr) {
		return;
	}
	detail::unregisterBuffer(p);
	if (a.owns()) {
		detail::freeUnifiedBytes(p);
	}
	a = Array<T>(val<T*>(static_cast<T*>(nullptr)));
}

} // namespace nautilus::gpu

namespace nautilus::engine::details {
/// Build a kernel/host `gpu::Array<T>` parameter from its single value ref.
template <class T>
struct arg_factory<nautilus::gpu::Array<T>> {
	static nautilus::gpu::Array<T> make(tracing::TypedValueRef ref) {
		return nautilus::gpu::Array<T>(val<T*>(ref));
	}
};
} // namespace nautilus::engine::details

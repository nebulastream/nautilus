
#pragma once

#include <cassert>
#include <cstddef>
#include <vector>

namespace nautilus {

/**
 * Thread-local arena for alloca'd memory in non-tracing (interpreted) execution.
 *
 * In the compiled path, val<T*>::alloca() maps to an IR AllocaOperation which becomes
 * an LLVM stack allocation — memory is freed automatically on function return.
 *
 * In the non-tracing (direct execution) path this arena provides equivalent lifetime
 * semantics: all allocations are freed when the AllocationContextGuard is destroyed
 * at the call boundary in Engine::CallableFunction::operator().
 */
class AllocationContext {
public:
	template <typename T>
	T* allocate() {
		void* raw = ::operator new(sizeof(T));
		entries_.push_back(raw);
		return static_cast<T*>(raw);
	}

	~AllocationContext() {
		for (auto* ptr : entries_)
			::operator delete(ptr);
	}

	static AllocationContext* get() {
		return current_;
	}

private:
	std::vector<void*> entries_;
	inline static thread_local AllocationContext* current_ = nullptr;
	friend class AllocationContextGuard;
};

/**
 * RAII guard that activates an AllocationContext for the lifetime of a function call.
 * Nesting is supported: the previous context is restored on destruction.
 */
class AllocationContextGuard {
public:
	AllocationContextGuard() : prev_(AllocationContext::current_) {
		AllocationContext::current_ = &ctx_;
	}

	~AllocationContextGuard() {
		AllocationContext::current_ = prev_;
	}

	AllocationContextGuard(const AllocationContextGuard&) = delete;
	AllocationContextGuard& operator=(const AllocationContextGuard&) = delete;

private:
	AllocationContext* prev_;
	AllocationContext ctx_;
};

} // namespace nautilus

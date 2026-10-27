// Replaces the global operator new of the nautilus-nir-tracer-tests executable to count heap allocations, so that a
// test can state how many a trace makes (docs/plans/nautilus-0.4/12-validation.md, row 01: no malloc per recorded op).
#include "AllocationCounter.hpp"
#include <atomic>
#include <cstdlib>
#include <new>

namespace {
std::atomic<uint64_t> allocationCount {0};
} // namespace

namespace nautilus::testing {
uint64_t allocations() {
	return allocationCount.load(std::memory_order_relaxed);
}
} // namespace nautilus::testing

void* operator new(std::size_t size) {
	allocationCount.fetch_add(1, std::memory_order_relaxed);
	if (void* p = std::malloc(size == 0 ? 1 : size)) {
		return p;
	}
	throw std::bad_alloc();
}
void* operator new[](std::size_t size) {
	return operator new(size);
}
void* operator new(std::size_t size, std::align_val_t align) {
	allocationCount.fetch_add(1, std::memory_order_relaxed);
	auto a = static_cast<std::size_t>(align);
	if (void* p = std::aligned_alloc(a, (size + a - 1) / a * a)) {
		return p;
	}
	throw std::bad_alloc();
}
void* operator new[](std::size_t size, std::align_val_t align) {
	return operator new(size, align);
}
void operator delete(void* p) noexcept {
	std::free(p);
}
void operator delete[](void* p) noexcept {
	std::free(p);
}
void operator delete(void* p, std::size_t) noexcept {
	std::free(p);
}
void operator delete[](void* p, std::size_t) noexcept {
	std::free(p);
}
void operator delete(void* p, std::align_val_t) noexcept {
	std::free(p);
}
void operator delete[](void* p, std::align_val_t) noexcept {
	std::free(p);
}
void operator delete(void* p, std::size_t, std::align_val_t) noexcept {
	std::free(p);
}
void operator delete[](void* p, std::size_t, std::align_val_t) noexcept {
	std::free(p);
}

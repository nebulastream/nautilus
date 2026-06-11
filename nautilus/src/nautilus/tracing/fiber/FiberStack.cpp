
#include "FiberStack.hpp"
#include "nautilus/exceptions/RuntimeException.hpp"
#include <string>
#include <sys/mman.h>
#include <unistd.h>

namespace nautilus::tracing::detail {

FiberStack::FiberStack(size_t size) {
	const auto pageSize = static_cast<size_t>(sysconf(_SC_PAGESIZE));
	const size_t usable = ((size + pageSize - 1) / pageSize) * pageSize;
	mappingSize_ = usable + pageSize; // one guard page at the low end
	void* mapping = mmap(nullptr, mappingSize_, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	if (mapping == MAP_FAILED) {
		throw RuntimeException("Failed to allocate the tracing side stack (" + std::to_string(mappingSize_) +
		                       " bytes)");
	}
	mapping_ = mapping;
	if (mprotect(mapping, pageSize, PROT_NONE) != 0) {
		munmap(mapping, mappingSize_);
		mapping_ = nullptr;
		throw RuntimeException("Failed to protect the tracing side stack guard page");
	}
	bottom_ = static_cast<std::byte*>(mapping) + pageSize;
	top_ = bottom_ + usable;
}

FiberStack::~FiberStack() {
	if (mapping_ != nullptr) {
		munmap(mapping_, mappingSize_);
	}
}

} // namespace nautilus::tracing::detail



#include "TagRecorder.hpp"
#include "nautilus/tracing/exceptions/TagCreationException.hpp"
#include <execinfo.h>

namespace nautilus::tracing {

#pragma GCC diagnostic ignored "-Wframe-address"

static void* getReturnAddress(uint32_t offset);

TagRecorder::TagRecorder(TagAddress startAddress) : startAddress(startAddress) {
	useBuiltinTagCreation = getReturnAddress(1) != nullptr;
}

Tag* TagRecorder::createReferenceTagBacktrace() {
	auto* currentTagNode = &rootTagTrieNode;
	void* tagBuffer[MAX_TAG_SIZE];
	int size = backtrace(tagBuffer, MAX_TAG_SIZE);
	for (int i = 0; i < size; i++) {
		auto tagAddress = (TagAddress) tagBuffer[i];
		if (tagAddress == startAddress) {
			return currentTagNode;
		}
		currentTagNode = currentTagNode->append(tagAddress);
	}
	throw TagCreationException("Stack is too deep. This could indicate the use "
	                           "of recursive control-flow,"
	                           " which is not supported in Nautilus code.");
}

Tag* TagRecorder::createReferenceTagBuildin() {
	auto* currentTagNode = &rootTagTrieNode;

	for (size_t i = 0; i <= MAX_TAG_SIZE; i++) {
		auto tagAddress = (TagAddress) getReturnAddress(i);
		if (tagAddress == startAddress) {
			return currentTagNode;
		}
		currentTagNode = currentTagNode->append(tagAddress);
	}
	throw TagCreationException("Stack is too deep. This could indicate the use "
	                           "of recursive control-flow,"
	                           " which is not supported in Nautilus code.");
}

Tag* TagRecorder::createReferenceTag() {
	if (useBuiltinTagCreation) {
		return createReferenceTagBuildin();
	}
	return createReferenceTagBacktrace();
}

template <size_t StackSize>
__attribute__((noinline)) void* get_addr(size_t index) {
	return [&]<std::size_t... ints>(std::index_sequence<ints...>) __attribute__((noinline)) {
		void* addr = nullptr;
		(void) ((index == ints &&
		         (void(addr = __builtin_extract_return_addr(__builtin_return_address(ints + 2))), true)) ||
		        ...);
		return addr;
	}
	(std::make_index_sequence<StackSize> {});
}

static void* getReturnAddress(uint32_t offset) {
	return get_addr<TagRecorder::MAX_TAG_SIZE>(offset);
}

} // namespace nautilus::tracing

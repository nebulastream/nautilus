

#include "TagRecorder.hpp"
#include "nautilus/exceptions/NotImplementedException.hpp"
#include "nautilus/tracing/exceptions/TagCreationException.hpp"
#include <execinfo.h>

namespace nautilus::tracing {

#pragma GCC diagnostic ignored "-Wframe-address"

static void* getReturnAddress(uint32_t offset);

TagRecorder::TagRecorder(TagAddress startAddress) : startAddress(startAddress) {
	useBuiltinTagCreation = getReturnAddress(1) != nullptr;
}

// check if gnu backtrace is available.
#if defined(BACKWARD_HAS_BACKTRACE) & !defined(HOST_IS_MUSL)
TagVector TagRecorder::createBaseTag() {
	void* tagBuffer[MAX_TAG_SIZE];
	int size = backtrace(tagBuffer, MAX_TAG_SIZE);
	std::vector<TagAddress> addresses;
	for (int i = 0; i < size; i++) {
		addresses.emplace_back((TagAddress) tagBuffer[i]);
	}
	return {addresses};
}
#else

static void* getReturnAddress(uint32_t offset);

TagVector TagRecorder::createBaseTag() {
	// throw NotImplementedException("No plugin registered that can handle this
	// operation between");
	[[maybe_unused]] void* root = __builtin_thread_pointer();
	std::vector<TagAddress> addresses;
	for (size_t i = 0; i < MAX_TAG_SIZE; i++) {
		auto address = getReturnAddress(i);
		[[maybe_unused]] void* addr = __builtin_extract_return_addr(address);
		addresses.emplace_back((TagAddress) address);
	}
	return addresses;
}

#endif

TagAddress TagRecorder::getBaseAddress(TagVector& tag1, TagVector& tag2) {
	auto& tag1Addresses = tag1.getAddresses();
	auto& tag2Addresses = tag2.getAddresses();
	auto size = std::min(tag1Addresses.size(), tag2Addresses.size());
	size_t index = 0;
	while (index < size && tag1Addresses[index] == tag2Addresses[index]) {
		index++;
	}
	while (index < size && tag1Addresses[index] != tag2Addresses[index]) {
		index++;
	}
	if (index >= size) {
		return -1;
	}
	return tag1Addresses[index];
}

TrieIndex TagRecorder::createReferenceTagBacktrace() {
	auto currentTagNode = trie.root();
	void* tagBuffer[MAX_TAG_SIZE];
	int size = backtrace(tagBuffer, MAX_TAG_SIZE);
	for (int i = 0; i < size; i++) {
		auto tagAddress = reinterpret_cast<TagAddress>(tagBuffer[i]);
		if (tagAddress == startAddress) {
			return currentTagNode;
		}
		currentTagNode = trie.append(currentTagNode, tagAddress);
	}
	throw TagCreationException("Stack is too deep. This could indicate the use "
	                           "of recursive control-flow,"
	                           " which is not supported in Nautilus code.");
}

TrieIndex TagRecorder::createReferenceTagBuildin() {
	auto currentTagNode = trie.root();
#pragma GCC diagnostic ignored "-Wframe-address"
	[[maybe_unused]] auto tag1 = __builtin_return_address(0);
	[[maybe_unused]] auto tag2 = __builtin_return_address(1);
	[[maybe_unused]] auto tag3 = __builtin_return_address(1);

	for (size_t i = 0; i <= MAX_TAG_SIZE; i++) {
		auto tagAddress = reinterpret_cast<TagAddress>(getReturnAddress(i));
		if (tagAddress == startAddress) {
			return currentTagNode;
		}
		currentTagNode = trie.append(currentTagNode, tagAddress);
	}
	throw TagCreationException("Stack is too deep. This could indicate the use "
	                           "of recursive control-flow,"
	                           " which is not supported in Nautilus code.");
}

TrieIndex TagRecorder::createReferenceTag() {
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
		         (void(addr = __builtin_extract_return_addr(__builtin_return_address(ints + 3))), true)) ||
		        ...);
		return addr;
	}
	(std::make_index_sequence<StackSize> {});
}

static void* getReturnAddress(uint32_t offset) {
	return get_addr<TagRecorder::MAX_TAG_SIZE>(offset);
}

} // namespace nautilus::tracing

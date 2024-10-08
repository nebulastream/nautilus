

#include "TagRecorder.hpp"
#include "nautilus/exceptions/NotImplementedException.hpp"
#include <execinfo.h>
#include <nautilus/tracing/exceptions/TagCreationException.hpp>

namespace nautilus::tracing {

#pragma GCC diagnostic ignored "-Wframe-address"

template<size_t StackSize>
struct ReturnAddressHelper {
	template<size_t Depth = 1>
	static void* get_addr(size_t index){
		if(index == StackSize){
			return __builtin_return_address(StackSize + Depth);
		}else{
			return ReturnAddressHelper<StackSize-1>::template get_addr<Depth + 1>(index);
		}
	}
};

template<>
struct ReturnAddressHelper<0>{
	template<size_t Depth = 1>
	static void* get_addr(size_t){
		return __builtin_return_address(Depth);
	}
};

static void* getReturnAddress(uint32_t offset) {
	return ReturnAddressHelper<TagRecorder::MAX_TAG_SIZE>::get_addr(offset);

}

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

Tag* TagRecorder::createReferenceTagBacktrace() {
	auto* currentTagNode = &rootTagThreeNode;
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
	auto* currentTagNode = &rootTagThreeNode;
#pragma GCC diagnostic ignored "-Wframe-address"
	[[maybe_unused]] auto tag1 = __builtin_return_address(0);
	[[maybe_unused]] auto tag2 = __builtin_return_address(1);
	[[maybe_unused]] auto tag3 = __builtin_return_address(1);

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

} // namespace nautilus::tracing

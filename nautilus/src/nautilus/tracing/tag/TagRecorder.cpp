

#include "TagRecorder.hpp"
#include "nautilus/exceptions/NotImplementedException.hpp"
#include "nautilus/tracing/exceptions/TagCreationException.hpp"
#include <execinfo.h>

namespace nautilus::tracing {

#pragma GCC diagnostic ignored "-Wframe-address"

namespace {
// Largest plausible distance between a callee's frame record and its caller's.
// Real frames are far smaller; this only has to rule out wild pointers.
constexpr uintptr_t MAX_PLAUSIBLE_FRAME_DELTA = 64ull * 1024 * 1024;

/**
 * @brief Best-effort check that @p caller is a real frame record linked from @p callee.
 *
 * The frame-pointer walks below rely on the frame-record convention shared by the x86-64 SysV and
 * AArch64 AAPCS64 ABIs (frame[0] = caller's frame pointer, frame[1] = return address), which only
 * holds when everything on the stack was built with -fno-omit-frame-pointer. Nautilus applies that
 * flag to itself and exports it to consumers (see nautilus/CMakeLists.txt), but a caller that
 * compiles against the library without going through CMake gets the compiler default instead --
 * omitted frame pointers at -O1 and above. frame[0] then holds ordinary local data, and following
 * it walks into unmapped memory.
 *
 * A well-formed chain runs towards higher addresses (the stack grows down, so a caller's frame
 * always sits above its callee's) and every link is pointer-aligned, which arbitrary data almost
 * never satisfies. This cannot be exhaustive -- data that happens to look like a frame pointer is
 * indistinguishable -- but it turns the common case from a segfault into a diagnosable error.
 */
bool isPlausibleCallerFrame(void** callee, void** caller) {
	if (caller == nullptr) {
		return false;
	}
	const auto calleeAddress = reinterpret_cast<uintptr_t>(callee);
	const auto callerAddress = reinterpret_cast<uintptr_t>(caller);
	if (callerAddress % alignof(void*) != 0) {
		return false;
	}
	if (callerAddress <= calleeAddress) {
		return false;
	}
	return callerAddress - calleeAddress <= MAX_PLAUSIBLE_FRAME_DELTA;
}

[[noreturn]] void throwBrokenFrameChain() {
	throw TagCreationException(
	    "Could not walk the call stack: the frame-pointer chain is broken. Nautilus derives the tag that "
	    "identifies each traced instruction from the caller's stack frames, which requires every frame on the "
	    "stack to have a frame pointer. Rebuild the code that calls into Nautilus with -fno-omit-frame-pointer. "
	    "Linking against the CMake target 'nautilus' applies this flag automatically; compiling against the "
	    "library by hand does not.");
}
} // namespace

TagRecorder::TagRecorder(TagAddress startAddress, common::Arena& arena) : startAddress(startAddress), arena(arena) {
	useBuiltinTagCreation = __builtin_return_address(1) != nullptr;
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

// Walks the frame-pointer chain in a single linear pass; see the comment on createReferenceTagBuildin
// below for why this replaces resolving each frame independently via __builtin_return_address(N).
__attribute__((noinline)) TagVector TagRecorder::createBaseTag() {
	std::vector<TagAddress> addresses;
	auto** frame = static_cast<void**>(__builtin_frame_address(0));
	for (size_t i = 0; i < MAX_TAG_SIZE && frame != nullptr; i++) {
		addresses.emplace_back((TagAddress) frame[1]);
		auto** caller = static_cast<void**>(frame[0]);
		if (caller == nullptr) {
			break; // Clean end of the chain (outermost frame).
		}
		if (!isPlausibleCallerFrame(frame, caller)) {
			throwBrokenFrameChain();
		}
		frame = caller;
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
		currentTagNode = currentTagNode->append(tagAddress, arena);
	}
	throw TagCreationException("Stack is too deep. This could indicate the use "
	                           "of recursive control-flow,"
	                           " which is not supported in Nautilus code.");
}

// Walks the frame-pointer chain in a single linear pass instead of resolving each frame's return
// address independently via __builtin_return_address(N), which re-walks N frames from the top every
// call and makes a loop over depth 0..d cost O(d^2) frame dereferences in total (see issue #426).
// This relies on the frame-pointer chain being intact, which the project guarantees globally via
// -fno-omit-frame-pointer. The layout (frame[0] = saved/previous frame pointer, frame[1] = return
// address) matches the frame-record convention used by both the x86-64 SysV and AArch64 AAPCS64 ABIs.
__attribute__((noinline)) Tag* TagRecorder::createReferenceTagBuildin() {
	auto* currentTagNode = &rootTagThreeNode;
#pragma GCC diagnostic ignored "-Wframe-address"
	auto** frame = static_cast<void**>(__builtin_frame_address(0));
	for (size_t i = 0; i <= MAX_TAG_SIZE && frame != nullptr; i++) {
		auto tagAddress = (TagAddress) frame[1];
		if (tagAddress == startAddress) {
			return currentTagNode;
		}
		currentTagNode = currentTagNode->append(tagAddress, arena);
		auto** caller = static_cast<void**>(frame[0]);
		// Validate before dereferencing on the next iteration. The walk always
		// terminates by finding startAddress, so a chain that runs out early is
		// never normal -- it means frame[0] was not a frame pointer at all.
		if (!isPlausibleCallerFrame(frame, caller)) {
			throwBrokenFrameChain();
		}
		frame = caller;
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

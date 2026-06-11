
#pragma once

#include <cstddef>
#include <cstdint>

// AddressSanitizer needs to be told about stack switches: without the
// start/finish_switch_fiber annotations its shadow-memory bookkeeping is bound
// to the thread's original stack and the first traced operation on the side
// stack triggers false positives. The wrappers below compile to no-ops in
// non-ASAN builds.
//
// Note for ASAN users: the stack-copy tracer restores stack bytes of frames
// that already returned. This is incompatible with ASAN's use-after-return
// detection (the restored frames reference released fake-stack frames), so
// `detect_stack_use_after_return=0` must be set when tracing with
// engine.traceMode=stackCopyTracing under ASAN. The nautilus test binaries do
// this via __asan_default_options.

#if defined(__has_feature)
#if __has_feature(address_sanitizer)
#define NAUTILUS_HAS_ASAN 1
#endif
#elif defined(__SANITIZE_ADDRESS__)
#define NAUTILUS_HAS_ASAN 1
#endif

#ifdef NAUTILUS_HAS_ASAN
extern "C" {
void __sanitizer_start_switch_fiber(void** fake_stack_save, const void* bottom, size_t size);
void __sanitizer_finish_switch_fiber(void* fake_stack_save, const void** bottom_old, size_t* size_old);
void __asan_unpoison_memory_region(const volatile void* addr, size_t size);
}
#endif

namespace nautilus::tracing::detail {

inline void sanitizerStartSwitchFiber([[maybe_unused]] void** fakeStackSave, [[maybe_unused]] const void* bottom,
                                      [[maybe_unused]] size_t size) {
#ifdef NAUTILUS_HAS_ASAN
	__sanitizer_start_switch_fiber(fakeStackSave, bottom, size);
#endif
}

inline void sanitizerFinishSwitchFiber([[maybe_unused]] void* fakeStackSave) {
#ifdef NAUTILUS_HAS_ASAN
	__sanitizer_finish_switch_fiber(fakeStackSave, nullptr, nullptr);
#endif
}

/// Variant that also reports the bounds of the stack we switched away from; used on
/// first fiber entry to learn the host thread's stack bounds for the way back.
inline void sanitizerFinishSwitchFiber([[maybe_unused]] void* fakeStackSave, [[maybe_unused]] const void** oldBottom,
                                       [[maybe_unused]] size_t* oldSize) {
#ifdef NAUTILUS_HAS_ASAN
	__sanitizer_finish_switch_fiber(fakeStackSave, oldBottom, oldSize);
#endif
}

inline void sanitizerUnpoisonStack([[maybe_unused]] const void* addr, [[maybe_unused]] size_t size) {
#ifdef NAUTILUS_HAS_ASAN
	__asan_unpoison_memory_region(addr, size);
#endif
}

// Prevents the compiler from recognizing the copy loop below as a memcpy idiom and
// lowering it to the (ASAN-intercepted) libc call.
#if defined(__clang__)
#define NAUTILUS_NO_MEMCPY_LOWERING __attribute__((no_builtin("memcpy", "memmove")))
#elif defined(__GNUC__)
#define NAUTILUS_NO_MEMCPY_LOWERING __attribute__((optimize("no-tree-loop-distribute-patterns")))
#else
#define NAUTILUS_NO_MEMCPY_LOWERING
#endif

/**
 * @brief memcpy for raw stack regions that deliberately include ASAN redzones.
 *
 * Snapshotting copies the live stack byte-for-byte, poisoned redzones between
 * frames included; the libc memcpy is intercepted by ASAN and would report a
 * stack-buffer access for those bytes. This helper is exempt from instrumentation
 * and must not be lowered to a memcpy libcall (hence the word-wise loop and
 * NAUTILUS_NO_MEMCPY_LOWERING).
 */
__attribute__((no_sanitize("address"))) NAUTILUS_NO_MEMCPY_LOWERING inline void
rawStackCopy(void* destination, const void* source, size_t size) {
	auto* dst = static_cast<unsigned char*>(destination);
	const auto* src = static_cast<const unsigned char*>(source);
	size_t i = 0;
	for (; i + sizeof(uintptr_t) <= size; i += sizeof(uintptr_t)) {
		uintptr_t word;
		__builtin_memcpy(&word, src + i, sizeof(word));
		__builtin_memcpy(dst + i, &word, sizeof(word));
	}
	for (; i < size; i++) {
		dst[i] = src[i];
	}
}

} // namespace nautilus::tracing::detail

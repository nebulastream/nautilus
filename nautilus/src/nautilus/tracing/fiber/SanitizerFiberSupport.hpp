
#pragma once

#include <cstddef>

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
void __asan_unpoison_memory_region(void const volatile* addr, size_t size);
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

} // namespace nautilus::tracing::detail

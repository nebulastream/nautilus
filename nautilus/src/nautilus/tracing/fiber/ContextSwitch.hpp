
#pragma once

#include <cstddef>

namespace nautilus::tracing::detail {

// The stack-copy tracer requires a hand-written context switch. It is implemented
// for the architectures covered by CI (x86-64 SysV and AArch64 AAPCS).
#if defined(__x86_64__) || defined(__aarch64__)
#define NAUTILUS_FIBER_SUPPORTED 1
#endif

/**
 * @brief Architecture-specific register save area for a suspended execution context.
 *
 * Stores the callee-saved registers, the stack pointer and the resume address of a
 * capture point (setjmp-like semantics, but independent of libc so that resuming on
 * a different stack than the current one is well defined). The pointed-to stack
 * memory is NOT part of the context; callers that resume a context after its stack
 * region was reused must restore the stack bytes first (see StackCopyTraceContext).
 */
struct CapturedContext {
	// 8 slots are sufficient for x86-64 (rbx, rbp, r12-r15, rsp, rip + fp control),
	// AArch64 needs 21 (x19-x28, fp, sp, lr, d8-d15). Sized for the larger one.
	void* data[24];
};

extern "C" {

/**
 * @brief Saves the current execution context into @p ctx.
 * @return 0 when the context was captured, 1 when control returns here via
 *         nautilus_resumeContext.
 */
__attribute__((returns_twice)) int nautilus_captureContext(CapturedContext* ctx);

/**
 * @brief Restores @p ctx and continues execution at its capture point, where
 * nautilus_captureContext appears to return 1. The stack bytes at the captured
 * stack pointer must be valid (either still untouched or restored by the caller).
 */
[[noreturn]] void nautilus_resumeContext(const CapturedContext* ctx);

/**
 * @brief Captures the current context into @p from, switches the stack pointer to
 * @p stackTop and invokes entry(arg). @p entry must never return; the fiber hands
 * control back by calling nautilus_resumeContext(from), which makes this function
 * return 1. Resuming @p from multiple times is supported as long as the caller's
 * frame is still intact.
 */
__attribute__((returns_twice)) int nautilus_enterOnStack(CapturedContext* from, void* stackTop, void (*entry)(void*),
                                                         void* arg);
}

/// Returns the stack pointer that was live at the capture point of @p ctx.
inline void* capturedStackPointer(const CapturedContext& ctx) {
#if defined(__x86_64__)
	return ctx.data[6];
#elif defined(__aarch64__)
	return ctx.data[11];
#else
	return nullptr;
#endif
}

} // namespace nautilus::tracing::detail

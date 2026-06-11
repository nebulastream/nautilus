/*
 * Minimal context capture/resume primitives for the stack-copy tracer (AArch64 AAPCS).
 *
 * CapturedContext layout (8-byte slots):
 *   0-9: x19-x28  10: x29 (fp)  11: sp  12: x30 (resume address)
 *   13-20: d8-d15
 */

#if defined(__aarch64__)

#if defined(__APPLE__)
#define SYM(name) _##name
#else
#define SYM(name) name
#endif

.text

.globl SYM(nautilus_captureContext)
#if !defined(__APPLE__)
.type nautilus_captureContext, @function
#endif
.align 4
SYM(nautilus_captureContext):
	stp x19, x20, [x0, #0]
	stp x21, x22, [x0, #16]
	stp x23, x24, [x0, #32]
	stp x25, x26, [x0, #48]
	stp x27, x28, [x0, #64]
	str x29, [x0, #80]
	mov x9, sp
	str x9, [x0, #88]
	str x30, [x0, #96]
	stp d8, d9, [x0, #104]
	stp d10, d11, [x0, #120]
	stp d12, d13, [x0, #136]
	stp d14, d15, [x0, #152]
	mov w0, #0
	ret

.globl SYM(nautilus_resumeContext)
#if !defined(__APPLE__)
.type nautilus_resumeContext, @function
#endif
.align 4
SYM(nautilus_resumeContext):
	ldp x19, x20, [x0, #0]
	ldp x21, x22, [x0, #16]
	ldp x23, x24, [x0, #32]
	ldp x25, x26, [x0, #48]
	ldp x27, x28, [x0, #64]
	ldr x29, [x0, #80]
	ldr x9, [x0, #88]
	mov sp, x9
	ldr x30, [x0, #96]
	ldp d8, d9, [x0, #104]
	ldp d10, d11, [x0, #120]
	ldp d12, d13, [x0, #136]
	ldp d14, d15, [x0, #152]
	mov w0, #1
	ret

.globl SYM(nautilus_enterOnStack)
#if !defined(__APPLE__)
.type nautilus_enterOnStack, @function
#endif
.align 4
SYM(nautilus_enterOnStack):
	/* capture the caller's context exactly like nautilus_captureContext */
	stp x19, x20, [x0, #0]
	stp x21, x22, [x0, #16]
	stp x23, x24, [x0, #32]
	stp x25, x26, [x0, #48]
	stp x27, x28, [x0, #64]
	str x29, [x0, #80]
	mov x9, sp
	str x9, [x0, #88]
	str x30, [x0, #96]
	stp d8, d9, [x0, #104]
	stp d10, d11, [x0, #120]
	stp d12, d13, [x0, #136]
	stp d14, d15, [x0, #152]
	/* switch to the new stack; terminate the frame chain */
	and x9, x1, #-16
	mov sp, x9
	mov x29, #0
	mov x30, #0
	/* call entry(arg); entry must never return */
	mov x0, x3
	blr x2
	brk #0

#if !defined(__APPLE__)
.section .note.GNU-stack, "", @progbits
#endif

#endif /* __aarch64__ */

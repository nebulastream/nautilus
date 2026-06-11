/*
 * Minimal context capture/resume primitives for the stack-copy tracer (x86-64 SysV).
 *
 * CapturedContext layout (8-byte slots):
 *   0: rbx  1: rbp  2: r12  3: r13  4: r14  5: r15
 *   6: rsp (value after the capture call returns)
 *   7: rip (resume address = return address of the capture call)
 *   8: mxcsr (4 bytes) + x87 control word (2 bytes)
 */

#if defined(__x86_64__)

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
.align 16
SYM(nautilus_captureContext):
	movq %rbx, 0(%rdi)
	movq %rbp, 8(%rdi)
	movq %r12, 16(%rdi)
	movq %r13, 24(%rdi)
	movq %r14, 32(%rdi)
	movq %r15, 40(%rdi)
	leaq 8(%rsp), %rax
	movq %rax, 48(%rdi)
	movq (%rsp), %rax
	movq %rax, 56(%rdi)
	stmxcsr 64(%rdi)
	fnstcw 68(%rdi)
	xorl %eax, %eax
	ret

.globl SYM(nautilus_resumeContext)
#if !defined(__APPLE__)
.type nautilus_resumeContext, @function
#endif
.align 16
SYM(nautilus_resumeContext):
	movq 0(%rdi), %rbx
	movq 8(%rdi), %rbp
	movq 16(%rdi), %r12
	movq 24(%rdi), %r13
	movq 32(%rdi), %r14
	movq 40(%rdi), %r15
	ldmxcsr 64(%rdi)
	fldcw 68(%rdi)
	movq 48(%rdi), %rsp
	movl $1, %eax
	jmpq *56(%rdi)

.globl SYM(nautilus_enterOnStack)
#if !defined(__APPLE__)
.type nautilus_enterOnStack, @function
#endif
.align 16
SYM(nautilus_enterOnStack):
	/* capture the caller's context exactly like nautilus_captureContext */
	movq %rbx, 0(%rdi)
	movq %rbp, 8(%rdi)
	movq %r12, 16(%rdi)
	movq %r13, 24(%rdi)
	movq %r14, 32(%rdi)
	movq %r15, 40(%rdi)
	leaq 8(%rsp), %rax
	movq %rax, 48(%rdi)
	movq (%rsp), %rax
	movq %rax, 56(%rdi)
	stmxcsr 64(%rdi)
	fnstcw 68(%rdi)
	/* switch to the new stack; terminate the frame-pointer chain */
	movq %rsi, %rsp
	andq $-16, %rsp
	xorl %ebp, %ebp
	/* call entry(arg); entry must never return */
	movq %rcx, %rdi
	callq *%rdx
	ud2

#if !defined(__APPLE__)
.section .note.GNU-stack, "", @progbits
#endif

#endif /* __x86_64__ */

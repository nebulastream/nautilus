#pragma once

#include "nautilus/config.hpp"
#include "nautilus/tracing/Types.hpp"
#include <cstdint>
#include <vector>

// A NativeClosure hands an interpreted function to its caller as a real C
// function pointer. Two mechanisms implement that, selected at build time:
//
//   * dyncall callbacks (dcbNewCallback), the default. These write a trampoline
//     into RWX memory at runtime, which is forbidden on iOS.
//   * libffi closures built with static trampolines (NAUTILUS_FFI_CLOSURES): a
//     read-only code page plus a separate writable data page, so no runtime
//     PROT_EXEC. This is what makes the interpreting backends usable where
//     runtime codegen is banned.
//
// Both backends that need native entry points (bc and tbc) share this component,
// so the mechanism split lives here and nowhere else.
#ifdef NAUTILUS_FFI_CLOSURES
#include <ffi.h>
#else
#include <dyncall_args.h>
#include <dyncall_callback.h>
#endif

namespace nautilus::compiler {

/**
 * @brief Pull-style cursor over one call's incoming arguments.
 *
 * Arguments are read one at a time, in declaration order, and returned as raw
 * 64-bit register slots using the normalization both interpreters share: bools
 * are 0/1, narrow integers are zero-extended to their unsigned representation,
 * and floats/pointers keep their exact bit pattern in an otherwise-zero slot.
 *
 * The cursor is pull-style rather than an argument array so callers can read
 * straight into their own register file without staging through a temporary
 * buffer. Order matters: the dyncall implementation consumes a sequential
 * cursor, so `next` must be called exactly once per argument, in order.
 */
class ClosureArgs {
public:
#ifdef NAUTILUS_FFI_CLOSURES
	explicit ClosureArgs(void** args) : args_(args) {
	}
#else
	explicit ClosureArgs(DCArgs* args) : args_(args) {
	}
#endif

	/// Reads the next incoming argument as a normalized 64-bit register slot.
	uint64_t next(Type type);

private:
#ifdef NAUTILUS_FFI_CLOSURES
	void** args_;
	size_t index_ = 0;
#else
	DCArgs* args_;
#endif
};

/**
 * @brief The body a closure runs: reads its arguments from @p args, executes,
 *        and returns the raw 64-bit result slot (ignored for a void return).
 */
using ClosureHandler = uint64_t (*)(ClosureArgs& args, void* userdata);

/**
 * @brief One native-callable entry point for an interpreted function.
 *
 * Non-copyable and non-movable: the underlying closure stores a pointer to this
 * object, so its address must stay stable for as long as the function pointer
 * handed out by code() can be called.
 */
class NativeClosure {
public:
	NativeClosure(std::vector<Type> argTypes, Type returnType, ClosureHandler handler, void* userdata);
	~NativeClosure();

	NativeClosure(const NativeClosure&) = delete;
	NativeClosure& operator=(const NativeClosure&) = delete;
	NativeClosure(NativeClosure&&) = delete;
	NativeClosure& operator=(NativeClosure&&) = delete;

	/// The native function pointer. Callable with the signature described by the
	/// argument and return types this closure was built with.
	[[nodiscard]] void* code() const {
		return code_;
	}

	[[nodiscard]] const std::vector<Type>& argTypes() const {
		return argTypes_;
	}

	[[nodiscard]] Type returnType() const {
		return returnType_;
	}

	/// Runs the closure body. Public because the platform-specific closure
	/// handlers are free functions that reach the body through this object.
	uint64_t invokeBody(ClosureArgs& args) const {
		return handler_(args, userdata_);
	}

private:
	std::vector<Type> argTypes_;
	Type returnType_;
	ClosureHandler handler_;
	void* userdata_;
	void* code_ = nullptr;

#ifdef NAUTILUS_FFI_CLOSURES
	// The cif and its argument-type array must outlive the closure: ffi_prep_cif
	// stores a pointer into argFFITypes_, and the closure references the cif.
	ffi_cif cif_ {};
	std::vector<ffi_type*> argFFITypes_;
	ffi_closure* closure_ = nullptr;
#else
	DCCallback* callback_ = nullptr;
#endif
};

} // namespace nautilus::compiler

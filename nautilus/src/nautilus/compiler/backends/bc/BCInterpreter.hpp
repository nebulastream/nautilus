#pragma once

#include "nautilus/Executable.hpp"
#include "nautilus/compiler/backends/bc/ByteCode.hpp"
#include "nautilus/config.hpp"
#include <memory>
#include <unordered_map>
#include <vector>

// The bytecode backend hands each compiled function to its caller as a real C
// function pointer. The default mechanism is a dyncall callback (dcbNewCallback),
// which writes a trampoline into RWX memory at runtime. When NAUTILUS_BC_LIBFFI is
// defined the function pointer is instead a libffi closure built with static
// trampolines (no runtime-executable memory), which is what makes the backend
// usable on iOS where runtime codegen is banned. The two paths differ only at the
// closure boundary; the interpreter core is shared.
#ifdef NAUTILUS_BC_LIBFFI
#include <ffi.h>
#else
#include <dyncall_args.h>
#include <dyncall_callback.h>
#endif

namespace nautilus::compiler::bc {

/**
 * @brief Selects how the interpreter dispatches bytecode operations.
 *
 * Call:     indirect call through the OpTable function-pointer table (legacy default).
 * Switch:   an inlined switch over the opcode, which lets the compiler inline the
 *           operation bodies and keep the register base hot instead of paying a
 *           non-inlined indirect call per instruction.
 * Threaded: token-threaded dispatch via computed goto (labels-as-values). Each
 *           handler jumps directly to the next handler, giving the branch
 *           predictor a separate dispatch site per opcode. Falls back to Switch
 *           on compilers without the computed-goto extension (e.g. MSVC). This
 *           is still a pure interpreter — the label table is data, not generated
 *           machine code, so it remains usable where runtime codegen is banned.
 */
enum class DispatchMode { Call, Switch, Threaded };

/// Parse a "bc.dispatch" option value into a DispatchMode (defaults to Call on unknown input).
DispatchMode parseDispatchMode(const std::string& value);

/**
 * @brief Per-interpreter options resolved from engine options at compile time.
 *
 * dispatch:           how operations are dispatched (see DispatchMode).
 * reuseRegisterFile:  recycle the per-invocation register file from a thread-local
 *                     pool instead of heap-allocating a fresh copy each call.
 * superinstructions:  fuse compare+branch into a single op in the flattened
 *                     threaded stream (threaded path only).
 * immediates:         fold compile-time-constant operands directly into ops in the
 *                     flattened threaded stream (threaded path only).
 */
struct BCInterpreterOptions {
	DispatchMode dispatch = DispatchMode::Call;
	bool reuseRegisterFile = false;
	bool superinstructions = false;
	bool immediates = false;
};

/// Data passed to the closure handler for each function.
struct BCCallbackData {
	std::unique_ptr<class BCInterpreter> interpreter;
	std::vector<Type> argTypes;
	Type returnType;
#ifdef NAUTILUS_BC_LIBFFI
	// The cif and its argument-type array must outlive the closure: ffi_prep_cif
	// stores a pointer into argFFITypes, and the closure references the cif.
	ffi_cif cif {};
	std::vector<ffi_type*> argFFITypes;
	ffi_closure* closure = nullptr;
	void* code = nullptr;
#endif
};

#ifdef NAUTILUS_BC_LIBFFI
/// Handle to the per-function closure that owns the executable thunk; freed when
/// the executable is destroyed. libffi static-trampoline closure (iOS-safe).
using BCClosureHandle = ffi_closure*;
#else
/// dyncall callback handle (runtime-generated thunk).
using BCClosureHandle = DCCallback*;
#endif

/**
 * @brief Interprets a single bytecode function.
 */
class BCInterpreter {
public:
	BCInterpreter(Code code, RegisterFile registerFile, BCInterpreterOptions options = {});

#ifdef NAUTILUS_BC_LIBFFI
	/// Read arguments from a libffi closure's argument array (args[i] points to the
	/// i-th argument) into the register file, execute, and return the raw result.
	int64_t invoke(void** args, const std::vector<Type>& argTypes);
#else
	/// Read arguments from DCArgs directly into the register file, execute, and return the raw result.
	int64_t invoke(DCArgs* args, const std::vector<Type>& argTypes);
#endif

private:
	/// Shared invocation core: set up the per-invocation register file (recycled or
	/// fresh) and alloca buffers, load arguments via @p reader, then execute. The
	/// @p reader is called once per argument as reader(regs, reg, type, index) and is
	/// the only part that differs between the dyncall and libffi entry points, so the
	/// register-file/alloca setup and execution stay identical across both.
	template <class Reader>
	int64_t invokeImpl(const std::vector<Type>& argTypes, Reader reader);

	int64_t execute(RegisterFile& regs) const;

	/// Computed-goto (token-threaded) execution path. Only defined on compilers
	/// that support labels-as-values; execute() routes here only when supported.
	int64_t executeThreaded(RegisterFile& regs) const;

	/// Build the flattened instruction stream used by the threaded path: all block
	/// operations concatenated in order, each block followed by its terminator
	/// translated into a JMP/CJMP/RET pseudo-opcode. Called once at construction
	/// when the threaded path is active.
	void buildFlatCode();

	Code code;
	RegisterFile registerFile;
	BCInterpreterOptions options;

	/// Flattened form of `code` for the threaded path (empty otherwise). Reuses
	/// OpCode for value ops; terminators are JMP/CJMP/RET whose targets are block
	/// indices resolved to flat offsets via blockStart_.
	std::vector<OpCode> flatCode_;
	std::vector<int32_t> blockStart_;
};

/**
 * @brief Executable that wraps all BC functions as dyncallback thunks.
 *
 * Each function (including the main "execute" function) is lowered to bytecode,
 * wrapped in a BCInterpreter, and exposed via a dyncallback. The main function's
 * callback is returned from getInvocableFunctionPtr("execute").
 */
class BCExecutable : public Executable {
public:
	BCExecutable(std::unordered_map<std::string, void*> functionPtrs,
	             std::vector<std::unique_ptr<BCCallbackData>> callbackData, std::vector<BCClosureHandle> callbacks);

	~BCExecutable() override;

	void* getInvocableFunctionPtr(const std::string& member) override;

	bool hasInvocableFunctionPtr() override;

private:
	std::unordered_map<std::string, void*> functionPtrs_;
	std::vector<std::unique_ptr<BCCallbackData>> callbackData_;
	std::vector<BCClosureHandle> callbacks_;
};

} // namespace nautilus::compiler::bc

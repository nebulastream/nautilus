#pragma once

#include "nautilus/Executable.hpp"
#include "nautilus/compiler/backends/NativeClosure.hpp"
#include "nautilus/compiler/backends/bc/ByteCode.hpp"
#include "nautilus/config.hpp"
#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

// The bytecode backend hands each compiled function to its caller as a real C
// function pointer, built by the shared NativeClosure component (see
// backends/NativeClosure.hpp), which also owns the dyncall-callback vs.
// libffi-static-trampoline choice. The interpreter core is independent of it.

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
};

/**
 * @brief Interprets a single bytecode function.
 */
class BCInterpreter {
public:
	BCInterpreter(Code code, RegisterFile registerFile, BCInterpreterOptions options = {});

	/// Read the incoming arguments straight into the register file, execute, and
	/// return the raw result.
	int64_t invoke(ClosureArgs& args, const std::vector<Type>& argTypes);

private:
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
 * @brief Executable that wraps all BC functions as native closures.
 *
 * Each function (including the main "execute" function) is lowered to bytecode,
 * wrapped in a BCInterpreter, and exposed through a NativeClosure. The main
 * function's closure pointer is returned from getInvocableFunctionPtr("execute").
 */
class BCExecutable : public Executable {
public:
	/// @param functionsNeedingCapture names of the module's functions whose
	///        compiled body has at least one captured-exception call site (see
	///        CapturedExceptionTransport::functionsNeedingCapture). A function
	///        not in this set is reported as NativeUnwind: it never touches
	///        the ExceptionFrame machinery, so it is safe to call directly.
	BCExecutable(std::unordered_map<std::string, void*> functionPtrs,
	             std::vector<std::unique_ptr<BCCallbackData>> callbackData,
	             std::vector<std::unique_ptr<NativeClosure>> closures,
	             std::unordered_set<std::string> functionsNeedingCapture);

	void* getInvocableFunctionPtr(const std::string& member) override;

	bool hasInvocableFunctionPtr() override;

	[[nodiscard]] ExceptionPropagationMode getExceptionPropagationMode() const override {
		return ExceptionPropagationMode::CapturedHostRethrow;
	}

	[[nodiscard]] ExceptionPropagationMode getExceptionPropagationMode(const std::string& member) const override {
		return functionsNeedingCapture_.contains(member) ? ExceptionPropagationMode::CapturedHostRethrow
		                                                 : ExceptionPropagationMode::NativeUnwind;
	}

private:
	std::unordered_map<std::string, void*> functionPtrs_;
	std::vector<std::unique_ptr<BCCallbackData>> callbackData_;
	std::vector<std::unique_ptr<NativeClosure>> closures_;
	std::unordered_set<std::string> functionsNeedingCapture_;
};

} // namespace nautilus::compiler::bc

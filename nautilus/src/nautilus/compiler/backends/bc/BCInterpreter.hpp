#pragma once

#include "nautilus/Executable.hpp"
#include "nautilus/compiler/backends/bc/ByteCode.hpp"
#include <dyncall_args.h>
#include <dyncall_callback.h>
#include <memory>
#include <unordered_map>
#include <vector>

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

/// Data passed to the dyncallback handler for each function.
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
	BCInterpreter(Code code, RegisterFile registerFile, DispatchMode dispatchMode = DispatchMode::Call);

	/// Read arguments from DCArgs directly into the register file, execute, and return the raw result.
	int64_t invoke(DCArgs* args, const std::vector<Type>& argTypes);

private:
	int64_t execute(RegisterFile& regs) const;

	/// Computed-goto (token-threaded) execution path. Only defined on compilers
	/// that support labels-as-values; execute() routes here only when supported.
	int64_t executeThreaded(RegisterFile& regs) const;

	Code code;
	RegisterFile registerFile;
	DispatchMode dispatchMode;
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
	             std::vector<std::unique_ptr<BCCallbackData>> callbackData, std::vector<DCCallback*> callbacks);

	~BCExecutable() override;

	void* getInvocableFunctionPtr(const std::string& member) override;

	bool hasInvocableFunctionPtr() override;

private:
	std::unordered_map<std::string, void*> functionPtrs_;
	std::vector<std::unique_ptr<BCCallbackData>> callbackData_;
	std::vector<DCCallback*> callbacks_;
};

} // namespace nautilus::compiler::bc

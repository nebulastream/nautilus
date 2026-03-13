#pragma once

#include "nautilus/Executable.hpp"
#include "nautilus/compiler/backends/bc/ByteCode.hpp"
#include <dyncall_args.h>
#include <dyncall_callback.h>
#include <memory>
#include <unordered_map>
#include <vector>

namespace nautilus::compiler::bc {

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
	BCInterpreter(Code code, RegisterFile registerFile);

	/// Read arguments from DCArgs directly into the register file, execute, and return the raw result.
	int64_t invoke(DCArgs* args, const std::vector<Type>& argTypes);

private:
	int64_t execute(RegisterFile& regs) const;

	Code code;
	RegisterFile registerFile;
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

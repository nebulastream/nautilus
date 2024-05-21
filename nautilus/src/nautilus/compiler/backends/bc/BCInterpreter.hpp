#pragma once

#include "nautilus/Executable.hpp"
#include "nautilus/compiler/backends/bc/ByteCode.hpp"

namespace nautilus::compiler::bc {

/**
 * @brief Implements the interpreter for an specific executable bytecode fragment
 */
class BCInterpreter : public Executable {
public:
	/**
	 * Constructor to create a bytecode interpreter.
	 */
	BCInterpreter(Code code, RegisterFile registerFile);

	~BCInterpreter() override = default;

public:
	void* getInvocableFunctionPtr(const std::string& member) override;

	bool hasInvocableFunctionPtr() override;

	std::unique_ptr<GenericInvocable> getGenericInvocable(const std::string& string) override;

	std::any invokeGeneric(const std::vector<std::any>& arguments);

private:
	int64_t execute(RegisterFile& regs) const;

	Code code;
	RegisterFile registerFile;
};
} // namespace nautilus::compiler::bc

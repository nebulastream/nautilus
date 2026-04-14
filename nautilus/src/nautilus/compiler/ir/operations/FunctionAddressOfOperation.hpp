
#pragma once

#include "nautilus/compiler/ir/operations/Operation.hpp"
#include <string>

namespace nautilus::compiler::ir {

/// IR operation representing the address of a named function as a pointer value.
/// Used to get function pointers for Nautilus functions that can be passed to runtime calls.
class FunctionAddressOfOperation : public Operation {
public:
	FunctionAddressOfOperation(const std::string& functionSymbol, const std::string& functionName, void* functionPtr,
	                           OperationIdentifier identifier);

	~FunctionAddressOfOperation() override = default;

	const std::string& getFunctionSymbol() const;
	const std::string& getFunctionName() const;
	void* getFunctionPtr();

	static bool classof(const Operation* op);

private:
	const std::string functionSymbol;
	const std::string functionName;
	void* functionPtr;
};

} // namespace nautilus::compiler::ir

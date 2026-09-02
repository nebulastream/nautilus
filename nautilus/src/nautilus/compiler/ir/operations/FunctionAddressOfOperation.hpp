
#pragma once

#include "nautilus/compiler/ir/FunctionTable.hpp"
#include "nautilus/compiler/ir/operations/Operation.hpp"
#include <string>

namespace nautilus::compiler::ir {

/// IR operation representing the address of a named function as a pointer value.
/// Used to get function pointers for Nautilus functions that can be passed to runtime calls.
class FunctionAddressOfOperation : public Operation {
public:
	FunctionAddressOfOperation(common::Arena& arena, const std::string& functionSymbol, const std::string& functionName,
	                           void* functionPtr, OperationIdentifier identifier,
	                           FunctionId calleeId = INVALID_FUNCTION_ID);

	~FunctionAddressOfOperation() = default;

	const std::string& getFunctionSymbol() const;
	const std::string& getFunctionName() const;
	void* getFunctionPtr();

	/// The target's entry in the module function table. Unlike the stored
	/// pointer, this resolves for every linkage -- including externals, whose
	/// address could not previously be taken at all.
	[[nodiscard]] FunctionId getCalleeId() const {
		return calleeId;
	}

	static bool classof(const Operation* op);

private:
	const std::string functionSymbol;
	const std::string functionName;
	void* functionPtr;
	/// Defaults to the sentinel so the arena-constructing overloads that do
	/// not take one leave a value the verifier can flag, rather than garbage.
	FunctionId calleeId = INVALID_FUNCTION_ID;
};

} // namespace nautilus::compiler::ir

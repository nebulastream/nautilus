
#pragma once

#include "nautilus/common/FunctionAttributes.hpp"
#include "nautilus/compiler/ir/operations/Operation.hpp"
#include <vector>

namespace nautilus::compiler::ir {

/// IR operation representing a call through a runtime function pointer value.
///
/// Unlike ProxyCallOperation (which embeds the target address as a constant),
/// IndirectCallOperation takes the function pointer as an SSA operand — the
/// first element of inputs[] — followed by the call arguments.
class IndirectCallOperation : public Operation {
public:
	IndirectCallOperation(OperationIdentifier identifier, Operation* functionPtrOperand,
	                      std::vector<Operation*> inputArguments, Type resultType, FunctionAttributes fnAttrs);

	~IndirectCallOperation() override = default;

	/// The SSA operand that holds the runtime function pointer (inputs[0]).
	Operation* getFunctionPtrOperand() const;

	/// The call arguments (inputs[1..]).
	std::vector<Operation*> getInputArguments() const;

	[[nodiscard]] const FunctionAttributes& getFunctionAttributes() const;

	static bool classof(const Operation* op);

private:
	FunctionAttributes fnAttrs;
};

} // namespace nautilus::compiler::ir

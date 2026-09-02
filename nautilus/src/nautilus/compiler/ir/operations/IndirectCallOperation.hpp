
#pragma once

#include "nautilus/common/FunctionAttributes.hpp"
#include "nautilus/compiler/ir/operations/Operation.hpp"
#include <span>
#include <string>
#include <vector>

namespace nautilus::compiler::ir {

/// IR operation representing a call through a runtime function pointer value.
///
/// Unlike CallOperation (which embeds the target address as a constant),
/// IndirectCallOperation takes the function pointer as an SSA operand — the
/// first element of inputs[] — followed by the call arguments.
class IndirectCallOperation : public Operation {
public:
	struct Destructor {
		Operation* address;
		std::string functionSymbol;
		std::string functionName;
		void* functionPtr;
	};

	IndirectCallOperation(common::Arena& arena, OperationIdentifier identifier, Operation* functionPtrOperand,
	                      std::span<Operation* const> inputArguments, Type resultType, FunctionAttributes fnAttrs);

	IndirectCallOperation(common::Arena& arena, OperationIdentifier identifier, Operation* functionPtrOperand,
	                      std::span<Operation* const> inputArguments, Type resultType, FunctionAttributes fnAttrs,
	                      std::vector<Destructor> destructors, bool exceptionHandling, void* captureFunc = nullptr);

	~IndirectCallOperation() = default;

	/// The SSA operand that holds the runtime function pointer (inputs[0]).
	Operation* getFunctionPtrOperand() const;

	/// The call arguments (inputs[1..]).
	std::span<Operation* const> getInputArguments() const;

	[[nodiscard]] const FunctionAttributes& getFunctionAttributes() const;
	[[nodiscard]] const std::vector<Destructor>& getDestructors() const;
	[[nodiscard]] bool requiresExceptionHandling() const;
	/// Capture wrapper (`captureThrowingCall<R, Args...>`) for a potentially
	/// throwing call, or nullptr for `noUnwind` calls.
	[[nodiscard]] void* getCaptureFunc() const;

	static bool classof(const Operation* op);

private:
	FunctionAttributes fnAttrs;
	std::vector<Destructor> destructors;
	void* captureFunc = nullptr;
	bool exceptionHandling = false;
};

} // namespace nautilus::compiler::ir

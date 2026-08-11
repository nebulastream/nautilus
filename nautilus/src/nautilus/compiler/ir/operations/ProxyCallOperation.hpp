
#include "nautilus/common/ExceptionCleanup.hpp"
#include "nautilus/compiler/ir/operations/Operation.hpp"
#include <optional>
#include <span>
#include <string>
#include <vector>

namespace nautilus::compiler::ir {
class ProxyCallOperation : public Operation {
public:
	ProxyCallOperation(common::Arena& arena, OperationIdentifier identifier, std::span<Operation* const> inputArguments,
	                   Type resultType, CallKind callKind = CallKind::Regular,
	                   std::optional<CleanupStateId> cleanupState = std::nullopt);

	ProxyCallOperation(common::Arena& arena, const std::string& functionSymbol, const std::string& functionName,
	                   void* functionPtr, OperationIdentifier identifier, std::span<Operation* const> inputArguments,
	                   Type resultType, FunctionAttributes fnAttrs, CallKind callKind = CallKind::Regular,
	                   std::optional<ExceptionCaptureSpec> exceptionCapture = std::nullopt,
	                   std::optional<CleanupStateId> cleanupState = std::nullopt);

	~ProxyCallOperation() = default;

	std::span<Operation* const> getInputArguments() const;

	void setInputArguments(common::Arena& arena, std::span<Operation* const> newInputArguments);

	const std::string& getFunctionSymbol() const;
	const std::string& getFunctionName() const;
	void* getFunctionPtr();

	[[nodiscard]] const FunctionAttributes& getFunctionAttributes() const;
	[[nodiscard]] CallKind getCallKind() const;
	[[nodiscard]] const std::optional<CleanupStateId>& getCleanupState() const;
	[[nodiscard]] const std::optional<ExceptionCaptureSpec>& getExceptionCapture() const;

	static bool classof(const Operation* op);

private:
	const std::string mangedFunctionSymbol;
	const std::string functionName;
	void* functionPtr;
	FunctionAttributes fnAttrs;
	CallKind callKind;
	std::optional<ExceptionCaptureSpec> exceptionCapture;
	std::optional<CleanupStateId> cleanupState;
};
} // namespace nautilus::compiler::ir

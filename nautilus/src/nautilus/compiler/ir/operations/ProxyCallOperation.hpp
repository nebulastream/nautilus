
#include "nautilus/common/FunctionAttributes.hpp"
#include "nautilus/compiler/ir/operations/Operation.hpp"
#include <span>
#include <string>
#include <vector>

namespace nautilus::compiler::ir {
class ProxyCallOperation : public Operation {
public:
	struct Destructor {
		Operation* address;
		std::string functionSymbol;
		std::string functionName;
		void* functionPtr;
	};

	ProxyCallOperation(common::Arena& arena, OperationIdentifier identifier, std::span<Operation* const> inputArguments,
	                   Type resultType);

	ProxyCallOperation(common::Arena& arena, const std::string& functionSymbol, const std::string& functionName,
	                   void* functionPtr, OperationIdentifier identifier, std::span<Operation* const> inputArguments,
	                   Type resultType, FunctionAttributes fnAttrs, std::vector<Destructor> destructors = {},
	                   bool exceptionHandling = false, void* captureFunc = nullptr);

	~ProxyCallOperation() = default;

	std::span<Operation* const> getInputArguments() const;

	void setInputArguments(common::Arena& arena, std::span<Operation* const> newInputArguments);

	const std::string& getFunctionSymbol() const;
	const std::string& getFunctionName() const;
	void* getFunctionPtr();
	/// Capture wrapper (`captureThrowingCall<R, Args...>`) for a potentially
	/// throwing call, or nullptr for `noUnwind` calls.
	void* getCaptureFunc() const;

	[[nodiscard]] const FunctionAttributes& getFunctionAttributes() const;
	[[nodiscard]] const std::vector<Destructor>& getDestructors() const;
	[[nodiscard]] bool requiresExceptionHandling() const;

	static bool classof(const Operation* op);

private:
	const std::string mangedFunctionSymbol;
	const std::string functionName;
	void* functionPtr;
	void* captureFunc;
	FunctionAttributes fnAttrs;
	std::vector<Destructor> destructors;
	bool exceptionHandling = false;
};
} // namespace nautilus::compiler::ir

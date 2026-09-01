
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
	                   bool exceptionHandling = false, void* captureFunc = nullptr, bool isNautilusCall = false);

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

	/// True for a call into another traced Nautilus function (via
	/// NautilusFunction), false for a raw invoke() into an external
	/// function. Set from the trace-level `FunctionCall::isNautilusCall`
	/// marker; see NoThrowInferencePass for why this can't be inferred by
	/// matching function names instead.
	[[nodiscard]] bool isNautilusFunctionCall() const;

	/// Downgrades this call to noUnwind: clears the exception-handling flag
	/// and destructor list and marks `fnAttrs.noUnwind`. Called only by
	/// NoThrowInferencePass once whole-module analysis proves the callee
	/// cannot throw; never call this on a call whose callee might actually
	/// unwind.
	void markNoThrow();

	static bool classof(const Operation* op);

private:
	const std::string mangedFunctionSymbol;
	const std::string functionName;
	void* functionPtr;
	void* captureFunc;
	FunctionAttributes fnAttrs;
	std::vector<Destructor> destructors;
	bool exceptionHandling = false;
	bool isNautilusCall = false;
};
} // namespace nautilus::compiler::ir

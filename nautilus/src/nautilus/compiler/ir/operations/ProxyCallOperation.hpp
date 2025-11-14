
#include "nautilus/common/FunctionAttributes.hpp"
#include "nautilus/compiler/ir/operations/Operation.hpp"
#include <string>
#include <vector>

namespace nautilus::compiler::ir {
class ProxyCallOperation : public Operation {
public:
	ProxyCallOperation(OperationIdentifier identifier, const std::vector<Operation*>& inputArguments, Type resultType);

	ProxyCallOperation(const std::string& functionSymbol, const std::string& functionName, void* functionPtr,
	                   OperationIdentifier identifier, std::vector<Operation*> inputArguments, Type resultType,
	                   FunctionAttributes fnAttrs);

	~ProxyCallOperation() override = default;

	const std::vector<Operation*>& getInputArguments() const;

	void setInputArguments(std::vector<Operation*>& newInputArguments);

	const std::string& getFunctionSymbol() const;
	const std::string& getFunctionName() const;
	void* getFunctionPtr();

	[[nodiscard]] const FunctionAttributes& getFunctionAttributes() const;

private:
	const std::string mangedFunctionSymbol;
	const std::string functionName;
	void* functionPtr;
	FunctionAttributes fnAttrs;
};
} // namespace nautilus::compiler::ir

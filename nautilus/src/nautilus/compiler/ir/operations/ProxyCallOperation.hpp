
#include "nautilus/compiler/ir/operations/Operation.hpp"
#include <vector>

namespace nautilus::compiler::ir {
class ProxyCallOperation : public Operation {
public:
	ProxyCallOperation(OperationIdentifier identifier, const std::vector<Operation*>& inputArguments, Type resultType);

	ProxyCallOperation(const std::string& functionSymbol, const std::string& functionName, void* functionPtr, OperationIdentifier identifier, std::vector<Operation*> inputArguments, Type resultType);

	~ProxyCallOperation() override = default;

	const std::vector<Operation*>& getInputArguments() const;

	void setInputArguments(std::vector<Operation*>& newInputArguments);

	const std::string& getFunctionSymbol();
	const std::string& getFunctionName();

	std::string toString() override;

	void* getFunctionPtr();

private:
	const std::string mangedFunctionSymbol;
	const std::string functionName;
	void* functionPtr;
};
} // namespace nautilus::compiler::ir

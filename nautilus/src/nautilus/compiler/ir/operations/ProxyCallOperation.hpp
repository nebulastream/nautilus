
#include "nautilus/compiler/ir/operations/Operation.hpp"
#include <vector>

namespace nautilus::compiler::ir {
class ProxyCallOperation : public Operation {
public:
	ProxyCallOperation(OperationIdentifier identifier, const std::vector<Operation*>& inputArguments, Type resultType);

	ProxyCallOperation(std::string mangedFunctionSymbol, void* functionPtr, OperationIdentifier identifier,
	                   std::vector<Operation*> inputArguments, Type resultType);

	~ProxyCallOperation() override = default;

	const std::vector<Operation*>& getInputArguments() const;

	void setInputArguments(std::vector<Operation*>& newInputArguments);

	std::string getFunctionSymbol();

	std::string toString() override;

	void* getFunctionPtr();

private:
	std::string mangedFunctionSymbol;
	void* functionPtr;
};
} // namespace nautilus::compiler::ir

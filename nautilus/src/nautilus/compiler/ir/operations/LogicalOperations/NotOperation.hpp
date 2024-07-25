
#include "nautilus/compiler/ir/operations/Operation.hpp"

namespace nautilus::compiler::ir {

class NotOperation : public Operation {
public:
	NotOperation(OperationIdentifier identifier, Operation* input);

	~NotOperation() override = default;

	Operation* getInput();

	void setInput(Operation* newInput);

	std::string toString() override;

	bool classof(const Operation* Op);
};
} // namespace nautilus::compiler::ir

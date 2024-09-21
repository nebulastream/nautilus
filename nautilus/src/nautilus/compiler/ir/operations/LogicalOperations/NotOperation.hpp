
#include "nautilus/compiler/ir/operations/Operation.hpp"

namespace nautilus::compiler::ir {

class NotOperation : public Operation {
public:
	NotOperation(OperationIdentifier identifier, Operation* input);

	~NotOperation() override = default;

	Operation* getInput() const;

	void setInput(Operation* newInput);

	bool classof(const Operation* Op);
};
} // namespace nautilus::compiler::ir

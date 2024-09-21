
#include "nautilus/compiler/ir/operations/Operation.hpp"

namespace nautilus::compiler::ir {

class NegateOperation : public Operation {
public:
	NegateOperation(OperationIdentifier identifier, Operation* input);

	~NegateOperation() override = default;

	Operation* getInput() const;

	void setInput(Operation* newInput);

	bool classof(const Operation* Op);
};
} // namespace nautilus::compiler::ir

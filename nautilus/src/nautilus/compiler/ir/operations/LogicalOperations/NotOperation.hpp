
#include "nautilus/compiler/ir/operations/Operation.hpp"

namespace nautilus::compiler::ir {

class NotOperation : public Operation {
public:
	NotOperation(common::Arena& arena, OperationIdentifier identifier, Operation* input);

	~NotOperation() = default;

	Operation* getInput() const;

	void setInput(Operation* newInput);

	static bool classof(const Operation* Op);
};
} // namespace nautilus::compiler::ir

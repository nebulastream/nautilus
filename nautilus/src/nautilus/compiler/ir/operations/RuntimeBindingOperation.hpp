#pragma once

#include "nautilus/RuntimeBindingInfo.hpp"
#include "nautilus/compiler/ir/operations/Operation.hpp"

namespace nautilus::compiler::ir {

class RuntimeBindingOperation : public Operation {
public:
	RuntimeBindingOperation(common::Arena& arena, OperationIdentifier identifier,
	                        const runtime_binding::Entry& binding);

	const runtime_binding::Entry& getBinding() const;

	static bool classof(const Operation* op);

private:
	const runtime_binding::Entry binding;
};

} // namespace nautilus::compiler::ir


#pragma once

#include "nautilus/tracing/Types.hpp"
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace nautilus::compiler::ir {

class OperationIdentifier {
public:
	OperationIdentifier(uint32_t id);

	bool operator==(const OperationIdentifier& rhs) const;

	bool operator!=(const OperationIdentifier& rhs) const;

	friend std::hash<OperationIdentifier>;

	std::string toString() const;

	bool operator<(const OperationIdentifier& rhs) const;

	bool operator>(const OperationIdentifier& rhs) const;

	bool operator<=(const OperationIdentifier& rhs) const;

	bool operator>=(const OperationIdentifier& rhs) const;

	uint32_t getId() const;

private:
	uint32_t id;
};

class Operation {
public:
	enum class OperationType : uint8_t {
		AddOp,
		AndOp,
		NotOp,
		BasicBlockArgument,
		BlockInvocation,
		BranchOp,
		ConstIntOp,
		ConstBooleanOp,
		ConstPtrOp,
		ConstFloatOp,
		CastOp,
		CompareOp,
		DivOp,
		ModOp,
		FunctionOp,
		IfOp,
		LoopOp,
		LoadOp,
		MulOp,
		MLIR_YIELD,
		NegateOp,
		OrOp,
		ProxyCallOp,
		ReturnOp,
		StoreOp,
		SubOp,
		BinaryComp,
		ShiftOp,
	};

	explicit Operation(OperationType opType, const OperationIdentifier& identifier, Type type,
	                   const std::vector<Operation*>& inputs = {});

	explicit Operation(OperationType opType, Type type, const std::vector<Operation*>& inputs = {});

	virtual ~Operation() noexcept;

	const OperationIdentifier& getIdentifier() const;

	OperationType getOperationType() const;

	// std::string getOperationTypeAsString() const;
	const Type& getStamp() const;

	bool isConstOperation() const;

	std::vector<Operation*> getInputs();

	void replaceInput(Operation* toReplace, std::shared_ptr<Operation> replaceWith);

	template <typename OP>
	const OP* dynCast() const {
		return dynamic_cast<const OP*>(this);
	}

protected:
	const OperationType opType;
	const OperationIdentifier identifier;
	const Type stamp;
	std::vector<Operation*> inputs;
};

template <typename T>
T* as(const Operation* op) {
	return static_cast<T*>(op);
}

template <typename T>
T* as(const std::unique_ptr<Operation>& op) {
	return static_cast<T*>(op.get());
}

class BinaryOperation : public Operation {
public:
	BinaryOperation(OperationType opType, const OperationIdentifier& identifier, Type type, Operation* left,
	                Operation* right);

	Operation* getLeftInput() const;

	Operation* getRightInput() const;

	void setLeftInput(Operation* newLeftInput);

	void setRightInput(Operation* newRightInput);
};

} // namespace nautilus::compiler::ir

namespace std {
template <>
struct hash<nautilus::compiler::ir::OperationIdentifier> {
	std::size_t operator()(const nautilus::compiler::ir::OperationIdentifier& k) const {
		using std::hash;
		using std::size_t;
		return hash<uint16_t>()(k.id);
	}
};
} // namespace std

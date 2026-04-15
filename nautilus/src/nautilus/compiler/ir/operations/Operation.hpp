
#pragma once

#include "nautilus/tracing/Types.hpp"
#include <compare>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace nautilus::compiler::ir {

class OperationIdentifier {
public:
	constexpr OperationIdentifier(uint32_t id) : id(id) {
	}

	constexpr auto operator<=>(const OperationIdentifier&) const = default;
	constexpr bool operator==(const OperationIdentifier&) const = default;

	[[nodiscard]] std::string toString() const;

	[[nodiscard]] constexpr uint32_t getId() const {
		return id;
	}

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
		LoadOp,
		MulOp,
		MLIR_YIELD,
		NegateOp,
		OrOp,
		ProxyCallOp,
		IndirectCallOp,
		ReturnOp,
		SelectOp,
		StoreOp,
		SubOp,
		BinaryComp,
		ShiftOp,
		AllocaOp,
		FunctionAddressOfOp,
	};

	explicit Operation(OperationType opType, OperationIdentifier identifier, Type type,
	                   const std::vector<Operation*>& inputs = {});

	explicit Operation(OperationType opType, Type type, const std::vector<Operation*>& inputs = {});

	virtual ~Operation() noexcept;

	const OperationIdentifier& getIdentifier() const;

	OperationType getOperationType() const;

	// std::string getOperationTypeAsString() const;
	const Type& getStamp() const;

	bool isConstOperation() const;

	const std::vector<Operation*>& getInputs() const;

	template <typename OP>
	const OP* dynCast() const {
		return OP::classof(this) ? static_cast<const OP*>(this) : nullptr;
	}

protected:
	const OperationType opType;
	const OperationIdentifier identifier;
	const Type stamp;
	std::vector<Operation*> inputs;
};

/**
 * @brief LLVM-style tag-based type tests. Every Operation subclass must expose
 * `static bool classof(const Operation*)` returning true for the op types it
 * represents. These helpers then avoid RTTI / dynamic_cast entirely.
 */
template <typename T>
bool isa(const Operation* op) {
	return op != nullptr && T::classof(op);
}

template <typename T>
const T* dyn_cast(const Operation* op) {
	return isa<T>(op) ? static_cast<const T*>(op) : nullptr;
}

template <typename T>
T* dyn_cast(Operation* op) {
	return isa<T>(op) ? static_cast<T*>(op) : nullptr;
}

template <typename T>
const T* cast(const Operation* op) {
	return static_cast<const T*>(op);
}

template <typename T>
T* cast(Operation* op) {
	return static_cast<T*>(op);
}

template <typename T>
T* as(Operation* op) {
	return static_cast<T*>(op);
}

template <typename T>
const T* as(const Operation* op) {
	return static_cast<const T*>(op);
}

class BinaryOperation : public Operation {
public:
	BinaryOperation(OperationType opType, OperationIdentifier identifier, Type type, Operation* left, Operation* right);

	Operation* getLeftInput() const;

	Operation* getRightInput() const;

	void setLeftInput(Operation* newLeftInput);

	void setRightInput(Operation* newRightInput);

	static bool classof(const Operation* op);
};

} // namespace nautilus::compiler::ir

namespace std {
template <>
struct hash<nautilus::compiler::ir::OperationIdentifier> {
	std::size_t operator()(nautilus::compiler::ir::OperationIdentifier k) const noexcept {
		return std::hash<uint32_t> {}(k.getId());
	}
};
} // namespace std

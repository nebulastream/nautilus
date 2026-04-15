
#pragma once

#include "nautilus/common/Arena.hpp"
#include "nautilus/tracing/Types.hpp"
#include <algorithm>
#include <compare>
#include <cstdint>
#include <initializer_list>
#include <memory>
#include <span>
#include <string>

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

/**
 * @brief Base class for all IR operations.
 *
 * Operation inputs are stored as a raw pointer / count pair into an
 * Operation* array that is allocated from the surrounding `common::Arena`.
 * The pointer is stable for the lifetime of the arena (the same lifetime
 * the Operation itself enjoys), so storing inputs out-of-line costs the
 * IR no extra cache pressure compared to a `std::vector` while removing
 * the per-operation heap allocation, capacity word, and dynamic
 * destruction the vector previously required.
 *
 * Operation has a non-virtual destructor: every derived type knows its
 * own static type at construction time (the arena's `create<T>` call
 * captures it via the destroyThunk), so polymorphic deletion through
 * `Operation*` is never needed. Removing the virtual destructor lets
 * the simple, fixed-arity Operation subclasses (the vast majority:
 * Add, Sub, Cast, Load, Store, Const*, ...) become trivially
 * destructible — the arena then skips registering destructors for
 * them entirely, which both saves memory and accelerates `softReset`.
 */
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

	/// Constructs an Operation that has no SSA inputs.
	Operation(OperationType opType, OperationIdentifier identifier, Type type) noexcept
	    : opType(opType), identifier(identifier), stamp(type), inputs(nullptr), numInputs(0) {
	}

	/// Convenience constructor (no identifier, no inputs).
	Operation(OperationType opType, Type type) noexcept : Operation(opType, OperationIdentifier {0}, type) {
	}

	/// Constructs an Operation whose inputs are copied into a freshly
	/// arena-allocated array.
	Operation(common::Arena& arena, OperationType opType, OperationIdentifier identifier, Type type,
	          std::initializer_list<Operation*> ins)
	    : opType(opType), identifier(identifier), stamp(type), inputs(allocateInputs(arena, ins.size())),
	      numInputs(static_cast<uint32_t>(ins.size())) {
		std::copy(ins.begin(), ins.end(), inputs);
	}

	/// Variable-length input constructor (used by call-like operations).
	Operation(common::Arena& arena, OperationType opType, OperationIdentifier identifier, Type type,
	          std::span<Operation* const> ins)
	    : opType(opType), identifier(identifier), stamp(type), inputs(allocateInputs(arena, ins.size())),
	      numInputs(static_cast<uint32_t>(ins.size())) {
		std::copy(ins.begin(), ins.end(), inputs);
	}

	/// Non-virtual destructor (see class comment).
	~Operation() = default;

	const OperationIdentifier& getIdentifier() const {
		return identifier;
	}

	OperationType getOperationType() const {
		return opType;
	}

	const Type& getStamp() const {
		return stamp;
	}

	bool isConstOperation() const;

	/// Returns a non-owning view over the SSA inputs of this operation.
	std::span<Operation* const> getInputs() const noexcept {
		return {inputs, numInputs};
	}

	template <typename OP>
	const OP* dynCast() const {
		return OP::classof(this) ? static_cast<const OP*>(this) : nullptr;
	}

protected:
	/// Allocates a fresh, uninitialised Operation* array of the requested
	/// size from @p arena.  Returns nullptr when @p count is zero so the
	/// arena does not get hit with an empty allocation.
	static Operation** allocateInputs(common::Arena& arena, std::size_t count) {
		if (count == 0) {
			return nullptr;
		}
		return static_cast<Operation**>(arena.allocate(sizeof(Operation*) * count, alignof(Operation*)));
	}

	Operation* getInput(std::size_t i) const {
		return inputs[i];
	}

	void setInput(std::size_t i, Operation* op) {
		inputs[i] = op;
	}

	const OperationType opType;
	const OperationIdentifier identifier;
	const Type stamp;
	Operation** inputs;
	uint32_t numInputs;
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
	BinaryOperation(common::Arena& arena, OperationType opType, OperationIdentifier identifier, Type type,
	                Operation* left, Operation* right);

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

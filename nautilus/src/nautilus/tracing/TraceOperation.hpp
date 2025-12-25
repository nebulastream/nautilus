
#pragma once

#include "nautilus/tracing/Snapshot.hpp"
#include "nautilus/tracing/TracingUtil.hpp"
#include "nautilus/tracing/tag/Tag.hpp"
#include <any>
#include <nautilus/common/FunctionAttributes.hpp>
#include <type_traits>
#include <utility>
#include <variant>
#include <vector>

namespace nautilus::tracing {

class None {};
using BranchProbability = double;

/**
 * @brief Represents a function call operation in the trace.
 *
 * @warning The ptr member is a non-owning raw pointer to the function being called.
 * It must remain valid for the lifetime of the trace and any compiled executable.
 * The caller is responsible for ensuring the pointed-to function is not unloaded
 * or deallocated while the trace or executable is in use.
 */
struct FunctionCall {
	std::string functionName;
	std::string mangledName;
	/**
	 * @brief Non-owning pointer to the function being called.
	 * @warning Must remain valid for the lifetime of the trace and compiled executable.
	 * The caller is responsible for lifetime management.
	 */
	void* ptr;
	std::vector<TypedValueRef> arguments;
	FunctionAttributes fnAttrs;
};

struct BlockRef {
	explicit BlockRef(uint16_t block);
	uint16_t block;
	std::vector<TypedValueRef> arguments;
};

/**
 * @brief Lightweight alternative to std::variant for InputVariant.
 *
 * This class replaces std::variant<TypedValueRef, None, ConstantLiteral, BlockRef, FunctionCall, BranchProbability>
 * with a much more efficient tagged union implementation.
 *
 * Key optimizations:
 * - Uses only ~16 bytes instead of 112 bytes for std::variant
 * - Stores small types (TypedValueRef, BranchProbability) inline
 * - Stores large types (BlockRef, FunctionCall) as pointers
 * - ~14x memory reduction for common case (TypedValueRef)
 */
class InputVariant {
public:
	enum class Kind : uint8_t { ValueRef, None, Constant, Block, Function, Probability };

	// Constructors
	InputVariant() : kind_(Kind::None) {
		storage_.none = 0;
	}

	InputVariant(TypedValueRef val) : kind_(Kind::ValueRef) {
		storage_.valueRef = val;
	}

	InputVariant(None) : kind_(Kind::None) {
		storage_.none = 0;
	}

	InputVariant(ConstantLiteral lit) : kind_(Kind::Constant) {
		storage_.constant = new ConstantLiteral(std::move(lit));
	}

	InputVariant(BlockRef ref) : kind_(Kind::Block) {
		storage_.blockRef = new BlockRef(std::move(ref));
	}

	InputVariant(FunctionCall call) : kind_(Kind::Function) {
		storage_.functionCall = new FunctionCall(std::move(call));
	}

	InputVariant(BranchProbability prob) : kind_(Kind::Probability) {
		storage_.probability = prob;
	}

	// Copy constructor
	InputVariant(const InputVariant& other) : kind_(other.kind_) {
		switch (kind_) {
		case Kind::ValueRef:
			storage_.valueRef = other.storage_.valueRef;
			break;
		case Kind::None:
			storage_.none = 0;
			break;
		case Kind::Constant:
			storage_.constant = new ConstantLiteral(*other.storage_.constant);
			break;
		case Kind::Block:
			storage_.blockRef = new BlockRef(*other.storage_.blockRef);
			break;
		case Kind::Function:
			storage_.functionCall = new FunctionCall(*other.storage_.functionCall);
			break;
		case Kind::Probability:
			storage_.probability = other.storage_.probability;
			break;
		}
	}

	// Move constructor
	InputVariant(InputVariant&& other) noexcept : kind_(other.kind_) {
		storage_ = other.storage_;
		other.kind_ = Kind::None;
		other.storage_.none = 0;
	}

	// Copy assignment
	InputVariant& operator=(const InputVariant& other) {
		if (this != &other) {
			destroy();
			kind_ = other.kind_;
			switch (kind_) {
			case Kind::ValueRef:
				storage_.valueRef = other.storage_.valueRef;
				break;
			case Kind::None:
				storage_.none = 0;
				break;
			case Kind::Constant:
				storage_.constant = new ConstantLiteral(*other.storage_.constant);
				break;
			case Kind::Block:
				storage_.blockRef = new BlockRef(*other.storage_.blockRef);
				break;
			case Kind::Function:
				storage_.functionCall = new FunctionCall(*other.storage_.functionCall);
				break;
			case Kind::Probability:
				storage_.probability = other.storage_.probability;
				break;
			}
		}
		return *this;
	}

	// Move assignment
	InputVariant& operator=(InputVariant&& other) noexcept {
		if (this != &other) {
			destroy();
			kind_ = other.kind_;
			storage_ = other.storage_;
			other.kind_ = Kind::None;
			other.storage_.none = 0;
		}
		return *this;
	}

	// Destructor
	~InputVariant() {
		destroy();
	}

	// Type queries
	Kind kind() const {
		return kind_;
	}

	bool isValueRef() const {
		return kind_ == Kind::ValueRef;
	}
	bool isNone() const {
		return kind_ == Kind::None;
	}
	bool isConstant() const {
		return kind_ == Kind::Constant;
	}
	bool isBlock() const {
		return kind_ == Kind::Block;
	}
	bool isFunction() const {
		return kind_ == Kind::Function;
	}
	bool isProbability() const {
		return kind_ == Kind::Probability;
	}

	// Accessors (unchecked - caller must verify type first)
	TypedValueRef& getValueRef() {
		return storage_.valueRef;
	}
	const TypedValueRef& getValueRef() const {
		return storage_.valueRef;
	}

	ConstantLiteral& getConstant() {
		return *storage_.constant;
	}
	const ConstantLiteral& getConstant() const {
		return *storage_.constant;
	}

	BlockRef& getBlock() {
		return *storage_.blockRef;
	}
	const BlockRef& getBlock() const {
		return *storage_.blockRef;
	}

	FunctionCall& getFunction() {
		return *storage_.functionCall;
	}
	const FunctionCall& getFunction() const {
		return *storage_.functionCall;
	}

	BranchProbability& getProbability() {
		return storage_.probability;
	}
	const BranchProbability& getProbability() const {
		return storage_.probability;
	}

private:
	void destroy() {
		switch (kind_) {
		case Kind::Constant:
			delete storage_.constant;
			break;
		case Kind::Block:
			delete storage_.blockRef;
			break;
		case Kind::Function:
			delete storage_.functionCall;
			break;
		default:
			break;
		}
	}

	Kind kind_;
	union Storage {
		uint8_t none;
		TypedValueRef valueRef;
		ConstantLiteral* constant;
		BlockRef* blockRef;
		FunctionCall* functionCall;
		BranchProbability probability;

		Storage() : none(0) {
		}
	} storage_;
};

// std::variant-compatible helper functions
template <typename T>
bool holds_alternative(const InputVariant& v) {
	if constexpr (std::is_same_v<T, TypedValueRef>) {
		return v.isValueRef();
	} else if constexpr (std::is_same_v<T, None>) {
		return v.isNone();
	} else if constexpr (std::is_same_v<T, ConstantLiteral>) {
		return v.isConstant();
	} else if constexpr (std::is_same_v<T, BlockRef>) {
		return v.isBlock();
	} else if constexpr (std::is_same_v<T, FunctionCall>) {
		return v.isFunction();
	} else if constexpr (std::is_same_v<T, BranchProbability>) {
		return v.isProbability();
	}
	return false;
}

template <typename T>
T& get(InputVariant& v) {
	if constexpr (std::is_same_v<T, TypedValueRef>) {
		return v.getValueRef();
	} else if constexpr (std::is_same_v<T, ConstantLiteral>) {
		return v.getConstant();
	} else if constexpr (std::is_same_v<T, BlockRef>) {
		return v.getBlock();
	} else if constexpr (std::is_same_v<T, FunctionCall>) {
		return v.getFunction();
	} else if constexpr (std::is_same_v<T, BranchProbability>) {
		return v.getProbability();
	}
}

template <typename T>
const T& get(const InputVariant& v) {
	if constexpr (std::is_same_v<T, TypedValueRef>) {
		return v.getValueRef();
	} else if constexpr (std::is_same_v<T, ConstantLiteral>) {
		return v.getConstant();
	} else if constexpr (std::is_same_v<T, BlockRef>) {
		return v.getBlock();
	} else if constexpr (std::is_same_v<T, FunctionCall>) {
		return v.getFunction();
	} else if constexpr (std::is_same_v<T, BranchProbability>) {
		return v.getProbability();
	}
}

template <typename T>
T* get_if(InputVariant* v) {
	if (!v || !holds_alternative<T>(*v)) {
		return nullptr;
	}
	return &get<T>(*v);
}

template <typename T>
const T* get_if(const InputVariant* v) {
	if (!v || !holds_alternative<T>(*v)) {
		return nullptr;
	}
	return &get<T>(*v);
}

/**
 * @brief Represents an individual operation in a trace.
 */
class TraceOperation {
public:
	TraceOperation(Snapshot& tag, Op op, Type resultType, TypedValueRef ref, std::vector<InputVariant>&& input);
	TraceOperation(Op op, std::vector<InputVariant>&& input);
	Snapshot tag;
	Op op;
	Type resultType;
	TypedValueRef resultRef;
	std::vector<InputVariant> input;
};

} // namespace nautilus::tracing

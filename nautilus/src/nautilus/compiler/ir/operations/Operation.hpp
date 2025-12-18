
#pragma once

#include "nautilus/tracing/Types.hpp"
#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace nautilus::compiler::ir {

/**
 * @brief Represents a single stack frame in the execution call stack.
 */
struct StackFrame {
	std::string function_name;
	std::string module_name;
	uint64_t return_address;
	std::optional<std::string> source_file;
	std::optional<uint32_t> source_line;

	StackFrame() = default;

	StackFrame(const std::string& fn, const std::string& mn, uint64_t ra)
		: function_name(fn), module_name(mn), return_address(ra) {}
};

/**
 * @brief Full location context including source location and call stack.
 */
struct LocationContext {
	struct SourceLoc {
		std::string file;
		uint32_t line;
		uint32_t column;
		std::string function;

		SourceLoc() = default;

		SourceLoc(const std::string& f, uint32_t l, uint32_t c, const std::string& fn)
			: file(f), line(l), column(c), function(fn) {}

		bool has_location() const {
			return !file.empty() && line > 0;
		}
	};

	std::optional<SourceLoc> source_location;
	std::vector<StackFrame> call_stack;

	LocationContext() = default;
};

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

	/**
	 * @brief Set location and semantic information for this operation.
	 */
	void setLocationContext(const LocationContext& loc) {
		this->location_context = loc;
	}

	/**
	 * @brief Get location and semantic information for this operation.
	 */
	const std::optional<LocationContext>& getLocationContext() const {
		return this->location_context;
	}

	/**
	 * @brief Set semantic name for this operation (e.g., "sum_result", "doubled_value").
	 */
	void setSemanticName(const std::string& name) {
		this->semantic_name = name;
	}

	/**
	 * @brief Get semantic name for this operation.
	 */
	std::optional<std::string_view> getSemanticName() const {
		if (semantic_name) {
			return std::string_view(*semantic_name);
		}
		return std::nullopt;
	}

	template <typename OP>
	const OP* dynCast() const {
		return dynamic_cast<const OP*>(this);
	}

protected:
	const OperationType opType;
	const OperationIdentifier identifier;
	const Type stamp;
	std::vector<Operation*> inputs;

	// Location and naming information
	std::optional<LocationContext> location_context;
	std::optional<std::string> semantic_name;
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

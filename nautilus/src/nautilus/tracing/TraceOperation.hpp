
#pragma once

#include "nautilus/tracing/BoundedInputArray.hpp"
#include "nautilus/tracing/Snapshot.hpp"
#include "nautilus/tracing/TracingUtil.hpp"
#include "nautilus/tracing/tag/Tag.hpp"
#include <any>
#include <array>
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

using InputVariant = std::variant<TypedValueRef, None, ConstantLiteral, BlockRef, FunctionCall, BranchProbability>;

/**
 * @brief Represents an individual operation in a trace.
 */
class TraceOperation {
public:
	// Constructors with explicit input counts (0-4 inputs)
	TraceOperation(Snapshot& tag, Op op, Type resultType, TypedValueRef ref);
	TraceOperation(Snapshot& tag, Op op, Type resultType, TypedValueRef ref, InputVariant input0);
	TraceOperation(Snapshot& tag, Op op, Type resultType, TypedValueRef ref, InputVariant input0, InputVariant input1);
	TraceOperation(Snapshot& tag, Op op, Type resultType, TypedValueRef ref, InputVariant input0, InputVariant input1,
	               InputVariant input2);
	TraceOperation(Snapshot& tag, Op op, Type resultType, TypedValueRef ref, InputVariant input0, InputVariant input1,
	               InputVariant input2, InputVariant input3);

	TraceOperation(Op op);
	TraceOperation(Op op, InputVariant input0);
	TraceOperation(Op op, InputVariant input0, InputVariant input1);
	TraceOperation(Op op, InputVariant input0, InputVariant input1, InputVariant input2);
	TraceOperation(Op op, InputVariant input0, InputVariant input1, InputVariant input2, InputVariant input3);

	// Constructor accepting BoundedInputArray directly
	TraceOperation(Snapshot& tag, Op op, Type resultType, TypedValueRef ref,
	               BoundedInputArray<InputVariant, 4>&& inputs);
	TraceOperation(Op op, BoundedInputArray<InputVariant, 4>&& inputs);

	// Default copy/move semantics
	TraceOperation(const TraceOperation&) = default;
	TraceOperation& operator=(const TraceOperation&) = default;
	TraceOperation(TraceOperation&&) = default;
	TraceOperation& operator=(TraceOperation&&) = default;

	Snapshot tag;
	Op op;
	Type resultType;
	TypedValueRef resultRef;
	BoundedInputArray<InputVariant, 4> input;
};

} // namespace nautilus::tracing


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

using InputVariant = std::variant<TypedValueRef, None, ConstantLiteral, BlockRef, FunctionCall, BranchProbability>;

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

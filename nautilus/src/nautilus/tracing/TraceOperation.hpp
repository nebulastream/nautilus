
#pragma once

#include "nautilus/tracing/Snapshot.hpp"
#include "nautilus/tracing/TracingUtil.hpp"
#include "nautilus/tracing/tag/Tag.hpp"
#include <any>
#include <cstddef>
#include <nautilus/common/FunctionAttributes.hpp>
#include <type_traits>
#include <utility>
#include <variant>
#include <vector>

namespace nautilus::tracing {

class None {};
using BranchProbability = double;

/// Index into ExecutionTrace::allocaSpecs identifying which alloca slot this
/// trace operation refers to.  Each Op::ALLOCA carries one of these as its
/// only input; the backing (size, align) pair is held centrally on the trace
/// so backends can iterate the full table when emitting the function prologue.
using AllocaIndex = uint32_t;

/// Per-function alloca-table entry.  Recorded at trace time, copied onto the
/// resulting FunctionOperation, then materialised as one real alloca per entry
/// in each backend's function prologue.
struct AllocaSpec {
	size_t size;
	size_t align;
};

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
	explicit BlockRef(uint32_t block);
	uint32_t block;
	std::vector<TypedValueRef> arguments;
};

using InputVariant =
    std::variant<TypedValueRef, None, ConstantLiteral, BlockRef, FunctionCall, BranchProbability, AllocaIndex>;

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

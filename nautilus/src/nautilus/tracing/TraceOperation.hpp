
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
using BranchProbability = float;

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

struct FunctionCallId {
	uint32_t id;
	explicit FunctionCallId(uint32_t id) : id(id) {
	}
	operator uint32_t() const {
		return id;
	}
};

struct BlockRef {
	explicit BlockRef(uint16_t block);
	uint16_t block;
	std::vector<TypedValueRef> arguments;
};

struct BlockRefId {
	uint32_t id;
	explicit BlockRefId(uint32_t id) : id(id) {
	}
	operator uint32_t() const {
		return id;
	}
};

struct ConstantLiteralId {
	uint32_t id;
	explicit ConstantLiteralId(uint32_t id) : id(id) {
	}
	operator uint32_t() const {
		return id;
	}
};

using InputVariant =
    std::variant<TypedValueRef, None, ConstantLiteralId, BlockRefId, FunctionCallId, BranchProbability>;

/**
 * @brief Represents an individual operation in a trace.
 */
class TraceOperation {
private:
	/**
	 * @brief Helper to construct the input array with proper padding
	 */
	template <typename... Inputs>
	static BoundedInputArray<InputVariant, 4> makeInputArray(Inputs&&... inputs) {
		constexpr size_t count = sizeof...(Inputs);
		static_assert(count <= 4, "TraceOperation supports at most 4 inputs");
		std::array<InputVariant, 4> arr;
		size_t i = 0;
		((arr[i++] = std::forward<Inputs>(inputs)), ...);
		while (i < 4) {
			arr[i++] = None {};
		}
		return BoundedInputArray<InputVariant, 4>(std::move(arr), count);
	}

public:
	/**
	 * @brief Variadic constructor for operations with a result and snapshot
	 * @tparam Inputs Types of input variants (up to 4)
	 * @param tag Execution snapshot
	 * @param op Operation type
	 * @param ref Result value reference
	 * @param inputs Input variants (forwarded)
	 */
	template <typename... Inputs>
	TraceOperation(Snapshot& tag, Op op, TypedValueRef ref, Inputs&&... inputs)
	    : input(makeInputArray(std::forward<Inputs>(inputs)...)), tag(tag), resultRef(ref), op(op) {
	}

	/**
	 * @brief Variadic constructor for operations without a result (void operations)
	 * @tparam Inputs Types of input variants (up to 4)
	 * @param op Operation type
	 * @param inputs Input variants (forwarded)
	 */
	template <typename... Inputs>
	TraceOperation(Op op, Inputs&&... inputs)
	    : input(makeInputArray(std::forward<Inputs>(inputs)...)), tag(), resultRef(), op(op) {
	}

	// Constructor accepting BoundedInputArray directly
	TraceOperation(Snapshot& tag, Op op, TypedValueRef ref, BoundedInputArray<InputVariant, 4>&& inputs);
	TraceOperation(Op op, BoundedInputArray<InputVariant, 4>&& inputs);

	// Default copy/move semantics
	TraceOperation(const TraceOperation&) = default;
	TraceOperation& operator=(const TraceOperation&) = default;
	TraceOperation(TraceOperation&&) = default;
	TraceOperation& operator=(TraceOperation&&) = default;

	/**
	 * @brief Get the result type of this operation
	 * @return Type The type of the result value
	 */
	Type getResultType() const {
		return resultRef.type;
	}

	BoundedInputArray<InputVariant, 4> input;
	Snapshot tag;
	TypedValueRef resultRef;
	Op op;
};

} // namespace nautilus::tracing

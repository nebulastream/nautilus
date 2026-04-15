
#pragma once

#include "nautilus/common/Arena.hpp"
#include "nautilus/tracing/Snapshot.hpp"
#include "nautilus/tracing/TracingUtil.hpp"
#include "nautilus/tracing/tag/Tag.hpp"
#include <any>
#include <cassert>
#include <concepts>
#include <cstddef>
#include <initializer_list>
#include <nautilus/common/FunctionAttributes.hpp>
#include <new>
#include <span>
#include <type_traits>
#include <utility>
#include <variant>
#include <vector>

namespace nautilus::tracing {

class None {};
using BranchProbability = double;
using AllocSize = size_t;

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

/// Represents an indirect call through a runtime function pointer value.
struct IndirectFunctionCall {
	TypedValueRef fnPtr;
	std::vector<TypedValueRef> arguments;
	FunctionAttributes fnAttrs;
};

struct BlockRef {
	explicit BlockRef(uint32_t block);
	uint32_t block;
	std::vector<TypedValueRef> arguments;
};

/**
 * @brief Operand type carried in a TraceOperation's input array.
 *
 * The three heavyweight alternatives (BlockRef, FunctionCall,
 * IndirectFunctionCall) each carry std::vector / std::string members and are
 * therefore stored by raw pointer into the Arena rather than inline.  The
 * pointed-to objects are allocated via Arena::create, which registers their
 * destructors with the Arena so their internal containers are cleaned up on
 * Arena::softReset / Arena destruction.
 *
 * Storing pointers makes InputVariant trivially destructible: an array of
 * InputVariants requires no per-element cleanup, so the array's Arena bytes
 * can simply be reclaimed in bulk with the rest of the Arena.
 */
using InputVariant = std::variant<TypedValueRef, None, ConstantLiteral, BlockRef*, FunctionCall*, BranchProbability,
                                  AllocSize, IndirectFunctionCall*>;

static_assert(std::is_trivially_destructible_v<InputVariant>,
              "InputVariant must stay trivially destructible so TraceOperation input arrays need no dtor sweep");

/**
 * @brief Represents an individual operation in a trace.
 *
 * The input operand array is backed by the same common::Arena that owns the
 * TraceOperation itself: it is a plain pointer + length (std::span) rather than
 * a std::vector, so creating an operation no longer performs a per-op heap
 * allocation.  The array length is statically derived from the op code (see
 * @ref inputCountFor) and exposed via @ref inputCount.
 *
 * Because InputVariant (and every other member) is trivially destructible,
 * TraceOperation is itself trivially destructible and the Arena skips
 * destructor tracking for it.
 *
 * TraceOperation instances must be constructed through the @ref makeTraceOp /
 * @ref cloneTraceOp factory functions below, which are responsible for
 * allocating the input storage in the Arena before invoking the constructor.
 */
class TraceOperation {
public:
	TraceOperation(Snapshot& tag, Op op, Type resultType, TypedValueRef ref, std::span<InputVariant> input);
	TraceOperation(Op op, std::span<InputVariant> input);

	// TraceOperation lives in an Arena and is referenced by stable pointers;
	// copying or moving would invalidate those references and desynchronise the
	// input span from its arena-allocated storage.  Use cloneTraceOp to create
	// an independent copy in a target arena.
	TraceOperation(const TraceOperation&) = delete;
	TraceOperation& operator=(const TraceOperation&) = delete;
	TraceOperation(TraceOperation&&) = delete;
	TraceOperation& operator=(TraceOperation&&) = delete;

	/// Number of operand slots this operation has.  Derived purely from the op
	/// code (and, for RETURN, the result type).
	[[nodiscard]] uint8_t inputCount() const noexcept {
		return inputCountFor(op, resultType);
	}

	Snapshot tag;
	Op op;
	Type resultType;
	TypedValueRef resultRef;
	/// View over the Arena-allocated input array.  The storage is adjacent to
	/// (and has the same lifetime as) this TraceOperation.
	std::span<InputVariant> input;
};

namespace detail {

/// Allocates an uninitialised array of @p count InputVariants from @p arena.
/// Returns nullptr when @p count is zero to avoid touching the bump pointer
/// for a zero-length allocation.
inline InputVariant* allocateInputArray(common::Arena& arena, std::size_t count) {
	if (count == 0) {
		return nullptr;
	}
	return static_cast<InputVariant*>(arena.allocate(sizeof(InputVariant) * count, alignof(InputVariant)));
}

} // namespace detail

/**
 * @brief Allocates an input array in @p arena, constructs an InputVariant for
 * each forwarded @p inputs directly in that storage, and wires up a new
 * TraceOperation referencing the array.
 *
 * Perfect-forwards every argument into the Arena-backed storage in a single
 * placement-new per input: no intermediate initializer_list copy.  The number
 * of inputs is derived from the pack size at compile time; the runtime
 * assertion just verifies it matches the op's declared arity.
 *
 * The returned TraceOperation and its input storage all live in @p arena and
 * share its lifetime.
 */
template <typename... Inputs>
    requires(std::constructible_from<InputVariant, Inputs &&> && ...)
TraceOperation* makeTraceOp(common::Arena& arena, Snapshot& tag, Op op, Type resultType, TypedValueRef ref,
                            Inputs&&... inputs) {
	constexpr std::size_t count = sizeof...(Inputs);
	assert(count == inputCountFor(op, resultType) &&
	       "TraceOperation input count disagrees with inputCountFor(op, resultType)");
	InputVariant* buffer = detail::allocateInputArray(arena, count);
	if constexpr (count > 0) {
		std::size_t i = 0;
		(::new (&buffer[i++]) InputVariant(std::forward<Inputs>(inputs)), ...);
	}
	return arena.create<TraceOperation>(tag, op, resultType, ref, std::span<InputVariant> {buffer, count});
}

/**
 * @brief Tag-less variadic overload for operations that do not participate in
 * the tag map (e.g. the synthetic JMPs inserted during control-flow merging).
 */
template <typename... Inputs>
    requires(std::constructible_from<InputVariant, Inputs &&> && ...)
TraceOperation* makeTraceOp(common::Arena& arena, Op op, Inputs&&... inputs) {
	constexpr std::size_t count = sizeof...(Inputs);
	assert(count == inputCountFor(op, Type::v) && "Tag-less TraceOperation input count disagrees with inputCountFor");
	InputVariant* buffer = detail::allocateInputArray(arena, count);
	if constexpr (count > 0) {
		std::size_t i = 0;
		(::new (&buffer[i++]) InputVariant(std::forward<Inputs>(inputs)), ...);
	}
	return arena.create<TraceOperation>(op, std::span<InputVariant> {buffer, count});
}

/**
 * @brief initializer_list overload used when the caller forwards an already-
 * built list of inputs (e.g. ExecutionTrace::addOperationWithResult).  Prefer
 * the variadic templates above at direct call sites to avoid the extra
 * InputVariant copy the initializer_list backing array entails.
 */
TraceOperation* makeTraceOp(common::Arena& arena, Snapshot& tag, Op op, Type resultType, TypedValueRef ref,
                            std::initializer_list<InputVariant> inputs);

TraceOperation* makeTraceOp(common::Arena& arena, Op op, std::initializer_list<InputVariant> inputs);

/**
 * @brief Deep-copies @p source (including its input array) into @p arena.
 *
 * Replaces the previous `arena.create<TraceOperation>(*source)` idiom: the
 * copy-constructor is deleted because it would silently share the source's
 * input span with the destination.
 */
TraceOperation* cloneTraceOp(common::Arena& arena, const TraceOperation& source);

} // namespace nautilus::tracing

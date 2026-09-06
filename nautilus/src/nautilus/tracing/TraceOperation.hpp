
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
#include <limits>
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

/// Index into ExecutionTrace::regions. Operations and blocks that belong to no
/// region carry NO_REGION.
using RegionIndex = uint32_t;
inline constexpr RegionIndex NO_REGION = std::numeric_limits<RegionIndex>::max();

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
/// Which kind of callee a FunctionCall names.
///
/// The tracer knows this for certain at the moment it records the call, and
/// this is where it writes it down. Without it, everything downstream has to
/// re-derive the answer by looking a name up in a side table -- which is what
/// the function table exists to stop.
enum class CalleeKind : uint8_t {
	/// A native function reached through its real code address.
	External,
	/// Another Nautilus function, traced into this same module. `ptr` is its
	/// NautilusFunctionDefinition, which is an identity, NOT a callable
	/// address.
	Internal,
};

struct FunctionCall {
	struct Destructor {
		TypedValueRef address;
		std::string functionName;
		std::string mangledName;
		void* ptr;
	};

	std::string functionName;
	std::string mangledName;
	/**
	 * @brief Non-owning pointer identifying the function being called.
	 * @warning Must remain valid for the lifetime of the trace and compiled executable.
	 * The caller is responsible for lifetime management.
	 *
	 * For CalleeKind::External this is a real code address. For
	 * CalleeKind::Internal it is a NautilusFunctionDefinition* used purely as
	 * an identity key -- calling it would execute a data object.
	 */
	void* ptr;
	/**
	 * @brief Capture wrapper for a potentially-throwing call. Points at a
	 * `captureThrowingCall<R, Args...>` instantiation generated at the typed
	 * invoke() site — a real C++ frame that catches exceptions before they
	 * cross a generated/interpreted frame without unwind tables.
	 * Null for `noUnwind` calls.
	 */
	void* captureFunc = nullptr;
	CalleeKind kind = CalleeKind::External;
	std::vector<TypedValueRef> arguments;
	FunctionAttributes fnAttrs;
	std::vector<Destructor> destructors;
	/**
	 * @brief True for a call into another traced Nautilus function (via
	 * NautilusFunction), false for a raw invoke() into an external
	 * function. Lets IR passes tell the two apart without guessing from
	 * `functionName` collisions: only a Nautilus-to-Nautilus call can be
	 * proven noUnwind by inspecting the callee's own traced body, since the
	 * callee's function pointer type isn't the source of truth here (see
	 * NoThrowInferencePass).
	 */
	bool isNautilusCall = false;
};

/// Represents an indirect call through a runtime function pointer value.
struct IndirectFunctionCall {
	TypedValueRef fnPtr;
	/**
	 * @brief Capture wrapper for a potentially-throwing indirect call. See
	 * `FunctionCall::captureFunc`; null for `noUnwind` calls.
	 */
	void* captureFunc = nullptr;
	std::vector<TypedValueRef> arguments;
	FunctionAttributes fnAttrs;
	std::vector<FunctionCall::Destructor> destructors;
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
                                  AllocaIndex, IndirectFunctionCall*>;

static_assert(std::is_trivially_destructible_v<InputVariant>,
              "InputVariant must stay trivially destructible so TraceOperation input arrays need no dtor sweep");

/** Visits every value reference nested in an input without exposing its storage. */
template <typename Callback>
void forEachValueRef(const InputVariant& input, Callback&& callback) {
	if (const auto* value_ref = std::get_if<TypedValueRef>(&input)) {
		callback(*value_ref);
	} else if (const auto* block_ref = std::get_if<BlockRef*>(&input); block_ref != nullptr && *block_ref != nullptr) {
		// The callback may append block arguments elsewhere. Snapshot the count and
		// pass values by copy so vector reallocation cannot invalidate this walk.
		const auto argument_count = (*block_ref)->arguments.size();
		for (size_t i = 0; i < argument_count; ++i) {
			const auto argument = (*block_ref)->arguments[i];
			callback(argument);
		}
	} else if (const auto* call = std::get_if<FunctionCall*>(&input); call != nullptr && *call != nullptr) {
		for (const auto argument : (*call)->arguments) {
			callback(argument);
		}
		for (const auto& destructor : (*call)->destructors) {
			callback(destructor.address);
		}
	} else if (const auto* call = std::get_if<IndirectFunctionCall*>(&input); call != nullptr && *call != nullptr) {
		callback((*call)->fnPtr);
		for (const auto argument : (*call)->arguments) {
			callback(argument);
		}
		for (const auto& destructor : (*call)->destructors) {
			callback(destructor.address);
		}
	}
}

/** Visits every mutable value reference nested in an input. */
template <typename Callback>
void forEachMutableValueRef(InputVariant& input, Callback&& callback) {
	if (auto* value_ref = std::get_if<TypedValueRef>(&input)) {
		callback(*value_ref);
	} else if (auto* block_ref = std::get_if<BlockRef*>(&input); block_ref != nullptr && *block_ref != nullptr) {
		for (auto& argument : (*block_ref)->arguments) {
			callback(argument);
		}
	} else if (auto* call = std::get_if<FunctionCall*>(&input); call != nullptr && *call != nullptr) {
		for (auto& argument : (*call)->arguments) {
			callback(argument);
		}
		for (auto& destructor : (*call)->destructors) {
			callback(destructor.address);
		}
	} else if (auto* call = std::get_if<IndirectFunctionCall*>(&input); call != nullptr && *call != nullptr) {
		callback((*call)->fnPtr);
		for (auto& argument : (*call)->arguments) {
			callback(argument);
		}
		for (auto& destructor : (*call)->destructors) {
			callback(destructor.address);
		}
	}
}

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
	/// The innermost region (docs/region.md) this operation was recorded inside,
	/// or NO_REGION for one recorded outside every region. Stamped by
	/// ExecutionTrace from the region the tracer had open at the time, and
	/// carried into the IR by TraceToIRConversionPhase -- which is why it lives
	/// on the operation rather than on its block: the block that bounds a region
	/// is collapsed by the block-cleanup passes, the operations are not.
	RegionIndex regionIndex = NO_REGION;
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

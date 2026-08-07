#include "nautilus/tracing/TraceOperation.hpp"
#include "nautilus/common/Arena.hpp"
#include <new>

namespace nautilus::tracing {

BlockRef::BlockRef(uint32_t block) : block(block) {
}

TraceOperation::TraceOperation(Snapshot& tag, Op op, Type resultType, TypedValueRef ref, std::span<InputVariant> input)
    : tag(tag), op(op), resultType(resultType), resultRef(ref), input(input) {
}

TraceOperation::TraceOperation(Op op, std::span<InputVariant> input)
    : tag(), op(op), resultType(Type::v), resultRef(), input(input) {
}

static_assert(std::is_trivially_destructible_v<TraceOperation>,
              "TraceOperation must be trivially destructible so the Arena skips destructor tracking");

TraceOperation* makeTraceOp(common::Arena& arena, Snapshot& tag, Op op, Type resultType, TypedValueRef ref,
                            std::initializer_list<InputVariant> inputs) {
	assert(inputs.size() == inputCountFor(op, resultType) &&
	       "TraceOperation input count disagrees with inputCountFor(op, resultType)");
	auto* buffer = detail::allocateInputArray(arena, inputs.size());
	size_t i = 0;
	for (const auto& in : inputs) {
		::new (&buffer[i++]) InputVariant(in);
	}
	return arena.create<TraceOperation>(tag, op, resultType, ref, std::span<InputVariant> {buffer, inputs.size()});
}

TraceOperation* makeTraceOp(common::Arena& arena, Op op, std::initializer_list<InputVariant> inputs) {
	assert(inputs.size() == inputCountFor(op, Type::v) &&
	       "Tag-less TraceOperation input count disagrees with inputCountFor");
	auto* buffer = detail::allocateInputArray(arena, inputs.size());
	size_t i = 0;
	for (const auto& in : inputs) {
		::new (&buffer[i++]) InputVariant(in);
	}
	return arena.create<TraceOperation>(op, std::span<InputVariant> {buffer, inputs.size()});
}

TraceOperation* cloneTraceOp(common::Arena& arena, const TraceOperation& source) {
	auto* buffer = detail::allocateInputArray(arena, source.input.size());
	for (size_t i = 0; i < source.input.size(); ++i) {
		::new (&buffer[i]) InputVariant(source.input[i]);
	}
	std::span<InputVariant> span(buffer, source.input.size());
	Snapshot copiedTag = source.tag;
	return arena.create<TraceOperation>(copiedTag, source.op, source.resultType, source.resultRef, span);
}

} // namespace nautilus::tracing

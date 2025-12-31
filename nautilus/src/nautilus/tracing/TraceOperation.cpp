#include "nautilus/tracing/TraceOperation.hpp"

namespace nautilus::tracing {

BlockRef::BlockRef(uint16_t block) : block(block) {
}

// Constructors accepting BoundedInputArray directly
TraceOperation::TraceOperation(Snapshot& tag, Op op, TypedValueRef ref, BoundedInputArray<InputVariant, 4>&& inputs)
    : input(std::move(inputs)), tag(tag), resultRef(ref), op(op) {
}

TraceOperation::TraceOperation(Op op, BoundedInputArray<InputVariant, 4>&& inputs)
    : input(std::move(inputs)), tag(), resultRef(), op(op) {
}

} // namespace nautilus::tracing

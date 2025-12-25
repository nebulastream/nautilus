#include "nautilus/tracing/TraceOperation.hpp"

namespace nautilus::tracing {

BlockRef::BlockRef(uint16_t block) : block(block) {
}

TraceOperation::TraceOperation(Snapshot& tag, Op op, Type resultType, TypedValueRef ref,
                               std::vector<InputVariant>&& input)
    : tag(tag), op(op), resultType(resultType), resultRef(ref), input(std::move(input)) {
}

TraceOperation::TraceOperation(Op op, std::vector<InputVariant>&& input)
    : tag(), op(op), resultType(Type::v), resultRef(), input(std::move(input)) {
}

} // namespace nautilus::tracing

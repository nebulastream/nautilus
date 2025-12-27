#include "nautilus/tracing/TraceOperation.hpp"

namespace nautilus::tracing {

BlockRef::BlockRef(uint16_t block) : block(block) {
}

// Constructors with Snapshot and result
// Note: Member order is: input, tag, resultRef, op, resultType
TraceOperation::TraceOperation(Snapshot& tag, Op op, Type resultType, TypedValueRef ref)
    : input(), tag(tag), resultRef(ref), op(op), resultType(resultType) {
}

TraceOperation::TraceOperation(Snapshot& tag, Op op, Type resultType, TypedValueRef ref, InputVariant input0)
    : input({{input0, None {}, None {}, None {}}}, 1), tag(tag), resultRef(ref), op(op), resultType(resultType) {
}

TraceOperation::TraceOperation(Snapshot& tag, Op op, Type resultType, TypedValueRef ref, InputVariant input0,
                               InputVariant input1)
    : input({{input0, input1, None {}, None {}}}, 2), tag(tag), resultRef(ref), op(op), resultType(resultType) {
}

TraceOperation::TraceOperation(Snapshot& tag, Op op, Type resultType, TypedValueRef ref, InputVariant input0,
                               InputVariant input1, InputVariant input2)
    : input({{input0, input1, input2, None {}}}, 3), tag(tag), resultRef(ref), op(op), resultType(resultType) {
}

TraceOperation::TraceOperation(Snapshot& tag, Op op, Type resultType, TypedValueRef ref, InputVariant input0,
                               InputVariant input1, InputVariant input2, InputVariant input3)
    : input({{input0, input1, input2, input3}}, 4), tag(tag), resultRef(ref), op(op), resultType(resultType) {
}

// Constructors without Snapshot (void operations)
TraceOperation::TraceOperation(Op op) : input(), tag(), resultRef(), op(op), resultType(Type::v) {
}

TraceOperation::TraceOperation(Op op, InputVariant input0)
    : input({{input0, None {}, None {}, None {}}}, 1), tag(), resultRef(), op(op), resultType(Type::v) {
}

TraceOperation::TraceOperation(Op op, InputVariant input0, InputVariant input1)
    : input({{input0, input1, None {}, None {}}}, 2), tag(), resultRef(), op(op), resultType(Type::v) {
}

TraceOperation::TraceOperation(Op op, InputVariant input0, InputVariant input1, InputVariant input2)
    : input({{input0, input1, input2, None {}}}, 3), tag(), resultRef(), op(op), resultType(Type::v) {
}

TraceOperation::TraceOperation(Op op, InputVariant input0, InputVariant input1, InputVariant input2,
                               InputVariant input3)
    : input({{input0, input1, input2, input3}}, 4), tag(), resultRef(), op(op), resultType(Type::v) {
}

// Constructors accepting BoundedInputArray directly
TraceOperation::TraceOperation(Snapshot& tag, Op op, Type resultType, TypedValueRef ref,
                               BoundedInputArray<InputVariant, 4>&& inputs)
    : input(std::move(inputs)), tag(tag), resultRef(ref), op(op), resultType(resultType) {
}

TraceOperation::TraceOperation(Op op, BoundedInputArray<InputVariant, 4>&& inputs)
    : input(std::move(inputs)), tag(), resultRef(), op(op), resultType(Type::v) {
}

} // namespace nautilus::tracing

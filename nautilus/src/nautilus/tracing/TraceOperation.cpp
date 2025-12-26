#include "nautilus/tracing/TraceOperation.hpp"

namespace nautilus::tracing {

BlockRef::BlockRef(uint16_t block) : block(block) {
}

// Constructors with Snapshot and result
TraceOperation::TraceOperation(Snapshot& tag, Op op, Type resultType, TypedValueRef ref)
    : tag(tag), op(op), resultType(resultType), resultRef(ref), input() {
}

TraceOperation::TraceOperation(Snapshot& tag, Op op, Type resultType, TypedValueRef ref, InputVariant input0)
    : tag(tag), op(op), resultType(resultType), resultRef(ref), input({{input0, None {}, None {}, None {}}}, 1) {
}

TraceOperation::TraceOperation(Snapshot& tag, Op op, Type resultType, TypedValueRef ref, InputVariant input0,
                               InputVariant input1)
    : tag(tag), op(op), resultType(resultType), resultRef(ref), input({{input0, input1, None {}, None {}}}, 2) {
}

TraceOperation::TraceOperation(Snapshot& tag, Op op, Type resultType, TypedValueRef ref, InputVariant input0,
                               InputVariant input1, InputVariant input2)
    : tag(tag), op(op), resultType(resultType), resultRef(ref), input({{input0, input1, input2, None {}}}, 3) {
}

TraceOperation::TraceOperation(Snapshot& tag, Op op, Type resultType, TypedValueRef ref, InputVariant input0,
                               InputVariant input1, InputVariant input2, InputVariant input3)
    : tag(tag), op(op), resultType(resultType), resultRef(ref), input({{input0, input1, input2, input3}}, 4) {
}

// Constructors without Snapshot (void operations)
TraceOperation::TraceOperation(Op op) : tag(), op(op), resultType(Type::v), resultRef(), input() {
}

TraceOperation::TraceOperation(Op op, InputVariant input0)
    : tag(), op(op), resultType(Type::v), resultRef(), input({{input0, None {}, None {}, None {}}}, 1) {
}

TraceOperation::TraceOperation(Op op, InputVariant input0, InputVariant input1)
    : tag(), op(op), resultType(Type::v), resultRef(), input({{input0, input1, None {}, None {}}}, 2) {
}

TraceOperation::TraceOperation(Op op, InputVariant input0, InputVariant input1, InputVariant input2)
    : tag(), op(op), resultType(Type::v), resultRef(), input({{input0, input1, input2, None {}}}, 3) {
}

TraceOperation::TraceOperation(Op op, InputVariant input0, InputVariant input1, InputVariant input2,
                               InputVariant input3)
    : tag(), op(op), resultType(Type::v), resultRef(), input({{input0, input1, input2, input3}}, 4) {
}

// Constructors accepting BoundedInputArray directly
TraceOperation::TraceOperation(Snapshot& tag, Op op, Type resultType, TypedValueRef ref,
                               BoundedInputArray<InputVariant, 4>&& inputs)
    : tag(tag), op(op), resultType(resultType), resultRef(ref), input(std::move(inputs)) {
}

TraceOperation::TraceOperation(Op op, BoundedInputArray<InputVariant, 4>&& inputs)
    : tag(), op(op), resultType(Type::v), resultRef(), input(std::move(inputs)) {
}

} // namespace nautilus::tracing

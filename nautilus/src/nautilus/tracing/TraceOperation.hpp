
#pragma once

#include "nautilus/tracing/Snapshot.hpp"
#include "nautilus/tracing/TracingUtil.hpp"
#include "nautilus/tracing/tag/Tag.hpp"
#include <any>
#include <cstdint>
#include <nautilus/common/FunctionAttributes.hpp>
#include <optional>
#include <type_traits>
#include <utility>
#include <variant>
#include <vector>

namespace nautilus::tracing {

class None {};

struct SourceLocation {
        std::string file;
        uint32_t line;
        uint32_t column;
};

struct FunctionCall {
        std::string functionName;
        std::string mangledName;
        void* ptr;
        std::vector<TypedValueRef> arguments;
        FunctionAttributes fnAttrs;
        std::optional<SourceLocation> location;
};

struct BlockRef {
	explicit BlockRef(uint16_t block);
	uint16_t block;
	std::vector<TypedValueRef> arguments;
};

using InputVariant = std::variant<TypedValueRef, None, ConstantLiteral, BlockRef, FunctionCall>;

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

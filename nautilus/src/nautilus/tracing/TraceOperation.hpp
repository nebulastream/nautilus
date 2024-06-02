
#pragma once

#include "nautilus/common/traceing.hpp"
#include "nautilus/tracing/Snapshot.hpp"
#include "nautilus/tracing/tag/Tag.hpp"
#include <any>
#include <ostream>
#include <sstream>
#include <type_traits>
#include <utility>
#include <variant>
#include <vector>

namespace nautilus::tracing {

class None {};

struct FunctionCall {
	std::string functionName;
	void* ptr;
	std::vector<value_ref> arguments;

	friend std::ostream& operator<<(std::ostream& os, const FunctionCall& call);
};

struct BlockRef {
	explicit BlockRef(uint16_t block);

	uint16_t block;
	std::vector<TypedValueRef> arguments;

	friend std::ostream& operator<<(std::ostream& os, const BlockRef& ref);
};

using InputVariant = std::variant<value_ref, None, std::any, BlockRef, FunctionCall>;

/**
 * @brief Represents an individual operation in a trace.
 */
class TraceOperation {
public:
	TraceOperation(Snapshot& tag, Op op, Type resultType, value_ref ref, std::vector<InputVariant>&& input);

	TraceOperation(Op op, std::vector<InputVariant>&& input);

	TraceOperation(const TraceOperation& other) = default;

	friend std::ostream& operator<<(std::ostream& os, const TraceOperation& operation);

	~TraceOperation() = default;

	Snapshot tag;
	Op op;
	Type resultType;
	value_ref resultRef;
	std::vector<InputVariant> input;
};

} // namespace nautilus::tracing

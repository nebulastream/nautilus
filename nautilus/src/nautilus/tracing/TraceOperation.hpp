
#pragma once

#include "nautilus/tracing/Snapshot.hpp"
#include "nautilus/tracing/TracingUtil.hpp"
#include "nautilus/tracing/tag/Tag.hpp"
#include <any>
#include <nautilus/common/FunctionAttributes.hpp>
#include <optional>
#include <string>
#include <type_traits>
#include <utility>
#include <variant>
#include <vector>

namespace nautilus::tracing {

class None {};
using BranchProbability = double;

struct FunctionCall {
	std::string functionName;
	std::string mangledName;
	void* ptr;
	std::vector<TypedValueRef> arguments;
	FunctionAttributes fnAttrs;
};

struct BlockRef {
	explicit BlockRef(uint16_t block);
	uint16_t block;
	std::vector<TypedValueRef> arguments;
};

using InputVariant = std::variant<TypedValueRef, None, ConstantLiteral, BlockRef, FunctionCall, BranchProbability>;

/**
 * @brief Source code location information captured from val<>
 */
struct SourceLocationInfo {
	std::string file;
	uint32_t line;
	uint32_t column;
	std::string function;

	SourceLocationInfo() = default;

	SourceLocationInfo(const std::string& f, uint32_t l, uint32_t c, const std::string& fn)
		: file(f), line(l), column(c), function(fn) {}

	bool has_location() const {
		return !file.empty() && line > 0;
	}
};

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

	// NEW: Source location and semantic naming
	std::optional<SourceLocationInfo> source_location;
	std::optional<std::string> semantic_name;
};

} // namespace nautilus::tracing

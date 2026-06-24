
#pragma once

#include "nautilus/tracing/Types.hpp"
namespace nautilus::tracing {

using ValueRef = uint32_t;

/// Identifies the two successor blocks created by an explicit (non-symbolic)
/// conditional branch. Returned by the explicit control-flow primitives so the
/// closure-style If/While/For constructs in control_flow.hpp can route their
/// bodies into the correct trace blocks.
struct ExplicitCmpBlocks {
	uint32_t trueBlock = 0;
	uint32_t falseBlock = 0;
};

struct TypedValueRef {
	ValueRef ref;
	Type type;

	TypedValueRef(ValueRef ref, Type type);
	TypedValueRef();

	bool operator==(const TypedValueRef& rhs) const {
		return ref == rhs.ref;
	}

	bool operator!=(const TypedValueRef& rhs) const {
		return !(rhs == *this);
	}

	bool operator<(const TypedValueRef& rhs) const {
		return ref < rhs.ref;
	}

	bool operator>(const TypedValueRef& rhs) const {
		return rhs < *this;
	}

	bool operator<=(const TypedValueRef& rhs) const {
		return !(rhs < *this);
	}

	bool operator>=(const TypedValueRef& rhs) const {
		return !(*this < rhs);
	}
};

class TypedValueRefHolder {
public:
	TypedValueRefHolder(TypedValueRef valueRef);
	TypedValueRefHolder(const TypedValueRefHolder&);
	TypedValueRefHolder& operator=(const TypedValueRefHolder& other);
	TypedValueRefHolder(const TypedValueRefHolder&&);
	TypedValueRefHolder& operator=(TypedValueRefHolder&& other);
	~TypedValueRefHolder();
	operator const TypedValueRef&() const;
	TypedValueRef valueRef;
};

} // namespace nautilus::tracing

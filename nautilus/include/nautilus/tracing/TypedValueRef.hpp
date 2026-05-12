
#pragma once

#include "nautilus/tracing/Types.hpp"
namespace nautilus::tracing {

using ValueRef = uint32_t;
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
	// Position-invariant identity of the val<T> that owns this holder, used as
	// the key for AliveVariableHash so that two iterations producing val<T>s
	// at the same source position hash identically (review item F).  Derived
	// from the trace context's most-recently-recorded Tag* (or an argument
	// sentinel) at construction time.  Outside an active trace this is 0 and
	// inert because allocate/freeValRef short-circuit on a null tracer.
	uint64_t positionId;
};

} // namespace nautilus::tracing

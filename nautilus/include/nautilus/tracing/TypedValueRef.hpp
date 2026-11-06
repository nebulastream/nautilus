
#pragma once

#include "nautilus/config.hpp"
#include "nautilus/tracing/AliveVariableHash.hpp"
#include "nautilus/tracing/Types.hpp"
#include <utility>
namespace nautilus::tracing {

class TracingInterface;

using ValueRef = uint32_t;

namespace detail {
/// The active tracer of this thread, and the reference counts of its current scope when they can be updated without a
/// virtual call (null while the scope is paused or no tracer is active). Both are inline thread-local variables: an
/// executable's access is a single `%fs`-relative load.
inline thread_local TracingInterface* activeTracer = nullptr;
inline thread_local AliveVariableHash* aliveFast = nullptr;
/// The slow paths: ask the tracer (paused scopes, values of an enclosing region, no tracer).
void allocateValRefSlow(ValueRef ref);
void freeValRefSlow(ValueRef ref);
} // namespace detail

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

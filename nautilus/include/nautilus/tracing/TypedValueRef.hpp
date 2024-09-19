
#pragma once

#include "nautilus/tracing/Types.hpp"
namespace nautilus::tracing {

using ValueRef = uint16_t;
struct TypedValueRef {
	ValueRef ref;
	Type type;

	TypedValueRef(uint16_t ref, Type type);
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

	[[nodiscard]] std::string toString() const;
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

using value_ref = TypedValueRef;
} // namespace nautilus::tracing

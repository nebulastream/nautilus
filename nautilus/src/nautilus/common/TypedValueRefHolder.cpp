#include "nautilus/config.hpp"
#include "nautilus/tracing/TracingUtil.hpp"
#include <cassert>

namespace nautilus::tracing {

TypedValueRef::TypedValueRef() : ref(0), type(Type::v) {
}

TypedValueRef::TypedValueRef(ValueRef ref, nautilus::Type type) : ref(ref), type(type) {
}

namespace {
inline void acquire(ValueRef ref) {
	if (auto* alive = detail::aliveFast) {
		alive->increment(ref);
	} else if (detail::activeTracer != nullptr) {
		detail::allocateValRefSlow(ref);
	}
}
inline void release(ValueRef ref) {
	if (auto* alive = detail::aliveFast) {
		if (alive->tryDecrement(ref)) {
			return;
		}
	}
	if (detail::activeTracer != nullptr) {
		detail::freeValRefSlow(ref);
	}
}
} // namespace

TypedValueRefHolder::operator const TypedValueRef&() const {
	return valueRef;
}

TypedValueRefHolder::TypedValueRefHolder(nautilus::tracing::TypedValueRef valueRef) : valueRef(valueRef) {
#ifdef ENABLE_TRACING
	acquire(valueRef.ref);
#endif
}

TypedValueRefHolder::TypedValueRefHolder(const nautilus::tracing::TypedValueRefHolder& other)
    : valueRef(other.valueRef) {
#ifdef ENABLE_TRACING
	acquire(valueRef.ref);
#endif
}

TypedValueRefHolder::TypedValueRefHolder(const nautilus::tracing::TypedValueRefHolder&& other)
    : valueRef(other.valueRef) {
#ifdef ENABLE_TRACING
	acquire(valueRef.ref);
#endif
}

TypedValueRefHolder& TypedValueRefHolder::operator=(const nautilus::tracing::TypedValueRefHolder& other) {
#ifdef ENABLE_TRACING
	acquire(valueRef.ref);
#endif
	valueRef = other.valueRef;
	return *this;
}

TypedValueRefHolder& TypedValueRefHolder::operator=(nautilus::tracing::TypedValueRefHolder&& other) {
	valueRef = std::move(other.valueRef);
	return *this;
}

TypedValueRefHolder::~TypedValueRefHolder() {
#ifdef ENABLE_TRACING
	release(valueRef.ref);
#endif
}

} // namespace nautilus::tracing

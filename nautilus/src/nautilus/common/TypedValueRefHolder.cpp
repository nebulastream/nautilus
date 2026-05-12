#include "nautilus/config.hpp"
#include "nautilus/tracing/TracingUtil.hpp"
#include <cassert>

namespace nautilus::tracing {

TypedValueRef::TypedValueRef() : ref(0), type(Type::v) {
}

TypedValueRef::TypedValueRef(ValueRef ref, nautilus::Type type) : ref(ref), type(type) {
}

TypedValueRefHolder::operator const TypedValueRef&() const {
	return valueRef;
}

TypedValueRefHolder::TypedValueRefHolder(nautilus::tracing::TypedValueRef valueRef)
    : valueRef(valueRef), positionId(0) {
#ifdef ENABLE_TRACING
	positionId = tracing::currentPositionId();
	tracing::allocateValRef(positionId);
#endif
}

TypedValueRefHolder::TypedValueRefHolder(const nautilus::tracing::TypedValueRefHolder& other)
    : valueRef(other.valueRef), positionId(other.positionId) {
#ifdef ENABLE_TRACING
	tracing::allocateValRef(positionId);
#endif
}

TypedValueRefHolder::TypedValueRefHolder(const nautilus::tracing::TypedValueRefHolder&& other)
    : valueRef(other.valueRef), positionId(other.positionId) {
#ifdef ENABLE_TRACING
	tracing::allocateValRef(positionId);
#endif
}

TypedValueRefHolder& TypedValueRefHolder::operator=(const nautilus::tracing::TypedValueRefHolder& other) {
#ifdef ENABLE_TRACING
	tracing::allocateValRef(positionId);
#endif
	valueRef = other.valueRef;
	positionId = other.positionId;
	return *this;
}

TypedValueRefHolder& TypedValueRefHolder::operator=(nautilus::tracing::TypedValueRefHolder&& other) {
	valueRef = std::move(other.valueRef);
	positionId = other.positionId;
	return *this;
}

TypedValueRefHolder::~TypedValueRefHolder() {
#ifdef ENABLE_TRACING
	tracing::freeValRef(positionId);
#endif
}

} // namespace nautilus::tracing

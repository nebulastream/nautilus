#include "nautilus/config.hpp"
#include "nautilus/tracing/TracingUtil.hpp"
#include <cassert>

namespace nautilus::tracing {

TypedValueRef::TypedValueRef() : ref(0), type(Type::v) {
}

TypedValueRef::TypedValueRef(uint16_t ref, nautilus::Type type) : ref(ref), type(type) {
}

TypedValueRefHolder::operator const TypedValueRef&() const {
	return valueRef;
}

TypedValueRefHolder::TypedValueRefHolder(nautilus::tracing::TypedValueRef valueRef) : valueRef(valueRef) {
#ifdef ENABLE_TRACING
	if (!inTracer())
		return;
	tracing::allocateValRef(valueRef.ref);
#endif
}

TypedValueRefHolder::TypedValueRefHolder(const nautilus::tracing::TypedValueRefHolder& other) : valueRef(other.valueRef) {
#ifdef ENABLE_TRACING
	if (!inTracer())
		return;
	tracing::allocateValRef(valueRef.ref);
#endif
}

TypedValueRefHolder::TypedValueRefHolder(const nautilus::tracing::TypedValueRefHolder&& other) : valueRef(other.valueRef) {
#ifdef ENABLE_TRACING
	if (!inTracer())
		return;
	tracing::allocateValRef(valueRef.ref);
#endif
}

TypedValueRefHolder& TypedValueRefHolder::operator=(const nautilus::tracing::TypedValueRefHolder& other) {
#ifdef ENABLE_TRACING
	tracing::allocateValRef(valueRef.ref);
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
	if (!inTracer())
		return;
	tracing::freeValRef(valueRef.ref);
#endif
}

} // namespace nautilus::tracing

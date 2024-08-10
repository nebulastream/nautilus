#include "nautilus/common/traceing.hpp"
#include "nautilus/config.hpp"
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
	auto& refCounter = tracing::getVarRefMap()[valueRef.ref];
	refCounter++;
#endif
}

TypedValueRefHolder::TypedValueRefHolder(const nautilus::tracing::TypedValueRefHolder& other) : valueRef(other.valueRef) {
#ifdef ENABLE_TRACING
	if (!inTracer())
		return;
	auto& refCounter = tracing::getVarRefMap()[valueRef.ref];
	refCounter++;
#endif
}

TypedValueRefHolder::TypedValueRefHolder(nautilus::tracing::TypedValueRefHolder&& other) : valueRef(other.valueRef) {
}

TypedValueRefHolder& TypedValueRefHolder::operator=(const nautilus::tracing::TypedValueRefHolder& other) {
#ifdef ENABLE_TRACING
	auto& refCounter = tracing::getVarRefMap()[valueRef.ref];
	refCounter++;
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
	auto& refCounter = tracing::getVarRefMap()[valueRef.ref];
	// the ref counter should always be greater than zero.
	assert(refCounter > 0);
	refCounter--;
#endif
}

} // namespace nautilus::tracing
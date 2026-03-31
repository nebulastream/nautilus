#pragma once

#include "nautilus/any_val.hpp"

#ifdef ENABLE_TRACING
#include "nautilus/tracing/TypedValueRef.hpp"
#endif

namespace nautilus {

/// Intermediate base class for val<T> specializations that own tracing state.
///
/// Consolidates the `state` member and `getState()` override that would
/// otherwise be duplicated in every val<T> specialization.  Specializations
/// that do NOT own their tracing state directly (e.g. val<FuncPtr> which
/// aliases an inner pointer's state, or val<Class> which delegates to an
/// inner val<T*>) should inherit from any_val directly instead.
class traceable_val : public any_val {
public:
#ifdef ENABLE_TRACING
	traceable_val(const traceable_val&) = default;
	traceable_val(traceable_val&&) = default;
	traceable_val& operator=(const traceable_val&) = delete;
	traceable_val& operator=(traceable_val&&) = delete;
	~traceable_val() override = default;

	[[nodiscard]] tracing::TypedValueRef getState() const override {
		return state;
	}

	const tracing::TypedValueRefHolder state;

protected:
	explicit traceable_val(tracing::TypedValueRef ref) : state(ref) {
	}
	explicit traceable_val(tracing::TypedValueRefHolder holder) : state(std::move(holder)) {
	}
#else
	traceable_val() = default;
	traceable_val(const traceable_val&) = default;
	traceable_val(traceable_val&&) = default;
	traceable_val& operator=(const traceable_val&) = default;
	traceable_val& operator=(traceable_val&&) = default;
	~traceable_val() override = default;
#endif
};

} // namespace nautilus

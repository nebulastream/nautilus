#pragma once

#include "nautilus/function.hpp"
#include "nautilus/tracing/TracingUtil.hpp"
#include "nautilus/val_base.hpp"
#include "nautilus/val_concepts.hpp"
#include "nautilus/val_details.hpp"
#include "nautilus/val_ptr.hpp"
#include <type_traits>

namespace nautilus {

/// val<> specialization for function pointer types R(*)(Args...).
///
/// Internally wraps a val<void*> to carry the function address and its
/// tracing state, following the same pattern as val<T*> in val_ptr.hpp.
///
/// Supported operations:
///   - Construction from a raw function pointer or nullptr
///   - Copy construction / copy assignment
///   - Calling through the pointer via operator() — delegates to invoke(),
///     which handles both tracing and non-tracing paths.  At trace time the
///     concrete runtime address is used, so the traced ProxyCallOperation is
///     specialised for that specific address.
///   - Null check via explicit == nullptr / != nullptr (no implicit operator bool())
///   - Pointer equality: == and != with another val<FuncPtr> of the same type
///   - Implicit conversion to val<void*> for passing to runtime functions that
///     accept a generic opaque callback pointer
///
/// Limitations:
///   - Capturing lambdas and std::bind results cannot be wrapped here because
///     they are callable objects, not plain function pointers.  Use the
///     memberFunc() helper in function.hpp instead.
///   - Non-capturing lambdas can be decayed to a function pointer first using
///     the unary + operator: make_fn_val(+myLambda).
///   - Member function pointers (R (C::*)(Args...)) have a different ABI;
///     use memberFunc() in function.hpp.

namespace details {

template <typename Derived, typename R, bool IsNoexcept, typename... Args>
class FunctionPointerVal : public val_base {
public:
	using raw_type = std::conditional_t<IsNoexcept, R (*)(Args...) noexcept, R (*)(Args...)>;
	using basic_type = raw_type;
	using return_type = R;

	[[nodiscard]] Type getType() const override {
		return Type::ptr;
	}

	[[nodiscard]] TypeId getTypeId() const override {
		return typeIdOf<Derived>();
	}

#ifdef ENABLE_TRACING
	[[nodiscard]] tracing::TypedValueRef getState() const override {
		return state;
	}
#endif

	FunctionPointerVal() : ptr(static_cast<void*>(nullptr)) {
	}

	FunctionPointerVal(raw_type fnptr) : ptr(reinterpret_cast<void*>(fnptr)) {
	}

	FunctionPointerVal(std::nullptr_t) : ptr(static_cast<void*>(nullptr)) {
	}

	FunctionPointerVal(const FunctionPointerVal& other) : ptr(other.ptr) {
	}

	FunctionPointerVal(FunctionPointerVal&& other) : ptr(std::move(other.ptr)) {
	}

	FunctionPointerVal& operator=(const FunctionPointerVal& other) {
		ptr = other.ptr;
		return *this;
	}

#ifdef ENABLE_TRACING
	// Constructors used by val<void*>::operator val<OtherType>() when casting
	// an opaque void* back to a typed function pointer val.
	FunctionPointerVal(raw_type fnptr, tracing::TypedValueRef tc) : ptr(reinterpret_cast<void*>(fnptr), tc) {
	}

	FunctionPointerVal(raw_type fnptr, const tracing::TypedValueRefHolder& tc)
	    : ptr(reinterpret_cast<void*>(fnptr), static_cast<tracing::TypedValueRef>(tc)) {
	}

	// Constructor used by createTraceableArgument() in Engine.hpp: receives a
	// traced function argument reference with no known concrete address yet.
	explicit FunctionPointerVal(tracing::TypedValueRef tc) : ptr(tc) {
	}
#endif

	// ---- Tracing state ----
	// StateResolver<val<FuncPtr>> accesses .state, so expose it as a const
	// reference to the inner val<void*>'s state.
#ifdef ENABLE_TRACING
	const tracing::TypedValueRefHolder& state = ptr.state;
#endif

	// ---- Null check ----
	// Intentionally no implicit `operator bool()`. Converting a function pointer to a
	// native bool materializes the null test into a control-flow branch (traceBool)
	// that can be split from the call/dereference it guards, leaving an unchecked use
	// that LLVM assumes non-null and miscompiles at -O3. Use the explicit
	// `== nullptr` / `!= nullptr` below, which stay symbolic `val<bool>` data values.

	// ---- Equality ----

	friend val<bool> operator==(const Derived& left, const Derived& right) {
		return left.ptr == right.ptr;
	}

	friend val<bool> operator==(const Derived& left, std::nullptr_t) {
		return left.ptr == nullptr;
	}

	friend val<bool> operator!=(const Derived& left, const Derived& right) {
		return left.ptr != right.ptr;
	}

	friend val<bool> operator!=(const Derived& left, std::nullptr_t) {
		return left.ptr != nullptr;
	}

	// ---- Conversion to val<void*> ----

	operator val<void*>() const {
		return ptr;
	}

	// ---- Indirect call ----
	// When tracing is active, records an IndirectCallOperation so the
	// function pointer is treated as a runtime SSA value rather than a
	// compile-time constant address.  When not tracing, calls through the
	// raw function pointer directly.

	template <typename... ValueArgs>
	decltype(auto) operator()(ValueArgs&&... args) {
#ifdef ENABLE_TRACING
		if (tracing::inTracer()) {
			auto fnPtrRef = details::StateResolver<const val<void*>&>::getState(ptr);
			auto argRefs = getArgumentReferences(std::forward<ValueArgs>(args)...);
			auto attributes = FunctionAttributes {.noUnwind = IsNoexcept};
			if constexpr (std::is_void_v<R>) {
				tracing::traceIndirectCall(
				    fnPtrRef, Type::v, argRefs, attributes,
				    ExceptionCaptureSpec {reinterpret_cast<void*>(exceptionCaptureFunction<R, Args...>())});
				return;
			} else {
				auto& resultRef = tracing::traceIndirectCall(
				    fnPtrRef, tracing::TypeResolver<R>::to_type(), argRefs, attributes,
				    ExceptionCaptureSpec {reinterpret_cast<void*>(exceptionCaptureFunction<R, Args...>())});
				return val<R>(resultRef);
			}
		}
#endif
		auto rawPtr = reinterpret_cast<raw_type>(details::RawValueResolver<void*>::getRawValue(ptr));
		if constexpr (std::is_void_v<R>) {
			rawPtr(details::RawValueResolver<Args>::getRawValue(std::forward<ValueArgs>(args))...);
		} else {
			return val<R>(rawPtr(details::RawValueResolver<Args>::getRawValue(std::forward<ValueArgs>(args))...));
		}
	}

private:
	val<void*> ptr;
};

} // namespace details

template <typename R, typename... Args>
class val<R (*)(Args...)> : public details::FunctionPointerVal<val<R (*)(Args...)>, R, false, Args...> {
public:
	using details::FunctionPointerVal<val<R (*)(Args...)>, R, false, Args...>::FunctionPointerVal;
};

template <typename R, typename... Args>
class val<R (*)(Args...) noexcept>
    : public details::FunctionPointerVal<val<R (*)(Args...) noexcept>, R, true, Args...> {
public:
	using details::FunctionPointerVal<val<R (*)(Args...) noexcept>, R, true, Args...>::FunctionPointerVal;
};

// ---- Free helper ----

/// Wraps a raw function pointer in a val<R(*)(Args...)>.
/// Non-capturing lambdas can be decayed first: make_fn_val(+myLambda).
template <typename R, typename... Args>
val<R (*)(Args...)> make_fn_val(R (*fnptr)(Args...)) {
	return val<R (*)(Args...)>(fnptr);
}

template <typename R, typename... Args>
val<R (*)(Args...) noexcept> make_fn_val(R (*fnptr)(Args...) noexcept) {
	return val<R (*)(Args...) noexcept>(fnptr);
}

} // namespace nautilus

namespace nautilus::details {

/// RawValueResolver specialisation for function pointer types R(*)(Args...).
///
/// Recovers the typed function pointer from a val<R(*)(Args...)> by extracting
/// the stored void* and reinterpreting it as the concrete type.  This lets
/// invoke() pass a val<FuncPtr> to a callee that declares the exact typed
/// function pointer parameter.
template <typename R, typename... Args>
struct RawValueResolver<R (*)(Args...)> {
	using raw_type = R (*)(Args...);

	static raw_type getRawValue(const val<raw_type>& v) {
		return reinterpret_cast<raw_type>(RawValueResolver<void*>::getRawValue(static_cast<val<void*>>(v)));
	}
};

template <typename R, typename... Args>
struct RawValueResolver<R (*)(Args...) noexcept> {
	using raw_type = R (*)(Args...) noexcept;

	static raw_type getRawValue(const val<raw_type>& v) {
		return reinterpret_cast<raw_type>(RawValueResolver<void*>::getRawValue(static_cast<val<void*>>(v)));
	}
};

} // namespace nautilus::details

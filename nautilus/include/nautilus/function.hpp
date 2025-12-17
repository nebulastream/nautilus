#pragma once

#include "nautilus/common/FunctionAttributes.hpp"
#include "nautilus/val.hpp"
#include "nautilus/val_ptr.hpp"
#include <functional>
#include <stdexcept>
#include <type_traits>
#include <utility>

namespace nautilus {

template <typename... ValueArguments>
auto getArgumentReferences(const ValueArguments&... arguments) {
	return std::vector<tracing::TypedValueRef> {details::StateResolver<const ValueArguments&>::getState(arguments)...};
}

template <typename R, typename... FunctionArguments>
class CallableRuntimeFunction {
public:
	explicit CallableRuntimeFunction(R (*fnptr)(FunctionArguments...)) : fnptr(fnptr) {
	}

	explicit CallableRuntimeFunction(R (*fnptr)(FunctionArguments...), const FunctionAttributes fnAttrs)
	    : fnAttrs(fnAttrs), fnptr(fnptr) {
	}

	template <typename... FunctionArgumentsRaw>
	    requires(!std::is_void_v<R>)
	auto operator()(FunctionArgumentsRaw&&... args) {
#ifdef ENABLE_TRACING
		if (tracing::inTracer()) {
			auto functionArgumentReferences = getArgumentReferences(std::forward<FunctionArgumentsRaw>(args)...);
			auto resultRef = tracing::traceCall(reinterpret_cast<void*>(fnptr), tracing::TypeResolver<R>::to_type(),
			                                    functionArgumentReferences, fnAttrs);
			return val<R>(resultRef);
		}
#endif
		return val<R>(fnptr(
		    details::RawValueResolver<FunctionArguments>::getRawValue(std::forward<FunctionArgumentsRaw>(args))...));
	}

	template <typename... FunctionArgumentsRaw>
	    requires std::is_void_v<R>
	void operator()(FunctionArgumentsRaw&&... args) {
#ifdef ENABLE_TRACING
		if (tracing::inTracer()) {
			auto functionArgumentReferences = getArgumentReferences(std::forward<FunctionArgumentsRaw>(args)...);
			tracing::traceCall(reinterpret_cast<void*>(fnptr), Type::v, functionArgumentReferences, fnAttrs);
			return;
		}
#endif
		(fnptr(details::RawValueResolver<FunctionArguments>::getRawValue(std::forward<FunctionArgumentsRaw>(args))...));
	}

	template <is_integral... FunctionArgumentsRaw>
	auto invoke(FunctionArgumentsRaw&&... args) {
		return (*this)(std::forward<FunctionArgumentsRaw>(args)...);
	}

private:
	FunctionAttributes fnAttrs;
	R (*fnptr)(FunctionArguments...);
};

/// Invoke calls without attributes
template <typename R, typename... FunctionArguments, typename... ValueArguments>
auto invoke(R (*fnptr)(FunctionArguments...), ValueArguments&&... args) {
	return CallableRuntimeFunction<R, FunctionArguments...>(fnptr)(std::forward<ValueArguments>(args)...);
}

template <typename R, typename... FunctionArguments, typename... ValueArguments>
auto invoke(std::function<R(FunctionArguments...)> func, ValueArguments&&... args) {
        auto fnptr = func.template target<R (*)(FunctionArguments...)>();

        if (fnptr == nullptr) {
                throw std::invalid_argument("std::function target is not a raw function pointer");
        }

        return CallableRuntimeFunction<R, FunctionArguments...>(*fnptr)(std::forward<ValueArguments>(args)...);
}

template <is_fundamental... FunctionArguments, typename... ValueArguments>
void invoke(void (*fnptr)(FunctionArguments...), ValueArguments&&... args) {
        auto func = CallableRuntimeFunction<void, FunctionArguments...>(fnptr);
        func(std::forward<ValueArguments>(args)...);
}

/// Invoke calls with attributes
template <typename R, typename... FunctionArguments, typename... ValueArguments>
auto invoke(const FunctionAttributes fnAttrs, R (*fnptr)(FunctionArguments...), ValueArguments&&... args) {
	return CallableRuntimeFunction<R, FunctionArguments...>(fnptr, fnAttrs)(std::forward<ValueArguments>(args)...);
}

template <typename R, typename... FunctionArguments, typename... ValueArguments>
auto invoke(const FunctionAttributes fnAttrs, std::function<R(FunctionArguments...)> func, ValueArguments&&... args) {
        auto fnptr = func.template target<R (*)(FunctionArguments...)>();

        if (fnptr == nullptr) {
                throw std::invalid_argument("std::function target is not a raw function pointer");
        }

        return CallableRuntimeFunction<R, FunctionArguments...>(*fnptr, fnAttrs)(std::forward<ValueArguments>(args)...);
}

template <is_fundamental... FunctionArguments, typename... ValueArguments>
void invoke(const FunctionAttributes fnAttrs, void (*fnptr)(FunctionArguments...), ValueArguments&&... args) {
	auto func = CallableRuntimeFunction<void, FunctionArguments...>(fnptr, fnAttrs);
	func(std::forward<ValueArguments>(args)...);
}

template <typename R, typename... FunctionArguments>
auto function(R (*fnptr)(FunctionArguments...)) {
	return CallableRuntimeFunction<R, FunctionArguments...>(fnptr);
}

class MemberFuncWrapper {};

template <typename T, typename Rp, typename Tp>
class MemberFuncWrapperImpl : public MemberFuncWrapper {
public:
	MemberFuncWrapperImpl(T func)
	    : func(func), callableRuntimeFunction(function(+[](MemberFuncWrapper* ptr, Tp* clazzPtr) -> auto {
		      auto p = static_cast<MemberFuncWrapperImpl<T, Rp, Tp>*>(ptr);
		      Rp (Tp::*func)() = p->func;
		      return (*clazzPtr.*func)();
	      })) {
	}

	template <typename... FunctionArgumentsRaw>
	auto operator()(FunctionArgumentsRaw... args) {
		auto state = val<MemberFuncWrapper*>(this);
		return callableRuntimeFunction(state, args...);
	}
	T func;
	CallableRuntimeFunction<Rp, MemberFuncWrapper*, Tp*> callableRuntimeFunction;
};

template <typename T>
struct member_function_traits;

template <typename C, typename R, typename... Args>
struct member_function_traits<R (C::*)(Args...)> {
	using class_type = C;
	using return_type = R;
	using arg_types = std::tuple<Args...>;
};

template <typename T>
auto& memberFunc(T func) {
	using traits = member_function_traits<T>;
	using ClassType = typename traits::class_type;
	using ReturnType = typename traits::return_type;
	// using ArgTypes = typename traits::arg_types;
	auto ptr = new MemberFuncWrapperImpl<T, ReturnType, ClassType>(func);
	return *ptr;
}

} // namespace nautilus

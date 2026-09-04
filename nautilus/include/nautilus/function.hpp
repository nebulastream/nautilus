#pragma once

#include "nautilus/common/FunctionAttributes.hpp"
#include "nautilus/val.hpp"
#include "nautilus/val_ptr.hpp"
#include <functional>
#include <stdexcept>
#include <string>
#include <string_view>
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

/// A compiler-resolved function identified by its LLVM symbol rather than a
/// host function pointer. Symbolic calls can only be evaluated while Nautilus
/// is tracing; a compiler plugin must provide their implementation before the
/// generated program is executed.
template <typename R>
class CallableNamedRuntimeFunction {
public:
	explicit CallableNamedRuntimeFunction(std::string_view symbolName, FunctionAttributes fnAttrs = {})
	    : symbolName(symbolName), fnAttrs(fnAttrs) {
		if (symbolName.empty()) {
			throw std::invalid_argument("A symbolic function name must not be empty");
		}
	}

	template <typename... ValueArguments>
	    requires(!std::is_void_v<R>)
	val<R> operator()(ValueArguments&&... args) {
#ifdef ENABLE_TRACING
		if (tracing::inTracer()) {
			auto arguments = getArgumentReferences(std::forward<ValueArguments>(args)...);
			auto result = tracing::traceCall(symbolName, tracing::TypeResolver<R>::to_type(), arguments, fnAttrs);
			return val<R>(result);
		}
#endif
		throw std::logic_error("A symbolic function can only be invoked while tracing");
	}

	template <typename... ValueArguments>
	    requires std::is_void_v<R>
	void operator()(ValueArguments&&... args) {
#ifdef ENABLE_TRACING
		if (tracing::inTracer()) {
			auto arguments = getArgumentReferences(std::forward<ValueArguments>(args)...);
			tracing::traceCall(symbolName, Type::v, arguments, fnAttrs);
			return;
		}
#endif
		throw std::logic_error("A symbolic function can only be invoked while tracing");
	}

private:
	std::string symbolName;
	FunctionAttributes fnAttrs;
};

/// Invoke calls without attributes
template <typename R, typename... FunctionArguments, typename... ValueArguments>
auto invoke(R (*fnptr)(FunctionArguments...), ValueArguments&&... args) {
	return CallableRuntimeFunction<R, FunctionArguments...>(fnptr)(std::forward<ValueArguments>(args)...);
}

/// Invoke a compiler-resolved function by LLVM symbol name. The return type
/// is explicit because there is no C++ function pointer from which to infer it:
/// `invoke<int64_t>("python_udf", value)`.
template <typename R, typename... ValueArguments>
auto invoke(std::string_view symbolName, ValueArguments&&... args) {
	return CallableNamedRuntimeFunction<R>(symbolName)(std::forward<ValueArguments>(args)...);
}

template <typename R, typename... FunctionArguments, typename... ValueArguments>
auto invoke(std::function<R(FunctionArguments...)> func, ValueArguments&&... args) {
	auto fnptr = func.template target<R(FunctionArguments...)>();
	return CallableRuntimeFunction<R, FunctionArguments...>(fnptr)(std::forward<ValueArguments>(args)...);
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

template <typename R, typename... ValueArguments>
auto invoke(const FunctionAttributes fnAttrs, std::string_view symbolName, ValueArguments&&... args) {
	return CallableNamedRuntimeFunction<R>(symbolName, fnAttrs)(std::forward<ValueArguments>(args)...);
}

template <typename R, typename... FunctionArguments, typename... ValueArguments>
auto invoke(const FunctionAttributes fnAttrs, std::function<R(FunctionArguments...)> func, ValueArguments&&... args) {
	auto fnptr = func.template target<R(FunctionArguments...)>();
	return CallableRuntimeFunction<R, FunctionArguments...>(fnptr, fnAttrs)(std::forward<ValueArguments>(args)...);
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

template <auto Func>
auto& memberFunc() {
	using T = decltype(Func);
	using traits = member_function_traits<T>;
	using ClassType = typename traits::class_type;
	using ReturnType = typename traits::return_type;
	static auto* ptr = new MemberFuncWrapperImpl<T, ReturnType, ClassType>(Func);
	return *ptr;
}

} // namespace nautilus

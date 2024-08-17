#pragma once

#include "nautilus/val.hpp"
#include "nautilus/val_ptr.hpp"
#include <cxxabi.h>
#include <dlfcn.h>
#include <functional>
#include <type_traits>
#include <utility> // For std::forward

namespace nautilus {

template <typename R, typename... FunctionArguments>
std::string getFunctionName(R (*fnptr)(FunctionArguments...)) {
	Dl_info info;
	dladdr(reinterpret_cast<void*>(fnptr), &info);
	if (info.dli_sname != nullptr) {
		return info.dli_sname;
	}
	return "xxx";
}

template <is_traceable_value Arg>
tracing::value_ref getRefs(Arg& argument) {
	return argument.state;
}

template <typename... ValueArguments>
auto getArgumentReferences(std::string_view, void*, const ValueArguments&... arguments) {
	std::vector<tracing::value_ref> functionArgumentReferences = {};
	functionArgumentReferences.reserve(sizeof...(ValueArguments));
	if constexpr (sizeof...(ValueArguments) > 0) {
		for (const tracing::value_ref& p : {getRefs(arguments)...}) {
			functionArgumentReferences.emplace_back(p);
		}
	}
	return functionArgumentReferences;
}

template <typename R, typename... FunctionArguments>
class CallableRuntimeFunction {
public:
	explicit CallableRuntimeFunction(R (*fnptr)(FunctionArguments...), std::string& functionName)
	    : fnptr(fnptr), functionName(functionName) {}

	template <typename Arg>
	auto transform(Arg&& argument) {
		return details::getRawValue(argument);
	}

	template <typename... FunctionArgumentsRaw>
	    requires(!std::is_void_v<R>)
	auto operator()(FunctionArgumentsRaw&&... args) {
		// function is called from an external context.
#ifdef ENABLE_TRACING
		if (tracing::inTracer()) {
			auto ptr = (void*) fnptr;
			auto functionArgumentReferences = getArgumentReferences(functionName, (void*) fnptr, std::forward<FunctionArgumentsRaw>(args)...);
			auto resultRef = tracing::traceCall(functionName, ptr, tracing::to_type<R>(), functionArgumentReferences);
			return val<R>(resultRef);
		}
#endif
		return val<R>(fnptr(transform(std::forward<FunctionArgumentsRaw>(args))...));
	}

	template <typename... FunctionArgumentsRaw>
	    requires std::is_void_v<R>
	void operator()(FunctionArgumentsRaw&&... args) {
		// function is called from an external context.
#ifdef ENABLE_TRACING
		if (tracing::inTracer()) {
			auto ptr = (void*) fnptr;
			auto functionArgumentReferences = getArgumentReferences("functionName", (void*) fnptr, std::forward<FunctionArgumentsRaw>(args)...);
			tracing::traceCall(functionName, ptr, Type::v, functionArgumentReferences);
            return;
		}
#endif
		fnptr(transform(std::forward<FunctionArgumentsRaw>(args))...);
	}

	template <is_integral... FunctionArgumentsRaw>
	auto invoke(FunctionArgumentsRaw&&... args) {
		return (*this)(std::forward<FunctionArgumentsRaw>(args)...);
	}

private:
	R (*fnptr)(FunctionArguments...);
	std::string functionName;
};

template <typename R, typename... FunctionArguments, typename... ValueArguments>
auto invoke(R (*fnptr)(FunctionArguments...), ValueArguments&&... args) {
	[[maybe_unused]] auto name = getFunctionName(fnptr);
	return CallableRuntimeFunction<R, FunctionArguments...>(fnptr, name)(std::forward<ValueArguments>(args)...);
}

template <typename R, typename... FunctionArguments, typename... ValueArguments>
auto invoke(std::function<R(FunctionArguments...)> func, ValueArguments&&... args) {
	typedef R (*DecisionFn)(FunctionArguments...);
	DecisionFn fnptr = func.template target<R(FunctionArguments...)>();
	[[maybe_unused]] auto name = getFunctionName(fnptr);
	return CallableRuntimeFunction<R, FunctionArguments...>(fnptr, name)(std::forward<ValueArguments>(args)...);
}

template <is_fundamental... FunctionArguments, typename... ValueArguments>
void invoke(void (*fnptr)(FunctionArguments...), ValueArguments&&... args) {
	[[maybe_unused]] auto name = getFunctionName(fnptr);
	auto func = CallableRuntimeFunction<void, FunctionArguments...>(fnptr, name);
	func(std::forward<ValueArguments>(args)...);
}

template <class>
constexpr bool is_reference_wrapper_v = false;
template <class U>
constexpr bool is_reference_wrapper_v<std::reference_wrapper<U>> = true;

template <class T>
using remove_cvref_t = std::remove_cv_t<std::remove_reference_t<T>>;

template <typename R, typename... FunctionArguments>
auto Function(R (*fnptr)(FunctionArguments...)) {
	return CallableNautilusFunction<R, FunctionArguments...>(fnptr);
}

} // namespace nautilus

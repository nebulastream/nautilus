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
auto getArgumentReferences(const ValueArguments&... arguments) {
	std::vector<tracing::value_ref> functionArgumentReferences;
	if constexpr (sizeof...(ValueArguments) > 0) {
		functionArgumentReferences.reserve(sizeof...(ValueArguments));
		for (const tracing::value_ref& p : {getRefs(arguments)...}) {
			functionArgumentReferences.emplace_back(p);
		}
	}
	return functionArgumentReferences;
}

template <typename R, typename... FunctionArguments>
class CallableRuntimeFunction {
public:
	explicit CallableRuntimeFunction(R (*fnptr)(FunctionArguments...)) : fnptr(fnptr) {
	}

	template <typename... FunctionArgumentsRaw>
	    requires(!std::is_void_v<R>)
	auto operator()(FunctionArgumentsRaw&&... args) {
		// function is called from an external context.
#ifdef ENABLE_TRACING
		if (tracing::inTracer()) {
			auto functionArgumentReferences = getArgumentReferences(std::forward<FunctionArgumentsRaw>(args)...);
			auto resultRef = tracing::traceCall(getFunctionName(fnptr), reinterpret_cast<void*>(fnptr), tracing::to_type<R>(), functionArgumentReferences);
			return val<R>(resultRef);
		}
#endif
		return val<R>(fnptr(details::getRawValue(std::forward<FunctionArgumentsRaw>(args))...));
	}

	template <typename... FunctionArgumentsRaw>
	    requires std::is_void_v<R>
	void operator()(FunctionArgumentsRaw&&... args) {
		// function is called from an external context.
#ifdef ENABLE_TRACING
		if (tracing::inTracer()) {
			auto functionArgumentReferences = getArgumentReferences(std::forward<FunctionArgumentsRaw>(args)...);
			tracing::traceCall(getFunctionName(fnptr), reinterpret_cast<void*>(fnptr), Type::v, functionArgumentReferences);
			return;
		}
#endif
		fnptr(details::getRawValue(std::forward<FunctionArgumentsRaw>(args))...);
	}

	template <is_integral... FunctionArgumentsRaw>
	auto invoke(FunctionArgumentsRaw&&... args) {
		return (*this)(std::forward<FunctionArgumentsRaw>(args)...);
	}

private:
	R (*fnptr)(FunctionArguments...);
};

template <typename R, typename... FunctionArguments, typename... ValueArguments>
auto invoke(R (*fnptr)(FunctionArguments...), ValueArguments&&... args) {
	return CallableRuntimeFunction<R, FunctionArguments...>(fnptr)(std::forward<ValueArguments>(args)...);
}

template <typename R, typename... FunctionArguments, typename... ValueArguments>
auto invoke(std::function<R(FunctionArguments...)> func, ValueArguments&&... args) {
	typedef R (*DecisionFn)(FunctionArguments...);
	DecisionFn fnptr = func.template target<R(FunctionArguments...)>();
	return CallableRuntimeFunction<R, FunctionArguments...>(fnptr)(std::forward<ValueArguments>(args)...);
}

template <is_fundamental... FunctionArguments, typename... ValueArguments>
void invoke(void (*fnptr)(FunctionArguments...), ValueArguments&&... args) {
	auto func = CallableRuntimeFunction<void, FunctionArguments...>(fnptr);
	func(std::forward<ValueArguments>(args)...);
}

} // namespace nautilus


#pragma once

#include "nautilus/Executable.hpp"
#include "nautilus/JITCompiler.hpp"
#include "nautilus/config.hpp"
#include "nautilus/core.hpp"
#include "nautilus/options.hpp"
#include <functional>

namespace nautilus::engine {
namespace details {

/**
 * Creates an val argument that has a correct value ref for tracing
 * @tparam Arg
 * @tparam I
 * @return val<ArgValueType>
 */
template <typename ArgValueType, size_t I>
auto createTraceableArgument() {
	auto type = tracing::to_type<typename ArgValueType::raw_type>();
	auto valueRef = tracing::registerFunctionArgument(type, I);
	return val<typename ArgValueType::raw_type>(valueRef);
}

#ifdef ENABLE_TRACING

template <size_t... Indices, typename R, typename... FunctionArguments>
std::function<void()> createFunctionWrapper(std::index_sequence<Indices...>, std::function<R(FunctionArguments...)> func) {
	[[maybe_unused]] std::size_t args = sizeof...(FunctionArguments);
	auto traceFunc = [=]() {
		if constexpr (std::is_void_v<R>) {
			func(details::createTraceableArgument<FunctionArguments, Indices>()...);
			tracing::traceReturnOperation(Type::v, tracing::value_ref());
		} else {
			auto returnValue = func(details::createTraceableArgument<FunctionArguments, Indices>()...);
			auto type = tracing::to_type<typename decltype(returnValue)::raw_type>();
			tracing::traceReturnOperation(type, returnValue.state);
		}
	};
	return traceFunc;
}

template <typename R, typename... FunctionArguments>
std::function<void()> createFunctionWrapper(std::function<R(FunctionArguments...)> func) {
	return createFunctionWrapper(std::make_index_sequence<sizeof...(FunctionArguments)> {}, func);
}
#endif
} // namespace details

template <typename R, typename... FunctionArguments>
class CallableFunction {
public:
	explicit CallableFunction(std::function<R(val<FunctionArguments>...)> func) : func(func), executable(nullptr) {
	}

	explicit CallableFunction(std::unique_ptr<compiler::Executable>& executable) : func(), executable(std::move(executable)) {
	}

	auto operator()(FunctionArguments... args)
	    requires std::is_void_v<R>
	{
		// function is called from an external context.
		// no executable is defined, call the underling function directly and convert all arguments to val objects
		if (executable == nullptr) {
			func(make_value((args))...);
			return;
		}
		auto callable = this->executable->template getInvocableMember<void, FunctionArguments...>("execute");
		callable(args...);
	}

	auto operator()(FunctionArguments... args)
	    requires(!std::is_void_v<R>)
	{
		// function is called from an external context.
		// no executable is defined, call the underling function directly and convert all arguments to val objects
		if (executable == nullptr) {
			auto result = func(make_value((args))...);
			return nautilus::details::getRawValue(result);
		}
		auto callable = this->executable->template getInvocableMember<typename R::raw_type, FunctionArguments...>("execute");
		return callable(args...);
	}

private:
	std::function<R(val<FunctionArguments>...)> func;
	std::unique_ptr<compiler::Executable> executable;
};

/**
 * The Nautilus Engine maintains the execution context of one or multiple nautilus functions,
 * which are registered using registerFunction.
 * Depending on the provided options, this functions may be compiled using a compilation backend or are executed directly.
 * In general, the NautilusEngine mussed outlive any registered functions.
 */
class NautilusEngine {
public:
	NautilusEngine(const Options& options = Options());

	template <typename R, is_val... FunctionArguments>
	auto registerFunction(R (*fnptr)(val<FunctionArguments>...)) const {
		std::function<R(val<FunctionArguments>...)> inputFunction = fnptr;
		return registerFunction(inputFunction);
	}

	template <typename R, typename... FunctionArguments>
	auto registerFunction(std::function<R(val<FunctionArguments>...)> func) const {
#ifdef ENABLE_TRACING
		if (options.getOptionOrDefault("engine.Compilation", true)) {
			auto wrapper = details::createFunctionWrapper(func);
			auto executable = jit.compile(wrapper);
			return CallableFunction<R, FunctionArguments...>(executable);
		}
#endif
		return CallableFunction<R, FunctionArguments...>(func);
	}

private:
	const compiler::JITCompiler jit;
	const Options options;
};
} // namespace nautilus::engine

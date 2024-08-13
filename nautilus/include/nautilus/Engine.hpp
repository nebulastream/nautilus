
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

template <typename Arg>
auto transform(Arg argument) {
	return make_value(argument);
}

#ifdef ENABLE_TRACING

template <size_t... Indices, typename R, typename... FunctionArguments>
std::function<void()> createFunctionWrapper(std::index_sequence<Indices...>, std::function<R(FunctionArguments...)>  func) {
	[[maybe_unused]] std::size_t args = sizeof...(FunctionArguments);
	auto traceFunc = [=]() {
		if constexpr (std::is_void_v<R>) {
			func(details::createTraceableArgument<FunctionArguments, Indices>()...);
			tracing::traceReturnOperation(Type::v, tracing::value_ref());
		} else {
			auto returnValue = func(details::createTraceableArgument<FunctionArguments, Indices>()...);
			auto type = tracing::to_type<typename decltype(returnValue)::basic_type>();
			tracing::traceReturnOperation(type, returnValue.state);
		}
	};
	return traceFunc;
}

template <typename R, typename... FunctionArguments>
std::function<void()> createFunctionWrapper(std::function<R(FunctionArguments...)>  func) {
	return createFunctionWrapper(std::make_index_sequence<sizeof...(FunctionArguments)> {}, func);
}
#endif
} // namespace details

template <typename R, typename... FunctionArguments>
class CallableFunction {
public:
	explicit CallableFunction(std::function<R(FunctionArguments...)>  func) : func(func), executable(nullptr) {
	}

	explicit CallableFunction(std::unique_ptr<compiler::Executable>& executable)
	    : func(), executable(std::move(executable)) {}

	template <typename... FunctionArgumentsRaw>
	    requires std::is_void_v<R>
	auto operator()(FunctionArgumentsRaw... args) {
		// function is called from an external context.
		// no executable is defined, call the underling function directly and convert all arguments to val objects
		if (executable == nullptr) {
			func(details::transform((args))...);
			return;
		}
		auto callable =
		    this->executable->template getInvocableMember<void, FunctionArgumentsRaw...>("execute");
		callable(args...);
	}

	template <typename... FunctionArgumentsRaw>
	    requires(!std::is_void_v<R>)
	auto operator()(FunctionArgumentsRaw... args) {
		// function is called from an external context.
		// no executable is defined, call the underling function directly and convert all arguments to val objects
		if (executable == nullptr) {
			auto result = func(details::transform((args))...);
			return nautilus::details::getRawValue(result);
		}
		auto callable =
		    this->executable->template getInvocableMember<typename R::raw_type, FunctionArgumentsRaw...>("execute");
		return callable(args...);;
	}

private:
	std::function<R(FunctionArguments...)> func;
	std::unique_ptr<compiler::Executable> executable;
};

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
			return CallableFunction<R, val<FunctionArguments>...>(executable);
		}
#endif
		return CallableFunction<R, val<FunctionArguments>...>(func);
	}

private:
	const compiler::JITCompiler jit;
	const Options options;
};
} // namespace nautilus::engine

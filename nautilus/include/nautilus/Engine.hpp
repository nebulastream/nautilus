
#pragma once

#include "nautilus/Executable.hpp"
#include "nautilus/JITCompiler.hpp"
#include "nautilus/config.hpp"
#include "nautilus/core.hpp"
#include "nautilus/options.hpp"
#include <functional>

namespace nautilus::engine {

namespace details {
template <typename T>
    requires std::is_fundamental_v<T>
val<T> createTraceVal(tracing::value_ref ref) {
	return val<T>(ref);
}

template <typename T>
    requires std::is_pointer_v<T>
val<T> createTraceVal(tracing::value_ref ref) {
	return val<T>(nullptr, ref);
}

template <typename Arg, size_t I>
auto createTraceArgument() {
	auto type = tracing::to_type<typename Arg::raw_type>();
	auto valueRef = tracing::registerFunctionArgument(type, I);
	return createTraceVal<typename Arg::raw_type>(valueRef);
}

template <typename Arg>
auto transform(Arg argument) {
	return make_value(argument);
}

#ifdef ENABLE_TRACING
template <size_t... Indices, typename R, typename... FunctionArguments>
std::function<void()> createFunctionWrapper(std::index_sequence<Indices...>, R (*fnptr)(FunctionArguments...)) {
	[[maybe_unused]] std::size_t args = sizeof...(FunctionArguments);
	auto traceFunc = [=]() {
		if constexpr (std::is_void_v<R>) {
			fnptr(details::createTraceArgument<FunctionArguments, Indices>()...);
			tracing::traceReturnOperation(Type::v, tracing::value_ref());
		} else {
			auto returnValue = fnptr(details::createTraceArgument<FunctionArguments, Indices>()...);
			auto type = tracing::to_type<typename decltype(returnValue)::basic_type>();
			tracing::traceReturnOperation(type, returnValue.state);
		}
	};
	return traceFunc;
}

template <typename R, typename... FunctionArguments>
std::function<void()> createFunctionWrapper(R (*fnptr)(FunctionArguments...)) {
	return createFunctionWrapper(std::make_index_sequence<sizeof...(FunctionArguments)> {}, fnptr);
}

template <size_t... Indices, typename R, typename... FunctionArguments>
std::function<void()> createFunctionWrapper(std::index_sequence<Indices...>, std::function<R(FunctionArguments...)>  func) {
	[[maybe_unused]] std::size_t args = sizeof...(FunctionArguments);
	auto traceFunc = [=]() {
		if constexpr (std::is_void_v<R>) {
			func(details::createTraceArgument<FunctionArguments, Indices>()...);
			tracing::traceReturnOperation(Type::v, tracing::value_ref());
		} else {
			auto returnValue = func(details::createTraceArgument<FunctionArguments, Indices>()...);
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
		return;
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
	NautilusEngine();

	NautilusEngine(const Options& options);

	template <is_val R, is_val... FunctionArguments>
	auto registerFunction(val<R> (*fnptr)(val<FunctionArguments>...)) const {

#ifdef ENABLE_TRACING
		if (options.getOptionOrDefault("engine.Compilation", true)) {
			auto wrapper = details::createFunctionWrapper(fnptr);
			auto executable = jit.compile(wrapper);
			return CallableFunction<val<R>, val<FunctionArguments>...>(executable);
		}
#endif
		std::function<val<R>(val<FunctionArguments>...)> inputWrapper = fnptr;
		return CallableFunction<val<R>, val<FunctionArguments>...>(inputWrapper);
	}

	template <typename... FunctionArguments>
	auto registerFunction(void (*fnptr)(FunctionArguments...)) const {
#ifdef ENABLE_TRACING
		if (options.getOptionOrDefault("engine.Compilation", true)) {
			auto wrapper = details::createFunctionWrapper(fnptr);
			auto executable = jit.compile(wrapper);
			return CallableFunction<void, FunctionArguments...>(executable);
		}
#endif
		std::function<void(FunctionArguments...)> inputWrapper = fnptr;
		return CallableFunction<void, FunctionArguments...>(inputWrapper);
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

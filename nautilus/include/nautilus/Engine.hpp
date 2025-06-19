
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
	auto type = tracing::TypeResolver<typename ArgValueType::raw_type>::to_type();
	auto valueRef = tracing::registerFunctionArgument(type, I);
	return val<typename ArgValueType::raw_type>(valueRef);
}

#ifdef ENABLE_TRACING

template <size_t... Indices, typename R, typename... FunctionArguments>
std::function<void()> createFunctionWrapper(std::index_sequence<Indices...>,
                                            std::function<R(FunctionArguments...)> func) {
	[[maybe_unused]] std::size_t args = sizeof...(FunctionArguments);
	return [=]() {
		if constexpr (std::is_void_v<R>) {
			func(details::createTraceableArgument<FunctionArguments, Indices>()...);
			tracing::traceReturnOperation(Type::v, tracing::TypedValueRef());
		} else {
			auto returnValue = func(details::createTraceableArgument<FunctionArguments, Indices>()...);
			auto type = tracing::TypeResolver<typename decltype(returnValue)::raw_type>::to_type();
			tracing::traceReturnOperation(type, returnValue.state);
		}
	};
}

template <typename R, typename... FunctionArguments>
std::function<void()> createFunctionWrapper(std::function<R(FunctionArguments...)> func) {
	return createFunctionWrapper(std::make_index_sequence<sizeof...(FunctionArguments)> {}, func);
}
#endif
} // namespace details

template <class... Ts>
struct overloaded : Ts... {
	using Ts::operator()...;
};
template <class... Ts>
overloaded(Ts...) -> overloaded<Ts...>;

template <typename R, typename... FunctionArguments>
class CallableFunction {
public:
	explicit CallableFunction(std::function<R(val<FunctionArguments>...)> func) : func(func), executable(nullptr) {
	}

	explicit CallableFunction(std::unique_ptr<compiler::Executable>& executable)
	    : func(executable->getInvocableMember<typename R::raw_type, FunctionArguments...>("execute")),
	      executable(std::move(executable)) {}

		CallableFunction(const CallableFunction& other) = delete;
		CallableFunction(CallableFunction && other) noexcept
		    : func(std::move(other.func)), executable(std::move(other.executable)) {
		}
		CallableFunction& operator=(const CallableFunction& other) = delete;
		CallableFunction& operator=(CallableFunction&& other) noexcept {
			if (this == &other)
				return *this;
			func = std::move(other.func);
			executable = std::move(other.executable);
			return *this;
		}

		typename R::raw_type operator()(FunctionArguments... args) {
			return std::visit(
			    overloaded {[&](std::function<R(val<FunctionArguments>...)>& fn) -> typename R::raw_type {
				                return nautilus::details::RawValueResolver<typename R::raw_type>::getRawValue(
				                    fn(make_value(args)...));
			                },
			                [&](compiler::Executable::Invocable<typename R::raw_type, FunctionArguments...>& fn) ->
			                typename R::raw_type {
				                return fn(args...);
			                }},
			    func);
		}

	private:
		std::variant<std::function<R(val<FunctionArguments>...)>,
		             compiler::Executable::Invocable<typename R::raw_type, FunctionArguments...>>
		    func;
		std::unique_ptr<compiler::Executable> executable;
	};

	/// Specialization for void return type
	template <typename... FunctionArguments>
	class CallableFunction<void, FunctionArguments...> {
	public:
		explicit CallableFunction(std::function<void(val<FunctionArguments>...)> func)
		    : func(func), executable(nullptr) {
		}

		explicit CallableFunction(std::unique_ptr<compiler::Executable>& executable)
		    : func(executable->getInvocableMember<void, FunctionArguments...>("execute")),
		      executable(std::move(executable)) {
		}

		auto operator()(FunctionArguments... args) {
			std::visit(overloaded {[&](std::function<void(val<FunctionArguments>...)>& fn) { fn(make_value(args)...); },
			                       [&](compiler::Executable::Invocable<void, FunctionArguments...>& fn) {
				                       fn(args...);
			                       }},
			           func);
		}

	private:
		std::variant<std::function<void(val<FunctionArguments>...)>,
		             compiler::Executable::Invocable<void, FunctionArguments...>>
		    func;
		std::unique_ptr<compiler::Executable> executable;
	};

	/**
	 * The Nautilus Engine maintains the execution context of one or multiple nautilus functions,
	 * which are registered using registerFunction.
	 * Depending on the provided options, this functions may be compiled using a compilation backend or are executed
	 * directly. In general, the NautilusEngine mussed outlive any registered functions.
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

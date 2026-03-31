
#pragma once

#include "nautilus/Executable.hpp"
#include "nautilus/JITCompiler.hpp"
#include "nautilus/Module.hpp"
#include "nautilus/config.hpp"
#include "nautilus/core.hpp"
#include "nautilus/options.hpp"
#include <any>
#include <functional>
#include <unordered_map>

#ifdef ENABLE_TRACING
#include "nautilus/CompilableFunction.hpp"
#endif

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
	return std::function([func = std::move(func)]() {
		if constexpr (std::is_void_v<R>) {
			func(details::createTraceableArgument<FunctionArguments, Indices>()...);
			tracing::traceReturnOperation(Type::v, tracing::TypedValueRef());
		} else {
			auto returnValue = func(details::createTraceableArgument<FunctionArguments, Indices>()...);
			auto type = tracing::TypeResolver<typename decltype(returnValue)::raw_type>::to_type();
			tracing::traceReturnOperation(type, returnValue.state);
		}
	});
}

template <typename R, typename... FunctionArguments>
std::function<void()> createFunctionWrapper(std::function<R(FunctionArguments...)> func) {
	return createFunctionWrapper(std::make_index_sequence<sizeof...(FunctionArguments)> {}, func);
}

// Optimized overload for raw function pointers - avoids std::function wrapper overhead
template <size_t... Indices, typename R, typename... FunctionArguments>
std::function<void()> createFunctionWrapper(std::index_sequence<Indices...>, R (*fnptr)(FunctionArguments...)) {
	[[maybe_unused]] std::size_t args = sizeof...(FunctionArguments);
	auto traceFunc = [=]() {
		if constexpr (std::is_void_v<R>) {
			fnptr(details::createTraceableArgument<FunctionArguments, Indices>()...);
			tracing::traceReturnOperation(Type::v, tracing::TypedValueRef());
		} else {
			auto returnValue = fnptr(details::createTraceableArgument<FunctionArguments, Indices>()...);
			auto type = tracing::TypeResolver<typename decltype(returnValue)::raw_type>::to_type();
			tracing::traceReturnOperation(type, returnValue.state);
		}
	};
	return traceFunc;
}

template <typename R, typename... FunctionArguments>
std::function<void()> createFunctionWrapper(R (*fnptr)(FunctionArguments...)) {
	return createFunctionWrapper(std::make_index_sequence<sizeof...(FunctionArguments)> {}, fnptr);
}

// Overload for general callables (lambdas, functors, etc.)
// This uses SFINAE to detect if F is callable but not a std::function or function pointer
template <typename F, typename = void>
struct is_std_function : std::false_type {};

template <typename R, typename... Args>
struct is_std_function<std::function<R(Args...)>> : std::true_type {};

template <typename F>
inline constexpr bool is_std_function_v = is_std_function<std::decay_t<F>>::value;

template <typename F>
inline constexpr bool is_function_pointer_v = std::is_pointer_v<F> && std::is_function_v<std::remove_pointer_t<F>>;

template <typename F>
    requires(!is_std_function_v<F> && !is_function_pointer_v<F>)
std::function<void()> createFunctionWrapper(F&& func) {
	// Convert the callable to std::function - deduction will happen at call site
	return createFunctionWrapper(std::function(std::forward<F>(func)));
}
/// Helper struct that creates specialized function wrappers where some arguments
/// are baked in as constants and the rest are symbolic traceable arguments.
/// SpecArgs are the 0-based indices of arguments to treat as constants.
template <size_t... SpecArgs>
struct SpecializedWrapperBuilder {

	/// Check if index I is one of the specialized arguments.
	template <size_t I>
	static constexpr bool isSpec() {
		return ((I == SpecArgs) || ...);
	}

	/// Compute the symbolic argument index for position I,
	/// counting only non-specialized positions before I.
	template <size_t I>
	static constexpr size_t symIndex() {
		size_t idx = 0;
		for (size_t j = 0; j < I; ++j) {
			if (!((j == SpecArgs) || ...)) {
				++idx;
			}
		}
		return idx;
	}

	/// Create a single argument at position I for the specialized wrapper.
	template <size_t I, typename ArgType>
	static auto makeArg(const std::unordered_map<size_t, std::any>& constants) {
		using raw_type = typename ArgType::raw_type;
		if constexpr (isSpec<I>()) {
			auto rawValue = std::any_cast<raw_type>(constants.at(I));
			return val<raw_type>(rawValue);
		} else {
			constexpr size_t sIdx = symIndex<I>();
			return createTraceableArgument<ArgType, sIdx>();
		}
	}

	/// Build the wrapper from a std::function and a constants map.
	template <size_t... Indices, typename R, typename... FunctionArguments>
	static std::function<void()> build(std::index_sequence<Indices...>, std::function<R(FunctionArguments...)> func,
	                                   std::unordered_map<size_t, std::any> constants) {
		return std::function([func = std::move(func), constants = std::move(constants)]() {
			if constexpr (std::is_void_v<R>) {
				func(SpecializedWrapperBuilder::makeArg<Indices, FunctionArguments>(constants)...);
				tracing::traceReturnOperation(Type::v, tracing::TypedValueRef());
			} else {
				auto returnValue = func(SpecializedWrapperBuilder::makeArg<Indices, FunctionArguments>(constants)...);
				auto type = tracing::TypeResolver<typename decltype(returnValue)::raw_type>::to_type();
				tracing::traceReturnOperation(type, returnValue.state);
			}
		});
	}
};

/// Creates a specialized function wrapper from a std::function.
/// Constants map holds {argIndex -> std::any(raw_value)} for each SpecArg.
template <size_t... SpecArgs, typename R, typename... FunctionArguments>
std::function<void()> createSpecializedFunctionWrapper(std::function<R(FunctionArguments...)> func,
                                                       std::unordered_map<size_t, std::any> constants) {
	return SpecializedWrapperBuilder<SpecArgs...>::build(std::make_index_sequence<sizeof...(FunctionArguments)> {},
	                                                     std::move(func), std::move(constants));
}

/// Public entry point: creates a specialized function wrapper.
/// F can be any callable; converted to std::function internally.
template <size_t... SpecArgs, typename F>
std::function<void()> createSpecializedFunctionWrapper(F& func, std::unordered_map<size_t, std::any> constants) {
	auto stdFunc = std::function(func);
	return createSpecializedFunctionWrapper<SpecArgs...>(std::move(stdFunc), std::move(constants));
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
	      executable(std::move(executable)) {
	}

	CallableFunction(const CallableFunction& other) = delete;
	CallableFunction(CallableFunction&& other) noexcept
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

	const compiler::Executable* getExecutable() {
		return executable.get();
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
	explicit CallableFunction(std::function<void(val<FunctionArguments>...)> func) : func(func), executable(nullptr) {
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
		if (isCompiled()) {
			auto wrapper = details::createFunctionWrapper(func);
			auto executable = jit.compile(wrapper);
			return CallableFunction<R, FunctionArguments...>(executable);
		}
#endif
		return CallableFunction<R, FunctionArguments...>(func);
	}

	/**
	 * @brief Creates a new module for registering multiple functions to be compiled together.
	 * @return NautilusModule builder
	 */
	NautilusModule createModule() const;

	std::string getNameOfBackend() const {
		return jit.getName();
	}

	bool isCompiled() const {
		return options.getOptionOrDefault("engine.Compilation", true);
	}

private:
	const compiler::JITCompiler jit;
	const Options options;
};

/**
 * @brief A module that accumulates multiple named function registrations and compiles them
 * together into a single compilation unit. All registered functions share one IR graph and
 * are compiled by the same backend in one pass.
 *
 * Usage:
 * @code
 * auto module = engine.createModule();
 * module.registerFunction<val<int32_t>(val<int32_t>)>("inc", [](val<int32_t> x) { return x + 1; });
 * module.registerFunction<val<int64_t>(val<int64_t>, val<int64_t>)>("sum",
 *     [](val<int64_t> a, val<int64_t> b) { return a + b; });
 * auto compiled = module.compile();
 * auto inc = compiled.getFunction<int32_t(int32_t)>("inc");
 * auto sum = compiled.getFunction<int64_t(int64_t, int64_t)>("sum");
 * @endcode
 */
class NautilusModule {
public:
#ifdef ENABLE_TRACING
	NautilusModule(const compiler::JITCompiler& jit, bool compiled) : jit_(jit), compiled_(compiled) {
	}
#else
	NautilusModule(const compiler::JITCompiler& /*jit*/, bool /*compiled*/) {
	}
#endif

	/**
	 * @brief Register a function with an explicit signature (needed for lambdas).
	 * @tparam Signature The val-typed function signature, e.g. val<int32_t>(val<int32_t>)
	 * @tparam F Callable type (lambda, functor)
	 * @param name Unique name for this function in the module
	 * @param func The callable to register
	 */
	template <typename Signature, typename F>
	void registerFunction(const std::string& name, F&& func) {
		using function_type = std::function<Signature>;
		function_type stdFunc(std::forward<F>(func));
		registerFunction(name, std::move(stdFunc));
	}

	/**
	 * @brief Register a function from a std::function.
	 * @param name Unique name for this function in the module
	 * @param func The std::function to register
	 */
	template <typename R, typename... FunctionArguments>
	void registerFunction(const std::string& name, std::function<R(val<FunctionArguments>...)> func) {
		interpretedFunctions_[name] = func;
#ifdef ENABLE_TRACING
		if (compiled_) {
			auto wrapper = details::createFunctionWrapper(std::move(func));
			functions_.emplace_back(name, std::move(wrapper));
		}
#endif
	}

	/**
	 * @brief Register a function from a function pointer.
	 * @param name Unique name for this function in the module
	 * @param fnptr The function pointer to register
	 */
	template <typename R, is_val... FunctionArguments>
	void registerFunction(const std::string& name, R (*fnptr)(val<FunctionArguments>...)) {
		std::function<R(val<FunctionArguments>...)> func = fnptr;
		interpretedFunctions_[name] = func;
#ifdef ENABLE_TRACING
		if (compiled_) {
			auto wrapper = details::createFunctionWrapper(fnptr);
			functions_.emplace_back(name, std::move(wrapper));
		}
#endif
	}

	/**
	 * @brief Compile all registered functions together into one compilation unit.
	 * When compilation is disabled, returns a module that interprets functions directly.
	 * @return CompiledModule with all functions accessible by name
	 */
	CompiledModule compile() {
#ifdef ENABLE_TRACING
		if (compiled_) {
			auto executable = jit_.compile(functions_);
			return CompiledModule(std::move(executable), std::move(interpretedFunctions_));
		}
#endif
		return CompiledModule(std::move(interpretedFunctions_));
	}

private:
	std::unordered_map<std::string, std::any> interpretedFunctions_;
#ifdef ENABLE_TRACING
	const compiler::JITCompiler& jit_;
	bool compiled_;
	std::list<compiler::CompilableFunction> functions_;
#endif
};

inline NautilusModule NautilusEngine::createModule() const {
	return NautilusModule(jit, isCompiled());
}

} // namespace nautilus::engine

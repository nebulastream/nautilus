
#pragma once

#include "nautilus/Executable.hpp"
#include "nautilus/JITCompiler.hpp"
#include "nautilus/Module.hpp"
#include "nautilus/TieredCompilation.hpp"
#include "nautilus/config.hpp"
#include "nautilus/core.hpp"
#include "nautilus/options.hpp"
#include <any>
#include <functional>
#include <memory>

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

/**
 * @brief Creates the appropriate IJITCompiler implementation based on options.
 *
 * If tiered compilation options are set (engine.tier0.backend and engine.tier1.backend),
 * returns a TieredJITCompiler. Otherwise returns a standard JITCompiler.
 */
inline std::unique_ptr<compiler::IJITCompiler> createJITCompiler(const Options& options) {
	auto tier0 = options.getOptionOrDefault<std::string>("engine.tier0.backend", "");
	auto tier1 = options.getOptionOrDefault<std::string>("engine.tier1.backend", "");
	if (!tier0.empty() && !tier1.empty()) {
		TieredCompilationConfig config;
		config.tier0.backend = tier0;
		config.tier1.backend = tier1;
		return std::make_unique<compiler::TieredJITCompiler>(options, config);
	}
	return std::make_unique<compiler::JITCompiler>(options);
}

#endif
} // namespace details

/**
 * @brief A compiled function handle returned by NautilusEngine::registerFunction().
 *
 * Wraps a CompiledModule with a single function named "execute". Callable via operator()
 * and provides access to the underlying executable for inspection (e.g. LLVM IR dumps).
 *
 * Internally uses the same module machinery as multi-function compilation, eliminating
 * the need for a separate single-function code path.
 *
 * @tparam Signature Raw function signature, e.g. int32_t(int32_t, int32_t)
 */
template <typename Signature>
class CompiledFunction;

template <typename R, typename... Args>
class CompiledFunction<R(Args...)> {
public:
	explicit CompiledFunction(CompiledModule module)
	    : module_(std::move(module)), fn_(module_.getFunction<R(Args...)>("execute")) {
	}

	CompiledFunction(const CompiledFunction&) = delete;
	CompiledFunction(CompiledFunction&&) noexcept = default;
	CompiledFunction& operator=(const CompiledFunction&) = delete;
	CompiledFunction& operator=(CompiledFunction&&) noexcept = default;

	R operator()(Args... args) const {
		return fn_(std::forward<Args>(args)...);
	}

	const compiler::Executable* getExecutable() const {
		return module_.getExecutable();
	}

private:
	CompiledModule module_;
	ModuleFunction<R(Args...)> fn_;
};

/**
 * The Nautilus Engine maintains the execution context of one or multiple nautilus functions,
 * which are registered using registerFunction.
 * Depending on the provided options, these functions may be compiled using a compilation backend or are executed
 * directly.
 */
class NautilusEngine {
public:
	NautilusEngine(const Options& options = Options());

	/**
	 * @brief Create an engine with a specific JIT compiler implementation.
	 * @param jit The JIT compiler to use (takes ownership)
	 * @param options Engine options
	 */
	NautilusEngine(std::unique_ptr<compiler::IJITCompiler> jit, const Options& options = Options());

	/// Register and compile a single function pointer. Defined after NautilusModule.
	template <typename R, is_val... FunctionArguments>
	auto registerFunction(R (*fnptr)(val<FunctionArguments>...)) const;

	/// Register and compile a single std::function. Defined after NautilusModule.
	template <typename R, typename... FunctionArguments>
	auto registerFunction(std::function<R(val<FunctionArguments>...)> func) const;

	/**
	 * @brief Creates a new module for registering multiple functions to be compiled together.
	 * @return NautilusModule builder
	 */
	NautilusModule createModule() const;

	std::string getNameOfBackend() const {
		return jit_->getName();
	}

	bool isCompiled() const {
		return options.getOptionOrDefault("engine.Compilation", true);
	}

private:
	std::unique_ptr<compiler::IJITCompiler> jit_;
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
	NautilusModule(const compiler::IJITCompiler& jit, bool compiled) : jit_(jit), compiled_(compiled) {
	}
#else
	NautilusModule(const compiler::IJITCompiler& /*jit*/, bool /*compiled*/) {
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
	const compiler::IJITCompiler& jit_;
	bool compiled_;
	std::list<compiler::CompilableFunction> functions_;
#endif
};

inline NautilusModule NautilusEngine::createModule() const {
	return NautilusModule(*jit_, isCompiled());
}

namespace details {

template <typename T, typename = void>
struct raw_return_type {
	using type = void;
};

template <typename T>
struct raw_return_type<T, std::void_t<typename T::raw_type>> {
	using type = typename T::raw_type;
};

template <typename T>
using raw_return_type_t = typename raw_return_type<T>::type;

} // namespace details

template <typename R, is_val... FunctionArguments>
auto NautilusEngine::registerFunction(R (*fnptr)(val<FunctionArguments>...)) const {
	using RawR = details::raw_return_type_t<R>;
	auto module = createModule();
	module.registerFunction("execute", fnptr);
	return CompiledFunction<RawR(FunctionArguments...)>(module.compile());
}

template <typename R, typename... FunctionArguments>
auto NautilusEngine::registerFunction(std::function<R(val<FunctionArguments>...)> func) const {
	using RawR = details::raw_return_type_t<R>;
	auto module = createModule();
	module.registerFunction("execute", std::move(func));
	return CompiledFunction<RawR(FunctionArguments...)>(module.compile());
}

} // namespace nautilus::engine

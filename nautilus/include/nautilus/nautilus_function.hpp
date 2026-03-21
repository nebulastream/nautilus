// nautilus_function.hpp
#pragma once

#include "nautilus/Engine.hpp"   // engine::details::createFunctionWrapper
#include "nautilus/function.hpp" // getArgumentReferences
#include "nautilus/val_func.hpp" // val<R(*)(Args...)>
#include <functional>            // std::invoke
#include <string>
#include <type_traits> // std::is_void_v, std::invoke_result_t
#include <utility>     // std::forward, std::move

namespace nautilus {

// Type trait: extracts the raw function pointer type from a callable F that
// operates on val<T> types.  E.g. for F with signature val<int>(val<int>, val<int>)
// this produces raw_func_ptr_t = int(*)(int, int).
namespace detail {

// Primary template — unspecialised
template <typename F, typename = void>
struct nautilus_function_traits;

// Specialisation for std::function<R(Args...)>
template <typename R, typename... Args>
struct nautilus_function_traits<std::function<R(Args...)>> {
	using raw_func_ptr_t = typename R::raw_type (*)(typename Args::raw_type...);
};

// Specialisation for std::function<void(Args...)>
template <typename... Args>
struct nautilus_function_traits<std::function<void(Args...)>> {
	using raw_func_ptr_t = void (*)(typename Args::raw_type...);
};

// Specialisation for raw function pointers R(*)(Args...)
template <typename R, typename... Args>
struct nautilus_function_traits<R (*)(Args...), void> {
	using raw_func_ptr_t = typename R::raw_type (*)(typename Args::raw_type...);
};

// Specialisation for void(*)(Args...)
template <typename... Args>
struct nautilus_function_traits<void (*)(Args...), void> {
	using raw_func_ptr_t = void (*)(typename Args::raw_type...);
};

// For general callables (lambdas, functors): deduce via std::function
template <typename F>
struct nautilus_function_traits<F, std::void_t<decltype(std::function(std::declval<F>()))>>
    : nautilus_function_traits<decltype(std::function(std::declval<F>()))> {};

} // namespace detail

template <typename F>
using nautilus_raw_func_ptr_t = typename detail::nautilus_function_traits<std::decay_t<F>>::raw_func_ptr_t;

class NautilusFunctionDefinition {
public:
	NautilusFunctionDefinition(std::string name) : name_(std::move(name)) {
	}

	const std::string& name() const noexcept {
		return name_;
	}

private:
	std::string name_;
};

// -----------------------------
// NautilusFunction
// Generic callable wrapper that intercepts calls.
// Works with:
//  - free functions
//  - lambdas / functors
//  - member functions bound to a specific instance (via helper below)
// -----------------------------
template <class F>
class NautilusFunction {
public:
	NautilusFunction(std::string name, F f)
	    : definition_(std::move(name)), f_(std::move(f))
#ifdef ENABLE_TRACING
	      ,
	      fwrapper(engine::details::createFunctionWrapper(f_))
#endif
	{
	}

	// NautilusFunction is non-copyable because fwrapper captures a copy of f_ at
	// construction time. Copying would create a second fwrapper still pointing at the
	// original f_'s copy, which is confusing. All uses are static, so this is fine.
	NautilusFunction(const NautilusFunction&) = delete;
	NautilusFunction& operator=(const NautilusFunction&) = delete;
	NautilusFunction(NautilusFunction&&) = delete;
	NautilusFunction& operator=(NautilusFunction&&) = delete;

public:
	template <class... Args>
	    requires std::invocable<F&, Args...>
	decltype(auto) operator()(Args&&... args) {
#ifdef ENABLE_TRACING
		if (tracing::inTracer()) {
			auto functionArgumentReferences = getArgumentReferences(std::forward<Args>(args)...);

			if constexpr (std::is_void_v<std::invoke_result_t<F&, Args...>>) {
				tracing::traceNautilusCall(&definition_, fwrapper, Type::v, functionArgumentReferences,
				                           FunctionAttributes {});
				return;
			} else {
				using R = std::invoke_result_t<F&, Args...>;
				auto resultRef = tracing::traceNautilusCall(&definition_, fwrapper,
				                                            tracing::TypeResolver<typename R::raw_type>::to_type(),
				                                            functionArgumentReferences, FunctionAttributes {});
				return R(resultRef);
			}
		}
#endif
		// Execute the actual function
		if constexpr (std::is_void_v<std::invoke_result_t<F&, Args...>>) {
			std::invoke(f_, std::forward<Args>(args)...);
		} else {
			decltype(auto) r = std::invoke(f_, std::forward<Args>(args)...);
			return r;
		}
	}

	const std::string& name() const noexcept {
		return definition_.name();
	}

	/// Returns a val wrapping the compiled function pointer for this NautilusFunction.
	/// During tracing: emits a FUNC_ADDR operation that registers the function for compilation.
	/// Outside tracing: JIT-compiles the function on demand and returns the compiled pointer.
	auto getFuncPtr() {
		using RawFuncPtr = nautilus_raw_func_ptr_t<F>;
#ifdef ENABLE_TRACING
		if (tracing::inTracer()) {
			auto& resultRef = tracing::traceNautilusFunctionPtr(&definition_, fwrapper);
			return val<RawFuncPtr>(resultRef);
		}
#endif
		// Non-tracing path: JIT-compile on demand and cache.
		if (!compiledFuncPtr_) {
			// Without tracing, create a trampoline that forwards to f_ directly.
			// NautilusFunction is non-copyable/non-movable, so the address is stable.
			TrampolineHelper<RawFuncPtr>::fn = &f_;
			compiledFuncPtr_ = reinterpret_cast<void*>(&TrampolineHelper<RawFuncPtr>::call);
		}
		return val<RawFuncPtr>(reinterpret_cast<RawFuncPtr>(compiledFuncPtr_));
	}

private:
	// Trampoline for non-tracing mode: a static function pointer that forwards
	// raw arguments to f_ via val<T> implicit conversions.
	template <typename RawFP>
	struct TrampolineHelper;

	template <typename Ret, typename... RawArgs>
	struct TrampolineHelper<Ret (*)(RawArgs...)> {
		static inline F* fn = nullptr;
		static Ret call(RawArgs... args) {
			if constexpr (std::is_void_v<Ret>) {
				std::invoke(*fn, make_value(args)...);
			} else {
				return details::RawValueResolver<Ret>::getRawValue(std::invoke(*fn, make_value(args)...));
			}
		}
	};

	NautilusFunctionDefinition definition_;
	F f_;
#ifdef ENABLE_TRACING
	std::function<void()> fwrapper;
#endif
	void* compiledFuncPtr_ = nullptr;
};

// CTAD helper (so you can write NautilusFunction{"name", callable};)
template <class F>
NautilusFunction(std::string, F) -> NautilusFunction<std::decay_t<F>>;

// Bind a member function to a specific instance.
// Returns a std::function with the concrete signature deduced from the method pointer.
// Usage: bind_instance<&Calculator::mul>(calc)
namespace detail {

template <typename R, typename C, typename... Args, typename F>
std::function<R(Args...)> make_bound_function(R (C::*)(Args...), F&& f) {
	return std::forward<F>(f);
}

template <typename R, typename C, typename... Args, typename F>
std::function<R(Args...)> make_bound_function(R (C::*)(Args...) const, F&& f) {
	return std::forward<F>(f);
}

} // namespace detail

template <auto Method, class Obj>
auto bind_instance(Obj& obj) {
	return detail::make_bound_function(Method, [&obj](auto&&... args) -> decltype(auto) {
		return (obj.*Method)(std::forward<decltype(args)>(args)...);
	});
}

} // namespace nautilus

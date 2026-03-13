// nautilus_function.hpp
#pragma once

#include "nautilus/Engine.hpp"   // engine::details::createFunctionWrapper
#include "nautilus/function.hpp" // getArgumentReferences
#include <functional>            // std::invoke
#include <string>
#include <type_traits> // std::is_void_v, std::invoke_result_t
#include <utility>     // std::forward, std::move

namespace nautilus {

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

private:
	NautilusFunctionDefinition definition_;
	F f_;
#ifdef ENABLE_TRACING
	std::function<void()> fwrapper;
#endif
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

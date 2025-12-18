// nautilus_function.hpp
#pragma once

#include <functional>   // std::invoke
#include <string>
#include <type_traits>  // std::is_void_v, std::invoke_result_t
#include <utility>      // std::forward, std::move
#include "nautilus/Engine.hpp"    // engine::details::createFunctionWrapper
#include "nautilus/function.hpp"  // getArgumentReferences

namespace nautilus {

    class NautilusFunctionDefinition {
    public:
        NautilusFunctionDefinition(std::string name)
            : name_(std::move(name)) {}
        private:
            std::string name_;
    }; // forward declaration   

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
        , fwrapper(engine::details::createFunctionWrapper(f_))
#endif
    {}

public:

    template <class... Args>
    requires std::invocable<F&, Args...>
    decltype(auto) operator()(Args&&... args) {
#ifdef ENABLE_TRACING
        if (tracing::inTracer()) {
            auto functionArgumentReferences = getArgumentReferences(std::forward<Args>(args)...);

            if constexpr (std::is_void_v<std::invoke_result_t<F&, Args...>>) {
                tracing::traceNautilusCall(definition_, fwrapper, Type::v, functionArgumentReferences, FunctionAttributes{});
                return;
            } else {
                using R = std::invoke_result_t<F&, Args...>;
                auto resultRef = tracing::traceNautilusCall(
                    definition_,
                    fwrapper,
                    tracing::TypeResolver<typename R::raw_type>::to_type(),
                    functionArgumentReferences,
                    FunctionAttributes{}
                );
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

    const std::string& name() const noexcept { return name_; }

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

// -----------------------------
// Helper: bind a member function to a specific instance using std::bind.
// Method is a pointer to member, e.g. &C::mul
// Returns a std::function with concrete signature.
// Usage: bind_instance<&Calculator::mul>(calc)
// -----------------------------
namespace detail {
    // Use std::bind with explicit placeholders for different arities
    template <typename R, typename C>
    std::function<R()> bind_member(R (C::*method)(), C& obj) {
        return std::bind(method, &obj);
    }

    template <typename R, typename C, typename A1>
    std::function<R(A1)> bind_member(R (C::*method)(A1), C& obj) {
        return std::bind(method, &obj, std::placeholders::_1);
    }

    template <typename R, typename C, typename A1, typename A2>
    std::function<R(A1, A2)> bind_member(R (C::*method)(A1, A2), C& obj) {
        return std::bind(method, &obj, std::placeholders::_1, std::placeholders::_2);
    }

    template <typename R, typename C, typename A1, typename A2, typename A3>
    std::function<R(A1, A2, A3)> bind_member(R (C::*method)(A1, A2, A3), C& obj) {
        return std::bind(method, &obj, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
    }

    template <typename R, typename C, typename A1, typename A2, typename A3, typename A4>
    std::function<R(A1, A2, A3, A4)> bind_member(R (C::*method)(A1, A2, A3, A4), C& obj) {
        return std::bind(method, &obj, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
    }

    // Add overloads for const member functions
    template <typename R, typename C>
    std::function<R()> bind_member(R (C::*method)() const, C& obj) {
        return std::bind(method, &obj);
    }

    template <typename R, typename C, typename A1>
    std::function<R(A1)> bind_member(R (C::*method)(A1) const, C& obj) {
        return std::bind(method, &obj, std::placeholders::_1);
    }

    template <typename R, typename C, typename A1, typename A2>
    std::function<R(A1, A2)> bind_member(R (C::*method)(A1, A2) const, C& obj) {
        return std::bind(method, &obj, std::placeholders::_1, std::placeholders::_2);
    }

    template <typename R, typename C, typename A1, typename A2, typename A3>
    std::function<R(A1, A2, A3)> bind_member(R (C::*method)(A1, A2, A3) const, C& obj) {
        return std::bind(method, &obj, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
    }

    template <typename R, typename C, typename A1, typename A2, typename A3, typename A4>
    std::function<R(A1, A2, A3, A4)> bind_member(R (C::*method)(A1, A2, A3, A4) const, C& obj) {
        return std::bind(method, &obj, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
    }
}

template <auto Method, class Obj>
auto bind_instance(Obj& obj) {
    return detail::bind_member(Method, obj);
}

} // namespace nautilus
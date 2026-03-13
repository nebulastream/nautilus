#pragma once

#include "nautilus/common/FunctionAttributes.hpp"
#include "nautilus/val.hpp"
#include "nautilus/val_ptr.hpp"
#include <functional>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>

namespace nautilus {

namespace detail {

/// Extract a human-readable function name for a compile-time function pointer FnPtr.
///
/// Uses __PRETTY_FUNCTION__ (GCC/Clang extension) to obtain the demangled name at
/// compile time without depending on dladdr or the dynamic symbol table.  This means
/// the name is available even for static, inline, and local functions that are
/// invisible to dladdr.
///
/// Examples of what GCC/Clang produce in the template instantiation:
///   GCC:   "... [with auto FnPtr = &add(int, int)]"
///   Clang: "... [FnPtr = &add]"
///
/// The returned string is initialized once (function-static) per distinct FnPtr value.
template <auto FnPtr>
const std::string& functionNameHint() {
	static const std::string name = [] {
		std::string_view sv(__PRETTY_FUNCTION__);
		// Both GCC and Clang embed "FnPtr = " in the expansion.
		constexpr std::string_view needle = "FnPtr = ";
		const auto pos0 = sv.find(needle);
		if (pos0 == std::string_view::npos) {
			return std::string(sv);
		}
		auto pos = pos0 + needle.size();
		// Skip leading address-of operator '&'.
		if (pos < sv.size() && sv[pos] == '&') {
			++pos;
		}
		// GCC may emit a cast expression "(return_type (*)(args...))funcname".
		// Skip over it so we are left with just the function name.
		if (pos < sv.size() && sv[pos] == '(') {
			size_t depth = 1;
			size_t scan = pos + 1;
			while (scan < sv.size() && depth > 0) {
				if (sv[scan] == '(') {
					++depth;
				} else if (sv[scan] == ')') {
					--depth;
				}
				++scan;
			}
			pos = scan;
		}
		// The template parameter list ends at ']'; GCC also appends the argument
		// types after the function name before ']', which we intentionally keep
		// because "add(int, int)" is more informative than just "add".
		const auto end = sv.find(']', pos);
		return std::string(sv.substr(pos, end == std::string_view::npos ? sv.size() - pos : end - pos));
	}();
	return name;
}

/// Helper that constructs a CallableRuntimeFunction from a compile-time function pointer
/// and forwards the deduced argument/return types automatically.
template <typename R, typename... FunctionArguments>
auto makeCallableWithHint(R (*fnptr)(FunctionArguments...), std::string_view hint) {
	return CallableRuntimeFunction<R, FunctionArguments...>(fnptr, hint);
}

} // namespace detail

template <typename... ValueArguments>
auto getArgumentReferences(const ValueArguments&... arguments) {
	return std::vector<tracing::TypedValueRef> {details::StateResolver<const ValueArguments&>::getState(arguments)...};
}

template <typename R, typename... FunctionArguments>
class CallableRuntimeFunction {
public:
	explicit CallableRuntimeFunction(R (*fnptr)(FunctionArguments...)) : fnptr(fnptr) {
	}

	explicit CallableRuntimeFunction(R (*fnptr)(FunctionArguments...), const FunctionAttributes fnAttrs)
	    : fnAttrs(fnAttrs), fnptr(fnptr) {
	}

	/// Constructor used by invoke<FnPtr>: carries a compile-time demangled name hint so that
	/// traceCall can use it instead of (or as a fallback for) the dladdr symbol lookup.
	explicit CallableRuntimeFunction(R (*fnptr)(FunctionArguments...), std::string_view nameHint)
	    : fnptr(fnptr), nameHint_(nameHint) {
	}

	template <typename... FunctionArgumentsRaw>
	    requires(!std::is_void_v<R>)
	auto operator()(FunctionArgumentsRaw&&... args) {
#ifdef ENABLE_TRACING
		if (tracing::inTracer()) {
			auto functionArgumentReferences = getArgumentReferences(std::forward<FunctionArgumentsRaw>(args)...);
			auto resultRef = tracing::traceCall(reinterpret_cast<void*>(fnptr), tracing::TypeResolver<R>::to_type(),
			                                    functionArgumentReferences, fnAttrs, nameHint_);
			return val<R>(resultRef);
		}
#endif
		return val<R>(fnptr(
		    details::RawValueResolver<FunctionArguments>::getRawValue(std::forward<FunctionArgumentsRaw>(args))...));
	}

	template <typename... FunctionArgumentsRaw>
	    requires std::is_void_v<R>
	void operator()(FunctionArgumentsRaw&&... args) {
#ifdef ENABLE_TRACING
		if (tracing::inTracer()) {
			auto functionArgumentReferences = getArgumentReferences(std::forward<FunctionArgumentsRaw>(args)...);
			tracing::traceCall(reinterpret_cast<void*>(fnptr), Type::v, functionArgumentReferences, fnAttrs, nameHint_);
			return;
		}
#endif
		(fnptr(details::RawValueResolver<FunctionArguments>::getRawValue(std::forward<FunctionArgumentsRaw>(args))...));
	}

	template <is_integral... FunctionArgumentsRaw>
	auto invoke(FunctionArgumentsRaw&&... args) {
		return (*this)(std::forward<FunctionArgumentsRaw>(args)...);
	}

private:
	FunctionAttributes fnAttrs;
	R (*fnptr)(FunctionArguments...);
	std::string_view nameHint_;
};

/// Invoke calls without attributes
template <typename R, typename... FunctionArguments, typename... ValueArguments>
auto invoke(R (*fnptr)(FunctionArguments...), ValueArguments&&... args) {
	return CallableRuntimeFunction<R, FunctionArguments...>(fnptr)(std::forward<ValueArguments>(args)...);
}

template <typename R, typename... FunctionArguments, typename... ValueArguments>
auto invoke(std::function<R(FunctionArguments...)> func, ValueArguments&&... args) {
	auto fnptr = func.template target<R(FunctionArguments...)>();
	return CallableRuntimeFunction<R, FunctionArguments...>(fnptr)(std::forward<ValueArguments>(args)...);
}

template <is_fundamental... FunctionArguments, typename... ValueArguments>
void invoke(void (*fnptr)(FunctionArguments...), ValueArguments&&... args) {
	auto func = CallableRuntimeFunction<void, FunctionArguments...>(fnptr);
	func(std::forward<ValueArguments>(args)...);
}

/// Invoke calls with attributes
template <typename R, typename... FunctionArguments, typename... ValueArguments>
auto invoke(const FunctionAttributes fnAttrs, R (*fnptr)(FunctionArguments...), ValueArguments&&... args) {
	return CallableRuntimeFunction<R, FunctionArguments...>(fnptr, fnAttrs)(std::forward<ValueArguments>(args)...);
}

template <typename R, typename... FunctionArguments, typename... ValueArguments>
auto invoke(const FunctionAttributes fnAttrs, std::function<R(FunctionArguments...)> func, ValueArguments&&... args) {
	auto fnptr = func.template target<R(FunctionArguments...)>();
	return CallableRuntimeFunction<R, FunctionArguments...>(fnptr, fnAttrs)(std::forward<ValueArguments>(args)...);
}

template <is_fundamental... FunctionArguments, typename... ValueArguments>
void invoke(const FunctionAttributes fnAttrs, void (*fnptr)(FunctionArguments...), ValueArguments&&... args) {
	auto func = CallableRuntimeFunction<void, FunctionArguments...>(fnptr, fnAttrs);
	func(std::forward<ValueArguments>(args)...);
}

template <typename R, typename... FunctionArguments>
auto function(R (*fnptr)(FunctionArguments...)) {
	return CallableRuntimeFunction<R, FunctionArguments...>(fnptr);
}

/// invoke<FnPtr>(args...) — non-type template parameter variant.
///
/// By taking the function pointer as a compile-time non-type template argument the
/// implementation can extract a reliable demangled name from __PRETTY_FUNCTION__ at
/// the point of template instantiation.  This name is then passed as a hint to the
/// tracer so that the trace correctly identifies static, inline, or lambda-wrapper
/// functions that would otherwise be invisible to the dladdr symbol lookup.
///
/// Usage:
///   val<int32_t> result = invoke<&add>(x, y);
template <auto FnPtr, typename... ValueArguments>
decltype(auto) invoke(ValueArguments&&... args) {
	return detail::makeCallableWithHint(FnPtr,
	                                    detail::functionNameHint<FnPtr>())(std::forward<ValueArguments>(args)...);
}

class MemberFuncWrapper {};

template <typename T, typename Rp, typename Tp>
class MemberFuncWrapperImpl : public MemberFuncWrapper {
public:
	MemberFuncWrapperImpl(T func)
	    : func(func), callableRuntimeFunction(function(+[](MemberFuncWrapper* ptr, Tp* clazzPtr) -> auto {
		      auto p = static_cast<MemberFuncWrapperImpl<T, Rp, Tp>*>(ptr);
		      Rp (Tp::*func)() = p->func;
		      return (*clazzPtr.*func)();
	      })) {
	}

	template <typename... FunctionArgumentsRaw>
	auto operator()(FunctionArgumentsRaw... args) {
		auto state = val<MemberFuncWrapper*>(this);
		return callableRuntimeFunction(state, args...);
	}
	T func;
	CallableRuntimeFunction<Rp, MemberFuncWrapper*, Tp*> callableRuntimeFunction;
};

template <typename T>
struct member_function_traits;

template <typename C, typename R, typename... Args>
struct member_function_traits<R (C::*)(Args...)> {
	using class_type = C;
	using return_type = R;
	using arg_types = std::tuple<Args...>;
};

template <typename T>
auto& memberFunc(T func) {
	using traits = member_function_traits<T>;
	using ClassType = typename traits::class_type;
	using ReturnType = typename traits::return_type;
	// using ArgTypes = typename traits::arg_types;
	auto ptr = new MemberFuncWrapperImpl<T, ReturnType, ClassType>(func);
	return *ptr;
}

} // namespace nautilus

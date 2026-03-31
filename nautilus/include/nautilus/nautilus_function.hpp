// nautilus_function.hpp
#pragma once

#include "nautilus/Engine.hpp"   // engine::details::createFunctionWrapper
#include "nautilus/function.hpp" // getArgumentReferences
#include "nautilus/val_func.hpp" // val<R(*)(Args...)>
#include <any>
#include <array>
#include <cstring>    // std::memcpy
#include <functional> // std::invoke
#include <mutex>
#include <string>
#include <type_traits> // std::is_void_v, std::invoke_result_t
#include <unordered_map>
#include <utility> // std::forward, std::move
#include <vector>

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

// =====================================================================
// SpecializableNautilusFunction — constant argument specialization
// =====================================================================

/// Key identifying a specialization by (argIndex, bitcast value) pairs.
struct SpecializationKey {
	std::vector<std::pair<size_t, uint64_t>> entries;

	bool operator==(const SpecializationKey& other) const {
		return entries == other.entries;
	}

	struct Hash {
		size_t operator()(const SpecializationKey& key) const {
			size_t h = 0;
			for (auto& [idx, val] : key.entries) {
				h ^= std::hash<size_t> {}(idx) + 0x9e3779b9 + (h << 6) + (h >> 2);
				h ^= std::hash<uint64_t> {}(val) + 0x9e3779b9 + (h << 6) + (h >> 2);
			}
			return h;
		}
	};
};

namespace detail {

/// Check at compile time whether index I is in the SpecArgs pack.
template <size_t I, size_t... SpecArgs>
constexpr bool IsSpecArg() {
	return ((I == SpecArgs) || ...);
}

/// Count how many indices in SpecArgs are less than I (used for renumbering symbolic args).
template <size_t I, size_t... SpecArgs>
constexpr size_t SymbolicIndex() {
	size_t count = 0;
	size_t specArr[] = {SpecArgs...};
	for (size_t s : specArr) {
		if (s < I) {
			++count;
		}
	}
	return I - count;
}

/// Bit-cast a raw value to uint64_t for use as a specialization key.
template <typename T>
uint64_t ToBits(T value) {
	uint64_t bits = 0;
	if constexpr (sizeof(T) <= sizeof(uint64_t)) {
		std::memcpy(&bits, &value, sizeof(T));
	}
	return bits;
}

/// Extract the raw value from a val<T> argument.
template <typename Arg>
auto ExtractRawValue(const Arg& arg) {
	return details::RawValueResolver<typename std::remove_cvref_t<Arg>::raw_type>::getRawValue(arg);
}

} // namespace detail

/// A variant of NautilusFunction that auto-specializes when arguments at the
/// positions given by SpecArgs are observed to be constant across calls.
///
/// Template parameters:
///   F          — the callable type (function pointer, lambda, etc.)
///   SpecArgs   — 0-based indices of arguments to speculate on
///
/// Usage:
///   static SpecializableNautilusFunction<decltype(myFunc), 0, 2> sf{"myFunc", myFunc};
///   sf(val<int>(42), val<float>(3.14f), val<int>(99));
///   // After STABILITY_THRESHOLD consecutive calls with the same value for args 0 and 2,
///   // a specialized variant is created where those args are baked in as constants.
template <class F, size_t... SpecArgs>
class SpecializableNautilusFunction {
	static_assert(sizeof...(SpecArgs) > 0, "At least one argument index must be marked as speculatable");

public:
	SpecializableNautilusFunction(std::string name, F f)
	    : definition_(std::move(name)), f_(std::move(f))
#ifdef ENABLE_TRACING
	      ,
	      generic_fwrapper_(engine::details::createFunctionWrapper(f_))
#endif
	{
	}

	SpecializableNautilusFunction(const SpecializableNautilusFunction&) = delete;
	SpecializableNautilusFunction& operator=(const SpecializableNautilusFunction&) = delete;
	SpecializableNautilusFunction(SpecializableNautilusFunction&&) = delete;
	SpecializableNautilusFunction& operator=(SpecializableNautilusFunction&&) = delete;

	template <class... Args>
	    requires std::invocable<F&, Args...>
	decltype(auto) operator()(Args&&... args) {
		using R = std::invoke_result_t<F&, Args...>;
#ifdef ENABLE_TRACING
		if (tracing::inTracer()) {
			return callInTracer<R>(std::forward<Args>(args)...);
		}
#endif
		return callOutsideTracer<R>(std::forward<Args>(args)...);
	}

	const std::string& name() const noexcept {
		return definition_.name();
	}

private:
	static constexpr uint32_t STABILITY_THRESHOLD = 3;

	NautilusFunctionDefinition definition_;
	F f_;
#ifdef ENABLE_TRACING
	std::function<void()> generic_fwrapper_;
#endif

	// --- Stability tracking ---
	struct ArgTracker {
		uint64_t last_value = 0;
		uint32_t stable_count = 0;
	};
	std::array<ArgTracker, sizeof...(SpecArgs)> trackers_ = {};

	// --- Specialization cache ---
	struct SpecializedVariant {
		NautilusFunctionDefinition definition;
#ifdef ENABLE_TRACING
		std::function<void()> fwrapper;
#endif
	};
	std::mutex cache_mutex_;
	std::unordered_map<SpecializationKey, SpecializedVariant, SpecializationKey::Hash> cache_;

	/// Build a SpecializationKey from the current argument values.
	template <class... Args>
	SpecializationKey buildKey(const std::tuple<Args...>& argsTuple) const {
		SpecializationKey key;
		key.entries.reserve(sizeof...(SpecArgs));
		buildKeyImpl(argsTuple, key, std::index_sequence_for<Args...> {});
		return key;
	}

	template <class Tuple, size_t... Is>
	void buildKeyImpl(const Tuple& t, SpecializationKey& key, std::index_sequence<Is...>) const {
		(buildKeyEntry<Is>(t, key), ...);
	}

	template <size_t I, class Tuple>
	void buildKeyEntry(const Tuple& t, SpecializationKey& key) const {
		if constexpr (detail::IsSpecArg<I, SpecArgs...>()) {
			auto raw = detail::ExtractRawValue(std::get<I>(t));
			key.entries.emplace_back(I, detail::ToBits(raw));
		}
	}

	/// Generate a unique name for a specialized variant.
	std::string makeSpecializedName(const SpecializationKey& key) const {
		std::string result = definition_.name();
		for (auto& [idx, bits] : key.entries) {
			result += "$" + std::to_string(idx) + "=" + std::to_string(bits);
		}
		return result;
	}

	/// Update trackers and check if all speculatable args are stable.
	template <class... Args>
	bool updateAndCheckStability(const std::tuple<Args...>& argsTuple) {
		bool allStable = true;
		updateStabilityImpl(argsTuple, allStable, std::index_sequence_for<Args...> {});
		return allStable;
	}

	template <class Tuple, size_t... Is>
	void updateStabilityImpl(const Tuple& t, bool& allStable, std::index_sequence<Is...>) {
		(updateStabilityEntry<Is>(t, allStable), ...);
	}

	template <size_t I, class Tuple>
	void updateStabilityEntry(const Tuple& t, bool& allStable) {
		if constexpr (detail::IsSpecArg<I, SpecArgs...>()) {
			// Find which tracker index corresponds to this arg
			constexpr size_t trackerIdx = trackerIndexFor<I>();
			auto raw = detail::ExtractRawValue(std::get<I>(t));
			uint64_t bits = detail::ToBits(raw);
			auto& tracker = trackers_[trackerIdx];
			if (bits == tracker.last_value) {
				if (tracker.stable_count < STABILITY_THRESHOLD) {
					tracker.stable_count++;
				}
			} else {
				tracker.last_value = bits;
				tracker.stable_count = 1;
			}
			if (tracker.stable_count < STABILITY_THRESHOLD) {
				allStable = false;
			}
		}
	}

	/// Compute the tracker array index for a given argument index.
	template <size_t TargetIdx>
	static constexpr size_t trackerIndexFor() {
		constexpr size_t specArr[] = {SpecArgs...};
		for (size_t i = 0; i < sizeof...(SpecArgs); ++i) {
			if (specArr[i] == TargetIdx) {
				return i;
			}
		}
		return 0; // unreachable if TargetIdx is in SpecArgs
	}

#ifdef ENABLE_TRACING
	/// Inside-tracer call path: behave like NautilusFunction (generic, all args symbolic).
	/// Specialization only applies to the outside-tracer (runtime) path.
	template <typename R, class... Args>
	decltype(auto) callInTracer(Args&&... args) {
		auto functionArgumentReferences = getArgumentReferences(std::forward<Args>(args)...);

		if constexpr (std::is_void_v<R>) {
			tracing::traceNautilusCall(&definition_, generic_fwrapper_, Type::v, functionArgumentReferences,
			                           FunctionAttributes {});
			return;
		} else {
			auto resultRef = tracing::traceNautilusCall(&definition_, generic_fwrapper_,
			                                            tracing::TypeResolver<typename R::raw_type>::to_type(),
			                                            functionArgumentReferences, FunctionAttributes {});
			return R(resultRef);
		}
	}

	/// Create a specialized tracing wrapper where speculatable args are constants.
	template <class... Args>
	std::function<void()> createSpecializedWrapper(const std::tuple<Args...>& argsTuple) {
		std::unordered_map<size_t, std::any> constants;
		captureConstants(argsTuple, constants, std::index_sequence_for<Args...> {});
		return engine::details::createSpecializedFunctionWrapper<SpecArgs...>(f_, constants);
	}

	template <class Tuple, size_t... Is>
	void captureConstants(const Tuple& t, std::unordered_map<size_t, std::any>& constants,
	                      std::index_sequence<Is...>) const {
		(captureConstantEntry<Is>(t, constants), ...);
	}

	template <size_t I, class Tuple>
	void captureConstantEntry(const Tuple& t, std::unordered_map<size_t, std::any>& constants) const {
		if constexpr (detail::IsSpecArg<I, SpecArgs...>()) {
			auto raw = detail::ExtractRawValue(std::get<I>(t));
			constants[I] = raw;
		}
	}
#endif // ENABLE_TRACING

	/// Outside-tracer call path: track stability and dispatch.
	template <typename R, class... Args>
	decltype(auto) callOutsideTracer(Args&&... args) {
		auto argsTuple = std::forward_as_tuple(args...);
		bool stable = updateAndCheckStability(argsTuple);
		(void) stable;
		// Always execute the original function with all arguments.
		// Specialization only benefits the tracing/compilation path.
		if constexpr (std::is_void_v<R>) {
			std::invoke(f_, std::forward<Args>(args)...);
		} else {
			return std::invoke(f_, std::forward<Args>(args)...);
		}
	}
};

// CTAD for SpecializableNautilusFunction is not possible for non-type template
// parameters, so users must specify SpecArgs explicitly:
//   SpecializableNautilusFunction<decltype(fn), 0, 2> sf{"name", fn};

} // namespace nautilus

#pragma once

#include "nautilus/nautilus_function.hpp"
#include "nautilus/specialization/specialize.hpp"
#include "nautilus/val.hpp"
#include <functional>
#include <memory>
#include <string>
#include <tuple>
#include <utility>

namespace nautilus {

namespace detail {

/// True if `I` appears in the pack `Idxs...`.
template <std::size_t I, std::size_t... Idxs>
constexpr bool isSpecializedIndex() {
	return ((I == Idxs) || ...);
}

/// N-th element of a `std::size_t` pack (C++20-friendly replacement for
/// C++26 pack indexing).
template <std::size_t N, std::size_t First, std::size_t... Rest>
constexpr std::size_t nthInPack() {
	if constexpr (N == 0) {
		return First;
	} else {
		return nthInPack<N - 1, Rest...>();
	}
}

/// Index of `I` within the pack `Idxs...` (assuming it is present). Used
/// to pick the matching constant out of the per-index constants tuple.
template <std::size_t I, std::size_t First, std::size_t... Rest>
constexpr std::size_t indexOfInPack() {
	if constexpr (I == First) {
		return 0;
	} else {
		return 1 + indexOfInPack<I, Rest...>();
	}
}

/// Pick one argument when calling into the original user function from
/// the reduced specialized variant. `nsTup` holds only the
/// non-specialized arguments (in `NonSpecIdxs...` order), while
/// `consts` holds the specialized constants (in `Idxs...` order).
///
/// For a position `I` that is specialized, returns a fresh
/// `val<T>(const)` — which traces as a local constant op, not as a
/// function parameter. For a non-specialized position, forwards the
/// corresponding element of `nsTup`.
template <std::size_t I, class NsTup, class ConstTup, std::size_t... Idxs, std::size_t... NonSpecIdxs>
decltype(auto) pickReducedArg(NsTup& nsTup, const ConstTup& consts, std::index_sequence<Idxs...>,
                              std::index_sequence<NonSpecIdxs...>) {
	if constexpr (isSpecializedIndex<I, Idxs...>()) {
		constexpr std::size_t pos = indexOfInPack<I, Idxs...>();
		using T = std::tuple_element_t<pos, ConstTup>;
		return val<T>(std::get<pos>(consts));
	} else {
		constexpr std::size_t pos = indexOfInPack<I, NonSpecIdxs...>();
		return std::get<pos>(nsTup);
	}
}

template <class F, class NsTup, class ConstTup, std::size_t... Is, std::size_t... Idxs, std::size_t... NonSpecIdxs>
decltype(auto) callWithReducedArgs(F& f, NsTup& nsTup, const ConstTup& consts, std::index_sequence<Is...>,
                                   std::index_sequence<Idxs...> specIdxs,
                                   std::index_sequence<NonSpecIdxs...> nonSpecIdxs) {
	return f(pickReducedArg<Is>(nsTup, consts, specIdxs, nonSpecIdxs)...);
}

/// Append `I` to an existing `std::index_sequence`.
template <std::size_t I, class Seq>
struct appendIndex;
template <std::size_t I, std::size_t... As>
struct appendIndex<I, std::index_sequence<As...>> {
	using type = std::index_sequence<As..., I>;
};

/// Compute the index-subsequence of `[0..N)` that is **not** in the
/// specialized pack `Idxs...`. Drives the reduced argument list of the
/// specialized variant so that specialized positions disappear from
/// the function signature entirely.
template <std::size_t I, std::size_t N, class Acc, std::size_t... Idxs>
struct filterNonSpec {
	static constexpr bool is_spec = ((I == Idxs) || ...);
	using acc_next = std::conditional_t<is_spec, Acc, typename appendIndex<I, Acc>::type>;
	using type = typename filterNonSpec<I + 1, N, acc_next, Idxs...>::type;
};
template <std::size_t N, class Acc, std::size_t... Idxs>
struct filterNonSpec<N, N, Acc, Idxs...> {
	using type = Acc;
};
template <std::size_t N, std::size_t... Idxs>
using nonSpecIndicesT = typename filterNonSpec<0, N, std::index_sequence<>, Idxs...>::type;

/// Slice a tuple by index sequence: `tupleSlice<tuple<A, B, C>, seq<0, 2>>::type == tuple<A, C>`.
template <class Tuple, class Seq>
struct tupleSlice;
template <class Tuple, std::size_t... Is>
struct tupleSlice<Tuple, std::index_sequence<Is...>> {
	using type = std::tuple<std::tuple_element_t<Is, Tuple>...>;
};

/// Turn a tuple of argument types into a `std::function<R(Ts...)>`.
template <class R, class Tuple>
struct funcFromTuple;
template <class R, class... Ts>
struct funcFromTuple<R, std::tuple<Ts...>> {
	using type = std::function<R(Ts...)>;
};

} // namespace detail

/// Argument-value-specialized wrapper around `NautilusFunction`.
///
/// `SpecializedNautilusFunction<Signature, Idxs...>` holds two internal
/// `NautilusFunction`s:
///
/// - `generic_` — wraps the user's callable unchanged. Always exists.
/// - `specialized_` — wraps a lambda that forwards to the user's
///   callable with every argument at a position in `Idxs...` replaced
///   by a freshly-traced `val<T>` constant. Built lazily the first time
///   **all** of the per-index profiles have become stable.
///
/// When called from inside a traced scope, `operator()` emits a
/// dispatcher into the caller's trace:
///
/// - **Any profile still unstable.** Emits `invoke(&ValueProfile<T>::profile,
///   ...)` proxy calls for the still-unstable indexes and forwards to
///   `generic_`.
/// - **All profiles stable.** Emits a conjunction of equality tests
///   `arg_i0 == c0 && arg_i1 == c1 && ...` and a traced `if/else` that
///   calls `specialized_` on the then-branch and `generic_` on the
///   else-branch.
///
/// Typical usage:
///
/// ```cpp
/// static nautilus::ValueProfile<int64_t> g_base_profile;
/// static nautilus::ValueProfile<int64_t> g_exp_profile;
///
/// SpecializedNautilusFunction<val<int64_t>(val<int64_t>, val<int64_t>), 0, 1>
///     pow_spec{"pow", pow_kernel, &g_base_profile, &g_exp_profile};
/// ```
template <class Signature, std::size_t... Idxs>
class SpecializedNautilusFunction;

template <class R, class... Args, std::size_t... Idxs>
class SpecializedNautilusFunction<R(Args...), Idxs...> {
	static_assert(sizeof...(Idxs) >= 1, "SpecializedNautilusFunction: at least one specialized index required");
	static_assert(((Idxs < sizeof...(Args)) && ...), "SpecializedNautilusFunction: Idx out of range");

	using UserFn = std::function<R(Args...)>;
	using ArgsTuple = std::tuple<Args...>;

	// Raw (unwrapped) type at specialized position `I`.
	template <std::size_t I>
	using RawAt = typename std::tuple_element_t<I, ArgsTuple>::raw_type;

	using ConstTuple = std::tuple<RawAt<Idxs>...>;
	using ProfileTuple = std::tuple<ValueProfile<RawAt<Idxs>>...>;

	// Index sequence of the non-specialized argument positions, the
	// corresponding argument-types tuple, and the reduced
	// `std::function` type used for the specialized variant. The
	// specialized `NautilusFunction` is built with this reduced
	// signature so that specialized arguments disappear from the IR
	// altogether instead of lingering as dead parameters.
	using NonSpecSeq = detail::nonSpecIndicesT<sizeof...(Args), Idxs...>;
	using NonSpecArgsTuple = typename detail::tupleSlice<ArgsTuple, NonSpecSeq>::type;
	using SpecFnType = typename detail::funcFromTuple<R, NonSpecArgsTuple>::type;

public:
	SpecializedNautilusFunction(std::string name, UserFn f)
	    : userFn_(std::move(f)), generic_(std::make_unique<NautilusFunction<UserFn>>(name + "__generic", userFn_)),
	      baseName_(name) {
		// Build the dispatcher's user-fn: a lambda that closes over
		// `this` (stable — the class is non-copyable) and contains the
		// profile-update / generic-vs-specialized branching logic. The
		// lambda body runs at the moment the dispatcher itself gets
		// traced; from the parent trace's point of view all of this is
		// hidden behind a single `traceNautilusCall` to the dispatcher
		// symbol.
		UserFn dispatchBody = [this](Args... args) -> R {
			return this->traceDispatchBody(args...);
		};
		dispatcher_ = std::make_unique<NautilusFunction<UserFn>>(std::move(name), std::move(dispatchBody));
	}

	SpecializedNautilusFunction(const SpecializedNautilusFunction&) = delete;
	SpecializedNautilusFunction& operator=(const SpecializedNautilusFunction&) = delete;
	SpecializedNautilusFunction(SpecializedNautilusFunction&&) = delete;
	SpecializedNautilusFunction& operator=(SpecializedNautilusFunction&&) = delete;

	const std::string& name() const noexcept {
		return baseName_;
	}

	/// Access the internally-owned `ValueProfile` for the `P`-th entry
	/// in the specialized-index pack `Idxs...`. Useful for tests that
	/// want to prime the profile directly without driving it via calls.
	template <std::size_t P>
	auto& profile() noexcept {
		static_assert(P < sizeof...(Idxs), "profile<P>: P out of range");
		return std::get<P>(profiles_);
	}

	/// External call entry point. Forwards to the dispatcher
	/// `NautilusFunction`, which from inside a tracer emits exactly one
	/// `traceNautilusCall` to the dispatcher symbol — the same shape a
	/// caller sees for any other `NautilusFunction`. The dispatch logic
	/// (profile update / specialized vs generic) lives inside the
	/// dispatcher's own traced body.
	R operator()(Args... args) {
		return (*dispatcher_)(args...);
	}

private:
	/// The dispatcher's traced body. Runs under the tracer when the
	/// pipeline traces the dispatcher `NautilusFunction`. Reads the
	/// current profile state at C++ level and emits either the
	/// profile-update + generic path or a traced if/else into the two
	/// child `NautilusFunction`s.
	R traceDispatchBody(Args... args) {
		maybeBuildSpecialized();

		R result;
		if (specialized_) {
			auto tup = std::forward_as_tuple(args...);
			if (buildPredicate(tup, std::index_sequence_for<Args...> {})) {
				result = callSpecializedVariant(tup, NonSpecSeq {});
			} else {
				result = (*generic_)(args...);
			}
		} else {
			if (ProfilingContext::shouldCollectProfile()) {
				auto tup = std::forward_as_tuple(args...);
				emitProfileUpdates(tup, std::make_index_sequence<sizeof...(Idxs)> {});
			}
			result = (*generic_)(args...);
		}
		return result;
	}

	template <class Tup, std::size_t... Is>
	val<bool> buildPredicate(Tup& tup, std::index_sequence<Is...>) {
		// (arg_i0 == c0) && (arg_i1 == c1) && ...
		return (... && eqAt<Is>(tup));
	}

	// Compare the original argument at position `I` against the cached
	// constant stored for `I`. Only called for `I \in Idxs...`.
	template <std::size_t I, class Tup>
	val<bool> eqAt(Tup& tup) {
		if constexpr (detail::isSpecializedIndex<I, Idxs...>()) {
			constexpr std::size_t pos = detail::indexOfInPack<I, Idxs...>();
			using T = RawAt<detail::nthInPack<pos, Idxs...>()>;
			return std::get<I>(tup) == val<T>(std::get<pos>(cachedConsts_));
		} else {
			return val<bool>(true);
		}
	}

	template <class Tup, std::size_t... Ps>
	void emitProfileUpdates(Tup& tup, std::index_sequence<Ps...>) {
		(emitOneProfileUpdate<Ps>(tup), ...);
	}

	template <std::size_t P, class Tup>
	void emitOneProfileUpdate(Tup& tup) {
		constexpr std::size_t I = detail::nthInPack<P, Idxs...>();
		using T = RawAt<I>;
		auto* profile = &std::get<P>(profiles_);
		if (profile->isStable()) {
			return;
		}
		invoke(&ValueProfile<T>::profile, val<ValueProfile<T>*>(profile), static_cast<val<T>>(std::get<I>(tup)));
	}

	void maybeBuildSpecialized() {
		if (specialized_) {
			return;
		}
		if (!allStable(std::make_index_sequence<sizeof...(Idxs)> {})) {
			return;
		}
		cachedConsts_ = captureConsts(std::make_index_sequence<sizeof...(Idxs)> {});
		buildSpecializedVariant(NonSpecSeq {});
	}

	/// Build the specialized `NautilusFunction` with a reduced
	/// signature. The lambda takes only the non-specialized arguments
	/// (`NonSpecIdxs...`); constants for the specialized positions are
	/// materialised inside the lambda body as `val<T>(c)` and thus
	/// become local `arith.constant` ops in the traced body rather than
	/// dead formal parameters.
	template <std::size_t... NonSpecIdxs>
	void buildSpecializedVariant(std::index_sequence<NonSpecIdxs...>) {
		auto userFn = userFn_;
		auto consts = cachedConsts_;
		SpecFnType specLambda = [userFn, consts](std::tuple_element_t<NonSpecIdxs, ArgsTuple>... nsArgs) mutable -> R {
			auto nsTup = std::forward_as_tuple(nsArgs...);
			return detail::callWithReducedArgs(userFn, nsTup, consts, std::index_sequence_for<Args...> {},
			                                   std::index_sequence<Idxs...> {}, std::index_sequence<NonSpecIdxs...> {});
		};
		specialized_ =
		    std::make_unique<NautilusFunction<SpecFnType>>(baseName_ + "__specialized", std::move(specLambda));
	}

	/// Forward the non-specialized subset of the dispatcher's traced
	/// arguments to the specialized variant.
	template <class Tup, std::size_t... NonSpecIdxs>
	R callSpecializedVariant(Tup& tup, std::index_sequence<NonSpecIdxs...>) {
		return (*specialized_)(std::get<NonSpecIdxs>(tup)...);
	}

	template <std::size_t... Ps>
	bool allStable(std::index_sequence<Ps...>) const {
		return (... && std::get<Ps>(profiles_).isStable());
	}

	template <std::size_t... Ps>
	ConstTuple captureConsts(std::index_sequence<Ps...>) const {
		return ConstTuple {std::get<Ps>(profiles_).dominant()...};
	}

	UserFn userFn_;
	ProfileTuple profiles_;
	std::unique_ptr<NautilusFunction<UserFn>> generic_;
	std::unique_ptr<NautilusFunction<SpecFnType>> specialized_;
	std::unique_ptr<NautilusFunction<UserFn>> dispatcher_;
	ConstTuple cachedConsts_ {};
	std::string baseName_;
};

} // namespace nautilus

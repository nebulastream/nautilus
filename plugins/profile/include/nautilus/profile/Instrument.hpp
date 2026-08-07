#pragma once

#include "nautilus/profile/Profiler.hpp"
#include <utility>

namespace nautilus::profile {

// Returns a callable that wraps `fn` in a profile::ScopedRegion named `name`.
// The region is emitted every time the compiled function runs, so the
// resulting callable is suitable for passing straight to
// NautilusModule::registerFunction / NautilusEngine::registerFunction.
//
// Usage:
//
//     auto f = engine.registerFunction(
//         profile::instrument("hash_agg.build", myPipeline));
//
// Works with raw lambdas, function pointers, and std::function. The
// wrapped callable's signature is preserved.
template <typename F>
auto instrument(const char* name, F fn) {
	return [name, fn = std::move(fn)](auto... args) -> decltype(fn(args...)) {
		ScopedRegion region(name);
		return fn(args...);
	};
}

} // namespace nautilus::profile

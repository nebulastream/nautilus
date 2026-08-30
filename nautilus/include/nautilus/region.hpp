#pragma once

#include "nautilus/config.hpp"
#include <utility>

#ifdef ENABLE_TRACING

#include "nautilus/tracing/TracingInterface.hpp"
#include "nautilus/tracing/TracingUtil.hpp"
#include <functional>

#endif

// Marks a function non-inlinable so the frame-pointer walk that derives tags terminates at
// the exact region call site instead of at some enclosing inlined frame.
#if defined(__clang__) || defined(__GNUC__)
#define NAUTILUS_REGION_NOINLINE __attribute__((noinline))
#else
#define NAUTILUS_REGION_NOINLINE
#endif

namespace nautilus {

/// Executes @p fn as an isolated, memoized tracing region at this call site.
///
/// The optional @p name is a debug label only; it is not recorded in the trace.
template <typename F>
NAUTILUS_REGION_NOINLINE void region(const char* name, F&& fn);

template <typename F>
NAUTILUS_REGION_NOINLINE void region(F&& fn);

template <typename F>
NAUTILUS_REGION_NOINLINE void region([[maybe_unused]] const char* name, F&& fn) {
#ifdef ENABLE_TRACING
	if (tracing::inTracer()) {
		// A region body is already void(); it needs none of the val<T>-argument
		// machinery of engine::details::createFunctionWrapper. Wrapping a reference to
		// fn keeps the callable's identity (the tracer re-invokes it once per explored
		// path) and fits in std::function's small buffer, so this does not allocate.
		std::function<void()> wrapper = [&fn]() {
			fn();
		};
		tracing::traceRegion(wrapper);
		return;
	}
#endif
	fn();
}

template <typename F>
NAUTILUS_REGION_NOINLINE void region(F&& fn) {
	region(nullptr, std::forward<F>(fn));
}

} // namespace nautilus

#undef NAUTILUS_REGION_NOINLINE

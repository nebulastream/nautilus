#pragma once

#include "nautilus/common/RegionAttributes.hpp"
#include "nautilus/config.hpp"
#include <source_location>
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

/// Executes @p fn as an isolated, memoized tracing region at this call site, described by
/// @p attributes.
///
/// Use this overload from a helper that wraps region() itself: the attributes of the
/// other overloads describe the call site inside the helper, this one lets the helper
/// pass the position of its own caller (and a name derived from it) instead.
template <typename F>
NAUTILUS_REGION_NOINLINE void region(const RegionAttributes& attributes, F&& fn);

/// Executes @p fn as an isolated, memoized tracing region named @p name.
///
/// The name and the call site's source location are recorded as the region's attributes;
/// they are visible in the trace and in the diagnostics a malformed region produces.
/// Neither affects the traced code or its identity. Pass @p location explicitly only to
/// override the call site, as described above.
template <typename F>
NAUTILUS_REGION_NOINLINE void region(const char* name, F&& fn,
                                     std::source_location location = std::source_location::current());

/// Executes @p fn as an isolated, memoized tracing region with no user-defined name.
template <typename F>
NAUTILUS_REGION_NOINLINE void region(F&& fn, std::source_location location = std::source_location::current());

template <typename F>
NAUTILUS_REGION_NOINLINE void region([[maybe_unused]] const RegionAttributes& attributes, F&& fn) {
#ifdef ENABLE_TRACING
	if (tracing::inTracer()) {
		// A region body is already void(); it needs none of the val<T>-argument
		// machinery of engine::details::createFunctionWrapper. Wrapping a reference to
		// fn keeps the callable's identity (the tracer re-invokes it once per explored
		// path) and fits in std::function's small buffer, so this does not allocate.
		std::function<void()> wrapper = [&fn]() {
			fn();
		};
		tracing::traceRegion(wrapper, attributes);
		return;
	}
#endif
	fn();
}

template <typename F>
NAUTILUS_REGION_NOINLINE void region(const char* name, F&& fn, std::source_location location) {
	region(RegionAttributes {name, SourceLocation::from(location)}, std::forward<F>(fn));
}

template <typename F>
NAUTILUS_REGION_NOINLINE void region(F&& fn, std::source_location location) {
	region(RegionAttributes {nullptr, SourceLocation::from(location)}, std::forward<F>(fn));
}

} // namespace nautilus

#undef NAUTILUS_REGION_NOINLINE

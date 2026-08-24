#pragma once

#include "nautilus/config.hpp"
#include <utility>

#ifdef ENABLE_TRACING

#include "nautilus/tracing/TracingInterface.hpp"
#include "nautilus/tracing/TracingUtil.hpp"

#endif

// Marks a function non-inlinable so __builtin_return_address(0) inside it resolves to
// the exact caller call-site (the region key) instead of some enclosing inlined frame.
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
NAUTILUS_REGION_NOINLINE void region(const char*, F&& fn) {
#ifdef ENABLE_TRACING
	// RAII guard: guarantees traceRegionEnd() runs exactly once, on both normal exit
	// and exception unwinding (RECORD mode may throw TraceTerminationException), and
	// only when a region was actually begun.
	struct guard {
		tracing::TracingInterface* tracer = nullptr;
		bool ownsEnd = false;
		~guard() noexcept {
			if (ownsEnd) {
				tracer->traceRegionEnd();
			}
		}
	} region_guard {};
	if (tracing::inTracer()) {
		tracing::TracingInterface* tracer = tracing::getActiveTracer();
		region_guard.tracer = tracer;
		// false skips the region body (memo replay); true begins it and runs it,
		// and the guard's destructor calls traceRegionEnd() on normal and unwinding exit.
		if (tracer->traceRegionBegin(reinterpret_cast<tracing::TagAddress>(__builtin_return_address(0)))) {
			region_guard.ownsEnd = true;
			// The active tracer is now the region (traceRegionBegin switched it);
			// re-fetch it each iteration rather than reusing `tracer`, which still
			// points at the enclosing context. traceRegionContinue() drives
			// region-local exploration: it returns true (and rewinds state for
			// another pass) while the region has an unresolved internal branch,
			// so implementations that don't support local exploration just
			// return false after one invocation, unchanged from before.
			do {
				fn();
			} while (tracing::getActiveTracer()->traceRegionContinue());
		}
		return;
	}
	fn();
#else
	fn();
#endif
}

template <typename F>
NAUTILUS_REGION_NOINLINE void region(F&& fn) {
	region(nullptr, std::forward<F>(fn));
}

} // namespace nautilus

#undef NAUTILUS_REGION_NOINLINE

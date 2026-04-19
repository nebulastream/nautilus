#pragma once

#include "nautilus/config.hpp"

#ifndef ENABLE_CONSTANT_TRACER
#error "nautilus/partial_evaluation/api.hpp included without ENABLE_CONSTANT_TRACER."
#endif

#include <cstdint>

namespace nautilus::tracing::pe {

// --- Runtime flags --------------------------------------------------------
//
// Cached in thread-local storage; initial values come from env vars the
// first time each thread touches the setting.

/// Runtime enable gate. Even when built with ENABLE_CONSTANT_TRACER on,
/// this lets you disable the fold fast-paths per-thread for A/B tests
/// without rebuilding. Env var: `NAUTILUS_CONSTANT_TRACER_ENABLED`
/// (default: enabled).
[[nodiscard]] bool isConstantTracerEnabled() noexcept;
void setConstantTracerEnabled(bool enabled) noexcept;

/// Blunt iteration-cap backstop: forces `materializeAllConstants()`
/// when the running count of Constant-to-Constant assignments since
/// the last flush crosses the threshold. Env var:
/// `NAUTILUS_CONSTANT_TRACER_WIDEN_AT` (default: 1024). 0 disables.
[[nodiscard]] uint64_t getConstantWideningThreshold() noexcept;
void setConstantWideningThreshold(uint64_t threshold) noexcept;

/// Stage 2 stratified-widening unroll limit. Controls how many
/// divergent revisits of the same program point are tolerated before
/// materialize fires. Env var: `NAUTILUS_CONSTANT_TRACER_UNROLL_LIMIT`
/// (default: 0 = widen on first divergence).
///
/// Prefer the per-site `nautilus::unroll_scope(N)` RAII guard over
/// setting this globally. See unroll_scope.hpp.
[[nodiscard]] uint64_t getConstantUnrollLimit() noexcept;
void setConstantUnrollLimit(uint64_t limit) noexcept;

// --- Constant registry ----------------------------------------------------

/// Function pointer that, given a holder*, materializes that holder
/// (i.e. emits a ConstOp into the current trace and transitions the
/// holder out of the Constant state). LazyTracedRef provides a static
/// thunk.
using ConstantMaterializeFn = void (*)(void*);

/// Intrusive linked-list node embedded in every LazyTracedRef. The
/// registry of currently-Constant holders is a doubly-linked list
/// threaded through these nodes — no map lookups, no allocations on
/// register/unregister.
struct ConstantRegistryNode {
	ConstantRegistryNode* prev = nullptr;
	ConstantRegistryNode* next = nullptr;
	ConstantMaterializeFn materialize_fn = nullptr;
	/// Hash of the holder's current constant value. Used by Stage 2
	/// stratified-divergence detection.
	uint64_t value_hash = 0;

	[[nodiscard]] bool isLinked() const noexcept {
		return next != nullptr;
	}
};

/// Append `node` to the active tracer's list of currently-Constant
/// holders. No-op when no tracer is active or the runtime flag is
/// off. Must be paired with unregisterConstant on destruction or
/// materialization. O(1).
void registerConstant(ConstantRegistryNode* node, ConstantMaterializeFn materialize_fn);

/// Unlink `node` from the Constant list. Safe on an unregistered node
/// (no-op). O(1).
void unregisterConstant(ConstantRegistryNode* node);

/// Walk every currently-registered Constant holder and call its
/// materialize function. Each materialization unregisters the holder,
/// so on return the registry is empty. Also resets Stage 2 state.
///
/// Called before every trace op that can split the trace or hit a
/// snapshot-match merge (traceBool, traceBinaryOp, traceUnaryOp,
/// traceTernaryOp, traceCall/IndirectCall/NautilusCall,
/// traceReturnOperation) so each predecessor block keeps its own
/// CONST ops and the SSA phase's block-argument threading stays
/// correct.
void materializeAllConstants();

/// Record that a Constant-to-Constant assignment updated a val's
/// constant value. Drives Stage 2 widening.
void noteConstantAssignment();

// --- Observability counters -----------------------------------------------
//
// All thread-local. Reset at the start of each new trace (the positive
// `setActiveTracer` call), so the value read after `Trace(...)` returns
// reflects exactly that one trace.
[[nodiscard]] uint64_t getConstantTracerFoldsElided() noexcept;
[[nodiscard]] uint64_t getConstantTracerFlushes() noexcept;
[[nodiscard]] uint64_t getConstantTracerWideningsCap() noexcept;
[[nodiscard]] uint64_t getConstantTracerWideningsStrat() noexcept;

/// Bump the folds-elided counter. Inline-called from val<T> fold fast
/// paths. Safe with no active tracer (no-op).
void noteConstantFoldElided() noexcept;

// --- Lifecycle hooks -------------------------------------------------------

/// Drop every entry from the Constant registry *without* materializing.
/// Used as a defense-in-depth sweep at the start of each inner-function
/// trace and from `tracing::setActiveTracer(nullptr)`.
void clearConstantRegistry() noexcept;

/// Zero all observability counters. Called from
/// `tracing::setActiveTracer(non-null)` so the post-trace counter reads
/// reflect only this trace.
void resetCountersForNewTrace() noexcept;

} // namespace nautilus::tracing::pe

#pragma once

#include "nautilus/config.hpp"

#ifndef ENABLE_CONSTANT_TRACER
#error "nautilus/partial_evaluation/unroll_scope.hpp included without ENABLE_CONSTANT_TRACER."
#endif

#include "nautilus/partial_evaluation/api.hpp"
#include "nautilus/tracing/TracingUtil.hpp"
#include <cstdint>

namespace nautilus {

/// RAII override for the Stage 2 stratified-unroll limit (see
/// `tracing::pe::setConstantUnrollLimit`). For the lifetime of an instance,
/// the per-Tag divergence tolerance is raised to `max_unroll`; on
/// destruction the previous value is restored. Mirrors the per-site
/// opt-in pattern of `nautilus::assume_stable` for pointer folding.
///
///     val<int32_t> sumOfSquares() {
///         val<int32_t> sum = 0;
///         nautilus::unroll_scope hint(16);           // ← annotation
///         for (val<int32_t> i = 0; i < 10; i = i + 1) {
///             sum = sum + i * i;
///         }
///         return sum;                                 // folds to CONST 285
///     }
///
/// Composable: nested instances stack, each restoring its outer
/// predecessor on destruction.
///
/// Scope-wide, not loop-precise: an instance affects every program
/// point evaluated during its lifetime, not just the one loop it sits
/// next to. In practice this is fine because users place the guard in
/// a tight scope around the loop they care about; if a function
/// contains multiple loops with different unroll budgets, declare a
/// fresh guard in each loop's own inner scope.
class unroll_scope {
public:
	explicit unroll_scope(uint64_t max_unroll) noexcept : saved_(tracing::pe::getConstantUnrollLimit()) {
		tracing::pe::setConstantUnrollLimit(max_unroll);
	}
	~unroll_scope() noexcept {
		tracing::pe::setConstantUnrollLimit(saved_);
	}
	unroll_scope(const unroll_scope&) = delete;
	unroll_scope& operator=(const unroll_scope&) = delete;
	unroll_scope(unroll_scope&&) = delete;
	unroll_scope& operator=(unroll_scope&&) = delete;

private:
	uint64_t saved_;
};

} // namespace nautilus

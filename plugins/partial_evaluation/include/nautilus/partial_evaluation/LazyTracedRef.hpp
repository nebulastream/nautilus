#pragma once

#include "nautilus/config.hpp"

#ifdef ENABLE_CONSTANT_TRACER

#include "nautilus/partial_evaluation/api.hpp"
#include "nautilus/tracing/TracingUtil.hpp"
#include "nautilus/tracing/TypedValueRef.hpp"
#include <cstring>
#include <functional>
#include <type_traits>
#include <utility>

namespace nautilus::tracing::pe {

namespace detail {
/// Type-stable value hash for the intrusive registry node's value_hash
/// field. Uses std::hash for all trivially-hashable fundamental types; for
/// others we memcpy into an integer and hash that. Stable across trace
/// iterations because it only depends on the raw bits of the value.
///
/// Caveat for floating-point: in-tracer folding evaluates `a OP b` under
/// the host's FP environment (rounding mode, denormal handling, etc.). If
/// the generated-code backend runs with a different FP environment at
/// runtime, the folded constant can differ in the last bit from what a
/// materialized ADD/MUL would have produced. Nautilus doesn't currently
/// manipulate rounding modes, and every supported backend uses IEEE-754
/// round-to-nearest-even by default, so the common case is bit-identical.
/// If you introduce a backend or an engine option that changes rounding,
/// either pin the rounding mode during trace with std::fesetround or
/// blacklist FP ops from the fold fast path.
template <typename T>
[[nodiscard]] inline uint64_t hashConstantValue(const T& v) noexcept {
	if constexpr (std::is_integral_v<T> || std::is_enum_v<T> || std::is_pointer_v<T> || std::is_same_v<T, bool>) {
		return static_cast<uint64_t>(std::hash<T> {}(v));
	} else if constexpr (std::is_floating_point_v<T>) {
		// Hash bit-pattern so +0/-0 differ (they compare equal in <; we want
		// a "changed" signal if the constant transitioned from +0 to -0).
		uint64_t bits = 0;
		std::memcpy(&bits, &v, sizeof(T) < sizeof(uint64_t) ? sizeof(T) : sizeof(uint64_t));
		return bits;
	} else {
		return 0; // unreachable for Constant-eligible T; kept for exposition.
	}
}
} // namespace detail

/// =============================================================================
/// Constant-folding tracer state holder. EXPERIMENTAL. Default OFF.
/// =============================================================================
///
/// LazyTracedRef<T> is the trace-side state of a val<T> when the experimental
/// constant-folding tracer (ENABLE_CONSTANT_TRACER) is enabled. It holds
/// either:
///   - a pure runtime constant of type T (no trace footprint, no aliveVars
///     entry), or
///   - a tracing::TypedValueRef pointing into the trace's value table (full footprint,
///     refcounted via tracing::allocateValRef / tracing::freeValRef just like
///     TypedValueRefHolder).
///
/// Materialization (Constant -> Materialized) is lazy and happens
/// transparently the first time the holder is converted to tracing::TypedValueRef.
/// The materialized form is then cached, so subsequent accesses are O(1).
///
/// Stage 1 — block-local folding with lazy materialization:
///   - Straight-line constant folding through pure binary/unary arithmetic.
///   - Constant equality/comparison and logical ops on constants.
///   - Dead-branch elimination when an `if` condition is itself constant.
///   - Lazy materialization on first conversion to tracing::TypedValueRef, so
///     never-used constant chains have zero trace footprint.
///   - §4.3 invariant ("no elision crosses a basic-block boundary"):
///     while in the Constant state, every LazyTracedRef is linked into a
///     thread-local intrusive doubly-linked Constant registry via its
///     ConstantRegistryNode base (register/unregister are O(1) and
///     allocation-free). Before any trace op that could split the trace
///     (traceBool) or trigger a snapshot-match merge (any trace op + the
///     return hook), materializeAllConstants() walks the registry and
///     forces every live Constant to materialize, so both branches see
///     stable trace refs and the existing merge logic produces the right
///     phi nodes.
///
/// Stage 2 — loop-aware widening:
///   - Iteration-cap backstop (NAUTILUS_CONSTANT_TRACER_WIDEN_AT, default
///     1024): after this many consecutive Constant-to-Constant assignments
///     without a materialization, the registry is flushed. Bounds trace
///     time in pathological cases where a loop folds on every iteration
///     (e.g. `for (val<i64> i = 0; i < N; ++i)` with constant N).
///   - Stratified divergence detection: TracingInterface::currentTag()
///     exposes a pointer-interned program-point identifier; each
///     LazyTracedRef carries a per-holder value_hash updated on every
///     value change. noteConstantAssignment() computes a registry-wide
///     value-hash and looks up the current Tag in a thread-local map.
///     A revisit with a different hash is a back-edge with changed
///     Constants — if the per-tag divergence count has crossed
///     NAUTILUS_CONSTANT_TRACER_UNROLL_LIMIT (default 0, i.e. widen on
///     first divergence), the registry is flushed immediately. This
///     catches runaway trace-time unrolling in 2 iterations rather than
///     waiting for the blunt cap to fire.
///   - The runtime flag NAUTILUS_CONSTANT_TRACER_ENABLED toggles the
///     feature at trace-start time without a rebuild; when off,
///     value-constructed LazyTracedRefs materialize eagerly and the
///     behavior matches pre-Stage-1 TypedValueRefHolder exactly.
template <typename T>
class LazyTracedRef : public ConstantRegistryNode {
public:
	/// Default-construct as a constant zero (no trace footprint).
	/// Constant tracking is only enabled for types representable as a
	/// ConstantLiteral; for others (e.g. long double) the holder is
	/// permanently in the Materialized-but-empty state and any conversion to
	/// tracing::TypedValueRef returns the default ref. Such types are expected to
	/// never appear as user-constructed val<T>(literal) — only as function
	/// arguments / loaded values, which use the tracing::TypedValueRef constructor
	/// below.
	///
	/// Runtime gate (NAUTILUS_CONSTANT_TRACER_ENABLED / tracing::
	/// setConstantTracerEnabled): when off, value-constructed LazyTracedRefs
	/// materialize eagerly (matching the TypedValueRefHolder path used when
	/// ENABLE_CONSTANT_TRACER is compiled out), so the whole feature can be
	/// A/B-toggled at runtime without a rebuild.
	LazyTracedRef() : is_constant_(false), constant_value_(T()), ref_(0, tracing::TypeResolver<T>::to_type()) {
		if constexpr (std::is_constructible_v<ConstantLiteral, T>) {
			if (isConstantTracerEnabled()) {
				is_constant_ = true;
				this->value_hash = detail::hashConstantValue(constant_value_);
				registerSelfIfConstant();
				return;
			}
			ref_ = tracing::traceConstant(constant_value_);
			tracing::allocateValRef(ref_.ref);
		}
	}

	/// Construct as a known runtime constant (no trace footprint, subject to
	/// the runtime-enable gate above).
	explicit LazyTracedRef(T value)
	    : is_constant_(false), constant_value_(value), ref_(0, tracing::TypeResolver<T>::to_type()) {
		if constexpr (std::is_constructible_v<ConstantLiteral, T>) {
			if (isConstantTracerEnabled()) {
				is_constant_ = true;
				this->value_hash = detail::hashConstantValue(constant_value_);
				registerSelfIfConstant();
				return;
			}
			ref_ = tracing::traceConstant(value);
			tracing::allocateValRef(ref_.ref);
		}
	}

	/// Construct from an existing trace ref. Increments aliveVars refcount.
	explicit LazyTracedRef(tracing::TypedValueRef ref) : is_constant_(false), constant_value_(T()), ref_(ref) {
		tracing::allocateValRef(ref_.ref);
	}

	/// Copy construct. If other is constant, no trace work; otherwise tracing::traceCopy.
	LazyTracedRef(const LazyTracedRef& other)
	    : is_constant_(other.is_constant_), constant_value_(other.constant_value_), ref_(other.ref_) {
		if (!is_constant_) {
			ref_ = tracing::traceCopy(other.ref_);
			tracing::allocateValRef(ref_.ref);
		} else {
			this->value_hash = detail::hashConstantValue(constant_value_);
		}
		registerSelfIfConstant();
	}

	/// Move construct. Transfers ownership of the materialized ref (no
	/// allocate/free pair). For Constants both source and destination remain
	/// registered with identical constant values; if the registry is flushed
	/// in between, the source materializes a redundant ConstOp — semantically
	/// harmless and very rare in practice (move on a Constant val<T> is
	/// usually elided by RVO).
	LazyTracedRef(LazyTracedRef&& other) noexcept
	    : is_constant_(other.is_constant_), constant_value_(other.constant_value_), ref_(other.ref_) {
		if (!is_constant_) {
			// Transfer ownership: clear source so its dtor doesn't free.
			other.ref_ = tracing::TypedValueRef(0, tracing::TypeResolver<T>::to_type());
			other.is_constant_ = true;
		} else {
			this->value_hash = detail::hashConstantValue(constant_value_);
		}
		registerSelfIfConstant();
	}

	/// Copy assign. Releases the current state (registry + ref ownership)
	/// and adopts the new state.
	LazyTracedRef& operator=(const LazyTracedRef& other) {
		if (this == &other) {
			return *this;
		}
		release();
		is_constant_ = other.is_constant_;
		constant_value_ = other.constant_value_;
		ref_ = other.ref_;
		if (!is_constant_) {
			ref_ = tracing::traceCopy(other.ref_);
			tracing::allocateValRef(ref_.ref);
		}
		registerSelfIfConstant();
		return *this;
	}

	/// Move assign.
	LazyTracedRef& operator=(LazyTracedRef&& other) noexcept {
		if (this == &other) {
			return *this;
		}
		release();
		is_constant_ = other.is_constant_;
		constant_value_ = other.constant_value_;
		ref_ = other.ref_;
		if (!is_constant_) {
			other.ref_ = tracing::TypedValueRef(0, tracing::TypeResolver<T>::to_type());
			other.is_constant_ = true;
		}
		registerSelfIfConstant();
		return *this;
	}

	~LazyTracedRef() {
		release();
	}

	/// True iff this holder is still a pure compile-time constant with no
	/// trace footprint.
	[[nodiscard]] bool isConstant() const noexcept {
		return is_constant_;
	}

	/// Returns the underlying constant value. Precondition: isConstant().
	[[nodiscard]] T constantValue() const noexcept {
		return constant_value_;
	}

	/// Forces materialization. After this call, the holder is in the
	/// Materialized state and the returned ref is stable for the lifetime of
	/// the holder. Idempotent.
	///
	/// The actual `tracing::traceConstant` call is gated on `T` being a valid
	/// `ConstantLiteral` variant member (e.g. excludes long double). For
	/// types where this gate fails, no constant state can ever exist
	/// (see the value constructor), so the runtime branch is unreachable —
	/// we keep the call out of compilation entirely so the materialize()
	/// instantiation succeeds for arg-only types like val<long double>.
	const tracing::TypedValueRef& materialize() const {
		if constexpr (std::is_constructible_v<ConstantLiteral, T>) {
			if (is_constant_) {
				auto& self = const_cast<LazyTracedRef&>(*this);
				// Unregister BEFORE flipping state so a re-entrant flush
				// (via materializeAllConstants snapshotting) doesn't see us
				// as an in-flight Constant any more.
				unregisterConstant(static_cast<ConstantRegistryNode*>(&self));
				self.ref_ = tracing::traceConstant(constant_value_);
				tracing::allocateValRef(self.ref_.ref);
				self.is_constant_ = false;
			}
		}
		return ref_;
	}

	/// Implicit conversion forces materialization. This is the bridge that
	/// keeps existing code (which passes `value.state` directly to trace*Op
	/// functions) working without changes.
	operator const tracing::TypedValueRef&() const { // NOLINT(google-explicit-constructor)
		return materialize();
	}

	/// Replace with a fresh trace ref (used by assignment operators).
	void resetToRef(tracing::TypedValueRef new_ref) {
		release();
		is_constant_ = false;
		ref_ = new_ref;
		tracing::allocateValRef(ref_.ref);
	}

	/// Replace with a fresh constant value, dropping any trace footprint.
	/// Called from operator= on the Constant-to-Constant fast path. Two
	/// widening paths hang off this call (both implemented in TracingUtil
	/// via noteConstantAssignment()):
	///   - Blunt iteration-cap backstop: every call bumps
	///     constantAssignmentCount; crossing the threshold flushes.
	///   - Stratified divergence detection: the freshly updated per-node
	///     value_hash participates in the registry-wide value hash; a
	///     revisit of the same program point with a different hash widens
	///     once the per-tag divergence count exceeds the configured unroll
	///     limit.
	void resetToConstant(T new_value) {
		release();
		is_constant_ = true;
		constant_value_ = new_value;
		ref_ = tracing::TypedValueRef(0, tracing::TypeResolver<T>::to_type());
		this->value_hash = detail::hashConstantValue(new_value);
		registerSelfIfConstant();
		noteConstantAssignment();
	}

private:
	/// Type-erased thunk so the registry can call materialize() through a
	/// ConstantRegistryNode* without knowing T. The input is the node
	/// pointer (this); static_cast back to the derived LazyTracedRef
	/// recovers the full object because LazyTracedRef inherits
	/// ConstantRegistryNode as its first base.
	static void materialize_thunk(void* p) {
		static_cast<LazyTracedRef*>(static_cast<ConstantRegistryNode*>(p))->materialize();
	}

	void registerSelfIfConstant() {
		if (is_constant_) {
			registerConstant(static_cast<ConstantRegistryNode*>(this), &materialize_thunk);
		}
	}

	void release() noexcept {
		if (is_constant_) {
			unregisterConstant(static_cast<ConstantRegistryNode*>(this));
		} else {
			tracing::freeValRef(ref_.ref);
		}
	}

	bool is_constant_;
	T constant_value_;
	tracing::TypedValueRef ref_;
};

} // namespace nautilus::tracing::pe

#endif // ENABLE_CONSTANT_TRACER

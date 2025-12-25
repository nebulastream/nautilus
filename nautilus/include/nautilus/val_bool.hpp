#pragma once

#include "nautilus/val_concepts.hpp"
#include "nautilus/val_details.hpp"
#include <utility>

namespace nautilus {

/// Type alias for specifying the probability of a boolean value being true.
///
/// The probability is a value between 0.0 and 1.0, where:
/// - 0.0 means the value is definitely false
/// - 1.0 means the value is definitely true
/// - 0.5 (default) means the value is unknown
///
/// This type is used with val<bool>::setIsTrueProbability() to provide hints
/// to the JIT compiler for branch prediction optimization.
///
/// # Usage
/// ```cpp
/// val<bool> likely_condition = get_condition();
/// likely_condition.setIsTrueProbability(0.95);  // 95% likely to be true
/// ```
///
/// # Performance Impact
/// Accurate probability hints can significantly improve code generation quality
/// and reduce branch misprediction penalties in the generated machine code.
using TrueProbability = double;

/// Specialization of val<T> for boolean values with probability tracking.
///
/// This template specialization provides a traced wrapper for boolean values,
/// enabling dynamic branch prediction and probability tracking during JIT compilation.
/// It extends the standard value wrapper with probability-based optimization hints.
///
/// # Key Features
///
/// 1. **Tracing Integration**: Boolean operations are recorded in the execution trace,
///    allowing the JIT compiler to optimize branch predictions based on runtime behavior.
///
/// 2. **Probability Tracking**: Each boolean value can be associated with a probability
///    of being true (default 0.5). The compiler uses this hint for branch prediction:
///    - Values with probability > 0.5 are treated as likely true
///    - Values with probability < 0.5 are treated as likely false
///    - This allows generation of faster code paths for the common case
///
/// 3. **Logical Operations**: Supports logical AND (&&), OR (||), and NOT (!) operations
///    with proper tracing and both val<bool> and raw bool operands.
///
/// 4. **Type Conversion**: Transparent conversion to bool for use in if statements
///    and other boolean contexts. The conversion operator calls traceBool() when tracing.
///
/// 5. **Copy and Move Semantics**: Full support for copy and move constructors with
///    proper state management in both traced and non-traced modes.
///
/// # Constructors
///
/// - **Default**: `val<bool>()` - Initializes to false with 0.5 probability
/// - **Value**: `val<bool>(bool)` - Initializes with explicit boolean value
/// - **Copy**: `val<bool>(const val<bool>&)` - Copies both value and state
/// - **Move**: `val<bool>(val<bool>&&)` - Moves both value and state
/// - **Trace**: `val<bool>(TypedValueRef&)` - Internal use for traced values
///
/// # Member Functions
///
/// - `operator bool()` - Converts to bool, recording the conversion if tracing
/// - `operator=(const val<bool>&)` - Assignment with tracing support
/// - `setIsTrueProbability(TrueProbability)` - Sets the probability hint
///
/// # Example Usage
///
/// ```cpp
/// #include "nautilus/val.hpp"
/// #include "nautilus/Engine.hpp"
///
/// Engine engine;
///
/// // Create a traced boolean value
/// val<bool> condition = false;
///
/// // Set probability hint: 90% likely to be true
/// condition.setIsTrueProbability(0.9);
///
/// // Use in compiled functions
/// auto executable = engine.Compile<int(val<bool>)>(
///     [](val<bool> cond) {
///         if (cond) {
///             return 1;  // True branch
///         } else {
///             return 0;  // False branch
///         }
///     }
/// );
///
/// // Execute the compiled function
/// int result = executable(true);  // Returns 1
/// ```
///
/// # Probability Recommendations
///
/// | Probability | Scenario |
/// |-------------|----------|
/// | 0.0-0.2 | Very unlikely (error/exceptional cases) |
/// | 0.2-0.4 | Unlikely (uncommon cases) |
/// | 0.4-0.6 | Unknown/no hint (default 0.5) |
/// | 0.6-0.8 | Likely (common cases) |
/// | 0.8-1.0 | Very likely (normal/expected cases) |
///
/// # Tracing Behavior
///
/// When `ENABLE_TRACING` is defined:
/// - Each boolean operation is recorded in the execution trace
/// - The trace captures operands, operation type, and results
/// - The probability hint is passed to traceBool() during conversion
/// - The JIT compiler uses this information for optimization
///
/// When `ENABLE_TRACING` is not defined:
/// - val<bool> behaves like a regular bool with minimal overhead
/// - The probability field is still present but has no effect
/// - Compiled code has zero runtime overhead compared to raw bool
///
/// # Performance Considerations
///
/// - The probability field is a single double (8 bytes)
/// - Copying val<bool> is equivalent to copying a bool plus state reference
/// - The tracing overhead is only incurred during trace collection, not during execution
/// - Setting accurate probabilities can reduce generated code size and improve performance
/// - Inaccurate probabilities may slow down code generation with suboptimal branch ordering
///
/// # Comparison with val<Arithmetic>
///
/// Unlike the generic arithmetic specialization (val<int>, val<double>, etc.),
/// val<bool> includes:
/// - Probability tracking through the probability_ field
/// - Special conversion operator with probability reporting
/// - No arithmetic operators (bool arithmetic is not defined)
/// - Logical operators only (&&, ||, !)
///
/// @see val_arith.hpp for generic arithmetic template specialization
/// @see val_details.hpp for implementation utilities
/// @see val.hpp for main header and operators
///
template <>
class val<bool> {
public:
	/// The raw type stored by this value wrapper
	using raw_type = bool;

	/// The basic underlying type
	using basic_type = bool;

#ifdef ENABLE_TRACING
	/// Holds the tracing state for this value when tracing is enabled
	const tracing::TypedValueRefHolder state;
#endif

	/// Default constructor.
	///
	/// Initializes the boolean value to false with default probability 0.5.
	/// If tracing is enabled, records a constant 0 (false) in the trace.
	///
	/// # Example
	/// ```cpp
	/// val<bool> b;  // b = false, probability = 0.5
	/// ```
#ifdef ENABLE_TRACING
	val() : state(tracing::traceConstant(0)), value(false) {
	}
#else
	val() {
	}
#endif

	/// Constructs a val<bool> from a boolean value.
	///
	/// Initializes with the provided boolean value and default probability 0.5.
	/// If tracing is enabled, records the boolean constant in the execution trace.
	///
	/// # Arguments
	/// * `value` - The boolean value to wrap (true or false)
	///
	/// # Example
	/// ```cpp
	/// val<bool> a = true;   // a = true, probability = 0.5
	/// val<bool> b = false;  // b = false, probability = 0.5
	/// ```
#ifdef ENABLE_TRACING
	val(bool value) : state(tracing::traceConstant(value)), value(value) {
	}
#else
	val(bool value) : value(value) {
	}
#endif

	/// Copy constructor.
	///
	/// Creates a copy of another val<bool>, including both its value and
	/// tracing state (when tracing is enabled). The copy operation is recorded
	/// in the execution trace for precise tracking.
	///
	/// # Arguments
	/// * `other` - The val<bool> instance to copy
	///
	/// # Example
	/// ```cpp
	/// val<bool> a = true;
	/// val<bool> b = a;  // b is a copy of a
	/// assert(b == a);
	/// ```
#ifdef ENABLE_TRACING
	val(const val<bool>& other) : state(tracing::traceCopy(other.state)), value(other.value) {
	}
#else
	val(const val<bool>& other) : value(other.value) {
	}
#endif

	/// Move constructor.
	///
	/// Moves the contents of another val<bool> into this instance, including
	/// both the boolean value and the tracing state. The move operation is
	/// efficient and preserves state information.
	///
	/// # Arguments
	/// * `other` - The val<bool> instance to move from (will be in valid but unspecified state)
	///
	/// # Example
	/// ```cpp
	/// val<bool> get_flag() {
	///     return val<bool>(true);  // Uses move constructor
	/// }
	/// ```
#ifdef ENABLE_TRACING
	val(val<bool>&& other) noexcept : state(std::move(other.state)), value(std::move(other.value)) {
	}
#else
	val(val<bool>&& other) noexcept : value(std::move(other.value)) {
	}
#endif

	/// Constructs a val<bool> from a TypedValueRef for internal tracing use.
	///
	/// This constructor is used internally by the tracing system to create
	/// val<bool> instances from traced operations. User code should not call
	/// this constructor directly.
	///
	/// # Arguments
	/// * `tc` - A TypedValueRef representing a traced boolean value
	///
	/// @internal This is for internal tracing machinery only
#ifdef ENABLE_TRACING
	val(tracing::TypedValueRef& tc) : state(tc), value() {
	}
#endif

	/// Copy assignment operator.
	///
	/// Assigns the value and state from another val<bool> to this instance.
	/// The assignment is recorded in the execution trace when tracing is enabled.
	///
	/// # Arguments
	/// * `other` - The val<bool> to assign from
	///
	/// # Returns
	/// A reference to this val<bool> instance (for chaining assignments)
	///
	/// # Example
	/// ```cpp
	/// val<bool> a = true;
	/// val<bool> b = false;
	/// b = a;  // b now contains true
	/// assert(b == a);
	/// ```
	val<bool>& operator=(const val<bool>& other) {
#ifdef ENABLE_TRACING
		tracing::traceAssignment(state, other.state, Type::b);
#endif
		this->value = other.value;
		return *this;
	}

	/// Converts this val<bool> to a native bool.
	///
	/// Enables use of val<bool> in control flow statements and boolean contexts.
	/// If tracing is enabled, this conversion is recorded with the probability hint,
	/// allowing the compiler to track branch prediction paths.
	///
	/// # Returns
	/// The underlying boolean value
	///
	/// # Example
	/// ```cpp
	/// val<bool> condition = get_value();
	/// if (condition) {  // Uses operator bool()
	///     do_something();
	/// }
	/// ```
	operator bool() const {
		if SHOULD_TRACE () {
#ifdef ENABLE_TRACING
			auto ref = state;
			return tracing::traceBool(ref, this->probability);
#endif
		}
		return value;
	}

	/// Sets the probability hint for branch prediction optimization.
	///
	/// This method allows you to provide a hint about how likely this boolean
	/// value is to be true. The JIT compiler uses this information to generate
	/// optimized code with the true branch as the fast path.
	///
	/// Setting accurate probabilities can significantly improve the quality of
	/// generated code by reducing branch misprediction penalties.
	///
	/// # Arguments
	/// * `prob` - A probability value between 0.0 and 1.0
	///   - 0.0: Value is definitely false
	///   - 1.0: Value is definitely true
	///   - 0.5: Value is unknown (default)
	///
	/// # Example
	/// ```cpp
	/// // For a condition that's usually true
	/// val<bool> likely_true = some_check();
	/// likely_true.setIsTrueProbability(0.95);
	///
	/// // For a condition that's usually false
	/// val<bool> likely_false = error_check();
	/// likely_false.setIsTrueProbability(0.05);
	///
	/// // For unknown conditions
	/// val<bool> unknown = external_data();
	/// unknown.setIsTrueProbability(0.5);  // Default
	/// ```
	///
	/// # Performance Tips
	/// - Use accurate probabilities based on profiling or domain knowledge
	/// - Default 0.5 is safe if you don't have probability information
	/// - Inaccurate probabilities can slow down code generation
	/// - Branch-heavy code benefits most from accurate probabilities
	void setIsTrueProbability(TrueProbability prob) {
		this->probability = prob;
	}

private:
	template <typename>
	friend struct details::RawValueResolver;

	/// The underlying boolean value (true or false)
	bool value;

	/// The probability of this value being true (0.0 to 1.0).
	/// Default is 0.5, meaning the value's truthiness is unknown.
	/// Used for branch prediction optimization in the JIT compiler.
	/// Set via setIsTrueProbability().
	TrueProbability probability = 0.5;
};

// ============================================================================
// Logical Operators for val<bool>
// ============================================================================

namespace details {

/// Logical OR operation for boolean values
val<bool> inline lOr(const val<bool>& left, const val<bool>& right) {
#ifdef ENABLE_TRACING
	if SHOULD_TRACE () {
		auto tc = tracing::traceBinaryOp(tracing::OR, Type::b, left.state, right.state);
		return val<bool> {tc};
	}
#endif
	return RawValueResolver<bool>::getRawValue(left) || RawValueResolver<bool>::getRawValue(right);
}

/// Logical AND operation for boolean values
val<bool> inline lAnd(const val<bool>& left, const val<bool>& right) {
#ifdef ENABLE_TRACING
	if SHOULD_TRACE () {
		auto tc = tracing::traceBinaryOp(tracing::AND, Type::b, left.state, right.state);
		return val<bool> {tc};
	}
#endif
	return RawValueResolver<bool>::getRawValue(left) && RawValueResolver<bool>::getRawValue(right);
}

/// Logical NOT operation for boolean values
val<bool> inline lNot(const val<bool>& arg) {
#ifdef ENABLE_TRACING
	if SHOULD_TRACE () {
		auto tc = tracing::traceUnaryOp(tracing::NOT, Type::b, arg.state);
		return val<bool> {tc};
	}
#endif
	return !RawValueResolver<bool>::getRawValue(arg);
}

} // namespace details

// Logical operator overloads for bool values
auto inline operator||(bool left, const val<bool>& right) {
	auto leftVal = make_value(left);
	return details::lOr(leftVal, right);
}

auto inline operator||(const val<bool>& left, bool right) {
	auto rightVal = make_value(right);
	return details::lOr(left, rightVal);
}

auto inline operator||(const val<bool>& left, const val<bool>& right) {
	return details::lOr(left, right);
}

auto inline operator&&(bool left, const val<bool>& right) {
	auto leftVal = make_value(left);
	return details::lAnd(leftVal, right);
}

auto inline operator&&(const val<bool>& left, bool right) {
	auto rightVal = make_value(right);
	return details::lAnd(left, rightVal);
}

auto inline operator&&(const val<bool>& left, const val<bool>& right) {
	return details::lAnd(left, right);
}

auto inline operator!(const val<bool>& left) {
	return details::lNot(left);
}

} // namespace nautilus

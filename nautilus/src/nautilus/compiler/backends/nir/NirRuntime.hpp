#pragma once

#include <cstdint>

// The runtime helpers that every NIR backend may call (docs/plans/nautilus-0.4/06-semantics.md §4). They are plain C++
// functions of the library, never `pointers` entries of a module.

namespace nautilus::compiler::nirbackend {

/// Throws `nautilus::ArithmeticException`: the cold path of a failed `div`/`rem` check (13 §5.10).
void nautilus_rt_throw_arith();

/// `nautilus_rt_throw_arith` called through the capture thunk of its type, for backends that cannot unwind through
/// their own frames: the exception is stored in the current exception frame instead of being thrown.
void* throwArithThunk();
void* throwArithTarget();

/// Float `rem`: C++ `fmod` (13 §5.10).
float nautilus_rt_fmod_f32(float a, float b);
double nautilus_rt_fmod_f64(double a, double b);

/// The saturating conversions of `fptosi` and `fptoui` (13 §5.5, D11): NaN gives 0, a value outside the range of the
/// result gives its minimum or maximum. `signedResult` selects `fptosi`, `bits` is the width of the integer result and
/// `sourceBits` the width of the float. The helper takes the float and returns the integer of that width, signed or unsigned.
void* floatToIntHelper(bool signedResult, unsigned bits, unsigned sourceBits);

} // namespace nautilus::compiler::nirbackend

#pragma once

#include "nautilus/val.hpp"
namespace nautilus {

/*
The boolean argument to this function is defined to be true.
The optimizer may analyze the form of the expression provided as the argument and deduce from that information used to
optimize the program. If the condition is violated during execution, the behavior is undefined. The argument itself is
never evaluated, so any side effects of the expression will be discarded.
*/
void nautilus_assume(val<bool> condition);

/*
The integer alignment argument to this function is assumed to be a power of two,
and the pointer provided is assumed to be aligned to at least that boundary.
The optimizer may use this information to generate more efficient memory
access instructions or eliminate unnecessary alignment checks.

If the pointer is not actually aligned to the specified boundary at runtime,
the behavior is undefined.

Like `nautilus_assume`, the pointer expression itself is never evaluated for
side effects; only its value is used for optimization.
*/
void nautilus_assume_aligned(val<void*> ptr, int alignment);

/*
Tells the optimizer that `value` is equal to `expected_value`. Along all
dominated uses, the optimizer is then free to fold `value` to the literal
constant, achieving the same code as if the value had been a compile-time
constant -- without forcing the user to recompile the function per value.

If `value != expected_value` at runtime, behavior is undefined.
*/
void nautilus_assume_constant(val<int64_t> value, int64_t expected_value);

/*
Tells the optimizer that `value` lies within the inclusive interval [lo, hi].
Enables range-based optimizations: bounds-check elimination, narrower integer
codegen, division/modulo strength reduction.

If `value` is outside [lo, hi] at runtime, behavior is undefined.
*/
void nautilus_assume_range(val<int64_t> value, int64_t lo, int64_t hi);

/*
Tells the optimizer that `value != 0`. Eliminates undefined-behavior checks
on division and intrinsics like ctz/clz.

If `value == 0` at runtime, behavior is undefined.
*/
void nautilus_assume_nonzero(val<int64_t> value);

} // namespace nautilus

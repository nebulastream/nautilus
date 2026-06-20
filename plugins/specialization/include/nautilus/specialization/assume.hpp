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
The two pointer arguments are assumed to point into separate, non-overlapping
storage; i.e. memory accessed through one pointer never aliases memory accessed
through the other. This is the equivalent of the C `restrict` qualifier or the
LLVM `noalias` attribute, expressed as an assumption between two concrete
pointers.

The optimizer may use this information to reorder, hoist, or vectorize memory
accesses through the pointers that it would otherwise have to keep ordered
because of a potential alias.

If the pointers actually alias (overlap) at runtime, the behavior is undefined.

Like the other assume helpers, the pointer expressions are never evaluated for
side effects; only their values are used for optimization.

Typed pointers (e.g. `val<int32_t*>`) implicitly convert to `val<void*>`, so
they can be passed directly without an explicit cast.
*/
void nautilus_assume_noalias(val<void*> a, val<void*> b);

} // namespace nautilus

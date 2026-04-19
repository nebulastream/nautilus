#pragma once

// Static anchors for the NAUTILUS_INLINE-annotated helpers referenced by
// the plugin test binaries. Taking the address of each helper with
// `[[maybe_unused]] static auto*` ensures the function symbols are actually
// emitted into each test TU, which in turn lets the Clang inlining pass
// plugin (running at compile time over the test binary) discover them and
// populate `InlineFunctionRegistry` at program startup.
//
// Including this header from every plugin test TU that exercises the
// registry keeps the anchor list in one place, avoiding the drift that
// occurred when InliningExecutionTest.cpp and LLVMIRInliningTest.cpp each
// declared their own (initially identical, now easy to diverge) anchors.

#include "RuntimeCallFunctions.hpp"

namespace nautilus::engine::detail {

[[maybe_unused]] static auto* inlining_anchor_add = &add;
[[maybe_unused]] static auto* inlining_anchor_sub = &sub;
[[maybe_unused]] static Derived inlining_anchor_derived;

} // namespace nautilus::engine::detail

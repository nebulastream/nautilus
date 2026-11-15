#pragma once

#include "nautilus/passes/Pass.hpp"

namespace nautilus::passes {

/// Moves loop-invariant instructions that have no effect, cannot throw and read no memory into the preheader of their
/// loop (`ir.enableLICM`). Loads, stores and calls with effects stay.
std::unique_ptr<Pass> makeLicmPass();

/// Replaces `base + iv * stride` in a counted loop with a pointer the back edge advances
/// (`ir.enableStrengthReduction`).
std::unique_ptr<Pass> makeStrengthReductionPass();

struct InlineConfig {
	/// The largest callee, in instructions, that is inlined (`ir.inline.maxInsts`).
	uint32_t maxInsts = 64;
	/// A caller does not grow beyond this many instructions by inlining.
	uint32_t maxCallerInsts = 4096;
	/// Inline the body of an imported callee whose exporter is known (13 §4.4.3).
	bool crossModule = true;
};

/// Replaces calls of internal callees (and, across modules, of imported ones) by their bodies (`ir.inline`), callees
/// first, and removes the internal functions nothing refers to afterwards.
std::unique_ptr<Pass> makeInlinePass(InlineConfig config);

/// The options of the optional passes of `options`: which core passes are enabled.
struct OptionalPasses {
	bool inlining = false;
	bool licm = false;
	bool strengthReduction = false;
	InlineConfig inlineConfig;
	size_t maxIterations = 2;

	[[nodiscard]] bool any() const {
		return inlining || licm || strengthReduction;
	}
};
OptionalPasses optionalPassesOf(const engine::Options& options);

/// The group of enabled core optional passes: inlining, then LICM, then strength reduction.
std::vector<std::unique_ptr<Pass>> makeOptionalPasses(const OptionalPasses& which);

} // namespace nautilus::passes

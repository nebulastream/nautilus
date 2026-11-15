#include "nautilus/passes/Passes.hpp"

namespace nautilus::passes {

OptionalPasses optionalPassesOf(const engine::Options& options) {
	OptionalPasses which;
	which.inlining = options.getOptionOrDefault("ir.inline", false);
	which.licm = options.getOptionOrDefault("ir.enableLICM", false);
	which.strengthReduction = options.getOptionOrDefault("ir.enableStrengthReduction", false);
	which.inlineConfig.maxInsts =
	    static_cast<uint32_t>(std::max(0, options.getOptionOrDefault("ir.inline.maxInsts", 64)));
	which.inlineConfig.crossModule = options.getOptionOrDefault("ir.inline.crossModule", true);
	which.maxIterations = static_cast<size_t>(std::max(1, options.getOptionOrDefault("ir.maxPipelineIterations", 4)));
	return which;
}

std::vector<std::unique_ptr<Pass>> makeOptionalPasses(const OptionalPasses& which) {
	std::vector<std::unique_ptr<Pass>> group;
	if (which.inlining) {
		group.push_back(makeInlinePass(which.inlineConfig));
	}
	if (which.licm) {
		group.push_back(makeLicmPass());
	}
	if (which.strengthReduction) {
		group.push_back(makeStrengthReductionPass());
	}
	return group;
}

} // namespace nautilus::passes

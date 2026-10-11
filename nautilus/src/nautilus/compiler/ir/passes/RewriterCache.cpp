#include "nautilus/compiler/ir/passes/RewriterCache.hpp"
#include "nautilus/compiler/ir/IRGraph.hpp"
#include "nautilus/compiler/ir/passes/IRPass.hpp"

namespace nautilus::compiler::ir {

FunctionRewriter& RewriterCache::rewriterFor(FunctionOperation& fn) {
	auto& rewriter = rewriters_[&fn];
	if (rewriter == nullptr) {
		rewriter = std::make_unique<FunctionRewriter>(fn, ir_.getArena());
	}
	return *rewriter;
}

bool IRPass::apply(IRGraph& ir, RewriterCache& rewriters) {
	const bool changed = apply(ir);
	if (changed) {
		rewriters.invalidate();
	}
	return changed;
}

} // namespace nautilus::compiler::ir

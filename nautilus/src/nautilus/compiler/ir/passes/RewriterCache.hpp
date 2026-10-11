#pragma once

#include "nautilus/compiler/ir/passes/FunctionRewriter.hpp"
#include <memory>
#include <unordered_map>

namespace nautilus::compiler::ir {

class IRGraph;

/**
 * @brief One `FunctionRewriter` session per function, shared by the passes
 * of an `IRPassManager` run.
 *
 * Building a session indexes every value of the function, and a pass that
 * built its own would pay that for every run -- on trace-sized graphs more
 * than the pass itself. A session stays valid as long as every mutation of
 * its function goes through it, which is the contract the rewriter already
 * has. Passes that keep it take their session from here (`rewriterFor`); a
 * pass that mutates the graph any other way runs through the plain
 * `IRPass::apply` and, if it changed anything, drops every session
 * (`invalidate`) so the next pass indexes the graph afresh.
 */
class RewriterCache {
public:
	explicit RewriterCache(IRGraph& ir) : ir_(ir) {
	}

	/// The session for @p fn, built on first use.
	FunctionRewriter& rewriterFor(FunctionOperation& fn);

	/// Drops every session; the next `rewriterFor` re-indexes its function.
	void invalidate() {
		rewriters_.clear();
	}

private:
	IRGraph& ir_;
	std::unordered_map<const FunctionOperation*, std::unique_ptr<FunctionRewriter>> rewriters_;
};

} // namespace nautilus::compiler::ir

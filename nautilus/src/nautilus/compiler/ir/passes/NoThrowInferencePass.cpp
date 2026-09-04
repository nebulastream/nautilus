#include "nautilus/compiler/ir/passes/NoThrowInferencePass.hpp"
#include "nautilus/compiler/ir/operations/CallOperation.hpp"
#include "nautilus/compiler/ir/operations/FunctionOperation.hpp"
#include "nautilus/compiler/ir/operations/IndirectCallOperation.hpp"
#include <unordered_map>

namespace nautilus::compiler::ir {

namespace {

/// Resolves a Nautilus-to-Nautilus proxy call to its callee's
/// FunctionOperation, or nullptr if the module has no function under that
/// name (shouldn't happen for a well-formed module, but a missing target is
/// treated the same as "unknown" -- conservatively throwing -- rather than
/// asserting).
const FunctionOperation* resolveCallee(const IRGraph& ir, const CallOperation& call) {
	return ir.getFunctionOperation(call.getFunctionName());
}

/// Whether a call site is potentially throwing, i.e. whether
/// ExceptionRegionPreparationPass would collect it. Both conditions matter:
/// `noUnwind` defaults to false, so a call can carry no exception-handling
/// flag and still be a throw site. This predicate must stay in lockstep
/// with that pass, otherwise this analysis could prove a function noThrow
/// while the region pass still builds a landing pad inside it.
template <typename CallOp>
bool isPotentiallyThrowing(const CallOp& call) {
	return call.requiresExceptionHandling() || !call.getFunctionAttributes().noUnwind;
}

/// Current estimate for @p fn, treating a function missing from the map as
/// throwing rather than indexing into it unchecked.
bool lookupCanThrow(const std::unordered_map<const FunctionOperation*, bool>& canThrow, const FunctionOperation* fn) {
	const auto it = canThrow.find(fn);
	return it == canThrow.end() || it->second;
}

/// True if `fn`, given the current (possibly partial) `canThrow` estimate
/// for every function in the module, can throw: it contains a potentially
/// throwing raw external call, an indirect call (callee unknown, always
/// conservative), or a Nautilus-to-Nautilus call into a function this
/// estimate already marks canThrow.
bool computeCanThrow(const FunctionOperation& fn, const IRGraph& ir,
                     const std::unordered_map<const FunctionOperation*, bool>& canThrow) {
	for (const auto* block : fn.getBasicBlocks()) {
		for (const auto* op : block->getOperations()) {
			if (const auto* proxy = dyn_cast<CallOperation>(op)) {
				if (!isPotentiallyThrowing(*proxy)) {
					continue;
				}
				if (!proxy->isNautilusFunctionCall()) {
					return true;
				}
				if (lookupCanThrow(canThrow, resolveCallee(ir, *proxy))) {
					return true;
				}
			} else if (const auto* indirect = dyn_cast<IndirectCallOperation>(op)) {
				if (isPotentiallyThrowing(*indirect)) {
					return true;
				}
			}
		}
	}
	return false;
}

} // namespace

bool NoThrowInferencePass::apply(IRGraph& ir) {
	const auto& functions = ir.getFunctionOperations();

	// Optimistic fixed point over the Nautilus-to-Nautilus call graph: start
	// assuming nothing throws, and only flip a function to canThrow when a
	// direct throw source is found or a callee it depends on already
	// flipped. Monotonic over a lattice bounded by functions.size(), so this
	// always converges -- a pure recursive cycle with no external throw
	// source never flips and is proven noThrow as a whole.
	std::unordered_map<const FunctionOperation*, bool> canThrow;
	for (const auto* fn : functions) {
		if (fn != nullptr) {
			canThrow[fn] = false;
		}
	}

	bool changedThisRound = true;
	while (changedThisRound) {
		changedThisRound = false;
		for (const auto* fn : functions) {
			if (fn == nullptr || canThrow[fn]) {
				continue;
			}
			if (computeCanThrow(*fn, ir, canThrow)) {
				canThrow[fn] = true;
				changedThisRound = true;
			}
		}
	}

	// Downgrade every Nautilus-to-Nautilus call whose callee the fixed point
	// proved cannot throw.
	bool changed = false;
	for (auto* fn : functions) {
		if (fn == nullptr) {
			continue;
		}
		for (auto* block : fn->getBasicBlocks()) {
			for (auto* op : block->getOperations()) {
				auto* proxy = dyn_cast<CallOperation>(op);
				if (proxy == nullptr || !proxy->isNautilusFunctionCall() || !isPotentiallyThrowing(*proxy)) {
					continue;
				}
				if (!lookupCanThrow(canThrow, resolveCallee(ir, *proxy))) {
					proxy->markNoThrow();
					changed = true;
				}
			}
		}
	}
	return changed;
}

} // namespace nautilus::compiler::ir

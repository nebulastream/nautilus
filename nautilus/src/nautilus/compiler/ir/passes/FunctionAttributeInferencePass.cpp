#include "nautilus/compiler/ir/passes/FunctionAttributeInferencePass.hpp"
#include "nautilus/compiler/ir/operations/CallOperation.hpp"
#include "nautilus/compiler/ir/operations/FunctionOperation.hpp"
#include "nautilus/compiler/ir/operations/OperationProperties.hpp"
#include "nautilus/compiler/ir/passes/LoopInfo.hpp"
#include <unordered_map>

namespace nautilus::compiler::ir {

namespace {

/// What we know so far about one function. Starts at the pessimistic default
/// and is only ever relaxed by evidence from the body.
struct DerivedFacts {
	bool mayRead = false;
	bool mayWrite = false;
	bool willReturn = true;
	bool noUnwind = true;

	/// Everything an unanalysable callee could do.
	static DerivedFacts pessimistic() {
		return DerivedFacts {.mayRead = true, .mayWrite = true, .willReturn = false, .noUnwind = false};
	}

	void absorb(const DerivedFacts& callee) {
		mayRead |= callee.mayRead;
		mayWrite |= callee.mayWrite;
		willReturn &= callee.willReturn;
		noUnwind &= callee.noUnwind;
	}

	[[nodiscard]] FunctionAttributes toAttributes() const {
		FunctionAttributes attrs;
		if (mayRead && mayWrite) {
			attrs.modRefInfo = ModRefInfo::ModRef;
		} else if (mayWrite) {
			attrs.modRefInfo = ModRefInfo::Mod;
		} else if (mayRead) {
			attrs.modRefInfo = ModRefInfo::Ref;
		} else {
			attrs.modRefInfo = ModRefInfo::NoModRef;
		}
		attrs.willReturn = willReturn;
		attrs.noUnwind = noUnwind;
		return attrs;
	}
};

DerivedFacts fromAttributes(const FunctionAttributes& attrs) {
	const auto modRef = static_cast<uint8_t>(attrs.modRefInfo);
	return DerivedFacts {.mayRead = (modRef & static_cast<uint8_t>(ModRefInfo::Ref)) != 0,
	                     .mayWrite = (modRef & static_cast<uint8_t>(ModRefInfo::Mod)) != 0,
	                     .willReturn = attrs.willReturn,
	                     .noUnwind = attrs.noUnwind};
}

enum class VisitState : uint8_t { Unvisited, InProgress, Done };

/// Walks the call graph, deriving each internal function's facts from its body
/// and from the functions it calls. Callees are resolved depth-first; a
/// re-entry into a function still being analysed is a cycle, and everything in
/// it becomes pessimistic.
class Inference {
public:
	explicit Inference(IRGraph& ir) : ir_(ir) {
	}

	/// Facts for a call to @p id, combined with what @p call itself declared
	/// when the callee is native. The table's own attrs come from whichever
	/// call site first interned that address (FunctionTable::intern keeps
	/// only the first descriptor), so a second call site that annotated the
	/// same function more conservatively -- or more accurately -- would
	/// otherwise be invisible here. This mirrors the union/intersect
	/// OperationEffects::effectsOf already performs for the same case, so the
	/// two readings of one call agree.
	DerivedFacts factsForCall(const CallOperation& call) {
		const auto& table = ir_.getFunctionTable();
		const auto id = call.getCalleeId();
		if (!table.contains(id)) {
			return DerivedFacts::pessimistic();
		}
		if (table.get(id).getLinkage() != Linkage::Internal) {
			auto facts = fromAttributes(table.get(id).getAttributes());
			facts.absorb(fromAttributes(call.getFunctionAttributes()));
			return facts;
		}
		return factsFor(id);
	}

	DerivedFacts factsFor(FunctionId id) {
		const auto& table = ir_.getFunctionTable();
		if (!table.contains(id)) {
			return DerivedFacts::pessimistic();
		}
		const auto& target = table.get(id);
		if (target.getLinkage() != Linkage::Internal) {
			// Reached only when resolving a callee id with no call site at
			// hand (there is none today -- analyseBody always goes through
			// factsForCall -- but this keeps the fallback correct on its own
			// terms rather than leaving it silently unreachable).
			return fromAttributes(target.getAttributes());
		}

		auto& state = state_[id];
		if (state == VisitState::Done) {
			return facts_[id];
		}
		if (state == VisitState::InProgress) {
			// Recursion. Bail out pessimistically and remember that this
			// function is in a cycle, so the in-flight analysis above cannot
			// conclude anything better either.
			cyclic_.insert(id);
			return DerivedFacts::pessimistic();
		}

		auto* definition = target.getDefinition();
		if (definition == nullptr) {
			state = VisitState::Done;
			facts_[id] = DerivedFacts::pessimistic();
			return facts_[id];
		}

		state = VisitState::InProgress;
		auto facts = analyseBody(*definition);
		if (cyclic_.contains(id)) {
			facts = DerivedFacts::pessimistic();
		}
		state = VisitState::Done;
		facts_[id] = facts;
		return facts;
	}

private:
	DerivedFacts analyseBody(FunctionOperation& definition) {
		DerivedFacts facts;

		// A loop might not terminate. Proving otherwise needs trip-count
		// reasoning this pass deliberately does not attempt, so any loop
		// costs willReturn -- and with it, movability.
		if (!findNaturalLoops(definition).empty()) {
			facts.willReturn = false;
		}

		for (const auto* block : definition.getBasicBlocks()) {
			if (block == nullptr) {
				continue;
			}
			for (const auto* op : block->getOperations()) {
				if (op == nullptr) {
					continue;
				}
				const auto opType = op->getOperationType();

				if (opType == Operation::OperationType::CallOp) {
					facts.absorb(factsForCall(*cast<CallOperation>(op)));
					continue;
				}
				if (opType == Operation::OperationType::IndirectCallOp) {
					// The callee is a runtime value; nothing is knowable.
					facts.absorb(DerivedFacts::pessimistic());
					continue;
				}
				facts.mayRead |= mayReadMemory(opType);
				facts.mayWrite |= mayWriteMemory(opType);
			}
		}
		return facts;
	}

	IRGraph& ir_;
	std::unordered_map<FunctionId, VisitState> state_;
	std::unordered_map<FunctionId, DerivedFacts> facts_;
	std::unordered_set<FunctionId> cyclic_;
};

} // namespace

bool FunctionAttributeInferencePass::apply(IRGraph& ir) {
	Inference inference(ir);
	auto& table = ir.getFunctionTableMut();

	// Snapshot the ids first: factsFor() recurses through the table, and the
	// set of entries does not change, but iterating while resolving keeps the
	// intent clearer than interleaving.
	std::vector<FunctionId> internalIds;
	for (const auto& target : table.getTargets()) {
		if (target.getLinkage() == Linkage::Internal && target.getDefinition() != nullptr) {
			internalIds.push_back(target.getId());
		}
	}

	for (const auto id : internalIds) {
		table.getMut(id).setDerivedAttributes(inference.factsFor(id).toAttributes());
	}

	// Analysis only: the graph is untouched, so report no change and let a
	// fixed-point group converge.
	return false;
}

} // namespace nautilus::compiler::ir

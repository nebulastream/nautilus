

#include "nautilus/compiler/ir/passes/ExceptionRegionPreparationPass.hpp"
#include "nautilus/common/FunctionAttributes.hpp"
#include "nautilus/compiler/ir/operations/FunctionOperation.hpp"
#include "nautilus/compiler/ir/operations/IndirectCallOperation.hpp"
#include "nautilus/compiler/ir/operations/ProxyCallOperation.hpp"
#include <algorithm>
#include <functional>
#include <map>
#include <span>
#include <string>
#include <vector>

namespace nautilus::compiler::ir {

namespace {

/// A backend-agnostic view of one destructor. ProxyCallOperation::Destructor
/// and IndirectCallOperation::Destructor are distinct nested types; both are
/// flattened into this so a single pad-interner can treat them uniformly.
struct DestructorSpec {
	Operation* address;
	std::string functionSymbol;
	std::string functionName;
	void* functionPtr;
};

/// A raw (uninterned) call site: the call operation plus its ordered
/// destructor list in construction order (must run in reverse on unwind).
struct RawCallSite {
	Operation* call;
	std::vector<DestructorSpec> destructors;
};

std::vector<DestructorSpec> convertDestructors(const std::vector<ProxyCallOperation::Destructor>& in) {
	std::vector<DestructorSpec> out;
	out.reserve(in.size());
	for (const auto& d : in) {
		out.push_back({d.address, d.functionSymbol, d.functionName, d.functionPtr});
	}
	return out;
}

std::vector<DestructorSpec> convertDestructors(const std::vector<IndirectCallOperation::Destructor>& in) {
	std::vector<DestructorSpec> out;
	out.reserve(in.size());
	for (const auto& d : in) {
		out.push_back({d.address, d.functionSymbol, d.functionName, d.functionPtr});
	}
	return out;
}

/// Total-order comparison for the pad-identity key (an ordered sequence of
/// address operands). Uses std::less so unrelated Operation* compare with a
/// strict total order rather than the built-in `<`.
struct VecLess {
	bool operator()(const std::vector<Operation*>& a, const std::vector<Operation*>& b) const {
		return std::lexicographical_compare(a.begin(), a.end(), b.begin(), b.end(), std::less<Operation*> {});
	}
};

bool applyToFunction(FunctionOperation& fn, common::Arena& arena) {
	if (fn.exceptionRegion.has_value()) {
		return false;
	}

	// 1. Collect every potentially-throwing call in the main CFG.
	std::vector<RawCallSite> rawSites;
	for (auto* block : fn.getBasicBlocks()) {
		for (auto* op : block->getOperations()) {
			if (auto* proxy = dyn_cast<ProxyCallOperation>(op)) {
				if (proxy->requiresExceptionHandling()) {
					rawSites.push_back({op, convertDestructors(proxy->getDestructors())});
				} else if (!proxy->getFunctionAttributes().noUnwind) {
					rawSites.push_back({op, {}});
				}
			} else if (auto* indirect = dyn_cast<IndirectCallOperation>(op)) {
				if (indirect->requiresExceptionHandling()) {
					rawSites.push_back({op, convertDestructors(indirect->getDestructors())});
				} else if (!indirect->getFunctionAttributes().noUnwind) {
					rawSites.push_back({op, {}});
				}
			}
		}
	}

	if (rawSites.empty()) {
		fn.exceptionRegion = FunctionExceptionRegion {};
		return false;
	}

	// 2. Intern pads by the ordered sequence of destructor address operands.
	// Two calls sharing the same ordered set of address operands share a pad.
	std::map<std::vector<Operation*>, size_t, VecLess> padIndex;

	// Fresh-identifier baseline: never collide with the main CFG's op/block
	// ids. The two identifier namespaces are distinct, so track each.
	uint32_t maxOpId = 0;
	uint32_t maxBlockId = 0;
	for (auto* block : fn.getBasicBlocks()) {
		maxBlockId = std::max(maxBlockId, block->getIdentifier().getId());
		for (auto* op : block->getOperations()) {
			maxOpId = std::max(maxOpId, op->getIdentifier().getId());
		}
	}
	uint32_t nextOpId = maxOpId + 1;
	uint32_t nextBlockId = maxBlockId + 1;

	FunctionExceptionRegion region;
	// Upper bound on distinct pads: never reallocates, so `pad` pointers into
	// `region.pads` stay stable while we intern.
	region.pads.reserve(rawSites.size());

	for (auto& site : rawSites) {
		if (site.destructors.empty()) {
			region.callSites.push_back({site.call, nullptr});
			continue;
		}

		std::vector<Operation*> identity;
		identity.reserve(site.destructors.size());
		for (const auto& d : site.destructors) {
			identity.push_back(d.address);
		}

		auto [it, inserted] = padIndex.try_emplace(identity, region.pads.size());
		const size_t index = it->second;
		if (inserted) {
			auto* padBlock =
			    arena.create<BasicBlock>(arena, BlockIdentifier {nextBlockId++}, std::vector<BasicBlockArgument*> {});
			// Destructors run in reverse construction order.
			for (auto dit = site.destructors.rbegin(); dit != site.destructors.rend(); ++dit) {
				FunctionAttributes dtorAttrs;
				dtorAttrs.noUnwind = true;
				std::vector<Operation*> args {dit->address};
				padBlock->addOperation<ProxyCallOperation>(dit->functionSymbol, dit->functionName, dit->functionPtr,
				                                           OperationIdentifier {nextOpId++},
				                                           std::span<Operation* const>(args), Type::v, dtorAttrs,
				                                           std::vector<ProxyCallOperation::Destructor> {}, false);
			}
			region.pads.push_back({padBlock});
		}
		region.callSites.push_back({site.call, &region.pads[index]});
	}

	fn.exceptionRegion = std::move(region);
	return true;
}

} // namespace

bool ExceptionRegionPreparationPass::apply(IRGraph& ir) {
	common::Arena& arena = ir.getArena();
	bool changed = false;
	for (auto* fn : ir.getFunctionOperations()) {
		if (fn != nullptr) {
			changed |= applyToFunction(*fn, arena);
		}
	}
	return changed;
}

} // namespace nautilus::compiler::ir

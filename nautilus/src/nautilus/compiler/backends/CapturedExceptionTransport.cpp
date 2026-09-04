#include "nautilus/compiler/backends/CapturedExceptionTransport.hpp"
#include "nautilus/compiler/ir/IRGraph.hpp"
#include "nautilus/compiler/ir/operations/CallOperation.hpp"
#include "nautilus/compiler/ir/operations/IndirectCallOperation.hpp"

namespace nautilus::compiler {

CapturedExceptionTransport::CapturedExceptionTransport(const ir::FunctionOperation& fn) {
	if (!fn.exceptionRegion) {
		return;
	}
	region_ = &*fn.exceptionRegion;
	siteIndex_.reserve(region_->callSites.size());
	for (size_t i = 0; i < region_->callSites.size(); ++i) {
		siteIndex_.emplace(region_->callSites[i].call, i);
	}
}

bool CapturedExceptionTransport::callNeedsCapture(const ir::Operation* call) const {
	return siteIndex_.contains(call);
}

size_t CapturedExceptionTransport::getPadIndexForCall(const ir::Operation* call) const {
	const auto it = siteIndex_.find(call);
	if (it == siteIndex_.end()) {
		return ir::noLandingPad;
	}
	return region_->callSites[it->second].padIndex;
}

const ir::LandingPadBlock* CapturedExceptionTransport::getPadForCall(const ir::Operation* call) const {
	const auto padIndex = getPadIndexForCall(call);
	return padIndex == ir::noLandingPad ? nullptr : &region_->pads[padIndex];
}

void* CapturedExceptionTransport::captureThunkFor(const ir::Operation* call) {
	if (const auto* proxy = ir::dyn_cast<ir::CallOperation>(call)) {
		return proxy->getCaptureFunc();
	}
	if (const auto* indirect = ir::dyn_cast<ir::IndirectCallOperation>(call)) {
		return indirect->getCaptureFunc();
	}
	return nullptr;
}

std::unordered_set<std::string> CapturedExceptionTransport::functionsNeedingCapture(const ir::IRGraph& ir) {
	std::unordered_set<std::string> names;
	for (const auto* fn : ir.getFunctionOperations()) {
		if (fn != nullptr && CapturedExceptionTransport(*fn).hasExceptionalCallSites()) {
			names.insert(fn->getName());
		}
	}
	return names;
}

bool CapturedExceptionTransport::anyFunctionNeedsCapture(const ir::IRGraph& ir) {
	for (const auto* fn : ir.getFunctionOperations()) {
		if (fn != nullptr && CapturedExceptionTransport(*fn).hasExceptionalCallSites()) {
			return true;
		}
	}
	return false;
}

} // namespace nautilus::compiler

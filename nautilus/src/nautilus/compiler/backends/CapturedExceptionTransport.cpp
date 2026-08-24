#include "nautilus/compiler/backends/CapturedExceptionTransport.hpp"

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

} // namespace nautilus::compiler

#include "nautilus/compiler/backends/CapturedExceptionTransport.hpp"

namespace nautilus::compiler {

CapturedExceptionTransport::CapturedExceptionTransport(const ir::FunctionOperation& fn) {
	if (fn.exceptionRegion) {
		region_ = &*fn.exceptionRegion;
	}
}

bool CapturedExceptionTransport::callNeedsCapture(const ir::Operation* call) const {
	return findCallSite(call) != nullptr;
}

const ir::LandingPadBlock* CapturedExceptionTransport::getPadForCall(const ir::Operation* call) const {
	const auto* callSite = findCallSite(call);
	return callSite != nullptr ? callSite->pad : nullptr;
}

const ir::ExceptionalCallSite* CapturedExceptionTransport::findCallSite(const ir::Operation* call) const {
	if (region_ == nullptr) {
		return nullptr;
	}
	for (const auto& callSite : region_->callSites) {
		if (callSite.call == call) {
			return &callSite;
		}
	}
	return nullptr;
}

} // namespace nautilus::compiler

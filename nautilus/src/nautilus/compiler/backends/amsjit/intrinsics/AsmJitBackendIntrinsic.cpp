
#include "nautilus/compiler/backends/amsjit/intrinsics/AsmJitBackendIntrinsic.hpp"

namespace nautilus::compiler::asmjit {

void AsmJitIntrinsicManager::addIntrinsic(IntrinsicTarget target, AsmJitIntrinsicFunction function) {
	intrinsicMap[target] = std::move(function);
}

std::optional<AsmJitIntrinsicFunction> AsmJitIntrinsicManager::getIntrinsic(IntrinsicTarget target) const {
	auto it = intrinsicMap.find(target);
	if (it != intrinsicMap.end()) {
		return it->second;
	}
	return std::nullopt;
}

void AsmJitIntrinsicPluginRegistry::addPlugin(std::shared_ptr<AsmJitIntrinsicPlugin> plugin) {
	std::lock_guard lock(mutex_);
	plugins_.push_back(std::move(plugin));
}

void AsmJitIntrinsicPluginRegistry::registerAllIntrinsics(AsmJitIntrinsicManager& manager) const {
	std::lock_guard lock(mutex_);
	for (const auto& p : plugins_) {
		if (p)
			p->registerIntrinsics(manager);
	}
}

AsmJitIntrinsicPluginRegistry& AsmJitIntrinsicPluginRegistry::instance() {
	static AsmJitIntrinsicPluginRegistry registry;
	return registry;
}

} // namespace nautilus::compiler::asmjit

#include "nautilus/compiler/backends/mlir/intrinsics/MLIRBackendIntrinsic.hpp"

namespace nautilus::compiler::mlir {

void MLIRIntrinsicManager::addIntrinsic(IntrinsicTarget target, IntrinsicFunction function) {
	intrinsicMap[target] = function;
}

std::optional<IntrinsicFunction> MLIRIntrinsicManager::getIntrinsic(IntrinsicTarget target) {
	auto it = intrinsicMap.find(target);
	if (it != intrinsicMap.end()) {
		return it->second;
	}
	return std::nullopt;
}

void MLIRIntrinsicPluginRegistry::addPlugin(std::shared_ptr<MLIRIntrinsicPlugin> plugin) {
	std::lock_guard lock(mutex_);
	plugins_.push_back(std::move(plugin));
}

void MLIRIntrinsicPluginRegistry::registerAllIntrinsics(MLIRIntrinsicManager& manager) const {
	std::lock_guard lock(mutex_);
	for (const auto& p : plugins_) {
		if (p)
			p->registerIntrinsics(manager);
	}
}

MLIRIntrinsicPluginRegistry& MLIRIntrinsicPluginRegistry::instance() {
	static MLIRIntrinsicPluginRegistry registry;
	return registry;
}

} // namespace nautilus::compiler::mlir

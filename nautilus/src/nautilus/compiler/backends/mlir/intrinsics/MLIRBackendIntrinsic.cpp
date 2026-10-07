#include "nautilus/compiler/backends/mlir/intrinsics/MLIRBackendIntrinsic.hpp"
#include "nautilus/common/ExecutableImage.hpp"

namespace nautilus::compiler::mlir {

void MLIRIntrinsicManager::addIntrinsic(IntrinsicTarget target, IntrinsicFunction function, std::string_view name) {
	// Interning here is what gives the intrinsic an identity in the IR: the
	// function table stamps this id onto the callee's entry when it is
	// interned, so by the time lowering runs the linkage is already decided
	// and there is no address left to match against.
	const auto id = ir::IntrinsicRegistry::instance().registerIntrinsic(target, name);
	intrinsicMap[id] = std::move(function);
}

std::optional<IntrinsicFunction> MLIRIntrinsicManager::getIntrinsic(ir::IntrinsicId id) const {
	if (id == ir::IntrinsicId::None) {
		return std::nullopt;
	}
	auto it = intrinsicMap.find(id);
	if (it != intrinsicMap.end()) {
		return it->second;
	}
	return std::nullopt;
}

void MLIRIntrinsicManager::merge(const MLIRIntrinsicManager& other) {
	for (const auto& [id, function] : other.intrinsicMap) {
		intrinsicMap[id] = function;
	}
}

std::optional<std::string> MLIRIntrinsicPlugin::cacheFingerprint() const {
	return std::nullopt;
}

bool MLIRIntrinsicPlugin::supportsArtifacts() const {
	return false;
}

std::optional<std::string> MLIRIntrinsicPlugin::cacheFingerprintForAddress(const void* address) {
	const auto image = common::locateExecutableAddress(address);
	if (!image) {
		return std::nullopt;
	}
	return image->buildId + ":" + std::to_string(image->loadOffset);
}

void MLIRIntrinsicPluginRegistry::addPlugin(std::shared_ptr<MLIRIntrinsicPlugin> plugin) {
	std::lock_guard lock(mutex_);
	try {
		if (plugin) {
			plugin->registerIntrinsics(harvested_);
		}
		plugins_.push_back(std::move(plugin));
	} catch (...) {
		registrationFailed_ = true;
		throw;
	}
}

void MLIRIntrinsicPluginRegistry::registerAllIntrinsics(MLIRIntrinsicManager& manager) const {
	std::lock_guard lock(mutex_);
	manager.merge(harvested_);
}

std::optional<std::string> MLIRIntrinsicPluginRegistry::cacheFingerprint() const {
	std::lock_guard lock(mutex_);
	if (registrationFailed_) {
		return std::nullopt;
	}
	std::string fingerprint;
	for (const auto& plugin : plugins_) {
		if (!plugin) {
			continue;
		}
		std::optional<std::string> identity;
		try {
			identity = plugin->cacheFingerprint();
		} catch (...) {
			return std::nullopt;
		}
		if (!identity || identity->empty()) {
			return std::nullopt;
		}
		fingerprint += std::to_string(identity->size()) + ":" + *identity;
	}
	return fingerprint;
}

std::optional<std::string> MLIRIntrinsicPluginRegistry::artifactFingerprint() const {
	std::lock_guard lock(mutex_);
	if (registrationFailed_) {
		return std::nullopt;
	}
	std::string fingerprint;
	for (const auto& plugin : plugins_) {
		if (!plugin) {
			continue;
		}
		try {
			const auto identity = plugin->cacheFingerprint();
			if (!plugin->supportsArtifacts() || !identity || identity->empty()) {
				return std::nullopt;
			}
			fingerprint += std::to_string(identity->size()) + ":" + *identity;
		} catch (...) {
			return std::nullopt;
		}
	}
	return fingerprint;
}

MLIRIntrinsicPluginRegistry& MLIRIntrinsicPluginRegistry::instance() {
	static MLIRIntrinsicPluginRegistry registry;
	return registry;
}

} // namespace nautilus::compiler::mlir

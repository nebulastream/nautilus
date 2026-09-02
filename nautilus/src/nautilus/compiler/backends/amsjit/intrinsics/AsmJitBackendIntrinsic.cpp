#include "nautilus/compiler/backends/amsjit/intrinsics/AsmJitBackendIntrinsic.hpp"

namespace nautilus::compiler::asmjit {

void AsmJitIntrinsicManager::addIntrinsic(IntrinsicTarget target, AsmJitIntrinsicFunction function,
                                          std::string_view name) {
	// Interning here is what gives the intrinsic an identity in the IR: the
	// function table stamps this id onto the callee's entry when it is
	// interned, so by the time lowering runs the linkage is already decided
	// and there is no address left to match against.
	const auto id = ir::IntrinsicRegistry::instance().registerIntrinsic(target, name);
	intrinsicMap[id] = std::move(function);
}

std::optional<AsmJitIntrinsicFunction> AsmJitIntrinsicManager::getIntrinsic(ir::IntrinsicId id) const {
	if (id == ir::IntrinsicId::None) {
		return std::nullopt;
	}
	auto it = intrinsicMap.find(id);
	if (it != intrinsicMap.end()) {
		return it->second;
	}
	return std::nullopt;
}

void AsmJitIntrinsicManager::merge(const AsmJitIntrinsicManager& other) {
	for (const auto& [id, function] : other.intrinsicMap) {
		intrinsicMap[id] = function;
	}
}

void AsmJitIntrinsicPluginRegistry::addPlugin(std::shared_ptr<AsmJitIntrinsicPlugin> plugin) {
	std::lock_guard lock(mutex_);
	if (plugin) {
		// Harvest now rather than at compile time. Interning an intrinsic is
		// what makes a call to it read as Intrinsic in the IR, and that
		// happens during trace-to-IR conversion -- so the identities have to
		// exist by then, not merely by the time a backend lowers.
		plugin->registerIntrinsics(harvested_);
	}
	plugins_.push_back(std::move(plugin));
}

void AsmJitIntrinsicPluginRegistry::registerAllIntrinsics(AsmJitIntrinsicManager& manager) const {
	std::lock_guard lock(mutex_);
	manager.merge(harvested_);
}

AsmJitIntrinsicPluginRegistry& AsmJitIntrinsicPluginRegistry::instance() {
	static AsmJitIntrinsicPluginRegistry registry;
	return registry;
}

} // namespace nautilus::compiler::asmjit

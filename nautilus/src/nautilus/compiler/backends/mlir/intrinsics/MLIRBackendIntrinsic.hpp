
#pragma once
#include "nautilus/compiler/backends/mlir/MLIRLoweringProvider.hpp"
#include <fcntl.h>
#include <functional>
#include <unordered_map>

namespace mlir {
class OpBuilder;
}

namespace nautilus::compiler::mlir {

using IntrinsicTarget = void*;
using IntrinsicFunction = std::function<bool(std::unique_ptr<::mlir::OpBuilder>&, const ir::ProxyCallOperation*,
                                             MLIRLoweringProvider::ValueFrame&)>;

class MLIRIntrinsicManager {
public:
	void addIntrinsic(IntrinsicTarget target, IntrinsicFunction function);
	std::optional<IntrinsicFunction> getIntrinsic(IntrinsicTarget target);

private:
	std::unordered_map<IntrinsicTarget, IntrinsicFunction> intrinsicMap;
};

class MLIRIntrinsicPlugin {
public:
	virtual void registerIntrinsics(MLIRIntrinsicManager& manager) = 0;
	virtual ~MLIRIntrinsicPlugin() = default;
};

class MLIRIntrinsicPluginRegistry {
public:
	// register a plugin instance (takes ownership)
	void addPlugin(std::shared_ptr<MLIRIntrinsicPlugin> plugin);

	// register all plugins into a manager
	void registerAllIntrinsics(MLIRIntrinsicManager& manager) const;

	// global accessor (thread-safe init)
	static MLIRIntrinsicPluginRegistry& instance();

private:
	MLIRIntrinsicPluginRegistry() = default;
	mutable std::mutex mutex_;
	std::vector<std::shared_ptr<MLIRIntrinsicPlugin>> plugins_;
};

} // namespace nautilus::compiler::mlir

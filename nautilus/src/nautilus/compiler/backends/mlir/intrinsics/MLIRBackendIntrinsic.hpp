
#pragma once
#include "nautilus/compiler/backends/mlir/MLIRLoweringProvider.hpp"
#include "nautilus/compiler/ir/IntrinsicRegistry.hpp"
#include <fcntl.h>
#include <functional>
#include <string_view>
#include <unordered_map>

namespace mlir {
class OpBuilder;
}

namespace nautilus::compiler::mlir {

/// A plugin still names an intrinsic by the address of its native
/// implementation -- that address is the intrinsic's identity *and* the
/// fallback every backend without a handler emits, so there is nothing else it
/// could sensibly be named by.
using IntrinsicTarget = void*;
using IntrinsicFunction = std::function<bool(std::unique_ptr<::mlir::OpBuilder>&, const ir::CallOperation*,
                                             MLIRLoweringProvider::ValueFrame&)>;

/**
 * @brief The MLIR backend's intrinsic handlers, keyed by IntrinsicId.
 *
 * Registration takes an address and lookup takes an id.  The asymmetry is the
 * point: the address is what a plugin author knows, and the id is what the IR
 * carries, so the address is resolved to an id exactly once -- here, at
 * registration -- instead of at every call site the backend lowers.
 */
class MLIRIntrinsicManager {
public:
	/// Registers @p function as the expansion of the intrinsic implemented by
	/// @p target, interning @p target in the process-wide IntrinsicRegistry.
	/// @p name is optional and only ever used in diagnostics.
	void addIntrinsic(IntrinsicTarget target, IntrinsicFunction function, std::string_view name = {});

	/// The handler for @p id, or nullopt when this backend does not expand it
	/// -- in which case the caller emits the ordinary call to its address.
	std::optional<IntrinsicFunction> getIntrinsic(ir::IntrinsicId id) const;

	/// Copies every handler in @p other into this manager, overwriting on
	/// collision -- the last plugin to claim an intrinsic wins, which is the
	/// behaviour registerAllIntrinsics had when it re-ran each plugin in turn.
	void merge(const MLIRIntrinsicManager& other);

private:
	std::unordered_map<ir::IntrinsicId, IntrinsicFunction> intrinsicMap;
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
	/// Handlers harvested as each plugin is added, so an intrinsic's identity
	/// exists in the IntrinsicRegistry from the moment its plugin is
	/// registered -- which is before any graph that calls it is converted.
	/// registerAllIntrinsics then copies from here instead of re-running every
	/// plugin's registration for each compile.
	MLIRIntrinsicManager harvested_;
};

} // namespace nautilus::compiler::mlir

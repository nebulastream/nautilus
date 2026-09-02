
#pragma once

#include "nautilus/compiler/backends/amsjit/AsmJitRegister.hpp"
#include "nautilus/compiler/ir/IntrinsicRegistry.hpp"
#include <functional>
#include <memory>
#include <mutex>
#include <optional>
#include <string_view>
#include <unordered_map>
#include <vector>

// Forward-declare: CallOperation.hpp lacks include guards (a project-wide
// convention — see the existing include sites). We only need a pointer-to-it
// here, so the forward declaration is enough.
namespace nautilus::compiler::ir {
class CallOperation;
} // namespace nautilus::compiler::ir

namespace nautilus::compiler::asmjit {

/// A plugin names an intrinsic by the raw `void*` address of its native
/// implementation — same convention as the MLIR intrinsic plugin. That address
/// is both the intrinsic's identity and the fallback a backend without a
/// handler emits, which is why registration is by address while lookup, below,
/// is by the IntrinsicId the IR carries.
using IntrinsicTarget = void*;

/// Context handed to intrinsic handlers. Holds the AsmJit Compiler and the
/// per-function RegisterFrame so a handler can:
///   1. Look up its operand registers via `frame.getValue(...)`
///   2. Emit instructions via `cc.<...>()`
///   3. Bind the result identifier via `frame.setValue(...)`
struct IntrinsicCallContext {
	AsmJitCompiler& cc;
	const ir::CallOperation* call;
	RegisterFrame& frame;
};

/// An intrinsic handler. Returns `true` if it fully handled the call and the
/// caller should skip the default Call lowering. Returning `false` lets
/// the caller fall back to the regular scalar function-call path (e.g. for a
/// width/op combo the handler decides not to specialise).
using AsmJitIntrinsicFunction = std::function<bool(IntrinsicCallContext&)>;

class AsmJitIntrinsicManager {
public:
	/// Registers @p function as the expansion of the intrinsic implemented by
	/// @p target, interning @p target in the process-wide IntrinsicRegistry.
	/// @p name is optional and only ever used in diagnostics.
	void addIntrinsic(IntrinsicTarget target, AsmJitIntrinsicFunction function, std::string_view name = {});

	/// The handler for @p id, or nullopt when this backend does not expand it
	/// — in which case the caller emits the ordinary call to its address.
	std::optional<AsmJitIntrinsicFunction> getIntrinsic(ir::IntrinsicId id) const;

	/// Copies every handler in @p other into this manager, overwriting on
	/// collision — the last plugin to claim an intrinsic wins.
	void merge(const AsmJitIntrinsicManager& other);

private:
	std::unordered_map<ir::IntrinsicId, AsmJitIntrinsicFunction> intrinsicMap;
};

class AsmJitIntrinsicPlugin {
public:
	virtual void registerIntrinsics(AsmJitIntrinsicManager& manager) = 0;
	virtual ~AsmJitIntrinsicPlugin() = default;
};

class AsmJitIntrinsicPluginRegistry {
public:
	/// Register a plugin instance. Thread-safe.
	void addPlugin(std::shared_ptr<AsmJitIntrinsicPlugin> plugin);

	/// Populate `manager` with the union of intrinsics from every registered plugin.
	void registerAllIntrinsics(AsmJitIntrinsicManager& manager) const;

	static AsmJitIntrinsicPluginRegistry& instance();

private:
	AsmJitIntrinsicPluginRegistry() = default;
	mutable std::mutex mutex_;
	std::vector<std::shared_ptr<AsmJitIntrinsicPlugin>> plugins_;
	/// Handlers harvested as each plugin is added, so an intrinsic's identity
	/// exists in the IntrinsicRegistry from the moment its plugin is
	/// registered — which is before any graph that calls it is converted.
	AsmJitIntrinsicManager harvested_;
};

} // namespace nautilus::compiler::asmjit

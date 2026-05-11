
#pragma once

#include "nautilus/compiler/backends/amsjit/AsmJitRegister.hpp"
#include <functional>
#include <memory>
#include <mutex>
#include <optional>
#include <unordered_map>
#include <vector>

// Forward-declare: ProxyCallOperation.hpp lacks include guards (a project-wide
// convention — see the existing include sites). We only need a pointer-to-it
// here, so the forward declaration is enough.
namespace nautilus::compiler::ir {
class ProxyCallOperation;
} // namespace nautilus::compiler::ir

namespace nautilus::compiler::asmjit {

/// Key identifying an intrinsic target. The plugin pattern keys on the raw
/// `void*` address of the implementation function the IR-level ProxyCallOperation
/// points at — same convention as the MLIR intrinsic plugin.
using IntrinsicTarget = void*;

/// Context handed to intrinsic handlers. Holds the AsmJit Compiler and the
/// per-function RegisterFrame so a handler can:
///   1. Look up its operand registers via `frame.getValue(...)`
///   2. Emit instructions via `cc.<...>()`
///   3. Bind the result identifier via `frame.setValue(...)`
struct IntrinsicCallContext {
	AsmJitCompiler& cc;
	const ir::ProxyCallOperation* call;
	RegisterFrame& frame;
};

/// An intrinsic handler. Returns `true` if it fully handled the call and the
/// caller should skip the default ProxyCall lowering. Returning `false` lets
/// the caller fall back to the regular scalar function-call path (e.g. for a
/// width/op combo the handler decides not to specialise).
using AsmJitIntrinsicFunction = std::function<bool(IntrinsicCallContext&)>;

class AsmJitIntrinsicManager {
public:
	void addIntrinsic(IntrinsicTarget target, AsmJitIntrinsicFunction function);
	std::optional<AsmJitIntrinsicFunction> getIntrinsic(IntrinsicTarget target) const;

private:
	std::unordered_map<IntrinsicTarget, AsmJitIntrinsicFunction> intrinsicMap;
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
};

} // namespace nautilus::compiler::asmjit

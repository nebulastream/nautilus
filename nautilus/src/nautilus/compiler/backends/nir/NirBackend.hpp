#pragma once

#include "nautilus/Executable.hpp"
#include "nautilus/common/ExceptionPropagation.hpp"
#include "nautilus/compiler/DumpHandler.hpp"
#include "nautilus/nir/Module.hpp"
#include "nautilus/options.hpp"
#include <atomic>
#include <cstdint>
#include <functional>
#include <map>
#include <memory>
#include <string>
#include <string_view>

namespace nautilus::compiler {
class CompilationStatistics;
}

// The backend interface of 0.4 (docs/plans/nautilus-0.4/05-backends.md, M4.0): a backend compiles a legalized SSA-level
// `nir::Module`. It lives next to the 0.3 `CompilationBackend` until the cut-over (M4.2), which replaces that
// interface with this one.
namespace nautilus::compiler::nirbackend {

/// Set by another thread to stop a compile that runs in the background (11-tiered-compilation.md §4.2). A backend
/// checks it at stage boundaries and returns null when it was cancelled.
class CancellationToken {
public:
	void cancel() noexcept {
		cancelled_.store(true, std::memory_order_relaxed);
	}
	[[nodiscard]] bool cancelled() const noexcept {
		return cancelled_.load(std::memory_order_relaxed);
	}

private:
	std::atomic<bool> cancelled_ {false};
};

/// What the host supplies for the parts of a module that are not in it (13 §4.7, §4.8).
struct HostEnvironment {
	/// Resolves a symbol of a `Portable` pointer (mode 1). Null if it is unknown.
	std::function<void*(std::string_view)> resolveSymbol;
	/// The address of runtime binding `index` of the module (13 §4.8). Null if the host supplies none.
	std::function<void*(const nir::Module&, uint32_t)> bindingAddress;
};

/// Everything a compile receives besides the module.
struct CompileContext {
	const DumpHandler& dumpHandler;
	const engine::Options& options;
	CompilationStatistics* statistics = nullptr;
	/// Null for a synchronous compile.
	const CancellationToken* cancellation = nullptr;
	HostEnvironment host {};
};

class NirBackend {
public:
	/// Compiles `module`, which must be a valid SSA-level module; the executable keeps the shared module alive. Returns
	/// null if `context.cancellation` was set.
	virtual std::unique_ptr<Executable> compile(std::shared_ptr<const nir::Module> module,
	                                            const CompileContext& context) const = 0;

	virtual ~NirBackend();
};

class NirBackendRegistry {
public:
	static NirBackendRegistry& instance();
	void registerBackend(const std::string& name, std::unique_ptr<NirBackend> backend);
	[[nodiscard]] const NirBackend* find(const std::string& name) const;

private:
	NirBackendRegistry();
	std::map<std::string, std::unique_ptr<NirBackend>> backends_;
};

/// The address a `pointers` entry denotes in this process (13 §4.7): the recorded address (mode 0) or the host's
/// resolution of the symbol (mode 1). Throws for an entry the host cannot resolve; build ids (mode 2) are M5's.
void* resolvePointer(const nir::Module& module, uint32_t pointerIndex, const HostEnvironment& host);

} // namespace nautilus::compiler::nirbackend

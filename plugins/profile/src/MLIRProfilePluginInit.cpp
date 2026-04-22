// Only compiled when ENABLE_MLIR_BACKEND is ON (see plugins/profile/CMakeLists.txt).
// Registers a jitCompiledFunctionObserver hook that forwards every JIT lookup
// into the profile plugin's JIT symbol table, so sample-based profiling can
// symbolicate frames that land inside JIT'd code.
#include "nautilus/compiler/backends/mlir/LLVMBackendHooks.hpp"
#include "nautilus/profile/JitSymbols.hpp"
#include <mutex>
#include <unordered_set>

namespace {

// MLIR ExecutionEngine does not expose a function's code size. For the
// purposes of sample symbolication the range only needs to dominate the
// emitted code, so we use a conservative default. Overlapping ranges are
// resolved by the most recent registration winning.
constexpr size_t kDefaultJitFunctionSize = 4 * 1024 * 1024;

// Idempotency: getInvocableFunctionPtr is invoked on every call site, not
// just once per compile. Track which (name, addr) pairs we've already
// registered so we don't rebuild the snapshot on every invocation.
std::mutex& seenMutex() {
	static std::mutex m;
	return m;
}
std::unordered_set<uintptr_t>& seenAddrs() {
	static std::unordered_set<uintptr_t> s;
	return s;
}

struct ProfileMLIRHookRegistrar {
	ProfileMLIRHookRegistrar() {
		nautilus::compiler::mlir::getLLVMBackendHooks().jitCompiledFunctionObserver = [](const std::string& name,
		                                                                                 void* addr) {
			if (addr == nullptr) {
				return;
			}
			const auto ip = reinterpret_cast<uintptr_t>(addr);
			{
				std::lock_guard<std::mutex> lock(seenMutex());
				if (!seenAddrs().insert(ip).second) {
					return;
				}
			}
			nautilus::profile::JitSymbol sym {};
			sym.addr = ip;
			sym.size = kDefaultJitFunctionSize;
			sym.name = name;
			sym.module = "mlir";
			nautilus::profile::registerJitCode(std::move(sym));
		};
	}
};

static ProfileMLIRHookRegistrar registrar_;

} // namespace

namespace nautilus::profile {
// Forces this TU to be linked when the profile plugin is a static library.
// Tests and users that want JIT-symbol registration on MLIR can call this
// once; it's a no-op after the static initializer above has run.
void ensureMLIRBackendHookInstalled() {
	(void) &registrar_;
}
} // namespace nautilus::profile

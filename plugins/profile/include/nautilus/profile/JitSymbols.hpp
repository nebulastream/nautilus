#pragma once

#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>

namespace nautilus::profile {

// A single entry in the JIT symbol registry. Address ranges describe code
// emitted by a backend that would otherwise look anonymous to a sampling
// profiler (MLIR/AsmJIT JIT'd functions). `name` and `module` must have
// static storage or be owned by the caller for the lifetime of the
// registration.
struct JitSymbol {
	uintptr_t addr;
	size_t size;
	std::string name;
	std::string module;
};

// Registers a JIT code region. Safe to call concurrently with lookups; the
// registry maintains an immutable snapshot and swaps it atomically.
void registerJitCode(JitSymbol sym);

// Removes a previously-registered region by its starting address. Safe to
// call concurrently with lookups.
void unregisterJitCode(uintptr_t addr);

// Returns the registered symbol whose range covers `ip`, or nullopt if
// none matches. Lookup is lock-free on the hot path.
std::optional<JitSymbol> lookupJitSymbol(uintptr_t ip);

// Writes a Linux-style perf map (`/tmp/perf-<pid>.map`) describing every
// currently-registered JIT symbol. The file format is one line per symbol:
// `<hex-addr> <hex-size> <name>`. Returns the path written or empty string
// on error.
std::string writePerfMap();

// Discards every registered symbol. Useful for tests.
void clearJitSymbols();

// When the profile plugin is linked as a static archive, the MLIR
// backend-observer translation unit may be dropped because nothing else
// references it. Call this function once (e.g. at program start) to force
// the linker to keep the TU and activate JIT-symbol registration for
// MLIR-compiled functions. No-op if ENABLE_MLIR_BACKEND was off.
void ensureMLIRBackendHookInstalled();

} // namespace nautilus::profile

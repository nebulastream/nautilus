#pragma once

#include "nautilus/compiler/JitSymbolRegistry.hpp"
#include "nautilus/profiling/sample_report.hpp"
#include <cstdint>
#include <memory>
#include <string>

namespace perf::util {
class SymbolResolver;
}

namespace nautilus::profiling::detail {

/// Resolves a sampled instruction pointer to a name, consulting nautilus's JIT
/// symbol registry before falling back to ELF symbols.
///
/// The order is what matters. perf-cpp's `util::SymbolResolver` builds its map
/// from `/proc/self/maps` plus each module's ELF symbol table, which is exactly
/// right for host code and structurally blind to JIT code: nautilus's generated
/// functions live in anonymous memory backed by no file, so no module covers
/// them and no symbol table names them. Its `_modules` map is private and it
/// exposes no way to add a symbol, so the JIT side cannot be taught to it --
/// hence a layer in front rather than a patch to the vendored copy.
///
/// A JIT address therefore resolves here, from the registry the MLIR backend
/// published to, and everything else falls through to perf-cpp unchanged.
class SymbolResolver {
public:
	SymbolResolver();
	~SymbolResolver();

	SymbolResolver(const SymbolResolver&) = delete;
	SymbolResolver& operator=(const SymbolResolver&) = delete;

	/// What one instruction pointer resolved to.
	struct Resolution {
		std::string name;
		SymbolOrigin origin = SymbolOrigin::Unresolved;
		/// The compile the address belongs to, or NO_MODULE for host code.
		/// Two modules in one process both produce a symbol called `execute`,
		/// so this is what keeps them apart in a report.
		compiler::ModuleIndex moduleIndex = compiler::NO_MODULE;
	};

	/// Names @p address, reporting where the name came from. Never fails: an
	/// address nothing claims comes back as `SymbolOrigin::Unresolved` with a
	/// hex placeholder, so it is still visible in a report as the share of the
	/// profile that could not be attributed.
	[[nodiscard]] Resolution resolve(uintptr_t address);

private:
	/// Constructed lazily on the first host-code lookup: it parses
	/// `/proc/self/maps` and every mapped ELF's symbol table, which is far too
	/// expensive to pay for a profile that turns out to be entirely JIT code.
	::perf::util::SymbolResolver& elfResolver();

	struct Impl;
	std::unique_ptr<Impl> impl_;
};

} // namespace nautilus::profiling::detail

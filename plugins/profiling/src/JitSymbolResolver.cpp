#include "JitSymbolResolver.hpp"
#include "nautilus/compiler/JitSymbolRegistry.hpp"
#include <cstdio>
#include <exception>
#include <memory>
#include <optional>
#include <perfcpp/util/symbol_resolver.hpp>
#include <utility>

namespace nautilus::profiling::detail {

struct SymbolResolver::Impl {
	std::optional<::perf::util::SymbolResolver> elf;
	/// Set once the ELF resolver has been tried; a construction failure must not
	/// be retried per sample.
	bool elfTried = false;
};

SymbolResolver::SymbolResolver() : impl_(std::make_unique<Impl>()) {
}

SymbolResolver::~SymbolResolver() = default;

::perf::util::SymbolResolver& SymbolResolver::elfResolver() {
	if (!impl_->elfTried) {
		impl_->elfTried = true;
		impl_->elf.emplace();
	}
	return *impl_->elf;
}

SymbolResolver::Resolution SymbolResolver::resolve(uintptr_t address) {
	// JIT first. A generated code range can only be named from the registry,
	// and the registry can only contain generated code, so there is no overlap
	// to arbitrate between -- the order is about cost, not precedence: a
	// registry hit is a binary search over a handful of ranges, while the ELF
	// path parses every mapped module's symbol table on first use.
	compiler::JitSymbol jitSymbol;
	if (compiler::JitSymbolRegistry::instance().resolve(address, jitSymbol)) {
		return {std::move(jitSymbol.name), SymbolOrigin::Jit, jitSymbol.moduleIndex};
	}

	try {
		if (const auto resolved = elfResolver().resolve(address); resolved.has_value()) {
			return {resolved->symbol().name(), SymbolOrigin::Host, compiler::NO_MODULE};
		}
	} catch (const std::exception&) {
		// A malformed or unreadable module leaves the address unresolved rather
		// than failing the whole report.
	}

	// Kept as an address rather than dropped: an unresolved share is
	// information -- usually the kernel, or a JIT compile that ran without
	// `perf.sample` -- and silently discarding it would make the remaining
	// percentages add up to something misleading.
	char buffer[32];
	std::snprintf(buffer, sizeof(buffer), "0x%llx", static_cast<unsigned long long>(address));
	return {buffer, SymbolOrigin::Unresolved, compiler::NO_MODULE};
}

} // namespace nautilus::profiling::detail

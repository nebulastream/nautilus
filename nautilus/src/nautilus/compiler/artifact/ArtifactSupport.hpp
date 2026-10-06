#pragma once

#include "nautilus/Artifact.hpp"
#include <string_view>

namespace nautilus::compiler::mlir {
struct MLIRCacheArtifacts;
}

namespace nautilus::artifact::detail {

struct ResolvedImports {
	std::vector<std::string> symbols;
	std::vector<void*> addresses;
	std::vector<std::string> auxiliarySymbols;
	std::vector<void*> auxiliaryAddresses;
};

NativeImage imageAt(const void* address);
Compatibility currentCompatibility(const engine::Options& options);
std::vector<std::string> exportNames(const Descriptor& descriptor);
void createImports(Descriptor& descriptor, const compiler::mlir::MLIRCacheArtifacts& artifacts,
                   std::string_view object);
ResolvedImports resolveImports(const Descriptor& descriptor, bool bytecode);

} // namespace nautilus::artifact::detail

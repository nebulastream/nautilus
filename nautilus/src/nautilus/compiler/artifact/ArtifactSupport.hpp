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
void createImports(Descriptor& descriptor, const compiler::mlir::MLIRCacheArtifacts& artifacts, std::string_view object,
                   const engine::Options& options = {});
ResolvedImports resolveImports(const Descriptor& descriptor, bool bytecode, const engine::Options& options = {});

} // namespace nautilus::artifact::detail

#pragma once

#include "nautilus/Artifact.hpp"
#include <mlir/IR/BuiltinOps.h>
#include <string>
#include <string_view>
#include <vector>

namespace nautilus::compiler::mlir {

struct MLIRObjectSymbols {
	std::vector<std::string> undefinedSymbols;
	std::vector<std::string> definedFunctionSymbols;
};

void validateArtifactMLIRModule(::mlir::ModuleOp module, const std::vector<std::string>& exportNames,
                                const std::vector<std::string>& externalSymbols,
                                const std::vector<void*>& externalAddresses);

std::vector<std::string> validateArtifactExportABI(::mlir::ModuleOp module,
                                                   const std::vector<::nautilus::artifact::ExportDescriptor>& exports);
MLIRObjectSymbols inspectArtifactObject(std::string_view object);

void validateArtifactObjectSymbols(std::string_view object, const std::vector<std::string>& exportNames,
                                   const std::vector<std::string>& externalSymbols,
                                   const std::vector<void*>& externalAddresses);

} // namespace nautilus::compiler::mlir

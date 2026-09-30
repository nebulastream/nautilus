#pragma once

#include "nautilus/options.hpp"
#include <mlir/IR/BuiltinOps.h>
#include <string>
#include <vector>

namespace nautilus::compiler::mlir {

void validateCachedMLIRModule(::mlir::ModuleOp module, const std::vector<std::string>& exportNames,
                              const std::vector<std::string>& externalSymbols,
                              const std::vector<void*>& externalAddresses, const engine::Options& options);

}

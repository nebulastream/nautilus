#include "nautilus/compiler/backends/mlir/MLIRCacheValidation.hpp"
#include "nautilus/exceptions/RuntimeException.hpp"
#include <mlir/Dialect/LLVMIR/LLVMDialect.h>
#include <unordered_map>
#include <unordered_set>

namespace nautilus::compiler::mlir {
namespace {

void validateExports(::mlir::ModuleOp module, const std::vector<std::string>& exportNames) {
	if (exportNames.empty()) {
		throw RuntimeException("Cached MLIR module has no exports");
	}
	std::unordered_set<std::string> names;
	for (const auto& name : exportNames) {
		if (name.empty() || !names.insert(name).second) {
			throw RuntimeException("Cached MLIR export manifest is invalid");
		}
		auto function = module.lookupSymbol<::mlir::LLVM::LLVMFuncOp>(name);
		if (!function || function->getRegion(0).empty()) {
			throw RuntimeException("Cached MLIR module is missing export '" + name + "'");
		}
	}
}

std::unordered_map<std::string, const runtime_binding::Entry*> validateRuntimeBindings(::mlir::ModuleOp module,
                                                                                       const engine::Options& options) {
	const auto& bindings = options.getRuntimeBindings();
	auto schema = module->getAttrOfType<::mlir::StringAttr>("nautilus.runtime_binding.schema");
	if (!schema || schema.getValue() != bindings.schema()) {
		throw RuntimeException("Cached MLIR runtime binding schema mismatch");
	}
	std::unordered_map<std::string, const runtime_binding::Entry*> symbols;
	for (const auto& [identity, entry] : bindings.entries()) {
		if (!entry || entry->identity != identity || entry->identity.empty() || entry->type.empty() ||
		    entry->symbol.empty() || entry->address == nullptr || !symbols.emplace(entry->symbol, entry.get()).second) {
			throw RuntimeException("MLIR runtime binding environment is invalid");
		}
	}
	for (auto function : module.getOps<::mlir::LLVM::LLVMFuncOp>()) {
		if (symbols.contains(function.getSymName().str())) {
			throw RuntimeException("Cached MLIR runtime binding symbol is declared as a function");
		}
	}
	for (auto global : module.getOps<::mlir::LLVM::GlobalOp>()) {
		auto identity = global->getAttrOfType<::mlir::StringAttr>("nautilus.runtime_binding.identity");
		auto type = global->getAttrOfType<::mlir::StringAttr>("nautilus.runtime_binding.type");
		auto binding = symbols.find(global.getSymName().str());
		if (binding == symbols.end()) {
			if (identity || type || (!global.getValueOrNull() && global.getInitializerRegion().empty())) {
				throw RuntimeException("Cached MLIR module contains an undeclared runtime binding");
			}
			continue;
		}
		if (!identity || !type || identity.getValue() != binding->second->identity ||
		    type.getValue() != binding->second->type || !global.getType().isInteger(8) || global.getConstant() ||
		    global.getLinkage() != ::mlir::LLVM::Linkage::External || global.getValueOrNull() ||
		    !global.getInitializerRegion().empty() || global.getThreadLocal_() || global.getAddrSpace() != 0 ||
		    global.getAlignment().value_or(1) != 1 || global.getUnnamedAddrAttr() || global.getDsoLocal() ||
		    global.getExternallyInitialized() || global.getComdatAttr() || global.getSectionAttr() ||
		    global.getVisibility_() != ::mlir::LLVM::Visibility::Default) {
			throw RuntimeException("Cached MLIR runtime binding declaration mismatch");
		}
	}
	return symbols;
}

void validateExternalSymbols(::mlir::ModuleOp module, const std::vector<std::string>& externalSymbols,
                             const std::vector<void*>& externalAddresses, const engine::Options& options) {
	if (externalSymbols.size() != externalAddresses.size()) {
		throw RuntimeException("Cached MLIR external symbol vectors differ in size");
	}
	const auto bindings = validateRuntimeBindings(module, options);
	std::unordered_set<std::string> allSymbols;
	std::unordered_set<std::string> expectedFunctions;
	for (std::size_t index = 0; index < externalSymbols.size(); ++index) {
		if (externalSymbols[index].empty() || externalAddresses[index] == nullptr ||
		    !allSymbols.insert(externalSymbols[index]).second) {
			throw RuntimeException("Cached MLIR external symbol manifest is invalid");
		}
		if (auto binding = bindings.find(externalSymbols[index]); binding != bindings.end()) {
			if (externalAddresses[index] != binding->second->address) {
				throw RuntimeException("Cached MLIR runtime binding address does not match the load environment");
			}
		} else {
			expectedFunctions.insert(externalSymbols[index]);
		}
	}
	for (auto function : module.getOps<::mlir::LLVM::LLVMFuncOp>()) {
		if (function->getRegion(0).empty() && !expectedFunctions.erase(function.getSymName().str())) {
			throw RuntimeException("Cached MLIR module contains an undeclared external function");
		}
	}
	if (!expectedFunctions.empty()) {
		throw RuntimeException("Cached MLIR external function is not declared by the module");
	}
	for (auto global : module.getOps<::mlir::LLVM::GlobalOp>()) {
		if (bindings.contains(global.getSymName().str()) && !allSymbols.contains(global.getSymName().str())) {
			throw RuntimeException("Cached MLIR runtime binding address is missing");
		}
	}
}

} // namespace

void validateCachedMLIRModule(::mlir::ModuleOp module, const std::vector<std::string>& exportNames,
                              const std::vector<std::string>& externalSymbols,
                              const std::vector<void*>& externalAddresses, const engine::Options& options) {
	validateExports(module, exportNames);
	validateExternalSymbols(module, externalSymbols, externalAddresses, options);
}

} // namespace nautilus::compiler::mlir

#include "nautilus/compiler/backends/mlir/MLIRArtifactValidation.hpp"
#include "nautilus/exceptions/RuntimeException.hpp"
#include <algorithm>
#include <llvm/BinaryFormat/ELF.h>
#include <llvm/Object/ELFObjectFile.h>
#include <llvm/Object/ObjectFile.h>
#include <llvm/Support/Error.h>
#include <llvm/Support/MemoryBufferRef.h>
#include <llvm/Support/raw_ostream.h>
#include <mlir/Dialect/LLVMIR/LLVMDialect.h>
#include <unordered_map>
#include <unordered_set>

namespace nautilus::compiler::mlir {
namespace {

bool validSymbolName(const std::string& name) {
	return !name.empty() && name.find('\0') == std::string::npos;
}

void validateExportNames(const std::vector<std::string>& exportNames) {
	if (exportNames.empty()) {
		throw RuntimeException("MLIR artifact has no exports");
	}
	std::unordered_set<std::string> names;
	for (const auto& name : exportNames) {
		if (!validSymbolName(name) || !names.insert(name).second) {
			throw RuntimeException("MLIR artifact export manifest is invalid");
		}
	}
}

std::unordered_set<std::string> validateExternalNames(const std::vector<std::string>& externalSymbols,
                                                      const std::vector<void*>& externalAddresses) {
	if (externalSymbols.size() != externalAddresses.size()) {
		throw RuntimeException("MLIR artifact external symbol vectors differ in size");
	}
	std::unordered_set<std::string> names;
	for (std::size_t index = 0; index < externalSymbols.size(); ++index) {
		if (!validSymbolName(externalSymbols[index]) || externalAddresses[index] == nullptr ||
		    !names.insert(externalSymbols[index]).second) {
			throw RuntimeException("MLIR artifact external symbol manifest is invalid");
		}
	}
	return names;
}

std::unordered_map<std::string, const runtime_binding::Entry*> bindingEnvironment(const engine::Options& options) {
	std::unordered_map<std::string, const runtime_binding::Entry*> symbols;
	for (const auto& [identity, entry] : options.getRuntimeBindings().entries()) {
		if (!entry || entry->identity != identity || identity.empty() || entry->type.empty() ||
		    entry->symbol != runtime_binding::symbolName(identity) || entry->address == nullptr ||
		    !symbols.emplace(entry->symbol, entry.get()).second) {
			throw RuntimeException("MLIR runtime binding environment is invalid");
		}
	}
	return symbols;
}

std::unordered_map<std::string, const runtime_binding::Entry*> validateRuntimeBindings(::mlir::ModuleOp module,
                                                                                       const engine::Options& options) {
	const auto& bindings = options.getRuntimeBindings();
	const auto schema = module->getAttrOfType<::mlir::StringAttr>("nautilus.runtime_binding.schema");
	if ((schema && schema.getValue() != bindings.schema()) || (!schema && !bindings.entries().empty())) {
		throw RuntimeException("MLIR artifact runtime binding schema mismatch");
	}
	const auto symbols = bindingEnvironment(options);
	for (auto function : module.getOps<::mlir::LLVM::LLVMFuncOp>()) {
		if (symbols.contains(function.getSymName().str())) {
			throw RuntimeException("MLIR artifact runtime binding symbol is declared as a function");
		}
	}
	for (auto global : module.getOps<::mlir::LLVM::GlobalOp>()) {
		const auto identity = global->getAttrOfType<::mlir::StringAttr>("nautilus.runtime_binding.identity");
		const auto type = global->getAttrOfType<::mlir::StringAttr>("nautilus.runtime_binding.type");
		const auto binding = symbols.find(global.getSymName().str());
		if (binding == symbols.end()) {
			if (identity || type) {
				throw RuntimeException("MLIR artifact contains an undeclared runtime binding");
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
			throw RuntimeException("MLIR artifact runtime binding declaration mismatch");
		}
	}
	return symbols;
}

} // namespace

void validateArtifactMLIRModule(::mlir::ModuleOp module, const std::vector<std::string>& exportNames,
                                const std::vector<std::string>& externalSymbols,
                                const std::vector<void*>& externalAddresses, const engine::Options& options) {
	if (!module) {
		throw RuntimeException("MLIR artifact module is missing");
	}
	validateExportNames(exportNames);
	for (const auto& name : exportNames) {
		auto function = module.lookupSymbol<::mlir::LLVM::LLVMFuncOp>(name);
		if (!function || function->getRegion(0).empty()) {
			throw RuntimeException("MLIR artifact module is missing export '" + name + "'");
		}
	}

	if (externalSymbols.size() != externalAddresses.size()) {
		throw RuntimeException("MLIR artifact external symbol vectors differ in size");
	}
	const auto bindings = validateRuntimeBindings(module, options);
	auto expectedFunctions = validateExternalNames(externalSymbols, externalAddresses);
	for (std::size_t index = 0; index < externalSymbols.size(); ++index) {
		if (const auto binding = bindings.find(externalSymbols[index]); binding != bindings.end()) {
			if (externalAddresses[index] != binding->second->address) {
				throw RuntimeException("MLIR artifact runtime binding address does not match the load environment");
			}
			expectedFunctions.erase(externalSymbols[index]);
		}
	}
	for (auto function : module.getOps<::mlir::LLVM::LLVMFuncOp>()) {
		if (function->getRegion(0).empty() && !expectedFunctions.erase(function.getSymName().str())) {
			throw RuntimeException("MLIR artifact contains an undeclared external function");
		}
	}
	if (!expectedFunctions.empty()) {
		throw RuntimeException("MLIR artifact external function is not declared by the module");
	}
	if (!module.getOps<::mlir::LLVM::GlobalCtorsOp>().empty() ||
	    !module.getOps<::mlir::LLVM::GlobalDtorsOp>().empty()) {
		throw RuntimeException("MLIR artifacts do not support module initializers or finalizers");
	}
	for (auto global : module.getOps<::mlir::LLVM::GlobalOp>()) {
		if (const auto section = global.getSection();
		    section &&
		    (section->starts_with(".init_array") || section->starts_with(".preinit_array") ||
		     section->starts_with(".fini_array") || section->starts_with(".ctors") || section->starts_with(".dtors"))) {
			throw RuntimeException("MLIR artifacts do not support module initializers or finalizers");
		}
		if (bindings.contains(global.getSymName().str())) {
			if (std::ranges::find(externalSymbols, global.getSymName().str()) == externalSymbols.end()) {
				throw RuntimeException("MLIR artifact runtime binding address is missing");
			}
		} else if (!global.getValueOrNull() && global.getInitializerRegion().empty()) {
			throw RuntimeException("MLIR artifacts do not support external globals");
		}
	}
	module.walk([&](::mlir::LLVM::IntToPtrOp pointer) {
		auto immediate = pointer.getArg().getDefiningOp<::mlir::LLVM::ConstantOp>();
		if (immediate) {
			const auto integer = llvm::dyn_cast<::mlir::IntegerAttr>(immediate.getValue());
			if (integer && integer.getValue().isZero()) {
				return;
			}
		}
		std::unordered_set<::mlir::Operation*> visited;
		std::function<bool(::mlir::Value)> runtimeDerived = [&](::mlir::Value value) {
			if (llvm::isa<::mlir::BlockArgument>(value)) {
				return true;
			}
			auto* operation = value.getDefiningOp();
			if (operation == nullptr || !visited.insert(operation).second) {
				return false;
			}
			if (llvm::isa<::mlir::LLVM::LoadOp, ::mlir::LLVM::CallOp, ::mlir::LLVM::InvokeOp, ::mlir::LLVM::AddressOfOp,
			              ::mlir::LLVM::AllocaOp>(operation)) {
				return true;
			}
			for (const auto operand : operation->getOperands()) {
				if (runtimeDerived(operand)) {
					return true;
				}
			}
			return false;
		};
		if (!runtimeDerived(pointer.getArg())) {
			throw RuntimeException("MLIR artifacts do not support immediate non-null pointers");
		}
	});
}

std::vector<std::string> validateArtifactExportABI(::mlir::ModuleOp module,
                                                   const std::vector<::nautilus::artifact::ExportDescriptor>& exports) {
	const auto matches = [](Type stamp, ::mlir::Type type) {
		switch (stamp) {
		case Type::v:
			return llvm::isa<::mlir::LLVM::LLVMVoidType>(type);
		case Type::ptr: {
			const auto pointer = llvm::dyn_cast<::mlir::LLVM::LLVMPointerType>(type);
			return pointer && pointer.getAddressSpace() == 0;
		}
		case Type::f32:
			return type.isF32();
		case Type::f64:
			return type.isF64();
		case Type::b:
			return type.isInteger(1);
		case Type::i8:
		case Type::ui8:
			return type.isInteger(8);
		case Type::i16:
		case Type::ui16:
			return type.isInteger(16);
		case Type::i32:
		case Type::ui32:
			return type.isInteger(32);
		case Type::i64:
		case Type::ui64:
			return type.isInteger(64);
		}
		return false;
	};
	const auto validateAttributes = [](Type stamp, llvm::ArrayRef<::mlir::NamedAttribute> attributes,
	                                   bool requireNarrowExtension) {
		const bool signedInteger = stamp == Type::i8 || stamp == Type::i16 || stamp == Type::i32 || stamp == Type::i64;
		const bool unsignedInteger =
		    stamp == Type::b || stamp == Type::ui8 || stamp == Type::ui16 || stamp == Type::ui32 || stamp == Type::ui64;
		bool extension = false;
		for (const auto& attribute : attributes) {
			const auto name = attribute.getName().strref();
			if (!llvm::isa<::mlir::UnitAttr>(attribute.getValue()) ||
			    !((name == "llvm.signext" && signedInteger) || (name == "llvm.zeroext" && unsignedInteger))) {
				throw RuntimeException("MLIR artifact export has unsupported C ABI attributes");
			}
			extension = true;
		}
		if (requireNarrowExtension &&
		    (stamp == Type::b || stamp == Type::i8 || stamp == Type::ui8 || stamp == Type::i16 ||
		     stamp == Type::ui16) &&
		    !extension) {
			throw RuntimeException("MLIR artifact export is missing a narrow C ABI extension");
		}
	};
	std::vector<std::string> result;
	for (const auto& entry : exports) {
		auto function = module.lookupSymbol<::mlir::LLVM::LLVMFuncOp>(entry.name);
		if (!function || function.getBody().empty()) {
			throw RuntimeException("MLIR artifact has no defined export '" + entry.name + "'");
		}
		const auto type = function.getFunctionType();
		if (type.isVarArg() || type.getParams().size() != entry.argumentTypes.size() ||
		    !matches(entry.returnType, type.getReturnType()) ||
		    static_cast<uint32_t>(function.getCConv()) != entry.callingConvention) {
			throw RuntimeException("MLIR artifact export signature mismatch: " + entry.name);
		}
		for (std::size_t index = 0; index < entry.argumentTypes.size(); ++index) {
			if (!matches(entry.argumentTypes[index], type.getParams()[index])) {
				throw RuntimeException("MLIR artifact export argument signature mismatch: " + entry.name);
			}
		}
		for (std::size_t index = 0; index < entry.argumentTypes.size(); ++index) {
			validateAttributes(entry.argumentTypes[index],
			                   ::mlir::cast<::mlir::FunctionOpInterface>(function.getOperation())
			                       .getArgAttrs(static_cast<unsigned>(index)),
			                   true);
		}
		if (entry.returnType != Type::v) {
			validateAttributes(entry.returnType, function.getResultAttrs(0), true);
		}
		std::string abi;
		llvm::raw_string_ostream output(abi);
		static_cast<::mlir::Type>(type).print(output);
		output << " convention=" << static_cast<uint32_t>(function.getCConv());
		std::vector<::mlir::NamedAttribute> attributes(function->getAttrs().begin(), function->getAttrs().end());
		std::ranges::sort(attributes, {}, [](const auto& attribute) { return attribute.getName().strref(); });
		for (const auto& attribute : attributes) {
			output << " " << attribute.getName().strref() << "=";
			attribute.getValue().print(output);
		}
		output.flush();
		if (!entry.loweredABI.empty() && entry.loweredABI != abi) {
			throw RuntimeException("MLIR artifact export ABI attributes mismatch: " + entry.name);
		}
		result.push_back(std::move(abi));
	}
	return result;
}

MLIRObjectSymbols inspectArtifactObject(std::string_view object, const engine::Options& options) {
	const auto bindings = bindingEnvironment(options);
	if (object.empty()) {
		throw RuntimeException("MLIR artifact object is empty");
	}
	auto parsed = llvm::object::ObjectFile::createObjectFile(
	    llvm::MemoryBufferRef(llvm::StringRef(object.data(), object.size()), "artifact-module.o"));
	if (!parsed) {
		throw RuntimeException("Could not inspect MLIR artifact object: " + llvm::toString(parsed.takeError()));
	}
	if (!(*parsed)->isELF() || !(*parsed)->isRelocatableObject()) {
		throw RuntimeException("MLIR artifacts require a relocatable ELF object");
	}
	if ((*parsed)->getArch() != llvm::Triple::x86_64 || !(*parsed)->isLittleEndian()) {
		throw RuntimeException("MLIR artifact object target is unsupported");
	}
	for (const auto& section : (*parsed)->sections()) {
		auto name = section.getName();
		if (!name) {
			throw RuntimeException("Could not read artifact object section: " + llvm::toString(name.takeError()));
		}
		const auto type = llvm::object::ELFSectionRef(section).getType();
		if (type == llvm::ELF::SHT_INIT_ARRAY || type == llvm::ELF::SHT_FINI_ARRAY ||
		    type == llvm::ELF::SHT_PREINIT_ARRAY || name->starts_with(".init_array") ||
		    name->starts_with(".preinit_array") || name->starts_with(".fini_array") || name->starts_with(".ctors") ||
		    name->starts_with(".dtors")) {
			throw RuntimeException("MLIR artifacts do not support native initializers or finalizers");
		}
	}
	MLIRObjectSymbols result;
	for (const auto& symbol : (*parsed)->symbols()) {
		auto name = symbol.getName();
		if (!name) {
			throw RuntimeException("Could not read MLIR artifact object symbol: " + llvm::toString(name.takeError()));
		}
		if (name->empty()) {
			continue;
		}
		if (name->contains('\0')) {
			throw RuntimeException("MLIR artifact object symbol is invalid");
		}
		auto flags = symbol.getFlags();
		if (!flags) {
			throw RuntimeException("Could not read MLIR artifact object symbol flags: " +
			                       llvm::toString(flags.takeError()));
		}
		if ((*flags & llvm::object::SymbolRef::SF_Global) && (*flags & llvm::object::SymbolRef::SF_Absolute)) {
			throw RuntimeException("MLIR artifacts do not support absolute native symbols");
		}
		auto type = symbol.getType();
		if (!type) {
			throw RuntimeException("Could not read MLIR artifact object symbol type: " +
			                       llvm::toString(type.takeError()));
		}
		if (*flags & llvm::object::SymbolRef::SF_Undefined) {
			if (*type != llvm::object::SymbolRef::ST_Function && *type != llvm::object::SymbolRef::ST_Unknown &&
			    !(*type == llvm::object::SymbolRef::ST_Data && bindings.contains(name->str()))) {
				throw RuntimeException("MLIR artifacts do not support external globals");
			}
			if (*type == llvm::object::SymbolRef::ST_Function && bindings.contains(name->str())) {
				throw RuntimeException("MLIR artifact runtime binding symbol is declared as a function");
			}
			result.undefinedSymbols.push_back(name->str());
		} else if (*flags & llvm::object::SymbolRef::SF_Global) {
			result.definedSymbols.push_back(name->str());
			if (*type == llvm::object::SymbolRef::ST_Function) {
				result.definedFunctionSymbols.push_back(name->str());
			}
		}
	}
	std::ranges::sort(result.undefinedSymbols);
	result.undefinedSymbols.erase(std::unique(result.undefinedSymbols.begin(), result.undefinedSymbols.end()),
	                              result.undefinedSymbols.end());
	std::ranges::sort(result.definedFunctionSymbols);
	result.definedFunctionSymbols.erase(
	    std::unique(result.definedFunctionSymbols.begin(), result.definedFunctionSymbols.end()),
	    result.definedFunctionSymbols.end());
	return result;
}

void validateArtifactObjectSymbols(std::string_view object, const std::vector<std::string>& exportNames,
                                   const std::vector<std::string>& externalSymbols,
                                   const std::vector<void*>& externalAddresses, const engine::Options& options) {
	validateExportNames(exportNames);
	const auto imports = validateExternalNames(externalSymbols, externalAddresses);
	const auto bindings = bindingEnvironment(options);
	for (std::size_t index = 0; index < externalSymbols.size(); ++index) {
		if (const auto binding = bindings.find(externalSymbols[index]);
		    binding != bindings.end() && externalAddresses[index] != binding->second->address) {
			throw RuntimeException("MLIR artifact runtime binding address does not match the load environment");
		}
	}
	const auto symbols = inspectArtifactObject(object, options);
	const std::unordered_set<std::string> functions(symbols.definedFunctionSymbols.begin(),
	                                                symbols.definedFunctionSymbols.end());
	for (const auto& name : exportNames) {
		if (!functions.contains(name) || imports.contains(name)) {
			throw RuntimeException("MLIR artifact object is missing defined export '" + name + "'");
		}
	}
	for (const auto& name : symbols.undefinedSymbols) {
		if (!imports.contains(name)) {
			throw RuntimeException("MLIR artifact object contains undeclared import '" + name + "'");
		}
	}
	const std::unordered_set<std::string> defined(symbols.definedSymbols.begin(), symbols.definedSymbols.end());
	for (const auto& name : externalSymbols) {
		if (defined.contains(name)) {
			throw RuntimeException("MLIR artifact import conflicts with a defined symbol '" + name + "'");
		}
	}
}

} // namespace nautilus::compiler::mlir

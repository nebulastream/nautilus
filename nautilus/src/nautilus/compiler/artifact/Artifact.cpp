#include "nautilus/Artifact.hpp"
#include "nautilus/exceptions/RuntimeException.hpp"

#ifdef ENABLE_MLIR_BACKEND
#include "nautilus/common/Arena.hpp"
#include "nautilus/common/ExecutableImage.hpp"
#include "nautilus/compiler/CompilationPipeline.hpp"
#include "nautilus/compiler/DumpHandler.hpp"
#include "nautilus/compiler/artifact/ArtifactCodec.hpp"
#include "nautilus/compiler/artifact/ArtifactPreflight.hpp"
#include "nautilus/compiler/artifact/ArtifactSupport.hpp"
#include "nautilus/compiler/backends/CompilationBackend.hpp"
#include "nautilus/compiler/backends/mlir/ExceptionPersonality.hpp"
#include "nautilus/compiler/backends/mlir/LLVMBackendHooks.hpp"
#include "nautilus/compiler/backends/mlir/MLIRArtifactValidation.hpp"
#include "nautilus/compiler/backends/mlir/MLIRCompilationBackend.hpp"
#include "nautilus/compiler/backends/mlir/intrinsics/MLIRBackendIntrinsic.hpp"
#include "nautilus/compiler/ir/IRGraph.hpp"
#include "nautilus/compiler/ir/passes/ExceptionRegionPreparationPass.hpp"
#include <algorithm>
#include <bit>
#include <llvm/Config/llvm-config.h>
#include <llvm/ExecutionEngine/Orc/JITTargetMachineBuilder.h>
#include <llvm/Support/Error.h>
#include <llvm/Target/TargetMachine.h>
#include <unordered_map>
#include <unordered_set>
#ifdef __linux__
#include <dlfcn.h>
#endif
#endif

namespace nautilus::artifact {
namespace {

[[maybe_unused, noreturn]] void unsupported() {
	throw RuntimeException("Module artifacts require the MLIR backend on Linux x86-64 with ELF build identities");
}

} // namespace

#ifdef ENABLE_MLIR_BACKEND
namespace detail {

NativeImage imageAt(const void* address) {
	const auto image = common::locateExecutableAddress(address);
	if (!image || common::resolveExecutableAddress(*image) != address) {
		throw RuntimeException("Artifact native image identity is missing or ambiguous");
	}
	return {image->buildId, image->loadOffset};
}

namespace {

void validateCapabilities(const engine::Options& options) {
#if !defined(__linux__) || !defined(__x86_64__)
	unsupported();
#endif
	const auto& hooks = compiler::mlir::getLLVMBackendHooks();
	if (hooks.preOptModuleTransform || hooks.jitSymbolContributor || hooks.callNameOverride) {
		throw RuntimeException("Module artifacts do not support active LLVM backend hooks");
	}
	if (options.getOptionOrDefault("engine.backend", std::string("mlir")) != "mlir" ||
	    !options.getOptionOrDefault("engine.Compilation", true) ||
	    options.getOptionOrDefault("mlir.inline_invoke_calls", false) ||
	    !options.getOptionOrDefault("mlir.targetCpu", std::string()).empty() ||
	    options.getOptionOrDefault("debug", false) || options.getOptionOrDefault("perf", false) ||
	    options.getOptionOrDefault("perf.sample", false)) {
		throw RuntimeException("Unsupported module artifact compilation options");
	}
}

} // namespace

Compatibility currentCompatibility(const engine::Options& options) {
	validateCapabilities(options);
	compiler::mlir::MLIRCompilationBackend backend;
	auto target = llvm::orc::JITTargetMachineBuilder::detectHost();
	if (!target) {
		throw RuntimeException("Could not detect artifact target: " + llvm::toString(target.takeError()));
	}
	target->setCodeGenOptLevel(llvm::CodeGenOptLevel::Aggressive);
	auto machine = target->createTargetMachine();
	if (!machine) {
		throw RuntimeException("Could not construct artifact target: " + llvm::toString(machine.takeError()));
	}
	const auto extensions = compiler::mlir::MLIRIntrinsicPluginRegistry::instance().artifactFingerprint();
	if (!extensions) {
		throw RuntimeException("Artifact intrinsic implementation identity is unavailable");
	}
	Compatibility value;
	value.compilerImage = imageAt(reinterpret_cast<const void*>(&compiler::CompilationBackendRegistry::getInstance));
	value.producerImage = imageAt(reinterpret_cast<const void*>(&emit));
	value.llvmVersion = LLVM_VERSION_STRING;
	value.targetTriple = target->getTargetTriple().str();
	value.cpu = target->getCPU();
	value.features = target->getFeatures().getString();
	value.dataLayout = (*machine)->createDataLayout().getStringRepresentation();
	value.optionsDigest = detail::optionsDigest(options);
	value.extensionFingerprint = *extensions;
	value.pointerSize = sizeof(void*);
	value.littleEndian = std::endian::native == std::endian::little;
	return value;
}

std::vector<std::string> exportNames(const Descriptor& descriptor) {
	std::vector<std::string> names;
	for (const auto& entry : descriptor.exports) {
		names.push_back(entry.name);
	}
	return names;
}

namespace {

void addImport(Descriptor& descriptor, std::string symbol, void* address, bool bytecodeImport) {
	if (symbol.empty() || symbol.find('\0') != std::string::npos || address == nullptr) {
		throw RuntimeException("Invalid artifact native import");
	}
	NativeImage image;
	try {
		image = imageAt(address);
	} catch (const RuntimeException& error) {
		throw RuntimeException("Artifact import '" + symbol + "': " + error.what());
	}
	const auto existing = std::ranges::find(descriptor.imports, symbol, &ImportDescriptor::symbol);
	if (existing != descriptor.imports.end()) {
		if (existing->image != image || existing->bytecodeImport != bytecodeImport) {
			throw RuntimeException("Conflicting artifact native import");
		}
		return;
	}
	descriptor.imports.push_back({std::move(symbol), image, bytecodeImport});
}

void* resolveHelper(const std::string& symbol) {
	if (symbol == "_Unwind_Resume") {
		return compiler::mlir::getUnwindResumeAddress();
	}
	static const std::unordered_set<std::string> helpers {"_Unwind_Resume",
	                                                      "__cxa_begin_catch",
	                                                      "__cxa_end_catch",
	                                                      "__cxa_rethrow",
	                                                      "__cxa_call_unexpected",
	                                                      "memcpy",
	                                                      "memmove",
	                                                      "memset",
	                                                      "memcmp",
	                                                      "bcmp",
	                                                      "sqrt",
	                                                      "sqrtf",
	                                                      "sin",
	                                                      "sinf",
	                                                      "cos",
	                                                      "cosf",
	                                                      "pow",
	                                                      "powf",
	                                                      "exp",
	                                                      "expf",
	                                                      "log",
	                                                      "logf",
	                                                      "log2",
	                                                      "log2f",
	                                                      "log10",
	                                                      "log10f",
	                                                      "fmod",
	                                                      "fmodf",
	                                                      "floor",
	                                                      "floorf",
	                                                      "ceil",
	                                                      "ceilf",
	                                                      "round",
	                                                      "roundf",
	                                                      "trunc",
	                                                      "truncf",
	                                                      "fma",
	                                                      "fmaf"};
	if (!helpers.contains(symbol)) {
		throw RuntimeException("Artifact object has an unsupported compiler-generated import: " + symbol);
	}
#ifdef __linux__
	auto* address = ::dlsym(RTLD_DEFAULT, symbol.c_str());
	if (address == nullptr) {
		throw RuntimeException("Artifact compiler-generated import is unresolved: " + symbol);
	}
	return address;
#else
	unsupported();
#endif
}

} // namespace

void createImports(Descriptor& descriptor, const compiler::mlir::MLIRCacheArtifacts& artifacts, std::string_view object,
                   const engine::Options& options) {
	if (descriptor.bindingSchema != options.getRuntimeBindings().schemaEntries()) {
		throw RuntimeException("Artifact runtime binding schema mismatch");
	}
	std::unordered_map<std::string, void*> bindings;
	for (const auto& [identity, entry] : options.getRuntimeBindings().entries()) {
		bindings.emplace(entry->symbol, entry->address);
	}
	if (artifacts.externalSymbols.size() != artifacts.externalAddresses.size()) {
		throw RuntimeException("Artifact external symbol vector mismatch");
	}
	for (std::size_t index = 0; index < artifacts.externalSymbols.size(); ++index) {
		const auto& symbol = artifacts.externalSymbols[index];
		if (const auto binding = bindings.find(symbol); binding != bindings.end()) {
			if (binding->second != artifacts.externalAddresses[index]) {
				throw RuntimeException("Artifact runtime binding address mismatch");
			}
		} else {
			addImport(descriptor, symbol, artifacts.externalAddresses[index], true);
		}
	}
	const auto symbols = compiler::mlir::inspectArtifactObject(object, options);
	for (const auto& symbol : symbols.undefinedSymbols) {
		if (!bindings.contains(symbol) &&
		    std::ranges::find(descriptor.imports, symbol, &ImportDescriptor::symbol) == descriptor.imports.end()) {
			addImport(descriptor, symbol, resolveHelper(symbol), false);
		}
	}
	std::ranges::sort(descriptor.imports, {}, &ImportDescriptor::symbol);
}

ResolvedImports resolveImports(const Descriptor& descriptor, bool bytecode, const engine::Options& options) {
	if (descriptor.bindingSchema != options.getRuntimeBindings().schemaEntries()) {
		throw RuntimeException("Artifact runtime binding schema mismatch");
	}
	ResolvedImports result;
	std::unordered_set<std::string> symbols;
	for (const auto& entry : descriptor.exports) {
		symbols.insert(entry.name);
	}
	for (const auto& entry : descriptor.imports) {
		if (!symbols.insert(entry.symbol).second) {
			throw RuntimeException("Artifact import symbol collision: " + entry.symbol);
		}
		const common::ExecutableImageLocation image {entry.image.buildId, entry.image.loadOffset};
		auto* address = common::resolveExecutableAddress(image);
		if (address == nullptr || imageAt(address) != entry.image) {
			throw RuntimeException("Artifact native import cannot be resolved uniquely: " + entry.symbol);
		}
		if (!bytecode || entry.bytecodeImport) {
			result.symbols.push_back(entry.symbol);
			result.addresses.push_back(address);
		} else {
			result.auxiliarySymbols.push_back(entry.symbol);
			result.auxiliaryAddresses.push_back(address);
		}
	}
	for (const auto& [identity, entry] : options.getRuntimeBindings().entries()) {
		if (!symbols.insert(entry->symbol).second || entry->address == nullptr) {
			throw RuntimeException("Artifact runtime binding symbol collision: " + entry->symbol);
		}
		result.symbols.push_back(entry->symbol);
		result.addresses.push_back(entry->address);
	}
	return result;
}

} // namespace detail

namespace {

void validateForLoad(const ModuleArtifact& value, const engine::Options& options, bool bytecode) {
	detail::validateDescriptor(value);
	if (value.descriptor.compatibility != detail::currentCompatibility(options)) {
		throw RuntimeException("Artifact compiler, target, options or extension compatibility mismatch");
	}
	if (bytecode) {
		detail::validatePayload(value.bytecode, value.descriptor.bytecodeDigest);
	} else {
		detail::validatePayload(value.object, value.descriptor.objectDigest);
	}
}

} // namespace
#endif

bool isSupported() {
#if defined(ENABLE_MLIR_BACKEND) && defined(__linux__) && defined(__x86_64__)
	try {
		detail::currentCompatibility({});
		return true;
	} catch (...) {
		return false;
	}
#else
	return false;
#endif
}

#ifdef ENABLE_TRACING
ModuleArtifact emit(std::list<compiler::CompilableFunction>& functions, const engine::ModuleOptions& options) {
#ifdef ENABLE_MLIR_BACKEND
	ModuleArtifact result;
	auto& descriptor = result.descriptor;
	descriptor.compatibility = detail::currentCompatibility(options);
	descriptor.bindingSchema = options.getRuntimeBindings().schemaEntries();
	std::unordered_set<std::string> names;
	for (const auto& function : functions) {
		if (function.getName().empty() || function.getName().find('\0') != std::string::npos ||
		    !names.insert(function.getName()).second || !function.getSignature()) {
			throw RuntimeException("Artifact exports require unique names and declared signatures");
		}
		ExportDescriptor entry;
		entry.name = function.getName();
		entry.returnType = function.getSignature()->returnType;
		entry.argumentTypes = function.getSignature()->argumentTypes;
		if (!function.getAttributes().empty()) {
			throw RuntimeException("Artifact exports do not support user-supplied function attributes");
		}
		entry.attributes.assign(function.getAttributes().begin(), function.getAttributes().end());
		std::ranges::sort(entry.attributes);
		descriptor.exports.push_back(std::move(entry));
	}
	if (functions.empty()) {
		throw RuntimeException("Artifact exports are empty");
	}
	std::ranges::sort(descriptor.exports, {}, &ExportDescriptor::name);
	common::ArenaPool tracePool;
	common::ArenaPool irPool;
	compiler::CompilationPipeline pipeline(options, tracePool, irPool);
	compiler::mlir::MLIRCompilationBackend backend;
	auto ir = pipeline.compileToIR(
	    functions, options, nullptr, backend.irOptimizationLevel(),
	    [&](compiler::ir::IRGraph& graph) { compiler::artifact::validateArtifactPreflight(graph, functions); },
	    ConstantOriginTracking::Enabled);
	if (detail::currentCompatibility(options) != descriptor.compatibility) {
		throw RuntimeException("Artifact implementation identity changed during tracing");
	}
	if (!options.getOptionOrDefault("ir.runPasses", true)) {
		compiler::ir::ExceptionRegionPreparationPass preparation;
		preparation.apply(*ir);
	}
	compiler::DumpHandler dump(options, ir->getId());
	compiler::mlir::MLIRCacheArtifacts raw;
	const auto objectPreflight = [&](std::string_view object) {
		detail::createImports(descriptor, raw, object, options);
		const auto resolved = detail::resolveImports(descriptor, false, options);
		compiler::mlir::validateArtifactObjectSymbols(object, detail::exportNames(descriptor), resolved.symbols,
		                                              resolved.addresses, options);
	};
	auto executable = backend.compileWithCacheArtifacts(ir, detail::exportNames(descriptor), dump, options, nullptr,
	                                                    raw, &descriptor.exports, objectPreflight);
	if (raw.object.empty() || raw.bytecode.empty() || raw.externalSymbols.size() != raw.externalAddresses.size() ||
	    raw.exportABIs.size() != descriptor.exports.size()) {
		throw RuntimeException("Artifact producer did not emit one complete native object and bytecode module");
	}
	for (std::size_t index = 0; index < descriptor.exports.size(); ++index) {
		descriptor.exports[index].loweredABI = raw.exportABIs[index];
	}
	detail::createImports(descriptor, raw, raw.object, options);
	const auto resolved = detail::resolveImports(descriptor, false, options);
	compiler::mlir::validateArtifactObjectSymbols(raw.object, detail::exportNames(descriptor), resolved.symbols,
	                                              resolved.addresses, options);
	if (detail::currentCompatibility(options) != descriptor.compatibility) {
		throw RuntimeException("Artifact implementation identity changed during code generation");
	}
	descriptor.moduleManifest = std::move(raw.moduleManifest);
	result.object = std::move(raw.object);
	result.bytecode = std::move(raw.bytecode);
	descriptor.objectDigest = detail::digest(result.object);
	descriptor.bytecodeDigest = detail::digest(result.bytecode);
	result.descriptorDigest = detail::digest(detail::encodeDescriptor(descriptor));
	detail::validateDescriptor(result);
	return result;
#else
	(void) functions;
	(void) options;
	unsupported();
#endif
}
#endif

engine::CompiledModule loadNative(const ModuleArtifact& value, const engine::Options& options) {
#ifdef ENABLE_MLIR_BACKEND
	validateForLoad(value, options, false);
	const auto imports = detail::resolveImports(value.descriptor, false, options);
	compiler::mlir::MLIRCompilationBackend backend;
	auto executable = backend.compileCachedObject(value.object, imports.symbols, imports.addresses,
	                                              detail::exportNames(value.descriptor), options, nullptr);
	return engine::CompiledModule(std::move(executable), {});
#else
	(void) value;
	(void) options;
	unsupported();
#endif
}
engine::CompiledModule loadBytecode(const ModuleArtifact& value, const engine::Options& options) {
#ifdef ENABLE_MLIR_BACKEND
	validateForLoad(value, options, true);
	const auto imports = detail::resolveImports(value.descriptor, true, options);
	compiler::DumpHandler dump(options, "artifact-bytecode");
	compiler::mlir::MLIRCompilationBackend backend;
	auto allSymbols = imports.symbols;
	auto allAddresses = imports.addresses;
	allSymbols.insert(allSymbols.end(), imports.auxiliarySymbols.begin(), imports.auxiliarySymbols.end());
	allAddresses.insert(allAddresses.end(), imports.auxiliaryAddresses.begin(), imports.auxiliaryAddresses.end());
	const auto objectPreflight = [names = detail::exportNames(value.descriptor), symbols = std::move(allSymbols),
	                              addresses = std::move(allAddresses), options](std::string_view object) {
		compiler::mlir::validateArtifactObjectSymbols(object, names, symbols, addresses, options);
	};
	auto executable = backend.compileCachedBytecode(
	    value.bytecode, value.descriptor.moduleManifest, imports.symbols, imports.addresses,
	    detail::exportNames(value.descriptor), dump, options, nullptr, nullptr, {}, imports.auxiliarySymbols,
	    imports.auxiliaryAddresses, &value.descriptor.exports, objectPreflight);
	return engine::CompiledModule(std::move(executable), {});
#else
	(void) value;
	(void) options;
	unsupported();
#endif
}

#ifndef ENABLE_MLIR_BACKEND
std::string encode(const ModuleArtifact&) {
	unsupported();
}
ModuleArtifact decode(std::string_view) {
	unsupported();
}
#endif

} // namespace nautilus::artifact

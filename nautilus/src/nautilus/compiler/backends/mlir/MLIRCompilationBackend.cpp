
#include "nautilus/compiler/backends/mlir/MLIRCompilationBackend.hpp"
#include "nautilus/CompilationStatistics.hpp"
#include "nautilus/compiler/backends/mlir/JITCompiler.hpp"
#include "nautilus/compiler/backends/mlir/LLVMIROptimizer.hpp"
#include "nautilus/compiler/backends/mlir/MLIRExecutable.hpp"
#include "nautilus/compiler/backends/mlir/MLIRLoweringProvider.hpp"
#include "nautilus/compiler/backends/mlir/MLIRPassManager.hpp"
#include "nautilus/compiler/backends/mlir/debug/DebugInfoOptions.hpp"
#include "nautilus/compiler/backends/mlir/intrinsics/MLIRBackendIntrinsic.hpp"
#include "nautilus/compiler/backends/mlir/intrinsics/MLIRMemoryIntrinsics.hpp"
#include "nautilus/compiler/ir/IRGraph.hpp"
#include "nautilus/compiler/ir/IRLocationMap.hpp"
#include "nautilus/compiler/ir/passes/IRLocationPass.hpp"
#include "nautilus/exceptions/RuntimeException.hpp"
#include <algorithm>
#include <chrono>
#include <fstream>
#include <llvm/Support/Error.h>
#include <llvm/Support/MemoryBuffer.h>
#include <llvm/Support/SourceMgr.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Support/raw_ostream.h>
#include <mlir/Bytecode/BytecodeWriter.h>
#include <mlir/Dialect/Arith/IR/Arith.h>
#include <mlir/Dialect/ControlFlow/IR/ControlFlow.h>
#include <mlir/Dialect/Func/Extensions/AllExtensions.h>
#include <mlir/Dialect/Func/IR/FuncOps.h>
#include <mlir/Dialect/LLVMIR/LLVMDialect.h>
#include <mlir/Dialect/LLVMIR/Transforms/InlinerInterfaceImpl.h>
#include <mlir/Dialect/Math/IR/Math.h>
#include <mlir/IR/MLIRContext.h>
#include <mlir/IR/Verifier.h>
#include <mlir/Parser/Parser.h>
#include <mlir/Target/LLVMIR/Dialect/All.h>
#include <mlir/Target/LLVMIR/Dialect/Builtin/BuiltinToLLVMIRTranslation.h>
#include <mlir/Target/LLVMIR/Dialect/LLVMIR/LLVMToLLVMIRTranslation.h>
#include <mlir/Transforms/Inliner.h>
#include <unordered_map>
#include <unordered_set>

namespace nautilus::compiler::mlir {
namespace {

::mlir::DialectRegistry createDialectRegistry() {
	::mlir::DialectRegistry registry;
	registry.insert<::mlir::arith::ArithDialect, ::mlir::cf::ControlFlowDialect, ::mlir::math::MathDialect,
	                ::mlir::LLVM::LLVMDialect, ::mlir::func::FuncDialect>();
	::mlir::func::registerAllExtensions(registry);
	registerBuiltinDialectTranslation(registry);
	registerLLVMDialectTranslation(registry);
	::mlir::LLVM::registerInlinerInterface(registry);
	return registry;
}

void appendField(std::string& output, std::string_view field) {
	output += std::to_string(field.size());
	output.push_back(':');
	output.append(field);
}

std::string printType(::mlir::Type type) {
	std::string result;
	llvm::raw_string_ostream output(result);
	type.print(output);
	output.flush();
	return result;
}

std::string printAttribute(::mlir::Attribute attribute) {
	std::string result;
	llvm::raw_string_ostream output(result);
	attribute.print(output);
	output.flush();
	return result;
}

std::string buildModuleManifest(::mlir::ModuleOp module) {
	std::string manifest = "nautilus.mlir.module";
	std::vector<::mlir::NamedAttribute> moduleAttributes(module->getAttrs().begin(), module->getAttrs().end());
	std::ranges::sort(moduleAttributes, {}, [](const auto& attribute) { return attribute.getName().strref(); });
	for (const auto& attribute : moduleAttributes) {
		appendField(manifest, attribute.getName().strref());
		appendField(manifest, printAttribute(attribute.getValue()));
	}

	std::vector<std::string> symbols;
	for (auto function : module.getOps<::mlir::LLVM::LLVMFuncOp>()) {
		std::string entry;
		appendField(entry, "function");
		appendField(entry, function.getSymName());
		appendField(entry, printType(function.getFunctionType()));
		appendField(entry, function->getRegion(0).empty() ? "external" : "defined");
		appendField(entry, std::to_string(static_cast<uint32_t>(function.getCConv())));
		appendField(entry, std::to_string(static_cast<uint32_t>(function.getLinkage())));
		std::vector<::mlir::NamedAttribute> attributes(function->getAttrs().begin(), function->getAttrs().end());
		std::ranges::sort(attributes, {}, [](const auto& attribute) { return attribute.getName().strref(); });
		for (const auto& attribute : attributes) {
			appendField(entry, attribute.getName().strref());
			appendField(entry, printAttribute(attribute.getValue()));
		}
		symbols.push_back(std::move(entry));
	}
	for (auto global : module.getOps<::mlir::LLVM::GlobalOp>()) {
		std::string entry;
		appendField(entry, "global");
		appendField(entry, global.getSymName());
		appendField(entry, printType(global.getType()));
		appendField(entry, !global.getValueOrNull() && global.getInitializerRegion().empty() ? "external" : "defined");
		std::vector<::mlir::NamedAttribute> attributes(global->getAttrs().begin(), global->getAttrs().end());
		std::ranges::sort(attributes, {}, [](const auto& attribute) { return attribute.getName().strref(); });
		for (const auto& attribute : attributes) {
			appendField(entry, attribute.getName().strref());
			appendField(entry, printAttribute(attribute.getValue()));
		}
		symbols.push_back(std::move(entry));
	}
	std::ranges::sort(symbols);
	for (const auto& symbol : symbols) {
		appendField(manifest, symbol);
	}
	return manifest;
}

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

llvm::CodeGenOptLevel getCodeGenLevel(const DebugInfoOptions& debugInfo) {
	return debugInfo.enableDebug ? llvm::CodeGenOptLevel::Less : llvm::CodeGenOptLevel::Aggressive;
}

void materializeExports(MLIRJit& jit, const std::vector<std::string>& exportNames) {
	for (const auto& name : exportNames) {
		auto result = jit.lookup(name);
		if (!result) {
			throw RuntimeException("Could not materialize MLIR export '" + name +
			                       "': " + llvm::toString(result.takeError()));
		}
	}
}

std::string serializeBytecode(::mlir::ModuleOp module) {
	std::string bytecode;
	llvm::raw_string_ostream output(bytecode);
	if (::mlir::failed(::mlir::writeBytecodeToFile(module.getOperation(), output))) {
		throw RuntimeException("Could not serialize MLIR bytecode");
	}
	output.flush();
	return bytecode;
}

void dumpMLIR(::mlir::ModuleOp module, const DumpHandler& dumpHandler) {
	dumpHandler.dump("after_mlir_generation", "mlir", [&]() {
		::mlir::OpPrintingFlags flags;
		std::string result;
		auto output = llvm::raw_string_ostream(result);
		module.print(output, flags);
		return result;
	});
}

} // namespace

MLIRCompilationBackend::MLIRCompilationBackend() {
	// Initialize information about the local machine in LLVM.
	llvm::InitializeNativeTarget();
	llvm::InitializeNativeTargetAsmPrinter();
	llvm::InitializeNativeTargetAsmParser();

	// Register default MLIR intrinsics
	RegisterMLIRMemoryIntrinsicPlugin();
}

std::unique_ptr<Executable> MLIRCompilationBackend::compile(const std::shared_ptr<ir::IRGraph>& ir,
                                                            const DumpHandler& dumpHandler,
                                                            const engine::Options& options,
                                                            CompilationStatistics* statistics) const {
	return compileIR(ir, nullptr, dumpHandler, options, statistics, nullptr);
}

std::unique_ptr<Executable> MLIRCompilationBackend::compileWithCacheArtifacts(
    const std::shared_ptr<ir::IRGraph>& ir, const std::vector<std::string>& exportNames, const DumpHandler& dumpHandler,
    const engine::Options& options, CompilationStatistics* statistics, MLIRCacheArtifacts& artifacts) const {
	return compileIR(ir, &exportNames, dumpHandler, options, statistics, &artifacts);
}

std::unique_ptr<Executable>
MLIRCompilationBackend::compileIR(const std::shared_ptr<ir::IRGraph>& ir, const std::vector<std::string>* exportNames,
                                  const DumpHandler& dumpHandler, const engine::Options& options,
                                  CompilationStatistics* statistics, MLIRCacheArtifacts* artifacts) const {
	const auto backendStart = std::chrono::steady_clock::now();
	if (ir == nullptr) {
		throw RuntimeException("MLIR compilation requires Nautilus IR");
	}

	auto registry = createDialectRegistry();
	::mlir::MLIRContext context(registry);
	if (!options.getOptionOrDefault("mlir.enableMultithreading", true)) {
		context.disableMultithreading();
	}

	MLIRIntrinsicManager intrinsicManager;
	if (options.getOptionOrDefault("mlir.enableIntrinsics", true)) {
		MLIRIntrinsicPluginRegistry::instance().registerAllIntrinsics(intrinsicManager);
	}

	// Build debug-info options once; downstream steps read from this struct.
	// Not const: a source-file write failure below patches `sourceFile` to a
	// fallback path before lowering ever reads it.
	auto debugInfo = debugInfoOptionsFromEngineOptions(options);

	// Prepare the Nautilus-IR "source file" used for debugging. The file
	// must exist before lowering runs so the FileLineColLocs attached by
	// MLIRLoweringProvider point at real content that GDB/LLDB (or, in
	// perf-only mode, `perf annotate`) can read.  `emitDebugInfo()` rather
	// than `enableDebug`: a perf-only compile needs this file just as much
	// as a debug one.
	//
	// computeIRLocations() must run after the last pass that mutates `ir`:
	// it records where each operation lands in this rendering, and an
	// operation minted or removed afterwards would invalidate every line.
	std::shared_ptr<const ir::IRLocationMap> locationMap;
	if (debugInfo.emitDebugInfo()) {
		// Normally computed by IRLocationPass as the pipeline's last pass and
		// published on the graph. Falling back keeps a direct compileIR() call
		// (one that never ran the pipeline's passes) working.
		locationMap = ir->getLocationMap();
		if (locationMap == nullptr) {
			ir::IRLocationPass locationPass;
			locationPass.apply(*ir);
			locationMap = locationPass.getResult();
		}
		if (!debugInfo.sourceFile.empty()) {
			std::ofstream out(debugInfo.sourceFile);
			out << locationMap->text;
			// $TMPDIR is almost always writable; the perf-only default (the
			// working directory -- see DebugInfoOptions.cpp) need not be (a
			// daemon, a read-only container). An unchecked failure here would
			// leave the DWARF naming a file that was never written, and
			// `perf annotate` / GDB's `list` would silently show nothing.
			// This must happen BEFORE loweringProvider->setDebugInfo() below:
			// the path is baked into every op's FileLineColLoc during
			// lowering and cannot be patched up afterwards.
			if (!out) {
				const auto fallback = debugSourceFallbackPath("ir");
				llvm::errs() << "nautilus: could not write debug source file '" << debugInfo.sourceFile
				             << "'; falling back to '" << fallback << "'\n";
				debugInfo.sourceFile = fallback;
				std::ofstream fallbackOut(debugInfo.sourceFile);
				fallbackOut << locationMap->text;
			}
		}
	}

	auto loweringProvider = std::make_unique<MLIRLoweringProvider>(context, options, intrinsicManager);
	if (debugInfo.emitDebugInfo() && locationMap) {
		loweringProvider->setDebugInfo(debugInfo, locationMap);
	}

	const auto loweringStart = std::chrono::steady_clock::now();
	auto mlirModule = loweringProvider->generateModuleFromIR(ir);
	if (*mlirModule == nullptr) {
		throw RuntimeException("verification of MLIR module failed!");
	}
	if (statistics != nullptr) {
		statistics->recordTimingMs("mlir.loweringFromIR.ms", loweringStart);
		int64_t opCount = 0;
		(*mlirModule)->walk([&](::mlir::Operation*) { ++opCount; });
		statistics->set("mlir.module.ops", opCount);
	}

	dumpMLIR(*mlirModule, dumpHandler);

	const auto pipelineStart = std::chrono::steady_clock::now();
	if (mlir::MLIRPassManager::lowerAndOptimizeMLIRModule(mlirModule, {}, debugInfo)) {
		throw RuntimeException("Could not lower and optimize MLIR module.");
	}
	if (statistics != nullptr) {
		statistics->recordTimingMs("mlir.pipeline.ms", pipelineStart);
	}

	auto externalSymbols = loweringProvider->getJitProxyFunctionSymbols();
	auto externalAddresses = loweringProvider->getJitProxyTargetAddresses();
	auto bindingSymbols = loweringProvider->getJitRuntimeBindingSymbols();
	auto bindingAddresses = loweringProvider->getJitRuntimeBindingTargetAddresses();
	externalSymbols.insert(externalSymbols.end(), bindingSymbols.begin(), bindingSymbols.end());
	externalAddresses.insert(externalAddresses.end(), bindingAddresses.begin(), bindingAddresses.end());
	if (externalSymbols.size() != externalAddresses.size()) {
		throw RuntimeException("MLIR external symbol vectors differ in size");
	}

	std::shared_ptr<MLIRJit::ObjectCapture> objectCapture;
	if (artifacts != nullptr) {
		if (exportNames == nullptr) {
			throw RuntimeException("Cache artifact compilation requires an export manifest");
		}
		validateExports(*mlirModule, *exportNames);
		validateExternalSymbols(*mlirModule, externalSymbols, externalAddresses, options);
		const auto serializationStart = std::chrono::steady_clock::now();
		artifacts->moduleManifest = buildModuleManifest(*mlirModule);
		artifacts->bytecode = serializeBytecode(*mlirModule);
		artifacts->externalSymbols = externalSymbols;
		artifacts->externalAddresses = externalAddresses;
		objectCapture = std::make_shared<MLIRJit::ObjectCapture>();
		if (statistics != nullptr) {
			statistics->recordTimingMs("mlir.bytecodeWrite.ms", serializationStart);
			statistics->set("mlir.bytecode.bytes", static_cast<int64_t>(artifacts->bytecode.size()));
		}
	}

	const auto optPipelineStart = std::chrono::steady_clock::now();
	auto optPipeline = LLVMIROptimizer::getLLVMOptimizerPipeline(options, dumpHandler);
	if (statistics != nullptr) {
		statistics->recordTimingMs("llvm.optimizerBuild.ms", optPipelineStart);
	}

	const auto jitStart = std::chrono::steady_clock::now();
	auto engine = JITCompiler::jitCompileModule(
	    mlirModule, optPipeline, externalSymbols, externalAddresses, getCodeGenLevel(debugInfo), debugInfo.enableDebug,
	    debugInfo.enablePerf, debugInfo.perfEmitDebugInfo, debugInfo.perfEmitUnwindInfo, debugInfo.perfRegionSymbols,
	    debugInfo.enableSampleSymbols, ir->getId(), objectCapture);
	if (exportNames != nullptr) {
		materializeExports(*engine, *exportNames);
	} else if (options.getOptionOrDefault("mlir.eager_compilation", false)) {
		std::vector<std::string> names;
		for (const auto* function : ir->getFunctionOperations()) {
			names.push_back(ir->getEmissionName(function));
		}
		materializeExports(*engine, names);
	}
	if (artifacts != nullptr) {
		if (auto object = objectCapture->getObject()) {
			artifacts->object = std::move(*object);
		}
	}
	if (statistics != nullptr) {
		statistics->recordTimingMs("jit.compile.ms", jitStart);
		statistics->recordTimingMs("backend.totalMs", backendStart);
	}
	return std::make_unique<MLIRExecutable>(std::move(engine));
}

std::unique_ptr<Executable> MLIRCompilationBackend::compileCachedBytecode(
    std::string_view bytecode, std::string_view moduleManifest, const std::vector<std::string>& externalSymbols,
    const std::vector<void*>& externalAddresses, const std::vector<std::string>& exportNames,
    const DumpHandler& dumpHandler, const engine::Options& options, CompilationStatistics* statistics,
    MLIRCacheArtifacts* regeneratedArtifacts, const std::string& compilationUnitId) const {
	const auto backendStart = std::chrono::steady_clock::now();
	auto registry = createDialectRegistry();
	::mlir::MLIRContext context(registry);
	if (!options.getOptionOrDefault("mlir.enableMultithreading", true)) {
		context.disableMultithreading();
	}

	const auto loadStart = std::chrono::steady_clock::now();
	auto input =
	    llvm::MemoryBuffer::getMemBufferCopy(llvm::StringRef(bytecode.data(), bytecode.size()), "cached-module.mlirbc");
	llvm::SourceMgr sourceManager;
	sourceManager.AddNewSourceBuffer(std::move(input), llvm::SMLoc());
	::mlir::ParserConfig parserConfig(&context);
	auto mlirModule = ::mlir::parseSourceFile<::mlir::ModuleOp>(sourceManager, parserConfig);
	if (!mlirModule || ::mlir::failed(::mlir::verify(*mlirModule))) {
		throw RuntimeException("Could not load cached MLIR bytecode");
	}
	validateExports(*mlirModule, exportNames);
	validateExternalSymbols(*mlirModule, externalSymbols, externalAddresses, options);
	if (moduleManifest.empty() || buildModuleManifest(*mlirModule) != moduleManifest) {
		throw RuntimeException("Cached MLIR module manifest mismatch");
	}
	if (statistics != nullptr) {
		statistics->recordTimingMs("mlir.bytecodeLoad.ms", loadStart);
		statistics->set("mlir.bytecode.bytes", static_cast<int64_t>(bytecode.size()));
	}
	dumpMLIR(*mlirModule, dumpHandler);

	const auto optPipelineStart = std::chrono::steady_clock::now();
	auto optPipeline = LLVMIROptimizer::getLLVMOptimizerPipeline(options, dumpHandler);
	if (statistics != nullptr) {
		statistics->recordTimingMs("llvm.optimizerBuild.ms", optPipelineStart);
	}

	std::shared_ptr<MLIRJit::ObjectCapture> objectCapture;
	if (regeneratedArtifacts != nullptr) {
		objectCapture = std::make_shared<MLIRJit::ObjectCapture>();
	}
	const auto debugInfo = debugInfoOptionsFromEngineOptions(options);
	const auto jitStart = std::chrono::steady_clock::now();
	auto engine = JITCompiler::jitCompileModule(
	    mlirModule, optPipeline, externalSymbols, externalAddresses, getCodeGenLevel(debugInfo), debugInfo.enableDebug,
	    debugInfo.enablePerf, debugInfo.perfEmitDebugInfo, debugInfo.perfEmitUnwindInfo, debugInfo.perfRegionSymbols,
	    debugInfo.enableSampleSymbols, compilationUnitId, objectCapture);
	materializeExports(*engine, exportNames);
	if (regeneratedArtifacts != nullptr) {
		regeneratedArtifacts->bytecode.assign(bytecode);
		regeneratedArtifacts->moduleManifest.assign(moduleManifest);
		regeneratedArtifacts->externalSymbols = externalSymbols;
		regeneratedArtifacts->externalAddresses = externalAddresses;
		if (auto object = objectCapture->getObject()) {
			regeneratedArtifacts->object = std::move(*object);
		}
	}
	if (statistics != nullptr) {
		statistics->recordTimingMs("jit.compile.ms", jitStart);
		statistics->recordTimingMs("backend.totalMs", backendStart);
	}
	return std::make_unique<MLIRExecutable>(std::move(engine));
}

std::unique_ptr<Executable> MLIRCompilationBackend::compileCachedObject(
    std::string_view object, const std::vector<std::string>& externalSymbols,
    const std::vector<void*>& externalAddresses, const std::vector<std::string>& exportNames,
    const engine::Options& options, CompilationStatistics* statistics, const std::string& compilationUnitId) const {
	const auto backendStart = std::chrono::steady_clock::now();
	if (externalSymbols.size() != externalAddresses.size() || exportNames.empty()) {
		throw RuntimeException("Cached object manifest is invalid");
	}
	std::unordered_set<std::string> symbols;
	for (std::size_t index = 0; index < externalSymbols.size(); ++index) {
		if (externalSymbols[index].empty() || externalAddresses[index] == nullptr ||
		    !symbols.insert(externalSymbols[index]).second) {
			throw RuntimeException("Cached object external symbol manifest is invalid");
		}
	}
	const auto jitStart = std::chrono::steady_clock::now();
	const auto debugInfo = debugInfoOptionsFromEngineOptions(options);
	MLIRJit::Options jitOptions;
	jitOptions.enableDebuggerSupport = debugInfo.enableDebug;
	jitOptions.enablePerfSupport = debugInfo.enablePerf;
	jitOptions.perfEmitDebugInfo = debugInfo.perfEmitDebugInfo;
	jitOptions.perfEmitUnwindInfo = debugInfo.perfEmitUnwindInfo;
	jitOptions.perfRegionSymbols = debugInfo.perfRegionSymbols;
	jitOptions.enableJitSymbolRegistration = debugInfo.enableSampleSymbols;
	jitOptions.compilationUnitId = compilationUnitId;
	auto engine = JITCompiler::jitLoadObject(object, externalSymbols, externalAddresses, jitOptions);
	materializeExports(*engine, exportNames);
	if (statistics != nullptr) {
		statistics->recordTimingMs("jit.objectLoad.ms", jitStart);
		statistics->recordTimingMs("backend.totalMs", backendStart);
	}
	return std::make_unique<MLIRExecutable>(std::move(engine));
}

} // namespace nautilus::compiler::mlir

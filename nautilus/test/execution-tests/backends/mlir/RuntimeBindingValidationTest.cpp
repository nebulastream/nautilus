#include "catch2/catch_test_macros.hpp"
#include "catch2/matchers/catch_matchers_string.hpp"
#include "nautilus/Artifact.hpp"
#include "nautilus/CompilationStatistics.hpp"
#include "nautilus/Engine.hpp"
#include "nautilus/RuntimeBinding.hpp"
#include "nautilus/compiler/artifact/ArtifactCodec.hpp"
#include "nautilus/compiler/artifact/ArtifactSupport.hpp"
#include "nautilus/compiler/backends/mlir/MLIRArtifactValidation.hpp"
#include "nautilus/exceptions/RuntimeException.hpp"
#include <algorithm>
#include <array>
#include <chrono>
#include <cstdint>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <llvm/ADT/SmallString.h>
#include <llvm/Analysis/LoopInfo.h>
#include <llvm/BinaryFormat/ELF.h>
#include <llvm/IR/Dominators.h>
#include <llvm/IR/GlobalVariable.h>
#include <llvm/IR/InlineAsm.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IRReader/IRReader.h>
#include <llvm/Object/ELFObjectFile.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/MemoryBuffer.h>
#include <llvm/Support/Program.h>
#include <llvm/Support/SourceMgr.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/TargetParser/Host.h>
#include <llvm/TargetParser/Triple.h>
#include <memory>
#include <mlir/Bytecode/BytecodeWriter.h>
#include <mlir/Dialect/LLVMIR/LLVMDialect.h>
#include <mlir/IR/Builders.h>
#include <mlir/IR/MLIRContext.h>
#include <mlir/IR/Verifier.h>
#include <mlir/Parser/Parser.h>
#include <optional>
#include <string>
#include <string_view>
#include <system_error>
#include <utility>
#include <vector>

namespace nautilus::engine {
namespace {

int32_t bindingValidationIncrement(int32_t value) {
	return value + 1;
}

std::string printBindingModule(::mlir::ModuleOp module) {
	std::string text;
	llvm::raw_string_ostream output(text);
	module.print(output);
	output.flush();
	return text;
}

void resealBindingArtifact(artifact::ModuleArtifact& value) {
	value.descriptor.bytecodeDigest = artifact::detail::digest(value.bytecode);
	value.descriptorDigest = artifact::detail::digest(artifact::detail::encodeDescriptor(value.descriptor));
}

void requireBindingArtifactPlatform() {
#if !defined(__linux__) || !defined(__x86_64__)
	SKIP("Module artifacts require the supported Linux x86-64 ELF producer");
#else
	REQUIRE(artifact::isSupported());
#endif
}

struct BindingValidationFixture {
	BindingValidationFixture() {
		context.disableMultithreading();
		context.loadDialect<::mlir::LLVM::LLVMDialect>();
		module = ::mlir::parseSourceString<::mlir::ModuleOp>(R"mlir(
module {
  llvm.mlir.global internal constant @constant(7 : i32) : i32
  llvm.func @bindingValidationIncrement(i32) -> i32
  llvm.func @execute(%value: i32) -> i32 {
    %result = llvm.call @bindingValidationIncrement(%value) : (i32) -> i32
    llvm.return %result : i32
  }
}
)mlir",
		                                                     &context);
		REQUIRE(static_cast<bool>(module));
		RuntimeBindings bindings;
		(void) bindings.bind<int64_t>("unused", &unused);
		options.setRuntimeBindings(bindings);
		setBindingSchema();
		REQUIRE(::mlir::succeeded(::mlir::verify(*module)));
	}

	void setBindingSchema() {
		module->getOperation()->setAttr("nautilus.runtime_binding.schema",
		                                ::mlir::StringAttr::get(&context, options.getRuntimeBindings().schema()));
	}

	::mlir::LLVM::GlobalOp addBindingDeclaration() {
		const auto& entry = options.getRuntimeBindings().entries().at("unused");
		::mlir::OpBuilder builder(&context);
		builder.setInsertionPointToStart(module->getBody());
		auto global =
		    ::mlir::LLVM::GlobalOp::create(builder, module->getLoc(), builder.getI8Type(), false,
		                                   ::mlir::LLVM::Linkage::External, entry->symbol, ::mlir::Attribute {}, 1);
		global->setAttr("nautilus.runtime_binding.identity", builder.getStringAttr(entry->identity));
		global->setAttr("nautilus.runtime_binding.type", builder.getStringAttr(entry->type));
		externalSymbols.push_back(entry->symbol);
		externalAddresses.push_back(entry->address);
		REQUIRE(::mlir::succeeded(::mlir::verify(*module)));
		REQUIRE_NOTHROW(validate());
		return global;
	}

	void validate() {
		compiler::mlir::validateArtifactMLIRModule(*module, exportNames, externalSymbols, externalAddresses, options);
	}

	void requireFailure(const std::string& expected) {
		REQUIRE(::mlir::succeeded(::mlir::verify(*module)));
		requireValidationFailure(expected);
	}

	void requireValidationFailure(const std::string& expected) {
		const auto before = printBindingModule(*module);
		const auto beforeExports = exportNames;
		const auto beforeSymbols = externalSymbols;
		const auto beforeAddresses = externalAddresses;
		const auto beforeOptions = options.getOptionValues();
		const auto beforeSchema = options.getRuntimeBindings().schema();
		const auto beforeBindings = options.getRuntimeBindings().entries();
		REQUIRE_THROWS_WITH(validate(), Catch::Matchers::ContainsSubstring(expected));
		REQUIRE(printBindingModule(*module) == before);
		REQUIRE(exportNames == beforeExports);
		REQUIRE(externalSymbols == beforeSymbols);
		REQUIRE(externalAddresses == beforeAddresses);
		REQUIRE(options.getOptionValues() == beforeOptions);
		REQUIRE(options.getRuntimeBindings().schema() == beforeSchema);
		REQUIRE(options.getRuntimeBindings().entries() == beforeBindings);
	}

	int64_t unused = 42;
	Options options;
	::mlir::MLIRContext context;
	::mlir::OwningOpRef<::mlir::ModuleOp> module;
	std::vector<std::string> exportNames {"execute"};
	std::vector<std::string> externalSymbols {"bindingValidationIncrement"};
	std::vector<void*> externalAddresses {reinterpret_cast<void*>(&bindingValidationIncrement)};
};

::mlir::LLVM::LLVMFuncOp declareBindingFunction(::mlir::ModuleOp module, const std::string& symbol, bool definition) {
	::mlir::OpBuilder builder(module.getContext());
	builder.setInsertionPointToStart(module.getBody());
	auto type = ::mlir::LLVM::LLVMFunctionType::get(::mlir::LLVM::LLVMVoidType::get(module.getContext()), {}, false);
	auto function = ::mlir::LLVM::LLVMFuncOp::create(builder, module.getLoc(), symbol, type);
	if (definition) {
		builder.setInsertionPointToStart(function.addEntryBlock(builder));
		::mlir::LLVM::ReturnOp::create(builder, module.getLoc(), ::mlir::ValueRange {});
	}
	return function;
}

void setBindingObjectSymbolType(std::string& object, const std::string& symbol, unsigned char type) {
	auto objectFile =
	    llvm::object::ObjectFile::createObjectFile(llvm::MemoryBufferRef(object, "binding-symbol-type.o"));
	REQUIRE(static_cast<bool>(objectFile));
	const auto* parsed = llvm::dyn_cast<llvm::object::ELF64LEObjectFile>(objectFile->get());
	REQUIRE(parsed != nullptr);
	std::size_t matches = 0;
	for (const auto& reference : parsed->symbols()) {
		auto name = reference.getName();
		REQUIRE(static_cast<bool>(name));
		if (*name != symbol) {
			continue;
		}
		auto entry = parsed->getSymbol(reference.getRawDataRefImpl());
		REQUIRE(static_cast<bool>(entry));
		REQUIRE((*entry)->isUndefined());
		REQUIRE((*entry)->isExternal());
		auto changed = **entry;
		changed.setType(type);
		const auto offset = static_cast<std::size_t>(reinterpret_cast<const char*>(*entry) - object.data());
		REQUIRE(offset <= object.size());
		REQUIRE(sizeof(changed) <= object.size() - offset);
		std::memcpy(object.data() + offset, &changed, sizeof(changed));
		REQUIRE(llvm::object::ELFSymbolRef(reference).getELFType() == type);
		++matches;
	}
	REQUIRE(matches == 1);
}

class BindingObjectInspectionDirectory {
public:
	BindingObjectInspectionDirectory() {
		llvm::SmallString<128> directory;
		if (const auto error = llvm::sys::fs::createUniqueDirectory("nautilus-binding-object-inspection", directory)) {
			throw std::system_error(error, "Create binding object inspection directory");
		}
		path_ = directory.str().str();
	}
	~BindingObjectInspectionDirectory() {
		std::error_code ignored;
		std::filesystem::remove_all(path_, ignored);
	}
	const std::filesystem::path& path() const {
		return path_;
	}

private:
	std::filesystem::path path_;
};

} // namespace

TEST_CASE_METHOD(BindingValidationFixture, "MLIR artifact validation preserves unused binding snapshots",
                 "[runtime-bindings][cache][mlir][validation]") {
	const auto& entry = options.getRuntimeBindings().entries().at("unused");
	REQUIRE(module->lookupSymbol(entry->symbol) == nullptr);
	SECTION("does not require an external manifest entry") {
		REQUIRE(externalSymbols.size() == 1);
	}
	SECTION("permits an external manifest entry without a module declaration") {
		externalSymbols.push_back(entry->symbol);
		externalAddresses.push_back(entry->address);
	}
	REQUIRE(::mlir::succeeded(::mlir::verify(*module)));
	const auto beforeModule = printBindingModule(*module);
	const auto beforeExports = exportNames;
	const auto beforeSymbols = externalSymbols;
	const auto beforeAddresses = externalAddresses;
	const auto beforeOptions = options.getOptionValues();
	const auto beforeSchema = options.getRuntimeBindings().schema();
	const auto beforeBindings = options.getRuntimeBindings().entries();
	REQUIRE_NOTHROW(validate());
	REQUIRE_NOTHROW(validate());
	REQUIRE(printBindingModule(*module) == beforeModule);
	REQUIRE(exportNames == beforeExports);
	REQUIRE(externalSymbols == beforeSymbols);
	REQUIRE(externalAddresses == beforeAddresses);
	REQUIRE(options.getOptionValues() == beforeOptions);
	REQUIRE(options.getRuntimeBindings().schema() == beforeSchema);
	REQUIRE(options.getRuntimeBindings().entries() == beforeBindings);
	REQUIRE(unused == 42);
}

TEST_CASE_METHOD(BindingValidationFixture, "MLIR artifact binding registries do not waive export validation",
                 "[runtime-bindings][cache][mlir][validation]") {
	std::string expected;
	SECTION("no exports") {
		exportNames.clear();
		expected = "MLIR artifact has no exports";
	}
	SECTION("empty export name") {
		exportNames[0].clear();
		expected = "MLIR artifact export manifest is invalid";
	}
	SECTION("duplicate export name") {
		exportNames.push_back("execute");
		expected = "MLIR artifact export manifest is invalid";
	}
	SECTION("missing export") {
		exportNames = {"missing"};
		expected = "MLIR artifact module is missing export 'missing'";
	}
	SECTION("external declaration is not an export definition") {
		exportNames = {"bindingValidationIncrement"};
		expected = "MLIR artifact module is missing export 'bindingValidationIncrement'";
	}
	SECTION("global is not an export function") {
		exportNames = {"constant"};
		expected = "MLIR artifact module is missing export 'constant'";
	}
	requireFailure(expected);
}

TEST_CASE_METHOD(BindingValidationFixture, "MLIR artifact validation preserves binding failure precedence",
                 "[runtime-bindings][cache][mlir][validation]") {
	std::string expected;
	SECTION("exports precede vector lengths and runtime bindings") {
		exportNames.clear();
		externalAddresses.clear();
		module->getOperation()->removeAttr("nautilus.runtime_binding.schema");
		expected = "MLIR artifact has no exports";
	}
	SECTION("every export is checked before external validation") {
		exportNames.push_back("missing");
		externalAddresses.clear();
		module->getOperation()->removeAttr("nautilus.runtime_binding.schema");
		expected = "MLIR artifact module is missing export 'missing'";
	}
	SECTION("export failures follow manifest order") {
		exportNames = {"missing", ""};
		expected = "MLIR artifact export manifest is invalid";
	}
	SECTION("external vector lengths precede runtime bindings") {
		externalAddresses.clear();
		module->getOperation()->removeAttr("nautilus.runtime_binding.schema");
		expected = "MLIR artifact external symbol vectors differ in size";
	}
	SECTION("runtime bindings precede external name validation") {
		externalSymbols[0].clear();
		module->getOperation()->removeAttr("nautilus.runtime_binding.schema");
		expected = "runtime binding schema mismatch";
	}
	SECTION("runtime bindings precede external declaration checks") {
		externalSymbols.clear();
		externalAddresses.clear();
		module->getOperation()->removeAttr("nautilus.runtime_binding.schema");
		expected = "runtime binding schema mismatch";
	}
	SECTION("external manifest validity precedes declaration checks") {
		externalSymbols[0] = "missing";
		externalAddresses[0] = nullptr;
		expected = "MLIR artifact external symbol manifest is invalid";
	}
	SECTION("unlisted module declarations precede extra manifest names") {
		externalSymbols[0] = "missing";
		expected = "MLIR artifact contains an undeclared external function";
	}
	requireFailure(expected);
}

TEST_CASE_METHOD(BindingValidationFixture, "MLIR binding declarations accept explicit and implicit byte alignment",
                 "[runtime-bindings][cache][mlir][validation][B3]") {
	auto global = addBindingDeclaration();
	SECTION("explicit byte alignment") {
		REQUIRE(global.getAlignment() == uint64_t {1});
	}
	SECTION("implicit byte alignment") {
		global.removeAlignmentAttr();
		REQUIRE_FALSE(global.getAlignment().has_value());
	}
	REQUIRE(global.getType().isInteger(8));
	REQUIRE(global.getLinkage() == ::mlir::LLVM::Linkage::External);
	REQUIRE_FALSE(global.getConstant());
	REQUIRE_FALSE(global.getValueOrNull());
	REQUIRE(global.getInitializerRegion().empty());
	REQUIRE_FALSE(global.getThreadLocal_());
	REQUIRE(global.getAddrSpace() == 0);
	REQUIRE_FALSE(global.getUnnamedAddrAttr());
	REQUIRE(global.getVisibility_() == ::mlir::LLVM::Visibility::Default);
	REQUIRE_FALSE(global.getDsoLocal());
	REQUIRE_FALSE(global.getExternallyInitialized());
	REQUIRE(::mlir::succeeded(::mlir::verify(*module)));
	const auto before = printBindingModule(*module);
	REQUIRE_NOTHROW(validate());
	REQUIRE_NOTHROW(validate());
	REQUIRE(printBindingModule(*module) == before);
	REQUIRE(unused == 42);
}

TEST_CASE_METHOD(BindingValidationFixture, "MLIR binding validation rejects nonzero address spaces without symbol uses",
                 "[runtime-bindings][cache][mlir][validation][B3]") {
	auto global = addBindingDeclaration();
	global.setAddrSpace(1);
	REQUIRE(global.getAddrSpace() == 1);
	requireFailure("MLIR artifact runtime binding declaration mismatch");
}

TEST_CASE_METHOD(BindingValidationFixture, "MLIR binding validation directly rejects unsupported declaration forms",
                 "[runtime-bindings][cache][mlir][validation][B3]") {
	auto global = addBindingDeclaration();
	SECTION("internal linkage without an initializer") {
		global.setLinkage(::mlir::LLVM::Linkage::Internal);
		REQUIRE(global.getLinkage() == ::mlir::LLVM::Linkage::Internal);
	}
	SECTION("appending linkage on a scalar declaration") {
		global.setLinkage(::mlir::LLVM::Linkage::Appending);
		REQUIRE(global.getLinkage() == ::mlir::LLVM::Linkage::Appending);
	}
	SECTION("zero alignment") {
		global.setAlignment(0);
		REQUIRE(global.getAlignment() == uint64_t {0});
	}
	SECTION("non-power-of-two alignment") {
		global.setAlignment(3);
		REQUIRE(global.getAlignment() == uint64_t {3});
	}
	requireValidationFailure("MLIR artifact runtime binding declaration mismatch");
}

TEST_CASE_METHOD(BindingValidationFixture, "MLIR binding schemas reject function collisions even for unused entries",
                 "[runtime-bindings][cache][mlir][validation][B3]") {
	bool definition = false;
	SECTION("unused binding symbol declared as an external function") {
	}
	SECTION("unused binding symbol defined as a function") {
		definition = true;
	}
	const auto& entry = options.getRuntimeBindings().entries().at("unused");
	auto function = declareBindingFunction(*module, entry->symbol, definition);
	REQUIRE(function.getSymName() == entry->symbol);
	REQUIRE(function.getBody().empty() == !definition);
	requireFailure("MLIR artifact runtime binding symbol is declared as a function");
}

TEST_CASE("RuntimeBindings validates cached MLIR declarations before the module manifest",
          "[runtime-bindings][cache][mlir][validation][B3]") {
	requireBindingArtifactPlatform();
	int64_t value = 42, replacement = 97;
	RuntimeBindings bindings;
	auto state = bindings.bind<int64_t>("state", &value);
	Options options;
	options.setOption("engine.backend", std::string("mlir"));
	options.setOption("mlir.enableMultithreading", false);
	options.setRuntimeBindings(bindings);
	NautilusEngine engine(options);
	auto builder = engine.createModule();
	int wrappers = 0;
	builder.registerFunction<val<int64_t>()>("execute", [state, &wrappers]() -> val<int64_t> {
		++wrappers;
		return *state.get();
	});
	auto artifacts = builder.createArtifact();
	REQUIRE(wrappers > 0);
	const auto coldWrappers = wrappers;
	REQUIRE(artifacts.descriptor.version == 2);
	REQUIRE(artifacts.descriptor.bindingSchema.size() == 1);
	auto pristine = artifact::loadBytecode(artifacts, options);
	REQUIRE(pristine.getFunction<int64_t()>("execute")() == 42);
	REQUIRE_FALSE(artifacts.bytecode.empty());
	REQUIRE_FALSE(artifacts.descriptor.moduleManifest.empty());
	auto imports = artifact::detail::resolveImports(artifacts.descriptor, true, options);
	REQUIRE(imports.symbols.size() == 1);
	REQUIRE(imports.symbols[0] == bindings.entries().at("state")->symbol);
	REQUIRE(imports.addresses.size() == 1);
	REQUIRE(imports.addresses[0] == &value);

	::mlir::MLIRContext context;
	context.disableMultithreading();
	context.loadDialect<::mlir::LLVM::LLVMDialect>();
	auto module = ::mlir::parseSourceString<::mlir::ModuleOp>(artifacts.bytecode, &context);
	REQUIRE(static_cast<bool>(module));
	auto global = module->lookupSymbol<::mlir::LLVM::GlobalOp>(imports.symbols[0]);
	REQUIRE(static_cast<bool>(global));
	std::string expected = "MLIR artifact module manifest mismatch";
	bool externalManifestOnly = false;
	SECTION("pristine bytecode reaches the manifest check") {
	}
	SECTION("absent schema") {
		module->getOperation()->removeAttr("nautilus.runtime_binding.schema");
		expected = "runtime binding schema mismatch";
	}
	SECTION("mismatched schema") {
		module->getOperation()->setAttr("nautilus.runtime_binding.schema",
		                                ::mlir::StringAttr::get(&context, RuntimeBindings {}.schema()));
		expected = "runtime binding schema mismatch";
	}
	SECTION("same schema loaded without its registry") {
		options.setRuntimeBindings(RuntimeBindings {});
		expected = "runtime binding schema mismatch";
		externalManifestOnly = true;
	}
	SECTION("absent binding identity") {
		global->removeAttr("nautilus.runtime_binding.identity");
		expected = "runtime binding declaration mismatch";
	}
	SECTION("mismatched binding identity") {
		global->setAttr("nautilus.runtime_binding.identity", ::mlir::StringAttr::get(&context, "different-state"));
		expected = "runtime binding declaration mismatch";
	}
	SECTION("absent binding type") {
		global->removeAttr("nautilus.runtime_binding.type");
		expected = "runtime binding declaration mismatch";
	}
	SECTION("same-sized wrong binding type") {
		global->setAttr("nautilus.runtime_binding.type",
		                ::mlir::StringAttr::get(&context, runtime_binding::typeSchema<uint64_t>()));
		expected = "runtime binding declaration mismatch";
	}
	SECTION("wrong global element type") {
		global.setGlobalType(::mlir::IntegerType::get(&context, 64));
		expected = "runtime binding declaration mismatch";
	}
	SECTION("constant binding global") {
		global.setConstant(true);
		expected = "runtime binding declaration mismatch";
	}
	SECTION("external weak binding linkage") {
		global.setLinkage(::mlir::LLVM::Linkage::ExternWeak);
		REQUIRE(global.getLinkage() == ::mlir::LLVM::Linkage::ExternWeak);
		expected = "runtime binding declaration mismatch";
	}
	SECTION("thread-local binding global") {
		global.setThreadLocal_(true);
		REQUIRE(global.getThreadLocal_());
		expected = "runtime binding declaration mismatch";
	}
	SECTION("over-aligned binding global") {
		global.setAlignment(8);
		REQUIRE(global.getAlignment() == uint64_t {8});
		expected = "runtime binding declaration mismatch";
	}
	SECTION("local unnamed address") {
		global.setUnnamedAddr(::mlir::LLVM::UnnamedAddr::Local);
		REQUIRE(global.getUnnamedAddr() == ::mlir::LLVM::UnnamedAddr::Local);
		expected = "runtime binding declaration mismatch";
	}
	SECTION("global unnamed address") {
		global.setUnnamedAddr(::mlir::LLVM::UnnamedAddr::Global);
		REQUIRE(global.getUnnamedAddr() == ::mlir::LLVM::UnnamedAddr::Global);
		expected = "runtime binding declaration mismatch";
	}
	SECTION("explicit none unnamed address") {
		global.setUnnamedAddr(::mlir::LLVM::UnnamedAddr::None);
		REQUIRE(global.getUnnamedAddr() == ::mlir::LLVM::UnnamedAddr::None);
		REQUIRE(static_cast<bool>(global.getUnnamedAddrAttr()));
		expected = "runtime binding declaration mismatch";
	}
	SECTION("hidden binding visibility") {
		global.setVisibility_(::mlir::LLVM::Visibility::Hidden);
		REQUIRE(global.getVisibility_() == ::mlir::LLVM::Visibility::Hidden);
		expected = "runtime binding declaration mismatch";
	}
	SECTION("protected binding visibility") {
		global.setVisibility_(::mlir::LLVM::Visibility::Protected);
		REQUIRE(global.getVisibility_() == ::mlir::LLVM::Visibility::Protected);
		expected = "runtime binding declaration mismatch";
	}
	SECTION("DSO-local binding global") {
		global.setDsoLocal(true);
		REQUIRE(global.getDsoLocal());
		expected = "runtime binding declaration mismatch";
	}
	SECTION("externally initialized binding global") {
		global.setExternallyInitialized(true);
		REQUIRE(global.getExternallyInitialized());
		expected = "runtime binding declaration mismatch";
	}
	SECTION("binding global placed in a section") {
		global.setSection(".data.runtime_binding");
		REQUIRE(global.getSection() == llvm::StringRef(".data.runtime_binding"));
		expected = "runtime binding declaration mismatch";
	}
	SECTION("binding global with a COMDAT selector") {
		::mlir::OpBuilder builder(&context);
		builder.setInsertionPointToStart(module->getBody());
		auto comdat = ::mlir::LLVM::ComdatOp::create(builder, module->getLoc(), "binding_comdat");
		builder.setInsertionPointToStart(&comdat.getBody().front());
		::mlir::LLVM::ComdatSelectorOp::create(builder, module->getLoc(), "any", ::mlir::LLVM::comdat::Comdat::Any);
		auto selector =
		    ::mlir::SymbolRefAttr::get(&context, "binding_comdat", {::mlir::FlatSymbolRefAttr::get(&context, "any")});
		global.setComdatAttr(selector);
		REQUIRE(global.getComdatAttr() == selector);
		expected = "runtime binding declaration mismatch";
	}
	SECTION("binding global with an initializer") {
		global.setValueAttr(::mlir::IntegerAttr::get(global.getType(), 0));
		expected = "runtime binding declaration mismatch";
	}
	SECTION("binding global with an initializer region") {
		::mlir::OpBuilder builder(&context);
		builder.createBlock(&global.getInitializerRegion());
		auto initial = ::mlir::LLVM::ConstantOp::create(builder, module->getLoc(), global.getType(), int64_t {0});
		::mlir::LLVM::ReturnOp::create(builder, module->getLoc(), ::mlir::ValueRange {initial.getRes()});
		REQUIRE_FALSE(global.getValueOrNull());
		REQUIRE_FALSE(global.getInitializerRegion().empty());
		expected = "runtime binding declaration mismatch";
	}
	SECTION("binding symbol declared as an external function") {
		global.erase();
		auto function = declareBindingFunction(*module, imports.symbols[0], false);
		REQUIRE(function.getBody().empty());
		expected = "runtime binding symbol is declared as a function";
	}
	SECTION("binding symbol defined as a function") {
		global.erase();
		auto function = declareBindingFunction(*module, imports.symbols[0], true);
		REQUIRE_FALSE(function.getBody().empty());
		expected = "runtime binding symbol is declared as a function";
	}
	SECTION("address differs from the load environment") {
		imports.addresses[0] = &replacement;
		expected = "runtime binding address does not match the load environment";
		externalManifestOnly = true;
	}
	SECTION("null binding address") {
		imports.addresses[0] = nullptr;
		expected = "MLIR artifact external symbol manifest is invalid";
		externalManifestOnly = true;
	}
	SECTION("binding address missing from both vectors") {
		imports.symbols.clear();
		imports.addresses.clear();
		expected = "runtime binding address is missing";
		externalManifestOnly = true;
	}
	SECTION("stale address after a compatible registry rebind") {
		RuntimeBindings rebound;
		(void) rebound.bind<int64_t>("state", &replacement);
		REQUIRE(rebound.schema() == bindings.schema());
		options.setRuntimeBindings(rebound);
		expected = "runtime binding address does not match the load environment";
		externalManifestOnly = true;
	}
	SECTION("compatible registry and fresh imports reload pristine bytecode and native object") {
		RuntimeBindings rebound;
		(void) rebound.bind<int64_t>("state", &replacement);
		REQUIRE(rebound.schema() == bindings.schema());
		options.setRuntimeBindings(rebound);
		const auto refreshed = artifact::detail::resolveImports(artifacts.descriptor, true, options);
		REQUIRE(refreshed.symbols == imports.symbols);
		REQUIRE(refreshed.addresses == std::vector<void*> {&replacement});
		auto loaded = artifact::loadBytecode(artifact::decode(artifact::encode(artifacts)), options);
		auto native = artifact::loadNative(artifacts, options);
		REQUIRE(loaded.getFunction<int64_t()>("execute")() == 97);
		REQUIRE(native.getFunction<int64_t()>("execute")() == 97);
		replacement = 113;
		REQUIRE(loaded.getFunction<int64_t()>("execute")() == 113);
		REQUIRE(native.getFunction<int64_t()>("execute")() == 113);
		REQUIRE(pristine.getFunction<int64_t()>("execute")() == 42);
		REQUIRE(wrappers == coldWrappers);
		return;
	}

	REQUIRE(::mlir::succeeded(::mlir::verify(*module)));
	if (expected != "MLIR artifact module manifest mismatch") {
		REQUIRE_THROWS_WITH(compiler::mlir::validateArtifactMLIRModule(*module, {"execute"}, imports.symbols,
		                                                               imports.addresses, options),
		                    Catch::Matchers::ContainsSubstring(expected));
	}
	if (externalManifestOnly) {
		if (options.getRuntimeBindings().entries().empty()) {
			REQUIRE_THROWS_AS(artifact::loadNative(artifacts, options), RuntimeException);
			REQUIRE_THROWS_AS(artifact::loadBytecode(artifacts, options), RuntimeException);
		} else {
			auto native = artifact::loadNative(artifacts, options);
			auto loaded = artifact::loadBytecode(artifacts, options);
			const auto* current =
			    static_cast<const int64_t*>(options.getRuntimeBindings().entries().at("state")->address);
			REQUIRE(native.getFunction<int64_t()>("execute")() == *current);
			REQUIRE(loaded.getFunction<int64_t()>("execute")() == *current);
		}
		REQUIRE(pristine.getFunction<int64_t()>("execute")() == 42);
		REQUIRE(value == 42);
		REQUIRE(replacement == 97);
		REQUIRE(wrappers == coldWrappers);
		return;
	}
	std::string bytecode;
	llvm::raw_string_ostream output(bytecode);
	REQUIRE(::mlir::succeeded(::mlir::writeBytecodeToFile(module->getOperation(), output)));
	output.flush();
	artifacts.bytecode = std::move(bytecode);
	artifacts.descriptor.moduleManifest = "deliberately-wrong-module-manifest";
	resealBindingArtifact(artifacts);
	REQUIRE_THROWS_WITH(artifact::loadBytecode(artifacts, options), Catch::Matchers::ContainsSubstring(expected));
	REQUIRE(value == 42);
	REQUIRE(replacement == 97);
	REQUIRE(wrappers == coldWrappers);
}

TEST_CASE("RuntimeBindings native symbol validation uses the supplied binding options",
          "[runtime-bindings][cache][mlir][validation][B3]") {
	requireBindingArtifactPlatform();
	int64_t value = 42, unused = 13, replacement = 97;
	RuntimeBindings bindings;
	auto state = bindings.bind<int64_t>("state", &value);
	(void) bindings.bind<int64_t>("unused", &unused);
	Options options;
	options.setOption("engine.backend", std::string("mlir"));
	options.setOption("mlir.enableMultithreading", false);
	options.setRuntimeBindings(bindings);
	NautilusEngine engine(options);
	auto builder = engine.createModule();
	int wrappers = 0;
	builder.registerFunction<val<int64_t>()>("execute", [state, &wrappers]() -> val<int64_t> {
		++wrappers;
		return *state.get();
	});
	auto artifacts = builder.createArtifact();
	REQUIRE(wrappers > 0);
	const auto coldWrappers = wrappers;
	const auto& symbol = bindings.entries().at("state")->symbol;
	const auto& unusedSymbol = bindings.entries().at("unused")->symbol;
	auto imports = artifact::detail::resolveImports(artifacts.descriptor, true, options);
	REQUIRE(imports.symbols == std::vector<std::string> {symbol, unusedSymbol});
	REQUIRE(imports.addresses == std::vector<void*> {&value, &unused});
	const auto original = compiler::mlir::inspectArtifactObject(artifacts.object, options);
	REQUIRE(original.undefinedSymbols == std::vector<std::string> {symbol});
	REQUIRE(std::ranges::find(original.definedFunctionSymbols, "execute") != original.definedFunctionSymbols.end());
	REQUIRE(std::ranges::find(original.undefinedSymbols, unusedSymbol) == original.undefinedSymbols.end());
	REQUIRE_NOTHROW(compiler::mlir::validateArtifactObjectSymbols(artifacts.object, {"execute"}, imports.symbols,
	                                                              imports.addresses, options));
	auto object = artifacts.object;
	setBindingObjectSymbolType(object, symbol, llvm::ELF::STT_OBJECT);
	std::vector<std::string> exports {"execute"};
	std::string inspectionError, validationError;
	SECTION("typed binding data symbol is accepted with its registry") {
	}
	SECTION("unused registrations require no native declaration") {
		REQUIRE(options.getRuntimeBindings().entries().size() == 2);
	}
	SECTION("typed binding data symbol is rejected without binding options") {
		options.setRuntimeBindings(RuntimeBindings {});
		inspectionError = "MLIR artifacts do not support external globals";
		validationError = inspectionError;
	}
	SECTION("typed binding function symbol collides with the registry") {
		setBindingObjectSymbolType(object, symbol, llvm::ELF::STT_FUNC);
		inspectionError = "MLIR artifact runtime binding symbol is declared as a function";
		validationError = inspectionError;
	}
	SECTION("same-schema replacement rejects stale import addresses") {
		RuntimeBindings rebound;
		(void) rebound.bind<int64_t>("state", &replacement);
		(void) rebound.bind<int64_t>("unused", &unused);
		REQUIRE(rebound.schema() == bindings.schema());
		options.setRuntimeBindings(rebound);
		validationError = "MLIR artifact runtime binding address does not match the load environment";
	}
	SECTION("same-schema replacement accepts freshly resolved import addresses") {
		RuntimeBindings rebound;
		(void) rebound.bind<int64_t>("state", &replacement);
		(void) rebound.bind<int64_t>("unused", &unused);
		REQUIRE(rebound.schema() == bindings.schema());
		options.setRuntimeBindings(rebound);
		imports = artifact::detail::resolveImports(artifacts.descriptor, true, options);
		REQUIRE(imports.symbols == std::vector<std::string> {symbol, unusedSymbol});
		REQUIRE(imports.addresses == std::vector<void*> {&replacement, &unused});
	}
	SECTION("binding import address differs from the supplied registry") {
		imports.addresses[0] = &replacement;
		validationError = "MLIR artifact runtime binding address does not match the load environment";
	}
	SECTION("missing binding import is rejected") {
		imports.symbols.clear();
		imports.addresses.clear();
		validationError = "MLIR artifact object contains undeclared import '" + symbol + "'";
	}
	SECTION("null binding import address is rejected") {
		imports.addresses[0] = nullptr;
		validationError = "MLIR artifact external symbol manifest is invalid";
	}
	SECTION("binding import is not a defined native export") {
		exports = {symbol};
		validationError = "MLIR artifact object is missing defined export '" + symbol + "'";
	}
	const auto beforeObject = object;
	const auto beforeExports = exports;
	const auto beforeSymbols = imports.symbols;
	const auto beforeAddresses = imports.addresses;
	const auto beforeOptions = options.getOptionValues();
	const auto beforeSchema = options.getRuntimeBindings().schema();
	const auto beforeBindings = options.getRuntimeBindings().entries();
	if (inspectionError.empty()) {
		const auto inspected = compiler::mlir::inspectArtifactObject(object, options);
		REQUIRE(inspected.undefinedSymbols == original.undefinedSymbols);
		REQUIRE(inspected.definedFunctionSymbols == original.definedFunctionSymbols);
		REQUIRE(inspected.definedSymbols == original.definedSymbols);
	} else {
		REQUIRE_THROWS_WITH(compiler::mlir::inspectArtifactObject(object, options),
		                    Catch::Matchers::ContainsSubstring(inspectionError));
	}
	if (validationError.empty()) {
		REQUIRE_NOTHROW(compiler::mlir::validateArtifactObjectSymbols(object, exports, imports.symbols,
		                                                              imports.addresses, options));
		REQUIRE_NOTHROW(compiler::mlir::validateArtifactObjectSymbols(object, exports, imports.symbols,
		                                                              imports.addresses, options));
	} else {
		REQUIRE_THROWS_WITH(
		    compiler::mlir::validateArtifactObjectSymbols(object, exports, imports.symbols, imports.addresses, options),
		    Catch::Matchers::ContainsSubstring(validationError));
	}
	REQUIRE(object == beforeObject);
	REQUIRE(exports == beforeExports);
	REQUIRE(imports.symbols == beforeSymbols);
	REQUIRE(imports.addresses == beforeAddresses);
	REQUIRE(options.getOptionValues() == beforeOptions);
	REQUIRE(options.getRuntimeBindings().schema() == beforeSchema);
	REQUIRE(options.getRuntimeBindings().entries() == beforeBindings);
	REQUIRE(value == 42);
	REQUIRE(unused == 13);
	REQUIRE(replacement == 97);
	REQUIRE(wrappers == coldWrappers);
}

TEST_CASE("RuntimeBindings hoists invariant binding addresses out of native loops on Linux x86-64",
          "[runtime-bindings][codegen][B3]") {
	const llvm::Triple host(llvm::sys::getProcessTriple());
	if (!host.isOSLinux() || host.getArch() != llvm::Triple::x86_64) {
		SKIP("Native binding relocation placement is checked only on Linux x86-64");
	}
	requireBindingArtifactPlatform();
	std::array<uint64_t, 32> values {};
	RuntimeBindings bindings;
	auto state = bindings.bind<uint64_t>("loop/state", values.data());
	Options options;
	options.setOption("engine.backend", std::string("mlir"));
	options.setOption("mlir.enableMultithreading", false);
	options.setOption("mlir.optimizationLevel", 3);
	options.setOption("dump.before_llvm_optimization", true);
	options.setOption("dump.after_llvm_generation", true);
	options.setOption("dump.file", true);
	options.setRuntimeBindings(bindings);
	NautilusEngine engine(options);
	auto builder = engine.createModule();
	int wrappers = 0;
	builder.registerFunction<val<uint64_t>(val<int32_t>)>("execute", [state, &wrappers](val<int32_t> count) {
		++wrappers;
		val<uint64_t> checksum = cacheLiteral<uint64_t {0}>();
		for (val<int32_t> index = cacheLiteral<int32_t {0}>(); index < count;
		     index = index + cacheLiteral<int32_t {1}>()) {
			auto address = state.get();
			auto slot = index & cacheLiteral<int32_t {31}>();
			val<uint64_t> previous = address[slot];
			auto next = previous + static_cast<val<uint64_t>>(index) + cacheLiteral<uint64_t {1}>();
			address[slot] = next;
			checksum = checksum + next;
		}
		return checksum;
	});
	auto artifacts = builder.createArtifact();
	REQUIRE(wrappers > 0);
	const auto coldWrappers = wrappers;
	auto executable = artifact::loadBytecode(artifacts, options);
	auto expected = values;
	uint64_t checksum = 0;
	for (uint64_t index = 0; index < 97; ++index) {
		expected[index & 31] += index + 1;
		checksum += expected[index & 31];
	}
	REQUIRE(executable.getFunction<uint64_t(int32_t)>("execute")(97) == checksum);
	REQUIRE(wrappers == coldWrappers);
	REQUIRE(values == expected);

	const auto& symbol = bindings.entries().at("loop/state")->symbol;
	for (const bool optimized : {false, true}) {
		CAPTURE(optimized);
		const auto path = std::filesystem::temp_directory_path() / "dump" / "artifact-bytecode" /
		                  (optimized ? "after_llvm_generation.ll" : "before_llvm_optimization.ll");
		REQUIRE(std::filesystem::is_regular_file(path));
		llvm::LLVMContext context;
		llvm::SMDiagnostic diagnostic;
		auto module = llvm::parseIRFile(path.string(), diagnostic, context);
		REQUIRE(module != nullptr);
		auto* function = module->getFunction("execute");
		REQUIRE(function != nullptr);
		auto* global = module->getGlobalVariable(symbol);
		REQUIRE(global != nullptr);
		REQUIRE_FALSE(global->isConstant());
		llvm::DominatorTree dominators(*function);
		llvm::LoopInfo loops(dominators);
		REQUIRE_FALSE(loops.empty());
		std::size_t addresses = 0;
		for (const auto& block : *function) {
			for (const auto& instruction : block) {
				const auto* call = llvm::dyn_cast<llvm::CallInst>(&instruction);
				if (call != nullptr && call->isInlineAsm() && call->arg_size() == 1 &&
				    call->getArgOperand(0)->stripPointerCasts() == module->getGlobalVariable(symbol)) {
					++addresses;
					const auto* barrier = llvm::cast<llvm::InlineAsm>(call->getCalledOperand());
					REQUIRE(barrier->getAsmString().empty());
					REQUIRE(barrier->getConstraintString() == "=r,0");
					REQUIRE_FALSE(barrier->hasSideEffects());
					REQUIRE_FALSE(barrier->isAlignStack());
					REQUIRE_FALSE(barrier->canThrow());
					REQUIRE(call->getType()->isPointerTy());
					REQUIRE(call->getType() == call->getArgOperand(0)->getType());
					REQUIRE_FALSE(call->use_empty());
					if (!optimized) {
						REQUIRE(loops.getLoopFor(&block) != nullptr);
					}
				}
			}
		}
		REQUIRE(addresses == 1);
		std::filesystem::remove(path);
	}

	BindingObjectInspectionDirectory temporary;
	const auto objectPath = (temporary.path() / "runtime-binding-codegen.o").string();
	std::ofstream objectFile(objectPath, std::ios::binary | std::ios::trunc);
	REQUIRE(objectFile.is_open());
	objectFile.write(artifacts.object.data(), static_cast<std::streamsize>(artifacts.object.size()));
	objectFile.close();
	REQUIRE_FALSE(objectFile.fail());
	const std::string helperPath = NAUTILUS_BINDING_OBJECT_INSPECTION_PATH;
	REQUIRE(std::filesystem::is_regular_file(helperPath));
	std::string executionError;
	bool executionFailed = false;
	const auto exitCode = llvm::sys::ExecuteAndWait(helperPath, {helperPath, objectPath, symbol}, std::nullopt, {}, 60,
	                                                0, &executionError, &executionFailed);
	CAPTURE(helperPath, objectPath, symbol, exitCode);
	INFO(executionError);
	REQUIRE_FALSE(executionFailed);
	REQUIRE(exitCode == 0);
}

} // namespace nautilus::engine

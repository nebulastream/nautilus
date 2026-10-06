#include "nautilus/config.hpp"

#ifdef ENABLE_MLIR_BACKEND

#include "nautilus/Artifact.hpp"
#include "nautilus/common/ExecutableImage.hpp"
#include "nautilus/compiler/artifact/ArtifactCodec.hpp"
#include "nautilus/compiler/backends/mlir/MLIRArtifactValidation.hpp"
#include "nautilus/compiler/backends/mlir/intrinsics/MLIRBackendIntrinsic.hpp"
#include "nautilus/compiler/backends/mlir/intrinsics/MLIRMemoryIntrinsics.hpp"
#include "nautilus/exceptions/RuntimeException.hpp"
#include <algorithm>
#include <array>
#include <atomic>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>
#include <cstdint>
#include <limits>
#include <llvm/Support/MemoryBuffer.h>
#include <llvm/Support/SourceMgr.h>
#include <llvm/Support/raw_ostream.h>
#include <memory>
#include <mlir/Bytecode/BytecodeWriter.h>
#include <mlir/Dialect/LLVMIR/LLVMDialect.h>
#include <mlir/IR/Builders.h>
#include <mlir/IR/MLIRContext.h>
#include <mlir/IR/Verifier.h>
#include <mlir/Parser/Parser.h>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#ifdef ENABLE_TRACING
#include "nautilus/Engine.hpp"
#include "nautilus/function.hpp"
#endif

namespace nautilus::compiler::mlir {
namespace {

int32_t transportIncrement(int32_t value) noexcept {
	return value + 1;
}

std::string printModule(::mlir::ModuleOp module) {
	std::string text;
	llvm::raw_string_ostream output(text);
	module.print(output);
	output.flush();
	return text;
}

constexpr std::string_view LOWERED_MODULE = R"mlir(
module {
  llvm.mlir.global internal constant @constant(7 : i32) : i32
  llvm.func @transportIncrement(i32) -> i32
  llvm.func @execute(%value: i32) -> i32 {
    %result = llvm.call @transportIncrement(%value) : (i32) -> i32
    llvm.return %result : i32
  }
}
)mlir";

struct LoweredArtifactFixture {
	LoweredArtifactFixture() {
		context.disableMultithreading();
		context.loadDialect<::mlir::LLVM::LLVMDialect>();
		parse(LOWERED_MODULE);
	}

	void parse(std::string_view text) {
		module = ::mlir::parseSourceString<::mlir::ModuleOp>(llvm::StringRef(text.data(), text.size()), &context);
		REQUIRE(static_cast<bool>(module));
		REQUIRE(::mlir::succeeded(::mlir::verify(*module)));
	}

	void validate() {
		validateArtifactMLIRModule(*module, exportNames, externalSymbols, externalAddresses);
	}

	void requireFailure(const std::string& expected) {
		REQUIRE(::mlir::succeeded(::mlir::verify(*module)));
		const auto beforeModule = printModule(*module);
		const auto beforeExports = exportNames;
		const auto beforeSymbols = externalSymbols;
		const auto beforeAddresses = externalAddresses;
		REQUIRE_THROWS_WITH(validate(), Catch::Matchers::StartsWith(expected));
		REQUIRE(printModule(*module) == beforeModule);
		REQUIRE(exportNames == beforeExports);
		REQUIRE(externalSymbols == beforeSymbols);
		REQUIRE(externalAddresses == beforeAddresses);
	}

	void requireABIFailure(const std::string& expected) {
		REQUIRE(::mlir::succeeded(::mlir::verify(*module)));
		const auto beforeModule = printModule(*module);
		const auto beforeDescriptors = exports;
		REQUIRE_THROWS_WITH(validateArtifactExportABI(*module, exports), Catch::Matchers::StartsWith(expected));
		REQUIRE(printModule(*module) == beforeModule);
		REQUIRE(exports == beforeDescriptors);
	}

	::mlir::MLIRContext context;
	::mlir::OwningOpRef<::mlir::ModuleOp> module;
	std::vector<std::string> exportNames {"execute"};
	std::vector<std::string> externalSymbols {"transportIncrement"};
	std::vector<void*> externalAddresses {reinterpret_cast<void*>(&transportIncrement)};
	std::vector<artifact::ExportDescriptor> exports {{"execute", Type::i32, {Type::i32}, {}, 0, {}}};
};

int32_t fingerprintArithmetic(int32_t value) {
	return value + 2;
}

bool lowerFingerprintArithmetic(std::unique_ptr<::mlir::OpBuilder>& builder, const ir::CallOperation* call,
                                MLIRLoweringProvider::ValueFrame& frame, bool multiply) {
	const auto input = frame.getValue(call->getInputArguments()[0]->getIdentifier());
	auto two = ::mlir::LLVM::ConstantOp::create(*builder, builder->getUnknownLoc(), builder->getI32Type(),
	                                            builder->getI32IntegerAttr(2));
	if (multiply) {
		auto result = ::mlir::LLVM::MulOp::create(*builder, builder->getUnknownLoc(), input, two);
		frame.setValue(call->getIdentifier(), result);
	} else {
		auto result = ::mlir::LLVM::AddOp::create(*builder, builder->getUnknownLoc(), input, two);
		frame.setValue(call->getIdentifier(), result);
	}
	return true;
}

class ArithmeticIntrinsicPlugin : public MLIRIntrinsicPlugin {
public:
	explicit ArithmeticIntrinsicPlugin(bool multiply = false)
	    : multiply_(std::make_shared<std::atomic<bool>>(multiply)) {
	}

	void registerIntrinsics(MLIRIntrinsicManager& manager) override {
		++registrations;
		manager.addIntrinsic(reinterpret_cast<void*>(&fingerprintArithmetic),
		                     [multiply = multiply_](std::unique_ptr<::mlir::OpBuilder>& builder,
		                                            const ir::CallOperation* call,
		                                            MLIRLoweringProvider::ValueFrame& frame) {
			                     return lowerFingerprintArithmetic(builder, call, frame, multiply->load());
		                     });
	}

	void setMultiply(bool multiply) {
		multiply_->store(multiply);
	}

	int registrations = 0;

protected:
	std::shared_ptr<std::atomic<bool>> multiply_;
};

class IdentifiedArithmeticPlugin : public ArithmeticIntrinsicPlugin {
public:
	using ArithmeticIntrinsicPlugin::ArithmeticIntrinsicPlugin;
	using MLIRIntrinsicPlugin::cacheFingerprintForAddress;

	std::optional<std::string> cacheFingerprint() const override {
		auto identity = cacheFingerprintForAddress(reinterpret_cast<const void*>(&lowerFingerprintArithmetic));
		if (!identity) {
			return std::nullopt;
		}
		return *identity + (multiply_->load() ? ":multiply" : ":add");
	}
};

class EmptyFingerprintPlugin final : public ArithmeticIntrinsicPlugin {
public:
	std::optional<std::string> cacheFingerprint() const override {
		return std::string {};
	}
};

class ThrowingFingerprintPlugin final : public ArithmeticIntrinsicPlugin {
public:
	std::optional<std::string> cacheFingerprint() const override {
		throw std::runtime_error("intrinsic fingerprint failed");
	}
};

class PartialRegistrationPlugin final : public IdentifiedArithmeticPlugin {
public:
	void registerIntrinsics(MLIRIntrinsicManager& manager) override {
		ArithmeticIntrinsicPlugin::registerIntrinsics(manager);
		throw std::runtime_error("intrinsic registration failed");
	}
};

std::string framedIdentity(std::string_view identity) {
	return std::to_string(identity.size()) + ":" + std::string(identity);
}

void requireFingerprintImages() {
#ifndef __linux__
	SKIP("Executable image fingerprints require Linux ELF build IDs");
#else
	REQUIRE(common::locateExecutableAddress(reinterpret_cast<const void*>(&lowerFingerprintArithmetic)).has_value());
#endif
}

#ifdef ENABLE_TRACING

engine::Options transportOptions() {
	engine::Options options;
	options.setOption("engine.backend", std::string("mlir"));
	return options;
}

void resealDescriptor(artifact::ModuleArtifact& value) {
	value.descriptorDigest = artifact::detail::digest(artifact::detail::encodeDescriptor(value.descriptor));
}

std::string resealedEnvelope(const artifact::ModuleArtifact& original, std::string_view descriptorBytes) {
	const auto originalEnvelope = artifact::encode(original);
	artifact::detail::Reader reader(originalEnvelope);
	artifact::detail::Writer writer;
	writer.string(reader.string());
	writer.string(descriptorBytes);
	writer.string(artifact::detail::digest(descriptorBytes));
	writer.string(original.object);
	writer.string(original.bytecode);
	return writer.take();
}

std::size_t stringRecordOffset(std::string_view descriptorBytes, std::string_view value) {
	artifact::detail::Writer writer;
	writer.string(value);
	const auto record = writer.take();
	const auto offset = descriptorBytes.find(record);
	REQUIRE(offset != std::string_view::npos);
	REQUIRE(descriptorBytes.find(record, offset + 1) == std::string_view::npos);
	return offset;
}

struct DescriptorResealFixture {
	DescriptorResealFixture() {
#if !defined(__linux__) || !defined(__x86_64__)
		SKIP("Module artifact transport requires Linux x86-64 ELF build identities");
#else
		REQUIRE(artifact::isSupported());
#endif
		engine::NautilusEngine engine(options);
		auto module = engine.createModule();
		module.registerFunction<val<int32_t>(val<int32_t>)>(
		    "execute", [](val<int32_t> value) { return invoke(transportIncrement, value); });
		original = module.createArtifact();
		REQUIRE(original.descriptor.exports.size() == 1);
		const auto image = common::locateExecutableAddress(reinterpret_cast<const void*>(&transportIncrement));
		REQUIRE(image.has_value());
		const artifact::NativeImage nativeImage {image->buildId, image->loadOffset};
		const auto imported = std::ranges::find_if(original.descriptor.imports, [&](const auto& entry) {
			return entry.bytecodeImport && entry.image == nativeImage;
		});
		REQUIRE(imported != original.descriptor.imports.end());
		importIndex = static_cast<std::size_t>(imported - original.descriptor.imports.begin());
		REQUIRE_NOTHROW(artifact::detail::validateDescriptor(original));
		changed = original;
	}

	void requireResealedStructure() {
		resealDescriptor(changed);
		REQUIRE(changed.descriptorDigest != original.descriptorDigest);
		REQUIRE_NOTHROW(artifact::detail::validateDescriptor(changed));
		REQUIRE_NOTHROW(artifact::detail::validatePayload(changed.object, changed.descriptor.objectDigest));
		REQUIRE_NOTHROW(artifact::detail::validatePayload(changed.bytecode, changed.descriptor.bytecodeDigest));
		const auto decoded = artifact::decode(artifact::encode(changed));
		REQUIRE(decoded.descriptor == changed.descriptor);
		REQUIRE(decoded.descriptorDigest == changed.descriptorDigest);
		REQUIRE(decoded.object == changed.object);
		REQUIRE(decoded.bytecode == changed.bytecode);
	}

	void requireLoadFailure(const std::string& nativeExpected, const std::string& bytecodeExpected) {
		const auto before = artifact::encode(changed);
		CHECK_THROWS_WITH(artifact::loadNative(changed, options), Catch::Matchers::StartsWith(nativeExpected));
		REQUIRE(artifact::encode(changed) == before);
		CHECK_THROWS_WITH(artifact::loadBytecode(changed, options), Catch::Matchers::StartsWith(bytecodeExpected));
		REQUIRE(artifact::encode(changed) == before);
	}

	std::pair<std::vector<std::string>, std::vector<void*>> resolvedImports(bool bytecode = false) const {
		std::vector<std::string> symbols;
		std::vector<void*> addresses;
		for (const auto& imported : original.descriptor.imports) {
			if (bytecode && !imported.bytecodeImport) {
				continue;
			}
			symbols.push_back(imported.symbol);
			addresses.push_back(common::resolveExecutableAddress({imported.image.buildId, imported.image.loadOffset}));
			REQUIRE(addresses.back() != nullptr);
		}
		return {std::move(symbols), std::move(addresses)};
	}

	engine::Options options = transportOptions();
	artifact::ModuleArtifact original;
	artifact::ModuleArtifact changed;
	std::size_t importIndex = 0;
};

#endif

} // namespace

TEST_CASE_METHOD(LoweredArtifactFixture, "Artifact lowered MLIR validation is read-only on success",
                 "[artifact][mlir][validation][B1]") {
	SECTION("attribute-initialized global and native function import") {
		REQUIRE(static_cast<bool>(module->lookupSymbol<::mlir::LLVM::GlobalOp>("constant").getValueOrNull()));
	}
	SECTION("region-initialized global") {
		auto source = std::string(LOWERED_MODULE);
		source.insert(source.rfind('}'), R"mlir(
  llvm.mlir.global internal constant @region_constant() : i32 {
    %value = llvm.mlir.constant(9 : i32) : i32
    llvm.return %value : i32
  }
)mlir");
		parse(source);
	}
	SECTION("null pointer immediate") {
		parse(R"mlir(
module {
  llvm.func @execute() -> !llvm.ptr {
    %zero = llvm.mlir.constant(0 : i64) : i64
    %pointer = llvm.inttoptr %zero : i64 to !llvm.ptr
    llvm.return %pointer : !llvm.ptr
  }
}
)mlir");
		externalSymbols.clear();
		externalAddresses.clear();
	}
	SECTION("argument-derived pointer") {
		parse(R"mlir(
module {
  llvm.func @execute(%address: i64) -> !llvm.ptr {
    %pointer = llvm.inttoptr %address : i64 to !llvm.ptr
    llvm.return %pointer : !llvm.ptr
  }
}
)mlir");
		externalSymbols.clear();
		externalAddresses.clear();
	}
	const auto beforeModule = printModule(*module);
	const auto beforeExports = exportNames;
	const auto beforeSymbols = externalSymbols;
	const auto beforeAddresses = externalAddresses;
	REQUIRE_NOTHROW(validate());
	REQUIRE_NOTHROW(validate());
	REQUIRE(printModule(*module) == beforeModule);
	REQUIRE(exportNames == beforeExports);
	REQUIRE(externalSymbols == beforeSymbols);
	REQUIRE(externalAddresses == beforeAddresses);
}

TEST_CASE_METHOD(LoweredArtifactFixture, "Artifact lowered MLIR validation rejects invalid export manifests",
                 "[artifact][mlir][validation][B1]") {
	std::string expected;
	SECTION("no exports") {
		exportNames.clear();
		expected = "MLIR artifact has no exports";
	}
	SECTION("empty name") {
		exportNames[0].clear();
		expected = "MLIR artifact export manifest is invalid";
	}
	SECTION("embedded nul") {
		exportNames[0] = std::string("execute\0other", 13);
		expected = "MLIR artifact export manifest is invalid";
	}
	SECTION("duplicate export") {
		exportNames.push_back(exportNames[0]);
		expected = "MLIR artifact export manifest is invalid";
	}
	SECTION("missing export") {
		exportNames[0] = "missing";
		expected = "MLIR artifact module is missing export 'missing'";
	}
	SECTION("external declaration") {
		exportNames[0] = "transportIncrement";
		expected = "MLIR artifact module is missing export 'transportIncrement'";
	}
	SECTION("global instead of function") {
		exportNames[0] = "constant";
		expected = "MLIR artifact module is missing export 'constant'";
	}
	requireFailure(expected);
}

TEST_CASE_METHOD(LoweredArtifactFixture, "Artifact lowered MLIR validation rejects invalid import manifests",
                 "[artifact][mlir][validation][B1]") {
	std::string expected;
	SECTION("more names than addresses") {
		externalAddresses.clear();
		expected = "MLIR artifact external symbol vectors differ in size";
	}
	SECTION("more addresses than names") {
		externalSymbols.clear();
		expected = "MLIR artifact external symbol vectors differ in size";
	}
	SECTION("empty import name") {
		externalSymbols[0].clear();
		expected = "MLIR artifact external symbol manifest is invalid";
	}
	SECTION("embedded nul import name") {
		externalSymbols[0].push_back('\0');
		expected = "MLIR artifact external symbol manifest is invalid";
	}
	SECTION("duplicate import") {
		externalSymbols.push_back(externalSymbols[0]);
		externalAddresses.push_back(externalAddresses[0]);
		expected = "MLIR artifact external symbol manifest is invalid";
	}
	SECTION("null address") {
		externalAddresses[0] = nullptr;
		expected = "MLIR artifact external symbol manifest is invalid";
	}
	SECTION("unlisted external declaration") {
		externalSymbols.clear();
		externalAddresses.clear();
		expected = "MLIR artifact contains an undeclared external function";
	}
	SECTION("manifest entry without a module declaration") {
		externalSymbols.push_back("missing");
		externalAddresses.push_back(externalAddresses[0]);
		expected = "MLIR artifact external function is not declared by the module";
	}
	SECTION("defined function is not an import") {
		externalSymbols.push_back("execute");
		externalAddresses.push_back(externalAddresses[0]);
		expected = "MLIR artifact external function is not declared by the module";
	}
	SECTION("global is not a function import") {
		externalSymbols.push_back("constant");
		externalAddresses.push_back(externalAddresses[0]);
		expected = "MLIR artifact external function is not declared by the module";
	}
	requireFailure(expected);
}

TEST_CASE_METHOD(LoweredArtifactFixture, "Artifact lowered MLIR validation rejects external globals and initializers",
                 "[artifact][mlir][validation][B1]") {
	std::string extra;
	std::string expected;
	SECTION("external global") {
		extra = "llvm.mlir.global external @external() : i32\n";
		expected = "MLIR artifacts do not support external globals";
	}
	SECTION("constructor") {
		extra = "llvm.func @initialize() { llvm.return }\n"
		        "llvm.mlir.global_ctors ctors = [@initialize], priorities = [0 : i32], data = [#llvm.zero]\n";
		expected = "MLIR artifacts do not support module initializers or finalizers";
	}
	SECTION("destructor") {
		extra = "llvm.func @finalize() { llvm.return }\n"
		        "llvm.mlir.global_dtors dtors = [@finalize], priorities = [0 : i32], data = [#llvm.zero]\n";
		expected = "MLIR artifacts do not support module initializers or finalizers";
	}
	auto source = std::string(LOWERED_MODULE);
	source.insert(source.rfind('}'), extra);
	parse(source);
	requireFailure(expected);
}

TEST_CASE_METHOD(LoweredArtifactFixture, "Artifact lowered MLIR validation orders structural failures",
                 "[artifact][mlir][validation][B1]") {
	SECTION("missing module") {
		REQUIRE_THROWS_WITH(validateArtifactMLIRModule({}, {}, {}, {}), "MLIR artifact module is missing");
	}
	SECTION("exports precede import vector lengths") {
		exportNames.clear();
		externalAddresses.clear();
		requireFailure("MLIR artifact has no exports");
	}
	SECTION("all export names are validated before symbol lookup") {
		exportNames = {"missing", ""};
		requireFailure("MLIR artifact export manifest is invalid");
	}
	SECTION("every export is checked before import validation") {
		exportNames.push_back("missing");
		externalAddresses.clear();
		requireFailure("MLIR artifact module is missing export 'missing'");
	}
	SECTION("import vector sizes precede invalid names") {
		externalSymbols[0].clear();
		externalAddresses.clear();
		requireFailure("MLIR artifact external symbol vectors differ in size");
	}
	SECTION("import validity precedes declarations") {
		externalSymbols[0] = "missing";
		externalAddresses[0] = nullptr;
		requireFailure("MLIR artifact external symbol manifest is invalid");
	}
	SECTION("unlisted declarations precede surplus manifest entries") {
		externalSymbols[0] = "missing";
		requireFailure("MLIR artifact contains an undeclared external function");
	}
}

TEST_CASE_METHOD(LoweredArtifactFixture, "Artifact export ABI validation captures lowered attributes read-only",
                 "[artifact][mlir][abi][B1]") {
	auto function = module->lookupSymbol<::mlir::LLVM::LLVMFuncOp>("execute");
	function->setAttr("artifact.z", ::mlir::StringAttr::get(&context, "last"));
	function->setAttr("artifact.a", ::mlir::StringAttr::get(&context, "first"));
	const auto beforeModule = printModule(*module);
	const auto beforeDescriptors = exports;
	const auto abis = validateArtifactExportABI(*module, exports);
	REQUIRE(abis.size() == 1);
	REQUIRE(abis[0].starts_with("!llvm.func<i32 (i32)> convention=0 "));
	REQUIRE(abis[0].find(" function_type=!llvm.func<i32 (i32)>") != std::string::npos);
	REQUIRE(abis[0].find(" sym_name=\"execute\"") != std::string::npos);
	const auto first = abis[0].find(" artifact.a=\"first\"");
	const auto last = abis[0].find(" artifact.z=\"last\"");
	REQUIRE(first != std::string::npos);
	REQUIRE(last != std::string::npos);
	REQUIRE(first < last);
	REQUIRE(validateArtifactExportABI(*module, exports) == abis);
	REQUIRE(printModule(*module) == beforeModule);
	REQUIRE(exports == beforeDescriptors);
	exports[0].loweredABI = abis[0];
	REQUIRE(validateArtifactExportABI(*module, exports) == abis);
	function->setAttr("artifact.a", ::mlir::StringAttr::get(&context, "changed"));
	requireABIFailure("MLIR artifact export ABI attributes mismatch: execute");
}

TEST_CASE_METHOD(LoweredArtifactFixture, "Artifact export ABI validation matches every scalar stamp and void",
                 "[artifact][mlir][abi][B1]") {
	const std::array<std::pair<Type, std::string_view>, 12> types {{{Type::b, "i1"},
	                                                                {Type::i8, "i8"},
	                                                                {Type::ui8, "i8"},
	                                                                {Type::i16, "i16"},
	                                                                {Type::ui16, "i16"},
	                                                                {Type::i32, "i32"},
	                                                                {Type::ui32, "i32"},
	                                                                {Type::i64, "i64"},
	                                                                {Type::ui64, "i64"},
	                                                                {Type::f32, "f32"},
	                                                                {Type::f64, "f64"},
	                                                                {Type::ptr, "!llvm.ptr"}}};
	for (const auto& [stamp, type] : types) {
		CAPTURE(stamp, type);
		const auto text = std::string(type);
		std::string extension;
		if (stamp == Type::i8 || stamp == Type::i16) {
			extension = " {llvm.signext}";
		} else if (stamp == Type::b || stamp == Type::ui8 || stamp == Type::ui16) {
			extension = " {llvm.zeroext}";
		}
		const auto result = extension.empty() ? text : "(" + text + extension + ")";
		parse("module { llvm.func @identity(%value: " + text + extension + ") -> " + result +
		      " { llvm.return %value : " + text + " } }");
		exports = {{"identity", stamp, {stamp}, {}, 0, {}}};
		const auto before = printModule(*module);
		const auto abis = validateArtifactExportABI(*module, exports);
		REQUIRE(abis.size() == 1);
		const auto abiType = stamp == Type::ptr ? "ptr" : text;
		REQUIRE(abis[0].starts_with("!llvm.func<" + abiType + " (" + abiType + ")> convention=0 "));
		exports[0].loweredABI = abis[0];
		REQUIRE(validateArtifactExportABI(*module, exports) == abis);
		REQUIRE(printModule(*module) == before);
	}
	parse(R"mlir(
module {
  llvm.func @store(%pointer: !llvm.ptr, %value: i64) {
    llvm.store %value, %pointer : i64, !llvm.ptr
    llvm.return
  }
}
)mlir");
	exports = {{"store", Type::v, {Type::ptr, Type::i64}, {}, 0, {}}};
	const auto abis = validateArtifactExportABI(*module, exports);
	REQUIRE(abis.size() == 1);
	REQUIRE(abis[0].starts_with("!llvm.func<void (ptr, i64)> convention=0 "));
}

TEST_CASE_METHOD(LoweredArtifactFixture, "Artifact export ABI validation seals narrow extension attributes and order",
                 "[artifact][mlir][abi][B1]") {
	parse(R"mlir(
module {
  llvm.func @signed_byte(%value: i8 {llvm.signext}) -> (i8 {llvm.signext}) {
    llvm.return %value : i8
  }
  llvm.func @unsigned_byte(%value: i8 {llvm.zeroext}) -> (i8 {llvm.zeroext}) {
    llvm.return %value : i8
  }
}
)mlir");
	exports = {{"unsigned_byte", Type::ui8, {Type::ui8}, {}, 0, {}}, {"signed_byte", Type::i8, {Type::i8}, {}, 0, {}}};
	const auto before = printModule(*module);
	const auto abis = validateArtifactExportABI(*module, exports);
	REQUIRE(abis.size() == 2);
	REQUIRE(abis[0].find(" arg_attrs=[{llvm.zeroext}]") != std::string::npos);
	REQUIRE(abis[0].find(" res_attrs=[{llvm.zeroext}]") != std::string::npos);
	REQUIRE(abis[0].find(" sym_name=\"unsigned_byte\"") != std::string::npos);
	REQUIRE(abis[1].find(" arg_attrs=[{llvm.signext}]") != std::string::npos);
	REQUIRE(abis[1].find(" res_attrs=[{llvm.signext}]") != std::string::npos);
	REQUIRE(abis[1].find(" sym_name=\"signed_byte\"") != std::string::npos);
	REQUIRE(abis[0] != abis[1]);
	REQUIRE(printModule(*module) == before);
	for (std::size_t index = 0; index < exports.size(); ++index) {
		exports[index].loweredABI = abis[index];
	}
	REQUIRE(validateArtifactExportABI(*module, exports) == abis);
	exports[0].loweredABI = abis[1];
	requireABIFailure("MLIR artifact export ABI attributes mismatch: unsigned_byte");
}

TEST_CASE_METHOD(LoweredArtifactFixture, "Artifact export ABI validation rejects incompatible signatures",
                 "[artifact][mlir][abi][B1]") {
	std::string expected = "MLIR artifact export signature mismatch: execute";
	SECTION("missing definition") {
		exports[0].name = "missing";
		expected = "MLIR artifact has no defined export 'missing'";
	}
	SECTION("declaration is not a definition") {
		exports[0].name = "transportIncrement";
		expected = "MLIR artifact has no defined export 'transportIncrement'";
	}
	SECTION("global is not a definition") {
		exports[0].name = "constant";
		expected = "MLIR artifact has no defined export 'constant'";
	}
	SECTION("wrong result stamp") {
		exports[0].returnType = Type::f64;
	}
	SECTION("invalid result stamp") {
		exports[0].returnType = static_cast<Type>(std::numeric_limits<uint8_t>::max());
	}
	SECTION("too few arguments") {
		exports[0].argumentTypes.clear();
	}
	SECTION("too many arguments") {
		exports[0].argumentTypes.push_back(Type::ptr);
	}
	SECTION("wrong argument stamp") {
		exports[0].argumentTypes[0] = Type::i64;
		expected = "MLIR artifact export argument signature mismatch: execute";
	}
	SECTION("void argument stamp") {
		exports[0].argumentTypes[0] = Type::v;
		expected = "MLIR artifact export argument signature mismatch: execute";
	}
	SECTION("calling convention") {
		exports[0].callingConvention = static_cast<uint32_t>(::mlir::LLVM::cconv::CConv::Fast);
	}
	SECTION("variadic function") {
		parse("module { llvm.func @execute(%value: i32, ...) -> i32 { llvm.return %value : i32 } }");
	}
	SECTION("changed ABI string") {
		exports[0].loweredABI = validateArtifactExportABI(*module, exports)[0] + " changed";
		expected = "MLIR artifact export ABI attributes mismatch: execute";
	}
	requireABIFailure(expected);
}

TEST_CASE_METHOD(LoweredArtifactFixture, "Artifact export ABI validation rejects incompatible ABI attributes",
                 "[artifact][mlir][abi][B1]") {
	SECTION("narrow signed export cannot use zero extension") {
		parse("module { llvm.func @execute(%value: i8 {llvm.zeroext}) -> (i8 {llvm.zeroext}) { llvm.return %value : i8 "
		      "} }");
		exports = {{"execute", Type::i8, {Type::i8}, {}, 0, {}}};
		requireABIFailure("MLIR artifact export has unsupported C ABI attributes");
	}
	SECTION("narrow export requires extension") {
		parse("module { llvm.func @execute(%value: i8) -> i8 { llvm.return %value : i8 } }");
		exports = {{"execute", Type::i8, {Type::i8}, {}, 0, {}}};
		requireABIFailure("MLIR artifact export is missing a narrow C ABI extension");
	}
	SECTION("unsupported pointer semantics") {
		parse("module { llvm.func @execute(%value: !llvm.ptr {llvm.noalias}) -> !llvm.ptr { llvm.return %value : "
		      "!llvm.ptr } }");
		exports = {{"execute", Type::ptr, {Type::ptr}, {}, 0, {}}};
		requireABIFailure("MLIR artifact export has unsupported C ABI attributes");
	}
}

TEST_CASE("Artifact intrinsic fingerprints reject non-executable addresses", "[artifact][mlir][intrinsics][B1]") {
	requireFingerprintImages();
	const auto address = reinterpret_cast<const void*>(static_cast<void (*)()>(&RegisterMLIRMemoryIntrinsicPlugin));
	const auto image = common::locateExecutableAddress(address);
	REQUIRE(image.has_value());
	const auto expected = image->buildId + ":" + std::to_string(image->loadOffset);
	REQUIRE(IdentifiedArithmeticPlugin::cacheFingerprintForAddress(address) == expected);
	REQUIRE(IdentifiedArithmeticPlugin::cacheFingerprintForAddress(address) == expected);
	REQUIRE(common::resolveExecutableAddress(*image) == address);
	REQUIRE_FALSE(IdentifiedArithmeticPlugin::cacheFingerprintForAddress(nullptr).has_value());
	int32_t data = 42;
	REQUIRE_FALSE(IdentifiedArithmeticPlugin::cacheFingerprintForAddress(&data).has_value());
	REQUIRE(data == 42);
}

TEST_CASE("Artifact intrinsic fingerprints track local plugin state and registration order",
          "[artifact][mlir][intrinsics][B1]") {
	requireFingerprintImages();
	MLIRIntrinsicPluginRegistry forward;
	MLIRIntrinsicPluginRegistry reverse;
	REQUIRE(forward.cacheFingerprint() == std::string {});
	forward.addPlugin(nullptr);
	REQUIRE(forward.cacheFingerprint() == std::string {});
	auto add = std::make_shared<IdentifiedArithmeticPlugin>(false);
	auto multiply = std::make_shared<IdentifiedArithmeticPlugin>(true);
	const auto addIdentity = add->cacheFingerprint();
	const auto multiplyIdentity = multiply->cacheFingerprint();
	REQUIRE(addIdentity.has_value());
	REQUIRE(multiplyIdentity.has_value());
	REQUIRE(addIdentity != multiplyIdentity);
	IdentifiedArithmeticPlugin equivalent(false);
	REQUIRE(equivalent.cacheFingerprint() == addIdentity);
	forward.addPlugin(add);
	REQUIRE(forward.cacheFingerprint() == framedIdentity(*addIdentity));
	add->setMultiply(true);
	REQUIRE(forward.cacheFingerprint() == framedIdentity(*multiplyIdentity));
	add->setMultiply(false);
	forward.addPlugin(multiply);
	const auto ordered = framedIdentity(*addIdentity) + framedIdentity(*multiplyIdentity);
	REQUIRE(forward.cacheFingerprint() == ordered);
	reverse.addPlugin(std::make_shared<IdentifiedArithmeticPlugin>(true));
	reverse.addPlugin(std::make_shared<IdentifiedArithmeticPlugin>(false));
	REQUIRE(reverse.cacheFingerprint() == framedIdentity(*multiplyIdentity) + framedIdentity(*addIdentity));
	REQUIRE(reverse.cacheFingerprint() != forward.cacheFingerprint());
	const auto id = ir::IntrinsicRegistry::instance().lookup(reinterpret_cast<void*>(&fingerprintArithmetic));
	REQUIRE(id != ir::IntrinsicId::None);
	MLIRIntrinsicManager first;
	MLIRIntrinsicManager second;
	forward.registerAllIntrinsics(first);
	forward.registerAllIntrinsics(second);
	REQUIRE(first.getIntrinsic(id).has_value());
	REQUIRE(second.getIntrinsic(id).has_value());
	REQUIRE_FALSE(first.getIntrinsic(ir::IntrinsicId::None).has_value());
	REQUIRE(add->registrations == 1);
	REQUIRE(multiply->registrations == 1);
	REQUIRE(forward.cacheFingerprint() == ordered);
	forward.addPlugin(nullptr);
	REQUIRE(forward.cacheFingerprint() == ordered);
	REQUIRE(ir::IntrinsicRegistry::instance().lookup(reinterpret_cast<void*>(&fingerprintArithmetic)) == id);
}

TEST_CASE("Artifact intrinsic fingerprints fail closed for absent empty or throwing identities",
          "[artifact][mlir][intrinsics][B1]") {
	MLIRIntrinsicPluginRegistry registry;
	std::shared_ptr<ArithmeticIntrinsicPlugin> plugin;
	SECTION("absent identity") {
		plugin = std::make_shared<ArithmeticIntrinsicPlugin>();
		REQUIRE_FALSE(plugin->cacheFingerprint().has_value());
	}
	SECTION("empty identity") {
		plugin = std::make_shared<EmptyFingerprintPlugin>();
		REQUIRE(plugin->cacheFingerprint() == std::string {});
	}
	SECTION("throwing identity") {
		plugin = std::make_shared<ThrowingFingerprintPlugin>();
		REQUIRE_THROWS_WITH(plugin->cacheFingerprint(), "intrinsic fingerprint failed");
	}
	REQUIRE(registry.cacheFingerprint().has_value());
	registry.addPlugin(plugin);
	REQUIRE_FALSE(registry.cacheFingerprint().has_value());
	const auto id = ir::IntrinsicRegistry::instance().lookup(reinterpret_cast<void*>(&fingerprintArithmetic));
	REQUIRE(id != ir::IntrinsicId::None);
	MLIRIntrinsicManager manager;
	registry.registerAllIntrinsics(manager);
	REQUIRE(manager.getIntrinsic(id).has_value());
	REQUIRE(plugin->registrations == 1);
	registry.addPlugin(nullptr);
	REQUIRE_FALSE(registry.cacheFingerprint().has_value());
	MLIRIntrinsicPluginRegistry independent;
	REQUIRE(independent.cacheFingerprint() == std::string {});
}

TEST_CASE("Artifact intrinsic fingerprints remain unavailable after partial local registration",
          "[artifact][mlir][intrinsics][B1]") {
	requireFingerprintImages();
	MLIRIntrinsicPluginRegistry registry;
	auto partial = std::make_shared<PartialRegistrationPlugin>();
	REQUIRE(partial->cacheFingerprint().has_value());
	REQUIRE_THROWS_WITH(registry.addPlugin(partial), "intrinsic registration failed");
	REQUIRE(partial->registrations == 1);
	REQUIRE_FALSE(registry.cacheFingerprint().has_value());
	const auto id = ir::IntrinsicRegistry::instance().lookup(reinterpret_cast<void*>(&fingerprintArithmetic));
	REQUIRE(id != ir::IntrinsicId::None);
	MLIRIntrinsicManager manager;
	registry.registerAllIntrinsics(manager);
	REQUIRE(manager.getIntrinsic(id).has_value());
	registry.addPlugin(std::make_shared<IdentifiedArithmeticPlugin>());
	registry.addPlugin(nullptr);
	REQUIRE_FALSE(registry.cacheFingerprint().has_value());
	MLIRIntrinsicPluginRegistry independent;
	independent.addPlugin(std::make_shared<IdentifiedArithmeticPlugin>());
	REQUIRE(independent.cacheFingerprint().has_value());
}

#ifdef ENABLE_TRACING

TEST_CASE_METHOD(DescriptorResealFixture, "Internal artifact descriptor sealing preserves a loadable transport",
                 "[artifact][mlir][transport][codec][B1]") {
	resealDescriptor(changed);
	REQUIRE(changed.descriptorDigest == original.descriptorDigest);
	const auto descriptorBytes = artifact::detail::encodeDescriptor(changed.descriptor);
	REQUIRE(artifact::detail::decodeDescriptor(descriptorBytes) == original.descriptor);
	const auto encoded = artifact::encode(changed);
	const auto decoded = artifact::decode(encoded);
	REQUIRE(artifact::encode(decoded) == encoded);
	for (const bool bytecode : {false, true}) {
		CAPTURE(bytecode);
		auto loaded = bytecode ? artifact::loadBytecode(decoded, options) : artifact::loadNative(decoded, options);
		REQUIRE(loaded.getFunction<int32_t(int32_t)>("execute")(-7) == transportIncrement(-7));
		REQUIRE(artifact::encode(changed) == encoded);
	}
}

TEST_CASE_METHOD(DescriptorResealFixture, "Artifact native object symbol validation is read-only on success",
                 "[artifact][mlir][validation][B1]") {
	auto [symbols, addresses] = resolvedImports();
	const std::vector<std::string> exports {"execute"};
	const auto beforeObject = original.object;
	const auto beforeSymbols = symbols;
	const auto beforeAddresses = addresses;
	const auto inspected = inspectArtifactObject(original.object);
	REQUIRE(std::ranges::find(inspected.definedFunctionSymbols, "execute") != inspected.definedFunctionSymbols.end());
	REQUIRE(std::ranges::find(inspected.undefinedSymbols, original.descriptor.imports[importIndex].symbol) !=
	        inspected.undefinedSymbols.end());
	REQUIRE_NOTHROW(validateArtifactObjectSymbols(original.object, exports, symbols, addresses));
	REQUIRE_NOTHROW(validateArtifactObjectSymbols(original.object, exports, symbols, addresses));
	const auto repeated = inspectArtifactObject(original.object);
	REQUIRE(repeated.definedFunctionSymbols == inspected.definedFunctionSymbols);
	REQUIRE(repeated.undefinedSymbols == inspected.undefinedSymbols);
	REQUIRE(original.object == beforeObject);
	REQUIRE(symbols == beforeSymbols);
	REQUIRE(addresses == beforeAddresses);
}

TEST_CASE_METHOD(DescriptorResealFixture, "Artifact native object symbol validation rejects malformed manifests",
                 "[artifact][mlir][validation][B1]") {
	auto [symbols, addresses] = resolvedImports();
	std::vector<std::string> exports {"execute"};
	std::string expected;
	SECTION("missing export list") {
		exports.clear();
		expected = "MLIR artifact has no exports";
	}
	SECTION("empty export name") {
		exports[0].clear();
		expected = "MLIR artifact export manifest is invalid";
	}
	SECTION("duplicate export") {
		exports.push_back(exports[0]);
		expected = "MLIR artifact export manifest is invalid";
	}
	SECTION("nonexistent export") {
		exports[0] = "missing";
		expected = "MLIR artifact object is missing defined export 'missing'";
	}
	SECTION("external function is not an export") {
		exports[0] = original.descriptor.imports[importIndex].symbol;
		expected = "MLIR artifact object is missing defined export";
	}
	SECTION("unequal import vectors") {
		addresses.pop_back();
		expected = "MLIR artifact external symbol vectors differ in size";
	}
	SECTION("empty import name") {
		symbols[0].clear();
		expected = "MLIR artifact external symbol manifest is invalid";
	}
	SECTION("duplicate import") {
		symbols.push_back(symbols[0]);
		addresses.push_back(addresses[0]);
		expected = "MLIR artifact external symbol manifest is invalid";
	}
	SECTION("null import address") {
		addresses[0] = nullptr;
		expected = "MLIR artifact external symbol manifest is invalid";
	}
	SECTION("missing object import") {
		symbols.erase(symbols.begin() + importIndex);
		addresses.erase(addresses.begin() + importIndex);
		expected = "MLIR artifact object contains undeclared import";
	}
	SECTION("import conflicts with exported function") {
		symbols.push_back("execute");
		addresses.push_back(addresses[0]);
		expected = "MLIR artifact object is missing defined export 'execute'";
	}
	const auto beforeObject = original.object;
	const auto beforeExports = exports;
	const auto beforeSymbols = symbols;
	const auto beforeAddresses = addresses;
	REQUIRE_THROWS_WITH(validateArtifactObjectSymbols(original.object, exports, symbols, addresses),
	                    Catch::Matchers::StartsWith(expected));
	REQUIRE(original.object == beforeObject);
	REQUIRE(exports == beforeExports);
	REQUIRE(symbols == beforeSymbols);
	REQUIRE(addresses == beforeAddresses);
}

TEST_CASE_METHOD(DescriptorResealFixture, "Artifact built-in memory intrinsic identity records its executable image",
                 "[artifact][mlir][intrinsics][B1]") {
	const auto image = common::locateExecutableAddress(
	    reinterpret_cast<const void*>(static_cast<void (*)()>(&RegisterMLIRMemoryIntrinsicPlugin)));
	REQUIRE(image.has_value());
	const auto identity = image->buildId + ":" + std::to_string(image->loadOffset);
	REQUIRE(original.descriptor.compatibility.extensionFingerprint.find(framedIdentity(identity)) != std::string::npos);
}

TEST_CASE_METHOD(DescriptorResealFixture, "Resealed artifact descriptors still reject incompatible implementations",
                 "[artifact][mlir][transport][validation][B1]") {
	auto& compatibility = changed.descriptor.compatibility;
	SECTION("compiler build image") {
		compatibility.compilerImage.buildId += "00";
	}
	SECTION("compiler entry offset") {
		++compatibility.compilerImage.loadOffset;
	}
	SECTION("producer build image") {
		compatibility.producerImage.buildId += "00";
	}
	SECTION("producer entry offset") {
		++compatibility.producerImage.loadOffset;
	}
	SECTION("LLVM version") {
		compatibility.llvmVersion += "-incompatible";
	}
	SECTION("target triple") {
		compatibility.targetTriple += "-incompatible";
	}
	SECTION("target CPU") {
		compatibility.cpu += "-incompatible";
	}
	SECTION("target features") {
		compatibility.features += ",+incompatible";
	}
	SECTION("target data layout") {
		compatibility.dataLayout += "-incompatible";
	}
	SECTION("pointer width") {
		++compatibility.pointerSize;
	}
	SECTION("endianness") {
		compatibility.littleEndian = !compatibility.littleEndian;
	}
	SECTION("producer options") {
		auto otherOptions = options;
		otherOptions.setOption("optimizationLevel", 0);
		compatibility.optionsDigest = artifact::detail::optionsDigest(otherOptions);
		REQUIRE(compatibility.optionsDigest != original.descriptor.compatibility.optionsDigest);
	}
	SECTION("intrinsic implementation identity") {
		compatibility.extensionFingerprint += framedIdentity("changed-implementation");
	}
	requireResealedStructure();
	REQUIRE(changed.object == original.object);
	REQUIRE(changed.bytecode == original.bytecode);
	requireLoadFailure("Artifact compiler, target, options or extension compatibility mismatch",
	                   "Artifact compiler, target, options or extension compatibility mismatch");
}

TEST_CASE_METHOD(DescriptorResealFixture, "Resealed artifact descriptors cannot omit or rename native imports",
                 "[artifact][mlir][transport][validation][B1]") {
	SECTION("missing import") {
		changed.descriptor.imports.erase(changed.descriptor.imports.begin() + importIndex);
	}
	SECTION("renamed import") {
		changed.descriptor.imports[importIndex].symbol += "_renamed";
	}
	requireResealedStructure();
	requireLoadFailure("MLIR artifact object contains undeclared import",
	                   "MLIR artifact contains an undeclared external function");
}

TEST_CASE_METHOD(DescriptorResealFixture,
                 "Resealed artifact descriptors cannot resolve absent native images or offsets",
                 "[artifact][mlir][transport][validation][B1]") {
	auto& image = changed.descriptor.imports[importIndex].image;
	SECTION("unloaded build image") {
		image.buildId += "00";
	}
	SECTION("unmapped executable offset") {
		image.loadOffset = std::numeric_limits<uint64_t>::max();
	}
	REQUIRE(common::resolveExecutableAddress({image.buildId, image.loadOffset}) == nullptr);
	requireResealedStructure();
	requireLoadFailure("Artifact native import cannot be resolved uniquely",
	                   "Artifact native import cannot be resolved uniquely");
}

TEST_CASE_METHOD(DescriptorResealFixture, "Resealed artifact descriptors cannot hide a bytecode import as an auxiliary",
                 "[artifact][mlir][transport][validation][B1]") {
	changed.descriptor.imports[importIndex].bytecodeImport = false;
	requireResealedStructure();
	REQUIRE_THROWS_WITH(artifact::loadBytecode(changed, options),
	                    Catch::Matchers::StartsWith("MLIR artifact contains an undeclared external function"));
}

TEST_CASE_METHOD(DescriptorResealFixture, "Resealed artifact descriptors reject duplicate export and import records",
                 "[artifact][mlir][transport][validation][B1]") {
	std::string expected;
	SECTION("duplicate export") {
		changed.descriptor.exports.push_back(changed.descriptor.exports.front());
		expected = "Invalid artifact export descriptor";
	}
	SECTION("duplicate import") {
		changed.descriptor.imports.push_back(changed.descriptor.imports[importIndex]);
		expected = "Invalid artifact import descriptor";
	}
	SECTION("import conflicts with export") {
		changed.descriptor.imports[importIndex].symbol = changed.descriptor.exports.front().name;
		expected = "Invalid artifact import descriptor";
	}
	resealDescriptor(changed);
	REQUIRE(changed.descriptorDigest != original.descriptorDigest);
	REQUIRE(changed.descriptorDigest ==
	        artifact::detail::digest(artifact::detail::encodeDescriptor(changed.descriptor)));
	REQUIRE_THROWS_WITH(artifact::detail::validateDescriptor(changed), expected);
	REQUIRE_THROWS_WITH(artifact::loadNative(changed, options), expected);
	REQUIRE_THROWS_WITH(artifact::loadBytecode(changed, options), expected);
	const auto descriptorBytes = artifact::detail::encodeDescriptor(changed.descriptor);
	REQUIRE_THROWS_WITH(artifact::detail::decodeDescriptor(descriptorBytes), expected);
	REQUIRE_THROWS_WITH(artifact::decode(resealedEnvelope(original, descriptorBytes)), expected);
}

TEST_CASE_METHOD(DescriptorResealFixture, "Resealed bytecode descriptors still reject altered export ABI records",
                 "[artifact][mlir][transport][abi][B1]") {
	auto& exported = changed.descriptor.exports.front();
	std::string expected = "MLIR artifact export signature mismatch: execute";
	SECTION("return type") {
		exported.returnType = Type::i64;
	}
	SECTION("argument type") {
		exported.argumentTypes[0] = Type::i64;
		expected = "MLIR artifact export argument signature mismatch: execute";
	}
	SECTION("argument count") {
		exported.argumentTypes.push_back(Type::ptr);
	}
	SECTION("lowered ABI attributes") {
		exported.loweredABI += " altered";
		expected = "MLIR artifact export ABI attributes mismatch: execute";
	}
	SECTION("module manifest") {
		changed.descriptor.moduleManifest += " altered";
		expected = "MLIR artifact module manifest mismatch";
	}
	requireResealedStructure();
	REQUIRE(changed.bytecode == original.bytecode);
	const auto before = artifact::encode(changed);
	REQUIRE_THROWS_WITH(artifact::loadBytecode(changed, options), Catch::Matchers::StartsWith(expected));
	REQUIRE(artifact::encode(changed) == before);
}

TEST_CASE_METHOD(DescriptorResealFixture, "Recomputed descriptor checksums do not excuse malformed record lengths",
                 "[artifact][mlir][transport][codec][B1]") {
	auto descriptorBytes = artifact::detail::encodeDescriptor(original.descriptor);
	std::size_t offset = 0;
	artifact::detail::Writer replacement;
	std::string expected;
	SECTION("export name exceeds the string limit") {
		offset = stringRecordOffset(descriptorBytes, original.descriptor.exports.front().name);
		replacement.u64(artifact::detail::MAX_ARTIFACT_SIZE + 1);
		expected = "Invalid artifact string length";
	}
	SECTION("import name exceeds remaining bytes") {
		offset = stringRecordOffset(descriptorBytes, original.descriptor.imports[importIndex].symbol);
		replacement.u64(descriptorBytes.size() + 1);
		expected = "Invalid artifact string length";
	}
	SECTION("module manifest length overflows") {
		offset = stringRecordOffset(descriptorBytes, original.descriptor.moduleManifest);
		replacement.u64(std::numeric_limits<uint64_t>::max());
		expected = "Invalid artifact string length";
	}
	SECTION("export record count exceeds the limit") {
		const auto nameOffset = stringRecordOffset(descriptorBytes, original.descriptor.exports.front().name);
		REQUIRE(nameOffset >= sizeof(uint32_t));
		offset = nameOffset - sizeof(uint32_t);
		replacement.u32(artifact::detail::MAX_RECORD_COUNT + 1);
		expected = "Invalid artifact record count";
	}
	SECTION("argument record count exceeds remaining bytes") {
		REQUIRE(descriptorBytes.size() < artifact::detail::MAX_RECORD_COUNT);
		const auto& exported = original.descriptor.exports.front();
		offset = stringRecordOffset(descriptorBytes, exported.name) + sizeof(uint64_t) + exported.name.size() +
		         sizeof(uint8_t);
		replacement.u32(static_cast<uint32_t>(descriptorBytes.size()));
		expected = "Invalid artifact record count";
	}
	SECTION("attribute record count exceeds the limit") {
		const auto& exported = original.descriptor.exports.front();
		offset = stringRecordOffset(descriptorBytes, exported.name) + sizeof(uint64_t) + exported.name.size() +
		         sizeof(uint8_t) + sizeof(uint32_t) + exported.argumentTypes.size();
		replacement.u32(artifact::detail::MAX_RECORD_COUNT + 1);
		expected = "Invalid artifact record count";
	}
	SECTION("import record count exceeds the limit") {
		const auto nameOffset = stringRecordOffset(descriptorBytes, original.descriptor.imports.front().symbol);
		REQUIRE(nameOffset >= sizeof(uint32_t));
		offset = nameOffset - sizeof(uint32_t);
		replacement.u32(artifact::detail::MAX_RECORD_COUNT + 1);
		expected = "Invalid artifact record count";
	}
	const auto replacementBytes = replacement.take();
	REQUIRE(offset + replacementBytes.size() <= descriptorBytes.size());
	descriptorBytes.replace(offset, replacementBytes.size(), replacementBytes);
	const auto envelope = resealedEnvelope(original, descriptorBytes);
	artifact::detail::Reader reader(envelope);
	(void) reader.string();
	REQUIRE(reader.string() == descriptorBytes);
	REQUIRE(reader.string() == artifact::detail::digest(descriptorBytes));
	REQUIRE(artifact::detail::digest(descriptorBytes) != original.descriptorDigest);
	REQUIRE_THROWS_WITH(artifact::detail::decodeDescriptor(descriptorBytes), expected);
	REQUIRE_THROWS_WITH(artifact::decode(envelope), expected);
}

TEST_CASE_METHOD(DescriptorResealFixture, "Resealed valid bytecode rejects raw non-null pointer immediates",
                 "[artifact][mlir][transport][producer-contract][B1]") {
	::mlir::MLIRContext context;
	context.disableMultithreading();
	context.loadDialect<::mlir::LLVM::LLVMDialect>();
	llvm::SourceMgr sourceManager;
	sourceManager.AddNewSourceBuffer(
	    llvm::MemoryBuffer::getMemBufferCopy(llvm::StringRef(original.bytecode), "artifact-pointer-test.mlirbc"),
	    llvm::SMLoc());
	auto module = ::mlir::parseSourceFile<::mlir::ModuleOp>(sourceManager, ::mlir::ParserConfig(&context));
	REQUIRE(static_cast<bool>(module));
	REQUIRE(::mlir::succeeded(::mlir::verify(*module)));
	const auto abis = validateArtifactExportABI(*module, original.descriptor.exports);
	auto function = module->lookupSymbol<::mlir::LLVM::LLVMFuncOp>("execute");
	REQUIRE(static_cast<bool>(function));
	auto& block = function.getBody().front();
	auto terminator = llvm::cast<::mlir::LLVM::ReturnOp>(block.getTerminator());
	::mlir::OpBuilder builder(terminator);
	int32_t captured = 42;
	auto address = ::mlir::LLVM::ConstantOp::create(
	    builder, builder.getUnknownLoc(), builder.getI64Type(),
	    builder.getI64IntegerAttr(static_cast<int64_t>(reinterpret_cast<uintptr_t>(&captured))));
	auto pointer =
	    ::mlir::LLVM::IntToPtrOp::create(builder, builder.getUnknownLoc(), ::mlir::LLVM::LLVMPointerType::get(&context),
	                                     address.getResult(), ::mlir::LLVM::DereferenceableAttr {});
	auto loaded = ::mlir::LLVM::LoadOp::create(builder, builder.getUnknownLoc(), builder.getI32Type(), pointer);
	terminator->setOperand(0, loaded.getResult());
	REQUIRE(::mlir::succeeded(::mlir::verify(*module)));
	REQUIRE(validateArtifactExportABI(*module, original.descriptor.exports) == abis);
	auto [symbols, addresses] = resolvedImports(true);
	SECTION("lowered producer-contract validation") {
		const auto before = printModule(*module);
		REQUIRE_THROWS_AS(validateArtifactMLIRModule(*module, {"execute"}, symbols, addresses), RuntimeException);
		REQUIRE(printModule(*module) == before);
	}
	SECTION("bytecode loader producer-contract validation after resealing") {
		changed.bytecode.clear();
		llvm::raw_string_ostream output(changed.bytecode);
		REQUIRE(::mlir::succeeded(::mlir::writeBytecodeToFile(module->getOperation(), output)));
		output.flush();
		REQUIRE(changed.bytecode != original.bytecode);
		changed.descriptor.bytecodeDigest = artifact::detail::digest(changed.bytecode);
		requireResealedStructure();
		REQUIRE(changed.descriptor.moduleManifest == original.descriptor.moduleManifest);
		REQUIRE(changed.descriptor.exports == original.descriptor.exports);
		REQUIRE(changed.descriptor.imports == original.descriptor.imports);
		const auto before = artifact::encode(changed);
		REQUIRE_THROWS_AS(artifact::loadBytecode(changed, options), RuntimeException);
		REQUIRE(artifact::encode(changed) == before);
	}
	REQUIRE(captured == 42);
}

#endif

} // namespace nautilus::compiler::mlir

#endif

#include "catch2/catch_test_macros.hpp"
#include "catch2/matchers/catch_matchers_string.hpp"
#include "nautilus/RuntimeBinding.hpp"
#include "nautilus/compiler/backends/mlir/MLIRCacheValidation.hpp"
#include <cstdint>
#include <llvm/Support/raw_ostream.h>
#include <mlir/Dialect/LLVMIR/LLVMDialect.h>
#include <mlir/IR/MLIRContext.h>
#include <mlir/IR/Verifier.h>
#include <mlir/Parser/Parser.h>
#include <string>
#include <vector>

namespace nautilus::compiler::mlir {
namespace {

int32_t increment(int32_t value) {
	return value + 1;
}

std::string printModule(::mlir::ModuleOp module) {
	std::string text;
	llvm::raw_string_ostream output(text);
	module.print(output);
	output.flush();
	return text;
}

struct CacheValidationFixture {
	CacheValidationFixture() {
		context.disableMultithreading();
		context.loadDialect<::mlir::LLVM::LLVMDialect>();
		module = ::mlir::parseSourceString<::mlir::ModuleOp>(R"mlir(
module {
  llvm.mlir.global internal constant @constant(7 : i32) : i32
  llvm.func @increment(i32) -> i32
  llvm.func @execute(%value: i32) -> i32 {
    %result = llvm.call @increment(%value) : (i32) -> i32
    llvm.return %result : i32
  }
}
)mlir",
		                                                     &context);
		REQUIRE(static_cast<bool>(module));
		setBindingSchema();
		REQUIRE(::mlir::succeeded(::mlir::verify(*module)));
	}

	void setBindingSchema() {
		module->getOperation()->setAttr("nautilus.runtime_binding.schema",
		                                ::mlir::StringAttr::get(&context, options.getRuntimeBindings().schema()));
	}

	void validate() {
		validateCachedMLIRModule(*module, exportNames, externalSymbols, externalAddresses, options);
	}

	void requireFailure(const std::string& expected) {
		REQUIRE(::mlir::succeeded(::mlir::verify(*module)));
		const auto before = printModule(*module);
		REQUIRE_THROWS_WITH(validate(), Catch::Matchers::StartsWith(expected));
		REQUIRE(printModule(*module) == before);
	}

	engine::Options options;
	::mlir::MLIRContext context;
	::mlir::OwningOpRef<::mlir::ModuleOp> module;
	std::vector<std::string> exportNames {"execute"};
	std::vector<std::string> externalSymbols {"increment"};
	std::vector<void*> externalAddresses {reinterpret_cast<void*>(&increment)};
};

} // namespace

TEST_CASE_METHOD(CacheValidationFixture, "Cached MLIR validation succeeds without mutating its inputs",
                 "[cache][mlir][validation]") {
	int64_t unused = 42;
	SECTION("empty runtime binding registry") {
		REQUIRE(options.getRuntimeBindings().entries().empty());
	}
	SECTION("unused runtime binding") {
		RuntimeBindings bindings;
		(void) bindings.bind<int64_t>("unused", &unused);
		options.setRuntimeBindings(bindings);
		setBindingSchema();
		const auto& entry = bindings.entries().at("unused");
		REQUIRE(module->lookupSymbol(entry->symbol) == nullptr);
		SECTION("does not require an external manifest entry") {
			REQUIRE(externalSymbols.size() == 1);
		}
		SECTION("permits an external manifest entry without a module declaration") {
			externalSymbols.push_back(entry->symbol);
			externalAddresses.push_back(entry->address);
		}
	}
	REQUIRE(::mlir::succeeded(::mlir::verify(*module)));
	const auto beforeModule = printModule(*module);
	const auto beforeExports = exportNames;
	const auto beforeSymbols = externalSymbols;
	const auto beforeAddresses = externalAddresses;
	const auto beforeOptions = options.getOptionValues();
	const auto beforeSchema = options.getRuntimeBindings().schema();
	const auto beforeBindings = options.getRuntimeBindings().entries();
	REQUIRE_NOTHROW(validate());
	REQUIRE_NOTHROW(validate());
	REQUIRE(printModule(*module) == beforeModule);
	REQUIRE(exportNames == beforeExports);
	REQUIRE(externalSymbols == beforeSymbols);
	REQUIRE(externalAddresses == beforeAddresses);
	REQUIRE(options.getOptionValues() == beforeOptions);
	REQUIRE(options.getRuntimeBindings().schema() == beforeSchema);
	REQUIRE(options.getRuntimeBindings().entries() == beforeBindings);
	REQUIRE(unused == 42);
}

TEST_CASE_METHOD(CacheValidationFixture, "Cached MLIR validation rejects invalid exports",
                 "[cache][mlir][validation]") {
	std::string expected;
	SECTION("no exports") {
		exportNames.clear();
		expected = "Cached MLIR module has no exports";
	}
	SECTION("empty export name") {
		exportNames[0].clear();
		expected = "Cached MLIR export manifest is invalid";
	}
	SECTION("duplicate export name") {
		exportNames.push_back("execute");
		expected = "Cached MLIR export manifest is invalid";
	}
	SECTION("missing export") {
		exportNames = {"missing"};
		expected = "Cached MLIR module is missing export 'missing'";
	}
	SECTION("external declaration is not an export definition") {
		exportNames = {"increment"};
		expected = "Cached MLIR module is missing export 'increment'";
	}
	SECTION("global is not an export function") {
		exportNames = {"constant"};
		expected = "Cached MLIR module is missing export 'constant'";
	}
	requireFailure(expected);
}

TEST_CASE_METHOD(CacheValidationFixture, "Cached MLIR validation rejects invalid external manifests",
                 "[cache][mlir][validation]") {
	std::string expected;
	SECTION("more names than addresses") {
		externalAddresses.clear();
		expected = "Cached MLIR external symbol vectors differ in size";
	}
	SECTION("more addresses than names") {
		externalSymbols.clear();
		expected = "Cached MLIR external symbol vectors differ in size";
	}
	SECTION("empty external name") {
		externalSymbols[0].clear();
		expected = "Cached MLIR external symbol manifest is invalid";
	}
	SECTION("duplicate external name") {
		externalSymbols.push_back("increment");
		externalAddresses.push_back(externalAddresses[0]);
		expected = "Cached MLIR external symbol manifest is invalid";
	}
	SECTION("null external address") {
		externalAddresses[0] = nullptr;
		expected = "Cached MLIR external symbol manifest is invalid";
	}
	SECTION("module declaration absent from external manifest") {
		externalSymbols.clear();
		externalAddresses.clear();
		expected = "Cached MLIR module contains an undeclared external function";
	}
	SECTION("external manifest name absent from module") {
		externalSymbols.push_back("missing");
		externalAddresses.push_back(externalAddresses[0]);
		expected = "Cached MLIR external function is not declared by the module";
	}
	SECTION("defined function is not an external declaration") {
		externalSymbols.push_back("execute");
		externalAddresses.push_back(externalAddresses[0]);
		expected = "Cached MLIR external function is not declared by the module";
	}
	SECTION("global is not an external function declaration") {
		externalSymbols.push_back("constant");
		externalAddresses.push_back(externalAddresses[0]);
		expected = "Cached MLIR external function is not declared by the module";
	}
	requireFailure(expected);
}

TEST_CASE_METHOD(CacheValidationFixture, "Cached MLIR validation preserves failure precedence",
                 "[cache][mlir][validation]") {
	std::string expected;
	SECTION("exports precede vector lengths and runtime bindings") {
		exportNames.clear();
		externalAddresses.clear();
		module->getOperation()->removeAttr("nautilus.runtime_binding.schema");
		expected = "Cached MLIR module has no exports";
	}
	SECTION("every export is checked before external validation") {
		exportNames.push_back("missing");
		externalAddresses.clear();
		module->getOperation()->removeAttr("nautilus.runtime_binding.schema");
		expected = "Cached MLIR module is missing export 'missing'";
	}
	SECTION("export failures follow manifest order") {
		exportNames = {"missing", ""};
		expected = "Cached MLIR module is missing export 'missing'";
	}
	SECTION("external vector lengths precede runtime bindings") {
		externalAddresses.clear();
		module->getOperation()->removeAttr("nautilus.runtime_binding.schema");
		expected = "Cached MLIR external symbol vectors differ in size";
	}
	SECTION("runtime bindings precede external name validation") {
		externalSymbols[0].clear();
		module->getOperation()->removeAttr("nautilus.runtime_binding.schema");
		expected = "Cached MLIR runtime binding schema mismatch";
	}
	SECTION("runtime bindings precede external declaration checks") {
		externalSymbols.clear();
		externalAddresses.clear();
		module->getOperation()->removeAttr("nautilus.runtime_binding.schema");
		expected = "Cached MLIR runtime binding schema mismatch";
	}
	SECTION("external manifest validity precedes declaration checks") {
		externalSymbols[0] = "missing";
		externalAddresses[0] = nullptr;
		expected = "Cached MLIR external symbol manifest is invalid";
	}
	SECTION("unlisted module declarations precede extra manifest names") {
		externalSymbols[0] = "missing";
		expected = "Cached MLIR module contains an undeclared external function";
	}
	requireFailure(expected);
}

} // namespace nautilus::compiler::mlir

#include "nautilus/config.hpp"

#if defined(ENABLE_TRACING) && defined(ENABLE_MLIR_BACKEND)

#include "nautilus/Artifact.hpp"
#include "nautilus/Engine.hpp"
#include "nautilus/common/ExecutableImage.hpp"
#include "nautilus/compiler/backends/mlir/jit/PackFunctionArguments.hpp"
#include "nautilus/function.hpp"
#include "nautilus/nautilus_function.hpp"
#include <algorithm>
#include <array>
#include <bit>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>
#include <cstdint>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/GlobalVariable.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/Support/MemoryBuffer.h>
#include <llvm/Support/SourceMgr.h>
#include <llvm/Support/raw_ostream.h>
#include <mlir/Dialect/LLVMIR/LLVMDialect.h>
#include <mlir/IR/MLIRContext.h>
#include <mlir/IR/Verifier.h>
#include <mlir/Parser/Parser.h>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>

namespace nautilus::engine {
namespace {

double reviewAddOne(double value) noexcept {
	return value + 1.0;
}

int32_t reviewInteger(int32_t value) noexcept {
	return value + 2;
}

double reviewMixed(double value, int8_t byte, uint16_t word) noexcept {
	return value + byte + word;
}

int8_t reviewNarrow(int8_t byte, uint16_t word, bool flag) noexcept {
	return static_cast<int8_t>(flag ? byte + word : byte - word);
}

void reviewStore(double* output, double value) noexcept {
	*output = reviewAddOne(value);
}

double reviewChecked(double* observed, double value) {
	*observed = value;
	if (value < 0) {
		throw std::runtime_error("review typed result failure");
	}
	return reviewAddOne(value);
}

void reviewCheckedVoid(double* output, double value) {
	*output = value;
	if (value < 0) {
		throw std::runtime_error("review typed void failure");
	}
	*output = reviewAddOne(value);
}

int64_t reviewPure(int64_t value) noexcept {
	return value + 1;
}

int32_t reviewSignedByte(int8_t value) noexcept {
	return value;
}

Options reviewOptions(bool interpreted = false) {
	Options options;
	options.setOption("engine.backend", std::string("mlir"));
	if (interpreted) {
		options.setOption("engine.Compilation", false);
	}
	return options;
}

void requireReviewArtifactSupport() {
#ifdef __linux__
	REQUIRE(artifact::isSupported());
#else
	if (!artifact::isSupported()) {
		SKIP("Module artifacts require Linux ELF executable identities");
	}
#endif
}

void registerReviewConversions(NautilusModule& module) {
	module.registerFunction<val<double>(val<int64_t>)>("integer_to_double",
	                                                   [](val<int64_t> value) { return invoke(reviewAddOne, value); });
	module.registerFunction<val<double>(val<int64_t>)>(
	    "function_wrapper", [](val<int64_t> value) { return function(reviewAddOne)(value); });
	module.registerFunction<val<int32_t>(val<double>)>("double_to_integer",
	                                                   [](val<double> value) { return invoke(reviewInteger, value); });
	module.registerFunction<val<double>(val<int64_t>, val<int32_t>, val<int32_t>)>(
	    "mixed", [](val<int64_t> value, val<int32_t> byte, val<int32_t> word) {
		    return invoke(reviewMixed, value, byte, word);
	    });
	module.registerFunction<val<int8_t>(val<int8_t (*)(int8_t, uint16_t, bool)>, val<int64_t>, val<int64_t>,
	                                    val<int32_t>)>(
	    "indirect_narrow", [](val<int8_t (*)(int8_t, uint16_t, bool)> pointer, val<int64_t> byte, val<int64_t> word,
	                          val<int32_t> flag) { return pointer(byte, word, flag); });
	module.registerFunction<val<int64_t>(val<int8_t (*)(int8_t, uint16_t, bool)>, val<int64_t>, val<int64_t>,
	                                     val<int32_t>)>(
	    "indirect_narrow_widened",
	    [](val<int8_t (*)(int8_t, uint16_t, bool)> pointer, val<int64_t> byte, val<int64_t> word, val<int32_t> flag) {
		    return static_cast<val<int64_t>>(pointer(byte, word, flag));
	    });
	module.registerFunction<val<double>(val<double (*)(double)>, val<int64_t*>)>(
	    "indirect_reference", [](val<double (*)(double)> pointer, val<int64_t*> value) { return pointer(*value); });
	module.registerFunction<val<double>(val<double (*)(double)>, val<int64_t>)>(
	    "indirect", [](val<double (*)(double)> pointer, val<int64_t> value) { return pointer(value); });
	module.registerFunction<void(val<void (*)(double*, double)>, val<double*>, val<int64_t>)>(
	    "indirect_void", [](val<void (*)(double*, double)> pointer, val<double*> output, val<int64_t> value) {
		    pointer(output, value);
	    });
	module.registerFunction<void(val<double*>, val<int64_t>)>(
	    "noexcept_void", [](val<double*> output, val<int64_t> value) { invoke(reviewStore, output, value); });
	module.registerFunction<val<double>(val<double*>, val<int64_t>)>(
	    "throwing_result",
	    [](val<double*> observed, val<int64_t> value) { return invoke(reviewChecked, observed, value); });
	module.registerFunction<void(val<double*>, val<int64_t>)>(
	    "throwing_void", [](val<double*> output, val<int64_t> value) { invoke(reviewCheckedVoid, output, value); });
	module.registerFunction<val<double>(val<int64_t*>)>(
	    "loaded_reference", [](val<int64_t*> address) { return invoke(reviewAddOne, *address); });
	module.registerFunction<val<double>(val<const int64_t*>)>(
	    "loaded_const_reference", [](val<const int64_t*> address) { return invoke(reviewAddOne, *address); });
	module.registerFunction<void(val<double*>, val<int64_t*>)>(
	    "void_loaded_reference",
	    [](val<double*> output, val<int64_t*> address) { invoke(reviewStore, output, *address); });
	module.registerFunction<val<double>(val<double*>, val<int64_t*>)>(
	    "throwing_loaded_reference",
	    [](val<double*> observed, val<int64_t*> address) { return invoke(reviewChecked, observed, *address); });
}

void checkReviewConversions(CompiledModule& module) {
	SECTION("scalar and mixed ABI conversions") {
		const auto converted = module.getFunction<double(int64_t)>("integer_to_double");
		const auto wrapped = module.getFunction<double(int64_t)>("function_wrapper");
		for (const int64_t value : {int64_t {0}, int64_t {42}, int64_t {-9}, int64_t {1} << 40}) {
			CAPTURE(value);
			REQUIRE(converted(value) == reviewAddOne(static_cast<double>(value)));
			REQUIRE(wrapped(value) == reviewAddOne(static_cast<double>(value)));
		}
		const auto integer = module.getFunction<int32_t(double)>("double_to_integer");
		for (const double value : {-3.75, 0.0, 17.75}) {
			REQUIRE(integer(value) == reviewInteger(static_cast<int32_t>(value)));
		}
		const auto mixed = module.getFunction<double(int64_t, int32_t, int32_t)>("mixed");
		REQUIRE(mixed(42, -128, 65535) == reviewMixed(42.0, int8_t {-128}, uint16_t {65535}));
		REQUIRE(mixed(-19, 127, 32768) == reviewMixed(-19.0, int8_t {127}, uint16_t {32768}));
	}
	SECTION("indirect native typed conversions") {
		const auto indirect = module.getFunction<double(double (*)(double), int64_t)>("indirect");
		const auto indirectVoid =
		    module.getFunction<void(void (*)(double*, double), double*, int64_t)>("indirect_void");
		REQUIRE(indirect(reviewAddOne, 42) == 43.0);
		const auto narrow = module.getFunction<int8_t(int8_t (*)(int8_t, uint16_t, bool), int64_t, int64_t, int32_t)>(
		    "indirect_narrow");
		const auto widened = module.getFunction<int64_t(int8_t (*)(int8_t, uint16_t, bool), int64_t, int64_t, int32_t)>(
		    "indirect_narrow_widened");
		for (const int32_t flag : {0, 1}) {
			REQUIRE(narrow(reviewNarrow, -128, 65535, flag) == reviewNarrow(-128, 65535, flag));
			REQUIRE(widened(reviewNarrow, -128, 65535, flag) == static_cast<int64_t>(reviewNarrow(-128, 65535, flag)));
		}
		const auto reference = module.getFunction<double(double (*)(double), int64_t*)>("indirect_reference");
		int64_t value = 42;
		REQUIRE(reference(reviewAddOne, &value) == 43.0);
		double output = 0;
		indirectVoid(reviewStore, &output, -9);
		REQUIRE(output == -8.0);
	}
	SECTION("noexcept void conversion") {
		const auto store = module.getFunction<void(double*, int64_t)>("noexcept_void");
		double output = 0;
		store(&output, 42);
		REQUIRE(output == 43.0);
		store(&output, -9);
		REQUIRE(output == -8.0);
	}
	SECTION("throwing result conversion") {
		const auto checked = module.getFunction<double(double*, int64_t)>("throwing_result");
		double observed = 0;
		REQUIRE(checked(&observed, 42) == 43.0);
		REQUIRE(observed == 42.0);
		REQUIRE_THROWS_WITH(checked(&observed, -9), "review typed result failure");
		REQUIRE(observed == -9.0);
		REQUIRE(checked(&observed, 5) == 6.0);
	}
	SECTION("throwing void conversion") {
		const auto checked = module.getFunction<void(double*, int64_t)>("throwing_void");
		double output = 0;
		checked(&output, 42);
		REQUIRE(output == 43.0);
		REQUIRE_THROWS_WITH(checked(&output, -9), "review typed void failure");
		REQUIRE(output == -9.0);
		checked(&output, 5);
		REQUIRE(output == 6.0);
	}
	SECTION("element references load before ABI conversion") {
		int64_t value = 42;
		const auto loaded = module.getFunction<double(int64_t*)>("loaded_reference");
		const auto constLoaded = module.getFunction<double(const int64_t*)>("loaded_const_reference");
		const auto store = module.getFunction<void(double*, int64_t*)>("void_loaded_reference");
		const auto checked = module.getFunction<double(double*, int64_t*)>("throwing_loaded_reference");
		REQUIRE(loaded(&value) == 43.0);
		REQUIRE(constLoaded(&value) == 43.0);
		double output = 0;
		store(&output, &value);
		REQUIRE(output == 43.0);
		REQUIRE(checked(&output, &value) == 43.0);
		REQUIRE(output == 42.0);
		value = -9;
		REQUIRE(loaded(&value) == -8.0);
		REQUIRE(constLoaded(&value) == -8.0);
		REQUIRE_THROWS_WITH(checked(&output, &value), "review typed result failure");
		REQUIRE(output == -9.0);
	}
}

::mlir::LLVM::LLVMFuncOp reviewImport(::mlir::ModuleOp module, const artifact::ModuleArtifact& artifact,
                                      const void* address) {
	const auto identity = common::locateExecutableAddress(address);
	REQUIRE(identity.has_value());
	const auto imported = std::ranges::find_if(artifact.descriptor.imports, [&](const auto& entry) {
		return entry.bytecodeImport && entry.image.buildId == identity->buildId &&
		       entry.image.loadOffset == identity->loadOffset;
	});
	REQUIRE(imported != artifact.descriptor.imports.end());
	auto function = module.lookupSymbol<::mlir::LLVM::LLVMFuncOp>(imported->symbol);
	REQUIRE(static_cast<bool>(function));
	REQUIRE(function.getBody().empty());
	return function;
}

struct ReviewAttributePair {
	FunctionAttributes first;
	FunctionAttributes second;
	FunctionAttributes merged;
};

constexpr std::array<ReviewAttributePair, 4> REVIEW_ATTRIBUTES {{
    {{ModRefInfo::NoModRef, true, true}, {ModRefInfo::ModRef, false, true}, {ModRefInfo::ModRef, false, true}},
    {{ModRefInfo::NoModRef, true, true}, {ModRefInfo::ModRef, false, false}, {ModRefInfo::ModRef, false, false}},
    {{ModRefInfo::Ref, true, true}, {ModRefInfo::Mod, true, false}, {ModRefInfo::ModRef, true, false}},
    {{ModRefInfo::NoModRef, true, true}, {ModRefInfo::Ref, false, true}, {ModRefInfo::Ref, false, true}},
}};

void registerReviewAttributes(NautilusModule& module, FunctionAttributes firstAttrs, FunctionAttributes secondAttrs) {
	int64_t (*pointer)(int64_t) = reviewPure;
	module.registerFunction<val<int64_t>(val<int64_t>)>("attributes", [=](val<int64_t> value) {
		auto first = invoke(firstAttrs, pointer, value);
		auto second = invoke(secondAttrs, pointer, value);
		return first + second;
	});
}

void checkReviewAttributes(CompiledModule& module) {
	const auto function = module.getFunction<int64_t(int64_t)>("attributes");
	REQUIRE(function(42) == 86);
	REQUIRE(function(-9) == -16);
}

void checkReviewCollision(CompiledModule& module) {
	REQUIRE(module.getFunction<int32_t(int32_t)>("caller")(5) == 12);
	REQUIRE(module.getFunction<int32_t(int32_t)>("second_caller")(5) == 22);
	REQUIRE(module.getFunction<int32_t(int32_t)>("helper")(5) == 16);
	REQUIRE(module.getFunction<int32_t(int32_t)>("helper_2")(5) == 18);
}

std::string printReviewLLVM(llvm::Module& module) {
	std::string text;
	llvm::raw_string_ostream output(text);
	module.print(output, nullptr);
	return text;
}

} // namespace

TEST_CASE("Typed native calls convert arguments to the declared ABI", "[invoke][mlir][review3]") {
	for (const bool interpreted : {true, false}) {
		DYNAMIC_SECTION((interpreted ? "interpreter" : "compiled")) {
			NautilusEngine engine(reviewOptions(interpreted));
			auto module = engine.createModule();
			registerReviewConversions(module);
			auto compiled = module.compile();
			checkReviewConversions(compiled);
			REQUIRE_FALSE(tracing::inTracer());
		}
	}
}

TEST_CASE("Module artifacts preserve typed native argument conversions", "[artifact][mlir][B1][review3]") {
	requireReviewArtifactSupport();
	NautilusEngine engine(reviewOptions());
	auto module = engine.createModule();
	registerReviewConversions(module);
	const auto original = module.createArtifact();
	const auto decoded = artifact::decode(artifact::encode(original));
	::mlir::MLIRContext context;
	context.disableMultithreading();
	context.loadDialect<::mlir::LLVM::LLVMDialect>();
	llvm::SourceMgr sourceManager;
	sourceManager.AddNewSourceBuffer(llvm::MemoryBuffer::getMemBufferCopy(decoded.bytecode), llvm::SMLoc());
	auto lowered = ::mlir::parseSourceFile<::mlir::ModuleOp>(sourceManager, ::mlir::ParserConfig(&context));
	REQUIRE(static_cast<bool>(lowered));
	auto narrow = lowered->lookupSymbol<::mlir::LLVM::LLVMFuncOp>("indirect_narrow");
	REQUIRE(static_cast<bool>(narrow));
	std::size_t calls = 0;
	narrow.walk([&](::mlir::LLVM::CallOp call) {
		++calls;
		REQUIRE_FALSE(call.getCallee().has_value());
		const auto arguments = call.getArgAttrsAttr();
		REQUIRE(arguments.size() == 3);
		REQUIRE(::mlir::cast<::mlir::DictionaryAttr>(arguments[0]).contains("llvm.signext"));
		REQUIRE(::mlir::cast<::mlir::DictionaryAttr>(arguments[1]).contains("llvm.zeroext"));
		REQUIRE(::mlir::cast<::mlir::DictionaryAttr>(arguments[2]).contains("llvm.zeroext"));
		REQUIRE_FALSE(call.getResAttrsAttr());
	});
	REQUIRE(calls == 1);
	for (const bool bytecode : {false, true}) {
		DYNAMIC_SECTION((bytecode ? "bytecode" : "native")) {
			auto loaded = bytecode ? artifact::loadBytecode(decoded) : artifact::loadNative(decoded);
			checkReviewConversions(loaded);
			REQUIRE_FALSE(tracing::inTracer());
		}
	}
}

TEST_CASE("Root and helper name collisions preserve different bodies", "[artifact][module][mlir][B1][review3]") {
	for (const bool helperRootFirst : {false, true}) {
		for (const std::string mode : {"interpreter", "compiled", "native", "bytecode"}) {
			DYNAMIC_SECTION(mode << " root-first=" << helperRootFirst) {
				if (mode == "native" || mode == "bytecode") {
					requireReviewArtifactSupport();
				}
				NautilusFunction firstHelper(
				    "helper", [](val<int32_t> value) noexcept { return value + cacheLiteral<int32_t {7}>(); });
				NautilusFunction secondHelper(
				    "helper", [](val<int32_t> value) noexcept { return value + cacheLiteral<int32_t {17}>(); });
				NautilusEngine engine(reviewOptions(mode == "interpreter"));
				auto module = engine.createModule();
				const auto root = [](val<int32_t> value) {
					return value + cacheLiteral<int32_t {11}>();
				};
				if (helperRootFirst) {
					module.registerFunction<val<int32_t>(val<int32_t>)>("helper", root);
				}
				module.registerFunction<val<int32_t>(val<int32_t>)>(
				    "caller", [&](val<int32_t> value) { return firstHelper(value); });
				module.registerFunction<val<int32_t>(val<int32_t>)>(
				    "second_caller", [&](val<int32_t> value) { return secondHelper(value); });
				if (!helperRootFirst) {
					module.registerFunction<val<int32_t>(val<int32_t>)>("helper", root);
				}
				module.registerFunction<val<int32_t>(val<int32_t>)>(
				    "helper_2", [](val<int32_t> value) { return value + cacheLiteral<int32_t {13}>(); });
				if (mode == "native" || mode == "bytecode") {
					const auto persisted = artifact::decode(artifact::encode(module.createArtifact()));
					auto loaded =
					    mode == "bytecode" ? artifact::loadBytecode(persisted) : artifact::loadNative(persisted);
					checkReviewCollision(loaded);
				} else {
					auto compiled = module.compile();
					checkReviewCollision(compiled);
				}
				REQUIRE_FALSE(tracing::inTracer());
			}
		}
	}
}

TEST_CASE("Packed wrapper export collisions reject artifact emission",
          "[artifact][mlir][B1][review3][packed-collision]") {
	requireReviewArtifactSupport();
	for (const bool reverse : {false, true}) {
		DYNAMIC_SECTION("reverse=" << reverse) {
			NautilusEngine engine(reviewOptions());
			auto module = engine.createModule();
			for (const auto& name : reverse ? std::array {"_mlir_foo", "foo"} : std::array {"foo", "_mlir_foo"}) {
				module.registerFunction<val<int32_t>()>(name, [] { return cacheLiteral<int32_t {7}>(); });
			}
			REQUIRE_THROWS_WITH(
			    module.createArtifact(),
			    Catch::Matchers::StartsWith("MLIR packed wrapper symbol conflicts with existing symbol:"));
			REQUIRE_FALSE(tracing::inTracer());
		}
	}
}

TEST_CASE("Packed wrapper collision guard checks all symbols before mutation", "[mlir][review3][packed-collision]") {
	for (const std::string kind : {"defined helper", "global", "declaration"}) {
		DYNAMIC_SECTION(kind) {
			llvm::LLVMContext context;
			llvm::Module module("review packed names", context);
			llvm::IRBuilder<> builder(context);
			const auto addFunction = [&](const char* name) {
				auto* function = llvm::Function::Create(llvm::FunctionType::get(builder.getInt32Ty(), false),
				                                        llvm::GlobalValue::ExternalLinkage, name, module);
				builder.SetInsertPoint(llvm::BasicBlock::Create(context, "entry", function));
				builder.CreateRet(builder.getInt32(7));
			};
			addFunction("unrelated");
			addFunction("foo");
			if (kind == "global") {
				new llvm::GlobalVariable(module, builder.getInt32Ty(), true, llvm::GlobalValue::InternalLinkage,
				                         builder.getInt32(11), "_mlir_foo");
			} else {
				auto* function = llvm::Function::Create(
				    llvm::FunctionType::get(builder.getVoidTy(), builder.getPtrTy(), false),
				    kind == "defined helper" ? llvm::GlobalValue::InternalLinkage : llvm::GlobalValue::ExternalLinkage,
				    "_mlir_foo", module);
				if (kind == "defined helper") {
					builder.SetInsertPoint(llvm::BasicBlock::Create(context, "entry", function));
					builder.CreateRetVoid();
				}
			}
			const auto before = printReviewLLVM(module);
			REQUIRE_THROWS_WITH(compiler::mlir::detail::packFunctionArguments(&module),
			                    "MLIR packed wrapper symbol conflicts with existing symbol: _mlir_foo");
			REQUIRE(printReviewLLVM(module) == before);
			REQUIRE(module.getNamedValue("_mlir_unrelated") == nullptr);
		}
	}
}

TEST_CASE("Truthful differing native attributes compile conservatively", "[invoke][mlir][review3]") {
	for (std::size_t index = 0; index < REVIEW_ATTRIBUTES.size(); ++index) {
		const auto& pair = REVIEW_ATTRIBUTES[index];
		for (const bool reverse : {false, true}) {
			for (const bool interpreted : {true, false}) {
				DYNAMIC_SECTION(index << " reverse=" << reverse << " interpreted=" << interpreted) {
					NautilusEngine engine(reviewOptions(interpreted));
					auto module = engine.createModule();
					registerReviewAttributes(module, reverse ? pair.second : pair.first,
					                         reverse ? pair.first : pair.second);
					auto compiled = module.compile();
					checkReviewAttributes(compiled);
					REQUIRE_FALSE(tracing::inTracer());
				}
			}
		}
	}
}

TEST_CASE("Artifact native declarations retain conservative attribute intersections", "[artifact][mlir][B1][review3]") {
	requireReviewArtifactSupport();
	for (std::size_t index = 0; index < REVIEW_ATTRIBUTES.size(); ++index) {
		const auto& pair = REVIEW_ATTRIBUTES[index];
		for (const bool reverse : {false, true}) {
			DYNAMIC_SECTION(index << " reverse=" << reverse) {
				NautilusEngine engine(reviewOptions());
				auto module = engine.createModule();
				registerReviewAttributes(module, reverse ? pair.second : pair.first,
				                         reverse ? pair.first : pair.second);
				const auto artifact = module.createArtifact();
				::mlir::MLIRContext context;
				context.disableMultithreading();
				context.loadDialect<::mlir::LLVM::LLVMDialect>();
				llvm::SourceMgr sourceManager;
				sourceManager.AddNewSourceBuffer(llvm::MemoryBuffer::getMemBufferCopy(artifact.bytecode),
				                                 llvm::SMLoc());
				auto lowered = ::mlir::parseSourceFile<::mlir::ModuleOp>(sourceManager, ::mlir::ParserConfig(&context));
				REQUIRE(static_cast<bool>(lowered));
				REQUIRE(::mlir::succeeded(::mlir::verify(*lowered)));
				auto imported = reviewImport(*lowered, artifact, reinterpret_cast<const void*>(&reviewPure));
				const auto effects = static_cast<::mlir::LLVM::ModRefInfo>(pair.merged.modRefInfo);
				REQUIRE(imported.getMemoryEffectsAttr() ==
				        ::mlir::LLVM::MemoryEffectsAttr::get(&context, effects, effects, effects, effects, effects,
				                                             effects));
				const auto passthrough = imported.getPassthroughAttr();
				const auto hasAttribute = [&](std::string_view name) {
					return passthrough && std::ranges::any_of(passthrough, [&](::mlir::Attribute attribute) {
						       const auto value = ::mlir::dyn_cast<::mlir::StringAttr>(attribute);
						       return value && value.getValue().str() == name;
					       });
				};
				REQUIRE((static_cast<bool>(imported.getNoUnwindAttr()) || hasAttribute("nounwind")) ==
				        pair.merged.noUnwind);
				REQUIRE((static_cast<bool>(imported.getWillReturnAttr()) || hasAttribute("willreturn")) ==
				        pair.merged.willReturn);
				for (const bool bytecode : {false, true}) {
					auto loaded = bytecode ? artifact::loadBytecode(artifact) : artifact::loadNative(artifact);
					checkReviewAttributes(loaded);
				}
				REQUIRE_FALSE(tracing::inTracer());
			}
		}
	}
}

TEST_CASE("Native declaration reconciliation still rejects conflicting C ABIs", "[invoke][mlir][review3]") {
	NautilusEngine engine(reviewOptions());
	auto module = engine.createModule();
	std::string expected = "conflicting signature";
	SECTION("narrow argument signedness") {
		expected = "conflicting argument attributes";
		module.registerFunction<val<int32_t>(val<int8_t>, val<uint8_t>)>(
		    "conflicting", [](val<int8_t> signedValue, val<uint8_t> unsignedValue) {
			    auto first = invoke(reviewSignedByte, signedValue);
			    auto second = invoke(std::bit_cast<int32_t (*)(uint8_t) noexcept>(&reviewSignedByte), unsignedValue);
			    return first + second;
		    });
	}
	SECTION("argument width") {
		module.registerFunction<val<int32_t>(val<int8_t>, val<int64_t>)>(
		    "conflicting", [](val<int8_t> byte, val<int64_t> wide) {
			    auto first = invoke(reviewSignedByte, byte);
			    auto second = invoke(std::bit_cast<int32_t (*)(int64_t) noexcept>(&reviewSignedByte), wide);
			    return first + second;
		    });
	}
	SECTION("result width") {
		module.registerFunction<val<int64_t>(val<int8_t>)>("conflicting", [](val<int8_t> value) {
			auto first = static_cast<val<int64_t>>(invoke(reviewSignedByte, value));
			auto second = invoke(std::bit_cast<int64_t (*)(int8_t) noexcept>(&reviewSignedByte), value);
			return first + second;
		});
	}
	REQUIRE_THROWS_WITH(module.compile(), Catch::Matchers::ContainsSubstring(expected));
	REQUIRE_FALSE(tracing::inTracer());
}

} // namespace nautilus::engine

#endif

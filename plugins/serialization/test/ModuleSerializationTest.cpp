#include "IRRoundTrip.hpp"
#include "nautilus/Engine.hpp"
#include "nautilus/config.hpp"
#include "nautilus/nautilus_function.hpp"
#include "nautilus/serialization/IRSerialization.hpp"
#include "nautilus/serialization/ModuleSerialization.hpp"
#include <algorithm>
#include <catch2/catch_all.hpp>
#include <random>

namespace nautilus::engine {

namespace ser = nautilus::serialization;

namespace {

int64_t moduleNativeOffset(int64_t x) noexcept {
	return x + 11;
}

val<int64_t> moduleSquareHelper(val<int64_t> x) {
	return x * x;
}

auto moduleSquare = NautilusFunction {"square", moduleSquareHelper};

val<int64_t> sumSquares(val<int64_t*> values, val<int64_t> count) {
	val<int64_t> sum = 0;
	for (val<int64_t> i = 0; i < count; i = i + 1) {
		sum = sum + moduleSquare(values[i]);
	}
	return sum;
}

val<int64_t> offsetTwice(val<int64_t> x) {
	return invoke(moduleNativeOffset, x) * 2;
}

std::vector<std::string> backendNames() {
	std::vector<std::string> names;
#ifdef ENABLE_MLIR_BACKEND
	names.emplace_back("mlir");
#endif
#ifdef ENABLE_C_BACKEND
	names.emplace_back("cpp");
#endif
#ifdef ENABLE_BC_BACKEND
	names.emplace_back("bc");
#endif
#ifdef ENABLE_TBC_BACKEND
	names.emplace_back("tbc");
#endif
#ifdef ENABLE_ASMJIT_BACKEND
	names.emplace_back("asmjit");
#endif
	return names;
}

/// An engine pinned to @p backend ("" for the default tiered compiler).
NautilusEngine makeEngine(const std::string& backend, bool retainIR) {
	Options options;
	if (!backend.empty()) {
		options.setOption("engine.backend", backend);
	}
	options.setOption("engine.retainIR", retainIR);
	return NautilusEngine(options);
}

CompiledModule compileTestModule(const NautilusEngine& engine) {
	auto module = engine.createModule();
	module.registerFunction("sum", sumSquares);
	module.registerFunction("offset", offsetTwice);
	return module.compile();
}

/// Results the test module must produce, computed natively.
void requireTestModuleResults(CompiledModule& module) {
	auto sum = module.getFunction<int64_t(int64_t*, int64_t)>("sum");
	auto offset = module.getFunction<int64_t(int64_t)>("offset");
	std::vector<int64_t> values {3, -4, 5, 10};
	REQUIRE(sum(values.data(), 4) == 9 + 16 + 25 + 100);
	REQUIRE(sum(values.data(), 0) == 0);
	REQUIRE(offset(1) == 24);
	REQUIRE(offset(-11) == 0);
}

const ser::FunctionSignature* findFunction(const ser::ModuleImageInfo& info, const std::string& name) {
	const auto it = std::find_if(info.functions.begin(), info.functions.end(),
	                             [&](const auto& function) { return function.name == name; });
	return it != info.functions.end() ? &*it : nullptr;
}

} // namespace

TEST_CASE("A compiled module survives a module image") {
	auto sender = makeEngine("bc", true);
	auto original = compileTestModule(sender);
	requireTestModuleResults(original);

	testing::SyntheticSymbols symbols;
	ser::SerializeOptions writeOptions;
	writeOptions.pointerMode = ser::PointerMode::Portable;
	writeOptions.namer = &symbols;
	const auto image = ser::saveModule(original, writeOptions);

	ser::DeserializeOptions readOptions;
	readOptions.resolver = &symbols;
	readOptions.verifyIR = true;

	SECTION("inspect") {
		const auto info = ser::inspectModule(image);
		REQUIRE(info.unitId == original.getIR()->getId());
		const auto* sum = findFunction(info, "sum");
		REQUIRE(sum != nullptr);
		REQUIRE(sum->result == Type::i64);
		REQUIRE(sum->params == std::vector<Type> {Type::ptr, Type::i64});
		REQUIRE(findFunction(info, "offset") != nullptr);
		REQUIRE(findFunction(info, "square") != nullptr);
		REQUIRE(info.options.getOptionOrDefault("engine.retainIR", false));
		REQUIRE(info.options.getOptionOrDefault<std::string>("engine.backend", "") == "bc");
	}

	SECTION("load on every backend") {
		auto backends = backendNames();
		backends.emplace_back(""); // the default tiered compiler
		for (const auto& backend : backends) {
			DYNAMIC_SECTION("backend '" << backend << "'") {
				auto receiver = makeEngine(backend, false);
				auto loaded = ser::loadModule(receiver, image, readOptions);
				requireTestModuleResults(loaded);
			}
		}
	}

	SECTION("a reloaded module saves to the same image") {
		auto receiver = makeEngine("bc", true);
		auto loaded = ser::loadModule(receiver, image, readOptions);
		REQUIRE(ser::saveModule(loaded, writeOptions) == image);
	}

	SECTION("the image is also a plain IR buffer") {
		REQUIRE(ser::deserialize(image, readOptions)->toString() == original.getIR()->toString());
	}

	SECTION("getFunction checks signatures of a loaded module") {
		auto receiver = makeEngine("bc", false);
		auto loaded = ser::loadModule(receiver, image, readOptions);
		REQUIRE_THROWS_WITH(loaded.getFunction<int32_t(int64_t)>("offset"),
		                    Catch::Matchers::ContainsSubstring("i64(i64)"));
		REQUIRE_THROWS_AS(loaded.getFunction<int64_t(int64_t, int64_t)>("offset"), std::invalid_argument);
		REQUIRE_THROWS_WITH(loaded.getFunction<int64_t(int64_t)>("missing"),
		                    Catch::Matchers::ContainsSubstring("no function named 'missing'"));
		// Width matters, signedness included.
		REQUIRE_THROWS_AS(loaded.getFunction<int64_t(uint64_t)>("offset"), std::invalid_argument);
	}

	SECTION("corrupted images fail cleanly") {
		std::mt19937 random(7);
		for (int round = 0; round < 500; ++round) {
			auto corrupted = image;
			corrupted[random() % corrupted.size()] = static_cast<std::byte>(random());
			try {
				(void) ser::inspectModule(corrupted, false);
			} catch (const ser::SerializationException&) {
			}
		}
	}
}

TEST_CASE("Saving a module needs its IR") {
	auto engine = makeEngine("bc", false);
	auto module = compileTestModule(engine);
	REQUIRE(module.getIR() == nullptr);
	REQUIRE_THROWS_WITH(ser::saveModule(module), Catch::Matchers::ContainsSubstring("engine.retainIR"));
}

TEST_CASE("A tiered module keeps its IR while it is promoted") {
	auto engine = makeEngine("", true);
	auto module = compileTestModule(engine);
	// Saving reads the graph while a background tier-1 compile may also be
	// reading it; both only read.
	const auto image = ser::saveModule(module);
	requireTestModuleResults(module);
	auto reloaded = ser::loadModule(engine, image);
	requireTestModuleResults(reloaded);
}

TEST_CASE("A plain IR buffer is not a module image") {
	auto engine = makeEngine("bc", true);
	auto module = compileTestModule(engine);
	const auto irOnly = ser::serialize(*module.getIR());
	REQUIRE_THROWS_WITH(ser::inspectModule(irOnly), Catch::Matchers::ContainsSubstring("not a module image"));
}

TEST_CASE("An interpreting engine cannot compile IR") {
	Options options;
	options.setOption("engine.Compilation", false);
	NautilusEngine interpreter(options);
	auto engine = makeEngine("bc", true);
	auto module = compileTestModule(engine);
	REQUIRE_THROWS_AS(ser::loadModule(interpreter, ser::saveModule(module)), std::logic_error);
}

} // namespace nautilus::engine

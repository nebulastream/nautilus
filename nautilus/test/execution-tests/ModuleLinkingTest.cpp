#include "ExecutionTest.hpp"
#include "nautilus/Engine.hpp"
#include "nautilus/common/Arena.hpp"
#include "nautilus/compiler/TieredCompiler.hpp"
#include "nautilus/config.hpp"
#include "nautilus/function.hpp"
#include "nautilus/link/ModuleRegistry.hpp"
#include "nautilus/nir/Module.hpp"
#include <atomic>
#include <catch2/catch_all.hpp>
#include <optional>
#include <stdexcept>
#include <string>
#include <thread>
#include <vector>

// Modules and linking on the engine (docs/plans/nautilus-0.4/13-nir-spec.md §4.4, plan M4.6): a named module is
// registered when it is compiled, another module imports its functions, and the call goes through an import slot on
// every backend. The rules of the link step alone are ModuleRegistryTest.cpp.

namespace nautilus::engine {

namespace {

val<uint64_t> linkHash(val<uint64_t> x) {
	return x * 31 + 7;
}

val<int32_t> linkScale(val<int32_t> a, val<int32_t> b) {
	return a * 3 + b;
}

val<uint8_t> linkNarrow(val<uint8_t> x, val<bool> flip) {
	if (flip) {
		val<uint8_t> one = 1;
		return x + one;
	}
	return x;
}

void linkThrowIfNegative(int32_t value) {
	if (value < 0) {
		throw std::runtime_error("negative value");
	}
}

val<int32_t> linkChecked(val<int32_t> x) {
	invoke(linkThrowIfNegative, x);
	return x + 1;
}

/// The backends whose pipeline handles imports: every one that runs the NIR tracer (the pseudo-backends of the 0.3
/// tracer do not), and the interpreter.
std::vector<std::string> linkBackends() {
	std::vector<std::string> names {"interpreter"};
	for (const auto& name : testing::availableBackends()) {
		if (name.starts_with("legacy-") || name.starts_with("nir-")) {
			continue;
		}
		names.push_back(name);
	}
	return names;
}

template <typename Body>
void forEachLinkBackend(Body&& body) {
	for (const auto& backend : linkBackends()) {
		DYNAMIC_SECTION(backend) {
			auto engine = testing::makeEngine(backend);
			body(engine);
		}
	}
}

CompiledModule compileLibrary(NautilusEngine& engine, const std::string& name = "lib.math") {
	auto lib = engine.createModule(name);
	lib.registerFunction("hash", linkHash);
	lib.registerFunction("scale", linkScale);
	lib.registerFunction("narrow", linkNarrow);
	lib.registerFunction("checked", linkChecked);
	return lib.compile();
}

} // namespace

TEST_CASE("Modules - an imported function is called through the import slot on every backend") {
	forEachLinkBackend([](NautilusEngine& engine) {
		auto library = compileLibrary(engine);

		auto query = engine.createModule("nes.query42");
		auto hash = query.importFunction<uint64_t(uint64_t)>("lib.math", "hash");
		auto scale = query.importFunction<int32_t(int32_t, int32_t)>("lib.math", "scale");
		query.registerFunction<val<uint64_t>(val<uint64_t>)>(
		    "twice", [hash](val<uint64_t> x) -> val<uint64_t> { return hash(hash(x)) + 1; });
		query.registerFunction<val<int32_t>(val<int32_t>)>(
		    "mixed", [scale](val<int32_t> x) -> val<int32_t> { return scale(x, x) + scale(x, 2); });
		auto compiled = query.compile();

		auto twice = compiled.getFunction<uint64_t(uint64_t)>("twice");
		auto mixed = compiled.getFunction<int32_t(int32_t)>("mixed");
		REQUIRE(twice(1) == (1 * 31 + 7) * 31 + 7 + 1);
		REQUIRE(twice(100) == (100 * 31 + 7) * 31 + 7 + 1);
		REQUIRE(mixed(5) == (5 * 3 + 5) + (5 * 3 + 2));
		REQUIRE(mixed(-4) == (-4 * 3 + -4) + (-4 * 3 + 2));
	});
}

TEST_CASE("Modules - narrow arguments and results cross the module boundary") {
	forEachLinkBackend([](NautilusEngine& engine) {
		auto library = compileLibrary(engine);
		auto query = engine.createModule("nes.q");
		auto narrow = query.importFunction<uint8_t(uint8_t, bool)>("lib.math", "narrow");
		query.registerFunction<val<uint8_t>(val<uint8_t>, val<bool>)>(
		    "run", [narrow](val<uint8_t> x, val<bool> flip) -> val<uint8_t> { return narrow(x, flip); });
		auto compiled = query.compile();
		auto run = compiled.getFunction<uint8_t(uint8_t, bool)>("run");
		REQUIRE(run(41, true) == 42);
		REQUIRE(run(41, false) == 41);
		REQUIRE(run(255, true) == 0);
	});
}

TEST_CASE("Modules - an exception of the exporter reaches the caller of the importer") {
	forEachLinkBackend([](NautilusEngine& engine) {
		auto library = compileLibrary(engine);
		auto query = engine.createModule("nes.q");
		auto checked = query.importFunction<int32_t(int32_t)>("lib.math", "checked");
		query.registerFunction<val<int32_t>(val<int32_t>)>(
		    "run", [checked](val<int32_t> x) -> val<int32_t> { return checked(x) * 2; });
		auto compiled = query.compile();
		auto run = compiled.getFunction<int32_t(int32_t)>("run");
		REQUIRE(run(3) == 8);
		REQUIRE_THROWS_AS(run(-1), std::runtime_error);
		// and again: no frame is left behind by the exception
		REQUIRE(run(4) == 10);
		REQUIRE_THROWS_AS(run(-7), std::runtime_error);
	});
}

TEST_CASE("Modules - the importer sees the exporter's guarantees") {
	forEachLinkBackend([](NautilusEngine& engine) {
		auto lib = engine.createModule("lib.math");
		lib.registerFunction("hash", linkHash);
		FunctionAttributes pure;
		pure.modRefInfo = ModRefInfo::NoModRef;
		pure.noUnwind = true;
		pure.willReturn = true;
		lib.setFunctionAttributes("hash", pure);
		auto library = lib.compile();

		SECTION("an import takes them over") {
			auto query = engine.createModule("nes.q");
			auto hash = query.importFunction<uint64_t(uint64_t)>("lib.math", "hash");
			query.registerFunction<val<uint64_t>(val<uint64_t>)>("run",
			                                                     [hash](val<uint64_t> x) { return hash(x) + 1; });
			auto compiled = query.compile();
			REQUIRE(compiled.getFunction<uint64_t(uint64_t)>("run")(2) == 2 * 31 + 7 + 1);
			if (engine.isCompiled()) {
				const auto module = engine.moduleRegistry()->find("nes.q")->module();
				REQUIRE(module != nullptr);
				REQUIRE(module->imports.size() == 1);
				REQUIRE((module->imports[0].attrs & nir::AttrNoUnwind) != 0);
				// the exporter's hash is recorded, to key compiled code that inlines it (13 §4.4.3)
				REQUIRE(engine.moduleRegistry()->find("nes.q")->imports()->bindings[0].exporterHash.has_value());
			}
		}
		SECTION("an import can assume less") {
			auto query = engine.createModule("nes.q");
			auto hash = query.importFunction<uint64_t(uint64_t)>("lib.math", "hash", FunctionAttributes {});
			query.registerFunction<val<uint64_t>(val<uint64_t>)>("run", [hash](val<uint64_t> x) { return hash(x); });
			auto compiled = query.compile();
			REQUIRE(compiled.getFunction<uint64_t(uint64_t)>("run")(2) == 2 * 31 + 7);
		}
	});
}

TEST_CASE("Modules - the module, the function, the signature and the guarantees are checked") {
	auto engine = testing::makeEngine(linkBackends().back());
	if (!engine.isCompiled()) {
		SKIP("linking is checked on compiling engines");
	}

	SECTION("a module that is not registered") {
		auto query = engine.createModule("nes.q");
		auto hash = query.importFunction<uint64_t(uint64_t)>("lib.nothing", "hash");
		query.registerFunction<val<uint64_t>(val<uint64_t>)>("run", [hash](val<uint64_t> x) { return hash(x); });
		REQUIRE_THROWS_WITH(query.compile(),
		                    Catch::Matchers::ContainsSubstring("module 'lib.nothing' is not registered"));
	}
	auto library = compileLibrary(engine);
	SECTION("a function that is not exported") {
		auto query = engine.createModule("nes.q");
		REQUIRE_THROWS_AS(query.importFunction<uint64_t(uint64_t)>("lib.math", "nope"), link::LinkError);
	}
	SECTION("the early check of importFunction finds a signature mismatch") {
		auto query = engine.createModule("nes.q");
		REQUIRE_THROWS_WITH(query.importFunction<int32_t(int32_t)>("lib.math", "hash"),
		                    Catch::Matchers::ContainsSubstring("signature mismatch"));
		REQUIRE_THROWS_AS(query.importFunction<uint64_t(uint64_t, uint64_t)>("lib.math", "hash"), link::LinkError);
	}
	SECTION("a signature mismatch is found by the link when the exporter was not compiled yet") {
		auto query = engine.createModule("nes.q");
		auto later = query.importFunction<int32_t(int32_t)>("lib.later", "f");
		query.registerFunction<val<int32_t>(val<int32_t>)>("run", [later](val<int32_t> x) { return later(x); });
		auto lib = engine.createModule("lib.later");
		lib.registerFunction("f", linkHash);
		auto laterLibrary = lib.compile();
		REQUIRE_THROWS_WITH(query.compile(), Catch::Matchers::ContainsSubstring("signature mismatch"));
	}
	SECTION("a guarantee the export does not give") {
		auto query = engine.createModule("nes.q");
		FunctionAttributes assumed;
		assumed.noUnwind = true;
		auto hash = query.importFunction<uint64_t(uint64_t)>("lib.math", "hash", assumed);
		query.registerFunction<val<uint64_t>(val<uint64_t>)>("run", [hash](val<uint64_t> x) { return hash(x); });
		REQUIRE_THROWS_WITH(query.compile(), Catch::Matchers::ContainsSubstring("assumes the guarantees"));
	}
	SECTION("a module cannot import from itself") {
		auto selfish = engine.createModule("selfish");
		auto f = selfish.importFunction<uint64_t(uint64_t)>("selfish", "f");
		selfish.registerFunction<val<uint64_t>(val<uint64_t>)>("f", [f](val<uint64_t> x) { return f(x); });
		REQUIRE_THROWS_WITH(selfish.compile(), Catch::Matchers::ContainsSubstring("cycle"));
	}
	SECTION("a failed link leaves the name free") {
		{
			auto query = engine.createModule("nes.q");
			auto hash = query.importFunction<uint64_t(uint64_t)>("lib.nothing", "hash");
			query.registerFunction<val<uint64_t>(val<uint64_t>)>("run", [hash](val<uint64_t> x) { return hash(x); });
			REQUIRE_THROWS_AS(query.compile(), link::LinkError);
		}
		REQUIRE_FALSE(engine.moduleRegistry()->stageOf("nes.q").has_value());
		auto query = engine.createModule("nes.q");
		query.registerFunction("run", linkHash);
		REQUIRE_NOTHROW(query.compile());
	}
}

TEST_CASE("Modules - a name refers to one module and is released with it") {
	forEachLinkBackend([](NautilusEngine& engine) {
		auto registry = engine.moduleRegistry();
		{
			auto library = compileLibrary(engine);
			REQUIRE(registry->stageOf("lib.math") == link::ModuleStage::Compiled);
			auto again = engine.createModule("lib.math");
			again.registerFunction("hash", linkHash);
			REQUIRE_THROWS_WITH(again.compile(), Catch::Matchers::ContainsSubstring("is registered"));
			// the first module is untouched
			REQUIRE(registry->stageOf("lib.math") == link::ModuleStage::Compiled);
		}
		REQUIRE_FALSE(registry->stageOf("lib.math").has_value());
		REQUIRE_NOTHROW(compileLibrary(engine));
	});
}

TEST_CASE("Modules - an exporter outlives its importers") {
	forEachLinkBackend([](NautilusEngine& engine) {
		if (!engine.isCompiled()) {
			// an interpreted importer looks the exporter up by name when it is called
			return;
		}
		auto registry = engine.moduleRegistry();
		std::optional<CompiledModule> library = compileLibrary(engine);
		std::weak_ptr<details::ModuleState> libraryWatch = library->getState();

		std::optional<CompiledModule> query;
		{
			auto module = engine.createModule("nes.q");
			auto hash = module.importFunction<uint64_t(uint64_t)>("lib.math", "hash");
			module.registerFunction<val<uint64_t>(val<uint64_t>)>("run", [hash](val<uint64_t> x) { return hash(x); });
			query.emplace(module.compile());
		}
		std::optional<ModuleFunction<uint64_t(uint64_t)>> run(query->getFunction<uint64_t(uint64_t)>("run"));

		// the user lets go of the exporter: the importer pins it, and still runs
		library.reset();
		REQUIRE_FALSE(libraryWatch.expired());
		REQUIRE(registry->stageOf("lib.math") == link::ModuleStage::Compiled);
		REQUIRE((*run)(2) == 2 * 31 + 7);

		// the importer's functions are handles of its state: with them gone, the importer is released, then the
		// exporter
		run.reset();
		query.reset();
		REQUIRE(libraryWatch.expired());
		REQUIRE(registry->size() == 0);
	});
}

TEST_CASE("Modules - modules chain: an importer is an exporter for the next") {
	forEachLinkBackend([](NautilusEngine& engine) {
		auto library = compileLibrary(engine);

		auto middle = engine.createModule("mid");
		auto hash = middle.importFunction<uint64_t(uint64_t)>("lib.math", "hash");
		middle.registerFunction<val<uint64_t>(val<uint64_t>)>("hash2", [hash](val<uint64_t> x) { return hash(x) + 1; });
		auto middleCompiled = middle.compile();

		auto top = engine.createModule("top");
		auto hash2 = top.importFunction<uint64_t(uint64_t)>("mid", "hash2");
		top.registerFunction<val<uint64_t>(val<uint64_t>)>("run", [hash2](val<uint64_t> x) { return hash2(x) * 2; });
		auto topCompiled = top.compile();

		REQUIRE(topCompiled.getFunction<uint64_t(uint64_t)>("run")(3) == (3 * 31 + 7 + 1) * 2);
		REQUIRE((!engine.isCompiled() || engine.moduleRegistry()->importsTransitively("top", "lib.math")));
		REQUIRE_FALSE(engine.moduleRegistry()->importsTransitively("lib.math", "top"));
	});
}

TEST_CASE("Modules - an anonymous module can import") {
	forEachLinkBackend([](NautilusEngine& engine) {
		auto library = compileLibrary(engine);
		auto query = engine.createModule();
		auto hash = query.importFunction<uint64_t(uint64_t)>("lib.math", "hash");
		query.registerFunction<val<uint64_t>(val<uint64_t>)>("run", [hash](val<uint64_t> x) { return hash(x); });
		auto compiled = query.compile();
		REQUIRE(compiled.getFunction<uint64_t(uint64_t)>("run")(1) == 38);
		// nobody can import from it
		REQUIRE(engine.moduleRegistry()->size() == 1);
	});
}

TEST_CASE("Modules - the module name is in the NIR and not in the hash") {
	auto engine = testing::makeEngine(linkBackends().back());
	if (!engine.isCompiled()) {
		SKIP("needs a compiling engine");
	}
	auto first = engine.createModule("lib.a");
	first.registerFunction("hash", linkHash);
	auto firstCompiled = first.compile();
	auto second = engine.createModule("lib.b");
	second.registerFunction("hash", linkHash);
	auto secondCompiled = second.compile();

	auto a = engine.moduleRegistry()->find("lib.a");
	auto b = engine.moduleRegistry()->find("lib.b");
	REQUIRE(a->module()->name(a->module()->module.nameId) == "lib.a");
	REQUIRE(b->module()->name(b->module()->module.nameId) == "lib.b");
	// equal modules under different names share cache entries (13 §4.4)
	REQUIRE(a->structuralHash().has_value());
	REQUIRE(a->structuralHash() == b->structuralHash());
}

// ---- tiers ---------------------------------------------------------------------------------------------------------

namespace {

std::pair<std::string, std::string> linkTiers() {
#if defined(ENABLE_TRACING) && defined(ENABLE_ASMJIT_BACKEND) && defined(ENABLE_MLIR_BACKEND)
	return {"asmjit", "mlir"};
#elif defined(ENABLE_TRACING) && defined(ENABLE_TBC_BACKEND) && defined(ENABLE_MLIR_BACKEND)
	return {"tbc", "mlir"};
#elif defined(ENABLE_TRACING) && defined(ENABLE_ASMJIT_BACKEND) && defined(ENABLE_C_BACKEND)
	return {"asmjit", "cpp"};
#elif defined(ENABLE_TRACING) && defined(ENABLE_TBC_BACKEND) && defined(ENABLE_C_BACKEND)
	return {"tbc", "cpp"};
#else
	return {"", ""};
#endif
}

} // namespace

TEST_CASE("Modules - the promotion of an exporter is seen by its importers") {
	const auto [tier0, tier1] = linkTiers();
	if (tier0.empty()) {
		SKIP("Need two compilation backends for tiered compilation");
	}
	TieredCompilationConfig config;
	config.tier0.backend = tier0;
	config.tier1.backend = tier1;
	common::ArenaPool traceArenaPool;
	common::ArenaPool irArenaPool;
	auto tieredJit = std::make_unique<compiler::TieredJITCompiler>(Options(), config, traceArenaPool, irArenaPool);
	auto* jit = tieredJit.get();
	auto engine = NautilusEngine(std::move(tieredJit));

	auto library = compileLibrary(engine);
	const auto* libraryTier0 = library.getExecutable();
	auto query = engine.createModule("nes.q");
	auto hash = query.importFunction<uint64_t(uint64_t)>("lib.math", "hash");
	auto checked = query.importFunction<int32_t(int32_t)>("lib.math", "checked");
	query.registerFunction<val<uint64_t>(val<uint64_t>)>("run", [hash](val<uint64_t> x) { return hash(x) + 1; });
	query.registerFunction<val<int32_t>(val<int32_t>)>("check", [checked](val<int32_t> x) { return checked(x) * 2; });
	auto compiled = query.compile();
	auto run = compiled.getFunction<uint64_t(uint64_t)>("run");
	auto check = compiled.getFunction<int32_t(int32_t)>("check");

	const auto link = engine.moduleRegistry()->find("nes.q")->imports();
	void* const entryBefore = link->slots.load(0);
	REQUIRE(run(2) == 2 * 31 + 7 + 1);
	REQUIRE(check(1) == 4);
	REQUIRE_THROWS_AS(check(-1), std::runtime_error);

	jit->waitForPendingPromotions();

	// the exporter runs other code now, the slot names it, and the importer (itself promoted) still computes the same
	REQUIRE(library.getExecutable() != libraryTier0);
	REQUIRE(link->slots.load(0) != entryBefore);
	REQUIRE(link->retainedExecutables() >= 2);
	REQUIRE(run(2) == 2 * 31 + 7 + 1);
	REQUIRE(check(1) == 4);
	REQUIRE_THROWS_AS(check(-1), std::runtime_error);
	REQUIRE(check(5) == 12);
}

TEST_CASE("Modules - calls run while the exporter is promoted") {
	const auto [tier0, tier1] = linkTiers();
	if (tier0.empty()) {
		SKIP("Need two compilation backends for tiered compilation");
	}
	TieredCompilationConfig config;
	config.tier0.backend = tier0;
	config.tier1.backend = tier1;
	common::ArenaPool traceArenaPool;
	common::ArenaPool irArenaPool;
	auto tieredJit = std::make_unique<compiler::TieredJITCompiler>(Options(), config, traceArenaPool, irArenaPool);
	auto* jit = tieredJit.get();
	auto engine = NautilusEngine(std::move(tieredJit));

	auto library = compileLibrary(engine);
	auto query = engine.createModule("nes.q");
	auto hash = query.importFunction<uint64_t(uint64_t)>("lib.math", "hash");
	query.registerFunction<val<uint64_t>(val<uint64_t>)>("run", [hash](val<uint64_t> x) { return hash(x) + 1; });
	auto compiled = query.compile();
	auto run = compiled.getFunction<uint64_t(uint64_t)>("run");

	// the threads call until the promotions are done; a call never sees anything but the right result
	std::atomic<bool> done {false};
	std::atomic<int> wrong {0};
	std::vector<std::thread> callers;
	for (int t = 0; t < 4; ++t) {
		callers.emplace_back([&, run]() mutable {
			for (uint64_t x = 0; !done.load(); ++x) {
				if (run(x) != x * 31 + 7 + 1) {
					++wrong;
				}
			}
		});
	}
	jit->waitForPendingPromotions();
	done = true;
	for (auto& caller : callers) {
		caller.join();
	}
	REQUIRE(wrong == 0);
}

} // namespace nautilus::engine

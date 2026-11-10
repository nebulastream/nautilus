#include "nautilus/Engine.hpp"
#include "nautilus/Executable.hpp"
#include "nautilus/link/ModuleRegistry.hpp"
#include <atomic>
#include <catch2/catch_all.hpp>
#include <memory>
#include <thread>

// The module registry and the import slots (docs/plans/nautilus-0.4/13-nir-spec.md §4.4, plan M4.6) without an engine:
// modules are fabricated from an executable that hands out chosen entry points, so that each rule of linking and each
// step of the lifecycle is checked alone. The engine-level behavior (tracing, backends, promotion) is
// ModuleLinkingTest.cpp.

namespace nautilus::link {

namespace {

using engine::details::ModuleState;
using nir::makeTypeWord;
using nir::TypeId;

int64_t entryA(int64_t x) {
	return x + 1;
}
int64_t entryB(int64_t x) {
	return x + 2;
}

/// An executable whose every function is `entry`.
class FakeExecutable : public compiler::Executable {
public:
	explicit FakeExecutable(void* entry) : entry_(entry) {
	}
	void* getInvocableFunctionPtr(const std::string&) override {
		return entry_;
	}
	bool hasInvocableFunctionPtr() override {
		return true;
	}

private:
	void* entry_;
};

const std::vector<uint32_t> i64ToI64 {makeTypeWord(TypeId::I64), makeTypeWord(TypeId::I64)};
const std::vector<uint32_t> i32ToI32 {makeTypeWord(TypeId::I32), makeTypeWord(TypeId::I32)};

std::shared_ptr<ModuleState> stateOf(void* entry) {
	auto state = std::make_shared<ModuleState>();
	state->executable = std::make_shared<FakeExecutable>(entry);
	return state;
}

/// A compiled module `name` that exports `hash` (i64 -> i64) with the guarantees `attrs`.
std::shared_ptr<ModuleState> compiledModule(ModuleRegistry& registry, const std::string& name, void* entry,
                                            uint32_t attrs = 0) {
	auto handle = registry.begin(name);
	auto state = stateOf(entry);
	handle->publish(state, {ExportInfo {"hash", i64ToI64, attrs}});
	return state;
}

ImportRequest importOf(const std::string& module, const std::string& function = "hash",
                       std::vector<uint32_t> signature = i64ToI64, uint32_t attrs = 0) {
	ImportRequest request;
	request.module = module;
	request.function = function;
	request.signature = std::move(signature);
	request.attrs = attrs;
	return request;
}

} // namespace

TEST_CASE("Registry - a module walks built, linked, compiled, released") {
	auto registry = std::make_shared<ModuleRegistry>();
	auto handle = registry->begin("lib.math");
	REQUIRE(handle->stage() == ModuleStage::Built);
	REQUIRE(registry->stageOf("lib.math") == ModuleStage::Built);
	// not visible to importers before it is compiled
	REQUIRE_FALSE(registry->findExport("lib.math", "hash").has_value());

	REQUIRE(handle->link({}) != nullptr);
	REQUIRE(handle->stage() == ModuleStage::Linked);

	auto state = stateOf(reinterpret_cast<void*>(&entryA));
	handle->publish(state, {ExportInfo {"hash", i64ToI64, 0}});
	REQUIRE(handle->stage() == ModuleStage::Compiled);
	REQUIRE(registry->stageOf("lib.math") == ModuleStage::Compiled);
	REQUIRE(registry->findExport("lib.math", "hash").has_value());
	REQUIRE(registry->compiledState("lib.math") == state);
	REQUIRE(registry->size() == 1);

	// the module owns its membership: with the state and the handle gone the name is free
	handle.reset();
	REQUIRE(registry->stageOf("lib.math") == ModuleStage::Compiled);
	state.reset();
	REQUIRE_FALSE(registry->stageOf("lib.math").has_value());
	REQUIRE(registry->size() == 0);
}

TEST_CASE("Registry - a built module cannot be linked twice or published twice") {
	auto registry = std::make_shared<ModuleRegistry>();
	auto handle = registry->begin("m");
	handle->link({});
	REQUIRE_THROWS_AS(handle->link({}), LinkError);
	auto state = stateOf(reinterpret_cast<void*>(&entryA));
	handle->publish(state, {});
	REQUIRE_THROWS_AS(handle->publish(state, {}), LinkError);
}

TEST_CASE("Registry - a name refers to one module") {
	auto registry = std::make_shared<ModuleRegistry>();
	auto first = registry->begin("lib.math");
	REQUIRE_THROWS_AS(registry->begin("lib.math"), LinkError);
	// anonymous modules are not registered and may repeat
	REQUIRE_NOTHROW(registry->begin(""));
	REQUIRE_NOTHROW(registry->begin(""));
	first.reset();
	REQUIRE_NOTHROW(registry->begin("lib.math"));
}

TEST_CASE("Registry - an import binds the exporter's entry point") {
	auto registry = std::make_shared<ModuleRegistry>();
	auto exporter = compiledModule(*registry, "lib.math", reinterpret_cast<void*>(&entryA));

	auto importer = registry->begin("nes.q");
	auto link = importer->link(std::vector {importOf("lib.math")});
	REQUIRE(importer->stage() == ModuleStage::Linked);
	REQUIRE(link->slots.size() == 1);
	REQUIRE(link->slots.load(0) == reinterpret_cast<void*>(&entryA));
	REQUIRE(*static_cast<void* const*>(link->slots.address(0)) == reinterpret_cast<void*>(&entryA));
	REQUIRE(link->bindings[0].exporterState == exporter);
	REQUIRE(importer->importedModules() == std::vector<std::string> {"lib.math"});
}

TEST_CASE("Registry - linking is rejected for a missing or uncompiled exporter or function") {
	auto registry = std::make_shared<ModuleRegistry>();
	auto importer = registry->begin("nes.q");

	SECTION("no such module") {
		REQUIRE_THROWS_WITH(importer->link(std::vector {importOf("lib.math")}),
		                    Catch::Matchers::ContainsSubstring("module 'lib.math' is not registered"));
	}
	SECTION("the exporter is not compiled") {
		auto exporter = registry->begin("lib.math");
		REQUIRE_THROWS_WITH(importer->link(std::vector {importOf("lib.math")}),
		                    Catch::Matchers::ContainsSubstring("not compiled (it is built)"));
	}
	SECTION("no such function") {
		auto exporter = compiledModule(*registry, "lib.math", reinterpret_cast<void*>(&entryA));
		REQUIRE_THROWS_WITH(importer->link(std::vector {importOf("lib.math", "nope")}),
		                    Catch::Matchers::ContainsSubstring("has no exported function 'nope'"));
	}
	// a failed link binds nothing: the importer is still built, and links once the problem is gone
	REQUIRE(importer->stage() == ModuleStage::Built);
	REQUIRE(importer->imports() == nullptr);
}

TEST_CASE("Registry - linking needs the exact signature") {
	auto registry = std::make_shared<ModuleRegistry>();
	auto exporter = compiledModule(*registry, "lib.math", reinterpret_cast<void*>(&entryA));
	auto importer = registry->begin("nes.q");

	SECTION("another type") {
		REQUIRE_THROWS_WITH(importer->link(std::vector {importOf("lib.math", "hash", i32ToI32)}),
		                    Catch::Matchers::ContainsSubstring("signature mismatch") &&
		                        Catch::Matchers::ContainsSubstring("i32 (i32)") &&
		                        Catch::Matchers::ContainsSubstring("i64 (i64)"));
	}
	SECTION("another arity") {
		REQUIRE_THROWS_AS(
		    importer->link(std::vector {importOf(
		        "lib.math", "hash", {makeTypeWord(TypeId::I64), makeTypeWord(TypeId::I64), makeTypeWord(TypeId::I64)})}),
		    LinkError);
	}
	SECTION("another extension kind") {
		// the same i32 with another widening is another signature (13 §4.5.1)
		auto narrow = compiledModule(*registry, "lib.narrow", reinterpret_cast<void*>(&entryA));
		auto other = registry->begin("other");
		auto exporter2 = registry->begin("lib.zext");
		auto zextState = stateOf(reinterpret_cast<void*>(&entryA));
		exporter2->publish(zextState,
		                   {ExportInfo {"f", {makeTypeWord(TypeId::I32, nir::ExtKind::Zext), makeTypeWord(TypeId::I32, nir::ExtKind::Zext)}, 0}});
		REQUIRE_THROWS_AS(other->link(std::vector {importOf(
		                      "lib.zext", "f",
		                      {makeTypeWord(TypeId::I32, nir::ExtKind::Sext), makeTypeWord(TypeId::I32, nir::ExtKind::Sext)})}),
		                  LinkError);
		REQUIRE_NOTHROW(other->link(std::vector {importOf(
		    "lib.zext", "f",
		    {makeTypeWord(TypeId::I32, nir::ExtKind::Zext), makeTypeWord(TypeId::I32, nir::ExtKind::Zext)})}));
	}
}

TEST_CASE("Registry - an export has to give the guarantees an import assumes") {
	auto registry = std::make_shared<ModuleRegistry>();
	const auto noUnwind = nir::AttrNoUnwind;
	const auto pure = nir::AttrNoArgRead | nir::AttrNoOtherRead | nir::AttrNoArgWrite | nir::AttrNoOtherWrite;
	auto exporter = compiledModule(*registry, "lib.math", reinterpret_cast<void*>(&entryA), noUnwind);

	SECTION("an importer that assumes nothing links to every export") {
		auto importer = registry->begin("a");
		REQUIRE_NOTHROW(importer->link(std::vector {importOf("lib.math", "hash", i64ToI64, 0)}));
	}
	SECTION("a guarantee the export gives") {
		auto importer = registry->begin("a");
		REQUIRE_NOTHROW(importer->link(std::vector {importOf("lib.math", "hash", i64ToI64, noUnwind)}));
	}
	SECTION("a guarantee the export does not give") {
		auto importer = registry->begin("a");
		REQUIRE_THROWS_WITH(importer->link(std::vector {importOf("lib.math", "hash", i64ToI64, noUnwind | pure)}),
		                    Catch::Matchers::ContainsSubstring("assumes the guarantees"));
		REQUIRE(importer->stage() == ModuleStage::Built);
	}
	SECTION("a bit that is not a guarantee is not compared") {
		auto importer = registry->begin("a");
		REQUIRE_NOTHROW(importer->link(std::vector {importOf("lib.math", "hash", i64ToI64, nir::AttrDeclared)}));
	}
}

TEST_CASE("Registry - the import graph is acyclic") {
	auto registry = std::make_shared<ModuleRegistry>();

	SECTION("a module cannot import from itself") {
		auto handle = registry->begin("loop");
		REQUIRE_THROWS_WITH(handle->link(std::vector {importOf("loop")}),
		                    Catch::Matchers::ContainsSubstring("cycle, a module cannot import from itself"));
	}
	SECTION("imports are followed through other modules") {
		// a <- b <- c: c imports b, b imports a
		auto a = compiledModule(*registry, "a", reinterpret_cast<void*>(&entryA));
		auto bHandle = registry->begin("b");
		auto bLink = bHandle->link(std::vector {importOf("a")});
		auto bState = stateOf(reinterpret_cast<void*>(&entryA));
		bHandle->publish(bState, {ExportInfo {"hash", i64ToI64, 0}});
		auto cHandle = registry->begin("c");
		auto cLink = cHandle->link(std::vector {importOf("b")});

		REQUIRE(registry->importsTransitively("c", "b"));
		REQUIRE(registry->importsTransitively("c", "a"));
		REQUIRE(registry->importsTransitively("b", "a"));
		REQUIRE_FALSE(registry->importsTransitively("a", "c"));
		REQUIRE_FALSE(registry->importsTransitively("a", "b"));
		REQUIRE_FALSE(registry->importsTransitively("b", "c"));
	}
}

TEST_CASE("Registry - a promotion of the exporter is seen by the importer") {
	auto registry = std::make_shared<ModuleRegistry>();
	auto exporter = compiledModule(*registry, "lib.math", reinterpret_cast<void*>(&entryA));
	auto importer = registry->begin("nes.q");
	auto link = importer->link(std::vector {importOf("lib.math")});
	auto* const before = link->slots.load(0);
	REQUIRE(before == reinterpret_cast<void*>(&entryA));
	const auto retainedBefore = link->retainedExecutables();

	// what the tiered compiler does: install the new code and bump the version, with the mutex held
	auto promoted = std::make_shared<FakeExecutable>(reinterpret_cast<void*>(&entryB));
	{
		std::unique_lock lock(exporter->mutex);
		exporter->executable = promoted;
		exporter->version.fetch_add(1, std::memory_order_release);
	}
	REQUIRE(link->slots.load(0) == reinterpret_cast<void*>(&entryB));
	// the code the slot used to name may still be running: it is kept
	REQUIRE(link->retainedExecutables() == retainedBefore + 1);

	// swapping to a module without code leaves the slot where it was
	{
		std::unique_lock lock(exporter->mutex);
		exporter->executable = nullptr;
		exporter->version.fetch_add(1, std::memory_order_release);
	}
	REQUIRE(link->slots.load(0) == reinterpret_cast<void*>(&entryB));
}

TEST_CASE("Registry - a promotion that races with the link is not lost") {
	auto registry = std::make_shared<ModuleRegistry>();
	auto exporter = compiledModule(*registry, "lib.math", reinterpret_cast<void*>(&entryA));
	auto promoted = std::make_shared<FakeExecutable>(reinterpret_cast<void*>(&entryB));

	std::atomic<bool> go {false};
	std::thread promoter([&] {
		while (!go.load()) {
		}
		std::unique_lock lock(exporter->mutex);
		exporter->executable = promoted;
		exporter->version.fetch_add(1, std::memory_order_release);
	});
	std::vector<std::pair<std::shared_ptr<ModuleHandle>, std::shared_ptr<const ImportLink>>> importers;
	go = true;
	for (int i = 0; i < 200; ++i) {
		auto importer = registry->begin("");
		importers.emplace_back(importer, importer->link(std::vector {importOf("lib.math")}));
	}
	promoter.join();
	// whenever each link happened relative to the promotion, it ends on the promoted code
	for (const auto& [handle, link] : importers) {
		REQUIRE(link->slots.load(0) == reinterpret_cast<void*>(&entryB));
	}
}

TEST_CASE("Registry - slots are read while they are published") {
	ImportSlots slots(2);
	std::atomic<bool> stop {false};
	std::atomic<int> bad {0};
	void* const a = reinterpret_cast<void*>(&entryA);
	void* const b = reinterpret_cast<void*>(&entryB);
	slots.publish(0, a);
	std::thread reader([&] {
		while (!stop.load()) {
			auto* entry = slots.load(0);
			if (entry != a && entry != b) {
				++bad;
			}
		}
	});
	for (int i = 0; i < 100000; ++i) {
		slots.publish(0, i % 2 == 0 ? b : a);
	}
	stop = true;
	reader.join();
	REQUIRE(bad == 0);
	REQUIRE(slots.load(1) == nullptr);
}

TEST_CASE("Registry - an exporter is released after its importers") {
	auto registry = std::make_shared<ModuleRegistry>();
	auto exporter = compiledModule(*registry, "lib.math", reinterpret_cast<void*>(&entryA));
	std::weak_ptr<ModuleState> exporterWatch = exporter;

	auto importerHandle = registry->begin("nes.q");
	auto importerLink = importerHandle->link(std::vector {importOf("lib.math")});
	auto importerState = stateOf(reinterpret_cast<void*>(&entryB));
	importerHandle->publish(importerState, {});
	importerHandle.reset();
	importerLink.reset();
	REQUIRE(registry->size() == 2);

	// the user lets go of the exporter: the importer pins it
	exporter.reset();
	REQUIRE_FALSE(exporterWatch.expired());
	REQUIRE(registry->stageOf("lib.math") == ModuleStage::Compiled);
	REQUIRE(registry->findExport("lib.math", "hash").has_value());

	// a module cannot take the name of one that is still pinned
	REQUIRE_THROWS_AS(registry->begin("lib.math"), LinkError);

	// the importer goes: the exporter follows, and its name is free
	importerState.reset();
	REQUIRE(exporterWatch.expired());
	REQUIRE(registry->size() == 0);
	REQUIRE_NOTHROW(registry->begin("lib.math"));
}

TEST_CASE("Registry - the structural hash of the exporter is recorded at the link") {
	auto registry = std::make_shared<ModuleRegistry>();
	// a module without NIR (an interpreted one) has no hash to record
	auto exporter = compiledModule(*registry, "lib.math", reinterpret_cast<void*>(&entryA));
	auto importer = registry->begin("nes.q");
	auto link = importer->link(std::vector {importOf("lib.math")});
	REQUIRE_FALSE(link->bindings[0].exporterHash.has_value());
}

} // namespace nautilus::link

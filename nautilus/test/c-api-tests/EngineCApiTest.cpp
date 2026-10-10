#include "CApiTestUtil.hpp"
#include "nautilus/c/engine.h"
#include "nautilus/c/ir.h"
#include <catch2/catch_all.hpp>
#include <memory>
#include <string>

using namespace nautilus::testing;

namespace {

struct EngineDeleter {
	void operator()(NautilusOpaqueEngine* engine) const {
		nautilus_engine_dispose(engine);
	}
};
using Engine = std::unique_ptr<NautilusOpaqueEngine, EngineDeleter>;

struct OptionsDeleter {
	void operator()(NautilusIROpaqueOptions* options) const {
		nautilus_ir_options_dispose(options);
	}
};
using Options = std::unique_ptr<NautilusIROpaqueOptions, OptionsDeleter>;

Engine engineWithBackend(const char* backend) {
	Options options(nautilus_ir_options_create());
	nautilus_ir_options_set_string(options.get(), "engine.backend", backend);
	Engine engine(nautilus_engine_create(options.get()));
	REQUIRE(engine);
	return engine;
}

/// `return 2 * 3`, which the IR passes fold to a single constant.
Graph foldableGraph(NautilusIRFunctionRef* function) {
	Graph graph(nautilus_ir_graph_create("foldable"));
	auto* fb = nautilus_ir_function_builder_create(graph.get(), "folded", NAUTILUS_IR_TYPE_I32);
	auto* entry = nautilus_ir_function_builder_add_block(fb, nullptr, 0);
	auto* two = nautilus_ir_build_const_int(fb, entry, 2, NAUTILUS_IR_TYPE_I32);
	auto* three = nautilus_ir_build_const_int(fb, entry, 3, NAUTILUS_IR_TYPE_I32);
	nautilus_ir_build_return(fb, entry, nautilus_ir_build_binary(fb, entry, NAUTILUS_IR_BINARY_MUL, two, three));
	*function = nautilus_ir_function_builder_finish(fb);
	REQUIRE(*function != nullptr);
	return graph;
}

size_t entryOperationCount(NautilusIRFunctionRef function) {
	return nautilus_ir_block_get_operation_count(nautilus_ir_function_get_block(function, 0));
}

} // namespace

TEST_CASE("C engine API: an engine compiles graphs with its pinned backend") {
	for (const char* backend : {"bc", "tbc", "cpp", "asmjit", "mlir"}) {
		if (!nautilus_ir_backend_is_available(backend)) {
			continue;
		}
		DYNAMIC_SECTION("backend " << backend) {
			auto engine = engineWithBackend(backend);
			REQUIRE(std::string(nautilus_engine_get_backend_name(engine.get())) == backend);
			auto graph = buildAll();
			Executable executable(nautilus_engine_compile(engine.get(), graph.get(), nullptr));
			INFO(lastError());
			REQUIRE(executable);
			checkAllPrograms(executable.get());
		}
	}
}

TEST_CASE("C engine API: a default engine compiles with its tier-1 backend") {
	Engine engine(nautilus_engine_create(nullptr));
	REQUIRE(engine);
	REQUIRE(nautilus_engine_get_backend_name(engine.get()) != nullptr);
	auto graph = buildAll();
	Executable executable(nautilus_engine_compile(engine.get(), graph.get(), nullptr));
	INFO(lastError());
	REQUIRE(executable);
	checkAllPrograms(executable.get());
}

TEST_CASE("C engine API: executables outlive the engine that compiled them") {
	auto graph = buildAll();
	// The engine is created and disposed inside this C helper.
	Executable executable(compile_with_engine(graph.get(), "bc"));
	INFO(lastError());
	REQUIRE(executable);
	checkAllPrograms(executable.get());
}

TEST_CASE("C engine API: per-compile overrides layer on the engine options") {
	auto engine = engineWithBackend("bc");

	SECTION("engine options apply by default") {
		NautilusIRFunctionRef fn = nullptr;
		auto graph = foldableGraph(&fn);
		Executable executable(nautilus_engine_compile(engine.get(), graph.get(), nullptr));
		REQUIRE(executable);
		REQUIRE(entryOperationCount(fn) == 2);
	}

	SECTION("an override changes this compile only") {
		Options overrides(nautilus_ir_options_create());
		nautilus_ir_options_set_bool(overrides.get(), "ir.runOptimizationPasses", 0);
		NautilusIRFunctionRef fn = nullptr;
		auto graph = foldableGraph(&fn);
		Executable executable(nautilus_engine_compile(engine.get(), graph.get(), overrides.get()));
		REQUIRE(executable);
		REQUIRE(entryOperationCount(fn) == 4);
		auto folded = function<int32_t (*)()>(executable.get(), "folded");
		REQUIRE(folded() == 6);
	}
}

TEST_CASE("C engine API: a graph optimized beforehand is not optimized again") {
	auto engine = engineWithBackend("bc");
	NautilusIRFunctionRef fn = nullptr;
	auto graph = foldableGraph(&fn);
	REQUIRE(nautilus_ir_graph_optimize(graph.get(), NAUTILUS_IR_OPTIMIZE_NONE, nullptr) == 0);
	Executable executable(nautilus_engine_compile(engine.get(), graph.get(), nullptr));
	REQUIRE(executable);
	// The engine's backend would have folded the multiply; the graph kept the
	// unoptimized pipeline it was explicitly given.
	REQUIRE(entryOperationCount(fn) == 4);
	REQUIRE(function<int32_t (*)()>(executable.get(), "folded")() == 6);
}

TEST_CASE("C engine API: failures are reported") {
	auto graph = buildAll();

	SECTION("compilation disabled") {
		Options options(nautilus_ir_options_create());
		nautilus_ir_options_set_bool(options.get(), "engine.Compilation", 0);
		Engine engine(nautilus_engine_create(options.get()));
		REQUIRE(engine);
		REQUIRE(nautilus_engine_compile(engine.get(), graph.get(), nullptr) == nullptr);
		REQUIRE(lastError().find("engine.Compilation") != std::string::npos);
	}

	SECTION("unknown backend") {
		auto engine = engineWithBackend("no-such-backend");
		REQUIRE(nautilus_engine_compile(engine.get(), graph.get(), nullptr) == nullptr);
		REQUIRE(lastError().find("not available") != std::string::npos);
	}

	SECTION("unfinished builder") {
		auto engine = engineWithBackend("bc");
		REQUIRE(nautilus_ir_function_builder_create(graph.get(), "pending", NAUTILUS_IR_TYPE_VOID) != nullptr);
		REQUIRE(nautilus_engine_compile(engine.get(), graph.get(), nullptr) == nullptr);
		REQUIRE(lastError().find("finished") != std::string::npos);
	}
}

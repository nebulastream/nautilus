
#include "nautilus/Engine.hpp"
#include "nautilus/std/iostream.h"
#include "nautilus/std/ostream.h"
#include "nautilus/std/sstream.h"
#include "nautilus/std/string.h"
#include "nautilus/std/string_view.h"
#include <catch2/catch_all.hpp>

namespace nautilus::engine {

void coutTest(val<int32_t> val) {
	cout << "value:" << val << endl;
}

void cerrTest(val<int32_t> val) {
	cerr << "value:" << val << endl;
	cerr << "val" << flush;
}

/*
val<char> sstreamTest(val<int32_t> value) {
    // allocates ptr
    stringstream ss;
    ss << "val" << value << endl;
    return ss.view().data()[4];
}*/

void sstreamHexTest(val<int32_t> value) {
	// allocates a stringstream, which is managed by the runtime and freed when the object goes out of scope
	stringstream ss;
	ss << hex << value << endl;
}

void sstreamDecTest(val<int32_t> value) {
	// allocates a stringstream, which is managed by the runtime and freed when the object goes out of scope
	stringstream ss;
	ss << dec << value << endl;
}

void sstreamOctTest(val<int32_t> value) {
	// allocates a stringstream, which is managed by the runtime and freed when the object goes out of scope
	stringstream ss;
	ss << oct << value << endl;
}

val<char> sstreamToStrTest(val<int32_t> value) {
	// allocates a stringstream, which is managed by the runtime and freed when the object goes out of scope
	stringstream ss;
	ss << "val" << value << endl;
	auto str = ss.str();
	return ss.str().at(4);
}

void runOstreamTest(engine::NautilusEngine& engine) {
	SECTION("sstreamToStrTest") {
		auto f = engine.registerFunction(sstreamToStrTest);
		auto ptr = f(42);
		[[maybe_unused]] auto _ = ptr;
	}
	SECTION("cout") {
		auto f = engine.registerFunction(coutTest);
		f(42);
		f(44);
	}
	SECTION("cerr") {
		auto f = engine.registerFunction(cerrTest);
		f(42);
		f(44);
	}

	SECTION("hex") {
		auto f = engine.registerFunction(sstreamHexTest);
		f(42);
		f(23);
	}

	SECTION("dec") {
		auto f = engine.registerFunction(sstreamDecTest);
		f(42);
		f(23);
	}

	SECTION("oct") {
		auto f = engine.registerFunction(sstreamOctTest);
		f(42);
		f(23);
	}
}

TEST_CASE("OstreamTest - Interpreter") {
	engine::Options options;
	options.setOption("engine.Compilation", false);
	auto engine = engine::NautilusEngine(options);
	runOstreamTest(engine);
}

#ifdef ENABLE_TRACING
TEST_CASE("OstreamTest - Compiler") {
	std::vector<std::string> backends = {};
#ifdef ENABLE_MLIR_BACKEND
	backends.emplace_back("mlir");
#endif
#ifdef ENABLE_C_BACKEND
	backends.emplace_back("cpp");
#endif
#ifdef ENABLE_BC_BACKEND
	backends.emplace_back("bc");
#endif
#ifdef ENABLE_ASMJIT_BACKEND
	backends.emplace_back("asmjit");
#endif
	for (auto& backend : backends) {
		DYNAMIC_SECTION(backend) {
			engine::Options options;
			options.setOption("engine.backend", backend);
			auto engine = engine::NautilusEngine(options);
			runOstreamTest(engine);
		}
	}
}
#endif
} // namespace nautilus::engine

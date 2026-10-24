#include "nautilus/CompilationStatistics.hpp"
#include "nautilus/Engine.hpp"
#include <catch2/catch_all.hpp>
#include <filesystem>
#include <string>

namespace nautilus::engine {

namespace {

val<int32_t> cppTempFileAddOne(val<int32_t> x) {
	return x + 1;
}

} // namespace

// The cpp backend writes its generated source and the shared library into the
// temp directory under the compilation unit ID. Both must be gone once the
// module is compiled, or every compile leaves files behind for good.
TEST_CASE("CPP backend removes its temporary files after compiling") {
	Options options;
	options.setOption("engine.backend", std::string("cpp"));
	auto engine = NautilusEngine(options);
	auto module = engine.createModule();
	module.registerFunction("add_one", cppTempFileAddOne);
	auto compiled = module.compile();
	REQUIRE(compiled.getFunction<int32_t(int32_t)>("add_one")(41) == 42);

	const auto* unitId = compiled.getStatistics()->find("compilation.unitId");
	REQUIRE(unitId != nullptr);
	const auto base = std::filesystem::temp_directory_path() / ("nautilus_" + std::get<std::string>(*unitId));
	CHECK_FALSE(std::filesystem::exists(base.string() + ".cpp"));
#ifdef __APPLE__
	CHECK_FALSE(std::filesystem::exists(base.string() + ".dylib"));
#else
	CHECK_FALSE(std::filesystem::exists(base.string() + ".so"));
#endif
}

} // namespace nautilus::engine

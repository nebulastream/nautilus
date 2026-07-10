#include "ExecutionTest.hpp"
#include "nautilus/Engine.hpp"
#include "nautilus/config.hpp"
#include <catch2/catch_all.hpp>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <unistd.h>

#ifdef ENABLE_TRACING

namespace nautilus::engine {

namespace {

val<int32_t> irDigitSum(val<int32_t> n) {
	val<int32_t> sum = 0;
	while (n > 0) {
		sum = sum + n % 10;
		n = n / 10;
	}
	return sum;
}

val<int32_t> irNestedControlFlow(val<int32_t> n) {
	val<int32_t> result = 0;
	for (val<int32_t> i = 0; i < n; i = i + 1) {
		if (i % 3 == 0) {
			result = result + 1;
		} else if (i % 3 == 1) {
			result = result + 2;
		} else {
			result = result + 3;
		}
	}
	return result;
}

val<double> irFloatMix(val<double> x, val<int32_t> factor) {
	val<double> scaled = x * static_cast<val<double>>(factor);
	return scaled + 1.5;
}

val<int32_t> irPointerAccumulate(val<int32_t*> data, val<int32_t> length) {
	val<int32_t> sum = 0;
	for (val<int32_t> i = 0; i < length; i = i + 1) {
		sum = sum + data[i];
	}
	return sum;
}

int32_t nativeTriple(int32_t x) {
	return x * 3;
}

val<int32_t> irRuntimeCall(val<int32_t> x) {
	return invoke(nativeTriple, x + 1);
}

} // namespace

TEST_CASE("IR serialization execution round trip") {
	nautilus::testing::forEachBackend(
	    [](engine::NautilusEngine& engine) {
		    SECTION("loops and nested control flow") {
			    auto module = engine.createModule();
			    module.registerFunction("digitSum", irDigitSum);
			    module.registerFunction("nested", irNestedControlFlow);
			    const auto irText = module.serializeIR();

			    auto loaded = engine.loadModuleFromIR(irText);
			    auto digitSum = loaded.getFunction<int32_t(int32_t)>("digitSum");
			    auto nested = loaded.getFunction<int32_t(int32_t)>("nested");

			    auto reference = module.compile();
			    auto referenceDigitSum = reference.getFunction<int32_t(int32_t)>("digitSum");
			    auto referenceNested = reference.getFunction<int32_t(int32_t)>("nested");

			    for (int32_t input : {0, 1, 7, 42, 12345, 987654}) {
				    REQUIRE(digitSum(input) == referenceDigitSum(input));
				    REQUIRE(nested(input % 100) == referenceNested(input % 100));
			    }
		    }

		    SECTION("floats and casts") {
			    auto module = engine.createModule();
			    module.registerFunction("mix", irFloatMix);
			    const auto irText = module.serializeIR();

			    auto loaded = engine.loadModuleFromIR(irText);
			    auto mix = loaded.getFunction<double(double, int32_t)>("mix");
			    REQUIRE(mix(2.5, 4) == Catch::Approx(11.5));
			    REQUIRE(mix(-1.25, 2) == Catch::Approx(-1.0));
		    }

		    SECTION("pointer loads") {
			    auto module = engine.createModule();
			    module.registerFunction("accumulate", irPointerAccumulate);
			    const auto irText = module.serializeIR();

			    auto loaded = engine.loadModuleFromIR(irText);
			    auto accumulate = loaded.getFunction<int32_t(int32_t*, int32_t)>("accumulate");
			    int32_t data[] = {1, 2, 3, 4, 5};
			    REQUIRE(accumulate(data, 5) == 15);
		    }

		    SECTION("runtime call with symbol resolver") {
			    auto module = engine.createModule();
			    module.registerFunction("tripled", irRuntimeCall);
			    const auto irText = module.serializeIR();

			    // The recorded symbol may be process-specific (static function
			    // in an executable); resolve it explicitly. There is exactly
			    // one external function in this module.
			    auto resolver = [](const std::string&, const std::string&) -> void* {
				    return reinterpret_cast<void*>(&nativeTriple);
			    };
			    auto loaded = engine.loadModuleFromIR(irText, resolver);
			    auto tripled = loaded.getFunction<int32_t(int32_t)>("tripled");
			    REQUIRE(tripled(3) == 12);
			    REQUIRE(tripled(-1) == 0);
		    }

		    SECTION("load from file") {
			    auto module = engine.createModule();
			    module.registerFunction("digitSum", irDigitSum);
			    const auto irText = module.serializeIR();

			    const auto path = std::filesystem::temp_directory_path() /
			                      ("nautilus_ir_load_test_" + std::to_string(::getpid()) + ".nautilus");
			    {
				    std::ofstream file(path);
				    file << irText;
			    }
			    auto loaded = engine.loadModuleFromIRFile(path.string());
			    std::filesystem::remove(path);
			    auto digitSum = loaded.getFunction<int32_t(int32_t)>("digitSum");
			    REQUIRE(digitSum(1234) == 10);
		    }
	    },
	    /*include_interpreter*/ false);
}

TEST_CASE("IR loading requires compilation") {
	engine::Options options;
	options.setOption("engine.Compilation", false);
	auto engine = engine::NautilusEngine(options);
	REQUIRE_THROWS_WITH(engine.loadModuleFromIR("nautilus {\n} //nautilus\n"),
	                    Catch::Matchers::ContainsSubstring("compilation"));
}

TEST_CASE("IR loading reports missing files") {
	auto engine = engine::NautilusEngine();
	REQUIRE_THROWS_WITH(engine.loadModuleFromIRFile("/definitely/not/a/real/path.nautilus"),
	                    Catch::Matchers::ContainsSubstring("Could not open"));
}

} // namespace nautilus::engine

#endif // ENABLE_TRACING

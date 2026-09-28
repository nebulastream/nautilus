#include "catch2/catch_test_macros.hpp"
#include "nautilus/CompilationStatistics.hpp"
#include "nautilus/Engine.hpp"
#include "nautilus/common/ExecutableImage.hpp"
#include "nautilus/compiler/cache/PersistentModuleCache.hpp"
#include "nautilus/val.hpp"
#include <cstdlib>
#include <filesystem>
#include <string>

namespace nautilus::engine {
namespace {

class TemporaryCacheDirectory {
public:
	TemporaryCacheDirectory() {
		auto pattern = (std::filesystem::temp_directory_path() / "nautilus-missing-compiler-id-XXXXXX").string();
		REQUIRE(::mkdtemp(pattern.data()) != nullptr);
		path = pattern;
	}

	~TemporaryCacheDirectory() {
		std::error_code error;
		std::filesystem::remove_all(path, error);
	}

	std::filesystem::path path;
};

int traces = 0;

val<int32_t> increment(val<int32_t> value) {
	++traces;
	return value + 1;
}

template <typename T>
T cacheStat(const CompiledModule& module, const std::string& name) {
	auto statistics = module.getStatistics();
	REQUIRE(statistics != nullptr);
	const auto* value = statistics->find(name);
	REQUIRE(value != nullptr);
	return std::get<T>(*value);
}

} // namespace

TEST_CASE("MLIR persistent cache declines compiler images without build IDs", "[cache][compiler-identity]") {
	REQUIRE_FALSE(common::locateExecutableAddress(reinterpret_cast<const void*>(&increment)).has_value());
	if (common::locateExecutableAddress(reinterpret_cast<const void*>(&compiler::compileWithPersistentModuleCache))) {
		SKIP("The compiler is provided by a shared library with an executable image identity");
	}

	TemporaryCacheDirectory cache;
	Options options;
	options.setOption("engine.backend", std::string("mlir"));
	options.setOption("engine.Blob.CacheDir", cache.path.string());
	options.setOption("engine.Blob.CacheKey", std::string("missing-compiler-identity-v1"));
	options.setOption("mlir.enableMultithreading", false);
	for (int iteration = 0; iteration < 2; ++iteration) {
		const auto before = traces;
		NautilusEngine engine(options);
		auto module = engine.createModule();
		module.registerFunction<val<int32_t>(val<int32_t>)>("execute", increment);
		auto compiled = module.compile();
		INFO(compiled.getStatistics()->toString());
		auto execute = compiled.getFunction<int32_t(int32_t)>("execute");
		REQUIRE(execute(41) == 42);
		REQUIRE(execute(-3) == -2);
		REQUIRE(traces > before);
		REQUIRE(cacheStat<std::string>(compiled, "backend.name") == "mlir");
		REQUIRE(cacheStat<int64_t>(compiled, "cache.eligible") == 0);
		REQUIRE(cacheStat<int64_t>(compiled, "cache.tracingRan") == 1);
		REQUIRE(cacheStat<std::string>(compiled, "cache.object") == "not_used");
		REQUIRE(cacheStat<std::string>(compiled, "cache.mlir") == "not_used");
		REQUIRE(cacheStat<std::string>(compiled, "cache.fallback") == "missing_compiler_identity");
		REQUIRE_FALSE(compiled.getStatistics()->contains("cache.key"));
		REQUIRE(std::filesystem::is_empty(cache.path));
	}
}

} // namespace nautilus::engine

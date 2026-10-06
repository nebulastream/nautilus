#include <cstdint>
#include <filesystem>
#include <iostream>
#include <nautilus/CompilationStatistics.hpp>
#include <nautilus/Engine.hpp>
#include <nautilus/val.hpp>
#include <stdexcept>
#include <string>
#ifdef NAUTILUS_CACHE_EXPECTED
#include <nautilus/cache/plugin.hpp>
#endif

namespace {

void require(bool condition, const char* message) {
	if (!condition) {
		throw std::runtime_error(message);
	}
}

#ifdef NAUTILUS_CACHE_EXPECTED
template <typename T>
T statistic(const nautilus::engine::CompiledModule& module, const std::string& name) {
	const auto statistics = module.getStatistics();
	require(statistics != nullptr, "Missing compilation statistics");
	const auto* value = statistics->find(name);
	require(value != nullptr && std::holds_alternative<T>(*value), "Missing or mistyped cache statistic");
	return std::get<T>(*value);
}
#endif

} // namespace

int main(int argc, char** argv) {
	using namespace nautilus;
	using namespace nautilus::engine;
	try {
		const std::string mode = argc > 1 ? argv[1] : "ordinary";
		Options options;
#ifdef ENABLE_MLIR_BACKEND
		options.setOption("engine.backend", std::string("mlir"));
#else
		options.setOption("engine.backend", std::string("cpp"));
#endif
		options.setOption("engine.cache.directory", argc > 2 ? std::string(argv[2]) : std::string());
		options.setOption("engine.cache.key", std::string("installed-increment/i32/v1"));
		options.setOption("mlir.enableMultithreading", false);
		std::unique_ptr<NautilusEngine> engine;
#ifdef NAUTILUS_CACHE_EXPECTED
		if (mode != "ordinary") {
			require(argc == 3 && (mode == "cold" || mode == "warm" || mode == "repair"), "Invalid cache mode");
			if (mode == "cold") {
				std::filesystem::remove_all(argv[2]);
			}
			engine = std::make_unique<NautilusEngine>(cache::createCompiler(options), options);
		} else
#endif
		{
			require(mode == "ordinary", "Persistence requested from a cache-OFF package");
			engine = std::make_unique<NautilusEngine>(options);
		}
		int wrappers = 0;
		auto builder = engine->createModule();
		builder.registerFunction<val<int32_t>(val<int32_t>)>("increment", [&wrappers](val<int32_t> value) {
			++wrappers;
			return value + cacheLiteral<int32_t {7}>();
		});
		auto module = builder.compile();
		const auto increment = module.getFunction<int32_t(int32_t)>("increment");
		require(increment(35) == 42 && increment(-8) == -1, "Installed consumer result mismatch");
		if (mode == "ordinary") {
			require(wrappers > 0, "Ordinary engine bypassed its callable");
			if (const auto statistics = module.getStatistics()) {
				for (const auto& [name, value] : *statistics) {
					require(!name.starts_with("cache."), "Ordinary engine emitted cache policy");
				}
			}
		}
#ifdef NAUTILUS_CACHE_EXPECTED
		else {
			const auto object = statistic<std::string>(module, "cache.object");
			const auto bytecode = statistic<std::string>(module, "cache.mlir");
			require(statistic<int64_t>(module, "cache.tracingRan") == (mode == "cold" ? 1 : 0),
			        "Wrong trace telemetry");
			require(mode == "cold" ? wrappers > 0 : wrappers == 0, "Unexpected wrapper execution");
			if (mode == "cold") {
				require(object == "written" && bytecode == "written", "Cold entry was not published");
			} else if (mode == "warm") {
				require(object == "hit" && bytecode == "not_checked", "Warm request was not native-first");
				require(statistic<std::string>(module, "cache.fallback") == "none", "Native hit reported fallback");
			} else {
				require(object == "invalid_rewritten" && bytecode == "hit", "Repair was counted as a native hit");
			}
			if (mode != "cold") {
				const auto statistics = module.getStatistics();
				for (const auto* name : {"tracing.ms", "frontend.totalMs", "ssaCreation.ms", "irGeneration.ms"}) {
					require(!statistics->contains(name), "Warm/repair request executed the frontend");
				}
			}
		}
#endif
		std::cout << mode << " passed; wrappers=" << wrappers << '\n';
		return 0;
	} catch (const std::exception& error) {
		std::cerr << error.what() << '\n';
		return 1;
	}
}

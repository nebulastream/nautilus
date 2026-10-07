#include <cstdint>
#include <filesystem>
#include <iostream>
#include <nautilus/CompilationStatistics.hpp>
#include <nautilus/Engine.hpp>
#include <nautilus/function.hpp>
#include <nautilus/val.hpp>
#include <nautilus/val_std.hpp>
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

struct alignas(64) OwnedBuffer {
	int32_t* counts = nullptr;
	int32_t id = 0;

	OwnedBuffer() noexcept = default;
	OwnedBuffer(int32_t* counts, int32_t id) noexcept : counts(counts), id(id) {
		++counts[2];
	}
	OwnedBuffer(const OwnedBuffer& other) noexcept : OwnedBuffer(other.counts, other.id + 1) {
	}
	~OwnedBuffer() noexcept {
		if (counts) {
			counts[0] = counts[0] * 10 + id;
			++counts[1];
		}
	}
};

int32_t consumeOwned(OwnedBuffer* buffer, bool fail) {
	require(reinterpret_cast<uintptr_t>(buffer) % alignof(OwnedBuffer) == 0, "Misaligned installed owned buffer");
	++buffer->counts[3];
	if (fail) {
		throw std::runtime_error("installed owned cleanup failure");
	}
	return buffer->id;
}

void checkOwned(nautilus::engine::CompiledModule& module) {
	const auto execute = module.getFunction<int32_t(int32_t*, bool)>("owned");
	int32_t counts[4] = {};
	for (const bool fail : {false, true, false}) {
		counts[0] = counts[1] = 0;
		if (fail) {
			try {
				execute(counts, true);
				throw std::logic_error("Installed owned exception was swallowed");
			} catch (const std::runtime_error& error) {
				require(std::string(error.what()) == "installed owned cleanup failure", "Wrong installed exception");
			}
		} else {
			require(execute(counts, false) == 2, "Installed owned result mismatch");
		}
		require(counts[0] == 21 && counts[1] == 2, "Installed owned cleanup order/count mismatch");
		require(counts[2] == 2 * counts[3], "Installed owned constructor/copy count mismatch");
	}
	require(counts[3] == 3, "Installed owned recovery call count mismatch");
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
		options.setOption("engine.cache.key", std::string("installed-increment/typed-owned/v2"));
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
		int wrappers = 0, ownedWrappers = 0;
		auto builder = engine->createModule();
		builder.registerFunction<val<int32_t>(val<int32_t>)>("increment", [&wrappers](val<int32_t> value) {
			++wrappers;
			return value + cacheLiteral<int32_t {7}>();
		});
		builder.registerFunction<val<int32_t>(val<int32_t*>, val<bool>)>(
		    "owned", [&ownedWrappers](val<int32_t*> counts, val<bool> fail) {
			    ++ownedWrappers;
			    val<OwnedBuffer> empty;
			    val<OwnedBuffer> first(counts, cacheLiteral<int32_t {1}>());
			    val<OwnedBuffer> copy(first);
			    val<OwnedBuffer> moved(std::move(copy));
			    return invoke(consumeOwned, &moved, fail);
		    });
		auto module = builder.compile();
		const auto increment = module.getFunction<int32_t(int32_t)>("increment");
		require(increment(35) == 42 && increment(-8) == -1, "Installed consumer result mismatch");
		checkOwned(module);
		if (mode == "ordinary") {
			require(wrappers > 0 && ownedWrappers > 0, "Ordinary engine bypassed its callable");
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
			require(mode == "cold" ? wrappers > 0 && ownedWrappers > 0 : wrappers == 0 && ownedWrappers == 0,
			        "Unexpected wrapper execution");
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
		std::cout << mode << " passed; wrappers=" << wrappers << " ownedWrappers=" << ownedWrappers << '\n';
		return 0;
	} catch (const std::exception& error) {
		std::cerr << error.what() << '\n';
		return 1;
	}
}

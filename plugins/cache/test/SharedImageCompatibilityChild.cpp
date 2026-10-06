#include "PersistentModuleCache.hpp"
#include "nautilus/CompilationStatistics.hpp"
#include "nautilus/Engine.hpp"
#include "nautilus/cache/plugin.hpp"
#include "nautilus/common/ExecutableImage.hpp"
#include "nautilus/compiler/backends/CompilationBackend.hpp"
#include "nautilus/val.hpp"
#include <cstdint>
#include <dlfcn.h>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>
#include <variant>

namespace {

void require(bool condition, const std::string& message) {
	if (!condition) {
		throw std::runtime_error(message);
	}
}

struct Image {
	std::filesystem::path path;
	uint64_t loadOffset;
	std::optional<nautilus::common::ExecutableImageLocation> identity;
};

Image identify(const void* address, const std::filesystem::path& expected) {
	Dl_info information {};
	require(::dladdr(address, &information) != 0 && information.dli_fname && information.dli_fbase,
	        "dladdr could not identify the implementation image");
	const auto path = std::filesystem::canonical(information.dli_fname);
	require(std::filesystem::equivalent(path, expected), "implementation was not loaded from " + expected.string());
	const auto offset = reinterpret_cast<uintptr_t>(address) - reinterpret_cast<uintptr_t>(information.dli_fbase);
	auto identity = nautilus::common::locateExecutableAddress(address);
	if (identity) {
		require(identity->loadOffset == offset, "image identity does not use the actual library load bias");
		require(nautilus::common::resolveExecutableAddress(*identity) == address,
		        "implementation identity does not uniquely resolve to its real symbol");
	}
	return {path, offset, std::move(identity)};
}

template <typename T>
T statistic(const nautilus::compiler::CompilationStatistics& statistics, const std::string& name) {
	const auto* value = statistics.find(name);
	require(value && std::holds_alternative<T>(*value), "missing or incorrectly typed statistic: " + name);
	return std::get<T>(*value);
}

int run(int argc, char** argv) {
	require(argc == 7, "usage: child MODE CACHE_DIRECTORY CORE_IMAGE PROVIDER_IMAGE REPORT_DIRECTORY REPORT_NAME");
	const std::string_view mode = argv[1];
	require(mode == "miss" || mode == "hit" || mode == "core-missing" || mode == "provider-missing", "invalid mode");
	const auto core =
	    identify(reinterpret_cast<const void*>(&nautilus::compiler::CompilationBackendRegistry::getInstance), argv[3]);
	const auto provider =
	    identify(reinterpret_cast<const void*>(&nautilus::cache::detail::compileWithPersistentModuleCache), argv[4]);
	require(!std::filesystem::equivalent(core.path, provider.path), "core and cache provider share an image");
	require(core.identity.has_value() == (mode != "core-missing"), "unexpected real core build-ID availability");
	require(provider.identity.has_value() == (mode != "provider-missing"),
	        "unexpected real cache provider build-ID availability");
	if (core.identity && provider.identity) {
		require(core.identity->buildId != provider.identity->buildId, "core and provider build IDs are not distinct");
	}

	nautilus::engine::Options options;
	options.setOption("engine.backend", std::string("mlir"));
	options.setOption("engine.cache.directory", std::string(argv[2]));
	options.setOption("engine.cache.key", std::string("shared-image-compatibility-increment-v1"));
	options.setOption("mlir.enableMultithreading", false);
	nautilus::engine::NautilusEngine engine(nautilus::cache::createCompiler(options), options);
	int wrapperCalls = 0;
	auto module = engine.createModule();
	module.registerFunction<nautilus::val<int32_t>(nautilus::val<int32_t>)>(
	    "execute", [&wrapperCalls](nautilus::val<int32_t> value) {
		    ++wrapperCalls;
		    return value + nautilus::cacheLiteral<int32_t {7}>();
	    });
	auto compiled = module.compile();
	require(compiled.getExecutable() != nullptr, "ordinary compiled execution was unavailable");
	const auto statistics = compiled.getStatistics();
	require(statistics != nullptr, "compilation statistics were unavailable");
	std::cout << statistics->toString() << '\n';
	const auto object = statistic<std::string>(*statistics, "cache.object");
	const auto bytecode = statistic<std::string>(*statistics, "cache.mlir");
	const auto fallback = statistic<std::string>(*statistics, "cache.fallback");
	const auto eligible = statistic<int64_t>(*statistics, "cache.eligible");
	const auto tracing = statistic<int64_t>(*statistics, "cache.tracingRan");
	require(statistic<std::string>(*statistics, "backend.name") == "mlir", "unexpected ordinary backend");
	std::string key;
	if (statistics->contains("cache.key")) {
		key = statistic<std::string>(*statistics, "cache.key");
	}
	if (mode == "hit") {
		require(object == "hit" && bytecode == "not_checked" && fallback == "none" && eligible == 1 && tracing == 0,
		        "unchanged shared images did not yield a native warm hit");
		require(wrapperCalls == 0, "warm native lookup ran the tracing wrapper");
		for (const auto* name : {"tracing.ms", "ssaCreation.ms", "irGeneration.ms", "frontend.totalMs"}) {
			require(!statistics->contains(name), "warm native lookup entered the frontend");
		}
	} else {
		require(wrapperCalls > 0 && tracing == 1, "declined or cold lookup did not run the real tracing wrapper");
		require(statistics->contains("tracing.ms"), "ordinary compilation did not actually trace");
		if (mode == "miss") {
			require(object == "written" && bytecode == "written" && fallback == "none" && eligible == 1,
			        "identified shared images did not publish an independent cache entry");
		} else {
			const auto expected =
			    mode == "core-missing" ? "missing_compiler_identity" : "missing_cache_provider_identity";
			require(object == "not_used" && bytecode == "not_used" && fallback == expected && eligible == 0,
			        "missing shared-image identity did not fail closed with its independent reason");
			require(!statistics->contains("cache.key"), "missing shared-image identity was assigned a cache key");
		}
	}
	const auto beforeExecution = wrapperCalls;
	const auto function = compiled.getFunction<int32_t(int32_t)>("execute");
	const auto result = function(35);
	require(result == 42 && function(-8) == -1 && function(0) == 7,
	        "shared-image compilation produced an incorrect result");
	require(wrapperCalls == beforeExecution, "ordinary result came from interpreted wrapper execution");

	std::ofstream report(std::filesystem::path(argv[5]) / argv[6]);
	require(report.is_open(), "could not write child report");
	report << "core.path=" << core.path.string() << '\n'
	       << "core.build_id=" << (core.identity ? core.identity->buildId : "") << '\n'
	       << "core.load_offset=" << core.loadOffset << '\n'
	       << "provider.path=" << provider.path.string() << '\n'
	       << "provider.build_id=" << (provider.identity ? provider.identity->buildId : "") << '\n'
	       << "provider.load_offset=" << provider.loadOffset << '\n'
	       << "cache.key=" << key << '\n'
	       << "cache.object=" << object << '\n'
	       << "cache.mlir=" << bytecode << '\n'
	       << "cache.fallback=" << fallback << '\n'
	       << "cache.eligible=" << eligible << '\n'
	       << "cache.tracingRan=" << tracing << '\n'
	       << "wrapper_calls=" << wrapperCalls << '\n'
	       << "result=" << result << '\n';
	report.close();
	require(!report.fail(), "child report write failed");
	return 0;
}

} // namespace

int main(int argc, char** argv) {
	try {
		return run(argc, argv);
	} catch (const std::exception& error) {
		std::cerr << "SharedImageCompatibility: " << error.what() << '\n';
		return 1;
	}
}

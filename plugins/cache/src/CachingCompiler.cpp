#include "CacheOptions.hpp"
#include "PersistentModuleCache.hpp"
#include "nautilus/CompilableFunction.hpp"
#include "nautilus/CompilationStatistics.hpp"
#include "nautilus/Executable.hpp"
#include "nautilus/Module.hpp"
#include "nautilus/cache/plugin.hpp"
#include "nautilus/common/Arena.hpp"
#include "nautilus/compiler/CompilationPipeline.hpp"
#include "nautilus/compiler/TieredCompiler.hpp"
#include "nautilus/logging.hpp"
#include <chrono>
#include <list>
#include <memory>
#include <string>
#include <utility>
#include <variant>

namespace nautilus::cache {
namespace {

compiler::TieredJITCompiler::FinalStatisticsDecorator fallbackStatisticsDecorator() {
	return [](compiler::CompilationStatistics& statistics, const engine::ModuleOptions& moduleOptions,
	          const std::string& compilerName) {
		const auto reason = compilerName == "mlir"                ? "missing_export_signature"
		                    : compilerName.starts_with("tiered(") ? "tiered_compilation"
		                                                          : "backend_not_mlir";
		detail::recordCacheDecline(statistics, moduleOptions, reason, true);
	};
}

class CachingCompiler final : public compiler::JITCompiler {
public:
	explicit CachingCompiler(const engine::Options& options)
	    : pipeline_(detail::normalizeOptions(options), traceArenaPool_, irArenaPool_),
	      fallback_(pipeline_.getOptions(), traceArenaPool_, irArenaPool_, fallbackStatisticsDecorator()) {
	}

	std::unique_ptr<compiler::Executable> compile(wrapper_function function,
	                                              const engine::ModuleOptions& moduleOptions) const override {
		const auto normalizedOptions = detail::normalizeOptions(moduleOptions);
		return fallback_.compile(std::move(function), normalizedOptions);
	}

	std::unique_ptr<compiler::Executable> compile(std::list<compiler::CompilableFunction>& functions,
	                                              const engine::ModuleOptions& moduleOptions) const override {
		const auto normalizedOptions = detail::normalizeOptions(moduleOptions);
		if (fallback_.getName() != "mlir") {
			return fallback_.compile(functions, normalizedOptions);
		}
		const auto compilationStart = std::chrono::steady_clock::now();
		auto statistics = std::make_shared<compiler::CompilationStatistics>();
		auto executable =
		    detail::compileWithPersistentModuleCache(pipeline_, functions, normalizedOptions, statistics.get());
		if (!executable) {
			auto fallbackOptions = normalizedOptions;
			if (fallbackOptions.getOptionOrDefault("engine.logStatistics", false)) {
				fallbackOptions.setOption("engine.logStatistics", false);
			}
			executable = fallback_.compile(functions, fallbackOptions);
			if (const auto fallbackStatistics = executable->getCompilationStatistics()) {
				for (const auto& [key, value] : *fallbackStatistics) {
					if (!key.starts_with("cache.") || !statistics->contains(key)) {
						statistics->set(key, value);
					}
				}
			}
		}

		statistics->set("backend.name", std::string {"mlir"});
		statistics->set("tier", std::string {"tier1"});
		if (!statistics->contains("compilation.unitId")) {
			if (const auto* key = statistics->find("cache.key")) {
				statistics->set("compilation.unitId", "cache-" + std::get<std::string>(*key));
			}
		}
		statistics->recordTimingMs("compilation.totalMs", compilationStart);
		if (normalizedOptions.getOptionOrDefault("engine.logStatistics", false)) {
			const auto* id = statistics->find("compilation.unitId");
			log::info("\n{}", statistics->formatReport(id ? std::get<std::string>(*id) : std::string {}, "mlir"));
		}
		executable->setCompilationStatistics(std::move(statistics));
		return executable;
	}

	void compileModule(std::list<compiler::CompilableFunction>& functions, const engine::ModuleOptions& moduleOptions,
	                   std::shared_ptr<engine::details::ModuleState> state) const override {
		const auto normalizedOptions = detail::normalizeOptions(moduleOptions);
		if (fallback_.getName() != "mlir") {
			fallback_.compileModule(functions, normalizedOptions, std::move(state));
			return;
		}
		state->executable = compile(functions, normalizedOptions);
	}

	std::string getName() const override {
		return fallback_.getName();
	}

	const engine::Options& getOptions() const override {
		return pipeline_.getOptions();
	}

private:
	common::ArenaPool traceArenaPool_;
	common::ArenaPool irArenaPool_;
	compiler::CompilationPipeline pipeline_;
	compiler::TieredJITCompiler fallback_;
};

} // namespace

std::unique_ptr<compiler::JITCompiler> createCompiler(const engine::Options& options) {
	return std::make_unique<CachingCompiler>(options);
}

} // namespace nautilus::cache

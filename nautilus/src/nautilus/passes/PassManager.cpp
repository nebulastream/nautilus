#include "nautilus/passes/Pass.hpp"
#include <algorithm>
#include <chrono>

namespace nautilus::passes {

void PassManager::addGroup(std::vector<std::unique_ptr<Pass>> passes, size_t maxIterations) {
	if (passes.empty()) {
		return;
	}
	groups_.push_back(Group {std::move(passes), std::max<size_t>(maxIterations, 1)});
}

std::optional<nir::Module> PassManager::run(const nir::Module& in, PassContext& context, PipelineReport& report,
                                            const AfterPass& afterPass) const {
	using Clock = std::chrono::steady_clock;
	const auto start = Clock::now();
	std::optional<nir::Module> current;
	auto statisticsOf = [&](std::string_view name) -> PassStatistics& {
		for (auto& s : report.passes) {
			if (s.name == name) {
				return s;
			}
		}
		report.passes.push_back(PassStatistics {std::string(name), 0, 0, 0.0});
		return report.passes.back();
	};
	for (const auto& group : groups_) {
		for (size_t round = 0; round < group.maxIterations; ++round) {
			bool changed = false;
			++report.iterations;
			for (const auto& pass : group.passes) {
				const nir::Module& input = current ? *current : in;
				context.changes = 0;
				const auto passStart = Clock::now();
				std::optional<nir::Module> result = pass->run(input, context);
				const double ms = std::chrono::duration<double, std::milli>(Clock::now() - passStart).count();
				auto& stat = statisticsOf(pass->name());
				++stat.runs;
				stat.ms += ms;
				if (result) {
					stat.changes += std::max<uint64_t>(context.changes, 1);
					current = std::move(result);
					changed = true;
					if (afterPass) {
						afterPass(*current, pass->name());
					}
				}
			}
			if (!changed) {
				break;
			}
		}
	}
	report.totalMs += std::chrono::duration<double, std::milli>(Clock::now() - start).count();
	return current;
}

} // namespace nautilus::passes

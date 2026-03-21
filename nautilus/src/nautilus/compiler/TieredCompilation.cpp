#include "nautilus/Module.hpp"
#include "nautilus/compiler/backends/CompilationBackend.hpp"
#include "nautilus/compiler/ir/IRGraph.hpp"
#include "nautilus/logging.hpp"
#include <thread>

#ifdef ENABLE_COMPILER

#include "nautilus/compiler/DumpHandler.hpp"

namespace nautilus::engine::details {

static std::string createPromotionUnitID() {
	auto now = std::chrono::system_clock::now();
	auto time_t = std::chrono::system_clock::to_time_t(now);
	std::tm tm {};
	localtime_r(&time_t, &tm);
	char buf[64];
	std::snprintf(buf, sizeof(buf), "%04d-%02d-%02d_%02d-%02d-%02d_tier1", tm.tm_year + 1900, tm.tm_mon + 1,
	              tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
	return std::string(buf);
}

void TryPromote(std::shared_ptr<ModuleState> state) {
	bool expected = false;
	if (!state->promotionInProgress.compare_exchange_strong(expected, true, std::memory_order_acq_rel)) {
		return; // Another thread already started promotion
	}

	state->promotionThread = std::thread([state]() {
		try {
			auto* registry = compiler::CompilationBackendRegistry::getInstance();
			auto* backend = registry->getBackend(state->tierConfig.tier1.backend);

			auto compilationId = createPromotionUnitID();
			auto dumpHandler = compiler::DumpHandler(state->options, compilationId);

			// Reuse the cached IR — no re-tracing needed
			auto executable = backend->compile(state->cachedIR, dumpHandler, state->options);
			executable->setGeneratedFiles(dumpHandler.getGeneratedFiles());

			// Swap in the new executable; version bump notifies all ModuleFunction handles
			{
				std::unique_lock<std::shared_mutex> lock(state->mutex);
				state->executable = std::move(executable);
				state->version.fetch_add(1, std::memory_order_release);
			}
			state->promoted.store(true, std::memory_order_release);

			// Release cached IR to free memory
			state->cachedIR.reset();
		} catch (const std::exception& e) {
			// On failure, mark as promoted to avoid infinite retry.
			// Module continues operating at tier 0.
			log::error("Tier 1 promotion failed: {}", e.what());
			state->promoted.store(true, std::memory_order_release);
		}
	});
}

} // namespace nautilus::engine::details

#else

namespace nautilus::engine::details {

void TryPromote(std::shared_ptr<ModuleState>) {
	// No-op when compiler is disabled
}

} // namespace nautilus::engine::details

#endif

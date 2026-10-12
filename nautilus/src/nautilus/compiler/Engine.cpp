#include "nautilus/Engine.hpp"
#include "nautilus/JITCompiler.hpp"
#include "nautilus/common/Arena.hpp"
#include "nautilus/compiler/TieredCompiler.hpp"
#include "nautilus/exceptions/RuntimeException.hpp"
#include "nautilus/logging.hpp"
#include <charconv>
#include <cstdlib>
#include <string>
#include <string_view>

namespace nautilus::engine {

/**
 * @brief Layers the options named in the NAUTILUS_OPTIONS environment variable
 * over @p options, so an embedding application's compilations can be
 * reconfigured (backend, passes, ...) without changing its code.
 *
 * NAUTILUS_OPTIONS holds `key=value` pairs separated by `;`. `true` and `false`
 * set booleans, integers and decimals set int and double options, and any other
 * value sets a string option.
 */
static Options applyEnvironmentOptions(const Options& options) {
	const char* overrides = std::getenv("NAUTILUS_OPTIONS");
	if (overrides == nullptr || *overrides == '\0') {
		return options;
	}
	Options result = options;
	std::string_view rest(overrides);
	while (!rest.empty()) {
		const auto end = rest.find(';');
		const auto entry = rest.substr(0, end);
		rest = end == std::string_view::npos ? std::string_view {} : rest.substr(end + 1);
		if (entry.empty()) {
			continue;
		}
		const auto separator = entry.find('=');
		if (separator == std::string_view::npos || separator == 0) {
			throw RuntimeException("NAUTILUS_OPTIONS entry '" + std::string(entry) + "' is not key=value");
		}
		const auto key = std::string(entry.substr(0, separator));
		const auto value = std::string(entry.substr(separator + 1));
		int integer = 0;
		const auto [integerEnd, integerError] = std::from_chars(value.data(), value.data() + value.size(), integer);
		char* decimalEnd = nullptr;
		const double decimal = std::strtod(value.c_str(), &decimalEnd);
		if (value == "true" || value == "false") {
			result.setOption(key, value == "true");
		} else if (!value.empty() && integerError == std::errc() && integerEnd == value.data() + value.size()) {
			result.setOption(key, integer);
		} else if (!value.empty() && decimalEnd == value.c_str() + value.size()) {
			result.setOption(key, decimal);
		} else {
			result.setOption(key, value);
		}
	}
	return result;
}

/**
 * @brief Creates the JITCompiler for the engine.
 *
 * The TieredJITCompiler is the only compilation strategy. Single-tier
 * compilation is configured through it: `engine.backend=<x>` pins a single
 * backend (synchronous, no promotion) and `engine.tiered.backgroundPromotion=false`
 * compiles single-tier with the tier-1 backend.
 *
 * The engine-owned @p traceArenaPool and @p irArenaPool are injected into the
 * compiler so every compile() acquires a fresh, recycled trace arena (trace
 * pool), and every IR-graph allocation likewise recycles its arena across
 * compiles (IR pool).  Both pools are internally synchronized, so a single
 * compiler may serve concurrent compile() calls from multiple threads.
 */
inline std::unique_ptr<compiler::JITCompiler>
createJITCompiler(const Options& options, common::ArenaPool& traceArenaPool, common::ArenaPool& irArenaPool) {
	if (!options.getOptionOrDefault<std::string>("engine.compilationStrategy", "").empty()) {
		log::warn("The option 'engine.compilationStrategy' was removed and is ignored; the tiered compiler is always "
		          "used. Set 'engine.backend' or 'engine.tiered.backgroundPromotion=false' for single-tier "
		          "compilation.");
	}
	return std::make_unique<compiler::TieredJITCompiler>(options, traceArenaPool, irArenaPool);
}

NautilusEngine::NautilusEngine(const Options& options)
    : traceArenaPool_(std::make_unique<common::ArenaPool>()), irArenaPool_(std::make_unique<common::ArenaPool>()),
      jit_(createJITCompiler(applyEnvironmentOptions(options), *traceArenaPool_, *irArenaPool_)),
      options(applyEnvironmentOptions(options)) {
}

NautilusEngine::NautilusEngine(std::unique_ptr<compiler::JITCompiler> jit, const Options& options)
    : traceArenaPool_(std::make_unique<common::ArenaPool>()), irArenaPool_(std::make_unique<common::ArenaPool>()),
      jit_(std::move(jit)), options(applyEnvironmentOptions(options)) {
}

NautilusEngine::~NautilusEngine() = default;
NautilusEngine::NautilusEngine(NautilusEngine&&) noexcept = default;

} // namespace nautilus::engine

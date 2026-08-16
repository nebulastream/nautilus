#pragma once

#include <iostream>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>

namespace nautilus::engine {

using OptionValue = std::variant<int, double, std::string, bool>;

class ModuleOptions;

/**
 * @brief Engine-wide options.
 *
 * Every Nautilus option can be defined here: it provides the engine-wide
 * defaults for the whole compilation pipeline. A subset of these options
 * (the per-compile "module" options such as `dump.*`, `ir.*`,
 * `mlir.optimizationLevel`, ...) can be overridden for an individual compiled
 * module via @ref ModuleOptions, which a module inherits from the engine and
 * may override. The remaining options (`engine.backend`,
 * `engine.tier0/tier1.backend`, `engine.tiered.backgroundPromotion`,
 * `engine.compilation`) configure the engine-owned, built-once compiler and
 * are therefore not overridable per module.
 */
class EngineOptions {

public:
	// Set option value
	template <typename T>
	void setOption(const std::string& name, const T& value) {
		options[name] = value;
	}

	template <typename T>
	T getOptionOrDefault(const std::string& name, T defaultValue) const {
		if (options.contains(name)) {
			const OptionValue& val = options.at(name);
			if (std::holds_alternative<T>(val)) {
				return std::get<T>(val);
			}
		}
		return defaultValue;
	}

	/**
	 * @brief Apply every option value set in @p other on top of this one;
	 * values present in @p other win.
	 *
	 * Used to layer per-module overrides on top of the engine-wide defaults.
	 */
	void applyOverrides(const EngineOptions& other) {
		for (const auto& entry : other.options) {
			options[entry.first] = entry.second;
		}
	}

	/**
	 * @brief Derive a ModuleOptions seeded with all of this engine's option
	 * values.
	 *
	 * A module starts from these inherited values and may override the
	 * per-compile subset before compiling. Overriding an engine-only option
	 * (e.g. `engine.backend`) has no effect because the compiler is already
	 * built by the time the module is compiled.
	 */
	[[nodiscard]] ModuleOptions deriveModuleOptions() const;

private:
	std::unordered_map<std::string, OptionValue> options;
};

/**
 * @brief Per-compiled-module options.
 *
 * Carries the values a single compiled module is compiled with. It inherits
 * its values from the engine (see @ref EngineOptions::deriveModuleOptions) and
 * the caller may override the per-compile subset via setOption() or by passing
 * an instance to NautilusEngine::createModule(). It derives from EngineOptions
 * purely so it reuses the same value storage and so every existing
 * consumer that accepts a `const Options&` transparently accepts module
 * options.
 */
class ModuleOptions : public EngineOptions {
public:
	ModuleOptions() = default;
	explicit ModuleOptions(const EngineOptions& base) : EngineOptions(base) {
	}
};

inline ModuleOptions EngineOptions::deriveModuleOptions() const {
	return ModuleOptions(*this);
}

// Backward-compatible alias: existing code constructs `Options` and passes it
// to the engine. The engine-wide options type is `EngineOptions`.
using Options = EngineOptions;

} // namespace nautilus::engine

#include "CacheOptions.hpp"
#include "nautilus/exceptions/RuntimeException.hpp"
#include <array>
#include <optional>
#include <utility>

namespace nautilus::cache::detail {
namespace {

std::optional<std::string> stringOption(const engine::Options& options, const char* name) {
	const auto& values = options.getOptionValues();
	const auto entry = values.find(name);
	if (entry == values.end()) {
		return std::nullopt;
	}
	const auto* value = std::get_if<std::string>(&entry->second);
	if (value == nullptr) {
		throw RuntimeException("Cache option '" + std::string(name) + "' must be a string");
	}
	return *value;
}

} // namespace

engine::ModuleOptions normalizeOptions(const engine::Options& options) {
	constexpr std::array<std::pair<const char*, const char*>, 2> aliases {
	    {{"engine.cache.directory", "engine.Blob.CacheDir"}, {"engine.cache.key", "engine.Blob.CacheKey"}}};
	engine::ModuleOptions normalized;
	for (const auto& [name, value] : options.getOptionValues()) {
		if (name != "engine.Blob.CacheDir" && name != "engine.Blob.CacheKey") {
			normalized.setOption(name, value);
		}
	}
	for (const auto& [canonical, alias] : aliases) {
		const auto canonicalValue = stringOption(options, canonical);
		const auto aliasValue = stringOption(options, alias);
		if (canonicalValue && aliasValue && *canonicalValue != *aliasValue) {
			throw RuntimeException("Conflicting cache options '" + std::string(canonical) + "' and '" + alias + "'");
		}
		if (aliasValue && !canonicalValue) {
			normalized.setOption(canonical, *aliasValue);
		}
	}
	return normalized;
}

} // namespace nautilus::cache::detail

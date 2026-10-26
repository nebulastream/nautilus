#pragma once

#include <cstdint>
#include <map>
#include <optional>
#include <string>
#include <string_view>
#include <variant>
#include <vector>

namespace nautilus {

/// The value kinds of an option (13-nir-spec.md §7.2: bool, i64, f64, string; integer options are widened to i64).
using OptionValue = std::variant<bool, int64_t, double, std::string>;
enum class OptionType { Bool, Int, Double, String };

/// Maps a host-dependent value to the concrete one that is hashed (`gpu.device=auto` into `cuda`, 17 §4.1).
using OptionResolveHook = OptionValue (*)(const OptionValue&);

/// A deprecated name of an option. `map` turns the alias's value into the target's, or into nothing to keep the
/// target's default (`engine.tiered.backgroundPromotion`, 11 §4.7).
struct DeprecatedAlias {
	std::string_view name;
	std::string_view target;
	std::optional<OptionValue> (*map)(const OptionValue&);
};

struct OptionSpec {
	std::string_view name;
	OptionType type;
	/// The default in the text form of NAUTILUS_OPTIONS; empty when the option has none (unset means "decided by the
	/// backend"); such an option is hashed only when it is set.
	std::string_view defaultText;
	/// The option changes the generated code, so it is part of the structural hash.
	bool affectsCode;
	/// Accepted values of a string option; empty means any.
	std::vector<std::string_view> allowed = {};
	OptionResolveHook resolve = nullptr;
};

/// The registry of every option of docs/options.md, in the order of that table.
const std::vector<OptionSpec>& optionRegistry();
const std::vector<DeprecatedAlias>& deprecatedAliases();

/// Parses the text of NAUTILUS_OPTIONS (`true`/`false` booleans, numbers, anything else a string) according to the
/// option's type. Throws std::invalid_argument for text of the wrong kind or outside `allowed`.
OptionValue parseOptionValue(const OptionSpec& spec, std::string_view text);

using OptionMap = std::map<std::string, OptionValue>;

/// The options in effect: registry defaults overlaid with the values set by the user, deprecated aliases mapped to
/// their targets, `resolve` hooks applied. Keys the registry does not know are application options and are kept as set.
struct EffectiveOptions {
	OptionMap values;
	/// One message per deprecated alias used and per alias that overrode its target; the caller logs each once.
	std::vector<std::string> warnings;
};

/// `registry` and `aliases` default to the global ones; tests pass their own.
EffectiveOptions effectiveOptions(const OptionMap& set, const std::vector<OptionSpec>& registry = optionRegistry(),
                                  const std::vector<DeprecatedAlias>& aliases = deprecatedAliases());

/// The options block of the structural hash (13-nir-spec.md §7.2, item 2): the code-affecting options in effect,
/// sorted by name, each `u32 nameLength, name, u8 kind, value`, preceded by a `u32` count.
std::vector<uint8_t> encodeOptionsBlock(const EffectiveOptions& options,
                                        const std::vector<OptionSpec>& registry = optionRegistry());

} // namespace nautilus

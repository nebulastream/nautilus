#include "nautilus/common/OptionRegistry.hpp"
#include <algorithm>
#include <charconv>
#include <cstring>
#include <stdexcept>

namespace nautilus {

const std::vector<OptionSpec>& optionRegistry() {
	// Generated from the table of docs/options.md; the doc-check test keeps the two in step.
	static const std::vector<OptionSpec> registry = {
	    {"engine.compilation", OptionType::Bool, "true", false, {}},
	    {"engine.backend", OptionType::String, "", false, {"mlir", "cpp", "bc", "tbc", "asmjit"}},
	    {"engine.tier0.backend", OptionType::String, "", false, {}},
	    {"engine.tier1.backend", OptionType::String, "", false, {}},
	    {"engine.tiered.promotion", OptionType::String, "eager", false, {"eager", "manual", "none"}},
	    {"engine.normalizeFunctionNames", OptionType::Bool, "false", false, {}},
	    {"engine.foldStaticConstants", OptionType::Bool, "true", true, {}},
	    {"engine.demangleFunctionNames", OptionType::Bool, "true", false, {}},
	    {"engine.resolveFunctionNames", OptionType::Bool, "", false, {}},
	    {"engine.logStatistics", OptionType::Bool, "false", false, {}},
	    {"dump.all", OptionType::Bool, "false", false, {}},
	    {"dump.copySites", OptionType::Bool, "false", false, {}},
	    {"dump.after_tracing", OptionType::Bool, "false", false, {}},
	    {"dump.after_ir_creation", OptionType::Bool, "false", false, {}},
	    {"dump.after_mlir_generation", OptionType::Bool, "false", false, {}},
	    {"dump.after_llvm_generation", OptionType::Bool, "false", false, {}},
	    {"dump.after_cpp_generation", OptionType::Bool, "false", false, {}},
	    {"dump.after_bc_generation", OptionType::Bool, "false", false, {}},
	    {"dump.console", OptionType::Bool, "false", false, {}},
	    {"dump.file", OptionType::Bool, "true", false, {}},
	    {"dump.graph", OptionType::Bool, "false", false, {}},
	    {"dump.graph.type", OptionType::String, "graphviz", false, {"mermaid", "graphviz"}},
	    {"dump.graph.full", OptionType::Bool, "false", false, {}},
	    {"ir.runPasses", OptionType::Bool, "true", true, {}},
	    {"ir.runOptimizationPasses", OptionType::Bool, "", true, {}},
	    {"ir.disableDeadCodeElimination", OptionType::Bool, "false", true, {}},
	    {"ir.disableAlgebraicSimplification", OptionType::Bool, "false", true, {}},
	    {"ir.disableConstantBranchFolding", OptionType::Bool, "false", true, {}},
	    {"ir.disableBlockMerging", OptionType::Bool, "false", true, {}},
	    {"ir.disableBlockArgumentPruning", OptionType::Bool, "false", true, {}},
	    {"ir.enableLocalCSE", OptionType::Bool, "false", true, {}},
	    {"ir.enableLICM", OptionType::Bool, "false", true, {}},
	    {"optimizationLevel", OptionType::Int, "", true, {}},
	    {"debug", OptionType::Bool, "false", true, {}},
	    {"perf", OptionType::Bool, "false", false, {}},
	    {"perf.sample", OptionType::Bool, "false", false, {}},
	    {"mlir.enableMultithreading", OptionType::Bool, "true", false, {}},
	    {"mlir.enableIntrinsics", OptionType::Bool, "true", true, {}},
	    {"mlir.inline_invoke_calls", OptionType::Bool, "false", true, {}},
	    {"mlir.eager_compilation", OptionType::Bool, "false", false, {}},
	    {"bc.registerAllocator", OptionType::Bool, "true", true, {}},
	    {"tbc.mode", OptionType::String, "interp", true, {"interp", "jit", "auto"}},
	    {"tbc.dispatch", OptionType::String, "auto", false, {"auto", "tailcall", "goto", "switch"}},
	    {"tbc.stackSizeKb", OptionType::Int, "1024", false, {}},
	    {"tbc.registerAllocator", OptionType::Bool, "true", true, {}},
	    {"tbc.coalescing", OptionType::Bool, "true", true, {}},
	    {"tbc.superinstructions", OptionType::Bool, "true", true, {}},
	    {"tbc.immediates", OptionType::Bool, "true", true, {}},
	    {"asmjit.enableIntrinsics", OptionType::Bool, "true", true, {}},
	    {"asmjit.enablePostRAPeephole", OptionType::Bool, "true", true, {}},
	    {"asmjit.enableBranchFusion", OptionType::Bool, "true", true, {}},
	    {"asmjit.enableConstFolding", OptionType::Bool, "true", true, {}},
	    {"asmjit.enableSelectCmov", OptionType::Bool, "true", true, {}},
	    {"asmjit.enableLazyNarrowing", OptionType::Bool, "true", true, {}},
	    {"asmjit.enableAddressFusion", OptionType::Bool, "true", true, {}},
	    {"asmjit.enableInlining", OptionType::Bool, "true", true, {}},
	    {"asmjit.inliningMaxOperations", OptionType::Int, "24", true, {}},
	    {"asmjit.enableLoopRotation", OptionType::Bool, "true", true, {}},
	    {"asmjit.enableTailDuplication", OptionType::Bool, "true", true, {}},
	};
	return registry;
}

namespace {

std::optional<OptionValue> mapBackgroundPromotion(const OptionValue& v) {
	if (auto* b = std::get_if<bool>(&v)) {
		if (!*b) {
			return OptionValue(std::string("none"));
		}
		return std::nullopt;
	}
	throw std::invalid_argument("engine.tiered.backgroundPromotion is a boolean");
}

const OptionSpec* find(const std::vector<OptionSpec>& registry, std::string_view name) {
	for (auto& s : registry) {
		if (s.name == name) {
			return &s;
		}
	}
	return nullptr;
}

OptionValue coerce(const OptionSpec& spec, const OptionValue& v) {
	auto bad = [&]() -> OptionValue {
		throw std::invalid_argument("option " + std::string(spec.name) + " has the wrong type");
	};
	switch (spec.type) {
	case OptionType::Bool:
		return std::holds_alternative<bool>(v) ? v : bad();
	case OptionType::Int:
		if (auto* i = std::get_if<int64_t>(&v)) {
			return *i;
		}
		return bad();
	case OptionType::Double:
		if (auto* d = std::get_if<double>(&v)) {
			return *d;
		}
		if (auto* i = std::get_if<int64_t>(&v)) {
			return static_cast<double>(*i);
		}
		return bad();
	case OptionType::String: {
		auto* s = std::get_if<std::string>(&v);
		if (s == nullptr) {
			return bad();
		}
		if (!spec.allowed.empty() && std::find(spec.allowed.begin(), spec.allowed.end(), *s) == spec.allowed.end()) {
			throw std::invalid_argument("option " + std::string(spec.name) + " does not accept '" + *s + "'");
		}
		return v;
	}
	}
	return bad();
}

void put32(std::vector<uint8_t>& out, uint32_t v) {
	for (int i = 0; i < 4; ++i) {
		out.push_back(static_cast<uint8_t>(v >> (8 * i)));
	}
}
void put64(std::vector<uint8_t>& out, uint64_t v) {
	for (int i = 0; i < 8; ++i) {
		out.push_back(static_cast<uint8_t>(v >> (8 * i)));
	}
}

} // namespace

const std::vector<DeprecatedAlias>& deprecatedAliases() {
	static const std::vector<DeprecatedAlias> aliases = {
	    {"engine.tiered.backgroundPromotion", "engine.tiered.promotion", mapBackgroundPromotion},
	};
	return aliases;
}

OptionValue parseOptionValue(const OptionSpec& spec, std::string_view text) {
	switch (spec.type) {
	case OptionType::Bool:
		if (text == "true") {
			return true;
		}
		if (text == "false") {
			return false;
		}
		break;
	case OptionType::Int: {
		int64_t v = 0;
		auto r = std::from_chars(text.data(), text.data() + text.size(), v);
		if (r.ec == std::errc() && r.ptr == text.data() + text.size() && !text.empty()) {
			return v;
		}
		break;
	}
	case OptionType::Double: {
		double v = 0;
		auto r = std::from_chars(text.data(), text.data() + text.size(), v);
		if (r.ec == std::errc() && r.ptr == text.data() + text.size() && !text.empty()) {
			return v;
		}
		break;
	}
	case OptionType::String:
		return coerce(spec, OptionValue(std::string(text)));
	}
	throw std::invalid_argument("option " + std::string(spec.name) + " does not accept '" + std::string(text) + "'");
}

EffectiveOptions effectiveOptions(const OptionMap& set, const std::vector<OptionSpec>& registry,
                                  const std::vector<DeprecatedAlias>& aliases) {
	EffectiveOptions result;
	for (auto& spec : registry) {
		if (!spec.defaultText.empty()) {
			result.values[std::string(spec.name)] = parseOptionValue(spec, spec.defaultText);
		}
	}
	// aliases are applied last, so that they win over their target (11 §4.7)
	std::vector<std::pair<const DeprecatedAlias*, const OptionValue*>> usedAliases;
	for (auto& [key, value] : set) {
		const DeprecatedAlias* alias = nullptr;
		for (auto& a : aliases) {
			if (a.name == key) {
				alias = &a;
			}
		}
		if (alias != nullptr) {
			usedAliases.push_back({alias, &value});
			continue;
		}
		if (auto* spec = find(registry, key)) {
			result.values[key] = coerce(*spec, value);
		} else {
			result.values[key] = value; // an application option
		}
	}
	for (auto [alias, value] : usedAliases) {
		result.warnings.push_back("option " + std::string(alias->name) + " is deprecated, use " + std::string(alias->target));
		auto* target = find(registry, alias->target);
		if (target == nullptr) {
			continue;
		}
		auto mapped = alias->map ? alias->map(*value) : std::optional<OptionValue>(*value);
		if (!mapped) {
			continue;
		}
		if (set.count(std::string(alias->target))) {
			result.warnings.push_back("option " + std::string(alias->name) + " overrides " + std::string(alias->target));
		}
		result.values[std::string(alias->target)] = *mapped;
	}
	for (auto& spec : registry) {
		if (spec.resolve != nullptr) {
			auto it = result.values.find(std::string(spec.name));
			if (it != result.values.end()) {
				it->second = spec.resolve(it->second);
			}
		}
	}
	return result;
}

std::vector<uint8_t> encodeOptionsBlock(const EffectiveOptions& options, const std::vector<OptionSpec>& registry) {
	std::vector<std::pair<std::string, const OptionValue*>> entries;
	for (auto& spec : registry) {
		if (!spec.affectsCode) {
			continue;
		}
		auto it = options.values.find(std::string(spec.name));
		if (it != options.values.end()) {
			entries.push_back({it->first, &it->second});
		}
	}
	std::sort(entries.begin(), entries.end(), [](auto& a, auto& b) { return a.first < b.first; });
	std::vector<uint8_t> out;
	put32(out, static_cast<uint32_t>(entries.size()));
	for (auto& [name, value] : entries) {
		put32(out, static_cast<uint32_t>(name.size()));
		out.insert(out.end(), name.begin(), name.end());
		if (auto* b = std::get_if<bool>(value)) {
			out.push_back(0);
			out.push_back(*b ? 1 : 0);
		} else if (auto* i = std::get_if<int64_t>(value)) {
			out.push_back(1);
			put64(out, static_cast<uint64_t>(*i));
		} else if (auto* d = std::get_if<double>(value)) {
			out.push_back(2);
			uint64_t bits;
			std::memcpy(&bits, d, 8);
			put64(out, bits);
		} else {
			auto& s = std::get<std::string>(*value);
			out.push_back(3);
			put32(out, static_cast<uint32_t>(s.size()));
			out.insert(out.end(), s.begin(), s.end());
		}
	}
	return out;
}

} // namespace nautilus

#include "nautilus/Module.hpp"
#include "nautilus/link/ModuleRegistry.hpp"
#include "nautilus/nir/Module.hpp"
#include "nautilus/nir/Records.hpp"
#include <fmt/format.h>
#include <string>
#include <string_view>

// `CompiledModule::explain()` and the rendering of `ModuleExplanation` (plan M4.9, 15 §4.3). Everything here reads state
// that the compile has already recorded; nothing runs on the compile path.

namespace nautilus::engine {

namespace {

const char* tierName(Tier tier) {
	switch (tier) {
	case Tier::interpreted:
		return "interpreted";
	case Tier::tier0:
		return "tier0";
	case Tier::tier1:
		return "tier1";
	}
	return "unknown";
}

const char* promotionName(PromotionState state) {
	switch (state) {
	case PromotionState::none:
		return "none";
	case PromotionState::queued:
		return "queued";
	case PromotionState::running:
		return "running";
	case PromotionState::done:
		return "done";
	case PromotionState::failed:
		return "failed";
	case PromotionState::cancelled:
		return "cancelled";
	case PromotionState::skipped:
		return "skipped";
	}
	return "unknown";
}

std::vector<std::string> attributeNames(uint32_t bits) {
	std::vector<std::string> names;
	const std::pair<uint32_t, const char*> table[] = {
	    {nir::AttrNoArgRead, "noArgRead"},     {nir::AttrNoArgWrite, "noArgWrite"},
	    {nir::AttrNoOtherRead, "noOtherRead"}, {nir::AttrNoOtherWrite, "noOtherWrite"},
	    {nir::AttrNoUnwind, "noUnwind"},       {nir::AttrWillReturn, "willReturn"},
	};
	for (const auto& [bit, name] : table) {
		if ((bits & bit) != 0) {
			names.emplace_back(name);
		}
	}
	return names;
}

std::string backendOf(const compiler::CompilationStatistics* statistics) {
	if (statistics != nullptr) {
		if (const auto* value = statistics->find("backend.name"); value != nullptr) {
			if (const auto* text = std::get_if<std::string>(value)) {
				return *text;
			}
		}
	}
	return "unknown";
}

void appendJsonString(std::string& out, std::string_view text) {
	out += '"';
	for (const unsigned char c : text) {
		switch (c) {
		case '"':
			out += "\\\"";
			break;
		case '\\':
			out += "\\\\";
			break;
		case '\n':
			out += "\\n";
			break;
		case '\r':
			out += "\\r";
			break;
		case '\t':
			out += "\\t";
			break;
		default:
			if (c < 0x20) {
				out += fmt::format("\\u{:04x}", c);
			} else {
				out += static_cast<char>(c);
			}
		}
	}
	out += '"';
}

void appendJsonStrings(std::string& out, const std::vector<std::string>& values) {
	out += '[';
	for (size_t i = 0; i < values.size(); ++i) {
		out += i == 0 ? "" : ",";
		appendJsonString(out, values[i]);
	}
	out += ']';
}

void appendJsonPending(std::string& out, const ModuleExplanation::Pending& pending) {
	out += fmt::format("{{\"available\":{},\"status\":", pending.available ? "true" : "false");
	appendJsonString(out, pending.status);
	out += ",\"milestone\":";
	appendJsonString(out, pending.milestone);
	out += '}';
}

} // namespace

ModuleExplanation CompiledModule::explain() const {
	ModuleExplanation report;
	std::shared_ptr<const compiler::CompilationStatistics> statistics;
	std::shared_ptr<void> link;
	Tier current;
	{
		std::shared_lock<std::shared_mutex> lock(state_->mutex);
		if (state_->executable) {
			statistics = state_->executable->getCompilationStatistics();
		}
		current = state_->executable ? state_->tier.load(std::memory_order_relaxed) : Tier::interpreted;
		link = state_->link;
	}
	const auto promotion = promotionStatus();
	report.tier = tierName(current);
	report.promotion = promotionName(promotion.state);
	report.promotionError = promotion.error;
	report.statistics = statistics;
	report.backend = current == Tier::interpreted ? "interpreter" : backendOf(statistics.get());

	// The tiers: a module with a promotion has two, whose backends the promotion remembers; any other has the one it runs.
	if (state_->promotionControl != nullptr) {
		const auto backends = state_->promotionControl->tierBackends();
		const bool tier0Interpreted = backends.tier0 == "interpreter";
		report.tiers.push_back({tier0Interpreted ? "interpreted" : "tier0", tier0Interpreted ? "interpreter" : backends.tier0,
		                        current == Tier::tier1 ? "replaced" : "active"});
		report.tiers.push_back({"tier1", backends.tier1, current == Tier::tier1 ? "active" : report.promotion});
	} else {
		report.tiers.push_back({report.tier, report.backend, "active"});
	}

	// The registry's view of the module: its name, imports, exports and NIR.
	const auto handle = std::static_pointer_cast<link::ModuleHandle>(link);
	if (handle == nullptr) {
		report.stage = "unregistered";
		return report;
	}
	report.name = handle->name();
	report.stage = link::toString(handle->stage());
	report.importedModules = handle->importedModules();
	if (const auto module = handle->module(); module != nullptr) {
		const auto exports = handle->exports();
		report.functionsAvailable = true;
		for (const auto& function : module->functions) {
			ModuleExplanation::Function entry;
			entry.name = std::string {module->name(function.nameId)};
			entry.tier = report.tier;
			entry.backend = report.backend;
			entry.blocks = function.blockCount;
			entry.instructions = function.instCount;
			entry.attributes = attributeNames(function.attrs);
			for (const auto& exported : exports) {
				entry.exported = entry.exported || exported.name == entry.name;
			}
			report.functions.push_back(std::move(entry));
		}
	}
	return report;
}

std::string ModuleExplanation::toString() const {
	std::string out;
	const auto line = [&](std::string_view key, std::string_view value) { out += fmt::format("{} = {}\n", key, value); };
	line("module.name", name.empty() ? "(anonymous)" : name);
	line("module.stage", stage);
	line("tier", tier);
	line("backend", backend);
	line("promotion", promotion);
	if (!promotionError.empty()) {
		line("promotion.error", promotionError);
	}
	for (const auto& t : tiers) {
		out += fmt::format("tiers.{} = backend {}, {}\n", t.tier, t.backend, t.status);
	}
	for (const auto& imported : importedModules) {
		line("imports", imported);
	}
	if (!functionsAvailable) {
		line("functions", "not recorded (no NIR kept for this module)");
	}
	for (const auto& f : functions) {
		std::string attributesText;
		for (const auto& a : f.attributes) {
			attributesText += (attributesText.empty() ? "" : ",") + a;
		}
		out += fmt::format("function {}: {} blocks, {} instructions, {}{}, attributes [{}]\n", f.name, f.blocks,
		                   f.instructions, f.tier, f.backend.empty() ? "" : " on " + f.backend, attributesText);
	}
	out += fmt::format("persistability = {} ({})\n", persistability.status, persistability.milestone);
	out += fmt::format("inlining = {} ({})\n", inlining.status, inlining.milestone);
	if (statistics != nullptr) {
		out += "statistics:\n";
		out += statistics->toString();
	} else {
		line("statistics", "none (no executable)");
	}
	return out;
}

std::string ModuleExplanation::toJson() const {
	std::string out = "{\"module\":{\"name\":";
	appendJsonString(out, name);
	out += ",\"stage\":";
	appendJsonString(out, stage);
	out += "},\"tier\":";
	appendJsonString(out, tier);
	out += ",\"backend\":";
	appendJsonString(out, backend);
	out += ",\"promotion\":{\"status\":";
	appendJsonString(out, promotion);
	out += ",\"error\":";
	appendJsonString(out, promotionError);
	out += "},\"tiers\":[";
	for (size_t i = 0; i < tiers.size(); ++i) {
		out += i == 0 ? "{\"tier\":" : ",{\"tier\":";
		appendJsonString(out, tiers[i].tier);
		out += ",\"backend\":";
		appendJsonString(out, tiers[i].backend);
		out += ",\"status\":";
		appendJsonString(out, tiers[i].status);
		out += '}';
	}
	out += "],\"imports\":";
	appendJsonStrings(out, importedModules);
	out += fmt::format(",\"functionsAvailable\":{},\"functions\":[", functionsAvailable ? "true" : "false");
	for (size_t i = 0; i < functions.size(); ++i) {
		const auto& f = functions[i];
		out += i == 0 ? "{\"name\":" : ",{\"name\":";
		appendJsonString(out, f.name);
		out += ",\"tier\":";
		appendJsonString(out, f.tier);
		out += ",\"backend\":";
		appendJsonString(out, f.backend);
		out += fmt::format(",\"blocks\":{},\"instructions\":{},\"exported\":{},\"attributes\":", f.blocks,
		                   f.instructions, f.exported ? "true" : "false");
		appendJsonStrings(out, f.attributes);
		out += '}';
	}
	out += "],\"persistability\":";
	appendJsonPending(out, persistability);
	out += ",\"inlining\":";
	appendJsonPending(out, inlining);
	out += ",\"statistics\":";
	out += statistics != nullptr ? statistics->toJson() : "null";
	out += '}';
	return out;
}

} // namespace nautilus::engine

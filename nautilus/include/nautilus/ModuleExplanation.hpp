#pragma once

#include "nautilus/CompilationStatistics.hpp"
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace nautilus::engine {

/// The result of `CompiledModule::explain()` (docs/plans/nautilus-0.4/15-agent-experience.md §4.3): what the engine
/// knows about a module, in one place, as plain data that renders as text or as JSON. Taking it reads state the
/// compile already recorded and costs the compile path nothing.
struct ModuleExplanation {
	/// A tier of the module. `status` is `active` (it runs the module), `replaced` (tier 0, after a promotion
	/// installed tier 1), or, for a tier 1 that is not running, the promotion state: `none`, `queued`, `running`,
	/// `failed`, `cancelled`, `skipped`.
	struct Tier {
		std::string tier; // `interpreted`, `tier0` or `tier1`
		std::string backend;
		std::string status;
	};

	/// A function of the module, when the engine kept its NIR (the NIR pipeline).
	struct Function {
		std::string name;
		/// The tier and backend that run it now: one executable holds every function of the module.
		std::string tier;
		std::string backend;
		uint32_t blocks = 0;
		uint32_t instructions = 0;
		/// The guarantees it declares or was inferred to have (13 §4.6.2): `noArgRead`, `noArgWrite`, `noOtherRead`,
		/// `noOtherWrite`, `noUnwind`, `willReturn`.
		std::vector<std::string> attributes;
		bool exported = false;
	};

	/// A part of the report whose producer is not built yet. It is reported as a value, never left out, so a reader
	/// can tell "not available yet" from "nothing to report".
	struct Pending {
		bool available = false;
		std::string status = "not available yet";
		/// The milestone of the plan that fills it in.
		std::string milestone;
	};

	/// The module's name; empty for an anonymous module.
	std::string name;
	/// The stage in the module registry (`built`, `linked`, `compiled`, `released`); `unregistered` for a module the
	/// registry does not know (the 0.3 pipeline, interpreted modules).
	std::string stage;
	/// The tier that runs the module now, and that tier's backend.
	std::string tier;
	std::string backend;
	/// Where the promotion to tier 1 stands, with the message of a failed one.
	std::string promotion;
	std::string promotionError;
	std::vector<Tier> tiers;
	/// False when the engine did not keep the module's NIR, so `functions` is empty (the counts of the whole module
	/// are in the statistics, as far as the pipeline records them).
	bool functionsAvailable = false;
	std::vector<Function> functions;
	std::vector<std::string> importedModules;
	/// The statistics of the compile that produced the running executable; null when there is none.
	std::shared_ptr<const compiler::CompilationStatistics> statistics;
	/// Why the module cannot be saved (`artifact.ineligibleReason`, 07 §5): filled with M5.3.
	Pending persistability {false, "not available yet", "M5.3"};
	/// Which calls were inlined and why others were not: filled with M3.8.
	Pending inlining {false, "not available yet", "M3.8"};

	/// One line per fact, for a terminal or a log.
	[[nodiscard]] std::string toString() const;
	/// One JSON object with the same content. The keys are stable; the statistics are the object of
	/// `CompilationStatistics::toJson()`. Single line.
	[[nodiscard]] std::string toJson() const;
};

} // namespace nautilus::engine

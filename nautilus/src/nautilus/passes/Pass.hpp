#pragma once

#include "nautilus/nir/Hash.hpp"
#include "nautilus/nir/Module.hpp"
#include "nautilus/options.hpp"
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

/// The optional passes and the plugin pass hooks (docs/plans/nautilus-0.4/03-conversion-and-passes.md §5).
///
/// A pass is a copy pass (02-nir.md §4): it reads an immutable SSA-level module and returns a new one, or nothing when
/// it has no change to make. Passes run only where a compile asks for them, so a compile that does not pays for none.
namespace nautilus::passes {

/// The modules that the imports of the module being compiled name, for inlining across modules (13 §4.4.3).
class ImportView {
public:
	virtual ~ImportView() = default;

	struct Exporter {
		/// The exporting module as the registry kept it after its conversion; null if it keeps none.
		std::shared_ptr<const nir::Module> module;
		/// Its name, and the structural hash at link time (what compiled code that inlines it records, §7.3).
		std::string name;
		std::optional<nir::Hash128> hash;
	};
	/// The exporter of import `importIndex` of `importer` (an index in `importer.imports`).
	virtual Exporter exporter(const nir::Module& importer, uint32_t importIndex) const = 0;
};

/// What a pass is told, and what it reports back.
struct PassContext {
	const engine::Options& options;
	/// For inlining across modules; null where the module has no imports.
	const ImportView* imports = nullptr;
	/// The number of rewrites the last `run` made (instructions hoisted, calls inlined, multiplications replaced). Set
	/// by the pass; the manager resets it before each run.
	uint64_t changes = 0;
	/// Decisions worth showing to a user (`explain()`), one line each; null where nobody asks.
	std::vector<std::string>* notes = nullptr;
	/// The imports whose body a pass inlined: `(index in imports, exporter name, structural hash)`. The compiled code
	/// keys on these (13 §7.3).
	struct InlinedImport {
		uint32_t importIndex = 0;
		std::string module;
		std::string function;
		std::optional<nir::Hash128> hash;
	};
	std::vector<InlinedImport>* inlinedImports = nullptr;
};

class Pass {
public:
	virtual ~Pass() = default;
	/// The name in the statistics keys `irPasses.<name>.ms/.runs/.changes`.
	[[nodiscard]] virtual std::string_view name() const = 0;
	/// The rewritten module, or nothing if the pass changed nothing.
	virtual std::optional<nir::Module> run(const nir::Module& in, PassContext& context) = 0;
};

/// A pass made of a function; the form plugins and tests register.
class FunctionPass final : public Pass {
public:
	using Function = std::function<std::optional<nir::Module>(const nir::Module&, PassContext&)>;
	FunctionPass(std::string name, Function function) : name_(std::move(name)), function_(std::move(function)) {
	}
	[[nodiscard]] std::string_view name() const override {
		return name_;
	}
	std::optional<nir::Module> run(const nir::Module& in, PassContext& context) override {
		return function_(in, context);
	}

private:
	std::string name_;
	Function function_;
};

struct PassStatistics {
	std::string name;
	uint32_t runs = 0;
	uint64_t changes = 0;
	double ms = 0;
};

/// What a pipeline did: the rounds it ran (a round is one pass over a group's passes), and per pass how often it ran,
/// how many rewrites it made and how long it took.
struct PipelineReport {
	uint32_t iterations = 0;
	double totalMs = 0;
	std::vector<PassStatistics> passes;
};

/// Runs groups of passes in registration order. A group is run again as a whole while one of its passes changed the
/// module, at most `maxIterations` rounds (as the fixed-point groups of 0.3's `IRPassManager`, which this replaces for
/// the optional passes).
class PassManager {
public:
	/// Called with the module after a pass changed it and the pass's name: the verifier hook.
	using AfterPass = std::function<void(const nir::Module&, std::string_view)>;

	void addGroup(std::vector<std::unique_ptr<Pass>> passes, size_t maxIterations);
	void addPass(std::unique_ptr<Pass> pass) {
		std::vector<std::unique_ptr<Pass>> group;
		group.push_back(std::move(pass));
		addGroup(std::move(group), 1);
	}
	[[nodiscard]] bool empty() const {
		return groups_.empty();
	}

	/// Runs every group on `in`; returns the result if any pass changed the module.
	std::optional<nir::Module> run(const nir::Module& in, PassContext& context, PipelineReport& report,
	                               const AfterPass& afterPass = {}) const;

private:
	struct Group {
		std::vector<std::unique_ptr<Pass>> passes;
		size_t maxIterations;
	};
	std::vector<Group> groups_;
};

} // namespace nautilus::passes

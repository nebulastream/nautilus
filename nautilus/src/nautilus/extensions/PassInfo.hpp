#pragma once

#include "nautilus/nir/Module.hpp"
#include "nautilus/options.hpp"
#include <cstdint>
#include <functional>
#include <optional>
#include <string>
#include <string_view>

namespace nautilus::ext {

/// Where in a compile a plugin pass runs (03-conversion-and-passes.md §5).
enum class PassPoint : uint8_t {
	/// On the SSA module the conversion produced, before the optional core passes.
	AfterConversion,
	/// On the module a backend is about to lower, after every other pass.
	BeforeLowering,
};

/// A *required* pass is part of the meaning of the program (the partition pass of the GPU plugin) and runs in every
/// compile. An *optional* pass improves code and runs only where the optional passes do: not on latency-critical
/// compiles (the tier-0 compile of a two-tier module), unless `ir.runOptimizationPasses` is true; never when it is
/// false.
enum class PassRequirement : uint8_t { Required, Optional };

/// What a plugin pass is told about the compile it runs in.
struct PassEnvironment {
	const engine::Options& options;
	/// The backend that will lower the module; empty for `AfterConversion`.
	std::string_view backend;
};

/// A copy pass: reads `in` and returns the rewritten module, or nothing if it changed nothing. It must return a valid
/// SSA-level module (the verifier runs on it when `ir.verifyAfterEachPass` is set) and must not keep a reference to
/// `in`. Passes of one point run in registration order.
using PassFunction = std::function<std::optional<nir::Module>(const nir::Module& in, const PassEnvironment&)>;

struct PassInfo {
	std::string name; // "gpu.partition"
	PassPoint point = PassPoint::AfterConversion;
	PassRequirement requirement = PassRequirement::Optional;
	PassFunction run;
};

} // namespace nautilus::ext

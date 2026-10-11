#pragma once

#include "nautilus/Module.hpp"
#include "nautilus/serialization/IRSerialization.hpp"
#include <cstddef>
#include <span>
#include <string>
#include <vector>

namespace nautilus::engine {
class NautilusEngine;
} // namespace nautilus::engine

/**
 * Saving and loading compiled modules (docs/ir-serialization.md#compiled-modules).
 *
 *   auto module = engine.createModule();          // engine with engine.retainIR=true
 *   module.registerFunction("agg", aggregate);
 *   auto bytes = saveModule(module.compile(), out);
 *   ...
 *   auto loaded = loadModule(otherEngine, bytes, in);
 *   auto agg = loaded.getFunction<int64_t(int64_t*, int64_t)>("agg");
 *
 * A module image is the IR the module was compiled from plus a manifest of its
 * functions and options. Loading recompiles that IR on the loading engine --
 * its backends, its tiering -- skipping tracing and the IR pass pipeline. No
 * machine code is shipped: every backend's output embeds process-specific
 * addresses.
 */
namespace nautilus::serialization {

/// One function of a module image.
struct FunctionSignature {
	std::string name;
	Type result = Type::v;
	std::vector<Type> params;
};

/// What a module image says about itself, readable without materializing its IR.
struct ModuleImageInfo {
	CompilationUnitID unitId;
	/// Every function the image holds, in IR order: the registered ones and
	/// the NautilusFunctions they call.
	std::vector<FunctionSignature> functions;
	/// The options the module was compiled with. loadModule() does not apply
	/// them; pass them as its overrides to reproduce the sender's configuration.
	engine::ModuleOptions options;
};

/**
 * @brief Captures @p module as a module image.
 * @throws SerializationException if the module kept no IR: compile it with
 *         the `engine.retainIR` option.
 */
[[nodiscard]] std::vector<std::byte> saveModule(const engine::CompiledModule& module,
                                                const SerializeOptions& options = {});

/**
 * @brief Reads a module image's manifest, without materializing its IR.
 * @throws SerializationException for a malformed buffer or a plain IR buffer.
 */
[[nodiscard]] ModuleImageInfo inspectModule(std::span<const std::byte> image, bool verifyChecksum = true);

/**
 * @brief Rebuilds a module image on @p engine.
 *
 * Materializes the IR (resolving symbols through @p options) and compiles it
 * with NautilusEngine::compileIR(), layering @p overrides on the engine's
 * options. getFunction() on the result checks each requested signature
 * against the image's.
 */
[[nodiscard]] engine::CompiledModule loadModule(const engine::NautilusEngine& engine, std::span<const std::byte> image,
                                                const DeserializeOptions& options = {},
                                                engine::ModuleOptions overrides = {});

} // namespace nautilus::serialization

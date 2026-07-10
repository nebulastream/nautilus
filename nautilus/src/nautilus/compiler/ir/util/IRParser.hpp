#pragma once

#include "nautilus/compiler/ir/IRGraph.hpp"
#include <functional>
#include <memory>
#include <string>
#include <string_view>

namespace nautilus::compiler::ir {

/**
 * @brief Resolves the runtime address of an external function referenced by
 * serialized IR (proxy calls and address-of operations).
 *
 * @param symbol the mangled symbol recorded at trace time
 * @param name the human-readable function name recorded at trace time
 * @return the address of the function in this process, or nullptr if the
 *         resolver does not know it (the parser then falls back to `dlsym`).
 */
using IRSymbolResolverFn = std::function<void*(const std::string& symbol, const std::string& name)>;

/**
 * @brief Parses the portable Nautilus IR text format (see
 * IRSerializationUtil.hpp) back into an `IRGraph`, so a previously generated
 * IR file can be compiled directly — skipping tracing.
 *
 * External functions referenced by proxy calls are re-resolved in this
 * process: first via @p symbolResolver (when provided), then via
 * `dlsym(RTLD_DEFAULT, symbol)`. Parsing fails if a referenced function
 * cannot be resolved, and the parsed graph is structurally verified
 * (`IRVerifier`) before it is returned.
 *
 * @param text serialized IR text
 * @param pool arena pool that backs the IR nodes of the returned graph
 * @param id compilation unit id for the new graph
 * @param symbolResolver optional resolver for external function symbols
 * @throws RuntimeException on syntax errors, unresolved symbols, or
 *         verification failures
 */
std::shared_ptr<IRGraph> parseIR(std::string_view text, common::ArenaPool& pool, const CompilationUnitID& id,
                                 const IRSymbolResolverFn& symbolResolver = nullptr);

/// Variant of @ref parseIR backed by a standalone arena (tests, tools).
std::shared_ptr<IRGraph> parseIR(std::string_view text, const CompilationUnitID& id,
                                 const IRSymbolResolverFn& symbolResolver = nullptr);

} // namespace nautilus::compiler::ir

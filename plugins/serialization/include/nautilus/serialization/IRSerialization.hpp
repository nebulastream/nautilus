#pragma once

#include "nautilus/JITCompiler.hpp"
#include <cstddef>
#include <cstdint>
#include <memory>
#include <optional>
#include <span>
#include <stdexcept>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

/**
 * Binary serialization of Nautilus IR (docs/ir-serialization.md).
 *
 *   SerializeOptions out;
 *   out.pointerMode = PointerMode::Portable;
 *   out.namer = &namer;
 *   auto bytes = serialize(graph, out);
 *   ... ship `bytes` ...
 *   DeserializeOptions in;
 *   in.resolver = &resolver;
 *   auto graph = deserialize(bytes, in);
 *
 * The format is versioned, checksummed and self-delimiting, so it can be
 * framed on a socket from its first 24 bytes (see `peekTotalSize`).
 * Deserialization never trusts its input: every offset, index and count is
 * bounds-checked and a malformed buffer is reported as a
 * `SerializationException`, never as undefined behaviour.
 *
 * The API names IRGraph and ArenaPool only by declaration, so this header
 * stays usable from code that does not see nautilus's internal headers.
 */
namespace nautilus::common {
class ArenaPool;
} // namespace nautilus::common

namespace nautilus::serialization {

using compiler::CompilationUnitID;
using compiler::ir::IRGraph;

class SerializationException : public std::runtime_error {
public:
	explicit SerializationException(std::string message);
};

/**
 * Names process-local addresses (native callees, destructors, constant
 * pointers) so they can be written portably. Supplied by the client: only it
 * knows how the receiving side will find the same code or data.
 */
class SymbolNamer {
public:
	virtual ~SymbolNamer() = default;
	/// The name @p address is known by, or nullopt if it has none.
	[[nodiscard]] virtual std::optional<std::string> nameOf(const void* address) const = 0;
};

/// Turns a symbol name written by a SymbolNamer back into an address.
class SymbolResolver {
public:
	virtual ~SymbolResolver() = default;
	/// The address of @p symbol in this process, or nullptr if unknown.
	[[nodiscard]] virtual void* resolve(std::string_view symbol) const = 0;
};

/**
 * Names and resolves through the dynamic linker (`dladdr` / `dlsym`). Works
 * for exported functions of shared libraries and of executables linked with
 * `-rdynamic`; it cannot name data or non-exported functions.
 */
class DynamicLinkerSymbols final : public SymbolNamer, public SymbolResolver {
public:
	[[nodiscard]] std::optional<std::string> nameOf(const void* address) const override;
	[[nodiscard]] void* resolve(std::string_view symbol) const override;
};

/**
 * An explicit name <-> address table. The natural choice when sender and
 * receiver agree on a fixed set of runtime functions and globals, and the
 * only way to ship a constant data pointer. Optionally falls back to another
 * namer/resolver (e.g. DynamicLinkerSymbols) for addresses it does not hold.
 */
class SymbolTable final : public SymbolNamer, public SymbolResolver {
public:
	SymbolTable() = default;
	SymbolTable(const SymbolNamer* fallbackNamer, const SymbolResolver* fallbackResolver);

	/// Registers @p address under @p name; both must be unique in the table.
	void add(std::string name, const void* address);

	[[nodiscard]] std::optional<std::string> nameOf(const void* address) const override;
	[[nodiscard]] void* resolve(std::string_view symbol) const override;

private:
	std::unordered_map<const void*, std::string> byAddress_;
	std::unordered_map<std::string, const void*> byName_;
	const SymbolNamer* fallbackNamer_ = nullptr;
	const SymbolResolver* fallbackResolver_ = nullptr;
};

enum class PointerMode : uint8_t {
	/// Raw addresses; the buffer is only readable by this process. The
	/// cheapest mode, for caching or handing IR between threads/stages.
	ProcessLocal,
	/// Every non-null pointer is written as a symbol from `namer`;
	/// serialization fails if one has no name. For the network or disk.
	Portable,
};

struct SerializeOptions {
	PointerMode pointerMode = PointerMode::ProcessLocal;
	/// Required in Portable mode. In ProcessLocal mode it is optional and its
	/// names are recorded alongside the raw addresses as a fallback.
	const SymbolNamer* namer = nullptr;
};

struct DeserializeOptions {
	/// Resolves symbolic pointers. Required unless every pointer in the
	/// buffer is null or raw and process-local.
	const SymbolResolver* resolver = nullptr;
	bool verifyChecksum = true;
	/// Run the IR verifier on the materialized graph and reject it if it
	/// reports an error. Off by default because the writer only ever emits
	/// what an IRGraph held, and a pre-pass graph can legitimately fail some
	/// checks the verifier makes after the pass pipeline.
	bool verifyIR = false;
	/// Overrides the compilation unit id stored in the buffer.
	std::optional<CompilationUnitID> compilationUnitId;
};

/// Serializes @p graph. Deterministic: equal graphs give equal bytes.
[[nodiscard]] std::vector<std::byte> serialize(const IRGraph& graph, const SerializeOptions& options = {});

/// Rebuilds an IRGraph from @p buffer. @p buffer is not referenced after
/// this returns: strings and side tables are copied into the graph.
[[nodiscard]] std::shared_ptr<IRGraph> deserialize(std::span<const std::byte> buffer,
                                                   const DeserializeOptions& options = {});

/// Same as above, allocating the graph's arena from @p pool.
[[nodiscard]] std::shared_ptr<IRGraph> deserialize(std::span<const std::byte> buffer, common::ArenaPool& pool,
                                                   const DeserializeOptions& options = {});

/// The full size of the buffer whose first bytes are @p prefix, or nullopt
/// if @p prefix is too short to tell or does not start with the magic.
/// Lets a network reader frame a message without parsing it.
[[nodiscard]] std::optional<uint64_t> peekTotalSize(std::span<const std::byte> prefix);

/// Identity of this process as recorded in a ProcessLocal buffer.
[[nodiscard]] uint64_t currentProcessToken();

} // namespace nautilus::serialization

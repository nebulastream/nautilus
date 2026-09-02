#pragma once

#include "nautilus/compiler/ir/FunctionTable.hpp"
#include <mutex>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace nautilus::compiler::ir {

/**
 * @brief The process-wide identity of every intrinsic any backend can expand.
 *
 * An intrinsic is a native function that a backend MAY replace with
 * instructions.  Before this registry each backend recognised its own
 * intrinsics by pattern-matching the raw `void*` at every call site it
 * lowered, which meant three separate things were true at once: the IR could
 * not say that a call was an intrinsic, two backends could disagree about
 * whether a given address was one, and a backend without a handler had no way
 * to know it was falling back.
 *
 * Registering an address here answers all three.  It mints a dense
 * `IntrinsicId` that the function table records on the callee's entry, so the
 * linkage is a property of the IR rather than a per-backend guess, and every
 * backend's handler map is keyed by that id instead of an address.
 *
 * @par The fallback invariant
 * The registered address MUST be a real, callable native function with the
 * intrinsic's exact signature -- `registerIntrinsic` rejects a null one for
 * that reason.  An intrinsic is therefore *always* also a valid external
 * call, which is what lets the C++, bytecode and threaded-bytecode backends
 * support every intrinsic without implementing a single one: they emit the
 * ordinary call to the address and get the right answer, only slower.  A
 * backend that expands the call is taking an optimisation, never supplying
 * the only implementation.
 *
 * @par Ordering
 * Resolution happens once, when the callee is interned during trace-to-IR
 * conversion.  A plugin must therefore register its intrinsics before the
 * graphs that call them are converted, which is what the in-tree plugins do
 * -- from a static initialiser, or from the eagerly-constructed backend
 * registry, both of which run before any tracing can start.
 */
class IntrinsicRegistry {
public:
	static IntrinsicRegistry& instance();

	/// Registers @p address as an intrinsic identity and returns its id,
	/// or returns the id it already has.  Idempotent, so several backends may
	/// each claim the same intrinsic and get the same id.
	///
	/// @param name Human-readable name, used in diagnostics and IR text.
	/// @throws RuntimeException if @p address is null -- see the fallback
	///         invariant above; an intrinsic with no callable implementation
	///         would silently break every backend that has no handler for it.
	IntrinsicId registerIntrinsic(void* address, std::string_view name);

	/// The id @p address was registered under, or IntrinsicId::None.
	[[nodiscard]] IntrinsicId lookup(void* address) const;

	/// The name @p id was registered under, or an empty string.
	[[nodiscard]] std::string getName(IntrinsicId id) const;

	[[nodiscard]] size_t size() const;

private:
	IntrinsicRegistry() = default;

	mutable std::mutex mutex_;
	std::unordered_map<void*, IntrinsicId> byAddress_;
	/// Indexed by `id - 1`; IntrinsicId::None is not a registered intrinsic.
	std::vector<std::string> names_;
};

} // namespace nautilus::compiler::ir

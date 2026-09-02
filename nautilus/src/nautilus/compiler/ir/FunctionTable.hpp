#pragma once

#include "nautilus/common/FunctionAttributes.hpp"
#include "nautilus/compiler/ir/FunctionName.hpp"
#include "nautilus/tracing/Types.hpp"
#include <cstdint>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

namespace nautilus {
class NautilusFunctionDefinition;
}

namespace nautilus::compiler::ir {

class FunctionOperation;

/// Dense, module-unique handle for one callee.  Stable for the lifetime of the
/// IRGraph that minted it, and the only thing an operation stores about its
/// target -- names are for humans, addresses are for linkers, ids are for us.
using FunctionId = uint32_t;

inline constexpr FunctionId INVALID_FUNCTION_ID = ~0U;

/// Identifies an intrinsic a backend may expand in place.  Resolved once, when
/// a native callee is interned, from a process-wide address -> id registry, so
/// no backend has to pattern-match a raw address in its lowering inner loop.
enum class IntrinsicId : uint32_t {
	None = 0,
};

/// How a callee is reached.  This is what every backend switches on, replacing
/// the name lookup each of them used to maintain privately.
///
/// Note that `Indirect` is deliberately absent: a call through a value has no
/// symbol and therefore no table entry.  It is a CallOperation carrying
/// INVALID_FUNCTION_ID and an SSA callee operand instead.
enum class Linkage : uint8_t {
	/// Defined in this module.  Its FunctionOperation *is* its declaration.
	Internal,
	/// Foreign machine code, called through its address.
	External,
	/// Foreign machine code a backend may replace with instructions.  Always
	/// retains its address, so a backend with no handler emits a plain call.
	Intrinsic,
};

/**
 * @brief A callee that exists as machine code outside this module.
 *
 * `address` is the identity and is always present.  A non-None `intrinsic`
 * means a backend MAY expand this call; `address` is what it falls back to
 * when none does -- that invariant is what lets the C++, bytecode and
 * threaded-bytecode backends support intrinsics without implementing any.
 */
struct NativeTarget {
	void* address = nullptr;
	IntrinsicId intrinsic = IntrinsicId::None;
	Type resultType = Type::v;
	std::vector<Type> paramTypes;
	FunctionAttributes attrs;
};

/**
 * @brief One entry in the module's function table.
 *
 * The payload is a genuine sum: an internal target is *nothing but* its
 * FunctionOperation, so its name, signature and attributes are read through
 * the definition rather than copied beside it and left to drift.  A native
 * target carries its own, because nothing else in the module describes it.
 */
class FunctionTarget {
public:
	FunctionTarget(FunctionId id, FunctionName name, FunctionOperation* definition);
	FunctionTarget(FunctionId id, FunctionName name, NativeTarget native);

	[[nodiscard]] FunctionId getId() const {
		return id_;
	}

	[[nodiscard]] Linkage getLinkage() const;

	[[nodiscard]] const FunctionName& getName() const {
		return name_;
	}
	[[nodiscard]] FunctionName& getNameMut() {
		return name_;
	}

	/// Internal only; nullptr for native targets.  Non-const because backends
	/// lower from a non-const walk of the graph.
	[[nodiscard]] FunctionOperation* getDefinition() const;

	/// External and Intrinsic only; nullptr for internal targets.
	[[nodiscard]] const NativeTarget* getNative() const;
	[[nodiscard]] NativeTarget* getNativeMut();

	/// Binds a traced body to an internal target.  A call to a Nautilus
	/// function is traced before that function's own body is, so the id is
	/// minted at the call site and the definition arrives later.
	void setDefinition(FunctionOperation* definition);

	/// Signature and attributes, forwarded to the definition for internal
	/// targets and read off the NativeTarget otherwise.
	[[nodiscard]] Type getResultType() const;
	[[nodiscard]] std::vector<Type> getParamTypes() const;
	[[nodiscard]] FunctionAttributes getAttributes() const;

	[[nodiscard]] void* getAddress() const;
	[[nodiscard]] IntrinsicId getIntrinsic() const;

	/// Records what an internal function was found to do, replacing the
	/// pessimistic default every traced call site declares. Set by the
	/// attribute inference pass; ignored for native targets, which carry
	/// whatever the call site declared about them.
	void setDerivedAttributes(FunctionAttributes attrs);

private:
	FunctionId id_;
	FunctionName name_;
	std::variant<FunctionOperation*, NativeTarget> payload_;
	/// Internal targets only. Starts at the pessimistic default, so a graph
	/// that never runs inference behaves exactly as it did before.
	FunctionAttributes derivedAttrs_;
};

/// What a caller hands the table to intern a callee.  Identity is an address
/// or a definition pointer -- never a name, which is the whole point.
struct CalleeDescriptor {
	enum class Kind : uint8_t { Internal, External };

	Kind kind = Kind::External;
	/// External: the real code address.  Internal: the definition pointer.
	void* key = nullptr;
	/// Names known at the call site; the table fills in `minted` and
	/// `emission` itself.
	std::string mangledName;
	std::string demangledName;
	std::string customName;
	Type resultType = Type::v;
	std::vector<Type> paramTypes;
	FunctionAttributes attrs;
};

/**
 * @brief The module's callee table: one interned entry per distinct callee.
 *
 * Replaces the six name-keyed side structures the lowering providers used to
 * maintain (and the IRGraph's own name index).  Two populations share one
 * dense id space:
 *
 *   - defined functions, whose FunctionOperation is their declaration, and
 *   - declared symbols (external, intrinsic), which nothing else describes.
 *
 * Backends walk `getTargets()` once as a prologue, emit a definition or a
 * declaration per `Linkage`, and index their own handles by FunctionId.
 */
class FunctionTable {
public:
	FunctionTable() = default;

	/// Interns @p descriptor, or returns the existing id when this callee has
	/// been seen before.  Idempotent, keyed on identity.
	FunctionId intern(const CalleeDescriptor& descriptor);

	/// Looks up an already-interned callee by identity, or INVALID_FUNCTION_ID.
	[[nodiscard]] FunctionId find(void* key) const;

	/// Looks up the entry a definition was bound to, or INVALID_FUNCTION_ID.
	/// Backends use this when emitting a function body, so the name they
	/// declare it under is the same one call sites will use.
	[[nodiscard]] FunctionId findByDefinition(const FunctionOperation* definition) const;

	/// Points an additional identity at an existing entry.
	///
	/// One body can be denoted by more than one identity -- a NautilusFunction
	/// that shares a name with a module-registered entry function is traced
	/// once and reached through both. They must resolve to one entry, or a
	/// call site and the definition it targets are emitted under different
	/// names.
	void alias(void* key, FunctionId id);

	/// Binds a traced body to an internal target minted earlier at a call site.
	void define(FunctionId id, FunctionOperation* definition);

	[[nodiscard]] const FunctionTarget& get(FunctionId id) const;
	[[nodiscard]] FunctionTarget& getMut(FunctionId id);

	[[nodiscard]] bool contains(FunctionId id) const {
		return id < targets_.size();
	}

	[[nodiscard]] const std::vector<FunctionTarget>& getTargets() const {
		return targets_;
	}

	[[nodiscard]] size_t size() const {
		return targets_.size();
	}

private:
	/// Produces the readable, valid, unique identifier a backend emits.
	/// Seeded from the display name so generated MLIR and C++ carry real
	/// function names; falls back to the minted name when there is nothing to
	/// seed from, and always uniquifies, because two distinct callees can
	/// sanitise to the same spelling and no single backend could detect that.
	std::string mintEmissionName(const FunctionName& name);

	std::vector<FunctionTarget> targets_;
	std::unordered_map<void*, FunctionId> byKey_;
	std::unordered_map<const FunctionOperation*, FunctionId> byDefinition_;
	std::unordered_map<std::string, uint32_t> emissionNameUses_;
};

} // namespace nautilus::compiler::ir

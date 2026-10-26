#pragma once

#include "nautilus/extensions/OpInfo.hpp"
#include "nautilus/extensions/TypeInfo.hpp"
#include <atomic>
#include <cstdint>
#include <memory>
#include <mutex>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace nautilus::ext {

/// Thrown for a registration the registry rejects: a reserved name, a duplicate, a late registration.
class RegistrationError : public std::runtime_error {
public:
	using std::runtime_error::runtime_error;
};

/// Who registers: the core owns the `core.*` names, plugins every other plugin name.
enum class Owner { Plugin, Core };

/// One registry for types, operations, intrinsics and passes with dense ids
/// (docs/plans/nautilus-0.4/04-extensions.md §3).
///
/// Registration happens before the first engine, from static initializers or plugin init functions. The registry is
/// frozen by its first reader (an engine, an image load, a text parse, a C API module): the freeze is a release store
/// of a flag, registration checks the flag under a mutex, and lookups after the freeze are lock-free acquire loads.
/// Late registration throws (D13).
class ExtensionRegistry {
public:
	static ExtensionRegistry& instance();

	// ---- registration -----------------------------------------------------------------------------------------

	/// Registers a type and returns its `TypeId` (16 to 255, at most 240 types).
	uint8_t registerType(TypeInfo info, Owner owner = Owner::Plugin);
	/// Registers an operation and returns its opcode (256 and up).
	uint16_t registerOp(ExtOpInfo info, Owner owner = Owner::Plugin);
	/// Registers an intrinsic and returns its dense id.
	uint32_t registerIntrinsic(IntrinsicInfo info, Owner owner = Owner::Plugin);
	/// Registers a pass by name and returns its dense id (the passes themselves belong to M3.9).
	uint32_t registerPass(std::string name, Owner owner = Owner::Plugin);

	/// Freezes the registry. Idempotent; called by every first reader.
	void freeze();
	bool frozen() const {
		return frozen_.load(std::memory_order_acquire);
	}

	// ---- lookups (lock-free after the freeze) -------------------------------------------------------------------

	const TypeInfo* type(uint8_t id) const;
	const ExtOpInfo* op(uint16_t opcode) const;
	const IntrinsicInfo* intrinsic(uint32_t id) const;
	std::optional<uint8_t> findType(std::string_view name) const;
	std::optional<uint16_t> findOp(std::string_view name) const;
	std::optional<uint32_t> findIntrinsic(std::string_view name) const;
	/// A traced `invoke` of a registered fallback address becomes an intrinsic callee at trace time (13 §4.6.1).
	std::optional<uint32_t> findIntrinsicByFallback(const void* address) const;
	size_t typeCount() const;
	size_t opCount() const;
	size_t intrinsicCount() const;

	/// How often a lookup or a registration took the registry's mutex; after the freeze lookups take none.
	uint64_t lockAcquisitions() const {
		return locks_.load(std::memory_order_relaxed);
	}

private:
	/// Registers the intrinsics the core owns (CoreIntrinsics.cpp), so they exist in every binary however it is linked.
	ExtensionRegistry();
	friend struct ExtensionRegistryTestAccess;

	template <class F>
	auto read(F&& f) const;
	void checkOpen(std::string_view what, const std::string& name) const;
	void checkName(const std::string& name, Owner owner, const char* what) const;

	std::atomic<bool> frozen_ {false};
	mutable std::mutex mutex_;
	mutable std::atomic<uint64_t> locks_ {0};
	std::vector<TypeInfo> types_;
	std::vector<ExtOpInfo> ops_;
	std::vector<IntrinsicInfo> intrinsics_;
	std::vector<std::string> passes_;
	std::unordered_map<std::string, uint32_t> byName_; // "type:...", "op:...", "intrinsic:...", "pass:..."
	std::unordered_map<const void*, uint32_t> byFallback_;
};

/// Splits "name@version" at the last '@'. Returns false if there is no '@' or the version is not a decimal u32.
bool splitNameVersion(std::string_view text, std::string_view& name, uint32_t& version);

/// Whether `plugin` may be the plugin name of an extension: not a core mnemonic of 13 §5 or the first dot-separated
/// component of one, not a type name of §2, not a keyword of §9.2, not `nir`. (The prefix `core` is checked against the
/// owner by the registry.) On failure `why` says which rule.
bool isValidPluginName(std::string_view plugin, std::string* why = nullptr);

} // namespace nautilus::ext

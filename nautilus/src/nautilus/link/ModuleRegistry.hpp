#pragma once

#include "nautilus/Executable.hpp"
#include "nautilus/Module.hpp"
#include "nautilus/exceptions/RuntimeException.hpp"
#include "nautilus/link/ImportSlots.hpp"
#include "nautilus/nir/Hash.hpp"
#include <cstdint>
#include <memory>
#include <mutex>
#include <optional>
#include <span>
#include <string>
#include <unordered_map>
#include <vector>

namespace nautilus::nir {
class Module;
}

/// Modules and linking (docs/plans/nautilus-0.4/13-nir-spec.md §4.4, plan M4.6).
///
/// A `ModuleRegistry` belongs to an engine and holds the compiled modules that have a name. A module takes part in it
/// through a `ModuleHandle`, which walks the lifecycle of §4.4.1:
///
///   built     the handle exists and the name is reserved (a name refers to one module);
///   linked    every import is resolved against a compiled exporter: signature equal, guarantees implied, the import
///             graph acyclic; the importer has pinned its exporters and owns an `ImportLink` (the slots);
///   compiled  the module's code is installed in its `ModuleState`; its exports are visible to other modules, and each
///             time the state's executable changes (a promotion) the slots of its importers are republished;
///   released  the handle is gone: the name is free and the pins on the exporters are dropped.
///
/// The handle is owned by the module's `ModuleState` (`ModuleState::link`), and the exporters are pinned through the
/// `ImportLink` the handle owns, so a module is released only after everything that links to it is: the release order
/// of §4.4.1 needs no bookkeeping beyond ownership.
namespace nautilus::link {

/// A failed link (§4.4.2): the message names the import and the reason. Linking is atomic: nothing is bound.
class LinkError : public RuntimeException {
public:
	explicit LinkError(std::string message) : RuntimeException(std::move(message)) {
	}
};

enum class ModuleStage : uint8_t { Built, Linked, Compiled, Released };
const char* toString(ModuleStage stage);

/// What a compiled module offers under a name: the interface of an exported function (§4.4).
struct ExportInfo {
	std::string name;
	/// The type words of the result, then of the parameters (13 §4.5.1).
	std::vector<uint32_t> signature;
	/// The guarantee bits (13 §4.6.2).
	uint32_t attrs = 0;
};

/// One import of a module that is to be linked (§4.4, `ImportRecord`) with what the C++ front end knows of it.
struct ImportRequest {
	std::string module;
	std::string function;
	std::vector<uint32_t> signature;
	uint32_t attrs = 0;
	void* captureThunk = nullptr;
	void* frameThunk = nullptr;
};

class ModuleRegistry;
class ModuleHandle;

/// The result of linking a module: its import slots and the exporters it has pinned (§4.4.2, steps 4 and 5).
class ImportLink {
public:
	struct Binding {
		std::string module;
		std::string function;
		/// Pins: the exporter cannot be released while this link exists.
		std::shared_ptr<ModuleHandle> exporter;
		std::shared_ptr<engine::details::ModuleState> exporterState;
		/// The structural hash of the exporter's module at link time: what compiled code that inlines the import
		/// records so that a different exporter never reuses it (§4.4.3, §7.3). Empty if the exporter keeps no NIR.
		std::optional<nir::Hash128> exporterHash;
	};

	explicit ImportLink(size_t count) : slots(count), bindings(count) {
	}
	ImportLink(const ImportLink&) = delete;
	ImportLink& operator=(const ImportLink&) = delete;

	ImportSlots slots;
	std::vector<Binding> bindings;

	/// Points slot `index` at the entry point of the binding's function in `executable`, with a release store. The
	/// executable is kept alive as long as the link: a call that started in the code a slot used to name may still be
	/// running when the slot changes (13 §4.4.2). Returns false, and changes nothing, if there is no such entry point.
	bool refresh(size_t index, const std::shared_ptr<compiler::Executable>& executable);

	/// The executables the slots have named so far (the old ones are retained, see `refresh`).
	[[nodiscard]] size_t retainedExecutables() const;

private:
	mutable std::mutex mutex_;
	std::vector<std::shared_ptr<compiler::Executable>> retained_;
};

/// The membership of one module in a registry; see the namespace comment.
class ModuleHandle : public std::enable_shared_from_this<ModuleHandle> {
public:
	/// Use `ModuleRegistry::begin`.
	ModuleHandle(std::shared_ptr<ModuleRegistry> registry, std::string name);
	~ModuleHandle();
	ModuleHandle(const ModuleHandle&) = delete;
	ModuleHandle& operator=(const ModuleHandle&) = delete;

	[[nodiscard]] const std::string& name() const {
		return name_;
	}
	[[nodiscard]] ModuleStage stage() const;

	/// built -> linked. Resolves every import (§4.4.2) and pins the exporters. Throws `LinkError` for a missing module
	/// or function, an exporter that is not compiled, a signature mismatch, guarantees the export does not give and a
	/// cycle; no slot is bound then and the handle stays built.
	std::shared_ptr<const ImportLink> link(std::span<const ImportRequest> imports);

	/// linked -> compiled (a built module passes through linked with no imports). `state` holds the module's code; the
	/// slots of every importer follow its executable from now on. `module` is the SSA NIR of the module, kept for
	/// inlining and for the structural hash; null for a module that has none (an interpreted one).
	void publish(const std::shared_ptr<engine::details::ModuleState>& state, std::vector<ExportInfo> exports,
	             std::shared_ptr<const nir::Module> module = nullptr);

	/// -> released: the name is free again and the pins on the exporters are dropped. The destructor does it.
	void release();

	/// The links of this module, or null before it is linked and after it is released.
	[[nodiscard]] std::shared_ptr<const ImportLink> imports() const;
	[[nodiscard]] std::vector<std::string> importedModules() const;
	/// The exported functions, empty before the module is compiled and after it is released.
	[[nodiscard]] std::vector<ExportInfo> exports() const;
	[[nodiscard]] std::shared_ptr<const nir::Module> module() const;
	/// The structural hash of `module()` (computed on first use), if the handle keeps one.
	[[nodiscard]] std::optional<nir::Hash128> structuralHash() const;

private:
	friend class ModuleRegistry;
	struct Dependent {
		std::weak_ptr<ImportLink> link;
		size_t index;
	};

	void addDependent(const std::shared_ptr<ImportLink>& link, size_t index);
	/// The module's executable changed: points the slots of the importers at it. Called with the state's mutex held.
	void onInstalled(const std::shared_ptr<compiler::Executable>& executable);

	const std::shared_ptr<ModuleRegistry> registry_;
	const std::string name_;

	// guarded by the registry's mutex
	ModuleStage stage_ = ModuleStage::Built;
	std::weak_ptr<engine::details::ModuleState> state_;
	std::vector<ExportInfo> exports_;
	std::shared_ptr<const nir::Module> module_;
	std::vector<std::string> importedModules_;
	std::shared_ptr<const ImportLink> imports_;

	// the importers, guarded by their own mutex: onInstalled runs with the state's mutex held, which is taken before
	// the registry's everywhere, so it must not take the registry's
	std::mutex dependentsMutex_;
	std::vector<Dependent> dependents_;

	mutable std::once_flag hashOnce_;
	mutable std::optional<nir::Hash128> hash_;
};

/// The compiled modules of an engine that have a name (§4.4.2). Thread-safe (§6.5).
class ModuleRegistry : public std::enable_shared_from_this<ModuleRegistry> {
public:
	ModuleRegistry() = default;
	ModuleRegistry(const ModuleRegistry&) = delete;
	ModuleRegistry& operator=(const ModuleRegistry&) = delete;

	/// Creates a built module and reserves `name`; an empty name makes an anonymous module (it imports, nobody can
	/// import from it). Throws `LinkError` if the name is registered.
	std::shared_ptr<ModuleHandle> begin(const std::string& name);

	/// The exported function `function` of the compiled module `module`, if there is one.
	[[nodiscard]] std::optional<ExportInfo> findExport(const std::string& module, const std::string& function) const;
	/// The state of the compiled module `module`, or null.
	[[nodiscard]] std::shared_ptr<engine::details::ModuleState> compiledState(const std::string& module) const;
	[[nodiscard]] std::optional<ModuleStage> stageOf(const std::string& module) const;
	[[nodiscard]] std::shared_ptr<ModuleHandle> find(const std::string& module) const;

	/// True if `from` imports `to`, directly or through other modules.
	[[nodiscard]] bool importsTransitively(const std::string& from, const std::string& to) const;

	/// The number of registered (not released) modules.
	[[nodiscard]] size_t size() const;

private:
	friend class ModuleHandle;
	struct Entry {
		std::weak_ptr<ModuleHandle> handle;
		const ModuleHandle* raw = nullptr;
	};

	bool importsTransitivelyLocked(const std::string& from, const std::string& to) const;

	mutable std::mutex mutex_;
	std::unordered_map<std::string, Entry> modules_;
};

/// `words` as text, `i32 (ptr, i64)`-like, for the messages of a signature mismatch.
std::string describeSignature(const std::vector<uint32_t>& words);

} // namespace nautilus::link

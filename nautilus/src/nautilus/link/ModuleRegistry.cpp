#include "nautilus/link/ModuleRegistry.hpp"
#include "nautilus/common/OptionRegistry.hpp"
#include "nautilus/logging.hpp"
#include "nautilus/nir/Module.hpp"
#include "nautilus/nir/Records.hpp"
#include <algorithm>
#include <fmt/format.h>
#include <unordered_set>

namespace nautilus::link {

const char* toString(ModuleStage stage) {
	switch (stage) {
	case ModuleStage::Built:
		return "built";
	case ModuleStage::Linked:
		return "linked";
	case ModuleStage::Compiled:
		return "compiled";
	case ModuleStage::Released:
		return "released";
	}
	return "?";
}

std::string describeSignature(const std::vector<uint32_t>& words) {
	auto typeName = [](uint32_t word) {
		static constexpr const char* names[] = {"void", "bool", "i8", "i16", "i32", "i64", "f32", "f64", "ptr"};
		const auto id = static_cast<uint8_t>(nir::typeOf(word));
		std::string text = id < std::size(names) ? names[id] : fmt::format("type#{}", id);
		switch (nir::extKindOf(word)) {
		case nir::ExtKind::Zext:
			text += " zext";
			break;
		case nir::ExtKind::Sext:
			text += " sext";
			break;
		case nir::ExtKind::None:
			break;
		}
		return text;
	};
	if (words.empty()) {
		return "(no signature)";
	}
	std::string text = typeName(words[0]) + " (";
	for (size_t i = 1; i < words.size(); ++i) {
		text += (i > 1 ? ", " : "") + typeName(words[i]);
	}
	return text + ")";
}

// ---- ImportLink ---------------------------------------------------------------------------------------------------

bool ImportLink::refresh(size_t index, const std::shared_ptr<compiler::Executable>& executable) {
	if (!executable) {
		return false;
	}
	void* entry = nullptr;
	try {
		entry = executable->getInvocableFunctionPtr(bindings[index].function);
	} catch (const std::exception& e) {
		log::warn("Import '{}'.'{}': no entry point in the new code: {}", bindings[index].module,
		          bindings[index].function, e.what());
		return false;
	}
	if (entry == nullptr) {
		return false;
	}
	{
		// the code the slot named until now may still be executing: it stays alive as long as this link does
		std::lock_guard lock(mutex_);
		if (std::find(retained_.begin(), retained_.end(), executable) == retained_.end()) {
			retained_.push_back(executable);
		}
	}
	slots.publish(index, entry);
	return true;
}

size_t ImportLink::retainedExecutables() const {
	std::lock_guard lock(mutex_);
	return retained_.size();
}

// ---- ModuleRegistry -----------------------------------------------------------------------------------------------

std::shared_ptr<ModuleHandle> ModuleRegistry::begin(const std::string& name) {
	auto self = shared_from_this();
	if (name.empty()) {
		return std::make_shared<ModuleHandle>(std::move(self), name);
	}
	std::lock_guard lock(mutex_);
	if (auto it = modules_.find(name); it != modules_.end() && !it->second.handle.expired()) {
		throw LinkError(fmt::format("module '{}' cannot be registered: a module with that name is registered", name));
	}
	auto handle = std::make_shared<ModuleHandle>(std::move(self), name);
	modules_[name] = Entry {handle, handle.get()};
	return handle;
}

std::shared_ptr<ModuleHandle> ModuleRegistry::find(const std::string& module) const {
	std::lock_guard lock(mutex_);
	const auto it = modules_.find(module);
	return it == modules_.end() ? nullptr : it->second.handle.lock();
}

std::optional<ExportInfo> ModuleRegistry::findExport(const std::string& module, const std::string& function) const {
	std::lock_guard lock(mutex_);
	const auto it = modules_.find(module);
	const auto handle = it == modules_.end() ? nullptr : it->second.handle.lock();
	if (!handle || handle->stage_ != ModuleStage::Compiled) {
		return std::nullopt;
	}
	for (const auto& e : handle->exports_) {
		if (e.name == function) {
			return e;
		}
	}
	return std::nullopt;
}

std::shared_ptr<engine::details::ModuleState> ModuleRegistry::compiledState(const std::string& module) const {
	std::lock_guard lock(mutex_);
	const auto it = modules_.find(module);
	const auto handle = it == modules_.end() ? nullptr : it->second.handle.lock();
	return handle && handle->stage_ == ModuleStage::Compiled ? handle->state_.lock() : nullptr;
}

std::optional<ModuleStage> ModuleRegistry::stageOf(const std::string& module) const {
	std::lock_guard lock(mutex_);
	const auto it = modules_.find(module);
	const auto handle = it == modules_.end() ? nullptr : it->second.handle.lock();
	if (!handle) {
		return std::nullopt;
	}
	return handle->stage_;
}

bool ModuleRegistry::importsTransitively(const std::string& from, const std::string& to) const {
	std::lock_guard lock(mutex_);
	return importsTransitivelyLocked(from, to);
}

bool ModuleRegistry::importsTransitivelyLocked(const std::string& from, const std::string& to) const {
	std::vector<std::string> work {from};
	std::unordered_set<std::string> seen {from};
	while (!work.empty()) {
		const auto current = std::move(work.back());
		work.pop_back();
		const auto it = modules_.find(current);
		const auto handle = it == modules_.end() ? nullptr : it->second.handle.lock();
		if (!handle) {
			continue;
		}
		for (const auto& next : handle->importedModules_) {
			if (next == to) {
				return true;
			}
			if (seen.insert(next).second) {
				work.push_back(next);
			}
		}
	}
	return false;
}

size_t ModuleRegistry::size() const {
	std::lock_guard lock(mutex_);
	size_t count = 0;
	for (const auto& [name, entry] : modules_) {
		count += !entry.handle.expired();
	}
	return count;
}

// ---- ModuleHandle -------------------------------------------------------------------------------------------------

ModuleHandle::ModuleHandle(std::shared_ptr<ModuleRegistry> registry, std::string name)
    : registry_(std::move(registry)), name_(std::move(name)) {
}

ModuleHandle::~ModuleHandle() {
	release();
}

ModuleStage ModuleHandle::stage() const {
	std::lock_guard lock(registry_->mutex_);
	return stage_;
}

std::shared_ptr<const ImportLink> ModuleHandle::imports() const {
	std::lock_guard lock(registry_->mutex_);
	return imports_;
}

std::vector<std::string> ModuleHandle::importedModules() const {
	std::lock_guard lock(registry_->mutex_);
	return importedModules_;
}

std::shared_ptr<const nir::Module> ModuleHandle::module() const {
	std::lock_guard lock(registry_->mutex_);
	return module_;
}

std::optional<nir::Hash128> ModuleHandle::structuralHash() const {
	const auto body = module();
	if (!body) {
		return std::nullopt;
	}
	std::call_once(hashOnce_, [&] {
		try {
			hash_ = nir::structuralHash(*body, effectiveOptions({}));
		} catch (const std::exception& e) {
			log::warn("The structural hash of module '{}' is unavailable: {}", name_, e.what());
		}
	});
	return hash_;
}

void ModuleHandle::addDependent(const std::shared_ptr<ImportLink>& link, size_t index) {
	std::lock_guard lock(dependentsMutex_);
	dependents_.push_back({link, index});
}

void ModuleHandle::onInstalled(const std::shared_ptr<compiler::Executable>& executable) {
	std::lock_guard lock(dependentsMutex_);
	for (auto it = dependents_.begin(); it != dependents_.end();) {
		if (const auto link = it->link.lock()) {
			link->refresh(it->index, executable);
			++it;
		} else {
			it = dependents_.erase(it);
		}
	}
}

std::shared_ptr<const ImportLink> ModuleHandle::link(std::span<const ImportRequest> imports) {
	const auto importer = name_.empty() ? std::string("<anonymous>") : name_;
	struct Resolved {
		std::shared_ptr<ModuleHandle> exporter;
		std::shared_ptr<engine::details::ModuleState> state;
	};
	std::vector<Resolved> resolved;
	resolved.reserve(imports.size());
	{
		// step 1-3 of §4.4.2 under the registry's lock: the exporters cannot change stage while they are looked at
		std::lock_guard lock(registry_->mutex_);
		if (stage_ != ModuleStage::Built) {
			throw LinkError(fmt::format("module '{}' cannot be linked: it is {}", importer, toString(stage_)));
		}
		for (const auto& request : imports) {
			const auto where =
			    fmt::format("import '{}'.'{}' of module '{}'", request.module, request.function, importer);
			if (!name_.empty() && request.module == name_) {
				throw LinkError(fmt::format("{}: cycle, a module cannot import from itself", where));
			}
			const auto it = registry_->modules_.find(request.module);
			auto exporter = it == registry_->modules_.end() ? nullptr : it->second.handle.lock();
			if (!exporter) {
				throw LinkError(fmt::format("{}: module '{}' is not registered", where, request.module));
			}
			if (!name_.empty() && registry_->importsTransitivelyLocked(request.module, name_)) {
				throw LinkError(fmt::format("{}: cycle, module '{}' imports '{}', directly or through other modules",
				                            where, request.module, name_));
			}
			if (exporter->stage_ != ModuleStage::Compiled) {
				throw LinkError(fmt::format("{}: module '{}' is not compiled (it is {})", where, request.module,
				                            toString(exporter->stage_)));
			}
			const auto exported = std::find_if(exporter->exports_.begin(), exporter->exports_.end(),
			                                   [&](const ExportInfo& e) { return e.name == request.function; });
			if (exported == exporter->exports_.end()) {
				throw LinkError(fmt::format("{}: module '{}' has no exported function '{}'", where, request.module,
				                            request.function));
			}
			if (exported->signature != request.signature) {
				throw LinkError(
				    fmt::format("{}: signature mismatch, the import is built against {} but the export is {}", where,
				                describeSignature(request.signature), describeSignature(exported->signature)));
			}
			if (!nir::attrsImply(exported->attrs, request.attrs)) {
				throw LinkError(fmt::format("{}: the import assumes the guarantees {:#x} but the export gives {:#x}",
				                            where, request.attrs & nir::GUARANTEE_MASK,
				                            exported->attrs & nir::GUARANTEE_MASK));
			}
			auto state = exporter->state_.lock();
			if (!state) {
				throw LinkError(fmt::format("{}: module '{}' has been released", where, request.module));
			}
			resolved.push_back({std::move(exporter), std::move(state)});
		}
	}

	// steps 4 and 5: bind and pin. Nothing is visible to anybody until the link is complete: the dependents hold the
	// link weakly, so a link that fails here just goes out of scope.
	auto link = std::make_shared<ImportLink>(imports.size());
	for (size_t i = 0; i < imports.size(); ++i) {
		const auto& request = imports[i];
		auto& binding = link->bindings[i];
		binding.module = request.module;
		binding.function = request.function;
		binding.exporter = resolved[i].exporter;
		binding.exporterState = resolved[i].state;
		binding.exporterHash = resolved[i].exporter->structuralHash();
		link->slots.setThunks(i, request.captureThunk, request.frameThunk);
		// The state's lock is held from reading the executable until the importer is a dependent: a promotion that
		// installs new code in between would be missed by both.
		std::shared_lock stateLock(resolved[i].state->mutex);
		if (!link->refresh(i, resolved[i].state->executable)) {
			throw LinkError(fmt::format("import '{}'.'{}' of module '{}': module '{}' has no native entry point for it "
			                            "yet (it still runs interpreted)",
			                            request.module, request.function, importer, request.module));
		}
		resolved[i].exporter->addDependent(link, i);
	}

	std::lock_guard lock(registry_->mutex_);
	if (stage_ != ModuleStage::Built) {
		throw LinkError(fmt::format("module '{}' cannot be linked: it is {}", importer, toString(stage_)));
	}
	for (const auto& request : imports) {
		if (std::find(importedModules_.begin(), importedModules_.end(), request.module) == importedModules_.end()) {
			importedModules_.push_back(request.module);
		}
	}
	imports_ = link;
	stage_ = ModuleStage::Linked;
	return link;
}

void ModuleHandle::publish(const std::shared_ptr<engine::details::ModuleState>& state, std::vector<ExportInfo> exports,
                           std::shared_ptr<const nir::Module> module) {
	if (stage() == ModuleStage::Built) {
		link({});
	}
	{
		std::lock_guard lock(registry_->mutex_);
		if (stage_ != ModuleStage::Linked) {
			throw LinkError(fmt::format("module '{}' cannot be published: it is {}", name_, toString(stage_)));
		}
	}
	// The observer goes in before the module is visible as compiled: an importer links against compiled modules only,
	// so every promotion after the first link is seen by it.
	{
		std::unique_lock stateLock(state->mutex);
		state->link = shared_from_this();
		state->installObservers.push_back(
		    [this](engine::details::ModuleState& installed) { onInstalled(installed.executable); });
	}
	std::lock_guard lock(registry_->mutex_);
	state_ = state;
	exports_ = std::move(exports);
	module_ = std::move(module);
	stage_ = ModuleStage::Compiled;
}

void ModuleHandle::release() {
	std::shared_ptr<const ImportLink> dropped;
	{
		std::lock_guard lock(registry_->mutex_);
		if (stage_ == ModuleStage::Released) {
			return;
		}
		if (const auto it = registry_->modules_.find(name_);
		    it != registry_->modules_.end() && it->second.raw == this) {
			registry_->modules_.erase(it);
		}
		stage_ = ModuleStage::Released;
		dropped = std::move(imports_);
		importedModules_.clear();
		exports_.clear();
		module_.reset();
	}
	// the pins go outside the lock: releasing the last pin of an exporter releases it, which locks the registry again
}

} // namespace nautilus::link

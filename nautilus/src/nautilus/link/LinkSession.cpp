#include "nautilus/link/LinkSession.hpp"
#include "nautilus/config.hpp"
#include "nautilus/nir/Module.hpp"
#include "nautilus/nir/Records.hpp"
#include <algorithm>
#include <fmt/format.h>

#ifdef ENABLE_TRACING
#include "nautilus/tracing/nir/NirTrace.hpp"
#endif

namespace nautilus::engine::details {

uint32_t signatureWord(Type type) {
#ifdef ENABLE_TRACING
	return tracing::nirtrace::typeWord(type);
#else
	using nir::ExtKind;
	using nir::TypeId;
	switch (type) {
	case Type::v:
		return nir::makeTypeWord(TypeId::Void);
	case Type::b:
		return nir::makeTypeWord(TypeId::Bool, ExtKind::Zext);
	case Type::i8:
		return nir::makeTypeWord(TypeId::I8, ExtKind::Sext);
	case Type::i16:
		return nir::makeTypeWord(TypeId::I16, ExtKind::Sext);
	case Type::i32:
		return nir::makeTypeWord(TypeId::I32, ExtKind::Sext);
	case Type::i64:
	case Type::ui64:
		return nir::makeTypeWord(TypeId::I64);
	case Type::ui8:
		return nir::makeTypeWord(TypeId::I8, ExtKind::Zext);
	case Type::ui16:
		return nir::makeTypeWord(TypeId::I16, ExtKind::Zext);
	case Type::ui32:
		return nir::makeTypeWord(TypeId::I32, ExtKind::Zext);
	case Type::f32:
		return nir::makeTypeWord(TypeId::F32);
	case Type::f64:
		return nir::makeTypeWord(TypeId::F64);
	case Type::ptr:
		return nir::makeTypeWord(TypeId::Ptr);
	}
	return nir::makeTypeWord(TypeId::Void);
#endif
}

uint32_t attributeBits(const FunctionAttributes& a) {
	uint32_t bits = 0;
	if (a.modRefInfo == ModRefInfo::NoModRef || a.modRefInfo == ModRefInfo::Ref) {
		bits |= nir::AttrNoArgWrite | nir::AttrNoOtherWrite;
	}
	if (a.modRefInfo == ModRefInfo::NoModRef || a.modRefInfo == ModRefInfo::Mod) {
		bits |= nir::AttrNoArgRead | nir::AttrNoOtherRead;
	}
	if (a.willReturn) {
		bits |= nir::AttrWillReturn;
	}
	if (a.noUnwind) {
		bits |= nir::AttrNoUnwind;
	}
	return bits;
}

FunctionAttributes attributesFromBits(uint32_t bits) {
	constexpr uint32_t noRead = nir::AttrNoArgRead | nir::AttrNoOtherRead;
	constexpr uint32_t noWrite = nir::AttrNoArgWrite | nir::AttrNoOtherWrite;
	// a half guarantee (no read through arguments only) is none that FunctionAttributes can state
	const bool reads = (bits & noRead) != noRead;
	const bool writes = (bits & noWrite) != noWrite;
	FunctionAttributes attributes;
	attributes.modRefInfo =
	    reads ? (writes ? ModRefInfo::ModRef : ModRefInfo::Ref) : (writes ? ModRefInfo::Mod : ModRefInfo::NoModRef);
	attributes.willReturn = (bits & nir::AttrWillReturn) != 0;
	attributes.noUnwind = (bits & nir::AttrNoUnwind) != 0;
	return attributes;
}

std::optional<FunctionAttributes> checkImport(const std::shared_ptr<link::ModuleRegistry>& registry,
                                              const ImportDeclaration& declaration) {
	const auto where = fmt::format("import '{}'.'{}'", declaration.module, declaration.function);
	if (registry->stageOf(declaration.module) != link::ModuleStage::Compiled) {
		return std::nullopt; // linked, and checked again, when the importer is compiled
	}
	const auto exported = registry->findExport(declaration.module, declaration.function);
	if (!exported) {
		throw link::LinkError(fmt::format("{}: module '{}' has no exported function '{}'", where, declaration.module,
		                                  declaration.function));
	}
	if (exported->signature != declaration.signature) {
		throw link::LinkError(fmt::format("{}: signature mismatch, the import is built against {} but the export is {}",
		                                  where, link::describeSignature(declaration.signature),
		                                  link::describeSignature(exported->signature)));
	}
	return attributesFromBits(exported->attrs);
}

std::shared_ptr<ModuleState> importedState(const std::shared_ptr<link::ModuleRegistry>& registry,
                                           const std::string& module) {
	auto state = registry->compiledState(module);
	if (!state) {
		throw link::LinkError(fmt::format("module '{}' is not a compiled module of this engine", module));
	}
	return state;
}

// ---- LinkSession --------------------------------------------------------------------------------------------------

struct LinkSession::Impl {
	std::shared_ptr<link::ModuleRegistry> registry;
	std::string moduleName;
	std::vector<ExportDeclaration> exports;
	std::vector<ImportDeclaration> imports;
	std::shared_ptr<link::ModuleHandle> handle;
	std::shared_ptr<const nir::Module> module;
};

namespace {
thread_local LinkSession* currentSession = nullptr;
}

LinkSession::LinkSession(std::shared_ptr<link::ModuleRegistry> registry, std::string moduleName,
                         std::vector<ExportDeclaration> exports, std::vector<ImportDeclaration> imports)
    : impl_(std::make_unique<Impl>()) {
	impl_->handle = registry->begin(moduleName);
	impl_->registry = std::move(registry);
	impl_->moduleName = std::move(moduleName);
	impl_->exports = std::move(exports);
	impl_->imports = std::move(imports);
	previous_ = currentSession;
	currentSession = this;
}

LinkSession::~LinkSession() {
	if (currentSession == this) {
		currentSession = previous_;
	}
}

LinkSession* LinkSession::current() noexcept {
	return currentSession;
}

void LinkSession::commit(const std::shared_ptr<ModuleState>& state) {
	std::vector<link::ExportInfo> exports;
	exports.reserve(impl_->exports.size());
	const auto* module = impl_->module.get();
	for (const auto& declared : impl_->exports) {
		link::ExportInfo info {declared.name, declared.signature, attributeBits(declared.attributes)};
		if (module != nullptr) {
			// the module is what was compiled: its signature and the guarantees it ended up with are the interface
			for (const auto& function : module->functions) {
				if (module->name(function.nameId) != declared.name) {
					continue;
				}
				const auto& signature = module->signatures[function.signature];
				info.signature.assign(1, signature.result);
				for (uint32_t i = 0; i < signature.paramCount; ++i) {
					info.signature.push_back(module->paramType(signature, i));
				}
				info.attrs = function.attrs & nir::GUARANTEE_MASK;
				break;
			}
		}
		exports.push_back(std::move(info));
	}
	impl_->handle->publish(state, std::move(exports), impl_->module);
}

} // namespace nautilus::engine::details

namespace nautilus::link {

struct SessionAccess {
	static engine::details::LinkSession::Impl& impl(engine::details::LinkSession& session) {
		return *session.impl_;
	}
};

std::shared_ptr<const ImportLink> prepareModule(engine::details::LinkSession& session, nir::Module& ssa) {
	auto& impl = SessionAccess::impl(session);
	if (!impl.moduleName.empty()) {
		ssa.module.nameId = ssa.internName(impl.moduleName);
	}
	// The guarantees an export declares are part of its interface (§4.5); the callee records of its internal callers
	// state the same bits (NIR-V033).
	for (const auto& declared : impl.exports) {
		const auto bits = engine::details::attributeBits(declared.attributes);
		if (bits == 0) {
			continue;
		}
		for (uint32_t f = 0; f < ssa.functions.size(); ++f) {
			if (ssa.name(ssa.functions[f].nameId) != declared.name) {
				continue;
			}
			ssa.functions[f].attrs = (ssa.functions[f].attrs & ~nir::GUARANTEE_MASK) | bits;
			for (auto& callee : ssa.callees) {
				if (callee.kind == static_cast<uint8_t>(nir::CalleeKind::Internal) && callee.target == f) {
					callee.attrs = (callee.attrs & ~nir::GUARANTEE_MASK) | bits;
				}
			}
		}
	}

	std::vector<ImportRequest> requests;
	requests.reserve(ssa.imports.size());
	for (const auto& record : ssa.imports) {
		ImportRequest request;
		request.module = std::string(ssa.name(record.moduleNameId));
		request.function = std::string(ssa.name(record.functionNameId));
		const auto& signature = ssa.signatures[record.signature];
		request.signature.push_back(signature.result);
		for (uint32_t i = 0; i < signature.paramCount; ++i) {
			request.signature.push_back(ssa.paramType(signature, i));
		}
		request.attrs = record.attrs;
		const auto declared =
		    std::find_if(impl.imports.begin(), impl.imports.end(), [&](const engine::details::ImportDeclaration& d) {
			    return d.module == request.module && d.function == request.function;
		    });
		if (declared != impl.imports.end()) {
			request.captureThunk = declared->captureThunk;
			request.frameThunk = declared->frameThunk;
		}
		requests.push_back(std::move(request));
	}
	return impl.handle->link(requests);
}

void noteCompiledModule(engine::details::LinkSession& session, std::shared_ptr<const nir::Module> module) {
	SessionAccess::impl(session).module = std::move(module);
}

} // namespace nautilus::link

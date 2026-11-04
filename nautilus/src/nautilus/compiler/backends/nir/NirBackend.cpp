#include "nautilus/compiler/backends/nir/NirBackend.hpp"
#include "nautilus/config.hpp"
#include "nautilus/exceptions/NotImplementedException.hpp"
#include "nautilus/exceptions/RuntimeException.hpp"
#ifdef ENABLE_TBC_BACKEND
#include "nautilus/compiler/backends/tbc/TBCNirBackend.hpp"
#endif

namespace nautilus::compiler::nirbackend {

NirBackend::~NirBackend() = default;

NirBackendRegistry& NirBackendRegistry::instance() {
	static NirBackendRegistry registry;
	return registry;
}

NirBackendRegistry::NirBackendRegistry() {
#ifdef ENABLE_TBC_BACKEND
	registerBackend("tbc", std::make_unique<tbc::TBCNirBackend>());
#endif
}

void NirBackendRegistry::registerBackend(const std::string& name, std::unique_ptr<NirBackend> backend) {
	backends_[name] = std::move(backend);
}

const NirBackend* NirBackendRegistry::find(const std::string& name) const {
	auto it = backends_.find(name);
	return it == backends_.end() ? nullptr : it->second.get();
}

void* resolvePointer(const nir::Module& module, uint32_t pointerIndex, const HostEnvironment& host) {
	const auto& record = module.pointers.at(pointerIndex);
	switch (static_cast<nir::PointerMode>(record.mode)) {
	case nir::PointerMode::ProcessLocal:
		return reinterpret_cast<void*>(record.address);
	case nir::PointerMode::PortableByName: {
		const auto name = module.name(record.nameId);
		void* address = host.resolveSymbol ? host.resolveSymbol(name) : nullptr;
		if (address == nullptr) {
			throw RuntimeException("the symbol \"" + std::string(name) + "\" of pointer #p" +
			                       std::to_string(pointerIndex) + " cannot be resolved");
		}
		return address;
	}
	case nir::PointerMode::PortableBuildId:
		throw NotImplementedException("pointers by build id are resolved from M5.1 on");
	}
	throw RuntimeException("a pointer of an unknown mode");
}

} // namespace nautilus::compiler::nirbackend

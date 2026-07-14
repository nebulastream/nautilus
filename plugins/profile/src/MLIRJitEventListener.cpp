// Custom llvm::JITEventListener that feeds the profile plugin's JIT symbol
// table with authoritative (name, address, size) triples taken directly
// from every JIT-emitted object file. Replaces the earlier
// /proc/self/maps size-inference heuristic.
//
// This TU is deliberately compiled with -fno-rtti to match how LLVM itself
// is built. The subclass's vtable would otherwise reference typeinfo
// symbols that LLVM's static libs never export (same reason the existing
// MLIRJitEventListenerTestFixture uses -fno-rtti).

#include "nautilus/profile/JitSymbols.hpp"
#include "nautilus/profile/MLIR.hpp"
#include <cstdint>
#include <cstdlib>
#include <cxxabi.h>
#include <llvm/ExecutionEngine/JITEventListener.h>
#include <llvm/ExecutionEngine/RuntimeDyld.h>
#include <llvm/Object/ObjectFile.h>
#include <llvm/Object/SymbolSize.h>
#include <string>

namespace nautilus::profile {

namespace {

// Best-effort Itanium demangle. Returns the input unchanged when the name
// isn't a C++ mangled symbol (e.g. "execute" or "__nautilus_profile_begin").
std::string demangle(llvm::StringRef mangled) {
	if (mangled.empty()) {
		return "<unnamed>";
	}
	std::string in = mangled.str();
	int status = 0;
	char* buf = abi::__cxa_demangle(in.c_str(), nullptr, nullptr, &status);
	if (status == 0 && buf != nullptr) {
		std::string out(buf);
		std::free(buf);
		return out;
	}
	return in;
}

class ProfileListener : public llvm::JITEventListener {
public:
	void notifyObjectLoaded(ObjectKey, const llvm::object::ObjectFile& obj,
	                        const llvm::RuntimeDyld::LoadedObjectInfo& loadedInfo) override {
		// computeSymbolSizes pairs each defined symbol with its actual code
		// size (ELF st_size, gap-computed on Mach-O/COFF). SymbolRef::getAddress
		// returns a pre-relocation section-relative value on ELF; we map it to
		// the runtime address by adding the section's load delta reported by
		// the object-linking layer.
		for (const auto& [sym, size] : llvm::object::computeSymbolSizes(obj)) {
			if (size == 0) {
				continue;
			}
			auto typeOrErr = sym.getType();
			if (!typeOrErr) {
				llvm::consumeError(typeOrErr.takeError());
				continue;
			}
			if (*typeOrErr != llvm::object::SymbolRef::ST_Function) {
				continue;
			}
			auto nameOrErr = sym.getName();
			auto symValueOrErr = sym.getAddress();
			auto sectOrErr = sym.getSection();
			if (!nameOrErr) {
				llvm::consumeError(nameOrErr.takeError());
				continue;
			}
			if (!symValueOrErr) {
				llvm::consumeError(symValueOrErr.takeError());
				continue;
			}
			if (!sectOrErr) {
				llvm::consumeError(sectOrErr.takeError());
				continue;
			}
			const llvm::object::SectionRef& sect = **sectOrErr;
			const uint64_t sectFileAddr = sect.getAddress();
			const uint64_t sectLoadAddr = loadedInfo.getSectionLoadAddress(sect);
			if (sectLoadAddr == 0) {
				continue; // section wasn't mapped (e.g. debug info)
			}
			const uint64_t offsetInSection = *symValueOrErr - sectFileAddr;
			const uint64_t runtimeAddr = sectLoadAddr + offsetInSection;

			JitSymbol out {};
			out.addr = static_cast<uintptr_t>(runtimeAddr);
			out.size = static_cast<size_t>(size);
			out.name = demangle(*nameOrErr);
			out.module = "mlir";
			registerJitCode(std::move(out));
		}
	}

	void notifyFreeingObject(ObjectKey) override {
		// No-op. We let stale symbols live: JIT memory is typically released
		// only at engine shutdown, and retaining stale entries through to
		// trace flush is cheaper than tracking per-object symbol sets.
	}
};

} // namespace

llvm::JITEventListener* mlirJitEventListener() {
	// Installing the listener is a strong signal that the user wants the
	// profile plugin engaged for MLIR compilations, so also pull in the
	// intrinsic lowering that inlines llvm.readcyclecounter(). Both sides
	// are idempotent.
	registerMLIRIntrinsics();
	static ProfileListener instance;
	return &instance;
}

} // namespace nautilus::profile

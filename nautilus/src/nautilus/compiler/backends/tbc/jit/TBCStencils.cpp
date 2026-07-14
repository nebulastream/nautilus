
#include "nautilus/compiler/backends/tbc/jit/TBCStencils.hpp"
#include "nautilus/compiler/backends/tbc/TBCCode.hpp"
#include <cstring>
#include <mutex>

namespace nautilus::compiler::tbc::jit {

// Select the generated stencil table matching this build's target. The .inc
// files are checked in and regenerated on demand via the tbc-regen-stencils
// CMake target (tools/stencils/build_stencils.py).
namespace gen {
#if defined(__x86_64__) && defined(__ELF__)
#include "generated/stencils_x86_64_elf.inc"
#define TBC_JIT_HAVE_STENCILS 1
#elif defined(__aarch64__) && defined(__ELF__)
#include "generated/stencils_aarch64_elf.inc"
#define TBC_JIT_HAVE_STENCILS 1
#endif
} // namespace gen

const StencilTable* stencilTable() {
#ifdef TBC_JIT_HAVE_STENCILS
	return &gen::kTable;
#else
	return nullptr;
#endif
}

namespace {

/// Resolve the name-keyed generated entries against the current Op enum.
/// Name-based resolution makes a stale generated table degrade safely: an
/// opcode without a stencil disables the JIT for programs containing it
/// (module-level interpreter fallback), and unknown names are ignored.
ResolvedStencils resolve() {
	ResolvedStencils resolved {};
	const StencilTable* table = stencilTable();
	if (table == nullptr) {
		return resolved;
	}

	const auto find = [&](const char* name) -> Stencil {
		for (uint32_t i = 0; i < table->entryCount; ++i) {
			if (std::strcmp(table->entries[i].name, name) == 0) {
				return table->entries[i].stencil;
			}
		}
		return {};
	};

	for (uint16_t op = 0; op < kOpCount; ++op) {
		resolved.byOp[op] = find(opName(static_cast<Op>(op)));
	}
	resolved.retValue = find(kSynRetValue);
	resolved.retVoid = find(kSynRetVoid);
	resolved.epilogue = find(kSynEpilogue);
	resolved.unwind = find(kSynUnwind);
	resolved.data = table->data;
	resolved.dataSize = table->dataSize;
	resolved.valid = resolved.retValue.code != nullptr && resolved.retVoid.code != nullptr &&
	                 resolved.epilogue.code != nullptr && resolved.unwind.code != nullptr;
	return resolved;
}

} // namespace

const ResolvedStencils& resolvedStencils() {
	static const ResolvedStencils resolved = resolve();
	return resolved;
}

} // namespace nautilus::compiler::tbc::jit

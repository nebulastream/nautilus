#pragma once

#include "nautilus/compiler/backends/tbc/TBCOpcodes.hpp"
#include "nautilus/compiler/backends/tbc/jit/TBCStencilRuntime.hpp"

namespace nautilus::compiler::tbc::jit {

/// Generated stencils resolved against the current Op enum (by opcode name).
/// A default-constructed Stencil (code == nullptr) means "no stencil for this
/// opcode" and forces the interpreter fallback for programs that use it.
struct ResolvedStencils {
	Stencil byOp[kOpCount] = {};
	Stencil retValue;
	Stencil retVoid;
	Stencil epilogue;
	Stencil unwind;
	const unsigned char* data = nullptr;
	uint32_t dataSize = 0;
	/// False when no stencil table exists for this target (JIT unavailable).
	bool valid = false;
};

const ResolvedStencils& resolvedStencils();

} // namespace nautilus::compiler::tbc::jit

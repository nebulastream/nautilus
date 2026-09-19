#pragma once

#include "nautilus/compiler/backends/tbc/TBCInstr.hpp"
#include "nautilus/compiler/backends/tbc/TBCOpcodes.hpp"
#include "nautilus/tracing/Types.hpp"
#include <cstdint>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace nautilus::compiler {
class NativeClosure;
}

namespace nautilus::compiler::tbc {

/// Per-call-site record. The instruction stream only carries the record index;
/// argument registers, types, and the target live here. Used by CALL (internal,
/// interpreter-native), CALL_EXT (external via dyncall), and CALL_IND (register
/// target, internal or external).
struct CallSite {
	void* target = nullptr;       // external function pointer (CALL_EXT)
	uint32_t internalFnIdx = ~0u; // callee index (CALL)
	Type returnType = Type::v;
	std::vector<Type> argTypes;    // callee signature, for dyncall marshaling
	std::vector<uint16_t> argRegs; // caller registers holding the arguments
};

/// One lowered function: a flat instruction stream plus its frame metadata.
/// Frame layout (in 8-byte slots, pushed on the contiguous VM stack):
///   base[0..2]                     header: caller fp, return ip, caller dst reg
///   fp[0 .. regSlots)              value + constant registers (fp = base + 3)
///   fp[regSlots .. frameSlots-3)   alloca area (16-aligned at runtime)
struct TBCFunction {
	std::string name;
	std::vector<Instr> code;
	/// Initial register contents, memcpy'd into the frame on every call.
	/// Constant slots hold their values; everything else is zero.
	std::vector<uint64_t> initImage;
	uint32_t regSlots = 0;
	uint32_t frameSlots = 0; // 3 header + regSlots + alloca slots (incl. alignment slack)
	uint32_t allocaBytes = 0;
	std::vector<std::pair<uint16_t, uint32_t>> allocaRegs; // register -> byte offset in the alloca area
	std::vector<uint16_t> argRegs;
	std::vector<Type> argTypes;
	Type returnType = Type::v;

	std::string toString() const;
};

enum class DispatchMode : uint8_t { Tailcall, Goto, Switch };

/// Stitched copy-and-patch code for a whole program (tbc.mode=jit). The
/// concrete subclass (jit/TBCStitcher.cpp) owns the executable span; this
/// base keeps TBCCode free of any JIT/asmjit dependency so the interpreter
/// builds identically with ENABLE_TBC_JIT off.
struct TBCJitCode {
	std::vector<void*> entries; // per-function stitched entry point
	void* epilogue = nullptr;   // entry-frame landing pad (@EPILOGUE stencil)
	size_t codeBytes = 0;       // stitched span size (statistics)
	virtual ~TBCJitCode() = default;
};

/// Per-function binding handed to that function's NativeClosure as userdata:
/// everything the closure body needs to enter the VM.
struct TBCClosureBinding {
	const struct TBCProgram* program = nullptr;
	uint32_t functionIndex = 0;
};

/// A whole compiled module: all functions plus the program-wide call-site
/// table. Heap-allocated once and never moved afterwards: every function's
/// native closure binds to the address of its entry in `closureBindings`.
struct TBCProgram {
	// Constructor and destructor are both out of line so this header can
	// forward-declare NativeClosure: constructing or destroying the closure
	// vector needs the complete type, and only TBCCode.cpp has it.
	TBCProgram();
	TBCProgram(const TBCProgram&) = delete;
	TBCProgram& operator=(const TBCProgram&) = delete;
	~TBCProgram();

	std::vector<TBCFunction> functions;
	/// Name -> function slot. Used by TBCExecutable to resolve a function the
	/// *user* asks for by name; not a lowering-time predicate.
	std::unordered_map<std::string, uint32_t> functionIndex;
	/// FunctionId -> function slot. This is what lowering resolves calls
	/// through, so an in-module call is recognised by identity rather than by
	/// a name lookup that could miss (and dispatch through a definition
	/// object as if it were code).
	std::unordered_map<uint32_t, uint32_t> functionSlotById;
	std::vector<CallSite> callsites;
	uint64_t minStackSlots = 0;
	DispatchMode dispatch = DispatchMode::Switch;
	/// Set when the program was stitched by the copy-and-patch JIT; execution
	/// then enters the stitched code instead of the dispatch loop. Stitched
	/// code patches `&functions[i]` / `&callsites[i]` into instructions, so
	/// neither vector may be resized once this is non-null (stitching is the
	/// last step of TBCBackend::compile).
	std::unique_ptr<TBCJitCode> jit;

	/// One native entry point per function, index-parallel with `functions`
	/// (see TBCClosure.hpp). `closureBindings` is sized once and never resized:
	/// each closure holds the address of its binding. Declared after `jit` so
	/// the closures are destroyed first -- a closure is callable until it is
	/// freed, so nothing may outlive the code it would enter.
	std::vector<TBCClosureBinding> closureBindings;
	std::vector<std::unique_ptr<compiler::NativeClosure>> closures;
};

} // namespace nautilus::compiler::tbc

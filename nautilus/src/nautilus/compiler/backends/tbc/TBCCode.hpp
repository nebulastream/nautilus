#pragma once

#include "nautilus/compiler/backends/tbc/TBCInstr.hpp"
#include "nautilus/compiler/backends/tbc/TBCOpcodes.hpp"
#include "nautilus/tracing/Types.hpp"
#include <cstdint>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

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

/// A whole compiled module: all functions plus the program-wide call-site
/// table. Heap-allocated once and never moved afterwards: escaping internal
/// function pointers (see TBCTrampoline.hpp) bind trampoline slots to this
/// object's address, released again by the destructor.
struct TBCProgram {
	TBCProgram() = default;
	TBCProgram(const TBCProgram&) = delete;
	TBCProgram& operator=(const TBCProgram&) = delete;
	~TBCProgram();

	std::vector<TBCFunction> functions;
	std::unordered_map<std::string, uint32_t> functionIndex;
	std::vector<CallSite> callsites;
	uint64_t minStackSlots = 0;
	DispatchMode dispatch = DispatchMode::Switch;
	/// Set when the program was stitched by the copy-and-patch JIT; execution
	/// then enters the stitched code instead of the dispatch loop. Stitched
	/// code patches `&functions[i]` / `&callsites[i]` into instructions, so
	/// neither vector may be resized once this is non-null (stitching is the
	/// last step of TBCBackend::compile).
	std::unique_ptr<TBCJitCode> jit;
};

} // namespace nautilus::compiler::tbc

#pragma once

#include <cstdint>

// Contract between the stencil extractor (tools/stencils/build_stencils.py),
// the generated stencil tables (jit/generated/stencils_<arch>_<fmt>.inc), and
// the runtime stitcher (jit/TBCStitcher.cpp).
//
// A stencil is the machine code of one opcode handler, compiled offline with
// holes: relocations against magic extern symbols (_JIT_A, _JIT_CONTINUE, …)
// that the stitcher patches per instruction. See jit/TBCStencilSource.cpp for
// the stencil source and tools/stencils/README.md for regeneration.

namespace nautilus::compiler::tbc::jit {

/// What value a hole is patched with. Positional operand holes (A..D) get
/// their meaning per opcode from the stitcher (register byte-offset,
/// raw field, or sign-extended immediate); the symbol only names the field.
enum class HoleSym : uint8_t {
	A,              // instruction field a (word1.a for the 2-word SELECT's D)
	B,              // instruction field b
	C,              // instruction field c
	D,              // second-word field a (SELECT false-register)
	Target,         // stitched address of the branch-taken instruction
	Continue,       // stitched address of the next instruction
	CallTarget,     // stitched entry of the callee function (CALL)
	Site,           // &program->callsites[siteIdx]
	Func,           // &program->functions[fnIdx]
	Unwind,         // program's UNWIND stencil address
	HelperPushFrame, // address of tbcJitPushFrame
	HelperExtCall,   // address of tbcJitExtCall
	HelperIndCall,   // address of tbcJitIndCall
	Data,           // base of the program's copy of the stencil rodata image
};

/// How the patch value is encoded at the hole offset.
enum class HoleKind : uint8_t {
	Abs64,         // little-endian 64-bit absolute (x86-64 movabs imm / .quad)
	A64MovwG0,     // aarch64 movz/movk :abs_g0_nc: bits [15:0]
	A64MovwG1,     // aarch64 movk :abs_g1_nc:      bits [31:16]
	A64MovwG2,     // aarch64 movk :abs_g2_nc:      bits [47:32]
	A64MovwG3,     // aarch64 movk :abs_g3:         bits [63:48]
	A64Jump26,     // aarch64 b:  imm26 pc-relative (tail branch; intra-span)
	A64Call26,     // aarch64 bl: imm26 pc-relative (helper call; via range thunk)
	A64AdrpPage21, // aarch64 adrp: page delta to the patch value
	A64AddLo12,    // aarch64 add: low 12 bits of the patch value
	A64Ldst64Lo12, // aarch64 64-bit load/store: low 12 bits, scaled by 8
	A64Ldst32Lo12, // aarch64 32-bit load/store: low 12 bits, scaled by 4
	A64Ldst128Lo12, // aarch64 128-bit load/store: low 12 bits, scaled by 16
	A64GotLoadPage21,  // Mach-O arm64: adrp of an emulated-GOT slot
	A64GotLoadPageOff12, // Mach-O arm64: ldr page-offset of an emulated-GOT slot
};

struct StencilHole {
	uint32_t offset; // byte offset of the patch site within the stencil code
	HoleKind kind;
	HoleSym sym;
	int64_t addend; // relocation addend (data-image offset for HoleSym::Data)
};

/// Sentinel for Stencil::continueJmpOffset: no elidable trailing jump.
inline constexpr uint32_t kNoContinueJmp = ~uint32_t {0};

struct Stencil {
	const unsigned char* code = nullptr;
	uint32_t size = 0;
	const StencilHole* holes = nullptr;
	uint16_t holeCount = 0;
	/// Byte offset of a trailing "jump to _JIT_CONTINUE" sequence that the
	/// stitcher may drop entirely when the next instruction is stitched
	/// immediately after this one (fall-through elision), or kNoContinueJmp.
	uint32_t continueJmpOffset = kNoContinueJmp;
};

/// One generated stencil, keyed by the handler's name (the `stencil_` symbol
/// prefix stripped). Regular opcode stencils use the opcode name (opName());
/// synthetic stencils use "@"-prefixed names (see kSyn* below). The runtime
/// resolves names against the current Op enum, so a stale generated table
/// degrades (missing stencil => interpreter fallback), never mis-executes.
struct StencilEntry {
	const char* name;
	Stencil stencil;
};

/// Synthetic stencil names (not tied to a single opcode).
inline constexpr const char* kSynRetValue = "@RET_VALUE"; // RET with a != kNoReg
inline constexpr const char* kSynRetVoid = "@RET_VOID";   // RET with a == kNoReg
inline constexpr const char* kSynEpilogue = "@EPILOGUE";  // entry frame landing pad
inline constexpr const char* kSynUnwind = "@UNWIND";      // pending-exception exit

/// Calling convention of the generated stencils. The entry shim must call the
/// first stitched instruction with the matching convention.
enum class StencilAbi : uint8_t {
	PreserveNone = 1, // uint64_t(preserve_none*)(uint64_t* fp, VMContext* ctx)
};

struct StencilTable {
	const StencilEntry* entries = nullptr;
	uint32_t entryCount = 0;
	const unsigned char* data = nullptr; // merged rodata image
	uint32_t dataSize = 0;
	StencilAbi abi = StencilAbi::PreserveNone;
};

/// The stencil table generated for this build's target, or nullptr when no
/// generated table matches {architecture, object format} (JIT unavailable).
const StencilTable* stencilTable();

} // namespace nautilus::compiler::tbc::jit

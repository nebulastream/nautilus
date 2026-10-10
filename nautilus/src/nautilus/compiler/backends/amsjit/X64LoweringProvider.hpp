
#pragma once

#include "nautilus/compiler/Frame.hpp"
#include "nautilus/compiler/backends/CapturedExceptionTransport.hpp"
#include "nautilus/compiler/backends/amsjit/AsmJitRegister.hpp"
#include "nautilus/compiler/backends/amsjit/X64PostRAPeepholePass.hpp"
#include "nautilus/compiler/backends/amsjit/intrinsics/AsmJitBackendIntrinsic.hpp"
#include "nautilus/compiler/ir/IRGraph.hpp"
#include "nautilus/compiler/ir/OperationDispatcher.hpp"
#include "nautilus/compiler/ir/blocks/BasicBlock.hpp"
#include "nautilus/compiler/ir/blocks/BasicBlockInvocation.hpp"
#include "nautilus/options.hpp"
#include <asmjit/x86.h>
#include <optional>
#include <span>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace nautilus::compiler {
class CompilationStatistics;
class DumpHandler;
} // namespace nautilus::compiler

namespace nautilus::compiler::asmjit {

/**
 * @brief Translates Nautilus IR to x86-64 machine code using AsmJit.
 *
 * All functions in the IR are compiled into a single CodeHolder using AsmJit's
 * two-pass pattern: first all FuncNodes are registered (establishing stable Labels),
 * then each function body is emitted. This allows forward and mutual references
 * between functions without any ordering constraints or external thunks.
 */
class AsmJitLoweringProvider {
public:
	AsmJitLoweringProvider() = default;

	struct LowerResult {
		void* basePtr = nullptr;                        ///< Single JIT allocation base; release exactly once.
		std::unordered_map<std::string, void*> jitPtrs; ///< Per-function pointers within the allocation.
		uint64_t codeSize = 0;                          ///< Total emitted machine-code size in bytes.
		std::string asmjitIR;                           ///< AsmJit builder node list (only captured when dumping).
		std::string assembly;                           ///< Generated assembly listing (only captured when dumping).
	};

	/// Compile all functions in the IR graph into one JIT allocation.
	///
	/// When @p statistics is non-null, the optional post-RA peephole pass
	/// (see @ref X64PostRAPeepholePass) records its per-run counters into
	/// it under the `asmjit.peephole.*` key namespace.
	///
	/// When @p dumpHandler requests the `after_asmjit_generation` /
	/// `after_asmjit_assembly` dumps, the corresponding textual representations
	/// are captured into @ref LowerResult.
	LowerResult lower(std::shared_ptr<ir::IRGraph> ir, ::asmjit::JitRuntime& runtime, const engine::Options& options,
	                  const DumpHandler& dumpHandler, CompilationStatistics* statistics = nullptr);

private:
	// AsmReg / RegisterFrame come from AsmJitRegister.hpp at namespace scope so
	// the intrinsic-plugin framework can name them too. The using-declarations
	// here keep AsmJitLoweringProvider::AsmReg available to legacy call sites.
	using AsmReg = nautilus::compiler::asmjit::AsmReg;
	using RegisterFrame = nautilus::compiler::asmjit::RegisterFrame;

	class LoweringContext : public ir::OperationDispatcher<LoweringContext> {
	public:
		LoweringContext(std::shared_ptr<ir::IRGraph> ir, ::asmjit::CodeHolder& code, const engine::Options& options,
		                CompilationStatistics* statistics, const AsmJitIntrinsicManager& intrinsicManager);

		/// Pass 1 + Pass 2 + finalize.
		///
		/// When @p asmjitIRDump is non-null, the AsmJit builder node list is formatted into it
		/// just before finalize() (i.e. while virtual registers are still present).
		void processAll(std::string* asmjitIRDump = nullptr);

		/// Must be called after processAll() and before rt.add() to capture label offsets.
		const std::unordered_map<std::string, ::asmjit::FuncNode*>& getFuncNodes() const {
			return funcNodesByName_;
		}

	private:
		// Allow the CRTP dispatcher to call our private visitXxx hooks.
		friend class ir::OperationDispatcher<LoweringContext>;

		::asmjit::x86::Compiler cc;
		std::shared_ptr<ir::IRGraph> ir;
		/// Maps function-pointer keys → intrinsic handler. Non-owning; the
		/// manager is owned by AsmJitLoweringProvider::lower() and lives for
		/// the duration of the LoweringContext.
		const AsmJitIntrinsicManager& intrinsicManager_;
		/// Maps Nautilus function name → AsmJit FuncNode (stable label for forward calls).
		/// FunctionId -> the AsmJit node emitting that in-module function.
		/// Keyed on the callee's table id, not its name: a name lookup that
		/// missed used to fall through to cc.invoke() on whatever the stored
		/// pointer happened to be -- a definition object, for a Nautilus call.
		std::unordered_map<ir::FunctionId, ::asmjit::FuncNode*> funcNodes_;
		/// Name -> node, kept only so the caller can export compiled
		/// addresses under the names a user asked for. Lowering never reads
		/// it; resolution goes through funcNodes_ above.
		std::unordered_map<std::string, ::asmjit::FuncNode*> funcNodesByName_;
		std::unordered_map<ir::BlockIdentifier, ::asmjit::Label> blockLabels;
		std::unordered_set<ir::BlockIdentifier> processedBlocks;
		/// A block-argument source: either a bound register or a deferred
		/// constant's canonical 64-bit pattern.
		struct BlockArgSource {
			std::optional<AsmReg> reg;
			int64_t imm = 0;
		};
		/// Scratch buffers of processBlockInvocation, reused across edges so a
		/// function with many block arguments per edge does not allocate per
		/// edge (issue #508).
		std::vector<BlockArgSource> blockArgSources_;
		std::vector<AsmReg> blockArgDsts_;
		std::vector<std::optional<AsmReg>> blockArgTemps_;
		/// Per virtual register index: the epoch of the last
		/// processBlockInvocation call that used it as a destination register.
		std::vector<uint32_t> dstMarks_;
		uint32_t dstMarkEpoch_ = 0;
		/// Static SSA usage counts for the current function (see ir::countUsages).
		/// Only populated when branch fusion is enabled; used to prove that a
		/// compare's sole consumer is the IfOperation that follows it.
		std::unordered_map<ir::OperationIdentifier, uint32_t> usageCounts_;
		/// Set by processBlock when the next dispatched operation is an
		/// IfOperation that can consume this compare's EFLAGS directly. The
		/// compare's own lowering (cmp+setcc+movzx) is skipped and visitIf
		/// emits a fused cmp+jcc instead.
		const ir::CompareOperation* pendingFusedCompare_ = nullptr;
		/// Gates the compare→branch fusion (option `asmjit.enableBranchFusion`).
		bool enableBranchFusion_ = true;
		/// Gates constant deferral + immediate folding (option
		/// `asmjit.enableConstFolding`). When on, integer/bool/ptr constants
		/// are not materialised at their definition; consumers either fold
		/// them as immediate operands or rematerialise them per use.
		bool enableConstFolding_ = true;
		/// Gates the branch-free select lowering via cmov (option
		/// `asmjit.enableSelectCmov`).
		bool enableSelectCmov_ = true;
		/// Gates lazy narrowing (option `asmjit.enableLazyNarrowing`). Off, every
		/// narrow integer result is re-extended to the canonical 64-bit pattern
		/// right after it is computed (see narrowToStamp). On, the results of
		/// operations whose low bits do not depend on the upper input bits
		/// (add/sub/mul/not/shl/and/or/xor/select) and narrow block parameters
		/// may hold garbage above their stamp's width; consumers that read the
		/// full register (div/mod, right shifts, int->float casts, 64-bit
		/// arithmetic, mixed-width compares, calls) extend them on demand. This
		/// keeps the re-extension out of loop-carried dependency chains.
		bool enableLazyNarrowing_ = true;
		/// Indexed by operation id (current function only): non-zero when the
		/// value's register may hold garbage above its narrow stamp's width.
		/// Conservative per identifier: an id that doubles as a block parameter
		/// (issue #321) is dirty if any of its definitions may be.
		std::vector<uint8_t> mayBeDirty_;
		/// Gates address-mode fusion (option `asmjit.enableAddressFusion`): a
		/// pointer/64-bit add of the form `base + (x * 2^k) + disp` is folded
		/// into the memory operands of the loads/stores that use it (or into
		/// one `lea` when it is materialised), and the scaling multiply or
		/// shift is folded into the address instead of being computed.
		bool enableAddressFusion_ = true;
		/// Indexed by operation id (current function only). Static use facts:
		/// bit 0 -- every use is a load/store address in the defining block;
		/// bit 1 -- every use is the scaled index of an address add (see
		/// matchAddress) in the defining block.
		std::vector<uint8_t> fusionUses_;
		/// Indexed by operation id: non-zero when lowering skipped the value's
		/// materialisation because every consumer folds it into an address.
		std::vector<uint8_t> deferredAddressParts_;
		int64_t fusedAddresses_ = 0;
		/// Gates inlining of small internal callees (option
		/// `asmjit.enableInlining`; `asmjit.inliningMaxOperations` bounds the
		/// callee size). See tryInlineCall.
		bool enableInlining_ = true;
		/// Gates loop rotation (option `asmjit.enableLoopRotation`); needs
		/// branch fusion. See tryRotateLoopBranch.
		bool enableLoopRotation_ = true;
		int64_t rotatedLoops_ = 0;
		size_t inliningMaxOperations_ = 24;
		int64_t inlinedCalls_ = 0;
		/// Statistics sink shared with the rest of the pipeline; may be null.
		CompilationStatistics* statistics_ = nullptr;
		int64_t fusedBranches_ = 0;
		int64_t foldedImmediates_ = 0;
		/// Pointer registers for the current function's alloca slots, indexed
		/// by AllocaOperation::getIndex(). Materialised once in the function
		/// prologue from FunctionOperation::getAllocaSpecs(); cleared per
		/// function to keep stale entries from leaking across functions.
		std::vector<AsmReg> functionAllocaSlots_;

		/// The function currently being lowered, or nullptr when lowering
		/// outside a function body (pads are lowered within the active body).
		const ir::FunctionOperation* currentFunction_ = nullptr;
		/// Captured-exception queries for `currentFunction_`, built once per
		/// function rather than once per call site.
		CapturedExceptionTransport transport_;
		/// Lazily-created AsmJit labels for the exception-region landing pads
		/// of the current function, keyed by the pad's address. Cleared per
		/// function.
		std::unordered_map<size_t, ::asmjit::Label> padLabels_;
		/// Shared exceptional-exit label for the current function, created on
		/// first use. All pads jump here after running destructors; it returns
		/// the ABI default for the function's return type.
		::asmjit::Label exceptionalExitLabel_;

		static ::asmjit::TypeId getTypeId(Type t);
		static bool isFloatType(Type t);
		static bool isUnsignedType(Type t);

		// All integer types are mapped to 64-bit GP; floats to XMM.
		AsmReg allocReg(Type t);
		static ::asmjit::x86::Gp toGp(const AsmReg& r);
		static ::asmjit::x86::Xmm toXmm(const AsmReg& r);

		// Re-sign/zero-extend a GP register's low `stamp`-width bits across the
		// full 64-bit register per `stamp`'s signedness. Integer arithmetic on
		// sub-64-bit types is computed at full 64-bit width (see the class
		// comment), which can leave the upper bits inconsistent with the
		// narrow-width invariant (e.g. an i32 add that overflows 32 bits still
		// produces a "correct" 64-bit sum, whose 64-bit sign-extension no
		// longer matches the sign-extension of the wrapped-around 32-bit
		// result). Every op whose narrow-width result can differ from its
		// 64-bit computation -- Add/Sub/Mul/Negate(bitwise not)/Shift -- must
		// restore the invariant afterward so later consumers (comparisons,
		// casts, ...) that read the full register see the right value.
		void narrowToStamp(::asmjit::x86::Gp reg, Type stamp);

		// ── Lazy narrowing (see enableLazyNarrowing_) ──────────────────────
		/// Width in bits of the low part of a register that carries @p t's value
		/// (64 for i64/ui64/ptr/bool and anything non-integer).
		static uint32_t stampBits(Type t);
		/// Fills mayBeDirty_ for @p funcOp (cleared when lazy narrowing is off).
		void computeDirtyValues(const ir::FunctionOperation* funcOp);
		/// True when @p in's register may hold garbage above its stamp's width.
		bool isDirty(const ir::Operation* in) const;
		/// dst = src's low `stamp`-width bits sign/zero-extended per `stamp`.
		void extendFromStamp(::asmjit::x86::Gp dst, ::asmjit::x86::Gp src, Type stamp);
		/// Like gpOperand, but guarantees the low @p width bits of the returned
		/// register are exact: a dirty operand narrower than @p width is
		/// extended into a fresh register first.
		::asmjit::x86::Gp gpOperandAtWidth(const ir::Operation* in, uint32_t width, RegisterFrame& frame);
		/// gpOperandAtWidth(in, 64): the canonical 64-bit pattern.
		::asmjit::x86::Gp cleanGpOperand(const ir::Operation* in, RegisterFrame& frame) {
			return gpOperandAtWidth(in, 64, frame);
		}
		/// narrowToStamp(reg, stamp), skipped under lazy narrowing (the value is
		/// then recorded as dirty by computeDirtyValues instead).
		void narrowResult(::asmjit::x86::Gp reg, Type stamp);

		// ── Address-mode fusion (see enableAddressFusion_) ─────────────────
		/// `base + offset + disp`, where offset is either a plain 64-bit value or
		/// `scaledInput << shift` computed by the operation `offset`.
		struct AddressParts {
			const ir::Operation* base = nullptr;
			const ir::Operation* offset = nullptr;      ///< null when the address is `base + disp`
			const ir::Operation* scaledInput = nullptr; ///< x when offset computes `x << shift`
			uint32_t shift = 0;
			int32_t disp = 0;
		};
		/// Structural match of an add as an x86 address; nullopt when @p op is
		/// not a ptr/i64/ui64 add that fits.
		std::optional<AddressParts> matchAddress(const ir::Operation* op);
		/// Fills fusionUses_ for @p funcOp (cleared when fusion is off).
		void computeFusionUses(const ir::FunctionOperation* funcOp);
		bool hasFusionUse(const ir::Operation* op, uint8_t bit) const;
		bool isDeferredAddressPart(const ir::Operation* op) const;
		void markDeferredAddressPart(const ir::Operation* op);
		/// True when @p op (an add matched by matchAddress, or the scaled index
		/// of one) need not be materialised: its consumers rebuild it from its
		/// parts. Marks the op deferred when so.
		bool deferAddressPart(const ir::Operation* op, uint8_t useBit, RegisterFrame& frame);
		/// The memory operand addressing `parts`.
		::asmjit::x86::Mem memFromParts(const AddressParts& parts, RegisterFrame& frame);
		/// The memory operand for a load/store address operand: the fused form
		/// for a deferred address add, `[reg]` otherwise.
		::asmjit::x86::Mem memOperand(const ir::Operation* addr, uint32_t size, RegisterFrame& frame);

		// ── Inlining ─────────────────────────────────────────────────────────
		/// True when @p callee can be lowered in place of a call to it: a single
		/// block of plain value operations (no control flow, calls or allocas)
		/// ending in a return.
		bool isInlinable(const ir::FunctionOperation* callee) const;
		/// Lowers the body of the internal callee of @p op in place of the
		/// call; false (nothing emitted) when the callee is not inlinable.
		bool tryInlineCall(ir::CallOperation* op, RegisterFrame& frame);

		::asmjit::Label getOrCreateLabel(ir::BlockIdentifier blockId);
		void emitMove(const AsmReg& dst, const AsmReg& src);

		// The canonical 64-bit register pattern of an integer-like constant
		// operation (sign-extended for signed stamps, zero-extended for
		// unsigned/bool/ptr) or of an integer cast of a constant chain, or
		// nullopt when @p in is not such a constant. The constant-folding IR
		// pass guarantees pointer-consistent input edges (issue #327), so the
		// definition is always recovered from the operation pointer itself.
		std::optional<int64_t> foldableConstValue(const ir::Operation* in);
		// Fetch @p in's GP register from the frame, or rematerialise a
		// deferred constant into a fresh register (per use — a shared lazy
		// binding would not dominate uses in sibling branches).
		::asmjit::x86::Gp gpOperand(const ir::Operation* in, RegisterFrame& frame);
		// Like gpOperand but preserves the GP/XMM distinction. Floats are
		// never deferred, so the rematerialisation path is GP-only.
		AsmReg regOperand(const ir::Operation* in, RegisterFrame& frame);
		// The constant's canonical pattern when @p in is a deferred constant
		// that fits a sign-extended imm32 operand; nullopt otherwise.
		std::optional<int32_t> imm32Operand(const ir::Operation* in);
		// Move @p src's value into @p dst: a register move when bound, a
		// direct `mov dst, imm` when @p src is a deferred constant.
		void emitMoveFromOperand(const AsmReg& dst, const ir::Operation* src, RegisterFrame& frame);

		// Bind an operation's freshly computed result to its SSA identifier.
		// For a normal (single) definition this just records the register.
		// But a value's identifier can coincide with a downstream merge
		// block's parameter whose register was already allocated by an
		// earlier-emitted predecessor edge -- Nautilus SSA reuses an incoming
		// value's name for the block parameter, and the diamond/loop CFG can
		// emit that predecessor before this definition. In that case the
		// identifier is already bound to the parameter register, and this
		// definition is the value flowing in along *this* edge, so its result
		// must be copied into the parameter register. Frame::setValue is
		// emplace-only and would silently ignore the rebind, orphaning the
		// computed value and leaving the merge parameter uninitialised along
		// this path (issue #321).
		void bindResult(const ir::OperationIdentifier& id, const AsmReg& reg, RegisterFrame& frame);

		void processBlock(const ir::BasicBlock* block, RegisterFrame& frame);
		void processBlockInvocation(const ir::BasicBlockInvocation& bi, RegisterFrame& frame);

		// True when `cmp` may skip materialising its boolean because the
		// immediately following IfOperation is its only consumer and can read
		// the flags directly.
		bool isFusibleCompare(const ir::CompareOperation* cmp, const ir::Operation* next, RegisterFrame& frame);
		// Emit the compare and the negated conditional jump to @p falseTarget
		// in place of the unfused cmp+setcc+movzx / test+jz sequence.
		/// With @p jumpIfTrue the jump is taken when the condition holds instead.
		void emitFusedCompareBranch(const ir::CompareOperation* cmp, ::asmjit::Label target, RegisterFrame& frame,
		                            bool jumpIfTrue = false);
		/// Loop rotation: when @p bi re-enters an already emitted block that only
		/// tests a fused compare (a loop header), emits that test with direct
		/// jumps to its successors in place of a jump back to the header. True
		/// when it did; the caller then emits no jump of its own.
		bool tryRotateLoopBranch(const ir::BasicBlockInvocation& bi, RegisterFrame& frame);
		/// True when lowering @p bi (block-argument copies) emits no code.
		bool isNoOpInvocation(const ir::BasicBlockInvocation& bi, RegisterFrame& frame);
		// The cmp/test of an integer (non-null-check) compare; see visitCompare.
		void emitIntegerCompare(const ir::CompareOperation* cmp, RegisterFrame& frame);

		// Per-operation hooks invoked by OperationDispatcher::dispatch.
		void visitConstBoolean(ir::ConstBooleanOperation* op, RegisterFrame& frame);
		void visitConstInt(ir::ConstIntOperation* op, RegisterFrame& frame);
		void visitConstFloat(ir::ConstFloatOperation* op, RegisterFrame& frame);
		void visitConstPtr(ir::ConstPtrOperation* op, RegisterFrame& frame);

		void visitAdd(ir::AddOperation* op, RegisterFrame& frame);
		void visitSub(ir::SubOperation* op, RegisterFrame& frame);
		void visitMul(ir::MulOperation* op, RegisterFrame& frame);
		void visitDiv(ir::DivOperation* op, RegisterFrame& frame);
		void visitMod(ir::ModOperation* op, RegisterFrame& frame);

		void visitCompare(ir::CompareOperation* op, RegisterFrame& frame);
		void visitAnd(ir::AndOperation* op, RegisterFrame& frame);
		void visitOr(ir::OrOperation* op, RegisterFrame& frame);
		void visitNot(ir::NotOperation* op, RegisterFrame& frame);
		void visitNegate(ir::NegateOperation* op, RegisterFrame& frame);
		void visitShift(ir::ShiftOperation* op, RegisterFrame& frame);
		void visitBinaryComp(ir::BinaryCompOperation* op, RegisterFrame& frame);

		void visitIf(ir::IfOperation* op, RegisterFrame& frame);
		void visitBranch(ir::BranchOperation* op, RegisterFrame& frame);
		void visitReturn(ir::ReturnOperation* op, RegisterFrame& frame);
		void visitSelect(ir::SelectOperation* op, RegisterFrame& frame);

		void visitLoad(ir::LoadOperation* op, RegisterFrame& frame);
		void visitStore(ir::StoreOperation* op, RegisterFrame& frame);
		void visitAlloca(ir::AllocaOperation* op, RegisterFrame& frame);
		void visitCall(ir::CallOperation* op, RegisterFrame& frame);
		void visitIndirectCall(ir::IndirectCallOperation* op, RegisterFrame& frame);
		void visitFunctionAddressOf(ir::FunctionAddressOfOperation* op, RegisterFrame& frame);
		void visitCast(ir::CastOperation* op, RegisterFrame& frame);

		// ── Captured-exception transport ───────────────────────────────────
		// When the current function has an exception region, potentially-
		// throwing calls are routed through the capture thunk and followed by
		// a pending-exception check plus a branch to the call's landing pad.

		/// Returns true when @p call needs captured exception transport (a
		/// pending check after the call plus a branch to its landing pad).
		[[nodiscard]] bool callNeedsCapture(const ir::Operation* call) const;
		/// Returns the landing pad for @p call, or nullptr when the call needs
		/// no pad (no destructors to run).
		[[nodiscard]] const ir::LandingPadBlock* getPadForCall(const ir::Operation* call) const;
		/// Resolves the capture thunk for @p call's signature. Throws
		/// NotImplementedException for unsupported signatures.
		[[nodiscard]] void* resolveCaptureThunk(const ir::Operation* call) const;
		/// Emits the pending-exception check for @p call: loads
		/// currentExceptionFrame(), reads its pending field, and jumps to the
		/// call's landing pad (or the exceptional-exit label) when set.
		void emitCheckPendingException(const ir::Operation* call);
		/// Returns the AsmJit label bound to @p pad (creating it on first use).
		[[nodiscard]] ::asmjit::Label getPadLabel(size_t padIndex);
		/// Returns the shared exceptional-exit label (creating it on first use).
		[[nodiscard]] ::asmjit::Label getExceptionalExitLabel();
		/// After the main CFG, emits the function's landing pads (each running
		/// its destructor calls then jumping to the exceptional exit) and the
		/// exceptional exit itself (returns the ABI default for the return type).
		void lowerExceptionPads(RegisterFrame& frame);
		/// Emits a return of the ABI default value for @p retType (0 / 0.0 /
		/// false / nullptr / nothing for void).
		void emitDefaultReturn(Type retType);
	};
};

} // namespace nautilus::compiler::asmjit

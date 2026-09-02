
#pragma once

#include "nautilus/compiler/Frame.hpp"
#include "nautilus/compiler/backends/CapturedExceptionTransport.hpp"
#include "nautilus/compiler/backends/amsjit/A64PostRAPeepholePass.hpp"
#include "nautilus/compiler/backends/amsjit/AsmJitRegister.hpp"
#include "nautilus/compiler/backends/amsjit/intrinsics/AsmJitBackendIntrinsic.hpp"
#include "nautilus/compiler/ir/IRGraph.hpp"
#include "nautilus/compiler/ir/OperationDispatcher.hpp"
#include "nautilus/compiler/ir/blocks/BasicBlock.hpp"
#include "nautilus/compiler/ir/blocks/BasicBlockInvocation.hpp"
#include "nautilus/options.hpp"
#include <asmjit/a64.h>
#include <unordered_map>
#include <unordered_set>

namespace nautilus::compiler {
class CompilationStatistics;
class DumpHandler;
} // namespace nautilus::compiler

namespace nautilus::compiler::asmjit {

/**
 * @brief Translates Nautilus IR to AArch64 machine code using AsmJit.
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
	/// (see @ref A64PostRAPeepholePass) records its per-run counters into
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

		::asmjit::a64::Compiler cc;
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
		/// Pointer registers for the current function's alloca slots, indexed
		/// by AllocaOperation::getIndex(). Materialised once in the function
		/// prologue from FunctionOperation::getAllocaSpecs(); cleared per
		/// function to keep stale entries from leaking across functions.
		std::vector<AsmReg> functionAllocaSlots_;
		/// Function being lowered, used by the captured-exception transport to
		/// find the call's landing pad. Cleared per function.
		const ir::FunctionOperation* currentFunction_ = nullptr;
		/// Captured-exception queries for `currentFunction_`, built once per
		/// function rather than once per call site.
		CapturedExceptionTransport transport_;
		/// AsmJit labels for the current function's exception landing pads.
		std::unordered_map<size_t, ::asmjit::Label> padLabels_;
		/// Shared exceptional-exit label for the current function (created on
		/// first use, reset per function).
		::asmjit::Label exceptionalExitLabel_;

		static ::asmjit::TypeId getTypeId(Type t);
		static bool isFloatType(Type t);
		static bool isUnsignedType(Type t);

		// Re-establish the register invariant for narrow integer stamps: every
		// value register holds its narrow value sign-extended (signed stamps) or
		// zero-extended (unsigned stamps) to the full 64 bits. Entry arguments
		// and casts establish it; any op whose 64-bit result can exceed the
		// stamp's width (add/sub/mul wraparound, left shift, bitwise not) must
		// call this afterwards, because downstream consumers (cmp, udiv/sdiv,
		// msub, ucvtf/scvtf) operate on the full X register and silently
		// mis-evaluate a non-canonical value. Mirrors X64LoweringProvider's
		// narrowToStamp.
		void narrowToStamp(::asmjit::a64::Gp reg, Type stamp);

		// All integer types are mapped to 64-bit GP (X); floats to Vec (S/D).
		AsmReg allocReg(Type t);
		static ::asmjit::a64::Gp toGp(const AsmReg& r);
		static ::asmjit::a64::Vec toVec(const AsmReg& r);

		::asmjit::Label getOrCreateLabel(ir::BlockIdentifier blockId);
		void emitMove(const AsmReg& dst, const AsmReg& src);

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
		// Mirrors the X64 backend's captured transport.

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

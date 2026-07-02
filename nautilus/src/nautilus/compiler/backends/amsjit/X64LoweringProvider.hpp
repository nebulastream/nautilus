
#pragma once

#include "nautilus/compiler/Frame.hpp"
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
#include <unordered_map>
#include <unordered_set>

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
			return funcNodes_;
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
		std::unordered_map<std::string, ::asmjit::FuncNode*> funcNodes_;
		std::unordered_map<ir::BlockIdentifier, ::asmjit::Label> blockLabels;
		std::unordered_set<ir::BlockIdentifier> processedBlocks;
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
		/// Statistics sink shared with the rest of the pipeline; may be null.
		CompilationStatistics* statistics_ = nullptr;
		int64_t fusedBranches_ = 0;
		int64_t foldedImmediates_ = 0;
		/// Pointer registers for the current function's alloca slots, indexed
		/// by AllocaOperation::getIndex(). Materialised once in the function
		/// prologue from FunctionOperation::getAllocaSpecs(); cleared per
		/// function to keep stale entries from leaking across functions.
		std::vector<AsmReg> functionAllocaSlots_;

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

		::asmjit::Label getOrCreateLabel(ir::BlockIdentifier blockId);
		void emitMove(const AsmReg& dst, const AsmReg& src);

		// The canonical 64-bit register pattern of an integer-like constant
		// operation (sign-extended for signed stamps, zero-extended for
		// unsigned/bool/ptr), or nullopt when @p in is not such a constant.
		static std::optional<int64_t> foldableConstValue(const ir::Operation* in);
		// Fetch @p in's GP register from the frame, or rematerialise a
		// deferred constant into a fresh register (per use — a shared lazy
		// binding would not dominate uses in sibling branches).
		::asmjit::x86::Gp gpOperand(const ir::Operation* in, RegisterFrame& frame);
		// Like gpOperand but preserves the GP/XMM distinction. Floats are
		// never deferred, so the rematerialisation path is GP-only.
		AsmReg regOperand(const ir::Operation* in, RegisterFrame& frame);
		// The constant's canonical pattern when @p in is a deferred constant
		// that fits a sign-extended imm32 operand; nullopt otherwise.
		std::optional<int32_t> imm32Operand(const ir::Operation* in, RegisterFrame& frame);
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
		void emitFusedCompareBranch(const ir::CompareOperation* cmp, ::asmjit::Label falseTarget, RegisterFrame& frame);

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
		void visitProxyCall(ir::ProxyCallOperation* op, RegisterFrame& frame);
		void visitIndirectCall(ir::IndirectCallOperation* op, RegisterFrame& frame);
		void visitFunctionAddressOf(ir::FunctionAddressOfOperation* op, RegisterFrame& frame);
		void visitCast(ir::CastOperation* op, RegisterFrame& frame);
	};
};

} // namespace nautilus::compiler::asmjit

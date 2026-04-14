
#pragma once

#include "nautilus/compiler/Frame.hpp"
#include "nautilus/compiler/ir/IRGraph.hpp"
#include "nautilus/compiler/ir/OperationDispatcher.hpp"
#include "nautilus/compiler/ir/blocks/BasicBlock.hpp"
#include "nautilus/compiler/ir/blocks/BasicBlockInvocation.hpp"
#include <asmjit/x86.h>
#include <unordered_map>
#include <unordered_set>
#include <variant>

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
	};

	/// Compile all functions in the IR graph into one JIT allocation.
	LowerResult lower(std::shared_ptr<ir::IRGraph> ir, ::asmjit::JitRuntime& runtime);

private:
	// Integer/pointer types → GP register; float types → XMM register.
	using AsmReg = std::variant<::asmjit::x86::Gp, ::asmjit::x86::Xmm>;
	using RegisterFrame = Frame<ir::OperationIdentifier, AsmReg>;

	class LoweringContext : public ir::OperationDispatcher<LoweringContext> {
	public:
		LoweringContext(std::shared_ptr<ir::IRGraph> ir, ::asmjit::CodeHolder& code);

		/// Pass 1 + Pass 2 + finalize.
		void processAll();

		/// Must be called after processAll() and before rt.add() to capture label offsets.
		const std::unordered_map<std::string, ::asmjit::FuncNode*>& getFuncNodes() const {
			return funcNodes_;
		}

	private:
		// Allow the CRTP dispatcher to call our private visitXxx hooks.
		friend class ir::OperationDispatcher<LoweringContext>;

		::asmjit::x86::Compiler cc;
		std::shared_ptr<ir::IRGraph> ir;
		/// Maps Nautilus function name → AsmJit FuncNode (stable label for forward calls).
		std::unordered_map<std::string, ::asmjit::FuncNode*> funcNodes_;
		std::unordered_map<ir::BlockIdentifier, ::asmjit::Label> blockLabels;
		std::unordered_set<ir::BlockIdentifier> processedBlocks;

		static ::asmjit::TypeId getTypeId(Type t);
		static bool isFloatType(Type t);
		static bool isUnsignedType(Type t);

		// All integer types are mapped to 64-bit GP; floats to XMM.
		AsmReg allocReg(Type t);
		static ::asmjit::x86::Gp toGp(const AsmReg& r);
		static ::asmjit::x86::Xmm toXmm(const AsmReg& r);

		::asmjit::Label getOrCreateLabel(ir::BlockIdentifier blockId);
		void emitMove(const AsmReg& dst, const AsmReg& src);

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
		void visitProxyCall(ir::ProxyCallOperation* op, RegisterFrame& frame);
		void visitIndirectCall(ir::IndirectCallOperation* op, RegisterFrame& frame);
		void visitFunctionAddressOf(ir::FunctionAddressOfOperation* op, RegisterFrame& frame);
		void visitCast(ir::CastOperation* op, RegisterFrame& frame);
	};
};

} // namespace nautilus::compiler::asmjit

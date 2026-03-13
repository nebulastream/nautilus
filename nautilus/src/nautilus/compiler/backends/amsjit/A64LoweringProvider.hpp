
#pragma once

#include "nautilus/compiler/Frame.hpp"
#include "nautilus/compiler/ir/IRGraph.hpp"
#include "nautilus/compiler/ir/blocks/BasicBlock.hpp"
#include "nautilus/compiler/ir/blocks/BasicBlockInvocation.hpp"
#include "nautilus/compiler/ir/operations/AllocaOperation.hpp"
#include "nautilus/compiler/ir/operations/ArithmeticOperations/AddOperation.hpp"
#include "nautilus/compiler/ir/operations/ArithmeticOperations/DivOperation.hpp"
#include "nautilus/compiler/ir/operations/ArithmeticOperations/ModOperation.hpp"
#include "nautilus/compiler/ir/operations/ArithmeticOperations/MulOperation.hpp"
#include "nautilus/compiler/ir/operations/ArithmeticOperations/SubOperation.hpp"
#include "nautilus/compiler/ir/operations/BinaryOperations/BinaryCompOperation.hpp"
#include "nautilus/compiler/ir/operations/BinaryOperations/NegateOperation.hpp"
#include "nautilus/compiler/ir/operations/BinaryOperations/ShiftOperation.hpp"
#include "nautilus/compiler/ir/operations/BranchOperation.hpp"
#include "nautilus/compiler/ir/operations/CastOperation.hpp"
#include "nautilus/compiler/ir/operations/ConstBooleanOperation.hpp"
#include "nautilus/compiler/ir/operations/ConstFloatOperation.hpp"
#include "nautilus/compiler/ir/operations/ConstIntOperation.hpp"
#include "nautilus/compiler/ir/operations/ConstPtrOperation.hpp"
#include "nautilus/compiler/ir/operations/FunctionOperation.hpp"
#include "nautilus/compiler/ir/operations/IfOperation.hpp"
#include "nautilus/compiler/ir/operations/IndirectCallOperation.hpp"
#include "nautilus/compiler/ir/operations/LoadOperation.hpp"
#include "nautilus/compiler/ir/operations/LogicalOperations/AndOperation.hpp"
#include "nautilus/compiler/ir/operations/LogicalOperations/CompareOperation.hpp"
#include "nautilus/compiler/ir/operations/LogicalOperations/NotOperation.hpp"
#include "nautilus/compiler/ir/operations/LogicalOperations/OrOperation.hpp"
#include "nautilus/compiler/ir/operations/ProxyCallOperation.hpp"
#include "nautilus/compiler/ir/operations/ReturnOperation.hpp"
#include "nautilus/compiler/ir/operations/SelectOperation.hpp"
#include "nautilus/compiler/ir/operations/StoreOperation.hpp"
#include <unordered_map>
#include <unordered_set>
#include <variant>

#if defined(__x86_64__) || defined(_M_X64) || defined(__i386__)
#include <asmjit/x86.h>
#else
#include <asmjit/a64.h>
#endif

namespace nautilus::compiler::asmjit {

/**
 * @brief Translates Nautilus IR to native machine code using AsmJit.
 *
 * On x86-64 hosts this lowers all IR operations to native x86-64 assembly.
 * Other architectures throw NotImplementedException.
 */
class AsmJitLoweringProvider {
public:
	AsmJitLoweringProvider() = default;

	void* lower(std::shared_ptr<ir::IRGraph> ir, ::asmjit::JitRuntime& runtime);

private:
#if defined(__x86_64__) || defined(_M_X64) || defined(__i386__)
	// Integer/pointer types → GP register; float types → XMM register.
	using AsmReg = std::variant<::asmjit::x86::Gp, ::asmjit::x86::Xmm>;
	using RegisterFrame = Frame<ir::OperationIdentifier, AsmReg>;

	class LoweringContext {
	public:
		explicit LoweringContext(std::shared_ptr<ir::IRGraph> ir, ::asmjit::CodeHolder& code);
		void process();

	private:
		::asmjit::x86::Compiler cc;
		std::shared_ptr<ir::IRGraph> ir;
		std::unordered_map<std::string, ::asmjit::Label> blockLabels;
		std::unordered_set<std::string> processedBlocks;

		static ::asmjit::TypeId getTypeId(Type t);
		static bool isFloatType(Type t);
		static bool isUnsignedType(Type t);

		// All integer types are mapped to 64-bit GP; floats to XMM.
		AsmReg allocReg(Type t);
		static ::asmjit::x86::Gp toGp(const AsmReg& r);
		static ::asmjit::x86::Xmm toXmm(const AsmReg& r);

		::asmjit::Label getOrCreateLabel(const std::string& blockId);
		void emitMove(const AsmReg& dst, const AsmReg& src);

		void processBlock(const ir::BasicBlock* block, RegisterFrame& frame);
		void processBlockInvocation(const ir::BasicBlockInvocation& bi, RegisterFrame& frame);
		void processOperation(const std::unique_ptr<ir::Operation>& op, RegisterFrame& frame);

		void processConstBool(ir::ConstBooleanOperation* op, RegisterFrame& frame);
		void processConstInt(ir::ConstIntOperation* op, RegisterFrame& frame);
		void processConstFloat(ir::ConstFloatOperation* op, RegisterFrame& frame);
		void processConstPtr(ir::ConstPtrOperation* op, RegisterFrame& frame);

		void processAdd(ir::AddOperation* op, RegisterFrame& frame);
		void processSub(ir::SubOperation* op, RegisterFrame& frame);
		void processMul(ir::MulOperation* op, RegisterFrame& frame);
		void processDiv(ir::DivOperation* op, RegisterFrame& frame);
		void processMod(ir::ModOperation* op, RegisterFrame& frame);

		void processCompare(ir::CompareOperation* op, RegisterFrame& frame);
		void processAnd(ir::AndOperation* op, RegisterFrame& frame);
		void processOr(ir::OrOperation* op, RegisterFrame& frame);
		void processNot(ir::NotOperation* op, RegisterFrame& frame);
		void processNegate(ir::NegateOperation* op, RegisterFrame& frame);
		void processShift(ir::ShiftOperation* op, RegisterFrame& frame);
		void processBinaryComp(ir::BinaryCompOperation* op, RegisterFrame& frame);

		void processIf(ir::IfOperation* op, RegisterFrame& frame);
		void processBranch(ir::BranchOperation* op, RegisterFrame& frame);
		void processReturn(ir::ReturnOperation* op, RegisterFrame& frame);
		void processSelect(ir::SelectOperation* op, RegisterFrame& frame);

		void processLoad(ir::LoadOperation* op, RegisterFrame& frame);
		void processStore(ir::StoreOperation* op, RegisterFrame& frame);
		void processAlloca(ir::AllocaOperation* op, RegisterFrame& frame);
		void processProxyCall(ir::ProxyCallOperation* op, RegisterFrame& frame);
		void processIndirectCall(ir::IndirectCallOperation* op, RegisterFrame& frame);
		void processCast(ir::CastOperation* op, RegisterFrame& frame);
	};
#endif // x86
};

} // namespace nautilus::compiler::asmjit

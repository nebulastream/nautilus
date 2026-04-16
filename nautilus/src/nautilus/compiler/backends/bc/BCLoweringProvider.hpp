#pragma once

#include "nautilus/compiler/Frame.hpp"
#include "nautilus/compiler/backends/bc/ByteCode.hpp"
#include "nautilus/compiler/ir/IRGraph.hpp"
#include "nautilus/compiler/ir/OperationDispatcher.hpp"
#include "nautilus/compiler/ir/blocks/BasicBlock.hpp"
#include <unordered_map>
#include <unordered_set>

namespace nautilus::compiler::bc {

/**
 * @brief Configuration for the BCLoweringProvider. Currently selects
 * whether the simple linear register allocator runs; disabling it
 * reproduces the "one fresh register per SSA value" behaviour and is
 * useful for A/B performance comparisons.
 */
struct LoweringOptions {
	bool enableRegisterAllocator = true;
};

/**
 * @brief The lowering provider translates the IR to the bytecode.
 */
class BCLoweringProvider {
public:
	BCLoweringProvider();

	std::tuple<Code, RegisterFile> lower(std::shared_ptr<ir::IRGraph> ir);

	/// Lower with a map of internal function name -> native function pointer.
	/// Used when NautilusFunction calls have been pre-compiled to dyncallback thunks.
	std::tuple<Code, RegisterFile> lower(std::shared_ptr<ir::IRGraph> ir,
	                                     const std::unordered_map<std::string, void*>& internalFunctionPtrs);

	/// Lower a specific named function from the IRGraph.
	std::tuple<Code, RegisterFile> lowerFunction(std::shared_ptr<ir::IRGraph> ir, const std::string& functionName);

	/// Lower a specific named function with resolved internal function pointers.
	std::tuple<Code, RegisterFile> lower(std::shared_ptr<ir::IRGraph> ir, const std::string& functionName,
	                                     const std::unordered_map<std::string, void*>& internalFunctionPtrs);

	/// Lower a specific named function with resolved internal function
	/// pointers and explicit lowering options (lets the caller disable
	/// the linear register allocator, e.g. for benchmarking).
	std::tuple<Code, RegisterFile> lower(std::shared_ptr<ir::IRGraph> ir, const std::string& functionName,
	                                     const std::unordered_map<std::string, void*>& internalFunctionPtrs,
	                                     LoweringOptions options);

private:
	using RegisterFrame = Frame<ir::OperationIdentifier, short>;

	class RegisterProvider {
	public:
		/// Allocate a register, preferring slots that have been returned
		/// to the free list.
		short allocRegister();

		/// Allocate a fresh register that will never be returned to the
		/// free list. Used for registers that hold pre-initialised
		/// values in the default register file (const/alloca/funcPtr
		/// slots): if their slot were ever reused by another op, that
		/// op's writes at runtime would clobber the pre-initialised
		/// value before subsequent loop iterations got to read it.
		short allocPinnedRegister();

		/// Allocate a register from the fresh counter only, bypassing
		/// the free list. The returned slot CAN later be handed to
		/// freeRegister(), at which point it joins the free list like
		/// any other allocation. Used to allocate block-invocation
		/// temporaries without risking aliasing with already-bound
		/// target-argument registers that may have returned to the
		/// free list earlier.
		short allocFreshRegister();

		/// Hand @p reg back to the free list so it can be reused by a
		/// future allocRegister() call. No-op if the register was
		/// allocated as pinned.
		void freeRegister(short reg);

		short getRegisterCount() const {
			return currentRegister;
		}

	private:
		short currentRegister = 0;
		std::vector<short> freeList;
		std::unordered_set<short> pinned;
	};

	class LoweringContext : public ir::OperationDispatcher<LoweringContext> {
	public:
		LoweringContext(std::shared_ptr<ir::IRGraph> ir, std::string targetFunctionName = "execute");
		LoweringContext(std::shared_ptr<ir::IRGraph> ir,
		                const std::unordered_map<std::string, void*>& internalFunctionPtrs);
		LoweringContext(std::shared_ptr<ir::IRGraph> ir, std::string targetFunctionName,
		                const std::unordered_map<std::string, void*>& internalFunctionPtrs);
		LoweringContext(std::shared_ptr<ir::IRGraph> ir, std::string targetFunctionName,
		                const std::unordered_map<std::string, void*>& internalFunctionPtrs, LoweringOptions options);

		std::tuple<Code, RegisterFile> process();

		short process(const ir::BasicBlock*, RegisterFrame& frame);

	private:
		// Allow the CRTP dispatcher to call our private visitXxx hooks.
		friend class ir::OperationDispatcher<LoweringContext>;

		Code program;
		RegisterFile defaultRegisterFile;
		std::shared_ptr<ir::IRGraph> ir;
		std::unordered_map<std::string, void*> internalFunctionPtrs;
		std::string targetFunctionName = "execute";
		LoweringOptions loweringOptions;
		RegisterProvider registerProvider;
		std::unordered_map<ir::BlockIdentifier, short> activeBlocks;
		std::unordered_map<ir::OperationIdentifier, int> usageCounts;
		std::unordered_set<ir::OperationIdentifier> functionArgs;

		void process(const ir::BasicBlockInvocation& opt, short block, RegisterFrame& frame);

		// Per-operation hooks invoked by OperationDispatcher::dispatch.
		void visitAdd(ir::AddOperation* opt, short block, RegisterFrame& frame);
		void visitMul(ir::MulOperation* opt, short block, RegisterFrame& frame);
		void visitDiv(ir::DivOperation* opt, short block, RegisterFrame& frame);
		void visitMod(ir::ModOperation* opt, short block, RegisterFrame& frame);
		void visitSub(ir::SubOperation* opt, short block, RegisterFrame& frame);
		void visitIf(ir::IfOperation* opt, short block, RegisterFrame& frame);
		void visitCompare(ir::CompareOperation* opt, short block, RegisterFrame& frame);
		void visitBranch(ir::BranchOperation* opt, short block, RegisterFrame& frame);
		void visitLoad(ir::LoadOperation* opt, short block, RegisterFrame& frame);
		void visitStore(ir::StoreOperation* opt, short block, RegisterFrame& frame);
		void visitProxyCall(ir::ProxyCallOperation* opt, short block, RegisterFrame& frame);
		void visitIndirectCall(ir::IndirectCallOperation* opt, short block, RegisterFrame& frame);
		void visitOr(ir::OrOperation* opt, short block, RegisterFrame& frame);
		void visitAnd(ir::AndOperation* opt, short block, RegisterFrame& frame);
		void visitNegate(ir::NegateOperation* opt, short block, RegisterFrame& frame);
		void visitNot(ir::NotOperation* opt, short block, RegisterFrame& frame);
		void visitAlloca(ir::AllocaOperation* opt, short block, RegisterFrame& frame);
		void visitCast(ir::CastOperation* opt, short block, RegisterFrame& frame);
		void visitSelect(ir::SelectOperation* opt, short block, RegisterFrame& frame);
		void visitBinaryComp(ir::BinaryCompOperation* opt, short block, RegisterFrame& frame);
		void visitShift(ir::ShiftOperation* opt, short block, RegisterFrame& frame);
		void visitFunctionAddressOf(ir::FunctionAddressOfOperation* opt, short block, RegisterFrame& frame);
		void visitReturn(ir::ReturnOperation* opt, short block, RegisterFrame& frame);
		void visitConstInt(ir::ConstIntOperation* opt, short block, RegisterFrame& frame);
		void visitConstFloat(ir::ConstFloatOperation* opt, short block, RegisterFrame& frame);
		void visitConstBoolean(ir::ConstBooleanOperation* opt, short block, RegisterFrame& frame);
		void visitConstPtr(ir::ConstPtrOperation* opt, short block, RegisterFrame& frame);

		void processDynamicCall(ir::ProxyCallOperation* opt, short block, RegisterFrame& frame);

		short getResultRegister(ir::Operation* opt, RegisterFrame& frame);

		void allocateRegister(short registerIndex) {
			if (registerIndex >= static_cast<short>(defaultRegisterFile.size())) {
				defaultRegisterFile.resize(registerIndex + 1, 0);
			}
		}

		/// Linear register allocator: pre-pass that walks every reachable
		/// basic block once and records, for every SSA value, how many
		/// times it is statically referenced across the entire function.
		/// Inputs of each operation are counted via Operation::getInputs();
		/// the arguments of the BasicBlockInvocations embedded in the
		/// IfOp/BranchOp terminators are counted separately because they
		/// live outside the terminator's own inputs span.
		void countAllUsages(const ir::BasicBlock* entryBlock);

		/// Decrement the remaining-use counter of @p identifier. When it
		/// drops to zero the corresponding physical register is returned
		/// to the free list so later operations can re-use it. Function
		/// arguments are pinned and never returned.
		void useValue(const ir::OperationIdentifier& identifier, RegisterFrame& frame);
	};
};
} // namespace nautilus::compiler::bc

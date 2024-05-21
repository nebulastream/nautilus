#pragma once

#include "nautilus/compiler/Frame.hpp"
#include "nautilus/compiler/backends/bc/ByteCode.hpp"
#include "nautilus/compiler/ir/IRGraph.hpp"
#include "nautilus/compiler/ir/blocks/BasicBlock.hpp"
#include "nautilus/compiler/ir/operations/ArithmeticOperations/AddOperation.hpp"
#include "nautilus/compiler/ir/operations/ArithmeticOperations/DivOperation.hpp"
#include "nautilus/compiler/ir/operations/ArithmeticOperations/ModOperation.hpp"
#include "nautilus/compiler/ir/operations/ArithmeticOperations/MulOperation.hpp"
#include "nautilus/compiler/ir/operations/ArithmeticOperations/SubOperation.hpp"
#include "nautilus/compiler/ir/operations/BranchOperation.hpp"
#include "nautilus/compiler/ir/operations/CastOperation.hpp"
#include "nautilus/compiler/ir/operations/ConstBooleanOperation.hpp"
#include "nautilus/compiler/ir/operations/ConstFloatOperation.hpp"
#include "nautilus/compiler/ir/operations/ConstIntOperation.hpp"
#include "nautilus/compiler/ir/operations/FunctionOperation.hpp"
#include "nautilus/compiler/ir/operations/IfOperation.hpp"
#include "nautilus/compiler/ir/operations/LoadOperation.hpp"
#include "nautilus/compiler/ir/operations/LogicalOperations/AndOperation.hpp"
#include "nautilus/compiler/ir/operations/LogicalOperations/CompareOperation.hpp"
#include "nautilus/compiler/ir/operations/LogicalOperations/NegateOperation.hpp"
#include "nautilus/compiler/ir/operations/LogicalOperations/OrOperation.hpp"
#include "nautilus/compiler/ir/operations/Operation.hpp"
#include "nautilus/compiler/ir/operations/ProxyCallOperation.hpp"
#include "nautilus/compiler/ir/operations/ReturnOperation.hpp"
#include "nautilus/compiler/ir/operations/StoreOperation.hpp"
#include <unordered_set>

namespace nautilus::compiler::bc {

/**
 * @brief The lowering provider translates the IR to the bytecode.
 */
class BCLoweringProvider {
public:
	BCLoweringProvider();

	std::tuple<Code, RegisterFile> lower(std::shared_ptr<ir::IRGraph> ir);

private:
	using RegisterFrame = Frame<ir::OperationIdentifier, short>;

	class RegisterProvider {
	public:
		short allocRegister();

		void freeRegister();

	private:
		short currentRegister = 0;
	};

	class LoweringContext {
	public:
		LoweringContext(std::shared_ptr<ir::IRGraph> ir);

		std::tuple<Code, RegisterFile> process();

		short process(const ir::BasicBlock*, RegisterFrame& frame);

		void process(const std::unique_ptr<ir::Operation>& operation, short block, RegisterFrame& frame);

	private:
		Code program;
		RegisterFile defaultRegisterFile;
		std::shared_ptr<ir::IRGraph> ir;
		RegisterProvider registerProvider;
		std::unordered_map<std::string, short> activeBlocks;

		void process(ir::AddOperation* opt, short block, RegisterFrame& frame);

		void process(ir::MulOperation* opt, short block, RegisterFrame& frame);

		void process(ir::DivOperation* opt, short block, RegisterFrame& frame);

		void process(ir::ModOperation* opt, short block, RegisterFrame& frame);

		void process(ir::SubOperation* opt, short block, RegisterFrame& frame);

		void process(ir::IfOperation* opt, short block, RegisterFrame& frame);

		void process(ir::CompareOperation* opt, short block, RegisterFrame& frame);

		void process(ir::BranchOperation* opt, short block, RegisterFrame& frame);

		void process(ir::BasicBlockInvocation& opt, short block, RegisterFrame& frame);

		void process(ir::LoadOperation* opt, short block, RegisterFrame& frame);

		void process(ir::StoreOperation* opt, short block, RegisterFrame& frame);

		void process(ir::ProxyCallOperation* opt, short block, RegisterFrame& frame);

		void process(ir::OrOperation* opt, short block, RegisterFrame& frame);

		void process(ir::AndOperation* opt, short block, RegisterFrame& frame);

		void process(ir::NegateOperation* opt, short block, RegisterFrame& frame);

		void process(ir::CastOperation* opt, short block, RegisterFrame& frame);

		void processDynamicCall(ir::ProxyCallOperation* opt, short block, RegisterFrame& frame);

		short getResultRegister(ir::Operation* opt, RegisterFrame& frame);
	};
};
} // namespace nautilus::compiler::bc

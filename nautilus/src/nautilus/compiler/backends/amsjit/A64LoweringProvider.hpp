#pragma once

#include "nautilus/compiler/Frame.hpp"
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
#include <asmjit/a64.h>
#include <sstream>
#include <unordered_set>
#include <vector>

namespace nautilus::compiler::asmjit {

/**
 * @brief The lowering provider translates the IR to the a64 assembly.
 */
class A64LoweringProvider {
public:
	A64LoweringProvider() = default;

	void* lower(std::shared_ptr<ir::IRGraph> ir, ::asmjit::JitRuntime& runtime);

private:
	using RegisterFrame = Frame<ir::OperationIdentifier, ::asmjit::a64::Gp>;

	class LoweringContext {
	public:
		explicit LoweringContext(std::shared_ptr<ir::IRGraph> ir, ::asmjit::CodeHolder& code);

		void process();

	private:
		::asmjit::a64::Compiler cc;
		std::shared_ptr<ir::IRGraph> ir;
		std::unordered_map<std::string, std::string> activeBlocks;
		std::unordered_set<std::string> functionNames;
		std::string returnType;

		std::string process(const ir::BasicBlock*, RegisterFrame& frame);

		void process(ir::BasicBlockInvocation& opt, RegisterFrame& frame);

		void process(const std::unique_ptr<ir::Operation>& operation, RegisterFrame& frame);

		void process(ir::IfOperation* opt, short block, RegisterFrame& frame);

		void process(ir::CompareOperation* opt, short block, RegisterFrame& frame);

		void process(ir::BranchOperation* opt, short block, RegisterFrame& frame);

		void process(ir::LoadOperation* opt, short block, RegisterFrame& frame);

		void process(ir::StoreOperation* opt, short block, RegisterFrame& frame);

		void process(ir::ProxyCallOperation* opt, short block, RegisterFrame& frame);

		void process(ir::NegateOperation* opt, short block, RegisterFrame& frame);

		void process(ir::CastOperation* opt, short block, RegisterFrame& frame);

		static std::string getVariable(const ir::OperationIdentifier& id);

		static std::string getType(const Type& stamp);

		template <class Type>
		void processConst(const std::unique_ptr<ir::Operation>& opt, short blockIndex, RegisterFrame& frame) {
			auto constValue = static_cast<Type*>(opt.get());
			auto var = getVariable(constValue->getIdentifier());
			blockArguments << getType(constValue->getStamp()) << " " << var << ";\n";
			frame.setValue(constValue->getIdentifier(), var);
			blocks[blockIndex] << var << " = " << constValue->getValue() << ";\n";
		}

		template <class Type>
		void processBinary(const std::unique_ptr<ir::Operation>& o, const std::string& operation, short blockIndex,
		                   RegisterFrame& frame) {
			auto op = static_cast<Type*>(o.get());
			auto leftInput = frame.getValue(op->getLeftInput()->getIdentifier());
			auto rightInput = frame.getValue(op->getRightInput()->getIdentifier());
			auto resultVar = getVariable(op->getIdentifier());
			blockArguments << getType(op->getStamp()) << " " << resultVar << ";\n";
			frame.setValue(op->getIdentifier(), resultVar);
			blocks[blockIndex] << resultVar << " = " << leftInput << operation << rightInput << ";\n";
		}
	};
};
} // namespace nautilus::compiler::asmjit

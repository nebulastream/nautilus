#pragma once

#include "nautilus/compiler/Frame.hpp"
#include "nautilus/compiler/ir/IRGraph.hpp"
#include "nautilus/compiler/ir/OperationDispatcher.hpp"
#include "nautilus/compiler/ir/blocks/BasicBlock.hpp"
#include <cmath>
#include <iomanip>
#include <limits>
#include <sstream>
#include <type_traits>
#include <unordered_set>
#include <vector>

namespace nautilus::compiler::cpp {

/**
 * @brief The lowering provider translates the IR to the cpp code.
 */
class CPPLoweringProvider {
public:
	CPPLoweringProvider() = default;

	static std::string lower(std::shared_ptr<ir::IRGraph> ir);

private:
	using RegisterFrame = Frame<ir::OperationIdentifier, std::string>;
	using Code = std::stringstream;

	class LoweringContext : public ir::OperationDispatcher<LoweringContext> {
	public:
		explicit LoweringContext(std::shared_ptr<ir::IRGraph> ir);

		Code process();

	private:
		// Allow the CRTP dispatcher to call our private visitXxx hooks.
		friend class ir::OperationDispatcher<LoweringContext>;

		Code blockArguments;
		Code functions;
		std::vector<Code> blocks;
		std::shared_ptr<ir::IRGraph> ir;
		std::unordered_map<ir::BlockIdentifier, std::string> activeBlocks;
		std::unordered_set<std::string> functionNames;
		std::string returnType;
		/// Variable names for the current function's alloca slots, indexed by
		/// AllocaOperation::getIndex().  Populated in process()'s function
		/// prologue from FunctionOperation::getAllocaSpecs(); cleared per
		/// function so indices from a previous function don't bleed through.
		std::vector<std::string> functionAllocaSlots;

		std::string process(const ir::BasicBlock*, RegisterFrame& frame);

		void process(const ir::BasicBlockInvocation& opt, short block, RegisterFrame& frame);

		// Per-operation hooks invoked by OperationDispatcher::dispatch.
		void visitAdd(ir::AddOperation* opt, short block, RegisterFrame& frame);
		void visitSub(ir::SubOperation* opt, short block, RegisterFrame& frame);
		void visitMul(ir::MulOperation* opt, short block, RegisterFrame& frame);
		void visitDiv(ir::DivOperation* opt, short block, RegisterFrame& frame);
		void visitMod(ir::ModOperation* opt, short block, RegisterFrame& frame);
		void visitAnd(ir::AndOperation* opt, short block, RegisterFrame& frame);
		void visitOr(ir::OrOperation* opt, short block, RegisterFrame& frame);
		void visitShift(ir::ShiftOperation* opt, short block, RegisterFrame& frame);
		void visitBinaryComp(ir::BinaryCompOperation* opt, short block, RegisterFrame& frame);
		void visitConstInt(ir::ConstIntOperation* opt, short block, RegisterFrame& frame);
		void visitConstFloat(ir::ConstFloatOperation* opt, short block, RegisterFrame& frame);
		void visitConstBoolean(ir::ConstBooleanOperation* opt, short block, RegisterFrame& frame);
		void visitConstPtr(ir::ConstPtrOperation* opt, short block, RegisterFrame& frame);
		void visitReturn(ir::ReturnOperation* opt, short block, RegisterFrame& frame);
		void visitIf(ir::IfOperation* opt, short block, RegisterFrame& frame);
		void visitCompare(ir::CompareOperation* opt, short block, RegisterFrame& frame);
		void visitBranch(ir::BranchOperation* opt, short block, RegisterFrame& frame);
		void visitLoad(ir::LoadOperation* opt, short block, RegisterFrame& frame);
		void visitStore(ir::StoreOperation* opt, short block, RegisterFrame& frame);
		void visitProxyCall(ir::ProxyCallOperation* opt, short block, RegisterFrame& frame);
		void visitIndirectCall(ir::IndirectCallOperation* opt, short block, RegisterFrame& frame);
		void visitNegate(ir::NegateOperation* opt, short block, RegisterFrame& frame);
		void visitNot(ir::NotOperation* opt, short block, RegisterFrame& frame);
		void visitAlloca(ir::AllocaOperation* opt, short block, RegisterFrame& frame);
		void visitCast(ir::CastOperation* opt, short block, RegisterFrame& frame);
		void visitSelect(ir::SelectOperation* opt, short block, RegisterFrame& frame);
		void visitFunctionAddressOf(ir::FunctionAddressOfOperation* opt, short block, RegisterFrame& frame);

		static std::string getVariable(const ir::OperationIdentifier& id);

		static std::string getType(const Type& stamp);

		template <class Type>
		void processConst(Type* constValue, short blockIndex, RegisterFrame& frame) {
			auto var = getVariable(constValue->getIdentifier());
			if (!frame.contains(constValue->getIdentifier())) {
				blockArguments << getType(constValue->getStamp()) << " " << var << ";\n";
				frame.setValue(constValue->getIdentifier(), var);
			}

			std::stringstream ss;
			if constexpr (std::is_floating_point_v<decltype(constValue->getValue())>) {
				const auto value = constValue->getValue();
				if (std::isnan(value)) {
					// operator<< prints non-finite doubles as the bare words
					// "nan"/"inf", which aren't valid C++ literal syntax and
					// fail to recompile. Emit compiler builtins instead, which
					// need no extra #include in the generated source.
					ss << (std::signbit(value) ? "-__builtin_nan(\"\")" : "__builtin_nan(\"\")");
				} else if (std::isinf(value)) {
					ss << (value < 0 ? "-__builtin_inf()" : "__builtin_inf()");
				} else {
					// Floating-point constants need full round-trip precision:
					// the default stream precision is 6 significant digits, so
					// without this the generated C++ source re-parses a
					// rounded-off literal (e.g. a double could silently lose
					// ~11 significant digits).
					ss << std::setprecision(std::numeric_limits<double>::max_digits10);
					ss << value;
				}
			} else {
				ss << constValue->getValue();
			}
			if (ss.str() == "(nil)") {
				blocks[blockIndex] << var << " = (" << getType(constValue->getStamp()) << ")(nullptr);\n";
			} else {
				// Emit ss.str() (built with round-trip precision above), not a
				// fresh `<< constValue->getValue()` -- that would re-stream the
				// raw value at default (6 significant digit) precision and
				// silently undo the setprecision() above.
				blocks[blockIndex] << var << " = (" << getType(constValue->getStamp()) << ")" << ss.str() << ";\n";
			}
		}

		template <class Type>
		void processBinary(Type* op, const std::string& operation, short blockIndex, RegisterFrame& frame) {
			auto leftInput = frame.getValue(op->getLeftInput()->getIdentifier());
			auto rightInput = frame.getValue(op->getRightInput()->getIdentifier());
			auto resultVar = getVariable(op->getIdentifier());
			if (!frame.contains(op->getIdentifier())) {
				blockArguments << getType(op->getStamp()) << " " << resultVar << ";\n";
				frame.setValue(op->getIdentifier(), resultVar);
			}
			blocks[blockIndex] << resultVar << " = " << leftInput << operation << rightInput << ";\n";
		}
	};
};
} // namespace nautilus::compiler::cpp


#include "nautilus/compiler/ir/operations/ConstPtrOperation.hpp"
#include "nautilus/compiler/ir/operations/FunctionAddressOfOperation.hpp"
#include "nautilus/compiler/ir/operations/SelectOperation.hpp"
#include <cassert>
#include <nautilus/compiler/backends/cpp/CPPLoweringProvider.hpp>
#include <nautilus/compiler/ir/operations/ArithmeticOperations/DivOperation.hpp>
#include <nautilus/compiler/ir/operations/ArithmeticOperations/MulOperation.hpp>
#include <nautilus/compiler/ir/operations/LogicalOperations/AndOperation.hpp>
#include <nautilus/compiler/ir/operations/LogicalOperations/OrOperation.hpp>
#include <nautilus/compiler/ir/operations/Operation.hpp>
#include <nautilus/exceptions/NotImplementedException.hpp>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

namespace nautilus::compiler::cpp {

CPPLoweringProvider::LoweringContext::LoweringContext(std::shared_ptr<ir::IRGraph> ir) : ir(std::move(ir)) {
}

std::string CPPLoweringProvider::lower(std::shared_ptr<ir::IRGraph> ir) {
	auto ctx = LoweringContext(std::move(ir));
	return ctx.process().str();
}

std::string CPPLoweringProvider::LoweringContext::getType(const Type& stamp) {
	switch (stamp) {
	case Type::v:
		return "void";
	case Type::b:
		return "bool";
	case Type::i8:
		return "int8_t";
	case Type::i16:
		return "int16_t";
	case Type::i32:
		return "int32_t";
		;
	case Type::i64:
		return "int64_t";
		;
	case Type::ui8:
		return "uint8_t";
		;
	case Type::ui16:
		return "uint16_t";
		;
	case Type::ui32:
		return "uint32_t";
		;
	case Type::ui64:
		return "uint64_t";
		;
	case Type::f32:
		return "float";
	case Type::f64:
		return "double";
	case Type::ptr:
		return "uint8_t*";
	}
	assert(false);
	return "unknown";
}

std::stringstream CPPLoweringProvider::LoweringContext::process() {
	std::stringstream pipelineCode;
	pipelineCode << "\n";
	pipelineCode << "#include <cstdint>\n\n";

	// Process all function operations in the IR graph
	const auto& functionOperations = ir->getFunctionOperations();

	// Helper lambda to process a single function
	auto processFunction = [&](const ir::FunctionOperation& functionOperation) {
		// Reset state for each function
		blocks.clear();
		activeBlocks.clear();
		blockArguments.str("");
		blockArguments.clear();
		functions.str("");
		functions.clear();
		functionNames.clear();

		RegisterFrame rootFrame;
		std::vector<std::string> arguments;
		const auto& functionBasicBlock = functionOperation.getFunctionBasicBlock();

		for (auto i = 0ull; i < functionBasicBlock.getArguments().size(); i++) {
			auto argument = functionBasicBlock.getArguments()[i].get();
			auto var = getVariable(argument->getIdentifier());
			rootFrame.setValue(argument->getIdentifier(), var);
			arguments.emplace_back(getType(argument->getStamp()) + " " + var);
		}

		this->process(&functionBasicBlock, rootFrame);

		// Generate function code
		pipelineCode << "extern \"C\" " << returnType << " " << functionOperation.getName() << "(";
		for (size_t i = 0; i < arguments.size(); i++) {
			if (i != 0) {
				pipelineCode << ",";
			}
			pipelineCode << arguments[i] << " ";
		}
		pipelineCode << "){\n";
		pipelineCode << "//variable declarations\n";
		pipelineCode << blockArguments.str();
		if (!functions.str().empty()) {
			pipelineCode << "//function definitions\n";
			pipelineCode << functions.str();
		}
		pipelineCode << "//basic blocks\n";
		for (auto& block : blocks) {
			pipelineCode << block.str();
			pipelineCode << "\n";
		}
		pipelineCode << "}\n\n";
	};

	// Emit forward declarations so functions can call each other regardless of definition order.
	for (const auto& functionOperation : functionOperations) {
		const auto& functionBasicBlock = functionOperation->getFunctionBasicBlock();
		pipelineCode << "extern \"C\" " << getType(functionOperation->getOutputArg()) << " "
		             << functionOperation->getName() << "(";
		for (size_t i = 0; i < functionBasicBlock.getArguments().size(); i++) {
			if (i != 0) {
				pipelineCode << ",";
			}
			pipelineCode << getType(functionBasicBlock.getArguments()[i]->getStamp());
		}
		pipelineCode << ");\n";
	}
	pipelineCode << "\n";

	// Emit function definitions.
	for (const auto& functionOperation : functionOperations) {
		processFunction(*functionOperation);
	}

	return pipelineCode;
}

std::string CPPLoweringProvider::LoweringContext::process(const ir::BasicBlock* block, RegisterFrame& frame) {
	// assume that all argument registers are correctly set
	auto entry = activeBlocks.find(block->getIdentifier());
	if (entry == activeBlocks.end()) {

		for (auto& arg : block->getArguments()) {
			if (!frame.contains(arg->getIdentifier())) {
				auto var = getVariable(arg->getIdentifier());
				blockArguments << getType(arg->getStamp()) << " " << var << ";\n";
				frame.setValue(arg->getIdentifier(), var);
			}
		}
		// create bytecode block;
		auto blockName = "Block_" + std::to_string(block->getIdentifier().getId());
		short blockIndex = blocks.size();
		auto& currentBlock = blocks.emplace_back();
		currentBlock << blockName << ":\n";
		activeBlocks.emplace(block->getIdentifier(), blockName);
		for (auto& opt : block->getOperations()) {
			this->dispatch(opt, blockIndex, frame);
		}
		return blockName;
	} else {
		return entry->second;
	}
}

void CPPLoweringProvider::LoweringContext::visitCompare(ir::CompareOperation* cmpOp, short blockIndex,
                                                        RegisterFrame& frame) {
	auto leftInput = frame.getValue(cmpOp->getLeftInput()->getIdentifier());
	auto rightInput = frame.getValue(cmpOp->getRightInput()->getIdentifier());
	auto resultVar = getVariable(cmpOp->getIdentifier());
	if (!frame.contains(cmpOp->getIdentifier())) {
		blockArguments << getType(cmpOp->getStamp()) << " " << resultVar << ";\n";
		frame.setValue(cmpOp->getIdentifier(), resultVar);
	}

	// we have to handle the special case that we want to do a null check.
	// Currently, Nautilus IR just contains a x == 0, thus we check if x is a ptr
	// type.
	if (cmpOp->isEquals() && cmpOp->getLeftInput()->getStamp() == Type::ptr &&
	    isInteger(cmpOp->getRightInput()->getStamp())) {
		blocks[blockIndex] << resultVar << " = " << leftInput << " == nullptr;\n";
		return;
	}

	std::string comperator;
	if (cmpOp->getComparator() == ir::CompareOperation::Comparator::EQ) {
		comperator = " == ";
	} else if (cmpOp->getComparator() == ir::CompareOperation::Comparator::LT) {
		comperator = " < ";
	} else if (cmpOp->getComparator() == ir::CompareOperation::Comparator::GT) {
		comperator = " > ";
	} else if (cmpOp->getComparator() == ir::CompareOperation::Comparator::GE) {
		comperator = " >= ";
	} else if (cmpOp->getComparator() == ir::CompareOperation::Comparator::LE) {
		comperator = " <= ";
	} else if (cmpOp->getComparator() == ir::CompareOperation::Comparator::NE) {
		comperator = " != ";
	} else {
		throw NotImplementedException("Operation is not implemented.");
	}

	blocks[blockIndex] << resultVar << " = " << leftInput << comperator << rightInput << ";\n";
}

void CPPLoweringProvider::LoweringContext::visitLoad(ir::LoadOperation* loadOp, short blockIndex,
                                                     RegisterFrame& frame) {
	auto address = frame.getValue(loadOp->getAddress()->getIdentifier());
	auto resultVar = getVariable(loadOp->getIdentifier());
	if (!frame.contains(loadOp->getIdentifier())) {
		blockArguments << getType(loadOp->getStamp()) << " " << resultVar << ";\n";
		frame.setValue(loadOp->getIdentifier(), resultVar);
	}
	auto type = getType(loadOp->getStamp());
	blocks[blockIndex] << resultVar << " = *reinterpret_cast<" << type << "*>(" << address << ");\n";
}

void CPPLoweringProvider::LoweringContext::visitStore(ir::StoreOperation* storeOp, short blockIndex,
                                                      RegisterFrame& frame) {
	auto address = frame.getValue(storeOp->getAddress()->getIdentifier());
	auto value = frame.getValue(storeOp->getValue()->getIdentifier());
	auto type = getType(storeOp->getValue()->getStamp());
	blocks[blockIndex] << "*reinterpret_cast<" << type << "*>(" << address << ") = " << value << ";\n";
}

void CPPLoweringProvider::LoweringContext::process(const ir::BasicBlockInvocation& bi, short blockIndex,
                                                   RegisterFrame& parentFrame) {
	auto blockInputArguments = bi.getArguments();
	auto& blockTargetArguments = bi.getBlock()->getArguments();
	blocks[blockIndex] << "// prepare block arguments\n";
	blocks[blockIndex] << "{\n";
	for (uint64_t i = 0; i < blockInputArguments.size(); i++) {
		auto blockArgument = blockInputArguments[i]->getIdentifier();
		blocks[blockIndex] << getType(blockTargetArguments[i]->getStamp()) << " temp_" << i << " = "
		                   << parentFrame.getValue(blockArgument) << ";\n";
	}
	for (uint64_t i = 0; i < blockInputArguments.size(); i++) {
		auto blockTargetArgument = blockTargetArguments[i]->getIdentifier();

		if (!parentFrame.contains(blockTargetArgument)) {
			auto var = getVariable(blockTargetArgument);
			parentFrame.setValue(blockTargetArgument, var);
			blockArguments << getType(blockTargetArguments[i]->getStamp()) << " " << var << ";\n";
		}

		blocks[blockIndex] << parentFrame.getValue(blockTargetArgument) << " = " << "temp_" << i << ";\n";
	}
	blocks[blockIndex] << "}\n";
}

void CPPLoweringProvider::LoweringContext::visitIf(ir::IfOperation* ifOpt, short blockIndex, RegisterFrame& frame) {
	auto conditionalReg = frame.getValue(ifOpt->getValue()->getIdentifier());
	auto trueBlock = process(ifOpt->getTrueBlockInvocation().getBlock(), frame);
	auto falseBlock = process(ifOpt->getFalseBlockInvocation().getBlock(), frame);
	blocks[blockIndex] << "if (" << conditionalReg << "){\n";
	process(ifOpt->getTrueBlockInvocation(), blockIndex, frame);
	blocks[blockIndex] << "goto " << trueBlock << ";\n";
	blocks[blockIndex] << "}else{\n";
	process(ifOpt->getFalseBlockInvocation(), blockIndex, frame);
	blocks[blockIndex] << "goto " << falseBlock << ";}\n";
}

void CPPLoweringProvider::LoweringContext::visitBranch(ir::BranchOperation* branchOp, short blockIndex,
                                                       RegisterFrame& frame) {
	process(branchOp->getNextBlockInvocation(), blockIndex, frame);
	auto nextBlock = process(branchOp->getNextBlockInvocation().getBlock(), frame);
	blocks[blockIndex] << "goto " << nextBlock << ";\n";
}

// Thin wrappers that turn a typed op into the matching C operator and delegate
// to processBinary / processConst. All other per-op handlers keep their dedicated
// definitions below (renamed from `process` to `visitXxx`).
void CPPLoweringProvider::LoweringContext::visitAdd(ir::AddOperation* op, short blockIndex, RegisterFrame& frame) {
	processBinary<ir::AddOperation>(op, "+", blockIndex, frame);
}

void CPPLoweringProvider::LoweringContext::visitSub(ir::SubOperation* op, short blockIndex, RegisterFrame& frame) {
	processBinary<ir::SubOperation>(op, "-", blockIndex, frame);
}

void CPPLoweringProvider::LoweringContext::visitMul(ir::MulOperation* op, short blockIndex, RegisterFrame& frame) {
	processBinary<ir::MulOperation>(op, "*", blockIndex, frame);
}

void CPPLoweringProvider::LoweringContext::visitDiv(ir::DivOperation* op, short blockIndex, RegisterFrame& frame) {
	processBinary<ir::DivOperation>(op, "/", blockIndex, frame);
}

void CPPLoweringProvider::LoweringContext::visitMod(ir::ModOperation* op, short blockIndex, RegisterFrame& frame) {
	processBinary<ir::ModOperation>(op, "%", blockIndex, frame);
}

void CPPLoweringProvider::LoweringContext::visitOr(ir::OrOperation* op, short blockIndex, RegisterFrame& frame) {
	processBinary<ir::OrOperation>(op, "||", blockIndex, frame);
}

void CPPLoweringProvider::LoweringContext::visitAnd(ir::AndOperation* op, short blockIndex, RegisterFrame& frame) {
	processBinary<ir::AndOperation>(op, "&&", blockIndex, frame);
}

void CPPLoweringProvider::LoweringContext::visitShift(ir::ShiftOperation* op, short blockIndex, RegisterFrame& frame) {
	if (op->getType() == ir::ShiftOperation::LS) {
		processBinary<ir::ShiftOperation>(op, "<<", blockIndex, frame);
	} else {
		processBinary<ir::ShiftOperation>(op, ">>", blockIndex, frame);
	}
}

void CPPLoweringProvider::LoweringContext::visitBinaryComp(ir::BinaryCompOperation* op, short blockIndex,
                                                           RegisterFrame& frame) {
	switch (op->getType()) {
	case ir::BinaryCompOperation::BAND:
		processBinary<ir::BinaryCompOperation>(op, "&", blockIndex, frame);
		break;
	case ir::BinaryCompOperation::BOR:
		processBinary<ir::BinaryCompOperation>(op, "|", blockIndex, frame);
		break;
	case ir::BinaryCompOperation::XOR:
		processBinary<ir::BinaryCompOperation>(op, "^", blockIndex, frame);
		break;
	}
}

void CPPLoweringProvider::LoweringContext::visitConstInt(ir::ConstIntOperation* op, short blockIndex,
                                                         RegisterFrame& frame) {
	processConst<ir::ConstIntOperation>(op, blockIndex, frame);
}

void CPPLoweringProvider::LoweringContext::visitConstFloat(ir::ConstFloatOperation* op, short blockIndex,
                                                           RegisterFrame& frame) {
	processConst<ir::ConstFloatOperation>(op, blockIndex, frame);
}

void CPPLoweringProvider::LoweringContext::visitConstBoolean(ir::ConstBooleanOperation* op, short blockIndex,
                                                             RegisterFrame& frame) {
	processConst<ir::ConstBooleanOperation>(op, blockIndex, frame);
}

void CPPLoweringProvider::LoweringContext::visitConstPtr(ir::ConstPtrOperation* op, short blockIndex,
                                                         RegisterFrame& frame) {
	processConst<ir::ConstPtrOperation>(op, blockIndex, frame);
}

void CPPLoweringProvider::LoweringContext::visitReturn(ir::ReturnOperation* returnOpt, short blockIndex,
                                                       RegisterFrame& frame) {
	if (returnOpt->hasReturnValue()) {
		auto returnFOp = frame.getValue(returnOpt->getReturnValue()->getIdentifier());
		blocks[blockIndex] << "return " << returnFOp << ";\n";

		this->returnType = getType(returnOpt->getStamp());
	} else {
		blocks[blockIndex] << "return;\n";
		this->returnType = "void";
	}
}

void CPPLoweringProvider::LoweringContext::visitProxyCall(ir::ProxyCallOperation* opt, short blockIndex,
                                                          RegisterFrame& frame) {

	auto returnType = getType(opt->getStamp());
	std::stringstream argTypes;
	std::stringstream args;
	for (size_t i = 0; i < opt->getInputArguments().size(); i++) {
		auto arg = opt->getInputArguments()[i];

		if (i != 0) {
			argTypes << ",";
			args << ",";
		}
		args << frame.getValue(arg->getIdentifier());
		argTypes << getType(arg->getStamp());
	}
	// Check if this function is defined in the same compilation unit (i.e., a NautilusFunction).
	// If so, call it directly by name instead of through a function pointer cast,
	// because the stored ptr for NautilusFunction calls is not a valid function pointer.
	bool isInternalFunction = ir->getFunctionOperation(opt->getFunctionName()) != nullptr;

	if (!isInternalFunction && !functionNames.contains(opt->getFunctionSymbol())) {
		functions << "auto f_" << opt->getFunctionSymbol() << " = " << "(" << returnType << "(*)(" << argTypes.str()
		          << "))" << opt->getFunctionPtr() << ";\n";
		functionNames.emplace(opt->getFunctionSymbol());
	}
	if (opt->getStamp() != Type::v) {
		auto resultVar = getVariable(opt->getIdentifier());
		if (!frame.contains(opt->getIdentifier())) {
			blockArguments << getType(opt->getStamp()) << " " << resultVar << ";\n";
			frame.setValue(opt->getIdentifier(), resultVar);
		}
		blocks[blockIndex] << resultVar << " = ";
	}
	if (isInternalFunction) {
		blocks[blockIndex] << opt->getFunctionName() << "(" << args.str() << ");\n";
	} else {
		blocks[blockIndex] << "f_" << opt->getFunctionSymbol() << "(" << args.str() << ");\n";
	}
}

void CPPLoweringProvider::LoweringContext::visitIndirectCall(ir::IndirectCallOperation* opt, short blockIndex,
                                                             RegisterFrame& frame) {
	auto returnType = getType(opt->getStamp());
	std::stringstream argTypes;
	std::stringstream args;
	auto inputArguments = opt->getInputArguments();
	for (size_t i = 0; i < inputArguments.size(); i++) {
		auto arg = inputArguments[i];
		if (i != 0) {
			argTypes << ",";
			args << ",";
		}
		args << frame.getValue(arg->getIdentifier());
		argTypes << getType(arg->getStamp());
	}
	auto fnPtrVar = frame.getValue(opt->getFunctionPtrOperand()->getIdentifier());
	if (opt->getStamp() != Type::v) {
		auto resultVar = getVariable(opt->getIdentifier());
		if (!frame.contains(opt->getIdentifier())) {
			blockArguments << getType(opt->getStamp()) << " " << resultVar << ";\n";
			frame.setValue(opt->getIdentifier(), resultVar);
		}
		blocks[blockIndex] << resultVar << " = ((" << returnType << "(*)(" << argTypes.str() << "))" << fnPtrVar << ")("
		                   << args.str() << ");\n";
	} else {
		blocks[blockIndex] << "((" << returnType << "(*)(" << argTypes.str() << "))" << fnPtrVar << ")(" << args.str()
		                   << ");\n";
	}
}

void CPPLoweringProvider::LoweringContext::visitNegate(ir::NegateOperation* negateOperation, short blockIndex,
                                                       RegisterFrame& frame) {
	auto input = frame.getValue(negateOperation->getInput()->getIdentifier());
	auto resultVar = getVariable(negateOperation->getIdentifier());
	if (!frame.contains(negateOperation->getIdentifier())) {
		blockArguments << getType(negateOperation->getStamp()) << " " << resultVar << ";\n";
		frame.setValue(negateOperation->getIdentifier(), resultVar);
	}
	blocks[blockIndex] << resultVar << "= ~" << input << ";\n";
}

void CPPLoweringProvider::LoweringContext::visitNot(ir::NotOperation* notOperation, short blockIndex,
                                                    RegisterFrame& frame) {
	auto input = frame.getValue(notOperation->getInput()->getIdentifier());
	auto resultVar = getVariable(notOperation->getIdentifier());
	if (!frame.contains(notOperation->getIdentifier())) {
		blockArguments << getType(notOperation->getStamp()) << " " << resultVar << ";\n";
		frame.setValue(notOperation->getIdentifier(), resultVar);
	}
	blocks[blockIndex] << resultVar << "= !" << input << ";\n";
}

void CPPLoweringProvider::LoweringContext::visitCast(ir::CastOperation* castOp, short blockIndex,
                                                     RegisterFrame& frame) {
	auto input = frame.getValue(castOp->getInput()->getIdentifier());
	auto var = getVariable(castOp->getIdentifier());
	auto inputStamp = castOp->getInput()->getStamp();
	auto outputStamp = castOp->getStamp();
	auto targetType = getType(outputStamp);
	if (!frame.contains(castOp->getIdentifier())) {
		blockArguments << targetType << " " << var << ";\n";
		frame.setValue(castOp->getIdentifier(), var);
	}
	bool ptrToArith = (inputStamp == Type::ptr && outputStamp != Type::ptr);
	bool arithToPtr = (inputStamp != Type::ptr && outputStamp == Type::ptr);
	if (ptrToArith || arithToPtr) {
		blocks[blockIndex] << var << " = (" << targetType << ")(uintptr_t)" << input << ";\n";
	} else {
		blocks[blockIndex] << var << " = (" << targetType << ")" << input << ";\n";
	}
}

void CPPLoweringProvider::LoweringContext::visitSelect(ir::SelectOperation* selectOp, short blockIndex,
                                                       RegisterFrame& frame) {
	auto condition = frame.getValue(selectOp->getCondition()->getIdentifier());
	auto trueValue = frame.getValue(selectOp->getTrueValue()->getIdentifier());
	auto falseValue = frame.getValue(selectOp->getFalseValue()->getIdentifier());
	auto resultVar = getVariable(selectOp->getIdentifier());
	if (!frame.contains(selectOp->getIdentifier())) {
		blockArguments << getType(selectOp->getStamp()) << " " << resultVar << ";\n";
		frame.setValue(selectOp->getIdentifier(), resultVar);
	}
	blocks[blockIndex] << resultVar << " = " << condition << " ? " << trueValue << " : " << falseValue << ";\n";
}

void CPPLoweringProvider::LoweringContext::visitAlloca(ir::AllocaOperation* allocaOp, short blockIndex,
                                                       RegisterFrame& frame) {
	auto resultVar = getVariable(allocaOp->getIdentifier());
	auto bufVar = "alloca_buf_" + allocaOp->getIdentifier().toString();
	if (!frame.contains(allocaOp->getIdentifier())) {
		blockArguments << "alignas(8) uint8_t " << bufVar << "[" << allocaOp->getSize() << "];\n";
		blockArguments << "uint8_t* " << resultVar << ";\n";
		frame.setValue(allocaOp->getIdentifier(), resultVar);
	}
	blocks[blockIndex] << resultVar << " = " << bufVar << ";\n";
}

void CPPLoweringProvider::LoweringContext::visitFunctionAddressOf(ir::FunctionAddressOfOperation* funcAddrOp,
                                                                  short blockIndex, RegisterFrame& frame) {
	auto resultVar = getVariable(funcAddrOp->getIdentifier());
	if (!frame.contains(funcAddrOp->getIdentifier())) {
		blockArguments << "uint8_t* " << resultVar << ";\n";
		frame.setValue(funcAddrOp->getIdentifier(), resultVar);
	}
	bool isInternalFunction = ir->getFunctionOperation(funcAddrOp->getFunctionName()) != nullptr;
	if (isInternalFunction) {
		blocks[blockIndex] << resultVar << " = (uint8_t*)&" << funcAddrOp->getFunctionName() << ";\n";
	} else {
		if (!functionNames.contains(funcAddrOp->getFunctionSymbol())) {
			functions << "auto f_" << funcAddrOp->getFunctionSymbol() << " = (void*)" << funcAddrOp->getFunctionPtr()
			          << ";\n";
			functionNames.emplace(funcAddrOp->getFunctionSymbol());
		}
		blocks[blockIndex] << resultVar << " = (uint8_t*)f_" << funcAddrOp->getFunctionSymbol() << ";\n";
	}
}

std::string CPPLoweringProvider::LoweringContext::getVariable(const ir::OperationIdentifier& id) {
	return "var_" + id.toString();
}

} // namespace nautilus::compiler::cpp


#include "nautilus/compiler/ir/operations/ConstPtrOperation.hpp"
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
}

std::stringstream CPPLoweringProvider::LoweringContext::process() {

	auto functionOperation = ir->getRootOperation();
	RegisterFrame rootFrame;
	std::vector<std::string> arguments;
	auto functionBasicBlock = functionOperation->getFunctionBasicBlock();
	for (auto i = 0ull; i < functionBasicBlock->getArguments().size(); i++) {
		auto argument = functionBasicBlock->getArguments()[i].get();
		auto var = getVariable(argument->getIdentifier());
		rootFrame.setValue(argument->getIdentifier(), var);
		arguments.emplace_back(getType(argument->getStamp()) + " " + var);
	}
	this->process(functionBasicBlock, rootFrame);

	std::stringstream pipelineCode;
	pipelineCode << "\n";
	pipelineCode << "#include <cstdint>";
	pipelineCode << "\n";
	pipelineCode << "extern \"C\" " << returnType << " execute(";
	for (size_t i = 0; i < arguments.size(); i++) {
		if (i != 0) {
			pipelineCode << ",";
		}
		pipelineCode << arguments[i] << " ";
	}
	pipelineCode << "){\n";
	pipelineCode << "//variable declarations\n";
	pipelineCode << blockArguments.str();
	pipelineCode << "//function definitions\n";
	pipelineCode << functions.str();
	pipelineCode << "//basic blocks\n";
	for (auto& block : blocks) {
		pipelineCode << block.str();
		pipelineCode << "\n";
	}
	pipelineCode << "}\n";

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
		auto blockName = "Block_" + block->getIdentifier();
		short blockIndex = blocks.size();
		auto& currentBlock = blocks.emplace_back();
		currentBlock << blockName << ":\n";
		activeBlocks.emplace(block->getIdentifier(), blockName);
		for (auto& opt : block->getOperations()) {
			this->process(opt, blockIndex, frame);
		}
		return blockName;
	} else {
		return entry->second;
	}
}

void CPPLoweringProvider::LoweringContext::process(ir::CompareOperation* cmpOp, short blockIndex, RegisterFrame& frame) {
	auto leftInput = frame.getValue(cmpOp->getLeftInput()->getIdentifier());
	auto rightInput = frame.getValue(cmpOp->getRightInput()->getIdentifier());
	auto resultVar = getVariable(cmpOp->getIdentifier());
	blockArguments << getType(cmpOp->getStamp()) << " " << resultVar << ";\n";
	frame.setValue(cmpOp->getIdentifier(), resultVar);

	// we have to handle the special case that we want to do a null check.
	// Currently, Nautilus IR just contains a x == 0, thus we check if x is a ptr
	// type.
	if (cmpOp->isEquals() && cmpOp->getLeftInput()->getStamp() == Type::ptr && isInteger(cmpOp->getRightInput()->getStamp())) {
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

void CPPLoweringProvider::LoweringContext::process(ir::LoadOperation* loadOp, short blockIndex, RegisterFrame& frame) {
	auto address = frame.getValue(loadOp->getAddress()->getIdentifier());
	auto resultVar = getVariable(loadOp->getIdentifier());
	blockArguments << getType(loadOp->getStamp()) << " " << resultVar << ";\n";
	frame.setValue(loadOp->getIdentifier(), resultVar);
	auto type = getType(loadOp->getStamp());
	blocks[blockIndex] << resultVar << " = *reinterpret_cast<" << type << "*>(" << address << ");\n";
}

void CPPLoweringProvider::LoweringContext::process(ir::StoreOperation* storeOp, short blockIndex, RegisterFrame& frame) {
	auto address = frame.getValue(storeOp->getAddress()->getIdentifier());
	auto value = frame.getValue(storeOp->getValue()->getIdentifier());
	auto type = getType(storeOp->getValue()->getStamp());
	blocks[blockIndex] << "*reinterpret_cast<" << type << "*>(" << address << ") = " << value << ";\n";
}

void CPPLoweringProvider::LoweringContext::process(ir::BasicBlockInvocation& bi, short blockIndex, RegisterFrame& parentFrame) {
	auto blockInputArguments = bi.getArguments();
	auto& blockTargetArguments = bi.getBlock()->getArguments();
	blocks[blockIndex] << "// prepare block arguments\n";
	blocks[blockIndex] << "{\n";
	for (uint64_t i = 0; i < blockInputArguments.size(); i++) {
		auto blockArgument = blockInputArguments[i]->getIdentifier();
		blocks[blockIndex] << getType(blockTargetArguments[i]->getStamp()) << " temp_" << i << " = " << parentFrame.getValue(blockArgument) << ";\n";
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

void CPPLoweringProvider::LoweringContext::process(ir::IfOperation* ifOpt, short blockIndex, RegisterFrame& frame) {
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

void CPPLoweringProvider::LoweringContext::process(ir::BranchOperation* branchOp, short blockIndex, RegisterFrame& frame) {
	process(branchOp->getNextBlockInvocation(), blockIndex, frame);
	auto nextBlock = process(branchOp->getNextBlockInvocation().getBlock(), frame);
	blocks[blockIndex] << "goto " << nextBlock << ";\n";
}

void CPPLoweringProvider::LoweringContext::process(const std::unique_ptr<ir::Operation>& opt, short blockIndex, RegisterFrame& frame) {
	switch (opt->getOperationType()) {
	case ir::Operation::OperationType::ConstBooleanOp: {
		processConst<ir::ConstBooleanOperation>(opt, blockIndex, frame);
		return;
	}
	case ir::Operation::OperationType::ConstIntOp: {
		processConst<ir::ConstIntOperation>(opt, blockIndex, frame);
		return;
	}
	case ir::Operation::OperationType::ConstFloatOp: {
		processConst<ir::ConstFloatOperation>(opt, blockIndex, frame);
		return;
	}
	case ir::Operation::OperationType::ConstPtrOp: {
		processConst<ir::ConstPtrOperation>(opt, blockIndex, frame);
		return;
	}
	case ir::Operation::OperationType::AddOp: {
		processBinary<ir::AndOperation>(opt, "+", blockIndex, frame);
		return;
	}
	case ir::Operation::OperationType::MulOp: {
		processBinary<ir::MulOperation>(opt, "*", blockIndex, frame);
		return;
	}
	case ir::Operation::OperationType::SubOp: {
		processBinary<ir::SubOperation>(opt, "-", blockIndex, frame);
		return;
	}
	case ir::Operation::OperationType::DivOp: {
		processBinary<ir::DivOperation>(opt, "/", blockIndex, frame);
		return;
	}
	case ir::Operation::OperationType::ModOp: {
		processBinary<ir::ModOperation>(opt, "%", blockIndex, frame);
		return;
	}
	case ir::Operation::OperationType::ShiftOp: {
		auto op = static_cast<ir::ShiftOperation*>(opt.get());
		if (op->getType() == ir::ShiftOperation::LS) {
			processBinary<ir::ShiftOperation>(opt, "<<", blockIndex, frame);
		} else {
			processBinary<ir::ShiftOperation>(opt, ">>", blockIndex, frame);
		}
		return;
	}
	case ir::Operation::OperationType::BinaryComp: {
		auto op = static_cast<ir::BinaryCompOperation*>(opt.get());
		switch (op->getType()) {
		case ir::BinaryCompOperation::BAND:
			processBinary<ir::BinaryCompOperation>(opt, "&", blockIndex, frame);
			break;
		case ir::BinaryCompOperation::BOR:
			processBinary<ir::BinaryCompOperation>(opt, "|", blockIndex, frame);
			break;
		case ir::BinaryCompOperation::XOR:
			processBinary<ir::BinaryCompOperation>(opt, "^", blockIndex, frame);
			break;
		}
		return;
	}
	case ir::Operation::OperationType::ReturnOp: {
		auto returnOpt = static_cast<ir::ReturnOperation*>(opt.get());
		if (returnOpt->hasReturnValue()) {
			auto returnFOp = frame.getValue(returnOpt->getReturnValue()->getIdentifier());
			blocks[blockIndex] << "return " << returnFOp << ";\n";

			this->returnType = getType(returnOpt->getStamp());
		} else {
			blocks[blockIndex] << "return;\n";
			this->returnType = "void";
		}
		return;
	}
	case ir::Operation::OperationType::CompareOp: {
		auto compOpt = as<ir::CompareOperation>(opt);
		process(compOpt, blockIndex, frame);
		return;
	}
	case ir::Operation::OperationType::IfOp: {
		auto ifOpt = as<ir::IfOperation>(opt);
		process(ifOpt, blockIndex, frame);
		return;
	}

	case ir::Operation::OperationType::BranchOp: {
		auto branchOp = as<ir::BranchOperation>(opt);
		process(branchOp, blockIndex, frame);
		return;
	}
	case ir::Operation::OperationType::LoadOp: {
		auto load = as<ir::LoadOperation>(opt);
		process(load, blockIndex, frame);
		return;
	}
	case ir::Operation::OperationType::StoreOp: {
		auto store = as<ir::StoreOperation>(opt);
		process(store, blockIndex, frame);
		return;
	}
	case ir::Operation::OperationType::ProxyCallOp: {
		auto call = as<ir::ProxyCallOperation>(opt);
		process(call, blockIndex, frame);
		return;
	}
	case ir::Operation::OperationType::OrOp: {
		processBinary<ir::OrOperation>(opt, "||", blockIndex, frame);
		return;
	}
	case ir::Operation::OperationType::AndOp: {
		processBinary<ir::AndOperation>(opt, "&&", blockIndex, frame);
		return;
	}
	case ir::Operation::OperationType::NegateOp: {
		auto call = as<ir::NegateOperation>(opt);
		process(call, blockIndex, frame);
		return;
	}
	case ir::Operation::OperationType::NotOp: {
		auto call = as<ir::NotOperation>(opt);
		process(call, blockIndex, frame);
		return;
	}
	case ir::Operation::OperationType::CastOp: {
		auto cast = as<ir::CastOperation>(opt);
		process(cast, blockIndex, frame);
		return;
	}
	default: {
		throw NotImplementedException("Operation is not implemented:" + opt->toString());
		return;
	}
	}
}

void CPPLoweringProvider::LoweringContext::process(ir::ProxyCallOperation* opt, short blockIndex, RegisterFrame& frame) {

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
	if (!functionNames.contains(opt->getFunctionSymbol())) {
		functions << "auto f_" << opt->getFunctionSymbol() << " = " << "(" << returnType << "(*)(" << argTypes.str() << "))" << opt->getFunctionPtr() << ";\n";
		functionNames.emplace(opt->getFunctionSymbol());
	}
	if (opt->getStamp() != Type::v) {
		auto resultVar = getVariable(opt->getIdentifier());
		blockArguments << getType(opt->getStamp()) << " " << resultVar << ";\n";
		frame.setValue(opt->getIdentifier(), resultVar);
		blocks[blockIndex] << resultVar << " = ";
	}
	blocks[blockIndex] << "f_" << opt->getFunctionSymbol() << "(" << args.str() << ");\n";
}

void CPPLoweringProvider::LoweringContext::process(ir::NegateOperation* negateOperation, short blockIndex, RegisterFrame& frame) {
	auto input = frame.getValue(negateOperation->getInput()->getIdentifier());
	auto resultVar = getVariable(negateOperation->getIdentifier());
	blockArguments << getType(negateOperation->getStamp()) << " " << resultVar << ";\n";
	frame.setValue(negateOperation->getIdentifier(), resultVar);
	blocks[blockIndex] << resultVar << "= ~" << input << ";\n";
}

void CPPLoweringProvider::LoweringContext::process(ir::NotOperation* notOperation, short blockIndex, RegisterFrame& frame) {
	auto input = frame.getValue(notOperation->getInput()->getIdentifier());
	auto resultVar = getVariable(notOperation->getIdentifier());
	blockArguments << getType(notOperation->getStamp()) << " " << resultVar << ";\n";
	frame.setValue(notOperation->getIdentifier(), resultVar);
	blocks[blockIndex] << resultVar << "= !" << input << ";\n";
}

void CPPLoweringProvider::LoweringContext::process(ir::CastOperation* castOp, short blockIndex, RegisterFrame& frame) {
	auto input = frame.getValue(castOp->getInput()->getIdentifier());
	auto var = getVariable(castOp->getIdentifier());
	frame.setValue(castOp->getIdentifier(), var);
	auto targetType = getType(castOp->getStamp());
	blockArguments << targetType << " " << var << ";\n";
	blocks[blockIndex] << var << " = (" << targetType << ")" << input << ";\n";
}

std::string CPPLoweringProvider::LoweringContext::getVariable(const ir::OperationIdentifier& id) {
	return "var_" + id.toString();
}

} // namespace nautilus::compiler::cpp

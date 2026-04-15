

#include "nautilus/compiler/backends/bc/BCLoweringProvider.hpp"
#include "nautilus/compiler/backends/bc/ByteCode.hpp"
#include "nautilus/compiler/ir/operations/Operation.hpp"
#include "nautilus/exceptions/NotImplementedException.hpp"
#include <utility>

namespace nautilus::compiler::bc {

BCLoweringProvider::BCLoweringProvider() {
}

BCLoweringProvider::LoweringContext::LoweringContext(std::shared_ptr<ir::IRGraph> ir, std::string targetFunctionName)
    : program(), defaultRegisterFile(), ir(std::move(ir)), internalFunctionPtrs(),
      targetFunctionName(std::move(targetFunctionName)), registerProvider(), activeBlocks(), usageCounts(),
      functionArgs() {
}

BCLoweringProvider::LoweringContext::LoweringContext(std::shared_ptr<ir::IRGraph> ir,
                                                     const std::unordered_map<std::string, void*>& internalFunctionPtrs)
    : program(), defaultRegisterFile(), ir(std::move(ir)), internalFunctionPtrs(internalFunctionPtrs),
      targetFunctionName("execute"), registerProvider(), activeBlocks(), usageCounts(), functionArgs() {
}

BCLoweringProvider::LoweringContext::LoweringContext(std::shared_ptr<ir::IRGraph> ir, std::string targetFunctionName,
                                                     const std::unordered_map<std::string, void*>& internalFunctionPtrs)
    : program(), defaultRegisterFile(), ir(std::move(ir)), internalFunctionPtrs(internalFunctionPtrs),
      targetFunctionName(std::move(targetFunctionName)), registerProvider(), activeBlocks(), usageCounts(),
      functionArgs() {
}

std::tuple<Code, RegisterFile> BCLoweringProvider::lower(std::shared_ptr<ir::IRGraph> ir) {
	auto ctx = LoweringContext(ir);
	return ctx.process();
}

std::tuple<Code, RegisterFile>
BCLoweringProvider::lower(std::shared_ptr<ir::IRGraph> ir,
                          const std::unordered_map<std::string, void*>& internalFunctionPtrs) {
	auto ctx = LoweringContext(ir, internalFunctionPtrs);
	return ctx.process();
}

std::tuple<Code, RegisterFile> BCLoweringProvider::lowerFunction(std::shared_ptr<ir::IRGraph> ir,
                                                                 const std::string& functionName) {
	auto ctx = LoweringContext(std::move(ir), functionName);
	return ctx.process();
}

std::tuple<Code, RegisterFile>
BCLoweringProvider::lower(std::shared_ptr<ir::IRGraph> ir, const std::string& functionName,
                          const std::unordered_map<std::string, void*>& internalFunctionPtrs) {
	auto ctx = LoweringContext(std::move(ir), std::string(functionName), internalFunctionPtrs);
	return ctx.process();
}

short BCLoweringProvider::RegisterProvider::allocRegister() {
	// Reuse freed registers if available
	if (!freeList.empty()) {
		short reg = freeList.back();
		freeList.pop_back();
		return reg;
	}
	// Otherwise allocate a new register
	return currentRegister++;
}

void BCLoweringProvider::RegisterProvider::freeRegister(short reg) {
	freeList.push_back(reg);
}

std::tuple<Code, RegisterFile> BCLoweringProvider::LoweringContext::process() {
	// BC interpreter only processes a single function (the main "execute" function)
	// Get all functions and process the first one (typically "execute")
	const auto& functionOperations = ir->getFunctionOperations();
	if (functionOperations.empty()) {
		throw NotImplementedException("No functions found in IR graph");
	}

	// Find the target function by name, or use the first function as fallback
	const ir::FunctionOperation* targetFunction = nullptr;
	for (auto* funcOp : functionOperations) {
		if (funcOp->getName() == targetFunctionName) {
			targetFunction = funcOp;
			break;
		}
	}
	if (targetFunction == nullptr) {
		targetFunction = functionOperations.front();
	}

	RegisterFrame rootFrame;
	const auto& functionBasicBlock = targetFunction->getFunctionBasicBlock();
	for (auto i = 0ull; i < functionBasicBlock.getArguments().size(); i++) {
		auto* argument = functionBasicBlock.getArguments()[i];
		auto argumentRegister = registerProvider.allocRegister();
		rootFrame.setValue(argument->getIdentifier(), argumentRegister);
		program.arguments.emplace_back(argumentRegister);
		// Mark function arguments so they're never freed
		functionArgs.insert(argument->getIdentifier());
	}
	this->process(&functionBasicBlock, rootFrame);
	// Resize register file to actual number of registers used
	defaultRegisterFile.resize(registerProvider.getRegisterCount(), 0);
	return std::make_tuple(program, defaultRegisterFile);
}

short BCLoweringProvider::LoweringContext::process(const ir::BasicBlock* block, RegisterFrame& frame) {
	// assume that all argument registers are correctly set
	auto entry = activeBlocks.find(block->getIdentifier());
	if (entry == activeBlocks.end()) {
		short blockIndex = program.blocks.size();
		activeBlocks.emplace(block->getIdentifier(), blockIndex);
		// Count usages in this block for register reuse
		countUsages(block);
		// create bytecode block;
		program.blocks.emplace_back();
		for (auto* opt : block->getOperations()) {
			this->dispatch(opt, blockIndex, frame);
		}
		return blockIndex;
	} else {
		return entry->second;
	}
}

void BCLoweringProvider::LoweringContext::visitAnd(ir::AndOperation* addOpt, short block, RegisterFrame& frame) {
	auto leftInput = frame.getValue(addOpt->getLeftInput()->getIdentifier());
	auto rightInput = frame.getValue(addOpt->getRightInput()->getIdentifier());
	auto resultReg = getResultRegister(addOpt, frame);
	frame.setValue(addOpt->getIdentifier(), resultReg);
	ByteCode bc = ByteCode::AND_b;
	OpCode oc = {bc, leftInput, rightInput, resultReg};
	program.blocks[block].code.emplace_back(oc);
}

void BCLoweringProvider::LoweringContext::visitOr(ir::OrOperation* addOpt, short block, RegisterFrame& frame) {
	auto leftInput = frame.getValue(addOpt->getLeftInput()->getIdentifier());
	auto rightInput = frame.getValue(addOpt->getRightInput()->getIdentifier());
	auto resultReg = getResultRegister(addOpt, frame);
	frame.setValue(addOpt->getIdentifier(), resultReg);
	ByteCode bc = ByteCode::OR_b;
	OpCode oc = {bc, leftInput, rightInput, resultReg};
	program.blocks[block].code.emplace_back(oc);
}

void BCLoweringProvider::LoweringContext::visitAdd(ir::AddOperation* addOpt, short block, RegisterFrame& frame) {
	auto leftInput = frame.getValue(addOpt->getLeftInput()->getIdentifier());
	auto rightInput = frame.getValue(addOpt->getRightInput()->getIdentifier());
	auto resultReg = getResultRegister(addOpt, frame);
	frame.setValue(addOpt->getIdentifier(), resultReg);
	auto type = addOpt->getStamp();
	ByteCode bc;
	switch (type) {
	case Type::i8:
		bc = ByteCode::ADD_i8;
		break;
	case Type::i16:
		bc = ByteCode::ADD_i16;
		break;
	case Type::i32:
		bc = ByteCode::ADD_i32;
		break;
	case Type::i64:
		bc = ByteCode::ADD_i64;
		break;
	case Type::ui8:
		bc = ByteCode::ADD_ui8;
		break;
	case Type::ui16:
		bc = ByteCode::ADD_ui16;
		break;
	case Type::ui32:
		bc = ByteCode::ADD_ui32;
		break;
	case Type::ui64:
		bc = ByteCode::ADD_ui64;
		break;
	case Type::f64:
		bc = ByteCode::ADD_d;
		break;
	case Type::f32:
		bc = ByteCode::ADD_f;
		break;
	case Type::ptr:
		bc = ByteCode::ADD_i64;
		break;
	default: {
		throw NotImplementedException("This type is not supported.");
	}
	}
	OpCode oc = {bc, leftInput, rightInput, resultReg};
	program.blocks[block].code.emplace_back(oc);
}

void BCLoweringProvider::LoweringContext::visitSub(ir::SubOperation* subOpt, short block, RegisterFrame& frame) {
	auto leftInput = frame.getValue(subOpt->getLeftInput()->getIdentifier());
	auto rightInput = frame.getValue(subOpt->getRightInput()->getIdentifier());
	auto resultReg = getResultRegister(subOpt, frame);
	frame.setValue(subOpt->getIdentifier(), resultReg);
	ByteCode bc;
	switch ((subOpt->getStamp())) {
	case Type::i8:
		bc = ByteCode::SUB_i8;
		break;
	case Type::i16:
		bc = ByteCode::SUB_i16;
		break;
	case Type::i32:
		bc = ByteCode::SUB_i32;
		break;
	case Type::i64:
		bc = ByteCode::SUB_i64;
		break;
	case Type::ui8:
		bc = ByteCode::SUB_ui8;
		break;
	case Type::ui16:
		bc = ByteCode::SUB_ui16;
		break;
	case Type::ui32:
		bc = ByteCode::SUB_ui32;
		break;
	case Type::ui64:
		bc = ByteCode::SUB_ui64;
		break;
	case Type::f64:
		bc = ByteCode::SUB_d;
		break;
	case Type::f32:
		bc = ByteCode::SUB_f;
		break;
	case Type::ptr:
		bc = ByteCode::SUB_i64;
		break;
	default: {
		throw NotImplementedException("This type is not supported.");
	}
	}
	OpCode oc = {bc, leftInput, rightInput, resultReg};
	program.blocks[block].code.emplace_back(oc);
}

void BCLoweringProvider::LoweringContext::visitMul(ir::MulOperation* mulOpt, short block, RegisterFrame& frame) {
	auto leftInput = frame.getValue(mulOpt->getLeftInput()->getIdentifier());
	auto rightInput = frame.getValue(mulOpt->getRightInput()->getIdentifier());
	auto resultReg = getResultRegister(mulOpt, frame);
	frame.setValue(mulOpt->getIdentifier(), resultReg);

	ByteCode bc;
	switch ((mulOpt->getStamp())) {
	case Type::i8:
		bc = ByteCode::MUL_i8;
		break;
	case Type::i16:
		bc = ByteCode::MUL_i16;
		break;
	case Type::i32:
		bc = ByteCode::MUL_i32;
		break;
	case Type::i64:
		bc = ByteCode::MUL_i64;
		break;
	case Type::ui8:
		bc = ByteCode::MUL_ui8;
		break;
	case Type::ui16:
		bc = ByteCode::MUL_ui16;
		break;
	case Type::ui32:
		bc = ByteCode::MUL_ui32;
		break;
	case Type::ui64:
		bc = ByteCode::MUL_ui64;
		break;
	case Type::f64:
		bc = ByteCode::MUL_d;
		break;
	case Type::f32:
		bc = ByteCode::MUL_f;
		break;
	case Type::ptr:
		bc = ByteCode::MUL_i64;
		break;
	default: {
		throw NotImplementedException("This type is not supported.");
	}
	}
	OpCode oc = {bc, leftInput, rightInput, resultReg};
	program.blocks[block].code.emplace_back(oc);
}

void BCLoweringProvider::LoweringContext::visitDiv(ir::DivOperation* divOp, short block, RegisterFrame& frame) {
	auto leftInput = frame.getValue(divOp->getLeftInput()->getIdentifier());
	auto rightInput = frame.getValue(divOp->getRightInput()->getIdentifier());
	auto resultReg = getResultRegister(divOp, frame);
	frame.setValue(divOp->getIdentifier(), resultReg);
	ByteCode bc;
	switch ((divOp->getStamp())) {
	case Type::i8:
		bc = ByteCode::DIV_i8;
		break;
	case Type::i16:
		bc = ByteCode::DIV_i16;
		break;
	case Type::i32:
		bc = ByteCode::DIV_i32;
		break;
	case Type::i64:
		bc = ByteCode::DIV_i64;
		break;
	case Type::ui8:
		bc = ByteCode::DIV_ui8;
		break;
	case Type::ui16:
		bc = ByteCode::DIV_ui16;
		break;
	case Type::ui32:
		bc = ByteCode::DIV_ui32;
		break;
	case Type::ui64:
		bc = ByteCode::DIV_ui64;
		break;
	case Type::f64:
		bc = ByteCode::DIV_d;
		break;
	case Type::f32:
		bc = ByteCode::DIV_f;
		break;
	case Type::ptr:
		bc = ByteCode::DIV_i64;
		break;
	default: {
		throw NotImplementedException("This type is not supported.");
	}
	}
	OpCode oc = {bc, leftInput, rightInput, resultReg};
	program.blocks[block].code.emplace_back(oc);
}

void BCLoweringProvider::LoweringContext::visitMod(ir::ModOperation* divOp, short block, RegisterFrame& frame) {
	auto leftInput = frame.getValue(divOp->getLeftInput()->getIdentifier());
	auto rightInput = frame.getValue(divOp->getRightInput()->getIdentifier());
	auto resultReg = getResultRegister(divOp, frame);
	frame.setValue(divOp->getIdentifier(), resultReg);
	ByteCode bc;
	switch ((divOp->getStamp())) {
	case Type::i8:
		bc = ByteCode::MOD_i8;
		break;
	case Type::i16:
		bc = ByteCode::MOD_i16;
		break;
	case Type::i32:
		bc = ByteCode::MOD_i32;
		break;
	case Type::i64:
		bc = ByteCode::MOD_i64;
		break;
	case Type::ui8:
		bc = ByteCode::MOD_ui8;
		break;
	case Type::ui16:
		bc = ByteCode::MOD_ui16;
		break;
	case Type::ui32:
		bc = ByteCode::MOD_ui32;
		break;
	case Type::ui64:
		bc = ByteCode::MOD_ui64;
		break;
	default: {
		throw NotImplementedException("This type is not supported.");
	}
	}
	OpCode oc = {bc, leftInput, rightInput, resultReg};
	program.blocks[block].code.emplace_back(oc);
}

void BCLoweringProvider::LoweringContext::visitCompare(ir::CompareOperation* cmpOp, short block, RegisterFrame& frame) {
	auto leftInput = frame.getValue(cmpOp->getLeftInput()->getIdentifier());
	auto rightInput = frame.getValue(cmpOp->getRightInput()->getIdentifier());
	auto resultReg = getResultRegister(cmpOp, frame);
	frame.setValue(cmpOp->getIdentifier(), resultReg);

	if (cmpOp->getComparator() == ir::CompareOperation::Comparator::EQ) {
		auto type = cmpOp->getLeftInput()->getStamp();
		ByteCode bc;
		switch ((type)) {
		case Type::b:
			bc = ByteCode::EQ_b;
			break;
		case Type::i8:
			bc = ByteCode::EQ_i8;
			break;
		case Type::i16:
			bc = ByteCode::EQ_i16;
			break;
		case Type::i32:
			bc = ByteCode::EQ_i32;
			break;
		case Type::i64:
			bc = ByteCode::EQ_i64;
			break;
		case Type::ui8:
			bc = ByteCode::EQ_ui8;
			break;
		case Type::ui16:
			bc = ByteCode::EQ_ui16;
			break;
		case Type::ui32:
			bc = ByteCode::EQ_ui32;
			break;
		case Type::ui64:
			bc = ByteCode::EQ_ui64;
			break;
		case Type::f64:
			bc = ByteCode::EQ_d;
			break;
		case Type::f32:
			bc = ByteCode::EQ_f;
			break;
		case Type::ptr:
			bc = ByteCode::EQ_i64;
			break;
		default: {
			throw NotImplementedException("This type is not supported.");
		}
		}
		OpCode oc = {bc, leftInput, rightInput, resultReg};
		program.blocks[block].code.emplace_back(oc);
	} else if (cmpOp->getComparator() == ir::CompareOperation::Comparator::LT) {
		auto type = cmpOp->getLeftInput()->getStamp();
		ByteCode bc;
		switch ((type)) {
		case Type::i8:
			bc = ByteCode::LESS_THAN_i8;
			break;
		case Type::i16:
			bc = ByteCode::LESS_THAN_i16;
			break;
		case Type::i32:
			bc = ByteCode::LESS_THAN_i32;
			break;
		case Type::i64:
			bc = ByteCode::LESS_THAN_i64;
			break;
		case Type::ui8:
			bc = ByteCode::LESS_THAN_ui8;
			break;
		case Type::ui16:
			bc = ByteCode::LESS_THAN_ui16;
			break;
		case Type::ui32:
			bc = ByteCode::LESS_THAN_ui32;
			break;
		case Type::ui64:
			bc = ByteCode::LESS_THAN_ui64;
			break;
		case Type::f64:
			bc = ByteCode::LESS_THAN_d;
			break;
		case Type::f32:
			bc = ByteCode::LESS_THAN_f;
			break;
		case Type::ptr:
			bc = ByteCode::LESS_THAN_i64;
			break;
		default: {
			throw NotImplementedException("This type is not supported.");
		}
		}
		OpCode oc = {bc, leftInput, rightInput, resultReg};
		program.blocks[block].code.emplace_back(oc);
	} else if (cmpOp->getComparator() == ir::CompareOperation::Comparator::LE) {
		auto type = cmpOp->getLeftInput()->getStamp();
		ByteCode bc;
		switch ((type)) {
		case Type::i8:
			bc = ByteCode::LESS_THAN_EQUALS_i8;
			break;
		case Type::i16:
			bc = ByteCode::LESS_THAN_EQUALS_i16;
			break;
		case Type::i32:
			bc = ByteCode::LESS_THAN_EQUALS_i32;
			break;
		case Type::i64:
			bc = ByteCode::LESS_THAN_EQUALS_i64;
			break;
		case Type::ui8:
			bc = ByteCode::LESS_THAN_EQUALS_ui8;
			break;
		case Type::ui16:
			bc = ByteCode::LESS_THAN_EQUALS_ui16;
			break;
		case Type::ui32:
			bc = ByteCode::LESS_THAN_EQUALS_ui32;
			break;
		case Type::ui64:
			bc = ByteCode::LESS_THAN_EQUALS_ui64;
			break;
		case Type::f64:
			bc = ByteCode::LESS_THAN_EQUALS_d;
			break;
		case Type::f32:
			bc = ByteCode::LESS_THAN_EQUALS_f;
			break;
		case Type::ptr:
			bc = ByteCode::LESS_THAN_EQUALS_i64;
			break;
		default: {
			throw NotImplementedException("This type is not supported.");
		}
		}
		OpCode oc = {bc, leftInput, rightInput, resultReg};
		program.blocks[block].code.emplace_back(oc);
	} else if (cmpOp->getComparator() == ir::CompareOperation::Comparator::GT) {
		ByteCode bc;
		auto type = cmpOp->getLeftInput()->getStamp();
		switch ((type)) {
		case Type::i8:
			bc = ByteCode::GREATER_THAN_i8;
			break;
		case Type::i16:
			bc = ByteCode::GREATER_THAN_i16;
			break;
		case Type::i32:
			bc = ByteCode::GREATER_THAN_i32;
			break;
		case Type::i64:
			bc = ByteCode::GREATER_THAN_i64;
			break;
		case Type::ui8:
			bc = ByteCode::GREATER_THAN_ui8;
			break;
		case Type::ui16:
			bc = ByteCode::GREATER_THAN_ui16;
			break;
		case Type::ui32:
			bc = ByteCode::GREATER_THAN_ui32;
			break;
		case Type::ui64:
			bc = ByteCode::GREATER_THAN_ui64;
			break;
		case Type::f64:
			bc = ByteCode::GREATER_THAN_d;
			break;
		case Type::f32:
			bc = ByteCode::GREATER_THAN_f;
			break;
		case Type::ptr:
			bc = ByteCode::GREATER_THAN_i64;
			break;
		default: {
			throw NotImplementedException("This type is not supported.");
		}
		}
		OpCode oc = {bc, leftInput, rightInput, resultReg};
		program.blocks[block].code.emplace_back(oc);
	} else if (cmpOp->getComparator() == ir::CompareOperation::Comparator::GE) {
		ByteCode bc;
		auto type = cmpOp->getLeftInput()->getStamp();
		switch ((type)) {
		case Type::i8:
			bc = ByteCode::GREATER_THAN_EQUALS_i8;
			break;
		case Type::i16:
			bc = ByteCode::GREATER_THAN_EQUALS_i16;
			break;
		case Type::i32:
			bc = ByteCode::GREATER_THAN_EQUALS_i32;
			break;
		case Type::i64:
			bc = ByteCode::GREATER_THAN_EQUALS_i64;
			break;
		case Type::ui8:
			bc = ByteCode::GREATER_THAN_EQUALS_ui8;
			break;
		case Type::ui16:
			bc = ByteCode::GREATER_THAN_EQUALS_ui16;
			break;
		case Type::ui32:
			bc = ByteCode::GREATER_THAN_EQUALS_ui32;
			break;
		case Type::ui64:
			bc = ByteCode::GREATER_THAN_EQUALS_ui64;
			break;
		case Type::f64:
			bc = ByteCode::GREATER_THAN_EQUALS_d;
			break;
		case Type::f32:
			bc = ByteCode::GREATER_THAN_EQUALS_f;
			break;
		case Type::ptr:
			bc = ByteCode::GREATER_THAN_EQUALS_i64;
			break;
		default: {
			throw NotImplementedException("This type is not supported.");
		}
		}
		OpCode oc = {bc, leftInput, rightInput, resultReg};
		program.blocks[block].code.emplace_back(oc);
	} else if (cmpOp->getComparator() == ir::CompareOperation::Comparator::NE) {
		ByteCode bc;
		auto type = cmpOp->getLeftInput()->getStamp();
		switch ((type)) {
		case Type::b:
			bc = ByteCode::NOT_EQUALS_b;
			break;
		case Type::i8:
			bc = ByteCode::NOT_EQUALS_i8;
			break;
		case Type::i16:
			bc = ByteCode::NOT_EQUALS_i16;
			break;
		case Type::i32:
			bc = ByteCode::NOT_EQUALS_i32;
			break;
		case Type::i64:
			bc = ByteCode::NOT_EQUALS_i64;
			break;
		case Type::ui8:
			bc = ByteCode::NOT_EQUALS_ui8;
			break;
		case Type::ui16:
			bc = ByteCode::NOT_EQUALS_ui16;
			break;
		case Type::ui32:
			bc = ByteCode::NOT_EQUALS_ui32;
			break;
		case Type::ui64:
			bc = ByteCode::NOT_EQUALS_ui64;
			break;
		case Type::f64:
			bc = ByteCode::NOT_EQUALS_d;
			break;
		case Type::f32:
			bc = ByteCode::NOT_EQUALS_f;
			break;
		case Type::ptr:
			bc = ByteCode::NOT_EQUALS_i64;
			break;
		default: {
			throw NotImplementedException("This type is not supported.");
		}
		}
		OpCode oc = {bc, leftInput, rightInput, resultReg};
		program.blocks[block].code.emplace_back(oc);
	} else if (cmpOp->getComparator() == ir::CompareOperation::Comparator::LT) {
		ByteCode bc;
		auto type = cmpOp->getLeftInput()->getStamp();
		switch ((type)) {
		case Type::f32:
			bc = ByteCode::LESS_THAN_f;
			break;
		case Type::f64:
			bc = ByteCode::LESS_THAN_d;
			break;
		default: {
			throw NotImplementedException("This type is not supported.");
		}
		}
		OpCode oc = {bc, leftInput, rightInput, resultReg};
		program.blocks[block].code.emplace_back(oc);
	} else if (cmpOp->getComparator() == ir::CompareOperation::Comparator::EQ) {
		ByteCode bc;
		auto type = cmpOp->getLeftInput()->getStamp();
		switch ((type)) {
		case Type::f32:
			bc = ByteCode::EQ_f;
			break;
		case Type::f64:
			bc = ByteCode::EQ_d;
			break;
		default: {
			throw NotImplementedException("This type is not supported.");
		}
		}
		OpCode oc = {bc, leftInput, rightInput, resultReg};
		program.blocks[block].code.emplace_back(oc);
	} else {
		throw NotImplementedException("This type is not supported.");
	}
}

void BCLoweringProvider::LoweringContext::visitLoad(ir::LoadOperation* loadOp, short block, RegisterFrame& frame) {
	auto address = frame.getValue(loadOp->getAddress()->getIdentifier());
	auto resultReg = getResultRegister(loadOp, frame);
	frame.setValue(loadOp->getIdentifier(), resultReg);
	auto type = loadOp->getStamp();
	ByteCode bc;
	switch ((type)) {
	case Type::i8:
		bc = ByteCode::LOAD_i8;
		break;
	case Type::i16:
		bc = ByteCode::LOAD_i16;
		break;
	case Type::i32:
		bc = ByteCode::LOAD_i32;
		break;
	case Type::i64:
		bc = ByteCode::LOAD_i64;
		break;
	case Type::ui8:
		bc = ByteCode::LOAD_ui8;
		break;
	case Type::ui16:
		bc = ByteCode::LOAD_ui16;
		break;
	case Type::ui32:
		bc = ByteCode::LOAD_ui32;
		break;
	case Type::ui64:
		bc = ByteCode::LOAD_ui64;
		break;
	case Type::f32:
		bc = ByteCode::LOAD_f;
		break;
	case Type::f64:
		bc = ByteCode::LOAD_d;
		break;
	case Type::ptr:
		bc = ByteCode::LOAD_i64;
		break;
	case Type::b:
		bc = ByteCode::LOAD_b;
		break;
	default: {
		throw NotImplementedException("This type is not supported.");
	}
	}

	OpCode oc = {bc, address, -1, resultReg};
	program.blocks[block].code.emplace_back(oc);
}

void BCLoweringProvider::LoweringContext::visitStore(ir::StoreOperation* storeOp, short block, RegisterFrame& frame) {
	auto addressReg = frame.getValue(storeOp->getAddress()->getIdentifier());
	auto valueReg = frame.getValue(storeOp->getValue()->getIdentifier());
	auto type = storeOp->getValue()->getStamp();
	ByteCode bc;
	switch ((type)) {
	case Type::i8:
		bc = ByteCode::STORE_i8;
		break;
	case Type::i16:
		bc = ByteCode::STORE_i16;
		break;
	case Type::i32:
		bc = ByteCode::STORE_i32;
		break;
	case Type::i64:
		bc = ByteCode::STORE_i64;
		break;
	case Type::ui8:
		bc = ByteCode::STORE_ui8;
		break;
	case Type::ui16:
		bc = ByteCode::STORE_ui16;
		break;
	case Type::ui32:
		bc = ByteCode::STORE_ui32;
		break;
	case Type::ui64:
		bc = ByteCode::STORE_ui64;
		break;
	case Type::f64:
		bc = ByteCode::STORE_d;
		break;
	case Type::f32:
		bc = ByteCode::STORE_f;
		break;
	case Type::ptr:
		bc = ByteCode::STORE_i64;
		break;
	case Type::b:
		bc = ByteCode::STORE_b;
		break;
	default: {
		throw NotImplementedException("This type is not supported.");
	}
	}
	OpCode oc = {bc, addressReg, valueReg, -1};
	program.blocks[block].code.emplace_back(oc);
}

void BCLoweringProvider::LoweringContext::visitBinaryComp(ir::BinaryCompOperation* binaryCompOperation, short block,
                                                          RegisterFrame& frame) {
	auto leftReg = frame.getValue(binaryCompOperation->getLeftInput()->getIdentifier());
	auto rightReg = frame.getValue(binaryCompOperation->getRightInput()->getIdentifier());
	auto resultReg = getResultRegister(binaryCompOperation, frame);
	frame.setValue(binaryCompOperation->getIdentifier(), resultReg);
	auto type = binaryCompOperation->getStamp();
	auto opType = binaryCompOperation->getType();
	ByteCode bc = ByteCode::BLSH_i8;
	switch ((type)) {
	case Type::i8:
		switch (opType) {
		case ir::BinaryCompOperation::BAND:
			bc = ByteCode::BAND_i8;
			break;
		case ir::BinaryCompOperation::BOR:
			bc = ByteCode::BOR_i8;
			break;
		case ir::BinaryCompOperation::XOR:
			bc = ByteCode::BXOR_i8;
			break;
		}
		break;
	case Type::i16:
		switch (opType) {
		case ir::BinaryCompOperation::BAND:
			bc = ByteCode::BAND_i16;
			break;
		case ir::BinaryCompOperation::BOR:
			bc = ByteCode::BOR_i16;
			break;
		case ir::BinaryCompOperation::XOR:
			bc = ByteCode::BXOR_i16;
			break;
		}
		break;
	case Type::i32:
		switch (opType) {
		case ir::BinaryCompOperation::BAND:
			bc = ByteCode::BAND_i32;
			break;
		case ir::BinaryCompOperation::BOR:
			bc = ByteCode::BOR_i32;
			break;
		case ir::BinaryCompOperation::XOR:
			bc = ByteCode::BXOR_i32;
			break;
		}
		break;
	case Type::i64:
		switch (opType) {
		case ir::BinaryCompOperation::BAND:
			bc = ByteCode::BAND_i64;
			break;
		case ir::BinaryCompOperation::BOR:
			bc = ByteCode::BOR_i64;
			break;
		case ir::BinaryCompOperation::XOR:
			bc = ByteCode::BXOR_i64;
			break;
		}
		break;
	case Type::ui8:
		switch (opType) {
		case ir::BinaryCompOperation::BAND:
			bc = ByteCode::BAND_ui8;
			break;
		case ir::BinaryCompOperation::BOR:
			bc = ByteCode::BOR_ui8;
			break;
		case ir::BinaryCompOperation::XOR:
			bc = ByteCode::BXOR_ui8;
			break;
		}
		break;
	case Type::ui16:
		switch (opType) {
		case ir::BinaryCompOperation::BAND:
			bc = ByteCode::BAND_ui16;
			break;
		case ir::BinaryCompOperation::BOR:
			bc = ByteCode::BOR_ui16;
			break;
		case ir::BinaryCompOperation::XOR:
			bc = ByteCode::BXOR_ui16;
			break;
		}
		break;
	case Type::ui32:
		switch (opType) {
		case ir::BinaryCompOperation::BAND:
			bc = ByteCode::BAND_ui32;
			break;
		case ir::BinaryCompOperation::BOR:
			bc = ByteCode::BOR_ui32;
			break;
		case ir::BinaryCompOperation::XOR:
			bc = ByteCode::BXOR_ui32;
			break;
		}
		break;
	case Type::ui64:
		switch (opType) {
		case ir::BinaryCompOperation::BAND:
			bc = ByteCode::BAND_ui64;
			break;
		case ir::BinaryCompOperation::BOR:
			bc = ByteCode::BOR_ui64;
			break;
		case ir::BinaryCompOperation::XOR:
			bc = ByteCode::BXOR_ui64;
			break;
		}
		break;
	default: {
		throw NotImplementedException("This type is not supported.");
	}
	}

	OpCode oc = {bc, leftReg, rightReg, resultReg};
	program.blocks[block].code.emplace_back(oc);
}

void BCLoweringProvider::LoweringContext::visitShift(ir::ShiftOperation* shiftOperation, short block,
                                                     RegisterFrame& frame) {
	auto leftReg = frame.getValue(shiftOperation->getLeftInput()->getIdentifier());
	auto rightReg = frame.getValue(shiftOperation->getRightInput()->getIdentifier());
	auto resultReg = getResultRegister(shiftOperation, frame);
	frame.setValue(shiftOperation->getIdentifier(), resultReg);
	auto type = shiftOperation->getStamp();
	auto opType = shiftOperation->getType();
	ByteCode bc = ByteCode::BLSH_i8;
	switch ((type)) {
	case Type::i8:
		switch (opType) {
		case ir::ShiftOperation::LS:
			bc = ByteCode::BLSH_i8;
			break;
		case ir::ShiftOperation::RS:
			bc = ByteCode::BRSH_i8;
			break;
		}
		break;
	case Type::i16:
		switch (opType) {
		case ir::ShiftOperation::LS:
			bc = ByteCode::BLSH_i16;
			break;
		case ir::ShiftOperation::RS:
			bc = ByteCode::BRSH_i16;
			break;
		}
		break;
	case Type::i32:
		switch (opType) {
		case ir::ShiftOperation::LS:
			bc = ByteCode::BLSH_i32;
			break;
		case ir::ShiftOperation::RS:
			bc = ByteCode::BRSH_i32;
			break;
		}
		break;
	case Type::i64:
		switch (opType) {
		case ir::ShiftOperation::LS:
			bc = ByteCode::BLSH_i64;
			break;
		case ir::ShiftOperation::RS:
			bc = ByteCode::BRSH_i64;
			break;
		}
		break;
	case Type::ui8:
		switch (opType) {
		case ir::ShiftOperation::LS:
			bc = ByteCode::BLSH_ui8;
			break;
		case ir::ShiftOperation::RS:
			bc = ByteCode::BRSH_ui8;
			break;
		}
		break;
	case Type::ui16:
		switch (opType) {
		case ir::ShiftOperation::LS:
			bc = ByteCode::BLSH_ui16;
			break;
		case ir::ShiftOperation::RS:
			bc = ByteCode::BRSH_ui16;
			break;
		}
		break;
	case Type::ui32:
		switch (opType) {
		case ir::ShiftOperation::LS:
			bc = ByteCode::BLSH_ui32;
			break;
		case ir::ShiftOperation::RS:
			bc = ByteCode::BRSH_ui32;
			break;
		}
		break;
	case Type::ui64:
		switch (opType) {
		case ir::ShiftOperation::LS:
			bc = ByteCode::BLSH_ui64;
			break;
		case ir::ShiftOperation::RS:
			bc = ByteCode::BRSH_ui64;
			break;
		}
		break;
	default: {
		throw NotImplementedException("This type is not supported.");
	}
	}

	OpCode oc = {bc, leftReg, rightReg, resultReg};
	program.blocks[block].code.emplace_back(oc);
}

void BCLoweringProvider::LoweringContext::process(const ir::BasicBlockInvocation& bi, short block,
                                                  RegisterFrame& parentFrame) {
	auto blockInputArguments = bi.getArguments();
	auto& blockTargetArguments = bi.getBlock()->getArguments();
	std::vector<short> tempArgs;
	for (uint64_t i = 0; i < blockInputArguments.size(); i++) {
		auto blockArgument = blockInputArguments[i]->getIdentifier();
		auto parentFrameReg = parentFrame.getValue(blockArgument);
		auto tempArg = tempArgs.emplace_back(registerProvider.allocRegister());
		OpCode oc = {ByteCode::REG_MOV, parentFrameReg, -1, tempArg};
		program.blocks[block].code.emplace_back(oc);
	}
	for (uint64_t i = 0; i < blockInputArguments.size(); i++) {
		auto blockTargetArgument = blockTargetArguments[i]->getIdentifier();
		// auto parentFrameReg = parentFrame.getValue(blockArgument);
		if (!parentFrame.contains(blockTargetArgument)) {
			// auto resultReg = registerProvider.allocRegister();
			//  TODO use child frame
			parentFrame.setValue(blockTargetArgument, tempArgs[i]);
			// OpCode oc = {ByteCode::REG_MOV, parentFrameReg, -1, resultReg};
			// program.blocks[block].code.emplace_back(oc);
		} else {
			// TODO use child frame
			auto resultReg = parentFrame.getValue(blockTargetArgument);
			if (resultReg != tempArgs[i]) {
				OpCode oc = {ByteCode::REG_MOV, tempArgs[i], -1, resultReg};
				program.blocks[block].code.emplace_back(oc);
			}
		}
	}
}

void BCLoweringProvider::LoweringContext::visitIf(ir::IfOperation* ifOpt, short block, RegisterFrame& frame) {
	auto conditionalReg = frame.getValue(ifOpt->getValue()->getIdentifier());
	process(ifOpt->getTrueBlockInvocation(), block, frame);
	process(ifOpt->getFalseBlockInvocation(), block, frame);
	auto trueBlockIndex = process(ifOpt->getTrueBlockInvocation().getBlock(), frame);
	auto falseBlockIndex = process(ifOpt->getFalseBlockInvocation().getBlock(), frame);
	program.blocks[block].terminatorOp = ConditionalJumpOp {conditionalReg, trueBlockIndex, falseBlockIndex};
}

void BCLoweringProvider::LoweringContext::visitBranch(ir::BranchOperation* branchOp, short block,
                                                      RegisterFrame& frame) {
	process(branchOp->getNextBlockInvocation(), block, frame);
	auto blockIndex = process(branchOp->getNextBlockInvocation().getBlock(), frame);
	program.blocks[block].terminatorOp = BranchOp {blockIndex};
}

// Constant-operation hooks: emit a REG_MOV from a preloaded default register.
void BCLoweringProvider::LoweringContext::visitConstPtr(ir::ConstPtrOperation* constPtr, short block,
                                                        RegisterFrame& frame) {
	auto defaultRegister = registerProvider.allocRegister();
	allocateRegister(defaultRegister);
	defaultRegisterFile[defaultRegister] = (int64_t) constPtr->getValue();

	auto targetRegister = registerProvider.allocRegister();
	frame.setValue(constPtr->getIdentifier(), targetRegister);
	OpCode oc = {ByteCode::REG_MOV, defaultRegister, -1, targetRegister};
	program.blocks[block].code.emplace_back(oc);
}

void BCLoweringProvider::LoweringContext::visitConstBoolean(ir::ConstBooleanOperation* constInt, short block,
                                                            RegisterFrame& frame) {
	auto defaultRegister = registerProvider.allocRegister();
	allocateRegister(defaultRegister);
	defaultRegisterFile[defaultRegister] = constInt->getValue();

	auto targetRegister = registerProvider.allocRegister();
	frame.setValue(constInt->getIdentifier(), targetRegister);
	OpCode oc = {ByteCode::REG_MOV, defaultRegister, -1, targetRegister};
	program.blocks[block].code.emplace_back(oc);
}

void BCLoweringProvider::LoweringContext::visitConstInt(ir::ConstIntOperation* constInt, short block,
                                                        RegisterFrame& frame) {
	auto defaultRegister = registerProvider.allocRegister();
	allocateRegister(defaultRegister);
	defaultRegisterFile[defaultRegister] = constInt->getValue();
	auto targetRegister = registerProvider.allocRegister();
	frame.setValue(constInt->getIdentifier(), targetRegister);
	OpCode oc = {ByteCode::REG_MOV, defaultRegister, -1, targetRegister};
	program.blocks[block].code.emplace_back(oc);
}

void BCLoweringProvider::LoweringContext::visitConstFloat(ir::ConstFloatOperation* constInt, short block,
                                                          RegisterFrame& frame) {
	auto defaultRegister = registerProvider.allocRegister();
	allocateRegister(defaultRegister);
	if (constInt->getStamp() == Type::f32) {
		auto floatValue = (float) constInt->getValue();
		auto floatReg = reinterpret_cast<float*>(&defaultRegisterFile[defaultRegister]);
		*floatReg = floatValue;
	} else {
		auto floatValue = (double) constInt->getValue();
		auto floatReg = reinterpret_cast<double*>(&defaultRegisterFile[defaultRegister]);
		*floatReg = floatValue;
	}

	auto targetRegister = registerProvider.allocRegister();
	frame.setValue(constInt->getIdentifier(), targetRegister);
	OpCode oc = {ByteCode::REG_MOV, defaultRegister, -1, targetRegister};
	program.blocks[block].code.emplace_back(oc);
}

void BCLoweringProvider::LoweringContext::visitReturn(ir::ReturnOperation* returnOpt, short block,
                                                      RegisterFrame& frame) {
	if (returnOpt->hasReturnValue()) {
		auto returnFOp = frame.getValue(returnOpt->getReturnValue()->getIdentifier());
		program.blocks[block].terminatorOp = ReturnOp {returnFOp};
		program.returnType = returnOpt->getReturnValue()->getStamp();
	} else {
		program.blocks[block].terminatorOp = ReturnOp {-1};
	}
}

void BCLoweringProvider::LoweringContext::visitProxyCall(ir::ProxyCallOperation* opt, short block,
                                                         RegisterFrame& frame) {
	// create a dynamic call using dyncall.h
	processDynamicCall(opt, block, frame);
}

void BCLoweringProvider::LoweringContext::visitIndirectCall(ir::IndirectCallOperation* opt, short block,
                                                            RegisterFrame& frame) {
	auto& code = program.blocks[block].code;
	auto arguments = opt->getInputArguments();

	// 1. reset dyncall stack
	code.emplace_back(ByteCode::DYNCALL_reset, -1, -1, -1);

	// 2. set dyncall arguments
	for (auto& arg : arguments) {
		auto argType = (arg->getStamp());
		ByteCode bc;
		switch (argType) {
		case Type::i8:
			bc = ByteCode::DYNCALL_arg_i8;
			break;
		case Type::i16:
			bc = ByteCode::DYNCALL_arg_i16;
			break;
		case Type::i32:
			bc = ByteCode::DYNCALL_arg_i32;
			break;
		case Type::i64:
			bc = ByteCode::DYNCALL_arg_i64;
			break;
		case Type::ui8:
			bc = ByteCode::DYNCALL_arg_i8;
			break;
		case Type::ui16:
			bc = ByteCode::DYNCALL_arg_i16;
			break;
		case Type::ui32:
			bc = ByteCode::DYNCALL_arg_i32;
			break;
		case Type::ui64:
			bc = ByteCode::DYNCALL_arg_i64;
			break;
		case Type::f32:
			bc = ByteCode::DYNCALL_arg_f;
			break;
		case Type::f64:
			bc = ByteCode::DYNCALL_arg_d;
			break;
		case Type::b:
			bc = ByteCode::DYNCALL_arg_b;
			break;
		case Type::ptr:
			bc = ByteCode::DYNCALL_arg_ptr;
			break;
		default:
			throw NotImplementedException("This type is not supported.");
		}
		auto registerSlot = frame.getValue(arg->getIdentifier());
		code.emplace_back(bc, registerSlot, -1, -1);
	}

	// 3. call through the function pointer held in the register frame
	auto returnType = opt->getStamp();
	ByteCode bc;
	switch (returnType) {
	case Type::i8:
		bc = ByteCode::DYNCALL_call_i8;
		break;
	case Type::i16:
		bc = ByteCode::DYNCALL_call_i16;
		break;
	case Type::i32:
		bc = ByteCode::DYNCALL_call_i32;
		break;
	case Type::i64:
		bc = ByteCode::DYNCALL_call_i64;
		break;
	case Type::ui8:
		bc = ByteCode::DYNCALL_call_i8;
		break;
	case Type::ui16:
		bc = ByteCode::DYNCALL_call_i16;
		break;
	case Type::ui32:
		bc = ByteCode::DYNCALL_call_i32;
		break;
	case Type::ui64:
		bc = ByteCode::DYNCALL_call_i64;
		break;
	case Type::ptr:
		bc = ByteCode::DYNCALL_call_ptr;
		break;
	case Type::v:
		bc = ByteCode::DYNCALL_call_v;
		break;
	case Type::b:
		bc = ByteCode::DYNCALL_call_b;
		break;
	case Type::f32:
		bc = ByteCode::DYNCALL_call_f;
		break;
	case Type::f64:
		bc = ByteCode::DYNCALL_call_d;
		break;
	default: {
		throw NotImplementedException("This type is not supported.");
	}
	}

	// The function pointer SSA value is already in the register frame.
	auto funcPtrRegister = frame.getValue(opt->getFunctionPtrOperand()->getIdentifier());

	if (opt->getStamp() != Type::v) {
		auto resultRegister = getResultRegister(opt, frame);
		frame.setValue(opt->getIdentifier(), resultRegister);
		code.emplace_back(bc, funcPtrRegister, -1, resultRegister);
	} else {
		code.emplace_back(bc, funcPtrRegister, -1, -1);
	}
}

void BCLoweringProvider::LoweringContext::processDynamicCall(ir::ProxyCallOperation* opt, short block,
                                                             RegisterFrame& frame) {
	auto& code = program.blocks[block].code;
	// NES_DEBUG("CREATE " << opt->toString() << " : " <<
	// opt->getStamp()->toString())
	auto arguments = opt->getInputArguments();

	// 1. reset dyncall stack
	code.emplace_back(ByteCode::DYNCALL_reset, -1, -1, -1);

	// 2. set dyncall arguments
	for (auto& arg : arguments) {
		auto argType = (arg->getStamp());
		ByteCode bc;
		switch (argType) {
		case Type::i8:
			bc = ByteCode::DYNCALL_arg_i8;
			break;
		case Type::i16:
			bc = ByteCode::DYNCALL_arg_i16;
			break;
		case Type::i32:
			bc = ByteCode::DYNCALL_arg_i32;
			break;
		case Type::i64:
			bc = ByteCode::DYNCALL_arg_i64;
			break;
		case Type::ui8:
			bc = ByteCode::DYNCALL_arg_i8;
			break;
		case Type::ui16:
			bc = ByteCode::DYNCALL_arg_i16;
			break;
		case Type::ui32:
			bc = ByteCode::DYNCALL_arg_i32;
			break;
		case Type::ui64:
			bc = ByteCode::DYNCALL_arg_i64;
			break;
		case Type::f32:
			bc = ByteCode::DYNCALL_arg_f;
			break;
		case Type::f64:
			bc = ByteCode::DYNCALL_arg_d;
			break;
		case Type::b:
			bc = ByteCode::DYNCALL_arg_b;
			break;
		case Type::ptr:
			bc = ByteCode::DYNCALL_arg_ptr;
			break;
		default:
			throw NotImplementedException("This type is not supported.");
		}
		auto registerSlot = frame.getValue(arg->getIdentifier());
		code.emplace_back(bc, registerSlot, -1, -1);
	}

	// 3. call function
	auto returnType = opt->getStamp();
	ByteCode bc;
	switch (returnType) {
	case Type::i8:
		bc = ByteCode::DYNCALL_call_i8;
		break;
	case Type::i16:
		bc = ByteCode::DYNCALL_call_i16;
		break;
	case Type::i32:
		bc = ByteCode::DYNCALL_call_i32;
		break;
	case Type::i64:
		bc = ByteCode::DYNCALL_call_i64;
		break;
	case Type::ui8:
		bc = ByteCode::DYNCALL_call_i8;
		break;
	case Type::ui16:
		bc = ByteCode::DYNCALL_call_i16;
		break;
	case Type::ui32:
		bc = ByteCode::DYNCALL_call_i32;
		break;
	case Type::ui64:
		bc = ByteCode::DYNCALL_call_i64;
		break;
	case Type::ptr:
		bc = ByteCode::DYNCALL_call_ptr;
		break;
	case Type::v:
		bc = ByteCode::DYNCALL_call_v;
		break;
	case Type::b:
		bc = ByteCode::DYNCALL_call_b;
		break;
	case Type::f32:
		bc = ByteCode::DYNCALL_call_f;
		break;
	case Type::f64:
		bc = ByteCode::DYNCALL_call_d;
		break;
	default: {
		throw NotImplementedException("This type is not supported.");
	}
	}

	auto funcInfoRegister = registerProvider.allocRegister();
	allocateRegister(funcInfoRegister);
	// For internal NautilusFunction calls, use the pre-compiled callback pointer
	auto it = internalFunctionPtrs.find(opt->getFunctionName());
	if (it != internalFunctionPtrs.end()) {
		defaultRegisterFile[funcInfoRegister] = (int64_t) it->second;
	} else {
		defaultRegisterFile[funcInfoRegister] = (int64_t) opt->getFunctionPtr();
	}

	if (opt->getStamp() != Type::v) {
		auto resultRegister = getResultRegister(opt, frame);
		frame.setValue(opt->getIdentifier(), resultRegister);
		code.emplace_back(bc, funcInfoRegister, -1, resultRegister);
	} else {
		code.emplace_back(bc, funcInfoRegister, -1, -1);
	}
}

void BCLoweringProvider::LoweringContext::visitNot(ir::NotOperation* negateOperation, short block,
                                                   RegisterFrame& frame) {
	auto input = frame.getValue(negateOperation->getInput()->getIdentifier());
	auto resultReg = getResultRegister(negateOperation, frame);
	frame.setValue(negateOperation->getIdentifier(), resultReg);
	ByteCode bc = ByteCode::NOT_b;
	OpCode oc = {bc, input, -1, resultReg};
	program.blocks[block].code.emplace_back(oc);
}

void BCLoweringProvider::LoweringContext::visitNegate(ir::NegateOperation* negateOperation, short block,
                                                      RegisterFrame& frame) {
	auto input = frame.getValue(negateOperation->getInput()->getIdentifier());
	auto resultReg = getResultRegister(negateOperation, frame);
	frame.setValue(negateOperation->getIdentifier(), resultReg);
	ByteCode bc = ByteCode::BNEGATE_I64;
	OpCode oc = {bc, input, -1, resultReg};
	program.blocks[block].code.emplace_back(oc);
}

void BCLoweringProvider::LoweringContext::visitCast(ir::CastOperation* castOp, short block, RegisterFrame& frame) {
	auto input = frame.getValue(castOp->getInput()->getIdentifier());
	auto resultReg = getResultRegister(castOp, frame);
	frame.setValue(castOp->getIdentifier(), resultReg);
	auto srcType = (castOp->getInput()->getStamp());
	auto tar = (castOp->getStamp());
	ByteCode bc;
	// Identity casts (e.g. ui64 → ui64) can occur when two distinct C++ types
	// map to the same nautilus Type — for example on macOS, where size_t is
	// `unsigned long` and uint64_t is `unsigned long long`, both lowering to
	// Type::ui64. Treat them as a register move regardless of source type.
	if (srcType == tar) {
		bc = ByteCode::REG_MOV;
	} else if (srcType == Type::i8) {
		switch (tar) {
		case Type::i16:
			bc = ByteCode::CAST_i8_i16;
			break;
		case Type::i32:
			bc = ByteCode::CAST_i8_i32;
			break;
		case Type::i64:
			bc = ByteCode::CAST_i8_i64;
			break;
		case Type::ui8:
			bc = ByteCode::CAST_i8_ui8;
			break;
		case Type::ui16:
			bc = ByteCode::CAST_i8_ui16;
			break;
		case Type::ui32:
			bc = ByteCode::CAST_i8_ui32;
			break;
		case Type::ui64:
			bc = ByteCode::CAST_i8_ui64;
			break;
		case Type::f32:
			bc = ByteCode::CAST_i8_f;
			break;
		case Type::f64:
			bc = ByteCode::CAST_i8_d;
			break;
		case Type::ptr:
			bc = ByteCode::CAST_i8_ui64;
			break;
		default:
			throw NotImplementedException("This type is not supported.");
		}
	} else if (srcType == Type::i16) {
		switch (tar) {
		case Type::i8:
			bc = ByteCode::CAST_i16_i8;
			break;
		case Type::i32:
			bc = ByteCode::CAST_i16_i32;
			break;
		case Type::i64:
			bc = ByteCode::CAST_i16_i64;
			break;
		case Type::ui8:
			bc = ByteCode::CAST_i16_ui8;
			break;
		case Type::ui16:
			bc = ByteCode::CAST_i16_ui16;
			break;
		case Type::ui32:
			bc = ByteCode::CAST_i16_ui32;
			break;
		case Type::ui64:
			bc = ByteCode::CAST_i16_ui64;
			break;
		case Type::f32:
			bc = ByteCode::CAST_i16_f;
			break;
		case Type::f64:
			bc = ByteCode::CAST_i16_d;
			break;
		case Type::ptr:
			bc = ByteCode::CAST_i16_ui64;
			break;
		default:
			throw NotImplementedException("This type is not supported.");
		}
	} else if (srcType == Type::i32) {
		switch (tar) {
		case Type::i8:
			bc = ByteCode::CAST_i32_i8;
			break;
		case Type::i16:
			bc = ByteCode::CAST_i32_i16;
			break;
		case Type::i64:
			bc = ByteCode::CAST_i32_i64;
			break;
		case Type::ui8:
			bc = ByteCode::CAST_i32_ui8;
			break;
		case Type::ui16:
			bc = ByteCode::CAST_i32_ui16;
			break;
		case Type::ui32:
			bc = ByteCode::CAST_i32_ui32;
			break;
		case Type::ui64:
			bc = ByteCode::CAST_i32_ui64;
			break;
		case Type::f32:
			bc = ByteCode::CAST_i32_f;
			break;
		case Type::f64:
			bc = ByteCode::CAST_i32_d;
			break;
		case Type::ptr:
			bc = ByteCode::CAST_i32_ui64;
			break;
		default:
			throw NotImplementedException("This type is not supported.");
		}
	} else if (srcType == Type::i64) {
		switch (tar) {
		case Type::i8:
			bc = ByteCode::CAST_i64_i8;
			break;
		case Type::i16:
			bc = ByteCode::CAST_i64_i16;
			break;
		case Type::i32:
			bc = ByteCode::CAST_i64_i32;
			break;
		case Type::ui8:
			bc = ByteCode::CAST_i64_ui8;
			break;
		case Type::ui16:
			bc = ByteCode::CAST_i64_ui16;
			break;
		case Type::ui32:
			bc = ByteCode::CAST_i64_ui32;
			break;
		case Type::ui64:
			bc = ByteCode::CAST_i64_ui64;
			break;
		case Type::f32:
			bc = ByteCode::CAST_i64_f;
			break;
		case Type::f64:
			bc = ByteCode::CAST_i64_d;
			break;
		case Type::ptr:
			bc = ByteCode::CAST_i64_ui64;
			break;
		default:
			throw NotImplementedException("This type is not supported.");
		}
	} else if (srcType == Type::ui8) {
		switch (tar) {
		case Type::ui16:
			bc = ByteCode::CAST_ui8_ui16;
			break;
		case Type::ui32:
			bc = ByteCode::CAST_ui8_ui32;
			break;
		case Type::ui64:
			bc = ByteCode::CAST_ui8_ui64;
			break;
		case Type::i8:
			bc = ByteCode::CAST_ui8_i8;
			break;
		case Type::i16:
			bc = ByteCode::CAST_ui8_i16;
			break;
		case Type::i32:
			bc = ByteCode::CAST_ui8_i32;
			break;
		case Type::i64:
			bc = ByteCode::CAST_ui8_i64;
			break;
		case Type::f32:
			bc = ByteCode::CAST_ui8_f;
			break;
		case Type::f64:
			bc = ByteCode::CAST_ui8_d;
			break;
		case Type::ptr:
			bc = ByteCode::CAST_ui8_ui64;
			break;
		default:
			throw NotImplementedException("This type is not supported.");
		}
	} else if (srcType == Type::ui16) {
		switch (tar) {
		case Type::ui8:
			bc = ByteCode::CAST_ui16_ui8;
			break;
		case Type::ui32:
			bc = ByteCode::CAST_ui16_ui32;
			break;
		case Type::ui64:
			bc = ByteCode::CAST_ui16_ui64;
			break;
		case Type::i8:
			bc = ByteCode::CAST_ui16_i8;
			break;
		case Type::i16:
			bc = ByteCode::CAST_ui16_i16;
			break;
		case Type::i32:
			bc = ByteCode::CAST_ui16_i32;
			break;
		case Type::i64:
			bc = ByteCode::CAST_ui16_i64;
			break;
		case Type::f32:
			bc = ByteCode::CAST_ui16_f;
			break;
		case Type::f64:
			bc = ByteCode::CAST_ui16_d;
			break;
		case Type::ptr:
			bc = ByteCode::CAST_ui16_ui64;
			break;
		default:
			throw NotImplementedException("This type is not supported.");
		}
	} else if (srcType == Type::ui32) {
		switch (tar) {
		case Type::ui8:
			bc = ByteCode::CAST_ui32_ui8;
			break;
		case Type::ui16:
			bc = ByteCode::CAST_ui32_ui16;
			break;
		case Type::ui64:
			bc = ByteCode::CAST_ui32_ui64;
			break;
		case Type::i8:
			bc = ByteCode::CAST_ui32_i8;
			break;
		case Type::i16:
			bc = ByteCode::CAST_ui32_i16;
			break;
		case Type::i32:
			bc = ByteCode::CAST_ui32_i32;
			break;
		case Type::i64:
			bc = ByteCode::CAST_ui32_i64;
			break;
		case Type::f32:
			bc = ByteCode::CAST_ui32_f;
			break;
		case Type::f64:
			bc = ByteCode::CAST_ui32_d;
			break;
		case Type::ptr:
			bc = ByteCode::CAST_ui32_ui64;
			break;
		default:
			throw NotImplementedException("This type is not supported.");
		}

	} else if (srcType == Type::ui64) {
		switch (tar) {
		case Type::ui8:
			bc = ByteCode::CAST_ui64_ui8;
			break;
		case Type::ui16:
			bc = ByteCode::CAST_ui64_ui16;
			break;
		case Type::ui32:
			bc = ByteCode::CAST_ui64_ui32;
			break;
		case Type::i8:
			bc = ByteCode::CAST_ui64_i8;
			break;
		case Type::i16:
			bc = ByteCode::CAST_ui64_i16;
			break;
		case Type::i32:
			bc = ByteCode::CAST_ui64_i32;
			break;
		case Type::i64:
			bc = ByteCode::CAST_ui64_i64;
			break;
		case Type::f32:
			bc = ByteCode::CAST_ui64_f;
			break;
		case Type::f64:
			bc = ByteCode::CAST_ui64_d;
			break;
		case Type::ptr:
			bc = ByteCode::REG_MOV;
			break;
		default:
			throw NotImplementedException("This type is not supported.");
		}

	} else if (srcType == Type::f32) {
		switch (tar) {
		case Type::ui8:
			bc = ByteCode::CAST_f_ui8;
			break;
		case Type::ui16:
			bc = ByteCode::CAST_f_ui16;
			break;
		case Type::ui32:
			bc = ByteCode::CAST_f_ui32;
			break;
		case Type::ui64:
			bc = ByteCode::CAST_f_ui64;
			break;
		case Type::i8:
			bc = ByteCode::CAST_f_i8;
			break;
		case Type::i16:
			bc = ByteCode::CAST_f_i16;
			break;
		case Type::i32:
			bc = ByteCode::CAST_f_i32;
			break;
		case Type::i64:
			bc = ByteCode::CAST_f_i64;
			break;
		case Type::f64:
			bc = ByteCode::CAST_f_d;
			break;
		case Type::ptr:
			bc = ByteCode::CAST_f_ui64;
			break;
		default:
			throw NotImplementedException("This type is not supported.");
		}
	} else if (srcType == Type::f64) {
		switch (tar) {
		case Type::ui8:
			bc = ByteCode::CAST_d_ui8;
			break;
		case Type::ui16:
			bc = ByteCode::CAST_d_ui16;
			break;
		case Type::ui32:
			bc = ByteCode::CAST_d_ui32;
			break;
		case Type::ui64:
			bc = ByteCode::CAST_d_ui64;
			break;
		case Type::i8:
			bc = ByteCode::CAST_d_i8;
			break;
		case Type::i16:
			bc = ByteCode::CAST_d_i16;
			break;
		case Type::i32:
			bc = ByteCode::CAST_d_i32;
			break;
		case Type::i64:
			bc = ByteCode::CAST_d_i64;
			break;
		case Type::f32:
			bc = ByteCode::CAST_d_f;
			break;
		case Type::ptr:
			bc = ByteCode::CAST_d_ui64;
			break;
		default:
			throw NotImplementedException("This type is not supported.");
		}
	} else if (srcType == Type::ptr) {
		switch (tar) {
		case Type::i8:
			bc = ByteCode::CAST_ui64_i8;
			break;
		case Type::i16:
			bc = ByteCode::CAST_ui64_i16;
			break;
		case Type::i32:
			bc = ByteCode::CAST_ui64_i32;
			break;
		case Type::i64:
			bc = ByteCode::CAST_ui64_i64;
			break;
		case Type::ui8:
			bc = ByteCode::CAST_ui64_ui8;
			break;
		case Type::ui16:
			bc = ByteCode::CAST_ui64_ui16;
			break;
		case Type::ui32:
			bc = ByteCode::CAST_ui64_ui32;
			break;
		case Type::ui64:
			bc = ByteCode::REG_MOV;
			break;
		case Type::f32:
			bc = ByteCode::CAST_ui64_f;
			break;
		case Type::f64:
			bc = ByteCode::CAST_ui64_d;
			break;
		case Type::ptr:
			bc = ByteCode::REG_MOV;
			break;
		default:
			throw NotImplementedException("This type is not supported.");
		}
	} else {
		throw NotImplementedException("This type is not supported.");
	}
	OpCode oc = {bc, input, -1, resultReg};
	program.blocks[block].code.emplace_back(oc);
}

short BCLoweringProvider::LoweringContext::getResultRegister(ir::Operation*, RegisterFrame&) {
	// auto optResultIdentifier = opt->getIdentifier();

	// if the result value of opt is directly passed to an block argument, then we
	// can directly write the value to the correct target register.

	/*
	if (opt->getUsages().size() == 1) {
	    auto *usage = opt->getUsages()[0];
	    if (usage->getOperationType() ==
	ir::Operation::OperationType::BlockInvocation) { auto bi = dynamic_cast<const
	ir::BasicBlockInvocation *>(usage); auto blockInputArguments =
	bi->getArguments(); auto blockTargetArguments =
	bi->getBlock()->getArguments();

	        std::vector<uint64_t> matchingArguments;

	        for (uint64_t i = 0; i < blockInputArguments.size(); i++) {
	            auto blockArgument = blockInputArguments[i]->getIdentifier();
	            if (blockArgument == optResultIdentifier) {
	                matchingArguments.emplace_back(i);
	            }
	        }

	        if (matchingArguments.size() == 1) {
	            auto blockTargetArgumentIdentifier =
	blockTargetArguments[matchingArguments[0]]->getIdentifier(); if
	(!frame.contains(blockTargetArgumentIdentifier)) { auto resultReg =
	registerProvider.allocRegister();
	                frame.setValue(blockTargetArgumentIdentifier, resultReg);
	                return resultReg;
	            } else {
	                return frame.getValue(blockTargetArgumentIdentifier);
	            }
	        }
	    }
	}
	 */
	return registerProvider.allocRegister();
}

void BCLoweringProvider::LoweringContext::countUsages(const ir::BasicBlock* block) {
	// Count how many times each value is used in this block
	for (auto* opt : block->getOperations()) {
		// Count inputs to this operation
		auto countInput = [this](const ir::Operation* input) {
			if (input) {
				usageCounts[input->getIdentifier()]++;
			}
		};

		switch (opt->getOperationType()) {
		case ir::Operation::OperationType::AddOp:
		case ir::Operation::OperationType::SubOp:
		case ir::Operation::OperationType::MulOp:
		case ir::Operation::OperationType::DivOp:
		case ir::Operation::OperationType::ModOp: {
			auto binOp = static_cast<ir::Operation*>(opt);
			if (auto addOp = dynamic_cast<ir::AddOperation*>(binOp)) {
				countInput(addOp->getLeftInput());
				countInput(addOp->getRightInput());
			} else if (auto subOp = dynamic_cast<ir::SubOperation*>(binOp)) {
				countInput(subOp->getLeftInput());
				countInput(subOp->getRightInput());
			} else if (auto mulOp = dynamic_cast<ir::MulOperation*>(binOp)) {
				countInput(mulOp->getLeftInput());
				countInput(mulOp->getRightInput());
			} else if (auto divOp = dynamic_cast<ir::DivOperation*>(binOp)) {
				countInput(divOp->getLeftInput());
				countInput(divOp->getRightInput());
			} else if (auto modOp = dynamic_cast<ir::ModOperation*>(binOp)) {
				countInput(modOp->getLeftInput());
				countInput(modOp->getRightInput());
			}
			break;
		}
		case ir::Operation::OperationType::CompareOp: {
			auto cmpOp = dynamic_cast<ir::CompareOperation*>(opt);
			countInput(cmpOp->getLeftInput());
			countInput(cmpOp->getRightInput());
			break;
		}
		case ir::Operation::OperationType::LoadOp: {
			auto loadOp = dynamic_cast<ir::LoadOperation*>(opt);
			countInput(loadOp->getAddress());
			break;
		}
		case ir::Operation::OperationType::StoreOp: {
			auto storeOp = dynamic_cast<ir::StoreOperation*>(opt);
			countInput(storeOp->getAddress());
			countInput(storeOp->getValue());
			break;
		}
		case ir::Operation::OperationType::CastOp: {
			auto castOp = dynamic_cast<ir::CastOperation*>(opt);
			countInput(castOp->getInput());
			break;
		}
		case ir::Operation::OperationType::ReturnOp: {
			auto retOp = dynamic_cast<ir::ReturnOperation*>(opt);
			if (retOp->hasReturnValue()) {
				countInput(retOp->getReturnValue());
			}
			break;
		}
		default:
			// Other operations not critical for basic register reuse
			break;
		}
	}
}

void BCLoweringProvider::LoweringContext::useValue(const ir::OperationIdentifier& identifier, RegisterFrame& frame) {
	// Skip if this is a function argument
	if (functionArgs.count(identifier) > 0) {
		return;
	}

	// Decrement usage count
	auto it = usageCounts.find(identifier);
	if (it != usageCounts.end() && it->second > 0) {
		it->second--;
		// If this was the last use, free the register
		if (it->second == 0 && frame.contains(identifier)) {
			short reg = frame.getValue(identifier);
			registerProvider.freeRegister(reg);
		}
	}
}

void BCLoweringProvider::LoweringContext::visitSelect(ir::SelectOperation* selectOp, short block,
                                                      RegisterFrame& frame) {
	auto conditionReg = frame.getValue(selectOp->getCondition()->getIdentifier());
	auto trueValueReg = frame.getValue(selectOp->getTrueValue()->getIdentifier());
	auto falseValueReg = frame.getValue(selectOp->getFalseValue()->getIdentifier());
	auto resultReg = getResultRegister(selectOp, frame);
	frame.setValue(selectOp->getIdentifier(), resultReg);

	ByteCode bc;
	switch (selectOp->getStamp()) {
	case Type::i8:
		bc = ByteCode::SELECT_i8;
		break;
	case Type::i16:
		bc = ByteCode::SELECT_i16;
		break;
	case Type::i32:
		bc = ByteCode::SELECT_i32;
		break;
	case Type::i64:
		bc = ByteCode::SELECT_i64;
		break;
	case Type::ui8:
		bc = ByteCode::SELECT_ui8;
		break;
	case Type::ui16:
		bc = ByteCode::SELECT_ui16;
		break;
	case Type::ui32:
		bc = ByteCode::SELECT_ui32;
		break;
	case Type::ui64:
		bc = ByteCode::SELECT_ui64;
		break;
	case Type::f32:
		bc = ByteCode::SELECT_f;
		break;
	case Type::f64:
		bc = ByteCode::SELECT_d;
		break;
	case Type::b:
		bc = ByteCode::SELECT_b;
		break;
	case Type::ptr:
		bc = ByteCode::SELECT_ptr;
		break;
	default: {
		throw NotImplementedException("This type is not supported.");
	}
	}

	// OpCode constructor: op, reg1, reg2, output, reg3
	// For SELECT: reg1=condition, reg2=trueValue, reg3=falseValue, output=result
	OpCode oc = {bc, conditionReg, trueValueReg, resultReg, falseValueReg};
	program.blocks[block].code.emplace_back(oc);
}

void BCLoweringProvider::LoweringContext::visitAlloca(ir::AllocaOperation* allocaOp, short /*block*/,
                                                      RegisterFrame& frame) {
	auto resultRegister = registerProvider.allocRegister();
	allocateRegister(resultRegister);
	auto bufferIndex = program.allocaBuffers.size();
	program.allocaBuffers.emplace_back(allocaOp->getSize(), uint8_t {0});
	program.allocaRegisterMap.emplace_back(resultRegister, bufferIndex);
	defaultRegisterFile[resultRegister] = reinterpret_cast<int64_t>(program.allocaBuffers.back().data());
	frame.setValue(allocaOp->getIdentifier(), resultRegister);
}

void BCLoweringProvider::LoweringContext::visitFunctionAddressOf(ir::FunctionAddressOfOperation* funcAddrOp,
                                                                 short block, RegisterFrame& frame) {
	// Store the function pointer constant into a register, same pattern as ConstPtrOperation.
	auto defaultRegister = registerProvider.allocRegister();
	allocateRegister(defaultRegister);
	// For internal NautilusFunction calls, use the pre-compiled callback pointer
	auto it = internalFunctionPtrs.find(funcAddrOp->getFunctionName());
	if (it != internalFunctionPtrs.end()) {
		defaultRegisterFile[defaultRegister] = (int64_t) it->second;
	} else {
		defaultRegisterFile[defaultRegister] = (int64_t) funcAddrOp->getFunctionPtr();
	}

	auto targetRegister = registerProvider.allocRegister();
	frame.setValue(funcAddrOp->getIdentifier(), targetRegister);
	OpCode oc = {ByteCode::REG_MOV, defaultRegister, -1, targetRegister};
	program.blocks[block].code.emplace_back(oc);
}

} // namespace nautilus::compiler::bc

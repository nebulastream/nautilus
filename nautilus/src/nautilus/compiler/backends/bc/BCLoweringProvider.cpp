

#include "nautilus/compiler/backends/bc/BCLoweringProvider.hpp"
#include "nautilus/compiler/backends/bc/ByteCode.hpp"
#include "nautilus/compiler/ir/operations/Operation.hpp"
#include "nautilus/exceptions/NotImplementedException.hpp"
#include <utility>

namespace nautilus::compiler::bc {

BCLoweringProvider::BCLoweringProvider() {
}

BCLoweringProvider::LoweringContext::LoweringContext(std::shared_ptr<ir::IRGraph> ir) : ir(std::move(ir)) {
}

std::tuple<Code, RegisterFile> BCLoweringProvider::lower(std::shared_ptr<ir::IRGraph> ir) {
	auto ctx = LoweringContext(ir);
	return ctx.process();
}

short BCLoweringProvider::RegisterProvider::allocRegister() {
	// NES_ASSERT(currentRegister <= REGISTERS, "allocated to many registers.");
	return currentRegister++;
}

void BCLoweringProvider::RegisterProvider::freeRegister() {
	// TODO
}

std::tuple<Code, RegisterFile> BCLoweringProvider::LoweringContext::process() {
	const auto& functionOperation = ir->getRootOperation();
	RegisterFrame rootFrame;
	const auto& functionBasicBlock = functionOperation.getFunctionBasicBlock();
	for (auto i = 0ull; i < functionBasicBlock.getArguments().size(); i++) {
		auto& argument = functionBasicBlock.getArguments()[i];
		auto argumentRegister = registerProvider.allocRegister();
		rootFrame.setValue(argument->getIdentifier(), argumentRegister);
		program.arguments.emplace_back(argumentRegister);
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
		// create bytecode block;
		program.blocks.emplace_back();
		for (auto& opt : block->getOperations()) {
			this->process(opt, blockIndex, frame);
		}
		return blockIndex;
	} else {
		return entry->second;
	}
}

void BCLoweringProvider::LoweringContext::process(ir::AndOperation* addOpt, short block, RegisterFrame& frame) {
	auto leftInput = frame.getValue(addOpt->getLeftInput()->getIdentifier());
	auto rightInput = frame.getValue(addOpt->getRightInput()->getIdentifier());
	auto resultReg = getResultRegister(addOpt, frame);
	frame.setValue(addOpt->getIdentifier(), resultReg);
	ByteCode bc = ByteCode::AND_b;
	OpCode oc = {bc, leftInput, rightInput, resultReg};
	program.blocks[block].code.emplace_back(oc);
}

void BCLoweringProvider::LoweringContext::process(ir::OrOperation* addOpt, short block, RegisterFrame& frame) {
	auto leftInput = frame.getValue(addOpt->getLeftInput()->getIdentifier());
	auto rightInput = frame.getValue(addOpt->getRightInput()->getIdentifier());
	auto resultReg = getResultRegister(addOpt, frame);
	frame.setValue(addOpt->getIdentifier(), resultReg);
	ByteCode bc = ByteCode::OR_b;
	OpCode oc = {bc, leftInput, rightInput, resultReg};
	program.blocks[block].code.emplace_back(oc);
}

void BCLoweringProvider::LoweringContext::process(ir::AddOperation* addOpt, short block, RegisterFrame& frame) {
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

void BCLoweringProvider::LoweringContext::process(ir::SubOperation* subOpt, short block, RegisterFrame& frame) {
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

void BCLoweringProvider::LoweringContext::process(ir::MulOperation* mulOpt, short block, RegisterFrame& frame) {
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

void BCLoweringProvider::LoweringContext::process(ir::DivOperation* divOp, short block, RegisterFrame& frame) {
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

void BCLoweringProvider::LoweringContext::process(ir::ModOperation* divOp, short block, RegisterFrame& frame) {
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

void BCLoweringProvider::LoweringContext::process(ir::CompareOperation* cmpOp, short block, RegisterFrame& frame) {
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

void BCLoweringProvider::LoweringContext::process(ir::LoadOperation* loadOp, short block, RegisterFrame& frame) {
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

void BCLoweringProvider::LoweringContext::process(ir::StoreOperation* storeOp, short block, RegisterFrame& frame) {
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

void BCLoweringProvider::LoweringContext::process(ir::BinaryCompOperation* binaryCompOperation, short block,
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

void BCLoweringProvider::LoweringContext::process(ir::ShiftOperation* shiftOperation, short block,
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

void BCLoweringProvider::LoweringContext::process(ir::IfOperation* ifOpt, short block, RegisterFrame& frame) {
	auto conditionalReg = frame.getValue(ifOpt->getValue()->getIdentifier());
	process(ifOpt->getTrueBlockInvocation(), block, frame);
	process(ifOpt->getFalseBlockInvocation(), block, frame);
	auto trueBlockIndex = process(ifOpt->getTrueBlockInvocation().getBlock(), frame);
	auto falseBlockIndex = process(ifOpt->getFalseBlockInvocation().getBlock(), frame);
	program.blocks[block].terminatorOp = ConditionalJumpOp {conditionalReg, trueBlockIndex, falseBlockIndex};
}

void BCLoweringProvider::LoweringContext::process(ir::BranchOperation* branchOp, short block, RegisterFrame& frame) {
	process(branchOp->getNextBlockInvocation(), block, frame);
	auto blockIndex = process(branchOp->getNextBlockInvocation().getBlock(), frame);
	program.blocks[block].terminatorOp = BranchOp {blockIndex};
}

void BCLoweringProvider::LoweringContext::process(const std::unique_ptr<ir::Operation>& opt, short block,
                                                  RegisterFrame& frame) {
	switch (opt->getOperationType()) {
	case ir::Operation::OperationType::ConstPtrOp: {
		auto constPtr = as<ir::ConstPtrOperation>(opt);
		auto defaultRegister = registerProvider.allocRegister();
		defaultRegisterFile[defaultRegister] = (int64_t) constPtr->getValue();

		auto targetRegister = registerProvider.allocRegister();
		frame.setValue(constPtr->getIdentifier(), targetRegister);
		OpCode oc = {ByteCode::REG_MOV, defaultRegister, -1, targetRegister};
		program.blocks[block].code.emplace_back(oc);
		return;
	}
	case ir::Operation::OperationType::ConstBooleanOp: {
		auto constInt = as<ir::ConstBooleanOperation>(opt);
		auto defaultRegister = registerProvider.allocRegister();
		defaultRegisterFile[defaultRegister] = constInt->getValue();

		auto targetRegister = registerProvider.allocRegister();
		frame.setValue(constInt->getIdentifier(), targetRegister);
		OpCode oc = {ByteCode::REG_MOV, defaultRegister, -1, targetRegister};
		program.blocks[block].code.emplace_back(oc);
		return;
	}
	case ir::Operation::OperationType::ConstIntOp: {
		auto constInt = as<ir::ConstIntOperation>(opt);
		auto defaultRegister = registerProvider.allocRegister();
		defaultRegisterFile[defaultRegister] = constInt->getValue();
		auto targetRegister = registerProvider.allocRegister();
		frame.setValue(constInt->getIdentifier(), targetRegister);
		OpCode oc = {ByteCode::REG_MOV, defaultRegister, -1, targetRegister};
		program.blocks[block].code.emplace_back(oc);
		return;
	}
	case ir::Operation::OperationType::ConstFloatOp: {
		auto constInt = as<ir::ConstFloatOperation>(opt);
		auto defaultRegister = registerProvider.allocRegister();
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
		return;
	}
	case ir::Operation::OperationType::AddOp: {
		auto addOpt = as<ir::AddOperation>(opt);
		process(addOpt, block, frame);
		return;
	}
	case ir::Operation::OperationType::MulOp: {
		auto mulOpt = as<ir::MulOperation>(opt);
		process(mulOpt, block, frame);
		return;
	}
	case ir::Operation::OperationType::SubOp: {
		auto subOpt = as<ir::SubOperation>(opt);
		process(subOpt, block, frame);
		return;
	}
	case ir::Operation::OperationType::DivOp: {
		auto divOpt = as<ir::DivOperation>(opt);
		process(divOpt, block, frame);
		return;
	}
	case ir::Operation::OperationType::ReturnOp: {
		auto returnOpt = as<ir::ReturnOperation>(opt);
		if (returnOpt->hasReturnValue()) {
			auto returnFOp = frame.getValue(returnOpt->getReturnValue()->getIdentifier());
			program.blocks[block].terminatorOp = ReturnOp {returnFOp};
			program.returnType = returnOpt->getReturnValue()->getStamp();
			return;
		} else {
			program.blocks[block].terminatorOp = ReturnOp {-1};
			return;
		}
	}
	case ir::Operation::OperationType::CompareOp: {
		auto compOpt = as<ir::CompareOperation>(opt);
		process(compOpt, block, frame);
		return;
	}
	case ir::Operation::OperationType::IfOp: {
		auto ifOpt = as<ir::IfOperation>(opt);
		process(ifOpt, block, frame);
		return;
	}

	case ir::Operation::OperationType::BranchOp: {
		auto branchOp = as<ir::BranchOperation>(opt);
		process(branchOp, block, frame);
		return;
	}
	case ir::Operation::OperationType::LoadOp: {
		auto load = as<ir::LoadOperation>(opt);
		process(load, block, frame);
		return;
	}
	case ir::Operation::OperationType::StoreOp: {
		auto store = as<ir::StoreOperation>(opt);
		process(store, block, frame);
		return;
	}
	case ir::Operation::OperationType::ProxyCallOp: {
		auto call = as<ir::ProxyCallOperation>(opt);
		process(call, block, frame);
		return;
	}
	case ir::Operation::OperationType::OrOp: {
		auto call = as<ir::OrOperation>(opt);
		process(call, block, frame);
		return;
	}
	case ir::Operation::OperationType::AndOp: {
		auto call = as<ir::AndOperation>(opt);
		process(call, block, frame);
		return;
	}
	case ir::Operation::OperationType::NegateOp: {
		auto call = as<ir::NegateOperation>(opt);
		process(call, block, frame);
		return;
	}
	case ir::Operation::OperationType::NotOp: {
		auto call = as<ir::NotOperation>(opt);
		process(call, block, frame);
		return;
	}
	case ir::Operation::OperationType::CastOp: {
		auto cast = as<ir::CastOperation>(opt);
		process(cast, block, frame);
		return;
	}
	case ir::Operation::OperationType::ModOp: {
		auto cast = as<ir::ModOperation>(opt);
		process(cast, block, frame);
		return;
	}
	case ir::Operation::OperationType::BinaryComp: {
		auto cast = as<ir::BinaryCompOperation>(opt);
		process(cast, block, frame);
		return;
	}
	case ir::Operation::OperationType::ShiftOp: {
		auto cast = as<ir::ShiftOperation>(opt);
		process(cast, block, frame);
		return;
	}
	default: {

		throw NotImplementedException("This type is not supported.");
		// NES_THROW_RUNTIME_ERROR("Operation " << opt->toString() << " not
		// handled");
		return;
	}
	}
}

void BCLoweringProvider::LoweringContext::process(ir::ProxyCallOperation* opt, short block, RegisterFrame& frame) {
	// create a dynamic call using dyncall.h
	processDynamicCall(opt, block, frame);
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
	defaultRegisterFile[funcInfoRegister] = (int64_t) opt->getFunctionPtr();

	if (opt->getStamp() != Type::v) {
		auto resultRegister = getResultRegister(opt, frame);
		frame.setValue(opt->getIdentifier(), resultRegister);
		code.emplace_back(bc, funcInfoRegister, -1, resultRegister);
	} else {
		code.emplace_back(bc, funcInfoRegister, -1, -1);
	}
}

void BCLoweringProvider::LoweringContext::process(ir::NotOperation* negateOperation, short block,
                                                  RegisterFrame& frame) {
	auto input = frame.getValue(negateOperation->getInput()->getIdentifier());
	auto resultReg = getResultRegister(negateOperation, frame);
	frame.setValue(negateOperation->getIdentifier(), resultReg);
	ByteCode bc = ByteCode::NOT_b;
	OpCode oc = {bc, input, -1, resultReg};
	program.blocks[block].code.emplace_back(oc);
}

void BCLoweringProvider::LoweringContext::process(ir::NegateOperation* negateOperation, short block,
                                                  RegisterFrame& frame) {
	auto input = frame.getValue(negateOperation->getInput()->getIdentifier());
	auto resultReg = getResultRegister(negateOperation, frame);
	frame.setValue(negateOperation->getIdentifier(), resultReg);
	ByteCode bc = ByteCode::BNEGATE_I64;
	OpCode oc = {bc, input, -1, resultReg};
	program.blocks[block].code.emplace_back(oc);
}

void BCLoweringProvider::LoweringContext::process(ir::CastOperation* castOp, short block, RegisterFrame& frame) {
	auto input = frame.getValue(castOp->getInput()->getIdentifier());
	auto resultReg = getResultRegister(castOp, frame);
	frame.setValue(castOp->getIdentifier(), resultReg);
	auto srcType = (castOp->getInput()->getStamp());
	auto tar = (castOp->getStamp());
	ByteCode bc;
	if (srcType == Type::i8) {
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
			bc = ByteCode::CAST_i32_ui64;
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

} // namespace nautilus::compiler::bc


#include "nautilus/compiler/backends/mlir/MLIRLoweringProvider.hpp"
#include "nautilus/compiler/ir/operations/ArithmeticOperations/ModOperation.hpp"
#include "nautilus/exceptions/NotImplementedException.hpp"
#include <llvm/ADT/StringRef.h>
#include <llvm/Transforms/Utils/Cloning.h>
#include <mlir/Dialect/Arith/IR/Arith.h>
#include <mlir/Dialect/ControlFlow/IR/ControlFlow.h>
#include <mlir/Dialect/ControlFlow/IR/ControlFlowOps.h>
#include <mlir/Dialect/Func/IR/FuncOps.h>
#include <mlir/Dialect/LLVMIR/LLVMDialect.h>
#include <mlir/Dialect/LLVMIR/LLVMTypes.h>
#include <mlir/Dialect/SCF/IR/SCF.h>
#include <mlir/IR/Attributes.h>
#include <mlir/IR/Builders.h>
#include <mlir/IR/BuiltinAttributes.h>
#include <mlir/IR/Location.h>
#include <mlir/IR/Operation.h>
#include <mlir/IR/PatternMatch.h>
#include <mlir/IR/TypeRange.h>
#include <mlir/IR/Value.h>
#include <mlir/IR/Verifier.h>
#include <filesystem>

#include "llvm/BinaryFormat/Dwarf.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/Path.h"

namespace nautilus::compiler::mlir {
using namespace ::mlir;

//==-----------------------==//
//==-- UTILITY FUNCTIONS --==//
//==-----------------------==//
::mlir::Type MLIRLoweringProvider::getMLIRType(Type type) {
	switch (type) {
	case Type::v:
		return ::mlir::LLVM::LLVMVoidType::get(context);
	case Type::b:
		return builder->getI1Type();
	case Type::ui8:
	case Type::i8:
		return builder->getI8Type();
	case Type::ui16:
	case Type::i16:
		return builder->getI16Type();
	case Type::ui32:
	case Type::i32:
		return builder->getI32Type();
	case Type::ui64:
	case Type::i64:
		return builder->getI64Type();
	case Type::f32:
		return builder->getF32Type();
	case Type::f64:
		return builder->getF64Type();
	case Type::ptr:
		return mlir::LLVM::LLVMPointerType::get(context);
	}

	throw NotImplementedException("No matching type for stamp ");
}

std::vector<mlir::Type> MLIRLoweringProvider::getMLIRType(std::vector<ir::Operation*> types) {
	std::vector<mlir::Type> resultTypes;
	for (auto& type : types) {
		resultTypes.push_back(getMLIRType(type->getStamp()));
	}
	return resultTypes;
}

mlir::Location MLIRLoweringProvider::getLocationFromOperation(const ir::Operation* op) {
	if (op->hasLocation()) {
		auto fileLoc = op->getLocation();
		auto mlirFileLineColLoc = mlir::FileLineColLoc::get( builder->getStringAttr(fileLoc.filename), fileLoc.line, fileLoc.column);
		//return mlirFileLineColLoc;
		;

		return mlir::NameLoc::get(builder->getStringAttr(op->getIdentifier().toString()), mlirFileLineColLoc);
	}
	return mlir::UnknownLoc::get(builder->getContext());
}

mlir::arith::CmpIPredicate convertToIntMLIRComparison(ir::CompareOperation::Comparator comparisonType, Type& stamp) {
	if (isSignedInteger(stamp)) {
		switch (comparisonType) {
		case (ir::CompareOperation::Comparator::EQ):
			return mlir::arith::CmpIPredicate::eq;
		case (ir::CompareOperation::Comparator::NE):
			return mlir::arith::CmpIPredicate::ne;
		case (ir::CompareOperation::Comparator::LT):
			return mlir::arith::CmpIPredicate::slt;
		case (ir::CompareOperation::Comparator::LE):
			return mlir::arith::CmpIPredicate::sle;
		case (ir::CompareOperation::Comparator::GT):
			return mlir::arith::CmpIPredicate::sgt;
		case (ir::CompareOperation::Comparator::GE):
			return mlir::arith::CmpIPredicate::sge;
		default:
			return mlir::arith::CmpIPredicate::slt;
		}
	} else if (isUnsignedInteger(stamp)) {
		switch (comparisonType) {
		case (ir::CompareOperation::Comparator::EQ):
			return mlir::arith::CmpIPredicate::eq;
		case (ir::CompareOperation::Comparator::NE):
			return mlir::arith::CmpIPredicate::ne;
		case (ir::CompareOperation::Comparator::LT):
			return mlir::arith::CmpIPredicate::ult;
		case (ir::CompareOperation::Comparator::LE):
			return mlir::arith::CmpIPredicate::ule;
		case (ir::CompareOperation::Comparator::GT):
			return mlir::arith::CmpIPredicate::ugt;
		case (ir::CompareOperation::Comparator::GE):
			return mlir::arith::CmpIPredicate::uge;
		default:
			return mlir::arith::CmpIPredicate::ult;
		}
	}
	throw RuntimeException("");
}

mlir::arith::CmpFPredicate convertToFloatMLIRComparison(ir::CompareOperation::Comparator comparisonType) {
	switch (comparisonType) {
	// the U in U(LT/LE/..) stands for unordered, not unsigned! Float comparisons
	// are always signed.
	case (ir::CompareOperation::Comparator::LT):
		return mlir::arith::CmpFPredicate::OLT;
	case (ir::CompareOperation::Comparator::LE):
		return mlir::arith::CmpFPredicate::OLE;
	case (ir::CompareOperation::Comparator::EQ):
		return mlir::arith::CmpFPredicate::OEQ;
	case (ir::CompareOperation::Comparator::GT):
		return mlir::arith::CmpFPredicate::OGT;
	case (ir::CompareOperation::Comparator::GE):
		return mlir::arith::CmpFPredicate::OGE;
	case (ir::CompareOperation::Comparator::NE):
		return mlir::arith::CmpFPredicate::ONE;
	default:
		return mlir::arith::CmpFPredicate::OLT;
	}
}

mlir::LLVM::ICmpPredicate convertToLLVMComparison(ir::CompareOperation::Comparator comparisonType) {
	switch (comparisonType) {
	// the U in U(LT/LE/..) stands for unordered, not unsigned! Float comparisons
	// are always signed.
	case (ir::CompareOperation::Comparator::LT):
		return mlir::LLVM::ICmpPredicate::ult;
	case (ir::CompareOperation::Comparator::LE):
		return mlir::LLVM::ICmpPredicate::ule;
	case (ir::CompareOperation::Comparator::EQ):
		return mlir::LLVM::ICmpPredicate::eq;
	case (ir::CompareOperation::Comparator::GT):
		return mlir::LLVM::ICmpPredicate::ugt;
	case (ir::CompareOperation::Comparator::GE):
		return mlir::LLVM::ICmpPredicate::uge;
	case (ir::CompareOperation::Comparator::NE):
		return mlir::LLVM::ICmpPredicate::ne;
	default:
		assert(false);
	}
}

mlir::arith::CmpIPredicate convertToBooleanMLIRComparison(ir::CompareOperation::Comparator comparisonType) {
	switch (comparisonType) {
	// the U in U(LT/LE/..) stands for unordered, not unsigned! Float comparisons
	// are always signed.
	case (ir::CompareOperation::Comparator::EQ):
		return mlir::arith::CmpIPredicate::eq;
	case (ir::CompareOperation::Comparator::NE):
		return mlir::arith::CmpIPredicate::ne;
	case ir::CompareOperation::LT:
		return mlir::arith::CmpIPredicate::slt;
	case ir::CompareOperation::LE:
		return mlir::arith::CmpIPredicate::sle;
	case ir::CompareOperation::GT:
		return mlir::arith::CmpIPredicate::sgt;
	case ir::CompareOperation::GE:
		return mlir::arith::CmpIPredicate::sge;
	default:
		assert(false);
	}
}

mlir::FlatSymbolRefAttr MLIRLoweringProvider::insertExternalFunction(const std::string& name, void* functionPtr, mlir::Type resultType, std::vector<mlir::Type> argTypes, bool varArgs) {
	// Create function arg & result types (currently only int for result).
	mlir::LLVM::LLVMFunctionType llvmFnType = mlir::LLVM::LLVMFunctionType::get(resultType, argTypes, varArgs);

	// The InsertionGuard saves the current insertion point (IP) and restores it
	// after scope is left.
	mlir::PatternRewriter::InsertionGuard insertGuard(*builder);
	builder->restoreInsertionPoint(*globalInsertPoint);
	// Create function in global scope. Return reference.
	builder->create<mlir::LLVM::LLVMFuncOp>(theModule.getLoc(), name, llvmFnType, mlir::LLVM::Linkage::External, false);

	jitProxyFunctionSymbols.push_back(name);
	if (functionPtr == nullptr) {
		functionPtr = ProxyFunctions.getProxyFunctionAddress(name);
	}
	jitProxyFunctionTargetAddresses.push_back(functionPtr);
	return mlir::SymbolRefAttr::get(context, name);
}

//==---------------------------------==//
//==-- MAIN WORK - Generating MLIR --==//
//==---------------------------------==//
MLIRLoweringProvider::MLIRLoweringProvider(mlir::MLIRContext& context) : context(&context) {
	// Create builder object, which helps to generate MLIR. Create Module, which
	// contains generated MLIR.
	builder = std::make_unique<mlir::OpBuilder>(&context);
	builder->getContext()->loadDialect<mlir::cf::ControlFlowDialect>();
	builder->getContext()->loadDialect<mlir::LLVM::LLVMDialect>();
	builder->getContext()->loadDialect<mlir::func::FuncDialect>();
	builder->getContext()->loadDialect<mlir::scf::SCFDialect>();

	auto mlirFileLineColLoc = mlir::FileLineColLoc::get(builder->getStringAttr("/var/folders/jf/4514n07j3mz7nywrfrzqttym0000gp/T/dump/2024-09-01_22-08-04_#26627F0/final_ir.ir"), 0, 0);

	this->theModule = mlir::ModuleOp::create(mlirFileLineColLoc);
	// Store InsertPoint for inserting globals such as Strings or TupleBuffers.
	globalInsertPoint = new mlir::RewriterBase::InsertPoint(theModule.getBody(), theModule.begin());
}

MLIRLoweringProvider::~MLIRLoweringProvider() {
	delete globalInsertPoint;
}

mlir::OwningOpRef<mlir::ModuleOp> MLIRLoweringProvider::generateModuleFromIR(std::shared_ptr<ir::IRGraph> ir) {
	ValueFrame firstFrame;
	this->generateMLIR(ir->getRootOperation(), firstFrame);
	// If MLIR module creation is incorrect, gracefully emit error message, return
	// nullptr, and continue.
	if (failed(mlir::verify(theModule))) {
		theModule.emitError("module verification error");
		return nullptr;
	}
	return theModule;
}

void MLIRLoweringProvider::generateMLIR(const ir::BasicBlock* basicBlock, ValueFrame& frame) {
	for (const auto& operation : basicBlock->getOperations()) {
		generateMLIR(operation, frame);
	}
}

void MLIRLoweringProvider::generateMLIR(const std::unique_ptr<ir::Operation>& operation, ValueFrame& frame) {
	switch (operation->getOperationType()) {
	case ir::Operation::OperationType::FunctionOp:
		generateMLIR(as<ir::FunctionOperation>(operation), frame);
		break;
	case ir::Operation::OperationType::ConstIntOp:
		generateMLIR(as<ir::ConstIntOperation>(operation), frame);
		break;
	case ir::Operation::OperationType::ConstFloatOp:
		generateMLIR(as<ir::ConstFloatOperation>(operation), frame);
		break;
	case ir::Operation::OperationType::ConstPtrOp:
		generateMLIR(as<ir::ConstPtrOperation>(operation), frame);
		break;
	case ir::Operation::OperationType::AddOp:
		generateMLIR(as<ir::AddOperation>(operation), frame);
		break;
	case ir::Operation::OperationType::SubOp:
		generateMLIR(as<ir::SubOperation>(operation), frame);
		break;
	case ir::Operation::OperationType::MulOp:
		generateMLIR(as<ir::MulOperation>(operation), frame);
		break;
	case ir::Operation::OperationType::DivOp:
		generateMLIR(as<ir::DivOperation>(operation), frame);
		break;
	case ir::Operation::OperationType::ModOp:
		generateMLIR(as<ir::ModOperation>(operation), frame);
		break;
	case ir::Operation::OperationType::LoadOp:
		generateMLIR(as<ir::LoadOperation>(operation), frame);
		break;
	case ir::Operation::OperationType::StoreOp:
		generateMLIR(as<ir::StoreOperation>(operation), frame);
		break;
	case ir::Operation::OperationType::BranchOp:
		generateMLIR(as<ir::BranchOperation>(operation), frame);
		break;
	case ir::Operation::OperationType::IfOp:
		generateMLIR(as<ir::IfOperation>(operation), frame);
		break;
	case ir::Operation::OperationType::CompareOp:
		generateMLIR(as<ir::CompareOperation>(operation), frame);
		break;
	case ir::Operation::OperationType::ProxyCallOp:
		generateMLIR(as<ir::ProxyCallOperation>(operation), frame);
		break;
	case ir::Operation::OperationType::ReturnOp:
		generateMLIR(as<ir::ReturnOperation>(operation), frame);
		break;
	case ir::Operation::OperationType::OrOp:
		generateMLIR(as<ir::OrOperation>(operation), frame);
		break;
	case ir::Operation::OperationType::AndOp:
		generateMLIR(as<ir::AndOperation>(operation), frame);
		break;
	case ir::Operation::OperationType::NegateOp:
		generateMLIR(as<ir::NegateOperation>(operation), frame);
		break;
	case ir::Operation::OperationType::NotOp:
		generateMLIR(as<ir::NotOperation>(operation), frame);
		break;
	case ir::Operation::OperationType::CastOp:
		generateMLIR(as<ir::CastOperation>(operation), frame);
		break;
	case ir::Operation::OperationType::ConstBooleanOp:
		generateMLIR(as<ir::ConstBooleanOperation>(operation), frame);
		break;
	case ir::Operation::OperationType::BinaryComp:
		generateMLIR(as<ir::BinaryCompOperation>(operation), frame);
		break;
	case ir::Operation::OperationType::ShiftOp:
		generateMLIR(as<ir::ShiftOperation>(operation), frame);
		break;
	default: {
		throw NotImplementedException("");
	}
	}
}

void MLIRLoweringProvider::generateMLIR(ir::NegateOperation* negateOperation, ValueFrame& frame) {
	auto input = frame.getValue(negateOperation->getInput()->getIdentifier());
	auto constInt = builder->create<mlir::arith::ConstantOp>(getLocationFromOperation(negateOperation), input.getType(), builder->getIntegerAttr(input.getType(), ~0));
	auto negate = builder->create<mlir::arith::XOrIOp>(getLocationFromOperation(negateOperation), input, constInt);
	frame.setValue(negateOperation->getIdentifier(), negate);
}

void MLIRLoweringProvider::generateMLIR(ir::NotOperation* notOperation, ValueFrame& frame) {
	auto input = frame.getValue(notOperation->getInput()->getIdentifier());
	auto constTrue = builder->create<mlir::LLVM::ConstantOp>(getLocationFromOperation(notOperation), builder->getI1Type(), builder->getIntegerAttr(builder->getIndexType(), true));
	auto negate = builder->create<mlir::arith::XOrIOp>(getLocationFromOperation(notOperation), input, constTrue);
	frame.setValue(notOperation->getIdentifier(), negate);
}

void MLIRLoweringProvider::generateMLIR(ir::OrOperation* orOperation, ValueFrame& frame) {
	auto leftInput = frame.getValue(orOperation->getLeftInput()->getIdentifier());
	auto rightInput = frame.getValue(orOperation->getRightInput()->getIdentifier());
	auto mlirOrOp = builder->create<mlir::LLVM::OrOp>(getLocationFromOperation(orOperation), leftInput, rightInput);
	frame.setValue(orOperation->

	               getIdentifier(),
	               mlirOrOp

	);
}

void MLIRLoweringProvider::generateMLIR(ir::AndOperation* andOperation, ValueFrame& frame) {
	auto leftInput = frame.getValue(andOperation->getLeftInput()->getIdentifier());
	auto rightInput = frame.getValue(andOperation->getRightInput()->getIdentifier());
	auto mlirAndOp = builder->create<mlir::LLVM::AndOp>(getLocationFromOperation(andOperation), leftInput, rightInput);
	frame.setValue(andOperation->getIdentifier(), mlirAndOp);
}

void MLIRLoweringProvider::generateMLIR(ir::FunctionOperation* functionOp, ValueFrame& frame) {
	// Generate execute function. Set input/output types and get its entry block.
	llvm::SmallVector<mlir::Type> inputTypes(0);
	for (auto& inputArg : functionOp->getFunctionBasicBlock()->getArguments()) {
		inputTypes.emplace_back(getMLIRType(inputArg->getStamp()));
	}
	llvm::SmallVector<mlir::Type> outputTypes(1, getMLIRType(functionOp->getOutputArg()));
	;
	auto functionInOutTypes = builder->getFunctionType(inputTypes, outputTypes);
	auto mlirFileLineColLoc = mlir::FileLineColLoc::get(builder->getStringAttr("/var/folders/jf/4514n07j3mz7nywrfrzqttym0000gp/T/dump/2024-09-01_22-08-04_#26627F0/final_ir.ir"), 0, 0);

	auto mlirFunction = builder->create<mlir::func::FuncOp>(mlirFileLineColLoc, functionOp->getName(), functionInOutTypes);

	// Avoid function name mangling.
	mlirFunction->setAttr("llvm.emit_c_interface", mlir::UnitAttr::get(context));
	if (isUnsignedInteger(functionOp->getStamp())) {
		mlirFunction.setResultAttr(0, "llvm.zeroext", mlir::UnitAttr::get(context));
	} else if (isSignedInteger(functionOp->getStamp())) {
		mlirFunction.setResultAttr(0, "llvm.signext", mlir::UnitAttr::get(context));
	}
	// mlirFunction.setArgAttr(0, "llvm.signext",  mlir::UnitAttr::get(context));

	mlirFunction.addEntryBlock();

	// Set InsertPoint to beginning of the execute function.
	builder->setInsertionPointToStart(&mlirFunction.getBody().front());

	// Store references to function args in the valueMap map.
	auto valueMapIterator = mlirFunction.args_begin();
	for (int i = 0; i < (int) functionOp->getFunctionBasicBlock()->getArguments().size(); ++i) {
		frame.setValue(functionOp->getFunctionBasicBlock()->getArguments().at(i)->getIdentifier(), valueMapIterator[i]);
	}

	// Generate MLIR for operations in function body (BasicBlock).
	generateMLIR(functionOp->getFunctionBasicBlock(), frame);

	theModule.push_back(mlirFunction);
}

void MLIRLoweringProvider::generateMLIR(ir::LoadOperation* loadOp, ValueFrame& frame) {

	auto address = frame.getValue(loadOp->getAddress()->getIdentifier());

	// auto bitcast = builder->create<mlir::LLVM::BitcastOp>(getNameLoc("Bitcasted
	// address"),
	//                                                       mlir::LLVM::LLVMPointerType::get(context),
	//                                                       address);
	auto mlirLoadOp = builder->create<mlir::LLVM::LoadOp>(getLocationFromOperation(loadOp), getMLIRType(loadOp->getStamp()), address);
	frame.setValue(loadOp->getIdentifier(), mlirLoadOp);
}

void MLIRLoweringProvider::generateMLIR(ir::ConstIntOperation* constIntOp, ValueFrame& frame) {
	auto type = getMLIRType(constIntOp->getStamp());
	auto constOp = builder->create<mlir::arith::ConstantOp>(getLocationFromOperation(constIntOp), type, builder->getIntegerAttr(type, constIntOp->getValue()));
	frame.setValue(constIntOp->getIdentifier(), constOp);
}

void MLIRLoweringProvider::generateMLIR(ir::ConstPtrOperation* constPtr, ValueFrame& frame) {
	int64_t val = (int64_t) constPtr->getValue();
	auto constInt = builder->create<mlir::arith::ConstantOp>(getLocationFromOperation(constPtr), builder->getI64Type(), builder->getIntegerAttr(builder->getI64Type(), val));
	auto elementAddress = builder->create<mlir::LLVM::IntToPtrOp>(getLocationFromOperation(constPtr), mlir::LLVM::LLVMPointerType::get(context), constInt);
	frame.setValue(constPtr->getIdentifier(), elementAddress);
}

void MLIRLoweringProvider::generateMLIR(ir::ConstFloatOperation* constFloatOp, ValueFrame& frame) {
	if (isFloat(constFloatOp->getStamp())) {
		auto floatType = (constFloatOp->getStamp() == Type::f32) ? builder->getF32Type() : builder->getF64Type();
		frame.setValue(constFloatOp->getIdentifier(), builder->create<mlir::LLVM::ConstantOp>(getLocationFromOperation(constFloatOp), floatType, builder->getFloatAttr(floatType, constFloatOp->getValue())));
	}
}

//==---------------------------==//
//==-- ARITHMETIC OPERATIONS --==//
//==---------------------------==//
void MLIRLoweringProvider::generateMLIR(ir::AddOperation* addOp, ValueFrame& frame) {
	auto leftInput = frame.getValue(addOp->getLeftInput()->getIdentifier());
	auto rightInput = frame.getValue(addOp->getRightInput()->getIdentifier());
	auto loc = getLocationFromOperation(addOp);
	if (addOp->getLeftInput()->getStamp() == Type::ptr) {
		// if we add something to a ptr we have to use a llvm getelementptr
		mlir::Value elementAddress = builder->create<mlir::LLVM::GEPOp>(loc, mlir::LLVM::LLVMPointerType::get(context), builder->getI8Type(), leftInput, mlir::ArrayRef<mlir::Value>({rightInput}));
		frame.setValue(addOp->getIdentifier(), elementAddress);

	} else if (isFloat(addOp->getStamp())) {
		auto mlirAddOp = builder->create<mlir::LLVM::FAddOp>(loc, leftInput.getType(), leftInput, rightInput, mlir::LLVM::FastmathFlags::fast);
		frame.setValue(addOp->getIdentifier(), mlirAddOp);
	} else {
		if (!inductionVars.contains(addOp->getLeftInput()->getIdentifier())) {
			if (!frame.contains(addOp->getIdentifier())) {
				auto mlirAddOp = builder->create<mlir::LLVM::AddOp>(loc, leftInput, rightInput);
				frame.setValue(addOp->getIdentifier(), mlirAddOp);
			} else {
				auto mlirAddOp = builder->create<mlir::LLVM::AddOp>(loc, leftInput, rightInput);
				frame.setValue(addOp->getIdentifier(), mlirAddOp);
			}
		}
	}
}

void MLIRLoweringProvider::generateMLIR(ir::SubOperation* subIntOp, ValueFrame& frame) {
	auto leftInput = frame.getValue(subIntOp->getLeftInput()->getIdentifier());
	auto rightInput = frame.getValue(subIntOp->getRightInput()->getIdentifier());
	auto loc = getLocationFromOperation(subIntOp);
	if (subIntOp->getLeftInput()->getStamp() == Type::ptr) {
		// if we add something to a ptr we have to use a llvm getelementptr
		mlir::Value elementAddress = builder->create<mlir::LLVM::GEPOp>(loc, mlir::LLVM::LLVMPointerType::get(context), builder->getI8Type(), leftInput, mlir::ArrayRef<mlir::Value>({rightInput}));
		frame.setValue(subIntOp->getIdentifier(), elementAddress);

	} else if (isFloat(subIntOp->getStamp())) {
		auto mlirSubOp = builder->create<mlir::LLVM::FSubOp>(loc, leftInput, rightInput, mlir::LLVM::FastmathFlagsAttr::get(context, mlir::LLVM::FastmathFlags::fast));
		frame.setValue(subIntOp->getIdentifier(), mlirSubOp);
	} else {
		auto mlirSubOp = builder->create<mlir::LLVM::SubOp>(loc, leftInput, rightInput);
		frame.setValue(subIntOp->getIdentifier(), mlirSubOp);
	}
}

void MLIRLoweringProvider::generateMLIR(ir::MulOperation* mulOp, ValueFrame& frame) {
	auto leftInput = frame.getValue(mulOp->getLeftInput()->getIdentifier());
	auto rightInput = frame.getValue(mulOp->getRightInput()->getIdentifier());
	auto resultType = leftInput.getType();
	auto loc = getLocationFromOperation(mulOp);
	if (isFloat(mulOp->getStamp())) {
		auto mlirMulOp = builder->create<mlir::LLVM::FMulOp>(loc, resultType, leftInput, rightInput, mlir::LLVM::FastmathFlags::fast);
		frame.setValue(mulOp->getIdentifier(), mlirMulOp);
	} else {
		auto mlirMulOp = builder->create<mlir::LLVM::MulOp>(loc, resultType, leftInput, rightInput);
		frame.setValue(mulOp->getIdentifier(), mlirMulOp);
	}
}

void MLIRLoweringProvider::generateMLIR(ir::DivOperation* divIntOp, ValueFrame& frame) {
	auto leftInput = frame.getValue(divIntOp->getLeftInput()->getIdentifier());
	auto rightInput = frame.getValue(divIntOp->getRightInput()->getIdentifier());
	auto resultType = leftInput.getType();
	auto loc = getLocationFromOperation(divIntOp);
	if (isFloat(divIntOp->getStamp())) {
		auto mlirDivOp = builder->create<mlir::LLVM::FDivOp>(loc, resultType, leftInput, rightInput, mlir::LLVM::FastmathFlags::fast);
		frame.setValue(divIntOp->getIdentifier(), mlirDivOp);
	} else {
		if (resultType.isSignedInteger()) {
			auto mlirDivOp = builder->create<mlir::LLVM::UDivOp>(loc, resultType, leftInput, rightInput);
			frame.setValue(divIntOp->getIdentifier(), mlirDivOp);
		} else {
			auto mlirDivOp = builder->create<mlir::LLVM::SDivOp>(loc, resultType, leftInput, rightInput);
			frame.setValue(divIntOp->getIdentifier(), mlirDivOp);
		}
	}
}

void MLIRLoweringProvider::generateMLIR(ir::ModOperation* divIntOp, ValueFrame& frame) {
	auto leftInput = frame.getValue(divIntOp->getLeftInput()->getIdentifier());
	auto rightInput = frame.getValue(divIntOp->getRightInput()->getIdentifier());
	auto resultType = leftInput.getType();
	auto loc = getLocationFromOperation(divIntOp);
	if (isFloat(divIntOp->getStamp())) {
		auto mlirDivOp = builder->create<mlir::LLVM::FRemOp>(loc, resultType, leftInput, rightInput, mlir::LLVM::FastmathFlags::fast);
		frame.setValue(divIntOp->getIdentifier(), mlirDivOp);
	} else {
		if (resultType.isSignedInteger()) {
			auto mlirDivOp = builder->create<mlir::arith::RemSIOp>(loc, resultType, leftInput, rightInput);
			frame.setValue(divIntOp->getIdentifier(), mlirDivOp);
		} else {
			auto mlirDivOp = builder->create<mlir::arith::RemUIOp>(loc, resultType, leftInput, rightInput);
			frame.setValue(divIntOp->getIdentifier(), mlirDivOp);
		}
	}
}

void MLIRLoweringProvider::generateMLIR(ir::StoreOperation* storeOp, ValueFrame& frame) {
	auto value = frame.getValue(storeOp->getValue()->getIdentifier());
	auto address = frame.getValue(storeOp->getAddress()->getIdentifier());
	builder->create<mlir::LLVM::StoreOp>(getLocationFromOperation(storeOp), value, address);
}

void MLIRLoweringProvider::generateMLIR(ir::ReturnOperation* returnOp, ValueFrame& frame) {
	// Insert return into 'execute' function block. This is the FINAL return.
	if (!returnOp->hasReturnValue()) {
		builder->create<mlir::LLVM::ReturnOp>(getLocationFromOperation(returnOp), mlir::ValueRange());
	} else {
		builder->create<mlir::LLVM::ReturnOp>(getLocationFromOperation(returnOp), frame.getValue(returnOp->getReturnValue()->getIdentifier()));
	}
}

void MLIRLoweringProvider::generateMLIR(ir::ProxyCallOperation* proxyCallOp, ValueFrame& frame) {
	mlir::FlatSymbolRefAttr functionRef;
	if (theModule.lookupSymbol<mlir::LLVM::LLVMFuncOp>(proxyCallOp->getFunctionSymbol())) {
		functionRef = mlir::SymbolRefAttr::get(context, proxyCallOp->getFunctionSymbol());
	} else {
		functionRef = insertExternalFunction(proxyCallOp->getFunctionSymbol(), proxyCallOp->getFunctionPtr(), getMLIRType(proxyCallOp->getStamp()), getMLIRType(proxyCallOp->getInputArguments()), true);
	}

	std::vector<mlir::Value> functionArgs;
	for (const auto& arg : proxyCallOp->getInputArguments()) {
		functionArgs.push_back(frame.getValue(arg->getIdentifier()));
	}
	if (proxyCallOp->getStamp() != Type::v) {
		auto res = builder->create<mlir::LLVM::CallOp>(getLocationFromOperation(proxyCallOp), getMLIRType(proxyCallOp->getStamp()), functionRef, functionArgs);
		/*
		auto fileAttr = mlir::LLVM::DIFileAttr::get(builder->getContext(), "lastSnapShotFile", std::filesystem::current_path().string());
		auto compileUnitAttr = mlir::LLVM::DICompileUnitAttr::get(builder->getContext(), llvm::dwarf::DW_LANG_C, fileAttr, mlir::StringAttr::get(builder->getContext(), "LingoDB"), true, mlir::LLVM::DIEmissionKind::Full);

		auto subroutineType = mlir::LLVM::DISubroutineTypeAttr::get(builder->getContext(), {});
		auto subProgramAttr = mlir::LLVM::DISubprogramAttr::get(compileUnitAttr, fileAttr, proxyCallOp->getFunctionName(), proxyCallOp->getFunctionName(), fileAttr, 0, 0, mlir::LLVM::DISubprogramFlags::Definition | mlir::LLVM::DISubprogramFlags::Optimized, subroutineType);

		auto scope = mlir::FusedLocWith<mlir::LLVM::DIScopeAttr>::get(res->getLoc(), subProgramAttr, builder->getContext());
		auto x = ::mlir::StringAttr::get(context, proxyCallOp->getIdentifier().toString());

		auto atr = LLVM::DIDerivedTypeAttr::get(
		    builder->getContext(), llvm::dwarf::DW_TAG_typedef,
		    builder->getStringAttr("int32_t"), LLVM::DIBasicTypeAttr::get(
		                                          context, llvm::dwarf::DW_TAG_base_type, "int",
		//                                         sizeInBits=32, llvm::dwarf::DW_ATE_signed_char), sizeInBits=0,
		 //   alignInBits=0, offsetInBits=0);

	//	mlir::LLVM::DIFileAttr funcFileAttr =
	//	    mlir::LLVM::DIFileAttr::get(context, "fileName", "filePath");


	//	auto var = ::mlir::LLVM::DILocalVariableAttr::get(builder->getContext(), scope, x,funcFileAttr,  0,0,0,atr);

		//builder->create<mlir::UnrealizedConversionCastOp>(builder->getUnknownLoc(),res )

		    builder->create<mlir::LLVM::DbgValueOp>(res.getLoc(), res.getResult(), var);*/
		frame.setValue(proxyCallOp->getIdentifier(), res.getResult());
	} else {
		builder->create<mlir::LLVM::CallOp>(builder->getUnknownLoc(), mlir::TypeRange(), functionRef, functionArgs);
	}
}

void MLIRLoweringProvider::generateMLIR(ir::CompareOperation* compareOp, ValueFrame& frame) {
	auto leftStamp = compareOp->getLeftInput()->getStamp();
	auto rightStamp = compareOp->getRightInput()->getStamp();

	if ((isInteger(leftStamp) && isFloat(rightStamp)) || ((isInteger(rightStamp) && isFloat(leftStamp)))) {
		// Avoid comparing integer to float
		throw NotImplementedException("Type missmatch: cannot compare");
	} else if (compareOp->getComparator() == ir::CompareOperation::EQ && compareOp->getLeftInput()->getStamp() == Type::ptr && isInteger(compareOp->getRightInput()->getStamp())) {
		// add null check
		throw NotImplementedException("Null check is not implemented");
		// auto null =
		//         builder->create<mlir::LLVM::NullOp>(getNameLoc("null"),
		//         mlir::LLVM::LLVMPointerType::get(context));
		// auto cmpOp =
		// builder->create<mlir::LLVM::ICmpOp>(getNameLoc("comparison"),
		//                                                  mlir::LLVM::ICmpPredicate::eq,
		//                                                  frame.getValue(compareOp->getLeftInput()->getIdentifier()),
		//                                                  null);
		// frame.setValue(compareOp->getIdentifier(), cmpOp);
	} else if (isInteger(leftStamp) && isInteger(rightStamp)) {
		// handle integer
		auto cmpOp = builder->create<mlir::arith::CmpIOp>(getLocationFromOperation(compareOp), convertToIntMLIRComparison(compareOp->getComparator(), leftStamp), frame.getValue(compareOp->getLeftInput()->getIdentifier()),
		                                                  frame.getValue(compareOp->getRightInput()->getIdentifier()));
		frame.setValue(compareOp->getIdentifier(), cmpOp);
	} else if (isFloat(leftStamp) && isFloat(rightStamp)) {
		// handle float comparison
		auto cmpOp = builder->create<mlir::arith::CmpFOp>(getLocationFromOperation(compareOp), convertToFloatMLIRComparison(compareOp->getComparator()), frame.getValue(compareOp->getLeftInput()->getIdentifier()),
		                                                  frame.getValue(compareOp->getRightInput()->getIdentifier()));
		frame.setValue(compareOp->getIdentifier(), cmpOp);
	} else if (leftStamp == Type::b && rightStamp == Type::b) {
		// handle float comparison
		auto cmpOp = builder->create<mlir::arith::CmpIOp>(getLocationFromOperation(compareOp), convertToBooleanMLIRComparison(compareOp->getComparator()), frame.getValue(compareOp->getLeftInput()->getIdentifier()),
		                                                  frame.getValue(compareOp->getRightInput()->getIdentifier()));
		frame.setValue(compareOp->getIdentifier(), cmpOp);
	} else if (leftStamp == Type::ptr && rightStamp == Type::ptr) {
		// handle float comparison
		auto cmpOp = builder->create<mlir::LLVM::ICmpOp>(getLocationFromOperation(compareOp), convertToLLVMComparison(compareOp->getComparator()), frame.getValue(compareOp->getLeftInput()->getIdentifier()),
		                                                 frame.getValue(compareOp->getRightInput()->getIdentifier()));
		frame.setValue(compareOp->getIdentifier(), cmpOp);
	} else {
		throw NotImplementedException("Unknown type to compare");
	}
}

void MLIRLoweringProvider::generateMLIR(ir::IfOperation* ifOp, ValueFrame& frame) {
	auto parentBlockInsertionPoint = builder->saveInsertionPoint();

	std::vector<mlir::Value> trueBlockArgs;
	mlir::Block* trueBlock = generateBasicBlock(ifOp->getTrueBlockInvocation(), frame);
	for (auto blockArg : ifOp->getTrueBlockInvocation().getArguments()) {
		trueBlockArgs.push_back(frame.getValue(blockArg->getIdentifier()));
	}

	std::vector<mlir::Value> elseBlockArgs;
	mlir::Block* elseBlock = generateBasicBlock(ifOp->getFalseBlockInvocation(), frame);
	for (auto blockArg : ifOp->getFalseBlockInvocation().getArguments()) {
		elseBlockArgs.push_back(frame.getValue(blockArg->getIdentifier()));
	}

	builder->restoreInsertionPoint(parentBlockInsertionPoint);
	builder->create<mlir::cf::CondBranchOp>(getLocationFromOperation(ifOp), frame.getValue(ifOp->getValue()->getIdentifier()), trueBlock, trueBlockArgs, elseBlock, elseBlockArgs);
}

void MLIRLoweringProvider::generateMLIR(ir::BranchOperation* branchOp, ValueFrame& frame) {
	std::vector<mlir::Value> mlirTargetBlockArguments;
	for (auto targetBlockArgument : branchOp->getNextBlockInvocation().getArguments()) {
		mlirTargetBlockArguments.push_back(frame.getValue(targetBlockArgument->getIdentifier()));
	}
	auto* mlirTargetBlock = generateBasicBlock(branchOp->getNextBlockInvocation(), frame);
	builder->create<mlir::cf::BranchOp>(getLocationFromOperation(branchOp), mlirTargetBlock, mlirTargetBlockArguments);
}

mlir::Block* MLIRLoweringProvider::generateBasicBlock(ir::BasicBlockInvocation& blockInvocation, ValueFrame&) {
	auto targetBlock = blockInvocation.getBlock();
	// Check if the block already exists.
	if (blockMapping.contains(targetBlock->getIdentifier())) {
		return blockMapping[targetBlock->getIdentifier()];
	}

	auto parentBlockInsertionPoint = builder->saveInsertionPoint();
	// Create new block.
	auto mlirBasicBlock = builder->createBlock(builder->getBlock()->getParent());

	auto& targetBlockArguments = targetBlock->getArguments();
	// Add attributes as arguments to block.
	for (auto& headBlockHeadTypes : targetBlockArguments) {
		mlirBasicBlock->addArgument(getMLIRType(headBlockHeadTypes->getStamp()), getLocationFromOperation(headBlockHeadTypes.get()));
	}
	ValueFrame blockFrame;
	for (uint32_t i = 0; i < targetBlockArguments.size(); i++) {
		blockFrame.setValue(targetBlock->getArguments()[i]->getIdentifier(), mlirBasicBlock->getArgument(i));
	}

	blockMapping[blockInvocation.getBlock()->getIdentifier()] = mlirBasicBlock;
	builder->setInsertionPointToStart(mlirBasicBlock);
	generateMLIR(targetBlock, blockFrame);
	builder->restoreInsertionPoint(parentBlockInsertionPoint);

	return mlirBasicBlock;
}

MLIRLoweringProvider::ValueFrame MLIRLoweringProvider::createFrameFromParentBlock(MLIRLoweringProvider::ValueFrame& frame, ir::BasicBlockInvocation& invocation) {
	auto invocationArguments = invocation.getArguments();
	auto& childBlockArguments = invocation.getBlock()->getArguments();
	// NES_ASSERT(invocationArguments.size() == childBlockArguments.size(),
	//            "the number of invocation parameters has to be the same as the
	//            number of block arguments in the invoked block.");
	ValueFrame childFrame;
	// Copy all frame values to the child frame that are arguments of the child
	// block.
	for (uint64_t i = 0; i < invocationArguments.size(); i++) {
		auto parentOperation = invocationArguments[i];
		auto parentValue = frame.getValue(parentOperation->getIdentifier());
		auto& childBlockArgument = childBlockArguments[i];
		childFrame.setValue(childBlockArgument->getIdentifier(), parentValue);
	}
	return childFrame;
}

void MLIRLoweringProvider::generateMLIR(ir::CastOperation* castOperation, MLIRLoweringProvider::ValueFrame& frame) {
	auto inputStamp = castOperation->getInput()->getStamp();
	auto outputStamp = castOperation->getStamp();
	auto mlirInput = frame.getValue(castOperation->getInput()->getIdentifier());
	auto outputType = getMLIRType(outputStamp);

	if (inputStamp == outputStamp) {
		// we skip the cast if input and output is the same stamp.
		frame.setValue(castOperation->getIdentifier(), mlirInput);
		return;
	}
	auto loc = getLocationFromOperation(castOperation);
	if (isFloat(inputStamp) && isFloat(outputStamp)) {
		if (getBitWith(inputStamp) == getBitWith(outputStamp)) {
			// we skip the cast if input and output is the same bitwith.
			frame.setValue(castOperation->getIdentifier(), mlirInput);
			return;
		} else if (getBitWith(inputStamp) < getBitWith(outputStamp)) {
			// upcast
			auto mlirCast = builder->create<mlir::arith::ExtFOp>(loc, outputType, mlirInput);
			frame.setValue(castOperation->getIdentifier(), mlirCast);
			return;
		} else if (getBitWith(inputStamp) > getBitWith(outputStamp)) {
			// downcast
			auto mlirCast = builder->create<mlir::arith::TruncFOp>(loc, outputType, mlirInput);
			frame.setValue(castOperation->getIdentifier(), mlirCast);
			return;
		}
	} else if (isInteger(inputStamp) && isFloat(outputStamp)) {
		if (isSignedInteger(inputStamp)) {
			// cast to signed int to float
			auto mlirCast = builder->create<mlir::arith::SIToFPOp>(loc, outputType, mlirInput);
			frame.setValue(castOperation->getIdentifier(), mlirCast);
			return;
		} else {
			// cast to signed int to double
			auto mlirCast = builder->create<mlir::arith::UIToFPOp>(loc, outputType, mlirInput);
			frame.setValue(castOperation->getIdentifier(), mlirCast);
			return;
		}
	} else if (isFloat(inputStamp) && isInteger(outputStamp)) {
		if (isSignedInteger(outputStamp)) {
			// cast float or double to signed int
			auto mlirCast = builder->create<mlir::arith::FPToSIOp>(loc, outputType, mlirInput);
			frame.setValue(castOperation->getIdentifier(), mlirCast);
			return;
		} else if (isUnsignedInteger(outputStamp)) {
			// cast float or double to unsigned int
			auto mlirCast = builder->create<mlir::arith::FPToUIOp>(loc, outputType, mlirInput);
			frame.setValue(castOperation->getIdentifier(), mlirCast);
			return;
		}
	} else if (isInteger(inputStamp) && isInteger(outputStamp) && getBitWith(inputStamp) == getBitWith(outputStamp)) {
		// we skip the cast if input and output is the same stamp.
		frame.setValue(castOperation->getIdentifier(), mlirInput);
		return;
	} else if (getBitWith(inputStamp) < getBitWith(outputStamp)) {
		// upcast
		if (isSignedInteger(inputStamp) && (isUnsignedInteger(outputStamp) || isSignedInteger(outputStamp))) {
			auto mlirCast = builder->create<mlir::arith::ExtSIOp>(loc, getMLIRType(outputStamp), mlirInput);
			frame.setValue(castOperation->getIdentifier(), mlirCast);
			return;
		} else if (isUnsignedInteger(inputStamp) && (isUnsignedInteger(outputStamp) || isSignedInteger(outputStamp))) {
			auto mlirCast = builder->create<mlir::arith::ExtUIOp>(loc, getMLIRType(outputStamp), mlirInput);
			frame.setValue(castOperation->getIdentifier(), mlirCast);
			return;
		}
	} else if (isInteger(inputStamp) && isInteger(outputStamp) && getBitWith(inputStamp) > getBitWith(outputStamp)) {
		// downcast
		auto mlirCast = builder->create<mlir::arith::TruncIOp>(loc, getMLIRType(outputStamp), mlirInput);
		frame.setValue(castOperation->getIdentifier(), mlirCast);
		return;
	}

	throw NotImplementedException("Cast is not supported.");
}

void MLIRLoweringProvider::generateMLIR(ir::BinaryCompOperation* binaryCompOperation, nautilus::compiler::mlir::MLIRLoweringProvider::ValueFrame& frame) {
	auto leftInput = frame.getValue(binaryCompOperation->getLeftInput()->getIdentifier());
	auto rightInput = frame.getValue(binaryCompOperation->getRightInput()->getIdentifier());
	auto loc = getLocationFromOperation(binaryCompOperation);
	mlir::Value op;
	switch (binaryCompOperation->getType()) {
	case ir::BinaryCompOperation::BAND:
		op = builder->create<mlir::arith::AndIOp>(loc, leftInput, rightInput);
		break;
	case ir::BinaryCompOperation::BOR:
		op = builder->create<mlir::arith::OrIOp>(loc, leftInput, rightInput);
		break;
	case ir::BinaryCompOperation::XOR:
		op = builder->create<mlir::arith::XOrIOp>(loc, leftInput, rightInput);
		break;
	}
	frame.setValue(binaryCompOperation->getIdentifier(), op);
}

void MLIRLoweringProvider::generateMLIR(ir::ShiftOperation* shiftOperation, nautilus::compiler::mlir::MLIRLoweringProvider::ValueFrame& frame) {
	auto leftInput = frame.getValue(shiftOperation->getLeftInput()->getIdentifier());
	auto rightInput = frame.getValue(shiftOperation->getRightInput()->getIdentifier());
	auto loc = getLocationFromOperation(shiftOperation);
	mlir::Value op;
	switch (shiftOperation->getType()) {
	case ir::ShiftOperation::LS:
		op = builder->create<mlir::arith::ShLIOp>(loc, leftInput, rightInput);
		break;
	case ir::ShiftOperation::RS:
		op = builder->create<mlir::arith::ShRSIOp>(loc, leftInput, rightInput);
		break;
	}
	frame.setValue(shiftOperation->getIdentifier(), op);
}

void MLIRLoweringProvider::generateMLIR(ir::ConstBooleanOperation* constBooleanOp, MLIRLoweringProvider::ValueFrame& frame) {
	auto loc = getLocationFromOperation(constBooleanOp);
	auto constOp = builder->create<mlir::arith::ConstantOp>(loc, builder->getI1Type(), builder->getIntegerAttr(builder->getI1Type(), constBooleanOp->getValue()));
	frame.setValue(constBooleanOp->getIdentifier(), constOp);
}

std::vector<std::string> MLIRLoweringProvider::getJitProxyFunctionSymbols() {
	return std::move(jitProxyFunctionSymbols);
}

std::vector<void*> MLIRLoweringProvider::getJitProxyTargetAddresses() {
	return std::move(jitProxyFunctionTargetAddresses);
}

} // namespace nautilus::compiler::mlir

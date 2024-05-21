
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

mlir::Value MLIRLoweringProvider::getConstInt(const std::string& location, Type stamp, int64_t value) {
	auto type = getMLIRType(stamp);
	return builder->create<mlir::arith::ConstantOp>(getNameLoc(location), type, builder->getIntegerAttr(type, value));
}

mlir::Value MLIRLoweringProvider::getConstBool(const std::string& location, bool value) {
	return builder->create<mlir::LLVM::ConstantOp>(getNameLoc(location), builder->getI1Type(),
	                                               builder->getIntegerAttr(builder->getIndexType(), value));
}

// Todo Issue #3004: Currently, we are simply adding 'Query_1' as the FileLineLoc name. Moreover,
//      the provided 'name' often is not meaningful either.
mlir::Location MLIRLoweringProvider::getNameLoc(const std::string& name) {
	auto baseLocation = mlir::FileLineColLoc::get(builder->getStringAttr("Query_1"), 0, 0);
	return mlir::NameLoc::get(builder->getStringAttr(name), baseLocation);
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
	// the U in U(LT/LE/..) stands for unordered, not unsigned! Float comparisons are always signed.
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

mlir::arith::CmpIPredicate convertToBooleanMLIRComparison(ir::CompareOperation::Comparator comparisonType) {
	switch (comparisonType) {
	// the U in U(LT/LE/..) stands for unordered, not unsigned! Float comparisons are always signed.
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

mlir::FlatSymbolRefAttr MLIRLoweringProvider::insertExternalFunction(const std::string& name, void* functionPtr,
                                                                     mlir::Type resultType,
                                                                     std::vector<mlir::Type> argTypes, bool varArgs) {
	// Create function arg & result types (currently only int for result).
	mlir::LLVM::LLVMFunctionType llvmFnType = mlir::LLVM::LLVMFunctionType::get(resultType, argTypes, varArgs);

	// The InsertionGuard saves the current insertion point (IP) and restores it after scope is left.
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
	// Create builder object, which helps to generate MLIR. Create Module, which contains generated MLIR.
	builder = std::make_unique<mlir::OpBuilder>(&context);
	builder->getContext()->loadDialect<mlir::cf::ControlFlowDialect>();
	builder->getContext()->loadDialect<mlir::LLVM::LLVMDialect>();
	builder->getContext()->loadDialect<mlir::func::FuncDialect>();
	builder->getContext()->loadDialect<mlir::scf::SCFDialect>();
	this->theModule = mlir::ModuleOp::create(getNameLoc("module"));
	// Store InsertPoint for inserting globals such as Strings or TupleBuffers.
	globalInsertPoint = new mlir::RewriterBase::InsertPoint(theModule.getBody(), theModule.begin());
};

mlir::OwningOpRef<mlir::ModuleOp> MLIRLoweringProvider::generateModuleFromIR(std::shared_ptr<ir::IRGraph> ir) {
	ValueFrame firstFrame;
	this->generateMLIR(ir->getRootOperation(), firstFrame);
	// If MLIR module creation is incorrect, gracefully emit error message, return nullptr, and continue.
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
	case ir::Operation::OperationType::CastOp:
		generateMLIR(as<ir::CastOperation>(operation), frame);
		break;
	case ir::Operation::OperationType::ConstBooleanOp:
		generateMLIR(as<ir::ConstBooleanOperation>(operation), frame);
		break;
	default: {
		throw NotImplementedException("");
	}
	}
}

void MLIRLoweringProvider::generateMLIR(ir::NegateOperation* negateOperation, ValueFrame& frame) {
	auto input = frame.getValue(negateOperation->getInput()->getIdentifier());
	auto negate = builder->create<mlir::arith::CmpIOp>(getNameLoc("comparison"), mlir::arith::CmpIPredicate::eq, input,
	                                                   getConstBool("bool", false));
	frame.setValue(negateOperation->

	               getIdentifier(),
	               negate

	);
}

void MLIRLoweringProvider::generateMLIR(ir::OrOperation* orOperation, ValueFrame& frame) {
	auto leftInput = frame.getValue(orOperation->getLeftInput()->getIdentifier());
	auto rightInput = frame.getValue(orOperation->getRightInput()->getIdentifier());
	auto mlirOrOp = builder->create<mlir::LLVM::OrOp>(getNameLoc("binOpResult"), leftInput, rightInput);
	frame.setValue(orOperation->

	               getIdentifier(),
	               mlirOrOp

	);
}

void MLIRLoweringProvider::generateMLIR(ir::AndOperation* andOperation, ValueFrame& frame) {
	auto leftInput = frame.getValue(andOperation->getLeftInput()->getIdentifier());
	auto rightInput = frame.getValue(andOperation->getRightInput()->getIdentifier());
	auto mlirAndOp = builder->create<mlir::LLVM::AndOp>(getNameLoc("binOpResult"), leftInput, rightInput);
	frame.setValue(andOperation->

	               getIdentifier(),
	               mlirAndOp

	);
}

void MLIRLoweringProvider::generateMLIR(ir::FunctionOperation* functionOp, ValueFrame& frame) {
	// Generate execute function. Set input/output types and get its entry block.
	llvm::SmallVector<mlir::Type> inputTypes(0);
	for (auto& inputArg : functionOp->getFunctionBasicBlock()->getArguments()) {
		inputTypes.emplace_back(getMLIRType(inputArg->getStamp()));
	}
	llvm::SmallVector<mlir::Type> outputTypes(1, getMLIRType(functionOp->getOutputArg()));
	auto functionInOutTypes = builder->getFunctionType(inputTypes, outputTypes);
	auto loc = getNameLoc("EntryPoint");
	auto mlirFunction = builder->create<mlir::func::FuncOp>(loc, functionOp->getName(), functionInOutTypes);

	// Avoid function name mangling.
	mlirFunction->setAttr("llvm.emit_c_interface", mlir::UnitAttr::get(context));
	mlirFunction.

	    addEntryBlock();

	// Set InsertPoint to beginning of the execute function.
	builder->setInsertionPointToStart(&mlirFunction
	                                       .

	                                   getBody()

	                                       .

	                                   front()

	);

	// Store references to function args in the valueMap map.
	auto valueMapIterator = mlirFunction.args_begin();
	for (int i = 0;

	     i < (int)

	             functionOp->getFunctionBasicBlock()
	                 ->getArguments()
	                 .

	         size();

	     ++i) {
		frame.setValue(functionOp->getFunctionBasicBlock()->getArguments().at(i)->

		               getIdentifier(),
		               valueMapIterator[i]

		);
	}

	// Generate MLIR for operations in function body (BasicBlock).
	generateMLIR(functionOp->

	             getFunctionBasicBlock(),
	             frame

	);

	theModule.push_back(mlirFunction);
}

void MLIRLoweringProvider::generateMLIR(ir::LoadOperation* loadOp, ValueFrame& frame) {

	auto address = frame.getValue(loadOp->getAddress()->getIdentifier());

	// auto bitcast = builder->create<mlir::LLVM::BitcastOp>(getNameLoc("Bitcasted address"),
	//                                                       mlir::LLVM::LLVMPointerType::get(context), address);
	auto mlirLoadOp =
	    builder->create<mlir::LLVM::LoadOp>(getNameLoc("loadedValue"), getMLIRType(loadOp->getStamp()), address);
	frame.setValue(loadOp->getIdentifier(), mlirLoadOp);
}

void MLIRLoweringProvider::generateMLIR(ir::ConstIntOperation* constIntOp, ValueFrame& frame) {
	if (!frame.contains(constIntOp->getIdentifier())) {
		frame.setValue(constIntOp->getIdentifier(),
		               getConstInt("ConstantOp", constIntOp->getStamp(), constIntOp->getValue()));
	} else {
		frame.setValue(constIntOp->getIdentifier(),
		               getConstInt("ConstantOp", constIntOp->getStamp(), constIntOp->getValue()));
	}
}

void MLIRLoweringProvider::generateMLIR(ir::ConstFloatOperation* constFloatOp, ValueFrame& frame) {
	if (isFloat(constFloatOp->getStamp())) {
		auto floatType = (constFloatOp->getStamp() == Type::f32) ? builder->getF32Type() : builder->getF64Type();
		frame.setValue(constFloatOp->getIdentifier(), builder->create<mlir::LLVM::ConstantOp>(
		                                                  getNameLoc("constantFloat"), floatType,
		                                                  builder->getFloatAttr(floatType, constFloatOp->getValue())));
	}
}

//==---------------------------==//
//==-- ARITHMETIC OPERATIONS --==//
//==---------------------------==//
void MLIRLoweringProvider::generateMLIR(ir::AddOperation* addOp, ValueFrame& frame) {
	auto leftInput = frame.getValue(addOp->getLeftInput()->getIdentifier());
	auto rightInput = frame.getValue(addOp->getRightInput()->getIdentifier());
	if (addOp->getLeftInput()->getStamp() == Type::ptr) {
		// if we add something to a ptr we have to use a llvm getelementptr
		mlir::Value elementAddress = builder->create<mlir::LLVM::GEPOp>(
		    getNameLoc("fieldAccess"), mlir::LLVM::LLVMPointerType::get(context), builder->getI8Type(), leftInput,
		    mlir::ArrayRef<mlir::Value>({rightInput}));
		frame.setValue(addOp->getIdentifier(), elementAddress);

	} else if (isFloat(addOp->getStamp())) {
		auto mlirAddOp = builder->create<mlir::LLVM::FAddOp>(getNameLoc("binOpResult"), leftInput.getType(), leftInput,
		                                                     rightInput, mlir::LLVM::FastmathFlags::fast);
		frame.setValue(addOp->getIdentifier(), mlirAddOp);
	} else {
		if (!inductionVars.contains(addOp->getLeftInput()->getIdentifier())) {
			if (!frame.contains(addOp->getIdentifier())) {
				auto mlirAddOp = builder->create<mlir::LLVM::AddOp>(getNameLoc("binOpResult"), leftInput, rightInput);
				frame.setValue(addOp->getIdentifier(), mlirAddOp);
			} else {
				auto mlirAddOp = builder->create<mlir::LLVM::AddOp>(getNameLoc("binOpResult"), leftInput, rightInput);
				frame.setValue(addOp->getIdentifier(), mlirAddOp);
			}
		}
	}
}

void MLIRLoweringProvider::generateMLIR(ir::SubOperation* subIntOp, ValueFrame& frame) {
	auto leftInput = frame.getValue(subIntOp->getLeftInput()->getIdentifier());
	auto rightInput = frame.getValue(subIntOp->getRightInput()->getIdentifier());
	if (isFloat(subIntOp->getStamp())) {
		auto mlirSubOp = builder->create<mlir::LLVM::FSubOp>(
		    getNameLoc("binOpResult"), leftInput, rightInput,
		    mlir::LLVM::FastmathFlagsAttr::get(context, mlir::LLVM::FastmathFlags::fast));
		frame.setValue(subIntOp->getIdentifier(), mlirSubOp);
	} else {
		auto mlirSubOp = builder->create<mlir::LLVM::SubOp>(getNameLoc("binOpResult"), leftInput, rightInput);
		frame.setValue(subIntOp->getIdentifier(), mlirSubOp);
	}
}

void MLIRLoweringProvider::generateMLIR(ir::MulOperation* mulOp, ValueFrame& frame) {
	auto leftInput = frame.getValue(mulOp->getLeftInput()->getIdentifier());
	auto rightInput = frame.getValue(mulOp->getRightInput()->getIdentifier());
	auto resultType = leftInput.getType();
	if (isFloat(mulOp->getStamp())) {
		auto mlirMulOp = builder->create<mlir::LLVM::FMulOp>(getNameLoc("binOpResult"), resultType, leftInput,
		                                                     rightInput, mlir::LLVM::FastmathFlags::fast);
		frame.setValue(mulOp->getIdentifier(), mlirMulOp);
	} else {
		auto mlirMulOp =
		    builder->create<mlir::LLVM::MulOp>(getNameLoc("binOpResult"), resultType, leftInput, rightInput);
		frame.setValue(mulOp->getIdentifier(), mlirMulOp);
	}
}

void MLIRLoweringProvider::generateMLIR(ir::DivOperation* divIntOp, ValueFrame& frame) {
	auto leftInput = frame.getValue(divIntOp->getLeftInput()->getIdentifier());
	auto rightInput = frame.getValue(divIntOp->getRightInput()->getIdentifier());
	auto resultType = leftInput.getType();
	if (isFloat(divIntOp->getStamp())) {
		auto mlirDivOp = builder->create<mlir::LLVM::FDivOp>(getNameLoc("binOpResult"), resultType, leftInput,
		                                                     rightInput, mlir::LLVM::FastmathFlags::fast);
		frame.setValue(divIntOp->getIdentifier(), mlirDivOp);
	} else {
		if (resultType.isSignedInteger()) {
			auto mlirDivOp =
			    builder->create<mlir::LLVM::UDivOp>(getNameLoc("binOpResult"), resultType, leftInput, rightInput);
			frame.setValue(divIntOp->getIdentifier(), mlirDivOp);
		} else {
			auto mlirDivOp =
			    builder->create<mlir::LLVM::SDivOp>(getNameLoc("binOpResult"), resultType, leftInput, rightInput);
			frame.setValue(divIntOp->getIdentifier(), mlirDivOp);
		}
	}
}

void MLIRLoweringProvider::generateMLIR(ir::ModOperation* divIntOp, ValueFrame& frame) {
	auto leftInput = frame.getValue(divIntOp->getLeftInput()->getIdentifier());
	auto rightInput = frame.getValue(divIntOp->getRightInput()->getIdentifier());
	auto resultType = leftInput.getType();
	if (isFloat(divIntOp->getStamp())) {
		auto mlirDivOp = builder->create<mlir::LLVM::FRemOp>(getNameLoc("binOpResult"), resultType, leftInput,
		                                                     rightInput, mlir::LLVM::FastmathFlags::fast);
		frame.setValue(divIntOp->getIdentifier(), mlirDivOp);
	} else {
		if (resultType.isSignedInteger()) {
			auto mlirDivOp =
			    builder->create<mlir::arith::RemSIOp>(getNameLoc("binOpResult"), resultType, leftInput, rightInput);
			frame.setValue(divIntOp->getIdentifier(), mlirDivOp);
		} else {
			auto mlirDivOp =
			    builder->create<mlir::arith::RemUIOp>(getNameLoc("binOpResult"), resultType, leftInput, rightInput);
			frame.setValue(divIntOp->getIdentifier(), mlirDivOp);
		}
	}
}

void MLIRLoweringProvider::generateMLIR(ir::StoreOperation* storeOp, ValueFrame& frame) {
	auto value = frame.getValue(storeOp->getValue()->getIdentifier());
	auto address = frame.getValue(storeOp->getAddress()->getIdentifier());
	auto bitcast = builder->create<mlir::LLVM::BitcastOp>(getNameLoc("Address Bitcasted"),
	                                                      mlir::LLVM::LLVMPointerType::get(context), address);
	builder->create<mlir::LLVM::StoreOp>(getNameLoc("outputStore"), value, bitcast);
}

void MLIRLoweringProvider::generateMLIR(ir::ReturnOperation* returnOp, ValueFrame& frame) {
	// Insert return into 'execute' function block. This is the FINAL return.
	if (!returnOp->hasReturnValue()) {
		builder->create<mlir::LLVM::ReturnOp>(getNameLoc("return"), mlir::ValueRange());
	} else {
		builder->create<mlir::LLVM::ReturnOp>(getNameLoc("return"),
		                                      frame.getValue(returnOp->getReturnValue()->getIdentifier()));
	}
}

void MLIRLoweringProvider::generateMLIR(ir::ProxyCallOperation* proxyCallOp, ValueFrame& frame) {
	mlir::FlatSymbolRefAttr functionRef;
	if (theModule.lookupSymbol<mlir::LLVM::LLVMFuncOp>(proxyCallOp->getFunctionSymbol())) {
		functionRef = mlir::SymbolRefAttr::get(context, proxyCallOp->getFunctionSymbol());
	} else {
		functionRef = insertExternalFunction(proxyCallOp->getFunctionSymbol(), proxyCallOp->getFunctionPtr(),
		                                     getMLIRType(proxyCallOp->getStamp()),
		                                     getMLIRType(proxyCallOp->getInputArguments()), true);
	}

	std::vector<mlir::Value> functionArgs;
	for (const auto& arg : proxyCallOp->getInputArguments()) {
		functionArgs.push_back(frame.getValue(arg->getIdentifier()));
	}
	if (proxyCallOp->getStamp() != Type::v) {
		auto res = builder->create<mlir::LLVM::CallOp>(getNameLoc("printFunc"), getMLIRType(proxyCallOp->getStamp()),
		                                               functionRef, functionArgs);
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
	} else if (compareOp->getComparator() == ir::CompareOperation::EQ &&
	           compareOp->getLeftInput()->getStamp() == Type::ptr &&
	           isInteger(compareOp->getRightInput()->getStamp())) {
		// add null check
		throw NotImplementedException("Null check is not implemented");
		// auto null =
		//         builder->create<mlir::LLVM::NullOp>(getNameLoc("null"), mlir::LLVM::LLVMPointerType::get(context));
		// auto cmpOp = builder->create<mlir::LLVM::ICmpOp>(getNameLoc("comparison"),
		//                                                  mlir::LLVM::ICmpPredicate::eq,
		//                                                  frame.getValue(compareOp->getLeftInput()->getIdentifier()),
		//                                                  null);
		// frame.setValue(compareOp->getIdentifier(), cmpOp);
	} else if (isInteger(leftStamp) && isInteger(rightStamp)) {
		// handle integer
		auto cmpOp = builder->create<mlir::arith::CmpIOp>(
		    getNameLoc("comparison"), convertToIntMLIRComparison(compareOp->getComparator(), leftStamp),
		    frame.getValue(compareOp->getLeftInput()->getIdentifier()),
		    frame.getValue(compareOp->getRightInput()->getIdentifier()));
		frame.setValue(compareOp->getIdentifier(), cmpOp);
	} else if (isFloat(leftStamp) && isFloat(rightStamp)) {
		// handle float comparison
		auto cmpOp = builder->create<mlir::arith::CmpFOp>(getNameLoc("comparison"),
		                                                  convertToFloatMLIRComparison(compareOp->getComparator()),
		                                                  frame.getValue(compareOp->getLeftInput()->getIdentifier()),
		                                                  frame.getValue(compareOp->getRightInput()->getIdentifier()));
		frame.setValue(compareOp->getIdentifier(), cmpOp);
	} else if (leftStamp == Type::b && rightStamp == Type::b) {
		// handle float comparison
		auto cmpOp = builder->create<mlir::arith::CmpIOp>(getNameLoc("comparison"),
		                                                  convertToBooleanMLIRComparison(compareOp->getComparator()),
		                                                  frame.getValue(compareOp->getLeftInput()->getIdentifier()),
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
	builder->create<mlir::cf::CondBranchOp>(getNameLoc("branch"), frame.getValue(ifOp->getValue()->getIdentifier()),
	                                        trueBlock, trueBlockArgs, elseBlock, elseBlockArgs);
}

void MLIRLoweringProvider::generateMLIR(ir::BranchOperation* branchOp, ValueFrame& frame) {
	std::vector<mlir::Value> mlirTargetBlockArguments;
	for (auto targetBlockArgument : branchOp->getNextBlockInvocation().getArguments()) {
		mlirTargetBlockArguments.push_back(frame.getValue(targetBlockArgument->getIdentifier()));
	}
	auto* mlirTargetBlock = generateBasicBlock(branchOp->getNextBlockInvocation(), frame);
	builder->create<mlir::cf::BranchOp>(getNameLoc("branch"), mlirTargetBlock, mlirTargetBlockArguments);
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
		mlirBasicBlock->addArgument(getMLIRType(headBlockHeadTypes->getStamp()), getNameLoc("arg"));
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

MLIRLoweringProvider::ValueFrame
MLIRLoweringProvider::createFrameFromParentBlock(MLIRLoweringProvider::ValueFrame& frame,
                                                 ir::BasicBlockInvocation& invocation) {
	auto invocationArguments = invocation.getArguments();
	auto& childBlockArguments = invocation.getBlock()->getArguments();
	// NES_ASSERT(invocationArguments.size() == childBlockArguments.size(),
	//            "the number of invocation parameters has to be the same as the number of block arguments in the
	//            invoked block.");
	ValueFrame childFrame;
	// Copy all frame values to the child frame that are arguments of the child block.
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

	if (isInteger(inputStamp) && isInteger(outputStamp)) {
		auto mlirInput = frame.getValue(castOperation->getInput()->getIdentifier());
		if (inputStamp == outputStamp) {
			// we skip the cast if input bit width are the same.
			frame.setValue(castOperation->getIdentifier(), mlirInput);
		} else if (isSignedInteger(outputStamp)) {
			auto mlirCast =
			    builder->create<mlir::arith::ExtSIOp>(getNameLoc("location"), getMLIRType(outputStamp), mlirInput);
			frame.setValue(castOperation->getIdentifier(), mlirCast);
		} else {
			auto mlirCast =
			    builder->create<mlir::arith::ExtUIOp>(getNameLoc("location"), getMLIRType(outputStamp), mlirInput);
			frame.setValue(castOperation->getIdentifier(), mlirCast);
		}
		return;
	} else if (isFloat(inputStamp) && isFloat(outputStamp)) {
		auto mlirInput = frame.getValue(castOperation->getInput()->getIdentifier());
		auto mlirCast =
		    builder->create<mlir::arith::ExtFOp>(getNameLoc("location"), getMLIRType(outputStamp), mlirInput);
		frame.setValue(castOperation->getIdentifier(), mlirCast);
		return;
	} else if (isInteger(inputStamp) && isFloat(outputStamp)) {
		auto mlirInput = frame.getValue(castOperation->getInput()->getIdentifier());
		if (isSignedInteger(inputStamp)) {
			auto mlirCast =
			    builder->create<mlir::arith::SIToFPOp>(getNameLoc("location"), getMLIRType(outputStamp), mlirInput);
			frame.setValue(castOperation->getIdentifier(), mlirCast);
		} else {
			auto mlirCast =
			    builder->create<mlir::arith::UIToFPOp>(getNameLoc("location"), getMLIRType(outputStamp), mlirInput);
			frame.setValue(castOperation->getIdentifier(), mlirCast);
		}
		return;
	} else {
		throw NotImplementedException("Cast is not supported.");
	}
}

void MLIRLoweringProvider::generateMLIR(ir::ConstBooleanOperation* constBooleanOp,
                                        MLIRLoweringProvider::ValueFrame& frame) {
	auto constOp = builder->create<mlir::arith::ConstantOp>(
	    getNameLoc("location"), builder->getI1Type(),
	    builder->getIntegerAttr(builder->getI1Type(), constBooleanOp->getValue()));
	frame.setValue(constBooleanOp->getIdentifier(), constOp);
}

std::vector<std::string> MLIRLoweringProvider::getJitProxyFunctionSymbols() {
	return std::move(jitProxyFunctionSymbols);
}

std::vector<void*> MLIRLoweringProvider::getJitProxyTargetAddresses() {
	return std::move(jitProxyFunctionTargetAddresses);
}

} // namespace nautilus::compiler::mlir

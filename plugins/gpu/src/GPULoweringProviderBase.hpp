#pragma once

#include "nautilus/compiler/Frame.hpp"
#include "nautilus/compiler/ir/IRGraph.hpp"
#include "nautilus/compiler/ir/blocks/BasicBlock.hpp"
#include "nautilus/compiler/ir/blocks/BasicBlockInvocation.hpp"
#include "nautilus/compiler/ir/operations/AllocaOperation.hpp"
#include "nautilus/compiler/ir/operations/ArithmeticOperations/AddOperation.hpp"
#include "nautilus/compiler/ir/operations/ArithmeticOperations/DivOperation.hpp"
#include "nautilus/compiler/ir/operations/ArithmeticOperations/ModOperation.hpp"
#include "nautilus/compiler/ir/operations/ArithmeticOperations/MulOperation.hpp"
#include "nautilus/compiler/ir/operations/ArithmeticOperations/SubOperation.hpp"
#include "nautilus/compiler/ir/operations/BinaryOperations/BinaryCompOperation.hpp"
#include "nautilus/compiler/ir/operations/BinaryOperations/NegateOperation.hpp"
#include "nautilus/compiler/ir/operations/BinaryOperations/ShiftOperation.hpp"
#include "nautilus/compiler/ir/operations/BranchOperation.hpp"
#include "nautilus/compiler/ir/operations/CastOperation.hpp"
#include "nautilus/compiler/ir/operations/ConstBooleanOperation.hpp"
#include "nautilus/compiler/ir/operations/ConstFloatOperation.hpp"
#include "nautilus/compiler/ir/operations/ConstIntOperation.hpp"
#include "nautilus/compiler/ir/operations/ConstPtrOperation.hpp"
#include "nautilus/compiler/ir/operations/FunctionAddressOfOperation.hpp"
#include "nautilus/compiler/ir/operations/FunctionOperation.hpp"
#include "nautilus/compiler/ir/operations/IfOperation.hpp"
#include "nautilus/compiler/ir/operations/IndirectCallOperation.hpp"
#include "nautilus/compiler/ir/operations/LoadOperation.hpp"
#include "nautilus/compiler/ir/operations/LogicalOperations/AndOperation.hpp"
#include "nautilus/compiler/ir/operations/LogicalOperations/CompareOperation.hpp"
#include "nautilus/compiler/ir/operations/LogicalOperations/NotOperation.hpp"
#include "nautilus/compiler/ir/operations/LogicalOperations/OrOperation.hpp"
#include "nautilus/compiler/ir/operations/Operation.hpp"
#include "nautilus/compiler/ir/operations/ProxyCallOperation.hpp"
#include "nautilus/compiler/ir/operations/ReturnOperation.hpp"
#include "nautilus/compiler/ir/operations/SelectOperation.hpp"
#include "nautilus/compiler/ir/operations/StoreOperation.hpp"
#include "nautilus/exceptions/NotImplementedException.hpp"
#include "nautilus/gpu/intrinsic_targets.hpp"
#include "nautilus/options.hpp"
#include <cassert>
#include <functional>
#include <sstream>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace nautilus::compiler::gpu {

using RegisterFrame = Frame<ir::OperationIdentifier, std::string>;
using IntrinsicHandler = std::function<bool(ir::ProxyCallOperation*, short blockIndex, RegisterFrame& frame,
                                            std::stringstream& blockArgs, std::vector<std::stringstream>& blocks,
                                            std::string (*getVariable)(const ir::OperationIdentifier&))>;

/// Helper: register a nullary GPU intrinsic that emits a platform expression.
inline void registerNullaryIntrinsic(std::unordered_map<void*, IntrinsicHandler>& map, void* fnPtr,
                                     const std::string& expr, const std::string& resultType = "uint32_t") {
	map[fnPtr] = [expr, resultType](ir::ProxyCallOperation* call, short blockIndex, RegisterFrame& frame,
	                                std::stringstream& blockArguments, std::vector<std::stringstream>& blocks,
	                                std::string (*getVariable)(const ir::OperationIdentifier&)) -> bool {
		auto resultVar = getVariable(call->getIdentifier());
		if (!frame.contains(call->getIdentifier())) {
			blockArguments << resultType << " " << resultVar << ";\n";
			frame.setValue(call->getIdentifier(), resultVar);
		}
		blocks[blockIndex] << resultVar << " = " << expr << ";\n";
		return true;
	};
}

/// Helper: register a void GPU intrinsic that emits a platform statement.
inline void registerVoidIntrinsic(std::unordered_map<void*, IntrinsicHandler>& map, void* fnPtr,
                                  const std::string& stmt) {
	map[fnPtr] = [stmt](ir::ProxyCallOperation*, short blockIndex, RegisterFrame&, std::stringstream&,
	                    std::vector<std::stringstream>& blocks,
	                    std::string (*)(const ir::OperationIdentifier&)) -> bool {
		blocks[blockIndex] << stmt << ";\n";
		return true;
	};
}

/// Helper: register a launch config intrinsic (setGrid/setBlock) that captures
/// the 3 argument variable names into the provided string references.
inline void registerLaunchConfigIntrinsic(std::unordered_map<void*, IntrinsicHandler>& map, void* fnPtr,
                                          std::string& outX, std::string& outY, std::string& outZ, bool& hasConfig) {
	map[fnPtr] = [&outX, &outY, &outZ, &hasConfig](ir::ProxyCallOperation* call, short, RegisterFrame& frame,
	                                               std::stringstream&, std::vector<std::stringstream>&,
	                                               std::string (*)(const ir::OperationIdentifier&)) -> bool {
		auto inputArgs = call->getInputArguments();
		outX = frame.getValue(inputArgs[0]->getIdentifier());
		outY = frame.getValue(inputArgs[1]->getIdentifier());
		outZ = frame.getValue(inputArgs[2]->getIdentifier());
		hasConfig = true;
		return true;
	};
}

/// CRTP base class for GPU lowering providers.
/// Derived must implement: getType(), registerGPUIntrinsics(), processProxyCall(), processOperation() for
/// GPU-specific ops, and the main process() entry point.
template <typename Derived>
class GPULoweringProviderBase {
public:
	using Code = std::stringstream;

protected:
	Code blockArguments;
	Code functions;
	std::vector<Code> blocks;
	std::shared_ptr<ir::IRGraph> ir;
	std::unordered_map<ir::BlockIdentifier, std::string> activeBlocks;
	std::unordered_set<std::string> functionNames;
	std::string returnType;
	std::unordered_map<void*, IntrinsicHandler> gpuIntrinsics;
	std::unordered_set<void*> deviceIntrinsics; // Only these mark a function as a kernel
	std::unordered_set<std::string> kernelFunctions;

	// Launch config: variable names holding grid/block dims from the last setGrid/setBlock calls.
	// These are consumed by the next kernel launch.
	std::string pendingGridX, pendingGridY, pendingGridZ;
	std::string pendingBlockX, pendingBlockY, pendingBlockZ;
	bool hasLaunchConfig = false;

	bool isGPUIntrinsic(void* fnPtr) const {
		return gpuIntrinsics.contains(fnPtr);
	}

	bool functionUsesGPUIntrinsics(const ir::FunctionOperation& func) const {
		for (auto& block : func.getBasicBlocks()) {
			for (auto& op : block->getOperations()) {
				if (op->getOperationType() == ir::Operation::OperationType::ProxyCallOp) {
					auto* call = static_cast<ir::ProxyCallOperation*>(op);
					// Only device intrinsics (threadIdx, syncThreads, etc.) mark a function
					// as a kernel. Launch config intrinsics (setGrid, setBlock) do not.
					if (deviceIntrinsics.contains(call->getFunctionPtr())) {
						return true;
					}
				}
			}
		}
		return false;
	}

	void classifyKernelFunctions() {
		for (const auto& func : ir->getFunctionOperations()) {
			// Primary: explicit kernel attribute set by NautilusKernelFunction
			if (func->hasAttribute("kernel")) {
				kernelFunctions.insert(func->getName());
			}
			// Fallback: heuristic intrinsic scanning for free functions
			else if (functionUsesGPUIntrinsics(*func)) {
				kernelFunctions.insert(func->getName());
			}
		}
	}

	static std::string getVariable(const ir::OperationIdentifier& id) {
		return "var_" + id.toString();
	}

	// Calls Derived::getType() via CRTP
	std::string type(const Type& stamp) {
		return static_cast<Derived*>(this)->getType(stamp);
	}

	// --- Shared operation processing methods ---

	std::string processBlock(const ir::BasicBlock* block, RegisterFrame& frame) {
		auto entry = activeBlocks.find(block->getIdentifier());
		if (entry != activeBlocks.end()) {
			return entry->second;
		}
		for (auto& arg : block->getArguments()) {
			if (!frame.contains(arg->getIdentifier())) {
				auto var = getVariable(arg->getIdentifier());
				blockArguments << type(arg->getStamp()) << " " << var << ";\n";
				frame.setValue(arg->getIdentifier(), var);
			}
		}
		auto blockName = "Block_" + std::to_string(block->getIdentifier().getId());
		short blockIndex = blocks.size();
		auto& currentBlock = blocks.emplace_back();
		currentBlock << blockName << ":\n";
		activeBlocks.emplace(block->getIdentifier(), blockName);
		for (auto& opt : block->getOperations()) {
			static_cast<Derived*>(this)->processOperation(opt, blockIndex, frame);
		}
		return blockName;
	}

	void processBlockInvocation(const ir::BasicBlockInvocation& bi, short blockIndex, RegisterFrame& parentFrame) {
		auto blockInputArguments = bi.getArguments();
		auto& blockTargetArguments = bi.getBlock()->getArguments();
		blocks[blockIndex] << "{\n";
		for (uint64_t i = 0; i < blockInputArguments.size(); i++) {
			blocks[blockIndex] << type(blockTargetArguments[i]->getStamp()) << " temp_" << i << " = "
			                   << parentFrame.getValue(blockInputArguments[i]->getIdentifier()) << ";\n";
		}
		for (uint64_t i = 0; i < blockInputArguments.size(); i++) {
			auto blockTargetArgument = blockTargetArguments[i]->getIdentifier();
			if (!parentFrame.contains(blockTargetArgument)) {
				auto var = getVariable(blockTargetArgument);
				parentFrame.setValue(blockTargetArgument, var);
				blockArguments << type(blockTargetArguments[i]->getStamp()) << " " << var << ";\n";
			}
			blocks[blockIndex] << parentFrame.getValue(blockTargetArgument) << " = temp_" << i << ";\n";
		}
		blocks[blockIndex] << "}\n";
	}

	void processIf(ir::IfOperation* ifOpt, short blockIndex, RegisterFrame& frame) {
		auto conditionalReg = frame.getValue(ifOpt->getValue()->getIdentifier());
		auto trueBlock = processBlock(ifOpt->getTrueBlockInvocation().getBlock(), frame);
		auto falseBlock = processBlock(ifOpt->getFalseBlockInvocation().getBlock(), frame);
		blocks[blockIndex] << "if (" << conditionalReg << "){\n";
		processBlockInvocation(ifOpt->getTrueBlockInvocation(), blockIndex, frame);
		blocks[blockIndex] << "goto " << trueBlock << ";\n";
		blocks[blockIndex] << "}else{\n";
		processBlockInvocation(ifOpt->getFalseBlockInvocation(), blockIndex, frame);
		blocks[blockIndex] << "goto " << falseBlock << ";}\n";
	}

	void processBranch(ir::BranchOperation* branchOp, short blockIndex, RegisterFrame& frame) {
		processBlockInvocation(branchOp->getNextBlockInvocation(), blockIndex, frame);
		auto nextBlock = processBlock(branchOp->getNextBlockInvocation().getBlock(), frame);
		blocks[blockIndex] << "goto " << nextBlock << ";\n";
	}

	void processCompare(ir::CompareOperation* cmpOp, short blockIndex, RegisterFrame& frame) {
		auto leftInput = frame.getValue(cmpOp->getLeftInput()->getIdentifier());
		auto rightInput = frame.getValue(cmpOp->getRightInput()->getIdentifier());
		auto resultVar = getVariable(cmpOp->getIdentifier());
		if (!frame.contains(cmpOp->getIdentifier())) {
			blockArguments << type(cmpOp->getStamp()) << " " << resultVar << ";\n";
			frame.setValue(cmpOp->getIdentifier(), resultVar);
		}
		if (cmpOp->isEquals() && cmpOp->getLeftInput()->getStamp() == Type::ptr &&
		    isInteger(cmpOp->getRightInput()->getStamp())) {
			blocks[blockIndex] << resultVar << " = " << leftInput << " == nullptr;\n";
			return;
		}
		std::string comp;
		switch (cmpOp->getComparator()) {
		case ir::CompareOperation::Comparator::EQ:
			comp = " == ";
			break;
		case ir::CompareOperation::Comparator::LT:
			comp = " < ";
			break;
		case ir::CompareOperation::Comparator::GT:
			comp = " > ";
			break;
		case ir::CompareOperation::Comparator::GE:
			comp = " >= ";
			break;
		case ir::CompareOperation::Comparator::LE:
			comp = " <= ";
			break;
		case ir::CompareOperation::Comparator::NE:
			comp = " != ";
			break;
		default:
			throw NotImplementedException("Comparator not implemented.");
		}
		blocks[blockIndex] << resultVar << " = " << leftInput << comp << rightInput << ";\n";
	}

	void processLoad(ir::LoadOperation* loadOp, short blockIndex, RegisterFrame& frame) {
		auto address = frame.getValue(loadOp->getAddress()->getIdentifier());
		auto resultVar = getVariable(loadOp->getIdentifier());
		if (!frame.contains(loadOp->getIdentifier())) {
			blockArguments << type(loadOp->getStamp()) << " " << resultVar << ";\n";
			frame.setValue(loadOp->getIdentifier(), resultVar);
		}
		auto t = type(loadOp->getStamp());
		blocks[blockIndex] << resultVar << " = *((" << t << "*)(" << address << "));\n";
	}

	void processStore(ir::StoreOperation* storeOp, short blockIndex, RegisterFrame& frame) {
		auto address = frame.getValue(storeOp->getAddress()->getIdentifier());
		auto value = frame.getValue(storeOp->getValue()->getIdentifier());
		auto t = type(storeOp->getValue()->getStamp());
		blocks[blockIndex] << "*((" << t << "*)(" << address << ")) = " << value << ";\n";
	}

	void processNegate(ir::NegateOperation* op, short blockIndex, RegisterFrame& frame) {
		auto input = frame.getValue(op->getInput()->getIdentifier());
		auto resultVar = getVariable(op->getIdentifier());
		if (!frame.contains(op->getIdentifier())) {
			blockArguments << type(op->getStamp()) << " " << resultVar << ";\n";
			frame.setValue(op->getIdentifier(), resultVar);
		}
		blocks[blockIndex] << resultVar << " = ~" << input << ";\n";
	}

	void processNot(ir::NotOperation* op, short blockIndex, RegisterFrame& frame) {
		auto input = frame.getValue(op->getInput()->getIdentifier());
		auto resultVar = getVariable(op->getIdentifier());
		if (!frame.contains(op->getIdentifier())) {
			blockArguments << type(op->getStamp()) << " " << resultVar << ";\n";
			frame.setValue(op->getIdentifier(), resultVar);
		}
		blocks[blockIndex] << resultVar << " = !" << input << ";\n";
	}

	void processCast(ir::CastOperation* castOp, short blockIndex, RegisterFrame& frame) {
		auto input = frame.getValue(castOp->getInput()->getIdentifier());
		auto var = getVariable(castOp->getIdentifier());
		auto inputStamp = castOp->getInput()->getStamp();
		auto outputStamp = castOp->getStamp();
		auto targetType = type(outputStamp);
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

	void processSelect(ir::SelectOperation* selectOp, short blockIndex, RegisterFrame& frame) {
		auto condition = frame.getValue(selectOp->getCondition()->getIdentifier());
		auto trueValue = frame.getValue(selectOp->getTrueValue()->getIdentifier());
		auto falseValue = frame.getValue(selectOp->getFalseValue()->getIdentifier());
		auto resultVar = getVariable(selectOp->getIdentifier());
		if (!frame.contains(selectOp->getIdentifier())) {
			blockArguments << type(selectOp->getStamp()) << " " << resultVar << ";\n";
			frame.setValue(selectOp->getIdentifier(), resultVar);
		}
		blocks[blockIndex] << resultVar << " = " << condition << " ? " << trueValue << " : " << falseValue << ";\n";
	}

	void processReturn(ir::ReturnOperation* returnOpt, short blockIndex, RegisterFrame& frame) {
		if (returnOpt->hasReturnValue()) {
			blocks[blockIndex] << "return " << frame.getValue(returnOpt->getReturnValue()->getIdentifier()) << ";\n";
			this->returnType = type(returnOpt->getStamp());
		} else {
			blocks[blockIndex] << "return;\n";
			this->returnType = "void";
		}
	}

	void processAlloca(ir::AllocaOperation* allocaOp, short blockIndex, RegisterFrame& frame) {
		auto resultVar = getVariable(allocaOp->getIdentifier());
		auto bufVar = "alloca_buf_" + allocaOp->getIdentifier().toString();
		if (!frame.contains(allocaOp->getIdentifier())) {
			blockArguments << "alignas(8) uint8_t " << bufVar << "[" << allocaOp->getSize() << "];\n";
			blockArguments << "uint8_t* " << resultVar << ";\n";
			frame.setValue(allocaOp->getIdentifier(), resultVar);
		}
		blocks[blockIndex] << resultVar << " = " << bufVar << ";\n";
	}

	template <class OpType>
	void processConst(ir::Operation* opt, short blockIndex, RegisterFrame& frame) {
		auto constValue = static_cast<OpType*>(opt);
		auto var = getVariable(constValue->getIdentifier());
		if (!frame.contains(constValue->getIdentifier())) {
			blockArguments << type(constValue->getStamp()) << " " << var << ";\n";
			frame.setValue(constValue->getIdentifier(), var);
		}
		std::stringstream ss;
		ss << constValue->getValue();
		if (ss.str() == "(nil)") {
			blocks[blockIndex] << var << " = (" << type(constValue->getStamp()) << ")(nullptr);\n";
		} else {
			blocks[blockIndex] << var << " = (" << type(constValue->getStamp()) << ")" << constValue->getValue()
			                   << ";\n";
		}
	}

	template <class OpType>
	void processBinary(ir::Operation* o, const std::string& operation, short blockIndex,
	                   RegisterFrame& frame) {
		auto op = static_cast<OpType*>(o);
		auto leftInput = frame.getValue(op->getLeftInput()->getIdentifier());
		auto rightInput = frame.getValue(op->getRightInput()->getIdentifier());
		auto resultVar = getVariable(op->getIdentifier());
		if (!frame.contains(op->getIdentifier())) {
			blockArguments << type(op->getStamp()) << " " << resultVar << ";\n";
			frame.setValue(op->getIdentifier(), resultVar);
		}
		blocks[blockIndex] << resultVar << " = " << leftInput << operation << rightInput << ";\n";
	}

	/// Dispatch an operation to the appropriate handler. Handles all common operations
	/// and delegates GPU-specific ones to the derived class via processGPUOperation().
	void dispatchOperation(ir::Operation* opt, short blockIndex, RegisterFrame& frame) {
		using OT = ir::Operation::OperationType;
		switch (opt->getOperationType()) {
		case OT::ConstBooleanOp:
			processConst<ir::ConstBooleanOperation>(opt, blockIndex, frame);
			return;
		case OT::ConstIntOp:
			processConst<ir::ConstIntOperation>(opt, blockIndex, frame);
			return;
		case OT::ConstFloatOp:
			processConst<ir::ConstFloatOperation>(opt, blockIndex, frame);
			return;
		case OT::ConstPtrOp:
			processConst<ir::ConstPtrOperation>(opt, blockIndex, frame);
			return;
		case OT::AddOp:
			processBinary<ir::AddOperation>(opt, "+", blockIndex, frame);
			return;
		case OT::MulOp:
			processBinary<ir::MulOperation>(opt, "*", blockIndex, frame);
			return;
		case OT::SubOp:
			processBinary<ir::SubOperation>(opt, "-", blockIndex, frame);
			return;
		case OT::DivOp:
			processBinary<ir::DivOperation>(opt, "/", blockIndex, frame);
			return;
		case OT::ModOp:
			processBinary<ir::ModOperation>(opt, "%", blockIndex, frame);
			return;
		case OT::ShiftOp: {
			auto op = static_cast<ir::ShiftOperation*>(opt);
			if (op->getType() == ir::ShiftOperation::LS)
				processBinary<ir::ShiftOperation>(opt, "<<", blockIndex, frame);
			else
				processBinary<ir::ShiftOperation>(opt, ">>", blockIndex, frame);
			return;
		}
		case OT::BinaryComp: {
			auto op = static_cast<ir::BinaryCompOperation*>(opt);
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
		case OT::ReturnOp:
			processReturn(static_cast<ir::ReturnOperation*>(opt), blockIndex, frame);
			return;
		case OT::CompareOp:
			processCompare(ir::as<ir::CompareOperation>(opt), blockIndex, frame);
			return;
		case OT::IfOp:
			processIf(ir::as<ir::IfOperation>(opt), blockIndex, frame);
			return;
		case OT::BranchOp:
			processBranch(ir::as<ir::BranchOperation>(opt), blockIndex, frame);
			return;
		case OT::LoadOp:
			processLoad(ir::as<ir::LoadOperation>(opt), blockIndex, frame);
			return;
		case OT::StoreOp:
			processStore(ir::as<ir::StoreOperation>(opt), blockIndex, frame);
			return;
		case OT::OrOp:
			processBinary<ir::OrOperation>(opt, "||", blockIndex, frame);
			return;
		case OT::AndOp:
			processBinary<ir::AndOperation>(opt, "&&", blockIndex, frame);
			return;
		case OT::NegateOp:
			processNegate(ir::as<ir::NegateOperation>(opt), blockIndex, frame);
			return;
		case OT::NotOp:
			processNot(ir::as<ir::NotOperation>(opt), blockIndex, frame);
			return;
		case OT::CastOp:
			processCast(ir::as<ir::CastOperation>(opt), blockIndex, frame);
			return;
		case OT::SelectOp:
			processSelect(ir::as<ir::SelectOperation>(opt), blockIndex, frame);
			return;
		case OT::AllocaOp:
			processAlloca(ir::as<ir::AllocaOperation>(opt), blockIndex, frame);
			return;
		default:
			// Delegate to derived class for backend-specific operations
			// (ProxyCallOp, IndirectCallOp, FunctionAddressOfOp, etc.)
			static_cast<Derived*>(this)->processGPUOperation(opt, blockIndex, frame);
			return;
		}
	}

	/// Reset state for processing a new function.
	void resetFunctionState() {
		blocks.clear();
		activeBlocks.clear();
		blockArguments.str("");
		blockArguments.clear();
		functions.str("");
		functions.clear();
		functionNames.clear();
	}
};

} // namespace nautilus::compiler::gpu

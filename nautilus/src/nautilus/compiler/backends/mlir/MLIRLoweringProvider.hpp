
#pragma once

#include "nautilus/compiler/Frame.hpp"
#include "nautilus/compiler/backends/mlir/ProxyFunctions.hpp"
#include "nautilus/compiler/ir/IRGraph.hpp"
#include "nautilus/compiler/ir/blocks/BasicBlock.hpp"
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
#include "nautilus/compiler/ir/operations/FunctionOperation.hpp"
#include "nautilus/compiler/ir/operations/IfOperation.hpp"
#include "nautilus/compiler/ir/operations/LoadOperation.hpp"
#include "nautilus/compiler/ir/operations/LogicalOperations/AndOperation.hpp"
#include "nautilus/compiler/ir/operations/LogicalOperations/CompareOperation.hpp"
#include "nautilus/compiler/ir/operations/LogicalOperations/NotOperation.hpp"
#include "nautilus/compiler/ir/operations/LogicalOperations/OrOperation.hpp"
#include "nautilus/compiler/ir/operations/Operation.hpp"
#include "nautilus/compiler/ir/operations/ProxyCallOperation.hpp"
#include "nautilus/compiler/ir/operations/ReturnOperation.hpp"
#include "nautilus/compiler/ir/operations/StoreOperation.hpp"
#include <llvm/ADT/StringMap.h>
#include <llvm/ADT/StringSet.h>
#include <llvm/ExecutionEngine/JITSymbol.h>
#include <mlir/IR/PatternMatch.h>
#include <unordered_set>

namespace nautilus::compiler::mlir {

class MLIRLoweringProvider {
public:
	// A ValueFrame is hashmap that binds operation names to MLIR values.
	// It is used to 'pass' values between mlir operations.
	// Control Flow can cause new ValueFrames to be created, to correctly model value access rights (scopes).
	using ValueFrame = Frame<ir::OperationIdentifier, ::mlir::Value>;

	/**
	 * @brief Allows to lower Nautilus IR to MLIR.
	 * @param MLIRContext: Used by MLIR to manage MLIR module creation.
	 */
	explicit MLIRLoweringProvider(::mlir::MLIRContext& context);

	~MLIRLoweringProvider();

	/**
	 * @brief Root MLIR generation function. Takes  as an IRGraph, and recursively lowers its operations to MLIR.
	 * @param :  represented as an IRGraph.
	 * @return mlir::mlir::OwningOpRef<mlir::ModuleOp> that is equivalent to the  module, and can be lowered to LLVM IR
	 * in one step.
	 */
	::mlir::OwningOpRef<::mlir::ModuleOp> generateModuleFromIR(std::shared_ptr<ir::IRGraph>);

	/**
	 * @return std::vector<std::string>: All proxy function symbols used in the module.
	 */
	std::vector<std::string> getJitProxyFunctionSymbols();

	/**
	 * @return std::vector<llvm::JITTargetAddress>: All proxy function addresses used in the module.
	 */
	std::vector<void*> getJitProxyTargetAddresses();

private:
	// MLIR variables
	::mlir::MLIRContext* context;
	::mlir::ModuleOp theModule;
	std::unique_ptr<::mlir::OpBuilder> builder;
	NES::ProxyFunctions ProxyFunctions;
	std::vector<std::string> jitProxyFunctionSymbols;
	std::vector<void*> jitProxyFunctionTargetAddresses;
	std::unordered_set<ir::OperationIdentifier> inductionVars;
	// Utility
	::mlir::RewriterBase::InsertPoint* globalInsertPoint;
	::mlir::Value globalString;
	::mlir::FlatSymbolRefAttr printfReference;
	llvm::StringMap<::mlir::Value> printfStrings;
	std::unordered_map<std::string, ::mlir::Block*> blockMapping; // Keeps track of already created basic blocks.

	/**
	 * @brief Generates MLIR from a  basic block. Iterates over basic block operations and calls generate.
	 *
	 * @param basicBlock: The  basic block that MLIR code is generated for.
	 * @param frame: An unordered map that MLIR operations insert their resulting values, and identifiers in.
	 */
	void generateMLIR(const ir::BasicBlock* basicBlock, ValueFrame& frame);

	/**
	 * @brief Calls the specific generate function based on currentNode's type.
	 * @param Operation:  operation that the MLIRLoweringProvider generates MLIR code for.
	 * @param frame: An unordered map that MLIR operations insert their resulting values, and identifiers in.
	 */
	void generateMLIR(const std::unique_ptr<ir::Operation>& operation, ValueFrame& frame);

	void generateMLIR(ir::FunctionOperation* funcOp, ValueFrame& frame);

	void generateMLIR(ir::ConstIntOperation* constIntOp, ValueFrame& frame);

	void generateMLIR(ir::ConstFloatOperation* constFloatOp, ValueFrame& frame);

	void generateMLIR(ir::ConstBooleanOperation* constBooleanOp, ValueFrame& frame);
	void generateMLIR(ir::ConstPtrOperation* constPtrOperation, ValueFrame& frame);

	void generateMLIR(ir::AddOperation* addIntOp, ValueFrame& frame);
	void generateMLIR(ir::SubOperation* subIntOp, ValueFrame& frame);
	void generateMLIR(ir::MulOperation* mulIntOp, ValueFrame& frame);
	void generateMLIR(ir::DivOperation* divFloatOp, ValueFrame& frame);
	void generateMLIR(ir::ModOperation* divFloatOp, ValueFrame& frame);
	void generateMLIR(ir::StoreOperation* storeOp, ValueFrame& frame);
	void generateMLIR(ir::LoadOperation* loadOp, ValueFrame& frame);
	void generateMLIR(ir::IfOperation* ifOp, ValueFrame& frame);
	void generateMLIR(ir::CompareOperation* compareOp, ValueFrame& frame);
	void generateMLIR(ir::BranchOperation* branchOp, ValueFrame& frame);
	void generateMLIR(ir::ReturnOperation* returnOp, ValueFrame& frame);
	void generateMLIR(ir::ProxyCallOperation* proxyCallOp, ValueFrame& frame);
	void generateMLIR(ir::OrOperation* orOperation, ValueFrame& frame);
	void generateMLIR(ir::AndOperation* andOperation, ValueFrame& frame);
	void generateMLIR(ir::NegateOperation* negateOperation, ValueFrame& frame);
	void generateMLIR(ir::NotOperation* notOperation, ValueFrame& frame);
	void generateMLIR(ir::CastOperation* castOperation, ValueFrame& frame);
	void generateMLIR(ir::BinaryCompOperation* binaryCompOperation, ValueFrame& frame);
	void generateMLIR(ir::ShiftOperation* shiftOperation, ValueFrame& frame);

	/**
	 * @brief Generates a basic block inside of the current MLIR module. Used for control flow (if,loop).
	 * @param blockInvocation:  basic block that is invocated.
	 * @param frame: An unordered map that MLIR operations insert their resulting values, and identifiers in.
	 * @return mlir::Block*: Returns a pointer to an MLIR basic block.
	 */
	::mlir::Block* generateBasicBlock(ir::BasicBlockInvocation& blockInvocation, ValueFrame& frame);

	/**
	 * @brief Inserts an external, but non-class-member-function, into MLIR.
	 * @param name: Function name.
	 * @param numResultBits: Number of bits of returned Integer.
	 * @param argTypes: Argument types of function.
	 * @param varArgs: Include variable arguments.
	 * @return FlatSymbolRefAttr: Reference to function used in CallOps.
	 */
	::mlir::FlatSymbolRefAttr insertExternalFunction(const std::string& name, void* functionPtr,
	                                                 ::mlir::Type resultType, std::vector<::mlir::Type> argTypes,
	                                                 bool varArgs);

	/**
	 * @brief Generates a Name(d)Loc(ation) that is attached to the operation.
	 * @param name: Name of the location. Used for debugging.
	 */
	::mlir::Location getNameLoc(const std::string& name);

	/**
	 * @brief Get MLIR Type from a basic  type.
	 * @param type:  basic type.
	 * @return mlir::Type: MLIR Type.
	 */
	::mlir::Type getMLIRType(Type type);

	/**
	 * @brief Get a vector of MLIR Types from a vector of  types.
	 * @param types: Vector of basic  types.
	 * @return mlir::Type: Vector of MLIR types.
	 */
	std::vector<::mlir::Type> getMLIRType(std::vector<ir::Operation*> types);

	/**
	 * @brief Get a constant MLIR Integer.
	 * @param location: NamedLocation for debugging purposes.
	 * @param numBits: Bit width of the returned constant Integer.
	 * @param value: Value of the returned Integer.
	 * @return mlir::Value: Constant MLIR Integer value.
	 */
	::mlir::Value getConstInt(const std::string& location, Type stamp, int64_t value);

	/**
	 * @brief Get a constant MLIR Integer.
	 * @param location: NamedLocation for debugging purposes.
	 * @param value: Value of the returned boolean.
	 */
	::mlir::Value getConstBool(const std::string& location, bool value);

	/**
	 * @brief Get the Bit Width from a basic NES type.
	 * @param type: Basic NES type.
	 * @return int8_t: Bit width.
	 */
	int8_t getBitWidthFromType(Type type);

	/**
	 * @brief Create a ValueFrame From Parent Block object.
	 *
	 * @param frame: An unordered map that MLIR operations insert their resulting values, and identifiers in.
	 * @param invocation: The parent basic block.
	 * @return ValueFrame: An unordered map with identifiers and MLIR values in it.
	 */
	ValueFrame createFrameFromParentBlock(ValueFrame& frame, ir::BasicBlockInvocation& invocation);
};
} // namespace nautilus::compiler::mlir

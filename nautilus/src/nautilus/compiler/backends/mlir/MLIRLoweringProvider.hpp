
#pragma once

#include "nautilus/compiler/Frame.hpp"
#include "nautilus/compiler/backends/mlir/ProxyFunctions.hpp"
#include "nautilus/compiler/backends/mlir/debug/DebugInfoOptions.hpp"
#include "nautilus/compiler/backends/mlir/debug/IRSourceMap.hpp"
#include "nautilus/compiler/ir/IRGraph.hpp"
#include "nautilus/compiler/ir/OperationDispatcher.hpp"
#include "nautilus/compiler/ir/blocks/BasicBlock.hpp"
#include <llvm/ExecutionEngine/JITSymbol.h>
#include <memory>
#include <mlir/IR/PatternMatch.h>
#include <span>
#include <unordered_set>

namespace mlir { namespace func {
class FuncOp;
}} // namespace mlir::func

namespace nautilus::compiler::mlir {

class MLIRIntrinsicManager;
class MLIRLoweringProvider : public ir::OperationDispatcher<MLIRLoweringProvider> {
public:
	// A ValueFrame is hashmap that binds operation names to MLIR values.
	// It is used to 'pass' values between mlir operations.
	// Control Flow can cause new ValueFrames to be created, to correctly model value access rights (scopes).
	using ValueFrame = Frame<ir::OperationIdentifier, ::mlir::Value>;

	/**
	 * @brief Allows to lower Nautilus IR to MLIR.
	 * @param MLIRContext: Used by MLIR to manage MLIR module creation.
	 */
	explicit MLIRLoweringProvider(::mlir::MLIRContext& context, const engine::Options& options,
	                              MLIRIntrinsicManager& intrinsicManager);

	~MLIRLoweringProvider();

	/**
	 * @brief Root MLIR generation function. Takes  as an IRGraph, and recursively lowers its operations to MLIR.
	 * @param :  represented as an IRGraph.
	 * @return mlir::mlir::OwningOpRef<mlir::ModuleOp> that is equivalent to the  module, and can be lowered to LLVM IR
	 * in one step.
	 */
	::mlir::OwningOpRef<::mlir::ModuleOp> generateModuleFromIR(std::shared_ptr<ir::IRGraph>);

	/**
	 * @brief Enable emission of FileLineColLocs tied to a Nautilus IR dump.
	 *
	 * When set, getNameLoc() returns a FileLineColLoc pointing into
	 * debugInfo.sourceFile using the line numbers in sourceMap.  The
	 * DIScopeForLLVMFuncOpPass that runs later in the pipeline then
	 * materializes DISubprograms referencing those locations, giving
	 * GDB/LLDB a valid file/line mapping to step through.  The caller
	 * is responsible for having written `sourceMap->text` to
	 * `debugInfo.sourceFile` before invoking generateModuleFromIR.
	 */
	void setDebugInfo(DebugInfoOptions debugInfo, std::shared_ptr<const IRSourceMap> sourceMap);

	/**
	 * @return std::vector<std::string>: All proxy function symbols used in the module.
	 */
	std::vector<std::string> getJitProxyFunctionSymbols();

	/**
	 * @return std::vector<llvm::JITTargetAddress>: All proxy function addresses used in the module.
	 */
	std::vector<void*> getJitProxyTargetAddresses();

private:
	// Allow the CRTP dispatcher to call our private visitXxx hooks.
	friend class ir::OperationDispatcher<MLIRLoweringProvider>;

	MLIRIntrinsicManager& intrinsicManager;
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
	std::unordered_map<ir::BlockIdentifier, ::mlir::Block*>
	    blockMapping; // Keeps track of already created basic blocks.
	const engine::Options* options;

	// Debug-info state.  When debugInfo_.enable is false, all debug
	// paths are no-ops and the lowering produces byte-identical output
	// to a build without debug support.
	DebugInfoOptions debugInfo_;
	std::shared_ptr<const IRSourceMap> irSourceMap_;
	// The Nautilus IR op currently being lowered.  Set in generateMLIR
	// just before each dispatch() so that the ~73 existing
	// getNameLoc(name) call sites automatically produce a FileLineColLoc
	// pointing at the correct Nautilus IR line.
	const ir::Operation* currentOp_ = nullptr;
	// Dump line of the currently-lowered op — set positionally from
	// the enclosing block's blockOpLines.  Provides a line to
	// terminator ops (`br`, `if`, `return`) that have no `$N` id to
	// look up in `operationLines`, so GDB can stop on the terminator.
	uint32_t currentOpLine_ = 0;
	// One shadow alloca per Nautilus $N id used in the current function.
	// Allocated at function entry when debug info is active, refreshed
	// by a store at every site that (re)defines the identifier (function
	// param, body op, non-entry block arg).  EmitDbgValuePass later
	// emits a single dbg.declare per alloca so GDB can resolve $N by
	// reading its stack slot at any PC inside the function — no
	// dependency on register liveness.  The map is cleared in
	// generateFunction before each function's body is lowered.
	std::unordered_map<uint32_t, ::mlir::Value> debugAllocas_;
	// Line of the current function's header in the IR dump.  Used as
	// the `!dbg` location of the prologue (allocas, function-parameter
	// stores) so GDB treats it as a single source line at function
	// entry rather than jumping through each variable's eventual
	// declaration line during the prologue.
	uint32_t currentFunctionHeaderLine_ = 0;
	// Per-function line tables for the function currently being
	// lowered.  Nautilus IR re-uses `$N` ids and `Block_N` indices
	// across functions, so we must scope lookups to the current
	// function — consulting the global IRSourceMap by id alone would
	// return the wrong caller/callee line.
	const IRSourceMap::FunctionLines* currentFunctionLines_ = nullptr;

	/**
	 * @brief Generates MLIR from a  basic block. Iterates over basic block operations and calls generate.
	 *
	 * @param basicBlock: The  basic block that MLIR code is generated for.
	 * @param frame: An unordered map that MLIR operations insert their resulting values, and identifiers in.
	 */
	void generateMLIR(const ir::BasicBlock* basicBlock, ValueFrame& frame);

	void generateFunction(::mlir::func::FuncOp& mlirFunction, const ir::FunctionOperation& funcOp, ValueFrame& frame);
	::mlir::func::FuncOp generateFunctionDefinitions(const ir::FunctionOperation& funcOp);

	/// Build a `$N` NameLoc wrapping a FileLineColLoc at the IR dump line
	/// of the given identifier.  Returns a plain NameLoc("arg") location
	/// when debug info is disabled so the caller can use the result
	/// unconditionally.
	::mlir::Location makeDollarLoc(uint32_t id, llvm::StringRef fallbackName);

	/// Lazily create an `llvm.alloca` at the entry block of the currently
	/// enclosing `func.func` for shadow-storing $N's value.  The alloca
	/// is cached in `debugAllocas_` so subsequent stores reuse the same
	/// slot.  No-op when `debugInfo_.enable` is false.
	::mlir::Value ensureDebugAlloca(uint32_t id, ::mlir::Type type);

	/// Emit a store of `value` into $id's shadow alloca at the builder's
	/// current insertion point.  Creates the alloca on first use.  No-op
	/// when `debugInfo_.enable` is false.
	void storeDebugValue(uint32_t id, ::mlir::Value value, ::mlir::Location loc);

	// Per-operation hooks invoked by OperationDispatcher::dispatch.
	void visitConstInt(ir::ConstIntOperation* constIntOp, ValueFrame& frame);
	void visitConstFloat(ir::ConstFloatOperation* constFloatOp, ValueFrame& frame);
	void visitConstBoolean(ir::ConstBooleanOperation* constBooleanOp, ValueFrame& frame);
	void visitConstPtr(ir::ConstPtrOperation* constPtrOperation, ValueFrame& frame);

	void visitAdd(ir::AddOperation* addIntOp, ValueFrame& frame);
	void visitSub(ir::SubOperation* subIntOp, ValueFrame& frame);
	void visitMul(ir::MulOperation* mulIntOp, ValueFrame& frame);
	void visitDiv(ir::DivOperation* divFloatOp, ValueFrame& frame);
	void visitMod(ir::ModOperation* divFloatOp, ValueFrame& frame);
	void visitStore(ir::StoreOperation* storeOp, ValueFrame& frame);
	void visitLoad(ir::LoadOperation* loadOp, ValueFrame& frame);
	void visitIf(ir::IfOperation* ifOp, ValueFrame& frame);
	void visitCompare(ir::CompareOperation* compareOp, ValueFrame& frame);
	void visitBranch(ir::BranchOperation* branchOp, ValueFrame& frame);
	void visitReturn(ir::ReturnOperation* returnOp, ValueFrame& frame);
	void visitProxyCall(ir::ProxyCallOperation* proxyCallOp, ValueFrame& frame);
	void visitIndirectCall(ir::IndirectCallOperation* indirectCallOp, ValueFrame& frame);
	void visitOr(ir::OrOperation* orOperation, ValueFrame& frame);
	void visitAnd(ir::AndOperation* andOperation, ValueFrame& frame);
	void visitNegate(ir::NegateOperation* negateOperation, ValueFrame& frame);
	void visitNot(ir::NotOperation* notOperation, ValueFrame& frame);
	void visitSelect(ir::SelectOperation* selectOperation, ValueFrame& frame);
	void visitCast(ir::CastOperation* castOperation, ValueFrame& frame);
	void visitBinaryComp(ir::BinaryCompOperation* binaryCompOperation, ValueFrame& frame);
	void visitShift(ir::ShiftOperation* shiftOperation, ValueFrame& frame);
	void visitAlloca(ir::AllocaOperation* allocaOperation, ValueFrame& frame);
	void visitFunctionAddressOf(ir::FunctionAddressOfOperation* funcAddrOp, ValueFrame& frame);
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
	 * @param fnAttrs: Information on attributes, such as 'memory' (access)
	 * @return FlatSymbolRefAttr: Reference to function used in CallOps.
	 */
	::mlir::FlatSymbolRefAttr insertExternalFunction(const std::string& name, void* functionPtr,
	                                                 const ::mlir::Type& resultType,
	                                                 const std::vector<::mlir::Type>& argTypes,
	                                                 const FunctionAttributes& fnAttrs);

	/**
	 * @brief Generates a Name(d)Loc(ation) that is attached to the operation.
	 * @param name: Name of the location. Used for debugging.
	 *
	 * When setDebugInfo() has been called with a non-null source map and
	 * currentOp_ is set, returns a FileLineColLoc pointing at the line in
	 * the source file where the current op is defined, wrapped in a
	 * NameLoc.  Otherwise returns the legacy Query_1:0:0 placeholder.
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
	std::vector<::mlir::Type> getMLIRType(std::span<ir::Operation* const> types);

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

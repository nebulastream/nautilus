#pragma once

#include "nautilus/common/FunctionAttributes.hpp"
#include "nautilus/compiler/ir/IRGraph.hpp"
#include "nautilus/compiler/ir/blocks/BasicBlock.hpp"
#include "nautilus/compiler/ir/operations/AllocaOperation.hpp"
#include "nautilus/compiler/ir/operations/ArithmeticOperations/AddOperation.hpp"
#include "nautilus/compiler/ir/operations/ArithmeticOperations/DivOperation.hpp"
#include "nautilus/compiler/ir/operations/ArithmeticOperations/ModOperation.hpp"
#include "nautilus/compiler/ir/operations/ArithmeticOperations/MulOperation.hpp"
#include "nautilus/compiler/ir/operations/ArithmeticOperations/SubOperation.hpp"
#include "nautilus/compiler/ir/operations/BinaryOperations/BinaryCompOperation.hpp"
#include "nautilus/compiler/ir/operations/BinaryOperations/NegateOperation.hpp"
#include "nautilus/compiler/ir/operations/BinaryOperations/ShiftOperation.hpp"
#include "nautilus/compiler/ir/operations/CallOperation.hpp"
#include "nautilus/compiler/ir/operations/CastOperation.hpp"
#include "nautilus/compiler/ir/operations/ConstBooleanOperation.hpp"
#include "nautilus/compiler/ir/operations/ConstFloatOperation.hpp"
#include "nautilus/compiler/ir/operations/ConstIntOperation.hpp"
#include "nautilus/compiler/ir/operations/ConstPtrOperation.hpp"
#include "nautilus/compiler/ir/operations/FunctionAddressOfOperation.hpp"
#include "nautilus/compiler/ir/operations/FunctionOperation.hpp"
#include "nautilus/compiler/ir/operations/IndirectCallOperation.hpp"
#include "nautilus/compiler/ir/operations/LoadOperation.hpp"
#include "nautilus/compiler/ir/operations/LogicalOperations/AndOperation.hpp"
#include "nautilus/compiler/ir/operations/LogicalOperations/CompareOperation.hpp"
#include "nautilus/compiler/ir/operations/LogicalOperations/NotOperation.hpp"
#include "nautilus/compiler/ir/operations/LogicalOperations/OrOperation.hpp"
#include "nautilus/compiler/ir/operations/Operation.hpp"
#include "nautilus/compiler/ir/operations/SelectOperation.hpp"
#include "nautilus/compiler/ir/operations/StoreOperation.hpp"
#include "nautilus/tracing/ExecutionTrace.hpp"
#include "nautilus/tracing/TraceOperation.hpp"
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace nautilus::tracing {

// Internal to the two conversion phases, which spell IR types unqualified.
using namespace compiler::ir;

class TraceModule;

compiler::ir::OperationIdentifier createValueIdentifier(const TypedValueRef& val);
compiler::ir::OperationIdentifier createValueIdentifier(const InputVariant& val);

/// Binds a traced function body to the function-table entry its call sites
/// minted (or interns a fresh one).
void bindDefinition(compiler::ir::IRGraph& ir, TraceModule& traceModule, const std::string& functionName,
                    compiler::ir::FunctionOperation* functionOperation);

/**
 * @brief The parts of trace-to-IR conversion that do not depend on how
 * values are resolved: the region table, call interning, provenance, and the
 * conversion of every operation that is not control flow.
 *
 * The operation converters are templates over the value frame, the object
 * that resolves a traced value to its IR definition
 * (`getValue(OperationIdentifier)`) and records a new one
 * (`setValue(OperationIdentifier, Operation*)`); `TraceToIRConversionPhase`
 * supplies one that builds SSA form as it goes.
 */
class IRConversionSupport {
public:
	IRConversionSupport(ExecutionTrace* trace, std::shared_ptr<compiler::ir::IRGraph> ir)
	    : trace(trace), ir(std::move(ir)) {
	}

protected:
	using Operation = compiler::ir::Operation;
	using BasicBlock = compiler::ir::BasicBlock;

	/// Clears the per-function region tables.
	void resetFunctionState() {
		currentRegionSpecs.clear();
		regionMap.clear();
		regionKeys.clear();
	}

	/// Interns @p call in the module function table and returns its id.
	/// Keyed on identity -- a code address, or the definition pointer for
	/// an internal callee -- never on a name.
	compiler::ir::FunctionId internCallee(const FunctionCall& call, Type resultType,
	                                      const std::vector<compiler::ir::Operation*>& arguments);

	/// Snapshots the trace's central alloca table and converts each entry to
	/// the IR-side `AllocaSpec` shape so it can be embedded on the
	/// FunctionOperation.
	std::vector<compiler::ir::AllocaSpec> collectAllocaSpecs() const;

	/// Returns the provenance to stamp on every IR operation created from
	/// @p operation: its trace-time call stack and its region.
	compiler::ir::OperationProvenance provenanceOf(const TraceOperation& operation);

	/// Applies @p operation's provenance to an IR operation built outside
	/// `addTaggedOperation` (a branch terminator, an if).
	void stampProvenance(compiler::ir::Operation& irOperation, const TraceOperation& operation);

	/// Maps a region of the trace onto this function's region table, adding an entry
	/// (and its enclosing chain) on first sight.
	///
	/// The mapping is many-to-one: the trace records one region per *engagement* of a
	/// call site -- a region in an unrolled loop is entered once per iteration -- while
	/// the IR wants one entry per call site, so engagements that agree on their
	/// attributes and their enclosing region collapse into a single entry.
	compiler::ir::RegionIndex mapRegion(RegionIndex traceRegion);

	/// Converts @p operation -- any operation that only reads values and defines
	/// at most one -- into @p currentBlock, resolving operands through @p frame.
	/// Returns false for the operations that need the caller: control flow,
	/// returns and assignments.
	template <typename Frame>
	bool convertValueOperation(Frame& frame, compiler::ir::BasicBlock* currentBlock, TraceOperation& operation) {
		switch (operation.op) {
		case Op::ADD: {
			processBinaryOperator<AddOperation>(frame, currentBlock, operation);
			return true;
		}
		case Op::SUB: {
			processBinaryOperator<SubOperation>(frame, currentBlock, operation);
			return true;
		}
		case Op::DIV: {
			processBinaryOperator<DivOperation>(frame, currentBlock, operation);
			return true;
		}
		case Op::MUL: {
			processBinaryOperator<MulOperation>(frame, currentBlock, operation);
			return true;
		}
		case Op::MOD: {
			processBinaryOperator<ModOperation>(frame, currentBlock, operation);
			return true;
		}
		case Op::EQ: {
			processLogicalComperator(frame, currentBlock, operation, CompareOperation::Comparator::EQ);
			return true;
		}
		case Op::LT: {
			processLogicalComperator(frame, currentBlock, operation, CompareOperation::Comparator::LT);
			return true;
		}
		case Op::GT: {
			processLogicalComperator(frame, currentBlock, operation, CompareOperation::Comparator::GT);
			return true;
		}
		case Op::NEQ: {
			processLogicalComperator(frame, currentBlock, operation, CompareOperation::Comparator::NE);
			return true;
		}
		case Op::LTE: {
			processLogicalComperator(frame, currentBlock, operation, CompareOperation::Comparator::LE);
			return true;
		}
		case Op::GTE: {
			processLogicalComperator(frame, currentBlock, operation, CompareOperation::Comparator::GE);
			return true;
		}
		case Op::NEGATE: {
			processUnaryOperator<NegateOperation>(frame, currentBlock, operation);
			return true;
		}
		case Op::NOT: {
			processUnaryOperator<NotOperation>(frame, currentBlock, operation);
			return true;
		}
		case Op::AND: {
			processBinaryOperator<AndOperation>(frame, currentBlock, operation);
			return true;
		}
		case Op::OR: {
			processBinaryOperator<OrOperation>(frame, currentBlock, operation);
			return true;
		}
		case Op::CONST: {
			processConst(frame, currentBlock, operation);
			return true;
		}
		case Op::LOAD: {
			processLoad(frame, currentBlock, operation);
			return true;
		};
		case Op::STORE: {
			processStore(frame, currentBlock, operation);
			return true;
		};
		case Op::CAST: {
			processCast(frame, currentBlock, operation);
			return true;
		};
		case Op::CALL:
		case Op::CALL_WITH_EXCEPTION_HANDLING:
			processCall(frame, currentBlock, operation);
			return true;
		case Op::INDIRECT_CALL:
		case Op::INDIRECT_CALL_WITH_EXCEPTION_HANDLING:
			processIndirectCall(frame, currentBlock, operation);
			return true;
		case LSH:
			processShift(frame, currentBlock, operation, compiler::ir::ShiftOperation::LS);
			return true;
		case RSH:
			processShift(frame, currentBlock, operation, compiler::ir::ShiftOperation::RS);
			return true;
		case BOR:
			processBinaryComp(frame, currentBlock, operation, compiler::ir::BinaryCompOperation::Type::BOR);
			return true;
		case BAND:
			processBinaryComp(frame, currentBlock, operation, compiler::ir::BinaryCompOperation::BAND);
			return true;
		case BXOR:
			processBinaryComp(frame, currentBlock, operation, compiler::ir::BinaryCompOperation::XOR);
			return true;
		case SELECT:
			processTernaryOperator<SelectOperation>(frame, currentBlock, operation);
			return true;
		case ALLOCA:
			processAlloca(frame, currentBlock, operation);
			return true;
		case FUNC_ADDR:
			processFuncAddr(frame, currentBlock, operation);
			return true;
		default:
			return false;
		}
	}

	template <typename OpType, typename Frame>
	void processBinaryOperator(Frame& frame, compiler::ir::BasicBlock* currentBlock, TraceOperation& op) {
		auto leftInput = frame.getValue(createValueIdentifier(op.input[0]));
		auto rightInput = frame.getValue(createValueIdentifier(op.input[1]));
		auto resultIdentifier = createValueIdentifier(op.resultRef);
		auto operation =
		    currentBlock->addTaggedOperation<OpType>(provenanceOf(op), resultIdentifier, leftInput, rightInput);
		frame.setValue(resultIdentifier, operation);
	}

	template <typename OpType, typename Frame>
	void processUnaryOperator(Frame& frame, compiler::ir::BasicBlock* currentBlock, TraceOperation& op) {
		auto input = frame.getValue(createValueIdentifier(op.input[0]));
		auto resultIdentifier = createValueIdentifier(op.resultRef);
		auto operation = currentBlock->addTaggedOperation<OpType>(provenanceOf(op), resultIdentifier, input);
		frame.setValue(resultIdentifier, operation);
	}

	template <typename OpType, typename Frame>
	void processTernaryOperator(Frame& frame, compiler::ir::BasicBlock* currentBlock, TraceOperation& op) {
		auto firstInput = frame.getValue(createValueIdentifier(op.input[0]));
		auto secondInput = frame.getValue(createValueIdentifier(op.input[1]));
		auto thirdInput = frame.getValue(createValueIdentifier(op.input[2]));
		auto resultIdentifier = createValueIdentifier(op.resultRef);
		auto operation = currentBlock->addTaggedOperation<OpType>(provenanceOf(op), resultIdentifier, firstInput,
		                                                          secondInput, thirdInput, op.resultRef.type);
		frame.setValue(resultIdentifier, operation);
	}

	template <typename Frame>
	void processBinaryComp(Frame& frame, compiler::ir::BasicBlock* currentBlock,
	                       nautilus::tracing::TraceOperation& operation, compiler::ir::BinaryCompOperation::Type type) {
		auto leftInput = frame.getValue(createValueIdentifier(operation.input[0]));
		auto rightInput = frame.getValue(createValueIdentifier(operation.input[1]));
		auto resultIdentifier = createValueIdentifier(operation.resultRef);
		auto divOperation = currentBlock->addTaggedOperation<BinaryCompOperation>(
		    provenanceOf(operation), resultIdentifier, leftInput, rightInput, type);
		frame.setValue(resultIdentifier, divOperation);
	}

	template <typename Frame>
	void processShift(Frame& frame, compiler::ir::BasicBlock* currentBlock,
	                  nautilus::tracing::TraceOperation& operation, compiler::ir::ShiftOperation::ShiftType type) {
		auto leftInput = frame.getValue(createValueIdentifier(operation.input[0]));
		auto rightInput = frame.getValue(createValueIdentifier(operation.input[1]));
		auto resultIdentifier = createValueIdentifier(operation.resultRef);
		auto divOperation = currentBlock->addTaggedOperation<ShiftOperation>(provenanceOf(operation), resultIdentifier,
		                                                                     leftInput, rightInput, type);
		frame.setValue(resultIdentifier, divOperation);
	}

	template <typename Frame>
	void processLogicalComperator(Frame& frame, BasicBlock* currentBlock, TraceOperation& operation,
	                              CompareOperation::Comparator comp) {
		auto leftInput = frame.getValue(createValueIdentifier(operation.input[0]));
		auto rightInput = frame.getValue(createValueIdentifier(operation.input[1]));
		auto resultIdentifier = createValueIdentifier(operation.resultRef);
		auto compareOperation = currentBlock->addTaggedOperation<CompareOperation>(
		    provenanceOf(operation), resultIdentifier, leftInput, rightInput, comp);
		frame.setValue(resultIdentifier, compareOperation);
	}

	template <typename Frame>
	void processLoad(Frame& frame, BasicBlock* currentBlock, TraceOperation& operation) {
		auto address = frame.getValue(createValueIdentifier(operation.input[0]));
		auto resultIdentifier = createValueIdentifier(operation.resultRef);
		auto* loadOperation = currentBlock->addTaggedOperation<LoadOperation>(provenanceOf(operation), resultIdentifier,
		                                                                      address, operation.resultType);
		frame.setValue(resultIdentifier, loadOperation);
	}

	template <typename Frame>
	void processStore(Frame& frame, BasicBlock* currentBlock, TraceOperation& operation) {
		auto address = frame.getValue(createValueIdentifier(operation.input[0]));
		auto value = frame.getValue(createValueIdentifier(operation.input[1]));
		currentBlock->addTaggedOperation<StoreOperation>(provenanceOf(operation), value, address);
	}

	template <typename Frame>
	void processCall(Frame& frame, BasicBlock* currentBlock, TraceOperation& operation) {
		const FunctionCall& functionCallTarget = *std::get<FunctionCall*>(operation.input[0]);
		auto inputArguments = std::vector<Operation*> {};
		for (const auto& argument : functionCallTarget.arguments) {
			auto input = frame.getValue(createValueIdentifier(argument));
			inputArguments.emplace_back(input);
		}

		auto resultType = operation.resultType;
		auto resultIdentifier = createValueIdentifier(operation.resultRef);
		auto destructors = std::vector<CallOperation::Destructor> {};
		destructors.reserve(functionCallTarget.destructors.size());
		for (const auto& destructor : functionCallTarget.destructors) {
			destructors.push_back(CallOperation::Destructor {
			    .address = frame.getValue(createValueIdentifier(destructor.address)),
			    .functionSymbol = destructor.mangledName,
			    .functionName = destructor.functionName,
			    .functionPtr = destructor.ptr,
			});
		}
		const auto calleeId = internCallee(functionCallTarget, resultType, inputArguments);
		auto callOperation = currentBlock->addTaggedOperation<CallOperation>(
		    provenanceOf(operation), functionCallTarget.mangledName, functionCallTarget.functionName,
		    functionCallTarget.ptr, resultIdentifier, inputArguments, resultType, functionCallTarget.fnAttrs, calleeId,
		    std::move(destructors), operation.op == Op::CALL_WITH_EXCEPTION_HANDLING, functionCallTarget.captureFunc,
		    functionCallTarget.isNautilusCall);
		if (resultType != Type::v) {
			frame.setValue(resultIdentifier, callOperation);
		}
	}

	template <typename Frame>
	void processIndirectCall(Frame& frame, BasicBlock* currentBlock, TraceOperation& operation) {
		const IndirectFunctionCall& indirectCall = *std::get<IndirectFunctionCall*>(operation.input[0]);
		auto fnPtrOperand = frame.getValue(createValueIdentifier(indirectCall.fnPtr));
		auto inputArguments = std::vector<Operation*> {};
		for (const auto& argument : indirectCall.arguments) {
			inputArguments.emplace_back(frame.getValue(createValueIdentifier(argument)));
		}
		auto resultType = operation.resultType;
		auto resultIdentifier = createValueIdentifier(operation.resultRef);
		auto destructors = std::vector<IndirectCallOperation::Destructor> {};
		destructors.reserve(indirectCall.destructors.size());
		for (const auto& destructor : indirectCall.destructors) {
			destructors.push_back(IndirectCallOperation::Destructor {
			    .address = frame.getValue(createValueIdentifier(destructor.address)),
			    .functionSymbol = destructor.mangledName,
			    .functionName = destructor.functionName,
			    .functionPtr = destructor.ptr,
			});
		}
		auto indirectCallOp = currentBlock->addTaggedOperation<IndirectCallOperation>(
		    provenanceOf(operation), resultIdentifier, fnPtrOperand, inputArguments, resultType, indirectCall.fnAttrs,
		    std::move(destructors), operation.op == Op::INDIRECT_CALL_WITH_EXCEPTION_HANDLING,
		    indirectCall.captureFunc);
		if (resultType != Type::v) {
			frame.setValue(resultIdentifier, indirectCallOp);
		}
	}

	template <typename Frame>
	void processFuncAddr(Frame& frame, BasicBlock* currentBlock, TraceOperation& operation) {
		const FunctionCall& functionCallTarget = *std::get<FunctionCall*>(operation.input[0]);
		auto resultIdentifier = createValueIdentifier(operation.resultRef);
		// Taking an address interns the same target a call to it would, so the two
		// share one table entry -- and so address-of resolves for every linkage,
		// not just for functions defined in this module.
		const auto calleeId = internCallee(functionCallTarget, Type::ptr, {});
		auto funcAddrOp = currentBlock->addTaggedOperation<FunctionAddressOfOperation>(
		    provenanceOf(operation), functionCallTarget.mangledName, functionCallTarget.functionName,
		    functionCallTarget.ptr, resultIdentifier, calleeId);
		frame.setValue(resultIdentifier, funcAddrOp);
	}

	template <typename Frame>
	void processConst(Frame& frame, BasicBlock* currentBlock, TraceOperation& operation) {
		auto constant = std::get<ConstantLiteral>(operation.input[0]);
		auto resultIdentifier = createValueIdentifier(operation.resultRef);
		auto resultType = operation.resultType;
		const auto provenance = provenanceOf(operation);
		Operation* constOperation;
		std::visit(
		    [&](auto&& value) {
			    using T = std::decay_t<decltype(value)>;
			    if constexpr (std::is_same_v<T, bool>) {
				    constOperation =
				        currentBlock->addTaggedOperation<ConstBooleanOperation>(provenance, resultIdentifier, value);
			    } else if constexpr (std::is_integral_v<T>) {
				    constOperation = currentBlock->addTaggedOperation<ConstIntOperation>(provenance, resultIdentifier,
				                                                                         value, resultType);
			    } else if constexpr (std::is_floating_point_v<T>) {
				    constOperation = currentBlock->addTaggedOperation<ConstFloatOperation>(provenance, resultIdentifier,
				                                                                           value, resultType);
			    } else if constexpr (std::is_pointer_v<T>) {
				    constOperation =
				        currentBlock->addTaggedOperation<ConstPtrOperation>(provenance, resultIdentifier, value);
			    } else {
				    // static_assert(false, "non-exhaustive visitor!");
			    }
		    },
		    constant);

		frame.setValue(resultIdentifier, constOperation);
	}

	template <typename Frame>
	void processCast(Frame& frame, BasicBlock* currentBlock, TraceOperation& operation) {
		auto resultIdentifier = createValueIdentifier(operation.resultRef);
		auto input = frame.getValue(createValueIdentifier(operation.input[0]));
		auto castOperation = currentBlock->addTaggedOperation<CastOperation>(provenanceOf(operation), resultIdentifier,
		                                                                     input, operation.resultType);
		frame.setValue(resultIdentifier, castOperation);
	}

	template <typename Frame>
	void processAlloca(Frame& frame, BasicBlock* currentBlock, TraceOperation& operation) {
		auto resultIdentifier = createValueIdentifier(operation.resultRef);
		AllocaIndex index = std::get<AllocaIndex>(operation.input[0]);
		auto allocaOperation =
		    currentBlock->addTaggedOperation<AllocaOperation>(provenanceOf(operation), resultIdentifier, index);
		frame.setValue(resultIdentifier, allocaOperation);
	}

	/// Key identifying one region() call site within an enclosing region: what the
	/// call site said about itself, plus where it sits in the nesting.
	struct RegionKey {
		const char* name;
		const char* file;
		uint32_t line;
		uint32_t column;
		compiler::ir::RegionIndex parent;

		bool operator==(const RegionKey&) const = default;
	};

	struct RegionKeyHash {
		size_t operator()(const RegionKey& key) const noexcept;
	};

	ExecutionTrace* trace;
	std::shared_ptr<compiler::ir::IRGraph> ir;
	/// Region table of the function currently being built, and the two indexes into
	/// it: by the trace region it came from, and by call site (see mapRegion).
	std::vector<compiler::ir::RegionSpec> currentRegionSpecs;
	std::unordered_map<RegionIndex, compiler::ir::RegionIndex> regionMap;
	std::unordered_map<RegionKey, compiler::ir::RegionIndex, RegionKeyHash> regionKeys;
};

} // namespace nautilus::tracing

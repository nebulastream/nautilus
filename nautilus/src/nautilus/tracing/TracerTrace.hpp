#pragma once

#include "Snapshot.hpp"
#include "TracerTrace.hpp"
#include "nautilus/common/FunctionAttributes.hpp"
#include "nautilus/tracing/Operations.hpp"
#include "nautilus/tracing/TypedValueRef.hpp"
#include "nautilus/tracing/Types.hpp"
#include "tag/SharedHashMap.hpp"
#include <array>
#include <cassert>
#include <chrono>
#include <cstdint>
#include <cstring>
#include <span>
#include <string>
#include <sys/mman.h>
#include <type_traits>
#include <unordered_map>

namespace nautilus::tracing {
/// Operation identifier using byte offset instead of array index

// Input structs for different operation types
struct BinaryInput {
	TypedValueRef lhs;
	TypedValueRef rhs;
};

struct UnaryInput {
	TypedValueRef operand;
};

struct CmpInput {
	TypedValueRef condition;
	uint16_t trueBlock;
	uint16_t falseBlock;
	double probability;
};

struct JmpInput {
	uint16_t targetBlock;
};

struct MemoryInput {
	TypedValueRef ptr;
	TypedValueRef value;
};

struct CallInput {
	void* funcPtr;
	FunctionAttributes attrs;
	uint8_t argCount;
	// TypedValueRef args[argCount] follow inline after this struct
};

struct SelectInput {
	TypedValueRef condition;
	TypedValueRef trueValue;
	TypedValueRef falseValue;
};

/// Base operation header (fixed part of every operation)
struct TracerOperationHeader {
	Snapshot tag;
	Op op;
	Type resultType;
	TypedValueRef resultRef;

	// Get pointer to input data (immediately after header)
	template <typename T>
	T* getInput() {
		return reinterpret_cast<T*>(reinterpret_cast<char*>(this) + sizeof(TracerOperationHeader));
	}

	template <typename T>
	const T* getInput() const {
		return reinterpret_cast<const T*>(reinterpret_cast<const char*>(this) + sizeof(TracerOperationHeader));
	}
};

// Forward declarations for use in validateArgumentType
bool isBinaryOp(Op op);
bool isUnaryOp(Op op);

struct Trace {
	struct TraceHeader;
	struct OperationHeader;
	struct BlockHeader;

	struct BlockToOperationOffset {
		size_t offset;
		OperationHeader* address(BlockHeader* block) const {
			return reinterpret_cast<OperationHeader*>(reinterpret_cast<char*>(block) + offset);
		}
	};

	struct BlockToBlockOffset {
		size_t offset;
		static BlockToBlockOffset create(BlockHeader* prev, BlockHeader* succ) {
			assert(prev != nullptr && succ != nullptr && "Cannot create offset for null pointer");
			assert(reinterpret_cast<uintptr_t>(prev) < reinterpret_cast<uintptr_t>(succ) &&
			       "I don't know, but the prev block should have a lower address as the successor");
			assert(prev->magic == 0xDEADBEEF && "PREV Block header magic is incorrect");
			assert(succ->magic == 0xDEADBEEF && "SUCC Block header magic is incorrect");

			return BlockToBlockOffset(reinterpret_cast<uintptr_t>(succ) - reinterpret_cast<uintptr_t>(prev));
		}
		BlockHeader* address(BlockHeader* block) const {
			assert(offset != 0 && "BlockToBlockOffset should not be zero");
			return reinterpret_cast<BlockHeader*>(reinterpret_cast<char*>(block) + offset);
		}
	};

	struct BlockToFreeMemoryOffset {
		size_t offset;
		void* address(BlockHeader* block) const {
			return reinterpret_cast<char*>(block) + offset;
		}
	};

	struct TraceToBlockOffset {
		size_t offset;
		static TraceToBlockOffset create(TraceHeader* trace, BlockHeader* block) {
			assert(trace != nullptr && block != nullptr && "Cannot create offset for null pointer");
			assert(reinterpret_cast<uintptr_t>(trace) < reinterpret_cast<uintptr_t>(block) &&
			       "I don't know, but the block should have an higher address as the trace");
			assert(block->magic == 0xDEADBEEF && "Block header magic is incorrect");
			return TraceToBlockOffset(reinterpret_cast<uintptr_t>(block) - reinterpret_cast<uintptr_t>(trace));
		}
		BlockHeader* address(TraceHeader* block) const {
			assert(offset != 0 && "TraceToBlockOffset should not be zero");
			return reinterpret_cast<BlockHeader*>(reinterpret_cast<char*>(block) + offset);
		}
	};

	struct TraceToOperationOffset {
		size_t offset;

		static TraceToOperationOffset create(TraceHeader* trace, OperationHeader* operation) {
			assert(trace != nullptr && operation != nullptr && "Cannot create offset for null pointer");
			assert(reinterpret_cast<uintptr_t>(trace) < reinterpret_cast<uintptr_t>(operation) &&
			       "I don't know, but the operation should have an higher address as the trace");
			assert(operation->magic == 0xC0FFEE && "Block header magic is incorrect");
			return TraceToOperationOffset(reinterpret_cast<uintptr_t>(operation) - reinterpret_cast<uintptr_t>(trace));
		}

		OperationHeader* address(TraceHeader* block) const {
			return reinterpret_cast<OperationHeader*>(reinterpret_cast<char*>(block) + offset);
		}
	};

	friend TraceToOperationOffset operator+(const TraceToBlockOffset& traceToBlock,
	                                        const BlockToOperationOffset& blockToOperation);
	friend TraceToBlockOffset operator+(const TraceToBlockOffset& traceToBlock,
	                                    const BlockToBlockOffset& blockToBlockOffset);

	struct TraceToFreeMemoryOffset {
		size_t offset;
		void* address(TraceHeader* block) const {
			return reinterpret_cast<char*>(block) + offset;
		}
	};

	struct TracerOperationIdentifier {
		TraceToBlockOffset blockIndex;
		TraceToOperationOffset operationOffset;
	};

	struct OperationHeader {
		Snapshot tag;
		TypedValueRef resultRef;
		Op op;
		size_t magic = 0xC0FFEE;
		/// arguments
	};

	struct BlockHeader {
		size_t blockId;
		bool controlFlowMerge = false;
		/// BlockEnd and nextBlockOffset is not identical. There might be holes in the trace due to a controlFlowMerge
		/// which has erased operations
		BlockToFreeMemoryOffset blockEnd {sizeof(BlockHeader)};
		BlockToBlockOffset nextBlockOffset {0};
		size_t magic = 0xDEADBEEF;
		/// Operation operations[]
	};

	struct Operation {
		explicit Operation(OperationHeader* view) : view(view) {
			assert(view->magic == 0xC0FFEE && "Operation header magic is incorrect");
		}
		OperationHeader* view;
		size_t size();

		TypedValueRef& getResultRef() {
			return view->resultRef;
		}
		const TypedValueRef& getResultRef() const {
			return view->resultRef;
		}

		template <typename T>
		static constexpr bool validateArgumentType(Op op) {
			if constexpr (std::is_same_v<T, BinaryInput>) {
				return isBinaryOp(op);
			} else if constexpr (std::is_same_v<T, UnaryInput>) {
				return isUnaryOp(op);
			} else if constexpr (std::is_same_v<T, MemoryInput>) {
				return op == LOAD || op == STORE;
			} else if constexpr (std::is_same_v<T, CmpInput>) {
				return op == CMP;
			} else if constexpr (std::is_same_v<T, JmpInput>) {
				return op == JMP;
			} else if constexpr (std::is_same_v<T, ConstantLiteral>) {
				return op == CONST;
			} else if constexpr (std::is_same_v<T, CallInput>) {
				return op == CALL;
			} else if constexpr (std::is_same_v<T, TypedValueRef>) {
				return op == RETURN || op == ASSIGN || op == FREE;
			} else if constexpr (std::is_same_v<T, SelectInput>) {
				return op == SELECT;
			} else {
				return false;
			}
		}

		template <typename T>
		T& arguments() {
			assert(validateArgumentType<T>(view->op) && "Invalid argument type for operation");
			return *reinterpret_cast<T*>(reinterpret_cast<char*>(view) + sizeof(OperationHeader));
		}
	};

	struct Block {
		explicit Block(BlockHeader* view) : view(view) {
			assert(view->magic == 0xDEADBEEF && "Block header magic is incorrect");
		}
		BlockHeader* view;

		std::vector<Operation> getOperations() {
			std::vector<Operation> operations;
			BlockToOperationOffset offset {sizeof(BlockHeader)};
			while (offset.offset < view->blockEnd.offset) {
				operations.emplace_back(offset.address(view));
				offset.offset += operations.back().size();
			}
			return operations;
		}

		size_t getBlockId() {
			return view->blockId;
		}
	};

	struct TraceHeader {
		size_t forkCount = 0;
		size_t maxForkDepth = 0;
		size_t numberOfBlocks = 0;
		TraceToBlockOffset currentBlockOffset {0};
		TraceToFreeMemoryOffset freeMemoryOffset {sizeof(TraceHeader)};
		std::array<TypedValueRef, 64> arguments;
		size_t valueRefCounter = 1;
		size_t argumentCounter = 0;
		/// Block blocks[]
	};

	Operation getOperation(TracerOperationIdentifier oi) {
		return Operation(oi.operationOffset.address(view));
	}

	Block getCurrentBlock() {
		assert(view->numberOfBlocks > 0 && "Trace does not have an active block");
		return Block(view->currentBlockOffset.address(view));
	}

	template <typename T, typename... Args>
	T* allocate(Args&&... args) {
		T* ptr = new (view->freeMemoryOffset.address(view)) T(std::forward<Args>(args)...);
		view->freeMemoryOffset.offset += sizeof(T);
		return ptr;
	}

	Block createNewBlock() {
		if (view->numberOfBlocks != 0) {
			auto previousBlock = getCurrentBlock();
			auto newBlock = Block(allocate<BlockHeader>());
			newBlock.view->blockId = view->numberOfBlocks++;
			view->currentBlockOffset = TraceToBlockOffset::create(view, newBlock.view);
			previousBlock.view->nextBlockOffset = BlockToBlockOffset::create(previousBlock.view, newBlock.view);
			return newBlock;
		}

		auto newBlock = Block(allocate<BlockHeader>());
		newBlock.view->blockId = view->numberOfBlocks++;
		view->currentBlockOffset = TraceToBlockOffset::create(view, newBlock.view);
		return newBlock;
	}

	std::vector<Block> getBlocks() {
		std::vector<Block> blocks;
		blocks.reserve(view->numberOfBlocks);
		TraceToBlockOffset offset {sizeof(TraceHeader)};
		for (size_t i = 0; i < view->numberOfBlocks; i++) {
			blocks.emplace_back(offset.address(view));
			offset = offset + blocks.back().view->nextBlockOffset;
		}
		return blocks;
	}

	std::span<const TypedValueRef> getArguments() const {
		return std::span<const TypedValueRef>(view->arguments.data(), view->argumentCounter);
	}

	std::span<TypedValueRef> getArguments() {
		return std::span(view->arguments.data(), view->argumentCounter);
	}

	template <typename T>
	std::pair<TypedValueRef, TracerOperationIdentifier> appendOperation(Snapshot snapshot, TypedValueRef resultRef,
	                                                                    Op op, T&& args) {
		assert(view->numberOfBlocks > 0 && "Trace does not have an active block");
		Operation operation(allocate<OperationHeader>(std::move(snapshot), std::move(resultRef), op));
		allocate<std::decay_t<T>>(std::forward<T>(args));

		auto operationOffset = BlockToOperationOffset(getCurrentBlock().view->blockEnd.offset);
		getCurrentBlock().view->blockEnd.offset += operation.size();
		return {resultRef,
		        TracerOperationIdentifier(view->currentBlockOffset, view->currentBlockOffset + operationOffset)};
	}

	template <typename T>
	std::pair<TypedValueRef, TracerOperationIdentifier> appendOperation(Snapshot snapshot, Type type, Op op, T&& args) {
		return appendOperation(std::move(snapshot), TypedValueRef(view->valueRefCounter++, type), op,
		                       std::forward<T>(args));
	}

	std::pair<TypedValueRef, TracerOperationIdentifier> appendJump(uint16_t targetBlock) {
		return appendOperation<JmpInput>({}, TypedValueRef {}, JMP, JmpInput(targetBlock));
	}

	std::pair<TypedValueRef, TracerOperationIdentifier> appendCmp(Snapshot snapshot, const TypedValueRef condition,
	                                                              const double probability) {
		return appendOperation<CmpInput>(snapshot, {}, CMP, CmpInput(condition, 0, 0, probability));
	}

	std::pair<TypedValueRef, TracerOperationIdentifier> addReturn(Snapshot snapshot, TypedValueRef value) {
		auto result = appendOperation<>(snapshot, TypedValueRef(0, value.type), RETURN, value);
		return result;
	}

	/// Add a function call operation
	std::pair<TypedValueRef, TracerOperationIdentifier> addCallOperation(Snapshot snapshot, Type type, void* funcPtr,
	                                                                     FunctionAttributes attrs,
	                                                                     std::span<const TypedValueRef> args) {
		auto oi = appendOperation<CallInput>(std::move(snapshot), type, CALL, CallInput(funcPtr, attrs, args.size()));
		for (const auto& arg : args) {
			allocate<TypedValueRef>(arg);
		}

		return oi;
	}

	/// Add a constant operation
	std::pair<TypedValueRef, TracerOperationIdentifier> addConstOperation(Snapshot snapshot, Type type,
	                                                                      ConstantLiteral value) {
		return appendOperation(std::move(snapshot), type, CONST, std::move(value));
	}

	TypedValueRef addArgument(Type t) {
		assert(view->argumentCounter < view->arguments.size() && "Too many arguments");
		auto result = TypedValueRef(view->valueRefCounter++, t);
		view->arguments[view->argumentCounter++] = result;
		return result;
	}

	std::pair<TypedValueRef, TracerOperationIdentifier>
	addAssignmentOperation(Snapshot snapshot, TypedValueRef targetRef, TypedValueRef srcRef) {
		return appendOperation(std::move(snapshot), targetRef, ASSIGN, std::move(srcRef));
	}

	std::pair<TypedValueRef, TracerOperationIdentifier> addAssignmentOperation(Snapshot snapshot,
	                                                                           TypedValueRef srcRef) {
		return appendOperation(std::move(snapshot), srcRef.type, ASSIGN, std::move(srcRef));
	}

	bool checkTag(Snapshot& snapshot) {
		// check if operation is in global map -> we have a repeating operation ->
		// this is a control-flow merge or loop
		if (auto* ref = globalTagMap.get(snapshot)) {
			processControlFlowMerge(*ref);
			return false;
		}
		return true;
	}

	/// Add an operation that produces a result.
	/// Constructs the correct input struct (BinaryInput, UnaryInput, etc.) based on the Op type.
	std::pair<TypedValueRef, TracerOperationIdentifier>
	addOperationWithResult(Snapshot snapshot, Type resultType, Op op, std::span<const TypedValueRef> operands);

	/// Overload for initializer list convenience
	std::pair<TypedValueRef, TracerOperationIdentifier>
	addOperationWithResult(Snapshot snapshot, Type resultType, Op op, std::initializer_list<TypedValueRef> operands) {
		return addOperationWithResult(snapshot, resultType, op,
		                              std::span<const TypedValueRef>(operands.begin(), operands.size()));
	}

	void processControlFlowMerge(TracerOperationIdentifier oi) {
		auto mergeBlockIndex = view->numberOfBlocks;
		/// append jump to next block into current block
		appendJump(mergeBlockIndex);

		auto originalBlock = Block(oi.blockIndex.address(view));

		auto mergeBlock = createNewBlock();
		mergeBlock.view->controlFlowMerge = true;

		/// copy everything from the duplicated operation until the end of the block as the merged operations in the
		/// merge block
		auto operationBytes = originalBlock.view->blockEnd.offset - (oi.operationOffset.offset - oi.blockIndex.offset);
		memcpy(view->freeMemoryOffset.address(view), oi.operationOffset.address(view), operationBytes);

		/// This will grow the new merge block
		mergeBlock.view->blockEnd.offset += operationBytes;
		/// and we have to manually bump the allocation
		view->freeMemoryOffset.offset += operationBytes;

		/// original block is truncated to not include the duplicated instruction
		originalBlock.view->blockEnd.offset = (oi.operationOffset.offset - oi.blockIndex.offset);

		/// now we add a jump instruction
		new (originalBlock.view->blockEnd.address(originalBlock.view)) OperationHeader({}, {}, JMP);
		originalBlock.view->blockEnd.offset += sizeof(OperationHeader);
		new (originalBlock.view->blockEnd.address(originalBlock.view)) JmpInput(mergeBlockIndex);
		originalBlock.view->blockEnd.offset += sizeof(JmpInput);

		/// Update Tags in global tag map. The now point to a different operation
		auto operations = mergeBlock.getOperations();
		for (auto operation : operations) {
			if (operation.view->tag == Snapshot {}) {
				continue;
			}
			auto tag = globalTagMap.get(operation.view->tag);
			if (tag == nullptr) {
				continue;
			}
			*tag = {.blockIndex = view->currentBlockOffset,
			        .operationOffset = TraceToOperationOffset::create(view, operation.view)};
		}
	}

	TraceHeader* view;
	size_t capacity;
	size_t maxReturns;
	Map<Snapshot, TracerOperationIdentifier> globalTagMap {1024 * 1024};

	Trace();

	~Trace();
};

/// Calculate operation size based on Op type
size_t getOperationSize(Op op, uint8_t argCount = 0);

/// Check if an operation is a binary operation
bool isBinaryOp(Op op);

/// Check if an operation is a unary operation
bool isUnaryOp(Op op);

/// Convert a Trace to a human-readable string
std::string toString(Trace& trace);

// Forward declaration
class ExecutionTrace;

/// Convert a Trace to an ExecutionTrace
ExecutionTrace toExecutionTrace(Trace& trace);

} // namespace nautilus::tracing

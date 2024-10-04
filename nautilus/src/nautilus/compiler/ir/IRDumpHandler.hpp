#pragma once

#include "nautilus/compiler/ir/IRGraph.hpp"
#include "nautilus/compiler/ir/blocks/BasicBlock.hpp"
#include <memory>
#include <unordered_set>

namespace nautilus::compiler::ir {

/**
 * @brief Converts query plans and pipeline plans to the .nesviz format and dumps them to a file.m
 */
class NESIRDumpHandler {

public:
	virtual ~NESIRDumpHandler();

	static std::shared_ptr<NESIRDumpHandler> create(std::ostream& out);

	explicit NESIRDumpHandler(std::ostream& out);

	/**
	 * @brief Dump the NESIR of the funcOp into the 'out' stringstream.
	 * @param funcOp: FunctionOperation that exists on the top level of a NESIR module.
	 */
	void dump(const std::unique_ptr<FunctionOperation>& funcOp);

private:
	std::ostream& out;
	std::unordered_set<std::string> visitedBlocks; // We keep track of visited blocks to avoid multi or infinite
	                                               // dumping.

	/**
	 * @brief Traverses the NESIR to find a BB that is on the same or higher 'blockScopeLevel' compared to the initial
	 * 'basicBlock'. Note: There is always a 'next block', since we always have a return block at the very end of a
	 * function.
	 * @param basicBlock: Initially the block that we want to find the next BB for. Replaced while recursively
	 * traversing NESIR.
	 */
	const BasicBlock* getNextLowerOrEqualLevelBasicBlock(const BasicBlock* basicBlock);

	/**
	 * @brief Handle dumping terminator operations(LoopOp, BranchOp, IfOp, ReturnOp) to the 'out' stringstream.
	 *
	 * @param terminatorOp: Terminator operation that we append to the 'out' stringstream.
	 * @param scopeLevel: scopeLevel of the BasicBlock that is terminated by the terminator operation.
	 */
	void dumpHelper(Operation* terminatorOp);

	/**
	 * @brief Handle dumping BasicBlocks to the 'out' stringstream. Print all operations, then handle the terminatorOp.
	 *
	 * @param basicBlock: The basicBlock that is dumped to the 'out' stringstream.
	 */
	void dumpHelper(const BasicBlock* basicBlock);
};

} // namespace nautilus::compiler::ir


#include "GraphVizUtil.hpp"
#include "nautilus/compiler/ir/blocks/BasicBlock.hpp"
#include "nautilus/compiler/ir/operations/BranchOperation.hpp"
#include "nautilus/compiler/ir/operations/FunctionOperation.hpp"
#include "nautilus/compiler/ir/operations/IfOperation.hpp"
#include "nautilus/compiler/ir/operations/ReturnOperation.hpp"
#include "spdlog/fmt/fmt.h"
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

namespace nautilus::compiler::ir {
/*
digraph G {

    subgraph cluster_0 {
        style = filled;
        color = lightgrey;
        node[style = filled, color = white];
        a0->a1->a2->a3;
        label = "process #1";
    }

    subgraph cluster_1 {
        node[style = filled];
        b0->b1->b2->b3;
        label = "process #2";
        color = blue
    }
    start->a0;
    start->b0;
    a1->b3;
    b2->a3;
    a3->a0;
    a3->end;
    b3->end;

    start[shape = Mdiamond];
    end[shape = Msquare];
}
*/

std::string urlEncode(const std::string& value) {
	std::ostringstream encoded;
	encoded.fill('0');
	encoded << std::hex;

	for (const char& c : value) {
		// Check if the character is alphanumeric or one of the few unreserved characters
		if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
			encoded << c;
		} else {
			// For all other characters, convert to %XX format
			encoded << '%' << std::setw(2) << static_cast<int>(static_cast<unsigned char>(c));
		}
	}

	return encoded.str();
}

std::string createFunctionArguments(const std::shared_ptr<IRGraph>& graph) {
	// args of basic block
	std::stringstream ss;
	auto& functionArgs = graph->getRootOperation()->getFunctionBasicBlock()->getArguments();
	for (size_t i = 0; i < functionArgs.size(); i++) {
		auto& arg = functionArgs[i];
		ss << fmt::format(R"(arg_{}[shape="box", label="Arg"{}];)", i, arg->toString()) << "\n";
	}
	return ss.str();
}

std::string createGraphVizFromIr(const std::shared_ptr<IRGraph>& graph) {
	std::stringstream ss;
	std::vector<std::string> blockNode;
	std::vector<std::string> blockEdged;
	auto& basicBlocks = graph->getRootOperation()->getBasicBlocks();
	for (auto& block : basicBlocks) {
		std::stringstream blockSS;
		blockSS << "subgraph cluster_" << block->getIdentifier() << "{" << "\n";
		blockSS << "style = filled;" << "\n";
		blockSS << "color = lightgrey;" << "\n";
		blockSS << "node[style = filled, color = white];" << "\n";

		auto& ops = block->getOperations();

		for (size_t i = 0; i < ops.size(); i++) {
			auto& op = ops[i];
			blockSS << "op_" << block->getIdentifier() << "_" << i << "[label = \"" << op->toString() << "\"]" << "\n";
		}

		for (size_t i = 1; i < ops.size(); i++) {
			blockSS << "op_" << block->getIdentifier() << "_" << i - 1;
			blockSS << "->";
			blockSS << "op_" << block->getIdentifier() << "_" << i << "\n";
		}
		blockSS << "label = " << block->getIdentifier() << ";" << "\n";
		blockSS << "}" << "\n";
		blockNode.emplace_back(blockSS.str());

		auto& lastOp = ops.back();

		if (lastOp->getOperationType() == Operation::OperationType::IfOp) {
			auto ifOp = static_cast<IfOperation*>(lastOp.get());
			auto blockEdge = "op_" + block->getIdentifier() + "_" + std::to_string(ops.size() - 1) + "->" + "op_" + ifOp->getTrueBlockInvocation().getBlock()->getIdentifier() + "_0" + "\n";
			blockEdged.emplace_back(blockEdge);
			blockEdge = "op_" + block->getIdentifier() + "_" + std::to_string(ops.size() - 1) + "->" + "op_" + ifOp->getFalseBlockInvocation().getBlock()->getIdentifier() + "_0" + "\n";
			blockEdged.emplace_back(blockEdge);
		}
		if (lastOp->getOperationType() == Operation::OperationType::BranchOp) {
			auto branchOp = static_cast<BranchOperation*>(lastOp.get());
			auto blockEdge = "op_" + block->getIdentifier() + "_" + std::to_string(ops.size() - 1) + "->" + "op_" + branchOp->getNextBlockInvocation().getBlock()->getIdentifier() + "_0" + "\n ";
			blockEdged.emplace_back(blockEdge);
		}
	}

	ss << "digraph G {" << "\n";
	ss << "compound=true;" << "\n";
	ss << createFunctionArguments(graph);
	for (auto& block : blockNode) {
		ss << block;
	}
	for (auto& edge : blockEdged) {
		ss << edge;
	}
	ss << "}" << "\n";

	auto content = ss.str();
	std::cout << content << std::endl;
	return "https://dreampuf.github.io/GraphvizOnline/#" + urlEncode(content);
}
} // namespace nautilus::compiler::ir

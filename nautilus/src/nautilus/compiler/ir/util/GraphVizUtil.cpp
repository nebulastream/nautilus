
#include "GraphVizUtil.hpp"
#include "nautilus/compiler/ir/blocks/BasicBlock.hpp"
#include "nautilus/compiler/ir/operations/BranchOperation.hpp"
#include "nautilus/compiler/ir/operations/ConstBooleanOperation.hpp"
#include "nautilus/compiler/ir/operations/ConstFloatOperation.hpp"
#include "nautilus/compiler/ir/operations/ConstIntOperation.hpp"
#include "nautilus/compiler/ir/operations/ConstPtrOperation.hpp"
#include "nautilus/compiler/ir/operations/FunctionOperation.hpp"
#include "nautilus/compiler/ir/operations/IfOperation.hpp"
#include "nautilus/compiler/ir/operations/ProxyCallOperation.hpp"
#include "nautilus/compiler/ir/operations/ReturnOperation.hpp"
#include "spdlog/fmt/fmt.h"
#include <iomanip>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <unordered_map>

namespace fmt {
using namespace nautilus::compiler::ir;
template <>
struct formatter<nautilus::compiler::ir::Operation> : formatter<std::string_view> {
	static auto format(const nautilus::compiler::ir::Operation& c, format_context& ctx) -> format_context::iterator;
};
} // namespace fmt

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

const std::string WHITE_ICE = "#d7ede7";
// const std::string CRUISE = "#b8ddd1";
const std::string KEPPEL = "#3cb4a4";
const std::string CARISSMA = "#e98693";
const std::string AMARANTH = "#da2d4f";
const std::string BLACK = "#1a1919";
// const std::string WHITE = "#ffffff";
const std::string DUST = "#f9f9f9";
const std::string BIG_STONE = "#343d46";
// const std::string ICE_STONE = "#b3bbc3";
const std::string ORANGE = "#ffa500";
// const std::string LIGHT_BLUE = "#ccccff";
const std::string LIGHT_PURPLE = "#c39bd3";
const std::string LIGHT_YELLOW = "#ffffde";
const std::string DARK_YELLOW = "#aaaa33";
const std::map<std::string, std::string> EDGE_COLORS = {
    {"info", BIG_STONE}, {"control", AMARANTH}, {"loop", AMARANTH}, {"data", KEPPEL}, {"other", BLACK}};
const std::map<std::string, std::pair<std::string, std::string>> NODE_COLORS = {
    {"info", {DUST, BLACK}},           {"input", {WHITE_ICE, BLACK}},     {"control", {CARISSMA, BLACK}},
    {"memory", {LIGHT_PURPLE, BLACK}}, {"call", {AMARANTH, "white"}},     {"alloc", {AMARANTH, "white"}},
    {"sync", {AMARANTH, "white"}},     {"virtual", {BIG_STONE, "white"}}, {"guard", {ORANGE, BLACK}},
    {"calc", {KEPPEL, BLACK}},         {"other", {DUST, BLACK}}};

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

std::string write_attrs(const std::map<std::string, std::string>& attrs) {
	std::string result = "[";
	bool first = true;
	for (const auto& [key, value] : attrs) {
		if (!first) {
			result += ", ";
		}
		result += key + "=" + "\"" + value + "\"";
		first = false;
	}
	result += "]";
	return result;
}

class GraphvizWriter {
public:
	GraphvizWriter(std::ostream& stream) : stream(stream) {
	}
	void end_graph() {
		stream << "}" << std::endl;
	}
	void start_graph(const std::map<std::string, std::string>& attrs) {
		stream << "digraph G {" << std::endl;
		stream << "  graph " << write_attrs(attrs) << ";" << std::endl;
	}

	void write_block_argument_edges([[maybe_unused]] const std::shared_ptr<IRGraph>& graph,
	                                const BasicBlockInvocation& bi,
	                                [[maybe_unused]] bool hideIntermediateBlockArguments) {
		auto targetBlock = bi.getBlock();
		auto preBlocks = getPredecessorBlocks(graph, targetBlock);
		// if (hideIntermediateBlockArguments && preBlocks.size() == 1){
		//	return;
		// }
		//  crate an edge from the src block op to the argument in the target block.
		for (size_t i = 0; i < bi.getArguments().size(); i++) {
			const auto& from = nodeIdMap[bi.getArguments()[i]];
			const auto& to = nodeIdMap[targetBlock->getArguments()[i].get()];
			write_edge(from, to, "data");
		}
	}

	std::vector<std::tuple<BasicBlock*, BasicBlockInvocation*>>
	getPredecessorBlocks(const std::shared_ptr<IRGraph>& graph, const BasicBlock* targetBlock) {
		std::vector<std::tuple<BasicBlock*, BasicBlockInvocation*>> predeccessor;
		for (const auto& block : graph->getRootOperation().getBasicBlocks()) {
			auto& op = block->getOperations().back();
			if (op->getOperationType() == Operation::OperationType::IfOp) {
				auto ifOp = as<IfOperation>(op);
				if (ifOp->getFalseBlockInvocation().getBlock() == targetBlock) {
					auto tup = std::make_tuple<BasicBlock*, BasicBlockInvocation*>(block.get(),
					                                                               &ifOp->getFalseBlockInvocation());
					predeccessor.emplace_back(tup);
				}
				if (ifOp->getTrueBlockInvocation().getBlock() == targetBlock) {
					auto tup = std::make_tuple<BasicBlock*, BasicBlockInvocation*>(block.get(),
					                                                               &ifOp->getTrueBlockInvocation());
					predeccessor.emplace_back(tup);
				}
			} else if (op->getOperationType() == Operation::OperationType::BranchOp) {
				auto branchOp = as<BranchOperation>(op);
				if (branchOp->getNextBlockInvocation().getBlock() == targetBlock) {
					auto tup = std::make_tuple<BasicBlock*, BasicBlockInvocation*>(block.get(),
					                                                               &branchOp->getNextBlockInvocation());
					predeccessor.emplace_back(tup);
				}
			}
		}
		return predeccessor;
	}

	std::string getFromInput([[maybe_unused]] const std::shared_ptr<IRGraph>& graph,
	                         [[maybe_unused]] const BasicBlock* block, Operation* input,
	                         bool hideIntermediateBlockArguments) {

		if (hideIntermediateBlockArguments &&
		    input->getOperationType() == Operation::OperationType::BasicBlockArgument) {
			[[maybe_unused]] const auto* arg = as<const BasicBlockArgument>(input);
			size_t index = 0;
			for (size_t i = 0; i < block->getArguments().size(); i++) {
				if (block->getArguments()[i].get() == input) {
					index = i;
					break;
				}
			}
			auto preBlock = getPredecessorBlocks(graph, block);
			if (preBlock.size() == 1) {
				auto pred = preBlock[0];
				auto blockInvoke = std::get<1>(pred);
				auto srcOp = blockInvoke->getArguments()[index];
				return getFromInput(graph, std::get<0>(pred), srcOp, hideIntermediateBlockArguments);
			}
		}

		const auto& from = nodeIdMap[input];
		return from;
	}

	void addToBlockArgumentMap(const BasicBlockInvocation* bi, std::map<Operation*, std::vector<Operation*>>& map) {
		for (size_t i = 0; i < bi->getArguments().size(); i++) {
			const auto& from = bi->getArguments()[i];
			const auto& to = bi->getBlock()->getArguments()[i].get();
			std::vector<Operation*> inputs;
			if (map.contains(to)) {
				auto srcInput = map[to];
				for (auto src : srcInput) {
					inputs.emplace_back(src);
				}
			}

			if (map.contains(from)) {
				auto srcInput = map[from];
				for (auto src : srcInput) {
					inputs.emplace_back(src);
				}
			} else {
				inputs.emplace_back(from);
			}
			map[to] = inputs;
		}
	}

	void getBlockArgumentMap(const BasicBlock* block, std::set<const BasicBlock*>& visitedBlocks,
	                         std::map<Operation*, std::vector<Operation*>>& map) {
		if (visitedBlocks.contains(block)) {
			return;
		}
		visitedBlocks.emplace(block);

		auto& op = block->getOperations().back();
		if (op->getOperationType() == Operation::OperationType::IfOp) {
			auto ifOp = as<IfOperation>(op);
			addToBlockArgumentMap(&ifOp->getTrueBlockInvocation(), map);
			addToBlockArgumentMap(&ifOp->getFalseBlockInvocation(), map);
			getBlockArgumentMap(ifOp->getTrueBlockInvocation().getBlock(), visitedBlocks, map);
			getBlockArgumentMap(ifOp->getFalseBlockInvocation().getBlock(), visitedBlocks, map);
		} else if (op->getOperationType() == Operation::OperationType::BranchOp) {
			auto branchOp = as<BranchOperation>(op);
			addToBlockArgumentMap(&branchOp->getNextBlockInvocation(), map);
			getBlockArgumentMap(branchOp->getNextBlockInvocation().getBlock(), visitedBlocks, map);
		}
	}
	void write_edges(const std::shared_ptr<IRGraph>& graph, bool hideIntermediateBlockArguments, bool writeBlocksOnly) {

		std::map<Operation*, std::vector<Operation*>> operatorDataflowMapping;
		auto& rootBlock = graph->getRootOperation().getBasicBlocks()[0];
		std::set<const BasicBlock*> vistedBlocks;
		getBlockArgumentMap(rootBlock.get(), vistedBlocks, operatorDataflowMapping);
		// crate dataflow edges
		for (const auto& block : graph->getRootOperation().getBasicBlocks()) {
			if (writeBlocksOnly) {
				auto& op = block->getOperations().back();
				if (op->getOperationType() == Operation::OperationType::IfOp) {
					auto ifOp = as<IfOperation>(op);
					auto trueBlock = ifOp->getTrueBlockInvocation().getBlock()->getIdentifier();
					write_edge(block->getIdentifier(), trueBlock, "control");
					auto falseBlock = ifOp->getFalseBlockInvocation().getBlock()->getIdentifier();
					write_edge(block->getIdentifier(), falseBlock, "control");
				} else if (op->getOperationType() == Operation::OperationType::BranchOp) {
					auto branchOp = as<BranchOperation>(op);
					auto falseBlock = branchOp->getNextBlockInvocation().getBlock()->getIdentifier();
					write_edge(block->getIdentifier(), falseBlock, "control");
				}
			} else {
				for (const auto& op : block->getOperations()) {
					const auto& to = nodeIdMap[op.get()];
					// process inputs of node
					for (auto input : op->getInputs()) {
						const auto& from = nodeIdMap[input];
						write_edge(from, to, "data", input->getIdentifier().toString());
					}

					// handle control-flow arguments
					if (op->getOperationType() == Operation::OperationType::IfOp) {
						[[maybe_unused]] auto ifOp = as<IfOperation>(op);
						write_block_argument_edges(graph, ifOp->getTrueBlockInvocation(),
						                           hideIntermediateBlockArguments);
						write_block_argument_edges(graph, ifOp->getFalseBlockInvocation(),
						                           hideIntermediateBlockArguments);
					} else if (op->getOperationType() == Operation::OperationType::BranchOp) {
						[[maybe_unused]] auto branchOp = as<BranchOperation>(op);
						write_block_argument_edges(graph, branchOp->getNextBlockInvocation(),
						                           hideIntermediateBlockArguments);
					}
				}
			}
		}

		if (!writeBlocksOnly) {

			// create control-flow edges
			for (const auto& blocks : graph->getRootOperation().getBasicBlocks()) {
				std::string from = "start_" + blocks->getIdentifier();
				for (const auto& op : blocks->getOperations()) {
					if (isControlFlowOp(op.get())) {
						const auto& to = nodeIdMap[op.get()];
						write_edge(from, to, "control");
						from = to;
					}

					if (op->getOperationType() == Operation::OperationType::IfOp) {
						auto ifOp = as<IfOperation>(op);
						std::string toTrue = "start_" + ifOp->getTrueBlockInvocation().getBlock()->getIdentifier();
						write_edge(from, toTrue, "control", "true");
						std::string toFalse = "start_" + ifOp->getFalseBlockInvocation().getBlock()->getIdentifier();
						write_edge(from, toFalse, "control", "false");
					} else if (op->getOperationType() == Operation::OperationType::BranchOp) {
						auto branchOp = as<BranchOperation>(op);
						std::string to = "start_" + branchOp->getNextBlockInvocation().getBlock()->getIdentifier();
						write_edge(from, to, "control");
					}
				}
			}
		}
	}

	bool isControlFlowOp(Operation* op) {
		switch (op->getOperationType()) {
		case Operation::OperationType::BasicBlockArgument:
		case Operation::OperationType::ConstIntOp:
		case Operation::OperationType::ConstBooleanOp:
		case Operation::OperationType::ConstPtrOp:
		case Operation::OperationType::ConstFloatOp:
			return false;
		default:
			return true;
		}
	}

	void write_edge(const std::string& from, const std::string& to, const std::string type,
	                const std::string& label = "") {
		std::map<std::string, std::string> attrs;
		attrs["label"] = label; // Example label
		attrs["fontname"] = "arial";
		attrs["color"] = EDGE_COLORS.at(type);
		attrs["fontcolor"] = EDGE_COLORS.at(type);

		// Properties depending on the kind of edge.
		if (type == "control") {
			attrs["penwidth"] = "2";
		}
		stream << "  node" << from << " -> node" << to << " " << write_attrs(attrs) << ";" << std::endl;
	}
	void write_graph(const std::shared_ptr<IRGraph>& graph, bool hidpi = false, bool draw_blocks = true,
	                 bool drawBlocksOnly = true) {
		auto attrs = std::map<std::string, std::string>();
		attrs["bgcolor"] = "white";
		if (hidpi) {
			attrs["dpi"] = "200";
		}
		start_graph(attrs);
		write_nodes(graph, draw_blocks, drawBlocksOnly);
		write_edges(graph, true, drawBlocksOnly);
		end_graph();
	}
	void write_node(const std::string& indent, const std::string& label, const std::string& id,
	                const std::string type) {
		std::map<std::string, std::string> attrs;
		attrs["label"] = label;
		attrs["shape"] = "rectangle"; // Example shape
		attrs["fontname"] = "Arial";
		attrs["style"] = "filled";
		attrs["color"] = "black";

		// # Color depends on the kind of node.
		auto& color = NODE_COLORS.at(type);
		attrs["fillcolor"] = color.first;
		attrs["fontcolor"] = color.second;

		stream << indent << "node" << id << " " << write_attrs(attrs) << ";" << std::endl;
	}

	void write_node_for_op(const std::string& indent, const std::string& block, Operation* node) {
		std::string id = block + "_" + std::to_string(nodeIdMap.size());
		nodeIdMap[node] = id;

		write_node(indent, getNodeLabelForOp(node), id, getNodeTypeForOp(node->getOperationType()));
	}
	void end_subgraph() {
		stream << "  }" << std::endl;
	}

	void start_subgraph(const std::string& id) {
		stream << "  subgraph cluster_" << id << " {" << std::endl;
		stream << "    label=\"Block_" << id << " \";" << std::endl;
		stream << "    style=filled;" << std::endl;
		stream << "    color=\"" << DARK_YELLOW << "\";" << std::endl;
		stream << "    fillcolor=\"" << LIGHT_YELLOW << "\";" << std::endl;
	}

	void write_nodes_for_block(const std::unique_ptr<BasicBlock>& block) {
		write_node("  ", "Start", "start_" + block->getIdentifier(), "control");
		for (const auto& op : block->getArguments()) {
			std::string indent = "  ";
			write_node_for_op(indent, block->getIdentifier(), op.get());
		}

		for (const auto& op : block->getOperations()) {
			std::string indent = "  ";
			write_node_for_op(indent, block->getIdentifier(), op.get());
		}
	}
	void write_nodes(const std::shared_ptr<IRGraph>& graph, bool draw_blocks, bool drawBlocksOnly) {

		if (draw_blocks) {
			// Assuming you have a way to divide nodes into blocks
			for (const auto& block : graph->getRootOperation().getBasicBlocks()) {
				if (!drawBlocksOnly) {
					start_subgraph(block->getIdentifier());
					write_nodes_for_block(block);
					end_subgraph();
				} else {
					write_node("  ", "Block" + block->getIdentifier(), block->getIdentifier(), "control");
				}
			}
		} else {
			for (const auto& block : graph->getRootOperation().getBasicBlocks()) {
				write_nodes_for_block(block);
			}
		}
	}

	std::string getNodeLabelForOp(Operation* op) {
		switch (op->getOperationType()) {
		case Operation::OperationType::AddOp:
			return "+";
		case Operation::OperationType::AndOp:
			return "&&";
		case Operation::OperationType::NotOp:
			return "!";
		case Operation::OperationType::BasicBlockArgument:
			return "Arg(" + op->getIdentifier().toString() + ")";
		case Operation::OperationType::BranchOp:
			return "End";
		case Operation::OperationType::ConstIntOp:
			return std::to_string(static_cast<ConstIntOperation*>(op)->getValue());
		case Operation::OperationType::ConstBooleanOp:
			return std::to_string(static_cast<ConstBooleanOperation*>(op)->getValue());
		case Operation::OperationType::ConstPtrOp:
			return "#";
		case Operation::OperationType::ConstFloatOp:
			return std::to_string(static_cast<ConstFloatOperation*>(op)->getValue());
		case Operation::OperationType::DivOp:
			return "/";
		case Operation::OperationType::ModOp:
			return "%";
		case Operation::OperationType::IfOp:
			return "If";
		case Operation::OperationType::LoadOp:
			return "Load";
		case Operation::OperationType::MulOp:
			return "*";
		case Operation::OperationType::NegateOp:
			return "neg";
		case Operation::OperationType::OrOp:
			return "||";
		case Operation::OperationType::ProxyCallOp:
			return static_cast<ProxyCallOperation*>(op)->getFunctionName();
		case Operation::OperationType::StoreOp:
			return "store";
		case Operation::OperationType::SubOp:
			return "-";
		default:
			break;
		}
		return fmt::to_string(*op);
	}
	std::string getNodeTypeForOp(Operation::OperationType type) {
		switch (type) {
		case Operation::OperationType::BasicBlockArgument:
			return "input";
		case Operation::OperationType::ConstIntOp:
		case Operation::OperationType::ConstBooleanOp:
		case Operation::OperationType::ConstPtrOp:
		case Operation::OperationType::ConstFloatOp:
		case Operation::OperationType::CastOp:
			return "virtual";
		case Operation::OperationType::CompareOp:
		case Operation::OperationType::AddOp:
		case Operation::OperationType::AndOp:
		case Operation::OperationType::NotOp:
		case Operation::OperationType::OrOp:
		case Operation::OperationType::NegateOp:
		case Operation::OperationType::ShiftOp:
		case Operation::OperationType::SubOp:
		case Operation::OperationType::MulOp:
		case Operation::OperationType::ModOp:
		case Operation::OperationType::DivOp:
		case Operation::OperationType::BinaryComp:
			return "calc";
		case Operation::OperationType::FunctionOp:
		case Operation::OperationType::ProxyCallOp:
			return "call";
		case Operation::OperationType::IfOp:
		case Operation::OperationType::LoopOp:
		case Operation::OperationType::ReturnOp:
		case Operation::OperationType::BlockInvocation:
		case Operation::OperationType::BranchOp:
			return "control";
		case Operation::OperationType::LoadOp:
		case Operation::OperationType::StoreOp:
			return "memory";
		case Operation::OperationType::MLIR_YIELD:
			return "other";
		}
		return "other";
	}

	std::ostream& stream;
	std::map<const Operation*, std::string> nodeIdMap;

};

std::string createGraphVizFromIr(const std::shared_ptr<IRGraph>& graph) {
	std::stringstream ss;
	auto writer = GraphvizWriter(ss);
	writer.write_graph(graph, false, true);
	auto content = ss.str();
	std::cout << content << std::endl;
	return "https://dreampuf.github.io/GraphvizOnline/#" + urlEncode(content);
}
} // namespace nautilus::compiler::ir

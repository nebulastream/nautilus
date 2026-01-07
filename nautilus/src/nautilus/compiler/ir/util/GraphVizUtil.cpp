
#include "GraphVizUtil.hpp"
#include <sstream>
#ifdef ENABLE_LOGGING
#include "fmt/core.h"
#include "nautilus/compiler/ir/blocks/BasicBlock.hpp"
#include "nautilus/compiler/ir/operations/BranchOperation.hpp"
#include "nautilus/compiler/ir/operations/ConstBooleanOperation.hpp"
#include "nautilus/compiler/ir/operations/ConstFloatOperation.hpp"
#include "nautilus/compiler/ir/operations/ConstIntOperation.hpp"
#include "nautilus/compiler/ir/operations/FunctionOperation.hpp"
#include "nautilus/compiler/ir/operations/IfOperation.hpp"
#include "nautilus/compiler/ir/operations/ProxyCallOperation.hpp"
#include "nautilus/logging.hpp"
#include <iomanip>
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

const std::string WHITE_ICE = "#d7ede7";
const std::string KEPPEL = "#3cb4a4";
const std::string CARISSMA = "#e98693";
const std::string AMARANTH = "#da2d4f";
const std::string BLACK = "#1a1919";
// const std::string WHITE = "#ffffff";
const std::string DUST = "#f9f9f9";
const std::string BIG_STONE = "#343d46";
const std::string ORANGE = "#ffa500";
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

std::string writeAttrs(const std::map<std::string, std::string>& attrs) {
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

	void writeGraph(const std::shared_ptr<IRGraph>& graph, bool hidpi = false, bool draw_blocks = true,
	                bool drawBlocksOnly = false) {
		auto attrs = std::map<std::string, std::string>();
		attrs["bgcolor"] = "white";
		if (hidpi) {
			attrs["dpi"] = "200";
		}
		startGraph(attrs);
		writeNodes(graph, draw_blocks, drawBlocksOnly);
		writeEdges(graph, false, drawBlocksOnly);
		endGraph();
	}

protected:
	virtual void endGraph() {
		stream << "}" << std::endl;
	}
	virtual void startGraph(const std::map<std::string, std::string>& attrs) {
		stream << "digraph G {" << std::endl;
		stream << "  graph " << writeAttrs(attrs) << ";" << std::endl;
	}

	void writeBlockArgumentEdges(const std::shared_ptr<IRGraph>&, const BasicBlockInvocation& bi,
	                             [[maybe_unused]] bool hideIntermediateBlockArguments, const std::string& label) {
		//  crate an edge from the src block op to the argument in the target block.
		for (size_t i = 0; i < bi.getArguments().size(); i++) {
			const auto& from = nodeIdMap[bi.getArguments()[i]];
			auto to = nodeIdMap[&bi];
			auto node = to + "_" + std::to_string(i);
			writeEdge(from, node, "data", label);
		}
	}

	std::vector<std::tuple<BasicBlock*, BasicBlockInvocation*>>
	getPredecessorBlocks(const std::shared_ptr<IRGraph>& graph, const BasicBlock* targetBlock) {
		std::vector<std::tuple<BasicBlock*, BasicBlockInvocation*>> predeccessor;
		for (const auto& block : graph->getFunctionOperations()[0]->getBasicBlocks()) {
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
	void writeEdges(const std::shared_ptr<IRGraph>& graph, bool hideIntermediateBlockArguments, bool writeBlocksOnly) {
		std::map<Operation*, std::vector<Operation*>> operatorDataflowMapping;
		auto& rootBlock = graph->getFunctionOperations()[0]->getBasicBlocks()[0];
		std::set<const BasicBlock*> vistedBlocks;
		getBlockArgumentMap(rootBlock.get(), vistedBlocks, operatorDataflowMapping);
		// crate dataflow edges
		for (const auto& block : graph->getFunctionOperations()[0]->getBasicBlocks()) {
			if (writeBlocksOnly) {
				auto& op = block->getOperations().back();
				if (op->getOperationType() == Operation::OperationType::IfOp) {
					auto ifOp = as<IfOperation>(op);
					auto trueBlock = ifOp->getTrueBlockInvocation().getBlock()->getIdentifier();
					writeEdge(block->getIdentifier(), trueBlock, "control", "true");
					auto falseBlock = ifOp->getFalseBlockInvocation().getBlock()->getIdentifier();
					writeEdge(block->getIdentifier(), falseBlock, "control", "false");
				} else if (op->getOperationType() == Operation::OperationType::BranchOp) {
					auto branchOp = as<BranchOperation>(op);
					auto falseBlock = branchOp->getNextBlockInvocation().getBlock()->getIdentifier();
					writeEdge(block->getIdentifier(), falseBlock, "control", "");
				}
			} else {
				for (const auto& op : block->getOperations()) {
					const auto& to = nodeIdMap[op.get()];
					// process inputs of node
					for (auto input : op->getInputs()) {
						const auto& from = nodeIdMap[input];
						writeEdge(from, to, "data", input->getIdentifier().toString());
					}

					// handle control-flow arguments
					if (op->getOperationType() == Operation::OperationType::IfOp) {
						auto ifOp = as<IfOperation>(op);
						writeBlockArgumentEdges(graph, ifOp->getTrueBlockInvocation(), hideIntermediateBlockArguments,
						                        "false");
						writeBlockArgumentEdges(graph, ifOp->getFalseBlockInvocation(), hideIntermediateBlockArguments,
						                        "true");
					} else if (op->getOperationType() == Operation::OperationType::BranchOp) {
						auto branchOp = as<BranchOperation>(op);
						writeBlockArgumentEdges(graph, branchOp->getNextBlockInvocation(),
						                        hideIntermediateBlockArguments, "");
					}
				}
			}
		}

		if (!writeBlocksOnly) {

			// create control-flow edges
			for (const auto& blocks : graph->getFunctionOperations()[0]->getBasicBlocks()) {
				std::string from = "start_" + blocks->getIdentifier();
				for (const auto& op : blocks->getOperations()) {
					if (isControlFlowOp(op.get())) {
						const auto& to = nodeIdMap[op.get()];
						writeEdge(from, to, "control", "");
						from = to;
					}

					if (op->getOperationType() == Operation::OperationType::IfOp) {
						auto ifOp = as<IfOperation>(op);
						std::string toTrue = "start_" + ifOp->getTrueBlockInvocation().getBlock()->getIdentifier();
						writeEdge(from, toTrue, "control", "true");
						std::string toFalse = "start_" + ifOp->getFalseBlockInvocation().getBlock()->getIdentifier();
						writeEdge(from, toFalse, "control", "false");
					} else if (op->getOperationType() == Operation::OperationType::BranchOp) {
						auto branchOp = as<BranchOperation>(op);
						std::string to = "start_" + branchOp->getNextBlockInvocation().getBlock()->getIdentifier();
						writeEdge(from, to, "control", "");
					}
				}
			}
		}
	}

	static bool isControlFlowOp(Operation* op) {
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

	virtual void writeEdge(const std::string& from, const std::string& to, const std::string type,
	                       const std::string& label) {
		std::map<std::string, std::string> attrs;
		attrs["label"] = label; // Example label
		attrs["fontname"] = "arial";
		attrs["color"] = EDGE_COLORS.at(type);
		attrs["fontcolor"] = EDGE_COLORS.at(type);

		// Properties depending on the kind of edge.
		if (type == "control") {
			attrs["penwidth"] = "2";
		}
		stream << "  node" << from << " -> node" << to << " " << writeAttrs(attrs) << ";" << std::endl;
	}

	virtual void writeNode(const std::string& indent, const std::string& label, const std::string& id,
	                       const std::string& type) {
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

		stream << indent << "node" << id << " " << writeAttrs(attrs) << ";" << std::endl;
	}

	void writeNodeForOp(const std::string& indent, const std::string& block, Operation* node) {
		std::string id = block + "_" + std::to_string(nodeIdMap.size());
		nodeIdMap[node] = id;
		writeNode(indent, getNodeLabelForOp(node), id, getNodeTypeForOp(node->getOperationType()));
	}

	void writeNodeForOutputOp(const std::string& indent, const std::string& block, BasicBlockInvocation* bi) {
		std::string id = block + "_o" + std::to_string(nodeIdMap.size());
		nodeIdMap[bi] = id;
		for (size_t i = 0; i < bi->getArguments().size(); i++) {
			auto& targetArg = bi->getBlock()->getArguments()[i];
			auto lable = "Output(" + targetArg->getIdentifier().toString() + ")";
			writeNode(indent, lable, id + "_" + std::to_string(i), "input");
		}
	}
	virtual void endSubgraph() {
		stream << "  }" << std::endl;
	}

	virtual void startSubgraph(const std::string& id) {
		stream << "  subgraph cluster_" << id << " {" << std::endl;
		stream << "    label=\"Block_" << id << " \";" << std::endl;
		stream << "    style=filled;" << std::endl;
		stream << "    color=\"" << DARK_YELLOW << "\";" << std::endl;
		stream << "    fillcolor=\"" << LIGHT_YELLOW << "\";" << std::endl;
	}

	void writeNodesForBlock(const std::unique_ptr<BasicBlock>& block) {
		writeNode("  ", "Start", "start_" + block->getIdentifier(), "control");
		for (const auto& op : block->getArguments()) {
			std::string indent = "  ";
			writeNodeForOp(indent, block->getIdentifier(), op.get());
		}

		for (const auto& op : block->getOperations()) {
			std::string indent = "  ";
			writeNodeForOp(indent, block->getIdentifier(), op.get());

			if (op->getOperationType() == Operation::OperationType::IfOp) {
				auto ifOp = as<IfOperation>(op);
				writeNodeForOutputOp(indent, block->getIdentifier(), &ifOp->getTrueBlockInvocation());
				writeNodeForOutputOp(indent, block->getIdentifier(), &ifOp->getFalseBlockInvocation());
			} else if (op->getOperationType() == Operation::OperationType::BranchOp) {
				auto branchOp = as<BranchOperation>(op);
				writeNodeForOutputOp(indent, block->getIdentifier(), &branchOp->getNextBlockInvocation());
			}
		}
	}
	void writeNodes(const std::shared_ptr<IRGraph>& graph, bool draw_blocks, bool drawBlocksOnly) {

		if (draw_blocks) {
			// Assuming you have a way to divide nodes into blocks
			for (const auto& block : graph->getFunctionOperations()[0]->getBasicBlocks()) {
				if (!drawBlocksOnly) {
					startSubgraph(block->getIdentifier());
					writeNodesForBlock(block);
					endSubgraph();
				} else {
					writeNode("  ", "Block" + block->getIdentifier(), block->getIdentifier(), "control");
				}
			}
		} else {
			for (const auto& block : graph->getFunctionOperations()[0]->getBasicBlocks()) {
				writeNodesForBlock(block);
			}
		}
	}

	virtual std::string getNodeLabelForOp(Operation* op) {
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
		case Operation::OperationType::SelectOp:
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

class MermaidWriter : public GraphvizWriter {
public:
	MermaidWriter(std::ostream& stream) : GraphvizWriter(stream) {
	}

protected:
	int counter = 0;
	void startGraph(const std::map<std::string, std::string>&) override {
		// Ignore bgcolor, as I can't figure out how to do it in Mermaid
		stream << "flowchart TD" << std::endl;
	}

	void endGraph() override {
		// do nothing
	}

	void startSubgraph(const std::string& id) override {
		stream << "  subgraph cluster_" << id << "[Block " << id << "]" << std::endl;
	}

	void writeNode(const std::string& indent, const std::string& label, const std::string& id,
	               const std::string& type) override {
		auto localLabel = label;
		std::replace(localLabel.begin(), localLabel.end(), '(', ' ');
		std::replace(localLabel.begin(), localLabel.end(), ')', ' ');
		stream << indent << " node" << id << "[" << localLabel << "]" << std::endl;
		auto& color = NODE_COLORS.at(type);
		stream << indent << "style node" << id << " fill:" << color.first << ",color:" << color.second << std::endl;
	}

	void writeEdge(const std::string& from, const std::string& to, const std::string type,
	               const std::string& label = "") override {
		if (!label.empty()) {
			stream << "  node" << from << "--" << label << " --> node" << to << std::endl;
		} else {
			stream << "  node" << from << " --> node" << to << std::endl;
		}

		std::map<std::string, std::string> attrs;
		attrs["label"] = label;
		attrs["fontname"] = "arial";
		attrs["color"] = EDGE_COLORS.at(type);
		attrs["fontcolor"] = EDGE_COLORS.at(type);

		// Properties depending on the kind of edge.
		if (type == "control") {
			attrs["penwidth"] = "2";
		} else {
			attrs["penwidth"] = "1";
		}

		stream << "  linkStyle " << counter++ << " stroke:" << attrs["color"] << ",stroke-width:" << attrs["penwidth"]
		       << "px" << std::endl;
	}

	void endSubgraph() override {
		stream << "  end" << std::endl;
	}

	std::string getNodeLabelForOp(Operation* op) override {
		switch (op->getOperationType()) {
		case Operation::OperationType::AddOp:
			return "#plus;";
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
};

void createGraphVizFromIr(const std::shared_ptr<IRGraph>& graph, const engine::Options& options,
                          const DumpHandler& dumpHandler) {

	auto type = options.getOptionOrDefault("dump.graph.type", std::string("graphviz"));
	auto fullGraph = options.getOptionOrDefault("dump.graph.full", true);
	std::stringstream ss;
	if (type == "mermaid") {
		auto writer = MermaidWriter(ss);
		writer.writeGraph(graph, true, true, !fullGraph);
		auto content = ss.str();
		dumpHandler.forceDump("mermaid", "mermaid", content);
	} else if (type == "graphviz") {
		auto writer = GraphvizWriter(ss);
		writer.writeGraph(graph, true, true, !fullGraph);
		auto content = ss.str();
		log::info("https://dreampuf.github.io/GraphvizOnline/#{}", urlEncode(content));
		dumpHandler.forceDump("graphviz", "dot", content);
	}
}
} // namespace nautilus::compiler::ir
#else

namespace nautilus::compiler::ir {
void createGraphVizFromIr(const std::shared_ptr<IRGraph>&, const engine::Options&, const DumpHandler&) {
	std::cerr << "Logging needs to be enabled to dump graphs!" << std::endl;
}
} // namespace nautilus::compiler::ir
#endif

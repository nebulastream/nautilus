#include "nautilus/compiler/ir/IRGraph.hpp"
#include "nautilus/compiler/ir/blocks/BasicBlock.hpp"
#include "nautilus/compiler/ir/blocks/BasicBlockArgument.hpp"
#include "nautilus/compiler/ir/operations/ArithmeticOperations/AddOperation.hpp"
#include "nautilus/compiler/ir/operations/ConstIntOperation.hpp"
#include "nautilus/compiler/ir/operations/FunctionOperation.hpp"
#include "nautilus/compiler/ir/operations/LogicalOperations/CompareOperation.hpp"
#include "nautilus/compiler/ir/operations/ReturnOperation.hpp"
#include "nautilus/compiler/ir/passes/ConstantFoldingAndCopyPropagationPass.hpp"
#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>
#include <memory>
#include <string>
#include <vector>

namespace nautilus::engine {

namespace {

using namespace compiler::ir; // NOLINT

/// Synthetic single-block functions, sized so that a pass that is quadratic
/// in the block size shows up as a large regression rather than noise.
enum class Shape {
	/// `acc = acc + c_i`: every add folds, each fold enabling the next.
	Chain,
	/// Independent `c_i < 7` compares: every compare folds, none chain.
	Compares,
	/// `acc = acc + arg`: nothing is foldable.
	NoOp,
};

std::shared_ptr<IRGraph> buildGraph(Shape shape, int n) {
	auto ir = std::make_shared<IRGraph>("ir-pass-benchmark");
	auto& arena = ir->getArena();
	uint32_t id = 1;
	std::vector<BasicBlockArgument*> args;
	std::vector<Type> argTypes;
	if (shape == Shape::NoOp) {
		args.push_back(arena.create<BasicBlockArgument>(OperationIdentifier {id++}, Type::i64));
		argTypes.push_back(Type::i64);
	}
	auto* entry = arena.create<BasicBlock>(arena, BlockIdentifier {0}, args);
	Operation* last = nullptr;
	switch (shape) {
	case Shape::Chain:
		last = entry->addOperation<ConstIntOperation>(OperationIdentifier {id++}, int64_t {0}, Type::i64);
		for (int i = 0; i < n; ++i) {
			auto* c = entry->addOperation<ConstIntOperation>(OperationIdentifier {id++}, int64_t {i}, Type::i64);
			last = entry->addOperation<AddOperation>(OperationIdentifier {id++}, last, c);
		}
		break;
	case Shape::Compares:
		for (int i = 0; i < n; ++i) {
			auto* l = entry->addOperation<ConstIntOperation>(OperationIdentifier {id++}, int64_t {i}, Type::i64);
			auto* r = entry->addOperation<ConstIntOperation>(OperationIdentifier {id++}, int64_t {7}, Type::i64);
			last = entry->addOperation<CompareOperation>(OperationIdentifier {id++}, l, r, CompareOperation::LT);
		}
		break;
	case Shape::NoOp:
		last = args[0];
		for (int i = 0; i < n; ++i) {
			last = entry->addOperation<AddOperation>(OperationIdentifier {id++}, last, args[0]);
		}
		break;
	}
	entry->addOperation<ReturnOperation>(last);
	ir->addFunctionOperation(arena.create<FunctionOperation>("execute", std::vector<BasicBlock*> {entry}, argTypes,
	                                                         std::vector<std::string> {},
	                                                         shape == Shape::Compares ? Type::b : Type::i64));
	return ir;
}

} // namespace

TEST_CASE("IR Pass Benchmark") {
	const std::pair<std::string, Shape> shapes[] = {
	    {"chain", Shape::Chain}, {"compares", Shape::Compares}, {"noop", Shape::NoOp}};
	for (const auto& [name, shape] : shapes) {
		Catch::Benchmark::Benchmark("pass_constantFolding_" + name + "10000")
		    .operator=([shape](Catch::Benchmark::Chronometer meter) {
			    // The pass rewrites its input, so every run gets a fresh graph,
			    // built outside the measured region.
			    std::vector<std::shared_ptr<IRGraph>> graphs;
			    graphs.reserve(meter.runs());
			    for (int i = 0; i < meter.runs(); ++i) {
				    graphs.push_back(buildGraph(shape, 10000));
			    }
			    meter.measure([&graphs](int i) {
				    ConstantFoldingAndCopyPropagationPass pass;
				    return pass.apply(*graphs[i]);
			    });
		    });
	}
}

} // namespace nautilus::engine

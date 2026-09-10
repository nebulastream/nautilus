#include "IRGraphFixtures.hpp"
#include "nautilus/compiler/ir/IRLocationMap.hpp"
#include "nautilus/compiler/ir/blocks/BasicBlock.hpp"
#include "nautilus/compiler/ir/operations/FunctionOperation.hpp"
#include "nautilus/compiler/ir/operations/OperationProperties.hpp"
#include <catch2/catch_all.hpp>
#include <string>
#include <vector>

namespace nautilus::testing {

namespace {

using namespace nautilus::compiler;

std::vector<std::string> splitLines(const std::string& text) {
	std::vector<std::string> lines;
	size_t start = 0;
	while (start <= text.size()) {
		const auto end = text.find('\n', start);
		if (end == std::string::npos) {
			lines.push_back(text.substr(start));
			break;
		}
		lines.push_back(text.substr(start, end - start));
		start = end + 1;
	}
	return lines;
}

/// The 1-based line @p lineNo of @p text. Empty for an out-of-range line, which
/// makes an off-by-one show up as a failed content check rather than a crash.
std::string lineAt(const std::string& text, uint32_t lineNo) {
	const auto lines = splitLines(text);
	if (lineNo == 0 || lineNo > lines.size()) {
		return {};
	}
	return lines[lineNo - 1];
}

std::vector<std::shared_ptr<ir::IRGraph>> allFixtures() {
	return {IRGraphFixtures::makeDiamondGraph(), IRGraphFixtures::makeSharedTargetIfGraph(),
	        IRGraphFixtures::makeNaturalLoopGraph(), IRGraphFixtures::makeDeadChainGraph()};
}

} // namespace

// The guarantee the whole design rests on: the map is a description of one
// specific rendering, so recording positions must not perturb what is rendered.
// Without this, every other property here could hold against text nobody sees.
TEST_CASE("IRLocationMap: text is identical to toString") {
	for (const auto& ir : allFixtures()) {
		const auto map = ir::computeIRLocations(*ir, ir::IRPrintOptions {});
		REQUIRE(map.text == ir->toString());
	}
}

TEST_CASE("IRLocationMap: every operation is recorded on a line that contains it") {
	for (const auto& ir : allFixtures()) {
		const auto map = ir::computeIRLocations(*ir, ir::IRPrintOptions {});
		for (const auto* function : ir->getFunctionOperations()) {
			for (const auto* block : function->getBasicBlocks()) {
				for (const auto* operation : block->getOperations()) {
					const uint32_t line = map.lineOf(operation);
					INFO("operation $" << operation->getIdentifier().getId() << " reported at line " << line);
					REQUIRE(line != 0);
					// A value-defining op spells its own `$N` on its line. This
					// is the check that a parser-based map could not make about
					// itself: it would be comparing the parse against itself.
					const std::string text = lineAt(map.text, line);
					const std::string dollar = "$" + std::to_string(operation->getIdentifier().getId());
					INFO("line text: " << text);
					REQUIRE_FALSE(text.empty());
					// The dump prints `$N = ` for an operation that defines a
					// value: non-void stamp, and not a terminator. A store and
					// a void call have a void stamp; `return ($N)` has the
					// returned value's stamp but defines nothing of its own.
					if (operation->getStamp() != Type::v && !ir::isTerminatorOp(operation->getOperationType())) {
						REQUIRE(text.find(dollar) != std::string::npos);
					}
				}
			}
		}
	}
}

TEST_CASE("IRLocationMap: blocks and functions land on their own header lines") {
	for (const auto& ir : allFixtures()) {
		const auto map = ir::computeIRLocations(*ir, ir::IRPrintOptions {});
		for (const auto* function : ir->getFunctionOperations()) {
			const uint32_t functionLine = map.lineOf(function);
			REQUIRE(functionLine != 0);
			INFO("function line: " << lineAt(map.text, functionLine));
			REQUIRE(lineAt(map.text, functionLine).find(function->getName()) != std::string::npos);

			for (const auto* block : function->getBasicBlocks()) {
				const uint32_t blockLine = map.lineOf(block);
				REQUIRE(blockLine != 0);
				const std::string expected = "Block_" + std::to_string(block->getIdentifier().getId()) + "(";
				INFO("block line: " << lineAt(map.text, blockLine));
				REQUIRE(lineAt(map.text, blockLine).find(expected) != std::string::npos);
			}
		}
	}
}

// The invariant the old positional `blockOpLines[i]` lockstep assumed but never
// checked. It is what made a multi-line operation silently misattribute every
// later line in its block.
TEST_CASE("IRLocationMap: operation lines increase in block order") {
	for (const auto& ir : allFixtures()) {
		const auto map = ir::computeIRLocations(*ir, ir::IRPrintOptions {});
		for (const auto* function : ir->getFunctionOperations()) {
			for (const auto* block : function->getBasicBlocks()) {
				uint32_t previous = map.lineOf(block);
				for (const auto* operation : block->getOperations()) {
					const uint32_t line = map.lineOf(operation);
					INFO("block " << block->getIdentifier().getId() << ", op $" << operation->getIdentifier().getId());
					REQUIRE(line > previous);
					previous = line;
				}
			}
		}
	}
}

// Fixtures open no region(), so the chain tables must be empty rather than
// populated with a default -- an operation with no region and one whose region
// failed to resolve must not look alike.
TEST_CASE("IRLocationMap: operations outside any region have an empty chain") {
	for (const auto& ir : allFixtures()) {
		const auto map = ir::computeIRLocations(*ir, ir::IRPrintOptions {});
		for (const auto* function : ir->getFunctionOperations()) {
			for (const auto* block : function->getBasicBlocks()) {
				for (const auto* operation : block->getOperations()) {
					REQUIRE(map.chainOf(operation).empty());
					REQUIRE(map.chainIndexOf(operation) == ir::IRLocationMap::NO_CHAIN);
				}
			}
		}
	}
}

TEST_CASE("IRLocationMap: an unknown node resolves to no line and no chain") {
	auto ir = IRGraphFixtures::makeDiamondGraph();
	const auto map = ir::computeIRLocations(*ir, ir::IRPrintOptions {});
	REQUIRE(map.lineOf(static_cast<const ir::Operation*>(nullptr)) == 0);
	REQUIRE(map.chainOf(static_cast<const ir::Operation*>(nullptr)).empty());
}

} // namespace nautilus::testing

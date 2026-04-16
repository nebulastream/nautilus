
#include "nautilus/compiler/ir/passes/IRStatistics.hpp"
#include "nautilus/compiler/ir/operations/FunctionOperation.hpp"
#include <fmt/format.h>

namespace nautilus::compiler::ir {

std::string IRStatistics::toString() const {
	return fmt::format("functions={} blocks={} operations={}", numFunctions, numBlocks, numOperations);
}

IRStatistics IRStatistics::operator-(const IRStatistics& rhs) const noexcept {
	IRStatistics out;
	out.numFunctions = numFunctions - rhs.numFunctions;
	out.numBlocks = numBlocks - rhs.numBlocks;
	out.numOperations = numOperations - rhs.numOperations;
	return out;
}

IRStatistics computeStatistics(const IRGraph& ir) {
	IRStatistics s;
	s.numFunctions = ir.getFunctionOperations().size();
	for (const auto* fn : ir.getFunctionOperations()) {
		s.numBlocks += fn->getBasicBlocks().size();
		for (const auto* block : fn->getBasicBlocks()) {
			s.numOperations += block->getOperations().size();
		}
	}
	return s;
}

} // namespace nautilus::compiler::ir


#include "nautilus/compiler/backends/mlir/debug/IRSourceMap.hpp"
#include "nautilus/compiler/ir/IRGraph.hpp"
#include "nautilus/compiler/ir/operations/FunctionOperation.hpp"
#include "nautilus/compiler/ir/util/IRSerializationUtil.hpp"
#include <cctype>
#include <cstdint>
#include <limits>
#include <string_view>

namespace nautilus::compiler::mlir {

namespace {

// Sentinel returned by parseDollarId when the view does not start with
// `$<digits>`.  Using a sentinel instead of std::optional sidesteps a
// GCC-12 false positive -Wmaybe-uninitialized when the optional is
// inlined into its caller.
static constexpr uint32_t kNoId = std::numeric_limits<uint32_t>::max();

// Parse a leading `$N` token and return N, or kNoId if the view does
// not start with `$<digits>`.
uint32_t parseDollarId(std::string_view s) {
	if (s.empty() || s[0] != '$') {
		return kNoId;
	}
	uint32_t id = 0;
	bool any = false;
	for (size_t i = 1; i < s.size(); ++i) {
		if (std::isdigit(static_cast<unsigned char>(s[i]))) {
			id = id * 10 + static_cast<uint32_t>(s[i] - '0');
			any = true;
		} else {
			break;
		}
	}
	return any ? id : kNoId;
}

std::string_view trimLeadingWhitespace(std::string_view s) {
	size_t i = 0;
	while (i < s.size() && (s[i] == ' ' || s[i] == '\t')) {
		++i;
	}
	return s.substr(i);
}

// Scan a `(...)` argument list for `$N:type` entries and record each
// discovered identifier as defined on `line` (unless a prior definition
// has already been recorded, which takes precedence).
void recordBlockArgs(std::string_view args, uint32_t line, std::unordered_map<uint32_t, uint32_t>& operationLines) {
	size_t pos = 0;
	while (pos < args.size()) {
		auto dollar = args.find('$', pos);
		if (dollar == std::string_view::npos) {
			break;
		}
		const uint32_t id = parseDollarId(args.substr(dollar));
		if (id != kNoId) {
			operationLines.try_emplace(id, line);
		}
		pos = dollar + 1;
	}
}

} // namespace

IRSourceMap dumpIRWithSourceMap(const ir::IRGraph& graph) {
	IRSourceMap result;
	result.text = graph.toString();

	std::string_view remaining = result.text;
	uint32_t lineNo = 0;

	// Nautilus IR restarts `$N` ids and `Block_N` indices at 0 for
	// every function.  Track which function each line belongs to so
	// we can partition the tables; a `name(...) {` line starts a new
	// function scope and a top-level `}` ends it.
	IRSourceMap::FunctionLines* currentFunction = nullptr;
	// The block whose body we are currently scanning — used to append
	// body-op lines (including terminators) to that block's sequence
	// so the lowering provider can map ops to lines by position.
	uint32_t currentBlockId = 0;
	bool insideBlock = false;

	while (!remaining.empty()) {
		++lineNo;
		auto nl = remaining.find('\n');
		std::string_view line = (nl == std::string_view::npos) ? remaining : remaining.substr(0, nl);

		std::string_view trimmed = trimLeadingWhitespace(line);
		// A function scope ends at a `}` that is the first character
		// on its line (top-level close brace; nested block-body
		// terminators are indented).
		if (!line.empty() && line[0] == '}') {
			currentFunction = nullptr;
			insideBlock = false;
		}
		// A blank line separates two blocks in the dump — close out
		// the current block so terminator lookups use the right id.
		if (trimmed.empty()) {
			insideBlock = false;
		}

		if (trimmed.empty()) {
			// fall through to advance
		} else if (trimmed.starts_with("Block_")) {
			if (currentFunction == nullptr) {
				// Stray block header outside a function — should not
				// happen for well-formed IR.  Skip to stay robust.
			} else {
				// Format: `Block_K($1:i32, $2:i32):`
				auto open = trimmed.find('(');
				auto close = (open == std::string_view::npos) ? std::string_view::npos : trimmed.find(')', open);
				if (open != std::string_view::npos && close != std::string_view::npos) {
					recordBlockArgs(trimmed.substr(open + 1, close - open - 1), lineNo,
					                currentFunction->operationLines);
				}
				// Parse the numeric suffix after `Block_` so the
				// lowering provider can map block-arg store !dbg to
				// the block header line.
				constexpr std::string_view prefix = "Block_";
				auto suffix = trimmed.substr(prefix.size());
				uint32_t blockId = 0;
				bool any = false;
				for (char c : suffix) {
					if (std::isdigit(static_cast<unsigned char>(c))) {
						blockId = blockId * 10 + static_cast<uint32_t>(c - '0');
						any = true;
					} else {
						break;
					}
				}
				if (any) {
					currentFunction->blockHeaderLines[blockId] = lineNo;
					currentBlockId = blockId;
					insideBlock = true;
				}
			}
		} else if (trimmed[0] == '$') {
			// Format: `$N = ...` — an operation definition.
			if (currentFunction != nullptr) {
				const uint32_t id = parseDollarId(trimmed);
				if (id != kNoId) {
					currentFunction->operationLines.try_emplace(id, lineNo);
					if (insideBlock) {
						currentFunction->blockOpLines[currentBlockId].push_back(lineNo);
					}
				}
			}
		} else if (std::isalpha(static_cast<unsigned char>(trimmed[0])) || trimmed[0] == '_') {
			// Format: `name(args) {` for function headers.  We
			// recognise this by the presence of `(` before the next
			// whitespace and an eventual `{` on the same line.
			auto paren = trimmed.find('(');
			auto brace = trimmed.find('{');
			if (paren != std::string_view::npos && brace != std::string_view::npos && brace > paren) {
				std::string name(trimmed.substr(0, paren));
				result.functionLines[name] = lineNo;
				currentFunction = &result.functions[name];
				insideBlock = false;
			} else if (insideBlock && currentFunction != nullptr) {
				// Indented line inside a block body that does not
				// start with `$` — a terminator like `if ... ? ... :
				// ...`, `br Block_K(...)`, or `return (...)`.  Record
				// its line so the lowering provider can give the
				// corresponding MLIR op a !dbg that lets GDB stop on
				// the terminator line.
				currentFunction->blockOpLines[currentBlockId].push_back(lineNo);
			}
		}

		if (nl == std::string_view::npos) {
			break;
		}
		remaining = remaining.substr(nl + 1);
	}

	// Fill each function's printed-id translation table: the dump numbers
	// values uniquely per function, while operations keep their (reusable)
	// stored identifiers in memory.
	for (const auto* function : graph.getFunctionOperations()) {
		if (auto it = result.functions.find(function->getName()); it != result.functions.end()) {
			it->second.printedIds = ir::computePrintedValueIds(*function);
		}
	}

	return result;
}

} // namespace nautilus::compiler::mlir

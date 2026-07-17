#pragma once

#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

namespace nautilus::compiler::ir {
class IRGraph;
class Operation;
} // namespace nautilus::compiler::ir

namespace nautilus::compiler::mlir {

// A text dump of a Nautilus IRGraph paired with lookup tables that map
// each SSA operation identifier ($N) and basic-block id to the 1-based
// line number where it appears in the dump.  Produced by
// dumpIRWithSourceMap() and consumed by MLIRLoweringProvider to tag
// emitted MLIR ops with FileLineColLocs pointing into `text`.
//
// Nautilus IR restarts `$N` identifiers and `Block_N` indices at 0 for
// every function, so the line tables must be scoped per-function —
// looking `$1` up without saying which function it belongs to would
// collide across callers and callees in the same module.
struct IRSourceMap {
	// Per-function line tables.  Populated once per Nautilus
	// FunctionOperation by dumpIRWithSourceMap().
	struct FunctionLines {
		// Maps OperationIdentifier id to the 1-based line in `text`
		// where the operation is defined in this function.  For
		// operations that produce a result this is the `$N = ...`
		// line; for block arguments it is the enclosing block
		// header.  First occurrence wins so function parameters keep
		// their Block_0 header line even when a later block re-uses
		// the same id as a block arg.
		std::unordered_map<uint32_t, uint32_t> operationLines;

		// Maps BasicBlock id (the integer after `Block_`) to its
		// header line.  Used for the !dbg on block-arg stores so
		// that entering a block advances GDB to the block's header
		// rather than back to where the same $N was first
		// introduced.
		std::unordered_map<uint32_t, uint32_t> blockHeaderLines;

		// For each BasicBlock id, the 1-based dump line of each op
		// in that block, in the order IRGraph::toString() prints
		// them.  The lowering provider consults this by position so
		// terminators (`br`, `if`, `return`) — which have no `$N`
		// identifier in the dump — still get a non-zero !dbg and
		// stay steppable in GDB.
		std::unordered_map<uint32_t, std::vector<uint32_t>> blockOpLines;

		// Maps each value-producing operation and block argument to the
		// `$N` id the dump printed for it. The writer numbers values
		// uniquely per function (unlike the stored identifiers, which the
		// tracer reuses across blocks), so the lowering provider must
		// translate through this map before consulting `operationLines`
		// or naming debug variables.
		std::unordered_map<const ir::Operation*, uint32_t> printedIds;
	};

	// The full text of the IR dump (identical to IRGraph::toString()).
	std::string text;

	// Maps function name to the 1-based line of its header (`name(...) {`).
	std::unordered_map<std::string, uint32_t> functionLines;

	// Per-function line tables indexed by function name.
	std::unordered_map<std::string, FunctionLines> functions;
};

// Dumps `graph` via IRGraph::toString() and parses the result to build
// the SSA-id to line-number map, keyed by the printed `$N` ids (see
// `computePrintedValueIds` in IRSerializationUtil.hpp, which fills each
// function's `printedIds` translation table).  Relies on the stable format
// emitted by the IR writer (`\t$N = ...` for definitions,
// `Block_K($N:type, ...):` for block arguments, `name(args) ... {` for
// function headers).  If the dump format is ever changed, update the
// parser here in lockstep.
IRSourceMap dumpIRWithSourceMap(const ir::IRGraph& graph);

} // namespace nautilus::compiler::mlir

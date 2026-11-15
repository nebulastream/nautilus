#pragma once

#include "nautilus/passes/MutableFunction.hpp"
#include <cstdint>
#include <vector>

namespace nautilus::passes {

/// The control-flow graph of a function over its normal edges (`br`, `condbr`), by block index in layout order. Landing
/// pads are not nodes: they are entered only by unwind entries and end the flow, so they change no dominance among the
/// other blocks.
struct Cfg {
	explicit Cfg(const MutableFunction& fn);

	uint32_t size() const {
		return static_cast<uint32_t>(succs.size());
	}
	/// `a` dominates `b` (both reachable). A block dominates itself.
	bool dominates(uint32_t a, uint32_t b) const;
	bool reachable(uint32_t b) const {
		return rpoNumber[b] != nir::NONE;
	}

	std::vector<std::vector<uint32_t>> succs, preds;
	std::vector<uint32_t> rpo;       ///< reverse post-order of the reachable blocks
	std::vector<uint32_t> rpoNumber; ///< position in `rpo`, or NONE
	std::vector<uint32_t> idom;      ///< immediate dominator; the entry's is itself
};

/// A natural loop: the blocks that reach a latch without passing the header, plus the header.
struct Loop {
	uint32_t header = nir::NONE;
	std::vector<uint32_t> latches;
	std::vector<uint32_t> body;         ///< block indices, ascending (layout order), header included
	std::vector<char> inBody;           ///< per block index
	std::vector<uint32_t> outsidePreds; ///< predecessors of the header outside the loop
};

/// The natural loops of the function, innermost first (a loop is listed before every loop that contains it). Loops
/// that share a header are one loop.
std::vector<Loop> findLoops(const Cfg& cfg);

/// The preheader of `loop` for hoisting: the one outside predecessor of the header if it ends in an unconditional `br`,
/// else a new block that every outside edge enters and that jumps to the header, inserted before the header in layout
/// order. Returns the uid of the block; it ends in a `br` to the header whose arguments are `header arguments`
/// available in the preheader. Returns `NONE` (and changes nothing) if there is no outside edge, an outside
/// predecessor lies after the header in layout order, or the header is the entry block.
/// Invalidates every `Cfg` and `Loop` of the function.
uint32_t ensurePreheader(MutableFunction& fn, const Cfg& cfg, const Loop& loop);

} // namespace nautilus::passes

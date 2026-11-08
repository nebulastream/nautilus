#include "nautilus/conversion/Conversion.hpp"
#include "nautilus/conversion/AttributeInference.hpp"
#include "nautilus/nir/Builder.hpp"
#include "nautilus/nir/Operands.hpp"
#include "nautilus/nir/Semantics.hpp"
#include <algorithm>
#include <array>
#include <cassert>
#include <map>
#include <stdexcept>
#include <string>
#include <vector>

namespace nautilus::conversion {

using namespace nautilus::nir;

namespace {

[[noreturn]] void fail(uint32_t function, const std::string& message) {
	throw std::runtime_error("conversion of function #" + std::to_string(function) + ": " + message);
}

/// An open-addressing hash table from a 64-bit key to a 32-bit value, without deletion. `reset` keeps the memory of
/// the biggest table it has had, so that the next function of a thread starts without allocating.
class FlatMap {
public:
	void reset() {
		slots_.assign(MinSlots, Slot {EMPTY, 0});
		count_ = 0;
	}
	/// The value of `key`, or nullptr.
	const uint32_t* find(uint64_t key) const {
		const size_t mask = slots_.size() - 1;
		for (size_t i = indexOf(key) & mask;; i = (i + 1) & mask) {
			if (slots_[i].key == key) {
				return &slots_[i].value;
			}
			if (slots_[i].key == EMPTY) {
				return nullptr;
			}
		}
	}
	void set(uint64_t key, uint32_t value) {
		if ((count_ + 1) * 2 > slots_.size()) {
			grow();
		}
		insert(key, value);
	}
	size_t capacityBytes() const {
		return slots_.capacity() * sizeof(Slot);
	}

private:
	struct Slot {
		uint64_t key;
		uint32_t value;
	};
	static constexpr uint64_t EMPTY = ~0ull;
	static constexpr size_t MinSlots = 32;
	static size_t indexOf(uint64_t key) {
		return static_cast<size_t>((key * 0x9E3779B97F4A7C15ull) >> 32);
	}
	void insert(uint64_t key, uint32_t value) {
		const size_t mask = slots_.size() - 1;
		size_t i = indexOf(key) & mask;
		while (slots_[i].key != EMPTY && slots_[i].key != key) {
			i = (i + 1) & mask;
		}
		count_ += slots_[i].key == EMPTY;
		slots_[i] = Slot {key, value};
	}
	void grow() {
		std::vector<Slot> old(slots_.size() * 2, Slot {EMPTY, 0});
		old.swap(slots_);
		count_ = 0;
		for (const auto& s : old) {
			if (s.key != EMPTY) {
				insert(s.key, s.value);
			}
		}
	}
	std::vector<Slot> slots_ = std::vector<Slot>(MinSlots, Slot {EMPTY, 0});
	size_t count_ = 0;
};

/// Lists of block ids, one per block, in one array.
struct Csr {
	std::vector<uint32_t> start, items;
	std::span<const uint32_t> operator[](uint32_t b) const {
		return {items.data() + start[b], start[b + 1] - start[b]};
	}
};

struct Edge {
	uint32_t from, to;
};

/// The layout of a control-flow graph: reverse postorder, with every natural loop placed contiguously where its header
/// is, and the loops it found. Irreducible regions (a retreating edge whose target does not dominate its source) keep
/// the plain reverse postorder. The object keeps its arrays between computations.
struct Layout {
	std::vector<uint32_t> rpo,
	    rpoIndex; // reachable blocks in reverse postorder, and the index of each (NONE: unreachable)
	std::vector<uint32_t> layout;
	std::vector<uint32_t> headers; // the loop headers, by block id
	/// Of a loop header: the other blocks of its loop in reverse postorder, as the slice [bodyBegin, bodyEnd) of
	/// `body`.
	std::vector<uint32_t> body, bodyBegin, bodyEnd;
	uint32_t irreducible = 0;
	/// `mark[b] == stamp` marks a member of the loop under examination; a new stamp empties the set.
	std::vector<uint32_t> mark;
	uint32_t stamp = 0;

	bool isHeader(uint32_t b) const {
		return bodyEnd[b] != NONE;
	}
	void compute(uint32_t n, const Csr& succ, const Csr& pred);
	size_t capacityBytes() const {
		return (rpo.capacity() + rpoIndex.capacity() + layout.capacity() + headers.capacity() + body.capacity() +
		        bodyBegin.capacity() + bodyEnd.capacity() + mark.capacity() + state_.capacity() + post_.capacity() +
		        idom_.capacity() + latchHead_.capacity() + latchNext_.capacity() + latchSource_.capacity() +
		        work_.capacity() + members_.capacity() + placed_.capacity()) *
		       sizeof(uint32_t);
	}

private:
	std::vector<uint8_t> state_, placed_;
	std::vector<uint32_t> post_, idom_, latchHead_, latchNext_, latchSource_, work_, members_;
	std::vector<std::pair<uint32_t, uint32_t>> dfs_;
	std::vector<std::pair<const uint32_t*, const uint32_t*>> placeStack_;
};

void Layout::compute(uint32_t n, const Csr& succ, const Csr& pred) {
	// reverse postorder from the entry block
	state_.assign(n, 0);
	post_.clear();
	dfs_.clear();
	dfs_.push_back({0, 0});
	state_[0] = 1;
	while (!dfs_.empty()) {
		auto& top = dfs_.back();
		const auto out = succ[top.first];
		if (top.second < out.size()) {
			auto next = out[top.second++];
			if (state_[next] == 0) {
				state_[next] = 1;
				dfs_.push_back({next, 0});
			}
		} else {
			post_.push_back(top.first);
			dfs_.pop_back();
		}
	}
	rpo.assign(post_.rbegin(), post_.rend());
	rpoIndex.assign(n, NONE);
	for (uint32_t i = 0; i < rpo.size(); ++i) {
		rpoIndex[rpo[i]] = i;
	}

	// dominators (Cooper, Harvey, Kennedy) over the reachable blocks, by reverse postorder index
	idom_.assign(n, NONE);
	idom_[0] = 0;
	auto intersect = [&](uint32_t a, uint32_t b) {
		while (a != b) {
			while (rpoIndex[a] > rpoIndex[b]) {
				a = idom_[a];
			}
			while (rpoIndex[b] > rpoIndex[a]) {
				b = idom_[b];
			}
		}
		return a;
	};
	for (bool changed = true; changed;) {
		changed = false;
		for (size_t i = 1; i < rpo.size(); ++i) {
			auto b = rpo[i];
			uint32_t candidate = NONE;
			for (auto p : pred[b]) {
				if (rpoIndex[p] == NONE || idom_[p] == NONE) {
					continue;
				}
				candidate = candidate == NONE ? p : intersect(p, candidate);
			}
			if (candidate != idom_[b]) {
				idom_[b] = candidate;
				changed = true;
			}
		}
	}
	auto dominates = [&](uint32_t a, uint32_t b) {
		while (true) {
			if (a == b) {
				return true;
			}
			if (b == 0 || idom_[b] == NONE) {
				return false;
			}
			b = idom_[b];
		}
	};

	// back edges (the target dominates the source) head the natural loops
	latchHead_.assign(n, NONE);
	latchNext_.clear();
	latchSource_.clear();
	irreducible = 0;
	for (auto s : rpo) {
		for (auto t : succ[s]) {
			if (rpoIndex[t] > rpoIndex[s]) {
				continue;
			}
			if (dominates(t, s)) {
				latchSource_.push_back(s);
				latchNext_.push_back(latchHead_[t]);
				latchHead_[t] = static_cast<uint32_t>(latchSource_.size() - 1);
			} else {
				++irreducible;
			}
		}
	}
	headers.clear();
	body.clear();
	bodyBegin.assign(n, 0);
	bodyEnd.assign(n, NONE);
	mark.assign(n, 0);
	stamp = 0;
	for (uint32_t h = 0; h < n; ++h) {
		if (latchHead_[h] == NONE) {
			continue;
		}
		headers.push_back(h);
		// the loop: the header, and everything that reaches a latch without passing the header
		const uint32_t s = ++stamp;
		mark[h] = s;
		work_.clear();
		members_.clear();
		for (auto l = latchHead_[h]; l != NONE; l = latchNext_[l]) {
			if (mark[latchSource_[l]] != s) {
				mark[latchSource_[l]] = s;
				work_.push_back(latchSource_[l]);
			}
		}
		while (!work_.empty()) {
			auto b = work_.back();
			work_.pop_back();
			members_.push_back(b);
			for (auto p : pred[b]) {
				if (rpoIndex[p] != NONE && mark[p] != s) {
					mark[p] = s;
					work_.push_back(p);
				}
			}
		}
		std::sort(members_.begin(), members_.end(), [&](uint32_t a, uint32_t b) { return rpoIndex[a] < rpoIndex[b]; });
		bodyBegin[h] = static_cast<uint32_t>(body.size());
		body.insert(body.end(), members_.begin(), members_.end());
		bodyEnd[h] = static_cast<uint32_t>(body.size());
	}

	// layout: reverse postorder, with each natural loop placed contiguously where its header is
	placed_.assign(n, 0);
	layout.clear();
	placeStack_.clear();
	placeStack_.push_back({rpo.data(), rpo.data() + rpo.size()});
	while (!placeStack_.empty()) {
		auto& top = placeStack_.back();
		if (top.first == top.second) {
			placeStack_.pop_back();
			continue;
		}
		const auto b = *top.first++;
		if (placed_[b]) {
			continue;
		}
		placed_[b] = 1;
		layout.push_back(b);
		if (isHeader(b)) {
			placeStack_.push_back({body.data() + bodyBegin[b], body.data() + bodyEnd[b]});
		}
	}
}

/// One function: layout (§3.1), Braun's SSA construction with an explicit stack (§3 step 1) and the emission into the
/// output module. Values during the conversion are `Ref`s: a temporary id (an instruction or block parameter this
/// converter made) or a constant reference. Temporaries become instruction ids when the function is emitted.
///
/// The object is reused for the functions of a module and for the modules of a thread: every table is a flat array
/// that `run` empties and refills, and the arrays keep their memory.
class FunctionConverter {
public:
	/// Binds the converter to a module; `functions` and `callees` are its tables with the attributes the pre-pass
	/// derived.
	void bind(const Module& pre, const std::vector<FunctionRecord>& functions, const std::vector<CalleeRecord>& callees,
	          Module& out, ConversionStatistics& stats, bool optimize) {
		pre_ = &pre;
		functions_ = &functions;
		callees_ = &callees;
		out_ = &out;
		stats_ = &stats;
		optimize_ = optimize;
	}
	void run(uint32_t function);
	/// The memory the tables hold between runs.
	size_t retainedBytes() const;
	/// Gives the memory back.
	void release();

private:
	const Module& pre() const {
		return *pre_;
	}
	Module& out() const {
		return *out_;
	}
	ConversionStatistics& stats() const {
		return *stats_;
	}
	const std::vector<CalleeRecord>& callees() const {
		return *callees_;
	}

	// ---- the pre-SSA function ---------------------------------------------------------------------------------

	std::span<const uint32_t> blockIds(uint32_t b) const {
		return {pre().extra.data() + blocks_[b].first, blocks_[b].count};
	}
	const Inst& terminator(uint32_t b) const {
		return insts_[blockIds(b).back()];
	}
	void reset(uint32_t function);
	void analyze();
	void buildCfg();
	void orderBlocks();

	// ---- SSA construction --------------------------------------------------------------------------------------

	enum class Kind : uint8_t { Inst, Phi, Param };

	/// What a temporary is. The hot data in 32 bytes; the variable-length parts live in `words_` and the side tables.
	struct Temp {
		Inst inst {}; // Inst: the instruction, operands as Refs
		uint32_t block = NONE;
		/// Inst: the index in `calls_` for a call and a checked division, the first of the (consecutive) edges of a
		/// terminator; Phi, Param: the index in `phiInfo_`.
		uint32_t aux = NONE;
		Kind kind = Kind::Inst;
		TypeId type = TypeId::Void;
	};
	/// A call's operand list and the words after its unwind entry (in `words_`), and the unwind of a call or a checked
	/// division: NO_UNWIND, NO_CLEANUP or a pad (padBase + index, resolved when emitted).
	struct CallInfo {
		uint32_t list = 0, listCount = 0, tail = 0, tailCount = 0;
		uint32_t unwind = NO_UNWIND;
		bool usesPad = false;
	};
	/// A phi or entry parameter: the variable, one operand per incoming edge in registration order (a slice of `words_`
	/// reserved when the block is sealed), the phis that use it, and its links in the lists of its block.
	struct PhiInfo {
		uint32_t variable = NONE;
		uint32_t ops = 0, opsCount = 0;
		uint32_t nextInBlock = NONE;    // the next phi of the block, in creation order
		uint32_t nextIncomplete = NONE; // the next phi of the block that waits for the seal
		std::vector<uint32_t> users;
	};
	/// What the conversion knows of a block while it works.
	struct BlockState {
		uint32_t phiHead = NONE, phiTail = NONE;               // phis in creation order (block 0: the parameters first)
		uint32_t incompleteHead = NONE, incompleteTail = NONE; // phis that wait for the seal
		uint32_t incomingCount = 0;                            // live edges that entered the block
		uint32_t bodyBegin = 0, bodyEnd = 0;                   // the temporaries of the instructions in `bodyAll_`
		uint32_t pending = 0;                                  // static predecessors not converted yet
		uint32_t ebbParent = NONE, ebbDepth = 0, epochAtEnd = 0;
		uint32_t finalPhis = 0;
		bool sealed = false, visited = false, emitted = false;
	};

	struct Pad {
		std::vector<std::pair<uint32_t, Ref>> entries; // destructor callee, object address
	};

	uint32_t newTemp(Kind kind, TypeId type, uint32_t block) {
		Temp t;
		t.kind = kind;
		t.type = type;
		t.block = block;
		if (kind != Kind::Inst) {
			t.aux = static_cast<uint32_t>(phiInfo_.size());
			phiInfo_.emplace_back();
		}
		temps_.push_back(t);
		fwd_.push_back(NONE);
		return static_cast<uint32_t>(temps_.size() - 1);
	}
	bool isTemp(Ref r) const {
		return r != NONE && !isConstRef(r);
	}
	Ref resolve(Ref r) const {
		while (isTemp(r) && fwd_[r] != NONE) {
			r = fwd_[r];
		}
		return r;
	}
	PhiInfo& phiOf(uint32_t phi) {
		return phiInfo_[temps_[phi].aux];
	}
	/// The operands of a phi so far.
	std::span<const uint32_t> opsOf(uint32_t phi) const {
		const auto& info = phiInfo_[temps_[phi].aux];
		return {words_.data() + info.ops, info.opsCount};
	}
	/// Makes room for `count` operands of the phi.
	void reserveOps(uint32_t phi, size_t count) {
		auto& info = phiOf(phi);
		info.ops = static_cast<uint32_t>(words_.size());
		info.opsCount = 0;
		words_.resize(words_.size() + count);
	}
	void pushOp(uint32_t phi, Ref value) {
		auto& info = phiOf(phi);
		words_[info.ops + info.opsCount++] = value;
	}
	/// Appends the phi (or parameter) to the list of its block.
	void linkPhi(uint32_t block, uint32_t phi) {
		auto& state = states_[block];
		if (state.phiTail == NONE) {
			state.phiHead = phi;
		} else {
			phiOf(state.phiTail).nextInBlock = phi;
		}
		state.phiTail = phi;
	}
	static uint64_t defKey(uint32_t variable, uint32_t block) {
		return (static_cast<uint64_t>(block) << 32) | variable;
	}
	Ref getDef(uint32_t variable, uint32_t block) const {
		const auto* v = currentDef_.find(defKey(variable, block));
		return v == nullptr ? NONE : resolve(*v);
	}
	void setDef(uint32_t variable, uint32_t block, Ref value) {
		currentDef_.set(defKey(variable, block), value);
	}
	/// The edges that entered the block live, in registration order: a slice of the room its static in-degree gives.
	uint32_t incomingEdge(uint32_t block, uint32_t i) const {
		return incomingEdges_[preds_.start[block] + i];
	}

	Ref operand(Ref r, uint32_t block);
	Ref readVariable(uint32_t variable, uint32_t block);
	struct Frame {
		uint32_t phi, block, next;
		uint32_t chain; // the blocks the read passed on its way to the phi: from here to the end of `chainWords_`
	};
	Ref startRead(uint32_t variable, uint32_t block);
	uint32_t newPhi(uint32_t variable, uint32_t block);
	Ref tryRemoveTrivialPhi(uint32_t phi);
	void seal(uint32_t block);
	void addPhiOperands(uint32_t phi);

	void convertBlock(uint32_t b);
	void convertInstruction(uint32_t b, uint32_t id, const Inst& inst);
	void convertTerminator(uint32_t b, const Inst& inst);
	uint32_t makeEdge(uint32_t from, uint32_t to);
	uint32_t padFor(const std::vector<std::pair<uint32_t, Ref>>& entries);
	uint32_t convertUnwind(uint32_t b, uint32_t cleanupOffset);
	Ref simplify(Inst& inst);

	// ---- common subexpressions (M3.4) --------------------------------------------------------------------------
	// Available values are recorded with the block that computed them, and the blocks are chained to their only static
	// predecessor (an extended basic block): a value computed in an ancestor dominates the block. Loads and calls that
	// read memory are keyed on the memory epoch, a number that every store and every call that may write takes anew; a
	// block with several predecessors starts a new epoch, so a store in one arm of an if never lets the join reuse a
	// load from before it. All entries of the function are in one hash table; of the entries with the key that were
	// computed in an ancestor, the nearest one wins.
	struct AvailKey {
		uint16_t op;
		uint8_t type, flags;
		uint32_t a, b, c, epoch;
		std::span<const uint32_t> list;
		uint64_t hash;
	};
	struct AvailEntry {
		uint64_t hash;
		uint32_t a, b, c, epoch;
		uint32_t block, value, next;
		uint32_t list, listCount; // in words_
		uint16_t op;
		uint8_t type, flags;
	};
	static constexpr uint32_t MaxAvailDepth = 32;
	static constexpr uint32_t MaxRefoldSweeps = 3;
	void enterBlockForCse(uint32_t b);
	static uint64_t hashOf(const AvailKey& k);
	Ref findAvailable(const AvailKey& key, uint32_t b) const;
	void addAvailable(const AvailKey& key, uint32_t b, Ref value);
	/// Whether `ni` may be shared, and the key that says so; calls are keyed on their argument list.
	bool availKey(const Inst& ni, uint32_t b, std::span<const uint32_t> args, AvailKey& key) const;
	std::vector<AvailEntry> availEntries_;
	std::vector<uint32_t> availHead_; // the buckets: the first entry, NONE for none
	uint32_t nextEpoch_ = 1;

	TypeId typeOfRef(Ref r) const {
		return isConstRef(r) ? static_cast<TypeId>(out().consts.record(r).type) : temps_[r].type;
	}

	// ---- emission ----------------------------------------------------------------------------------------------

	void emit();
	/// The arguments a branch along `edge` passes: the live phis of its target, in order, appended to `list_`.
	void appendBranchArgs(uint32_t edge);
	/// Whether the temporary's `aux` is an index in `calls_`.
	static bool hasCallInfo(const Temp& t) {
		const auto op = static_cast<Op>(t.inst.op);
		return t.aux != NONE && (op == Op::Call || op == Op::CallIndirect || op == Op::Div || op == Op::Rem);
	}
	Ref finalRef(Ref r) const {
		r = resolve(r);
		return isTemp(r) ? finalId_[r] : r;
	}

	const Module* pre_ = nullptr;
	const std::vector<FunctionRecord>* functions_ = nullptr;
	const std::vector<CalleeRecord>* callees_ = nullptr;
	Module* out_ = nullptr;
	ConversionStatistics* stats_ = nullptr;
	bool optimize_ = true;
	uint32_t f_ = 0;
	const FunctionRecord* rec_ = nullptr;
	std::span<const Inst> insts_;
	std::span<const BlockRecord> blocks_;
	/// Constant branches are folded and dead blocks dropped; off for a function with an irreducible region, where a
	/// block can be entered through an edge from a block that comes later.
	bool foldBranches_ = false;
	uint32_t irreducibleHere_ = 0;
	std::vector<uint32_t> emitOrder_;

	uint32_t n_ = 0; // blocks
	std::vector<uint8_t> isVariable_;
	Csr succ_; // per block: the successors, in terminator order
	Csr preds_;
	Layout cfg_, liveCfg_;
	/// Of a loop header, the variables assigned in the loop (key: header and variable), and the only block outside it
	/// that jumps to the header (NONE if there are several, or the function has an irreducible region).
	FlatMap loopAssigned_;
	std::vector<uint32_t> loopEntry_;
	std::vector<uint32_t> layoutIndex_;

	std::vector<Temp> temps_;
	std::vector<uint32_t> fwd_;
	uint32_t phisForwarded_ = 0; // the phis that tryRemoveTrivialPhi forwarded to another value
	std::vector<BlockState> states_;
	std::vector<uint32_t> bodyAll_;       // the temporaries of the instructions, block by block, terminator last
	FlatMap currentDef_;                  // (block, variable) -> value
	std::vector<uint32_t> incomingEdges_; // per block a slice (at preds_.start) of its live incoming edge ids
	std::vector<Ref> valueOf_;            // pre-SSA instruction id -> value, for ids that are never assigned
	std::vector<PhiInfo> phiInfo_;
	std::vector<CallInfo> calls_;
	std::vector<uint32_t> words_;                // the lists of calls and the operands of phis
	std::vector<uint32_t> args_, argsResolved_;  // scratch: the operands of the call being converted
	std::vector<Frame> frames_;                  // scratch of readVariable
	std::vector<uint32_t> chainWords_, phiWork_; // scratch of readVariable and tryRemoveTrivialPhi
	std::vector<Pad> pads_;
	std::map<std::vector<std::pair<uint32_t, Ref>>, uint32_t> padIndex_;
	std::vector<Edge> dynEdges_;
	std::vector<uint32_t> edgeSlot_; // per live edge: its position among the incoming edges of its target

	// the emission
	std::vector<uint32_t> finalId_;
	std::vector<uint8_t> marked_;
	std::vector<uint32_t> markWork_, list_, padBlock_, padOrder_, fillPosition_;
	Csr liveSucc_, livePred_;
};

size_t FunctionConverter::retainedBytes() const {
	return emitOrder_.capacity() * 4 + isVariable_.capacity() + (succ_.start.capacity() + succ_.items.capacity()) * 4 +
	       (preds_.start.capacity() + preds_.items.capacity()) * 4 + cfg_.capacityBytes() + liveCfg_.capacityBytes() +
	       loopAssigned_.capacityBytes() + currentDef_.capacityBytes() + loopEntry_.capacity() * 4 +
	       layoutIndex_.capacity() * 4 + temps_.capacity() * sizeof(Temp) + fwd_.capacity() * 4 +
	       states_.capacity() * sizeof(BlockState) + bodyAll_.capacity() * 4 + incomingEdges_.capacity() * 4 +
	       valueOf_.capacity() * 4 + phiInfo_.capacity() * sizeof(PhiInfo) + calls_.capacity() * sizeof(CallInfo) +
	       words_.capacity() * 4 + availEntries_.capacity() * sizeof(AvailEntry) + availHead_.capacity() * 4 +
	       dynEdges_.capacity() * sizeof(Edge) + edgeSlot_.capacity() * 4 + finalId_.capacity() * 4 +
	       marked_.capacity() +
	       (markWork_.capacity() + list_.capacity() + padBlock_.capacity() + padOrder_.capacity() +
	        fillPosition_.capacity()) *
	           4 +
	       (liveSucc_.items.capacity() + livePred_.items.capacity()) * 4;
}

void FunctionConverter::release() {
	*this = FunctionConverter();
}

void FunctionConverter::enterBlockForCse(uint32_t b) {
	auto& state = states_[b];
	state.ebbParent = NONE;
	state.ebbDepth = 0;
	const auto preds = preds_[b];
	if (preds.size() == 1 && preds[0] != b && states_[preds[0]].visited && states_[preds[0]].ebbDepth < MaxAvailDepth) {
		state.ebbParent = preds[0];
		state.ebbDepth = states_[preds[0]].ebbDepth + 1;
		state.epochAtEnd = states_[preds[0]].epochAtEnd;
	} else {
		state.epochAtEnd = nextEpoch_++;
	}
}

Ref FunctionConverter::findAvailable(const AvailKey& key, uint32_t b) const {
	Ref best = NONE;
	uint32_t bestDepth = 0;
	const uint32_t depthOfB = states_[b].ebbDepth;
	for (uint32_t e = availHead_[key.hash & (availHead_.size() - 1)]; e != NONE; e = availEntries_[e].next) {
		const auto& entry = availEntries_[e];
		if (entry.hash != key.hash || entry.op != key.op || entry.type != key.type || entry.flags != key.flags ||
		    entry.a != key.a || entry.b != key.b || entry.c != key.c || entry.epoch != key.epoch ||
		    entry.listCount != key.list.size() ||
		    !std::equal(key.list.begin(), key.list.end(), words_.begin() + entry.list)) {
			continue;
		}
		// the entry counts if its block is b or an ancestor of b; the depth drops by one per step up
		const uint32_t depth = states_[entry.block].ebbDepth;
		if (depth > depthOfB || (best != NONE && depth <= bestDepth)) {
			continue;
		}
		uint32_t at = b;
		for (uint32_t steps = depthOfB - depth; steps != 0; --steps) {
			at = states_[at].ebbParent;
		}
		if (at == entry.block) {
			best = entry.value;
			bestDepth = depth;
		}
	}
	return best == NONE ? NONE : resolve(best);
}

void FunctionConverter::addAvailable(const AvailKey& key, uint32_t b, Ref value) {
	if (availEntries_.size() >= availHead_.size()) {
		// the table is at most full: twice the buckets, and every entry chained again
		availHead_.assign(availHead_.size() * 2, NONE);
		for (uint32_t e = 0; e < availEntries_.size(); ++e) {
			auto& bucket = availHead_[availEntries_[e].hash & (availHead_.size() - 1)];
			availEntries_[e].next = bucket;
			bucket = e;
		}
	}
	auto& bucket = availHead_[key.hash & (availHead_.size() - 1)];
	const auto listOffset = static_cast<uint32_t>(words_.size());
	words_.insert(words_.end(), key.list.begin(), key.list.end());
	availEntries_.push_back(AvailEntry {key.hash, key.a, key.b, key.c, key.epoch, b, value, bucket, listOffset,
	                                    static_cast<uint32_t>(key.list.size()), key.op, key.type, key.flags});
	bucket = static_cast<uint32_t>(availEntries_.size() - 1);
}

uint64_t FunctionConverter::hashOf(const AvailKey& k) {
	uint64_t h = k.op * 0x9E3779B97F4A7C15ull ^ (uint64_t(k.type) << 8 | k.flags);
	for (uint32_t v : {k.a, k.b, k.c, k.epoch}) {
		h = (h ^ v) * 0x100000001B3ull;
		h ^= h >> 29;
	}
	for (uint32_t v : k.list) {
		h = (h ^ v) * 0x100000001B3ull;
		h ^= h >> 29;
	}
	// the low bits pick the bucket
	h ^= h >> 32;
	h *= 0x9E3779B97F4A7C15ull;
	return h ^ (h >> 29);
}

bool FunctionConverter::availKey(const Inst& ni, uint32_t b, std::span<const uint32_t> args, AvailKey& key) const {
	const auto op = static_cast<Op>(ni.op);
	const auto* info = coreOpInfo(ni.op);
	key = AvailKey {ni.op, ni.type, ni.flags, ni.a, ni.b, ni.c, 0, {}, 0};
	if (info->flags & OpPure) {
		const bool commutative = op == Op::Add || op == Op::Mul || op == Op::BAnd || op == Op::BOr || op == Op::BXor ||
		                         op == Op::And || op == Op::Or;
		if (commutative && !semantics::isFloatType(static_cast<TypeId>(ni.type)) && key.a > key.b) {
			std::swap(key.a, key.b);
		}
		key.hash = hashOf(key);
		return true;
	}
	if (op == Op::Load) {
		key.epoch = states_[b].epochAtEnd;
		key.hash = hashOf(key);
		return true;
	}
	if (op == Op::Call && ni.a < callees().size()) {
		// only a call that neither writes, nor throws, nor fails to return can be shared
		const auto attrs = callees()[ni.a].attrs;
		constexpr uint32_t Needed = AttrNoArgWrite | AttrNoOtherWrite | AttrNoUnwind | AttrWillReturn;
		if ((attrs & Needed) != Needed) {
			return false;
		}
		const bool reads = (attrs & (AttrNoArgRead | AttrNoOtherRead)) != (AttrNoArgRead | AttrNoOtherRead);
		key.c = 0;
		key.epoch = reads ? states_[b].epochAtEnd : 0;
		key.list = args;
		key.hash = hashOf(key);
		return true;
	}
	return false;
}

// ---- analysis -------------------------------------------------------------------------------------------------

void FunctionConverter::analyze() {
	isVariable_.assign(insts_.size(), 0);
	for (const auto& inst : insts_) {
		if (inst.op == static_cast<uint16_t>(Op::Assign) && inst.a < insts_.size()) {
			isVariable_[inst.a] = 1;
		}
	}
	valueOf_.assign(insts_.size(), NONE);
}

void FunctionConverter::buildCfg() {
	n_ = rec_->blockCount;
	succ_.start.assign(1, 0);
	succ_.items.clear();
	for (uint32_t b = 0; b < n_; ++b) {
		if (blocks_[b].count == 0) {
			fail(f_, "an empty block");
		}
		const auto& term = terminator(b);
		auto addSucc = [&](uint32_t to) {
			if (to >= n_) {
				fail(f_, "a branch to a block that does not exist");
			}
			succ_.items.push_back(to);
		};
		switch (static_cast<Op>(term.op)) {
		case Op::Br:
			addSucc(term.a);
			break;
		case Op::CondBr: {
			const uint32_t* list = pre().extra.data() + term.c;
			addSucc(term.b);
			addSucc(list[1 + list[1] + 1]); // [prob, trueArgc, trueArgs..., falseBlk, ...]
			break;
		}
		case Op::Ret:
			break;
		default:
			fail(f_, "a block does not end in a terminator");
		}
		succ_.start.push_back(static_cast<uint32_t>(succ_.items.size()));
	}
	// the predecessors, in the order of the blocks that name them
	preds_.start.assign(n_ + 1, 0);
	for (auto to : succ_.items) {
		++preds_.start[to + 1];
	}
	for (uint32_t b = 0; b < n_; ++b) {
		preds_.start[b + 1] += preds_.start[b];
	}
	preds_.items.assign(succ_.items.size(), 0);
	fillPosition_.assign(preds_.start.begin(), preds_.start.end() - 1);
	for (uint32_t b = 0; b < n_; ++b) {
		for (auto to : succ_[b]) {
			preds_.items[fillPosition_[to]++] = b;
		}
	}
}

void FunctionConverter::orderBlocks() {
	cfg_.compute(n_, succ_, preds_);
	if (!preds_[0].empty()) {
		for (auto p : preds_[0]) {
			if (cfg_.rpoIndex[p] != NONE) {
				fail(f_, "the entry block has a predecessor");
			}
		}
	}
	stats().irreducibleRegions += cfg_.irreducible;
	irreducibleHere_ += cfg_.irreducible;

	loopAssigned_.reset();
	loopEntry_.assign(n_, NONE);
	for (const auto h : cfg_.headers) {
		++stats().loops;
		// the variables assigned in the loop, and the one block that enters it: a variable nobody assigns in the loop
		// has the value it has on entry and needs no phi in the header
		const std::span<const uint32_t> body(cfg_.body.data() + cfg_.bodyBegin[h], cfg_.bodyEnd[h] - cfg_.bodyBegin[h]);
		auto note = [&](uint32_t b) {
			for (auto id : blockIds(b)) {
				const auto& inst = insts_[id];
				if (inst.op == static_cast<uint16_t>(Op::Assign)) {
					loopAssigned_.set(defKey(inst.a, h), 1);
				} else if (inst.op == static_cast<uint16_t>(Op::Var) && isVariable_[id]) {
					loopAssigned_.set(defKey(id, h), 1);
				}
			}
		};
		note(h);
		const uint32_t stamp = ++cfg_.stamp;
		cfg_.mark[h] = stamp;
		for (auto b : body) {
			note(b);
			cfg_.mark[b] = stamp;
		}
		uint32_t entry = NONE;
		bool unique = true;
		for (auto p : preds_[h]) {
			if (cfg_.mark[p] == stamp || cfg_.rpoIndex[p] == NONE) {
				continue;
			}
			unique = unique && entry == NONE;
			entry = p;
		}
		loopEntry_[h] = unique ? entry : NONE;
	}
	layoutIndex_.assign(n_, NONE);
	for (uint32_t i = 0; i < cfg_.layout.size(); ++i) {
		layoutIndex_[cfg_.layout[i]] = i;
	}
}

// ---- Braun's construction -------------------------------------------------------------------------------------

uint32_t FunctionConverter::newPhi(uint32_t variable, uint32_t block) {
	auto type = static_cast<TypeId>(insts_[variable].type);
	auto id = newTemp(Kind::Phi, type, block);
	phiOf(id).variable = variable;
	linkPhi(block, id);
	return id;
}

Ref FunctionConverter::operand(Ref r, uint32_t block) {
	if (r == NONE || isConstRef(r)) {
		return r;
	}
	if (r >= insts_.size()) {
		fail(f_, "an operand that is not an instruction");
	}
	if (isVariable_[r]) {
		return readVariable(r, block);
	}
	if (valueOf_[r] == NONE) {
		fail(f_, "%" + std::to_string(r) + " is used before it is defined");
	}
	return resolve(valueOf_[r]);
}

Ref FunctionConverter::startRead(uint32_t variable, uint32_t block) {
	const auto base = static_cast<uint32_t>(chainWords_.size());
	Ref value = NONE;
	for (;;) {
		if (auto d = getDef(variable, block); d != NONE) {
			value = d;
			break;
		}
		if (!states_[block].sealed && irreducibleHere_ == 0 && loopEntry_[block] != NONE &&
		    loopAssigned_.find(defKey(variable, block)) == nullptr) {
			// a loop header that nothing in the loop assigns the variable of: read it where the loop is entered
			const uint32_t entry = loopEntry_[block];
			bool live = false;
			for (uint32_t i = 0; i < states_[block].incomingCount; ++i) {
				live = live || dynEdges_[incomingEdge(block, i)].from == entry;
			}
			if (live) {
				++stats().invariantHeaderReads;
				chainWords_.push_back(block);
				block = entry;
				continue;
			}
		}
		if (!states_[block].sealed) {
			auto phi = newPhi(variable, block);
			auto& state = states_[block];
			if (state.incompleteTail == NONE) {
				state.incompleteHead = phi;
			} else {
				phiOf(state.incompleteTail).nextIncomplete = phi;
			}
			state.incompleteTail = phi;
			setDef(variable, block, phi);
			value = phi;
			break;
		}
		const auto count = states_[block].incomingCount;
		if (count == 1) {
			chainWords_.push_back(block);
			block = dynEdges_[incomingEdge(block, 0)].from;
			continue;
		}
		if (count == 0) {
			fail(f_, "%" + std::to_string(variable) + " is read before it is defined");
		}
		auto phi = newPhi(variable, block);
		reserveOps(phi, count);
		setDef(variable, block, phi);
		frames_.push_back(Frame {phi, block, 0, base});
		return NONE;
	}
	for (auto i = base; i < chainWords_.size(); ++i) {
		setDef(variable, chainWords_[i], value);
	}
	chainWords_.resize(base);
	return value;
}

Ref FunctionConverter::readVariable(uint32_t variable, uint32_t block) {
	const auto frameBase = frames_.size();
	if (Ref v = startRead(variable, block); v != NONE) {
		return v;
	}
	Ref delivered = NONE;
	while (frames_.size() > frameBase) {
		if (delivered != NONE) {
			auto& parent = frames_.back();
			pushOp(parent.phi, delivered);
			if (isTemp(delivered) && temps_[delivered].kind == Kind::Phi) {
				phiOf(delivered).users.push_back(parent.phi);
			}
			++parent.next;
			delivered = NONE;
		}
		auto& f = frames_.back();
		const auto count = states_[f.block].incomingCount;
		if (f.next < count) {
			const auto from = dynEdges_[incomingEdge(f.block, f.next)].from;
			const auto var = phiOf(f.phi).variable;
			if (Ref v = startRead(var, from); v != NONE) {
				delivered = v;
			}
			continue;
		}
		const Frame done = f;
		frames_.pop_back();
		const auto var = phiOf(done.phi).variable;
		Ref value = tryRemoveTrivialPhi(done.phi);
		setDef(var, done.block, value);
		for (auto i = done.chain; i < chainWords_.size(); ++i) {
			setDef(var, chainWords_[i], value);
		}
		chainWords_.resize(done.chain);
		if (frames_.size() == frameBase) {
			return resolve(value);
		}
		delivered = value;
	}
	fail(f_, "unreachable");
}

Ref FunctionConverter::tryRemoveTrivialPhi(uint32_t phi) {
	phiWork_.clear();
	phiWork_.push_back(phi);
	Ref result = phi;
	bool first = true;
	while (!phiWork_.empty()) {
		auto p = phiWork_.back();
		phiWork_.pop_back();
		if (fwd_[p] != NONE || temps_[p].kind != Kind::Phi) {
			continue;
		}
		Ref same = NONE;
		bool trivial = true;
		for (auto op : opsOf(p)) {
			op = resolve(op);
			if (op == same || op == p) {
				continue;
			}
			if (same != NONE) {
				trivial = false;
				break;
			}
			same = op;
		}
		if (!trivial || same == NONE) {
			continue; // merges two values, or only ever sees itself
		}
		fwd_[p] = same;
		++phisForwarded_;
		++stats().removedPhis;
		if (first) {
			result = same;
		}
		for (auto user : phiOf(p).users) {
			if (user != p) {
				phiWork_.push_back(user);
			}
		}
		if (isTemp(same) && temps_[same].kind == Kind::Phi) {
			auto& to = phiOf(same).users;
			for (auto user : phiOf(p).users) {
				to.push_back(user);
			}
		}
		first = false;
	}
	return resolve(result);
}

void FunctionConverter::addPhiOperands(uint32_t phi) {
	// the operands, one per incoming edge, in the order the edges were registered
	const auto block = temps_[phi].block;
	const auto variable = phiOf(phi).variable;
	const auto count = states_[block].incomingCount;
	reserveOps(phi, count);
	for (uint32_t i = 0; i < count; ++i) {
		auto from = dynEdges_[incomingEdge(block, i)].from;
		Ref v = readVariable(variable, from);
		pushOp(phi, v);
		if (isTemp(v) && temps_[v].kind == Kind::Phi) {
			phiOf(v).users.push_back(phi);
		}
	}
	tryRemoveTrivialPhi(phi);
}

void FunctionConverter::seal(uint32_t block) {
	states_[block].sealed = true;
	auto phi = states_[block].incompleteHead;
	states_[block].incompleteHead = states_[block].incompleteTail = NONE;
	while (phi != NONE) {
		const auto next = phiOf(phi).nextIncomplete;
		addPhiOperands(phi);
		phi = next;
	}
}

// ---- instructions -----------------------------------------------------------------------------------------------

uint32_t FunctionConverter::makeEdge(uint32_t from, uint32_t to) {
	dynEdges_.push_back({from, to});
	auto id = static_cast<uint32_t>(dynEdges_.size() - 1);
	auto slot = states_[to].incomingCount++;
	incomingEdges_[preds_.start[to] + slot] = id;
	edgeSlot_.push_back(slot);
	return id;
}

uint32_t FunctionConverter::padFor(const std::vector<std::pair<uint32_t, Ref>>& entries) {
	auto it = padIndex_.find(entries);
	if (it != padIndex_.end()) {
		return it->second;
	}
	pads_.push_back(Pad {entries});
	auto index = static_cast<uint32_t>(pads_.size() - 1);
	padIndex_.emplace(entries, index);
	return index;
}

uint32_t FunctionConverter::convertUnwind(uint32_t b, uint32_t cleanupOffset) {
	const uint32_t* list = pre().extra.data() + cleanupOffset;
	if (list[0] == 0) {
		return NO_CLEANUP;
	}
	std::vector<std::pair<uint32_t, Ref>> entries;
	for (uint32_t i = 0; i < list[0]; ++i) {
		entries.emplace_back(list[1 + 2 * i], operand(list[2 + 2 * i], b));
	}
	return padFor(entries);
}

Ref FunctionConverter::simplify(Inst& ni) {
	using namespace semantics;
	const auto op = static_cast<Op>(ni.op);
	const auto type = static_cast<TypeId>(ni.type);
	auto isC = [&](Ref r) {
		return isConstRef(r);
	};
	auto bitsOf = [&](Ref r) {
		return out().consts.record(r).bits;
	};
	auto make = [&](uint64_t bits, TypeId t, std::initializer_list<Ref> from) -> Ref {
		auto provenance = Provenance::Literal;
		for (auto r : from) {
			if (isC(r)) {
				provenance = leastTrusted(provenance, static_cast<Provenance>(out().consts.record(r).provenance));
			}
		}
		return out().consts.intern(t, provenance, bits);
	};
	const bool integer = type == TypeId::I8 || type == TypeId::I16 || type == TypeId::I32 || type == TypeId::I64;
	const bool isBool = type == TypeId::Bool;

	switch (op) {
	case Op::Add:
	case Op::Sub:
	case Op::Mul:
	case Op::Div:
	case Op::Rem:
	case Op::And:
	case Op::Or:
	case Op::BAnd:
	case Op::BOr:
	case Op::BXor:
	case Op::Shl:
	case Op::Shr: {
		if (type == TypeId::Ptr) {
			return NONE;
		}
		if (isC(ni.a) && isC(ni.b)) {
			if (auto r = evalBinary(op, ni.flags, type, bitsOf(ni.a), bitsOf(ni.b))) {
				return make(*r, type, {ni.a, ni.b});
			}
			return NONE; // a division that throws stays
		}
		if (op == Op::Div || op == Op::Rem) {
			// a divisor that makes every division safe needs no check
			if (!isFloatType(type) && (ni.flags & 2) && isC(ni.b) &&
			    divisorIsAlwaysSafe(ni.flags & 1, type, bitsOf(ni.b))) {
				ni.flags &= ~uint8_t(2);
				++stats().uncheckedDivisions;
			}
			return NONE;
		}
		if (!integer && !isBool) {
			return NONE; // floats: no algebra (IEEE keeps x + 0.0 and x * 1.0 apart from x for -0.0 and NaN payloads)
		}
		const bool aZero = isC(ni.a) && canon(bitsOf(ni.a), type) == 0;
		const bool bZero = isC(ni.b) && canon(bitsOf(ni.b), type) == 0;
		const bool aOne = isC(ni.a) && canon(bitsOf(ni.a), type) == 1;
		const bool bOne = isC(ni.b) && canon(bitsOf(ni.b), type) == 1;
		const bool aOnes = isC(ni.a) && canon(bitsOf(ni.a), type) == mask(type);
		const bool bOnes = isC(ni.b) && canon(bitsOf(ni.b), type) == mask(type);
		switch (op) {
		case Op::Add:
			return bZero ? ni.a : aZero ? ni.b : NONE;
		case Op::Sub:
			return bZero ? ni.a : NONE;
		case Op::Mul:
			if (aZero || bZero) {
				return make(0, type, {ni.a, ni.b});
			}
			return bOne ? ni.a : aOne ? ni.b : NONE;
		case Op::BAnd:
			if (aZero || bZero) {
				return make(0, type, {ni.a, ni.b});
			}
			return bOnes ? ni.a : aOnes ? ni.b : NONE;
		case Op::BOr:
			if (aOnes || bOnes) {
				return make(mask(type), type, {ni.a, ni.b});
			}
			return bZero ? ni.a : aZero ? ni.b : NONE;
		case Op::BXor:
			return bZero ? ni.a : aZero ? ni.b : NONE;
		case Op::Shl:
		case Op::Shr:
			return bZero ? ni.a : NONE;
		case Op::And:
			if (aZero || bZero) {
				return make(0, type, {ni.a, ni.b});
			}
			return bOne ? ni.a : aOne ? ni.b : NONE;
		case Op::Or:
			if (aOne || bOne) {
				return make(1, type, {ni.a, ni.b});
			}
			return bZero ? ni.a : aZero ? ni.b : NONE;
		default:
			return NONE;
		}
	}
	case Op::FNeg:
	case Op::Not:
	case Op::BNot:
		if (isC(ni.a)) {
			if (auto r = evalUnary(op, type, bitsOf(ni.a))) {
				return make(*r, type, {ni.a});
			}
		}
		return NONE;
	case Op::Cmp: {
		const auto operandType = typeOfRef(ni.a);
		if (isC(ni.a) && isC(ni.b)) {
			return make(evalCompare(ni.flags, bitsOf(ni.a), bitsOf(ni.b), operandType), TypeId::Bool, {ni.a, ni.b});
		}
		// the same integer value on both sides (a float may be NaN)
		if (ni.a == ni.b && !isFloatType(operandType) && !isC(ni.a)) {
			const unsigned predicate = ni.flags & 7;
			return make(predicate == 0 || predicate == 3 || predicate == 5 ? 1 : 0, TypeId::Bool, {});
		}
		return NONE;
	}
	case Op::Select:
		if (isC(ni.a)) {
			return (bitsOf(ni.a) & 1) ? ni.b : ni.c;
		}
		return ni.b == ni.c ? ni.b : NONE;
	case Op::Cast: {
		if (isC(ni.a)) {
			const auto from = typeOfRef(ni.a);
			if (from != TypeId::Ptr && type != TypeId::Ptr) {
				if (auto r = evalCast(ni.flags & 15, bitsOf(ni.a), from, type)) {
					return make(*r, type, {ni.a});
				}
			}
		}
		return NONE;
	}
	default:
		return NONE;
	}
}

void FunctionConverter::convertInstruction(uint32_t b, uint32_t id, const Inst& inst) {
	const auto op = static_cast<Op>(inst.op);
	switch (op) {
	case Op::BlockArg:
		return; // the parameters were made with the entry block
	case Op::Var: {
		Ref value = operand(inst.a, b);
		if (isVariable_[id]) {
			setDef(id, b, value);
		} else {
			valueOf_[id] = value;
		}
		return;
	}
	case Op::Assign:
		setDef(inst.a, b, operand(inst.b, b));
		return;
	default:
		break;
	}
	const auto* info = coreOpInfo(inst.op);
	if (info == nullptr) {
		fail(f_, "an extension instruction");
	}
	Inst ni = inst;
	if (info->a == OperandKind::Value || info->a == OperandKind::ValueOpt) {
		ni.a = operand(inst.a, b);
	}
	if (info->b == OperandKind::Value) {
		ni.b = operand(inst.b, b);
	}
	if (info->c == OperandKind::Value) {
		ni.c = operand(inst.c, b);
	}
	if (optimize_) {
		if (Ref folded = simplify(ni); folded != NONE) {
			++stats().folded;
			if (isVariable_[id]) {
				setDef(id, b, folded);
			} else {
				valueOf_[id] = folded;
			}
			return;
		}
	}
	AvailKey key {};
	bool shareable = false;
	if (optimize_ && op != Op::Call) {
		shareable = availKey(ni, b, {}, key);
		if (shareable) {
			if (Ref hit = findAvailable(key, b); hit != NONE) {
				++stats().cse;
				if (isVariable_[id]) {
					setDef(id, b, hit);
				} else {
					valueOf_[id] = hit;
				}
				return;
			}
		}
	}
	auto t = newTemp(Kind::Inst, static_cast<TypeId>(inst.type), b);
	temps_[t].inst = ni;
	switch (op) {
	case Op::Div:
	case Op::Rem:
		if (ni.flags & 2) {
			auto pad = convertUnwind(b, inst.c);
			temps_[t].aux = static_cast<uint32_t>(calls_.size());
			calls_.push_back(CallInfo {0, 0, 0, 0, pad, pad != NO_CLEANUP});
		}
		temps_[t].inst.c = 0;
		break;
	case Op::Call:
	case Op::CallIndirect: {
		const uint32_t* list = pre().extra.data() + inst.c;
		const uint32_t argc = list[0];
		args_.clear();
		for (uint32_t i = 0; i < argc; ++i) {
			args_.push_back(operand(list[1 + i], b)); // may grow temps_ and words_
		}
		const uint32_t unwind = list[1 + argc];
		// a callee that cannot throw needs no unwind entry and no cleanup (M3.5: only where the final noUnwind is
		// clear)
		const bool cannotThrow =
		    optimize_ && op == Op::Call && inst.a < callees().size() && (callees()[inst.a].attrs & AttrNoUnwind) != 0;
		CallInfo call;
		if (unwind == NO_UNWIND) {
			call.unwind = NO_UNWIND;
		} else if (cannotThrow) {
			call.unwind = NO_UNWIND;
			++stats().droppedUnwinds;
		} else {
			auto pad = convertUnwind(b, unwind);
			call.unwind = pad;
			call.usesPad = pad != NO_CLEANUP;
		}
		const uint32_t* tail = list + 2 + argc;
		const uint32_t tailCount = op == Op::Call ? 1 : 2;
		call.list = static_cast<uint32_t>(words_.size());
		call.listCount = argc;
		words_.insert(words_.end(), args_.begin(), args_.end());
		call.tail = static_cast<uint32_t>(words_.size());
		call.tailCount = tailCount;
		words_.insert(words_.end(), tail, tail + tailCount);
		temps_[t].aux = static_cast<uint32_t>(calls_.size());
		calls_.push_back(call);
		temps_[t].inst.c = 0;
		if (optimize_ && op == Op::Call) {
			argsResolved_.clear();
			for (auto v : args_) {
				argsResolved_.push_back(resolve(v));
			}
			shareable = availKey(ni, b, argsResolved_, key);
			if (shareable) {
				if (Ref hit = findAvailable(key, b); hit != NONE) {
					// the temporary stays out of the block's body and is never emitted
					++stats().cse;
					if (isVariable_[id]) {
						setDef(id, b, hit);
					} else {
						valueOf_[id] = hit;
					}
					return;
				}
			}
		}
		break;
	}
	default:
		break;
	}
	// a store, and a call that may write, start a new memory epoch
	if (optimize_ && !shareable) {
		const bool readsOnly =
		    op == Op::Call && inst.a < callees().size() &&
		    (callees()[inst.a].attrs & (AttrNoArgWrite | AttrNoOtherWrite)) == (AttrNoArgWrite | AttrNoOtherWrite);
		if (op == Op::Store || op == Op::CallIndirect || (op == Op::Call && !readsOnly)) {
			states_[b].epochAtEnd = nextEpoch_++;
		}
	}
	if (shareable && inst.type != static_cast<uint8_t>(TypeId::Void)) {
		addAvailable(key, b, t);
	}
	bodyAll_.push_back(t);
	if (inst.type != static_cast<uint8_t>(TypeId::Void)) {
		if (isVariable_[id]) {
			setDef(id, b, t);
		} else {
			valueOf_[id] = t;
		}
	}
}

void FunctionConverter::convertTerminator(uint32_t b, const Inst& inst) {
	auto t = newTemp(Kind::Inst, TypeId::Void, b);
	temps_[t].inst = inst;
	// the successors whose edge is live: both of a conditional branch unless its condition folded
	std::array<uint32_t, 2> targets {};
	uint32_t targetCount = 0;
	uint32_t dropped = NONE;
	const auto succ = succ_[b];
	switch (static_cast<Op>(inst.op)) {
	case Op::Ret:
		temps_[t].inst.a = operand(inst.a, b);
		break;
	case Op::Br:
		targets[targetCount++] = succ[0];
		temps_[t].inst.a = inst.a;
		break;
	case Op::CondBr: {
		const uint32_t* list = pre().extra.data() + inst.c;
		auto cond = operand(inst.a, b);
		temps_[t].inst.a = cond;
		temps_[t].inst.b = inst.b;
		temps_[t].inst.c = list[0]; // the probability
		if (foldBranches_ && isConstRef(cond)) {
			const bool taken = out().consts.record(cond).bits & 1;
			const auto target = succ[taken ? 0 : 1];
			// the conditional branch becomes a jump to the taken target
			temps_[t].inst.op = static_cast<uint16_t>(Op::Br);
			temps_[t].inst.a = target;
			temps_[t].inst.b = 0;
			temps_[t].inst.c = 0;
			targets[targetCount++] = target;
			dropped = succ[taken ? 1 : 0];
			++stats().foldedBranches;
		} else {
			targets[targetCount++] = succ[0];
			targets[targetCount++] = succ[1];
		}
		break;
	}
	default:
		fail(f_, "not a terminator");
	}
	bodyAll_.push_back(t);
	// register the live edges; a block whose last incoming edge this was can be sealed
	for (uint32_t i = 0; i < targetCount; ++i) {
		auto e = makeEdge(b, targets[i]);
		if (i == 0) {
			temps_[t].aux = e; // the false edge, if any, is the next one
		}
	}
	for (uint32_t i = 0; i < targetCount; ++i) {
		if (--states_[targets[i]].pending == 0) {
			seal(targets[i]);
		}
	}
	if (dropped != NONE && --states_[dropped].pending == 0) {
		seal(dropped);
	}
}

void FunctionConverter::convertBlock(uint32_t b) {
	const auto ids = blockIds(b);
	states_[b].bodyBegin = static_cast<uint32_t>(bodyAll_.size());
	for (size_t k = 0; k + 1 < ids.size(); ++k) {
		convertInstruction(b, ids[k], insts_[ids[k]]);
	}
	convertTerminator(b, insts_[ids.back()]);
	states_[b].bodyEnd = static_cast<uint32_t>(bodyAll_.size());
}

// ---- emission ---------------------------------------------------------------------------------------------------

void FunctionConverter::appendBranchArgs(uint32_t edge) {
	const auto to = dynEdges_[edge].to;
	if (!states_[to].emitted) {
		list_.push_back(0);
		return;
	}
	const auto countAt = list_.size();
	list_.push_back(0);
	for (auto phi = states_[to].phiHead; phi != NONE; phi = phiOf(phi).nextInBlock) {
		if (fwd_[phi] != NONE || !marked_[phi]) {
			continue;
		}
		const auto ops = opsOf(phi);
		const uint32_t slot = edgeSlot_[edge];
		list_.push_back(slot < ops.size() ? finalRef(ops[slot]) : NONE);
	}
	list_[countAt] = static_cast<uint32_t>(list_.size() - countAt - 1);
}

void FunctionConverter::emit() {
	Builder builder(out());
	builder.setTrackUses(false); // nobody asks for the use counts here
	emitOrder_.clear();
	if (foldBranches_) {
		// Folded branches and dropped blocks changed the graph: a block that the static graph put into a loop may no
		// longer reach its latch and must leave the loop's range. The layout is computed again on the live edges.
		liveSucc_.start.assign(n_ + 2, 0);
		livePred_.start.assign(n_ + 2, 0);
		for (const auto& edge : dynEdges_) {
			++liveSucc_.start[edge.from + 2];
			++livePred_.start[edge.to + 2];
		}
		for (uint32_t b = 0; b < n_; ++b) {
			liveSucc_.start[b + 2] += liveSucc_.start[b + 1];
			livePred_.start[b + 2] += livePred_.start[b + 1];
		}
		liveSucc_.items.resize(dynEdges_.size());
		livePred_.items.resize(dynEdges_.size());
		for (const auto& edge : dynEdges_) {
			liveSucc_.items[liveSucc_.start[edge.from + 1]++] = edge.to;
			livePred_.items[livePred_.start[edge.to + 1]++] = edge.from;
		}
		liveSucc_.start.resize(n_ + 1);
		livePred_.start.resize(n_ + 1);
		liveCfg_.compute(n_, liveSucc_, livePred_);
		emitOrder_.assign(liveCfg_.layout.begin(), liveCfg_.layout.end());
	} else {
		for (auto b : cfg_.layout) {
			if (states_[b].visited) {
				emitOrder_.push_back(b);
			}
		}
	}
	for (uint32_t i = 0; i < emitOrder_.size(); ++i) {
		layoutIndex_[emitOrder_[i]] = i;
		states_[emitOrder_[i]].emitted = true;
	}
	auto bodyOf = [&](uint32_t b) {
		return std::span<const uint32_t>(bodyAll_.data() + states_[b].bodyBegin,
		                                 states_[b].bodyEnd - states_[b].bodyBegin);
	};

	// ---- the refold sweep: phis that became trivial when their loop was sealed leave instructions behind that now
	// have constant or equal operands; a few sweeps over the emitted blocks fold them. Without a forwarded phi no
	// operand resolves to anything but what the instruction saw when it was made, so there is nothing to refold; and
	// an instruction whose operands still resolve to themselves simplifies to what it simplified to then.
	if (optimize_ && phisForwarded_ != 0) {
		for (uint32_t sweep = 0; sweep < MaxRefoldSweeps; ++sweep) {
			uint32_t changed = 0;
			for (auto b : emitOrder_) {
				for (auto t : bodyOf(b)) {
					auto& temp = temps_[t];
					if (fwd_[t] != NONE || temp.kind != Kind::Inst) {
						continue;
					}
					const auto* info = coreOpInfo(temp.inst.op);
					if (!(info->flags & OpPure)) {
						continue;
					}
					Inst ni = temp.inst;
					if (info->a == OperandKind::Value || info->a == OperandKind::ValueOpt) {
						ni.a = resolve(ni.a);
					}
					if (info->b == OperandKind::Value) {
						ni.b = resolve(ni.b);
					}
					if (info->c == OperandKind::Value) {
						ni.c = resolve(ni.c);
					}
					if (ni.a == temp.inst.a && ni.b == temp.inst.b && ni.c == temp.inst.c) {
						continue;
					}
					if (Ref folded = simplify(ni); folded != NONE && folded != t) {
						fwd_[t] = folded;
						++changed;
						++stats().refoldedInsts;
					} else {
						temp.inst = ni;
					}
				}
			}
			++stats().refoldSweeps;
			if (changed == 0) {
				break;
			}
		}
	}

	// ---- the dead-value sweep (§3.3): mark from the roots, drop what is not reached --------------------------------
	marked_.assign(temps_.size(), 0);
	markWork_.clear();
	auto mark = [&](Ref r) {
		r = resolve(r);
		if (isTemp(r) && !marked_[r]) {
			marked_[r] = 1;
			markWork_.push_back(r);
		}
	};
	if (optimize_) {
		for (auto b : emitOrder_) {
			for (auto p = states_[b].phiHead; p != NONE; p = phiOf(p).nextInBlock) {
				if (temps_[p].kind == Kind::Param) {
					mark(p);
				}
			}
			for (auto t : bodyOf(b)) {
				const auto& inst = temps_[t].inst;
				const auto* info = coreOpInfo(inst.op);
				const bool checkedDivision =
				    (inst.op == static_cast<uint16_t>(Op::Div) || inst.op == static_cast<uint16_t>(Op::Rem)) &&
				    (inst.flags & 2);
				if ((info->flags & (OpTerminator | OpEffect)) || checkedDivision) {
					mark(t);
				}
			}
		}
		while (!markWork_.empty()) {
			auto x = markWork_.back();
			markWork_.pop_back();
			const auto& temp = temps_[x];
			if (temp.kind == Kind::Phi) {
				for (auto op : opsOf(x)) {
					mark(op);
				}
				continue;
			}
			if (temp.kind != Kind::Inst) {
				continue;
			}
			const auto* info = coreOpInfo(temp.inst.op);
			if (info->a == OperandKind::Value || info->a == OperandKind::ValueOpt) {
				mark(temp.inst.a);
			}
			if (info->b == OperandKind::Value) {
				mark(temp.inst.b);
			}
			if (info->c == OperandKind::Value) {
				mark(temp.inst.c);
			}
			if (hasCallInfo(temp)) {
				const auto& call = calls_[temp.aux];
				for (uint32_t i = 0; i < call.listCount; ++i) {
					mark(words_[call.list + i]);
				}
				if (call.usesPad) {
					for (const auto& entry : pads_[call.unwind].entries) {
						mark(entry.second);
					}
				}
			}
		}
	} else {
		std::fill(marked_.begin(), marked_.end(), 1);
	}

	// the ids of the live phis and instructions
	finalId_.assign(temps_.size(), NONE);
	uint32_t next = 0;
	for (auto b : emitOrder_) {
		uint32_t live = 0;
		for (auto phi = states_[b].phiHead; phi != NONE; phi = phiOf(phi).nextInBlock) {
			if (fwd_[phi] != NONE) {
				continue;
			}
			if (!marked_[phi]) {
				++stats().deadValues;
				continue;
			}
			++live;
			finalId_[phi] = next++;
		}
		states_[b].finalPhis = live;
		for (auto t : bodyOf(b)) {
			if (!marked_[t]) {
				++stats().deadValues;
				continue;
			}
			finalId_[t] = next++;
		}
	}
	const uint32_t blockCount = static_cast<uint32_t>(emitOrder_.size());

	// landing pads are numbered by their first use among the instructions that are emitted
	padBlock_.assign(pads_.size(), NONE);
	padOrder_.clear();
	auto padOf = [&](uint32_t pad) {
		if (padBlock_[pad] == NONE) {
			padBlock_[pad] = blockCount + static_cast<uint32_t>(padOrder_.size());
			padOrder_.push_back(pad);
		}
		return padBlock_[pad];
	};

	auto signature = rec_->signature;
	builder.beginFunction(rec_->nameId, signature, static_cast<Visibility>(rec_->visibility), Level::SSA, rec_->attrs);
	for (const auto& a : pre().functionAllocas(f_)) {
		builder.addAlloca(a.size, a.align);
	}
	for (uint32_t li = 0; li < emitOrder_.size(); ++li) {
		const auto b = emitOrder_[li];
		builder.beginBlock(static_cast<uint16_t>(states_[b].finalPhis), 0);
		++stats().blocks;
		uint32_t position = 0;
		for (auto phi = states_[b].phiHead; phi != NONE; phi = phiOf(phi).nextInBlock) {
			if (fwd_[phi] != NONE || !marked_[phi]) {
				continue;
			}
			Ref id = builder.emit(Op::BlockArg, temps_[phi].type, 0, position++);
			assert(id == finalId_[phi]);
			(void) id;
			if (temps_[phi].kind == Kind::Phi) {
				++stats().phis;
			}
		}
		for (auto t : bodyOf(b)) {
			if (!marked_[t]) {
				continue;
			}
			auto& temp = temps_[t];
			Inst inst = temp.inst;
			const auto op = static_cast<Op>(inst.op);
			const auto* info = coreOpInfo(inst.op);
			if (info->a == OperandKind::Value || info->a == OperandKind::ValueOpt) {
				inst.a = finalRef(inst.a);
			}
			if (info->b == OperandKind::Value) {
				inst.b = finalRef(inst.b);
			}
			if (info->c == OperandKind::Value) {
				inst.c = finalRef(inst.c);
			}
			auto unwindWord = [&](const Temp& tmp) {
				const auto& call = calls_[tmp.aux];
				return call.usesPad ? padOf(call.unwind) : call.unwind;
			};
			switch (op) {
			case Op::Div:
			case Op::Rem:
				if (inst.flags & 2) {
					inst.c = unwindWord(temp);
				}
				break;
			case Op::Call:
			case Op::CallIndirect: {
				const auto& call = calls_[temp.aux];
				list_.clear();
				list_.push_back(call.listCount);
				for (uint32_t i = 0; i < call.listCount; ++i) {
					list_.push_back(finalRef(words_[call.list + i]));
				}
				list_.push_back(unwindWord(temp));
				list_.insert(list_.end(), words_.begin() + call.tail, words_.begin() + call.tail + call.tailCount);
				inst.c = out().addExtra(list_);
				break;
			}
			case Op::Br: {
				auto target = layoutIndex_[inst.a];
				list_.clear();
				appendBranchArgs(temp.aux);
				inst.a = target;
				inst.c = out().addExtra(list_);
				break;
			}
			case Op::CondBr: {
				auto probability = inst.c;
				list_.clear();
				list_.push_back(probability);
				appendBranchArgs(temp.aux); // [prob, trueArgc, trueArgs...]
				list_.push_back(layoutIndex_[succ_[b][1]]);
				appendBranchArgs(temp.aux + 1);
				inst.b = layoutIndex_[inst.b];
				inst.c = out().addExtra(list_);
				break;
			}
			default:
				break;
			}
			Ref id = builder.emit(inst);
			assert(id == finalId_[t]);
			(void) id;
			++stats().instructions;
		}
	}
	// landing pads, in the order of their first reference
	for (auto padIndex : padOrder_) {
		const auto& pad = pads_[padIndex];
		builder.beginBlock(0, BLOCK_LANDING_PAD);
		++stats().blocks;
		++stats().landingPads;
		for (const auto& [callee, address] : pad.entries) {
			uint32_t list[] = {1, finalRef(address), NO_UNWIND, NONE};
			builder.emit(Op::Call, TypeId::Void, 0, callee, 0, out().addExtra(list));
			++stats().instructions;
		}
		builder.emit(Op::Resume, TypeId::Void, 0);
		++stats().instructions;
	}
	builder.endFunction();
	++stats().functions;
}

void FunctionConverter::reset(uint32_t function) {
	f_ = function;
	rec_ = &(*functions_)[function];
	insts_ = pre().functionInsts(function);
	blocks_ = pre().functionBlocks(function);
	foldBranches_ = false;
	irreducibleHere_ = 0;
	phisForwarded_ = 0;
	nextEpoch_ = 1;
	emitOrder_.clear();
	temps_.clear();
	fwd_.clear();
	phiInfo_.clear();
	calls_.clear();
	words_.clear();
	bodyAll_.clear();
	pads_.clear();
	padIndex_.clear();
	dynEdges_.clear();
	edgeSlot_.clear();
	frames_.clear();
	chainWords_.clear();
	currentDef_.reset();
	availEntries_.clear();
	availHead_.assign(16, NONE);
}

void FunctionConverter::run(uint32_t function) {
	reset(function);
	analyze();
	buildCfg();
	orderBlocks();

	states_.assign(n_, BlockState {});
	incomingEdges_.assign(preds_.items.size(), 0);
	for (auto b : cfg_.rpo) {
		for (auto to : succ_[b]) {
			++states_[to].pending;
		}
	}

	// the parameters are variables of the entry block like any other
	const auto ids = blockIds(0);
	for (auto id : ids) {
		const auto& inst = insts_[id];
		if (inst.op != static_cast<uint16_t>(Op::BlockArg)) {
			break;
		}
		auto t = newTemp(Kind::Param, static_cast<TypeId>(inst.type), 0);
		linkPhi(0, t);
		if (isVariable_[id]) {
			setDef(id, 0, t);
		} else {
			valueOf_[id] = t;
		}
	}
	states_[0].sealed = true;

	foldBranches_ = optimize_ && irreducibleHere_ == 0;
	for (auto b : cfg_.layout) {
		if (foldBranches_ && b != 0 && states_[b].incomingCount == 0) {
			// no live edge enters the block: it is never emitted, and its successors no longer wait for it
			++stats().deadBlocks;
			for (auto to : succ_[b]) {
				if (--states_[to].pending == 0) {
					seal(to);
				}
			}
			continue;
		}
		enterBlockForCse(b);
		states_[b].visited = true;
		convertBlock(b);
	}
	for (auto b : cfg_.layout) {
		if (states_[b].visited && !states_[b].sealed) {
			fail(f_, "block ^b" + std::to_string(b) + " was never sealed");
		}
	}
	emit();
}

} // namespace

Module convert(const Module& preSSA, const ConversionOptions& options, ConversionStatistics* statistics) {
	ConversionStatistics local;
	auto& stats = statistics != nullptr ? *statistics : local;
	stats = {};

	// the attribute pre-pass runs over all bodies before any function is converted (03 §3.5); it stores its bits in
	// copies of the function and callee tables, so that the input is not copied as a whole
	auto functions = preSSA.functions;
	auto callees = preSSA.callees;
	if (options.inferAttributes && options.optimize) {
		auto inferred = inferAttributes(preSSA, functions, callees);
		stats.inferredFunctions = inferred.functions;
		stats.cyclicFunctions = inferred.cyclic;
		stats.inferredNoUnwind = inferred.noUnwind;
		stats.inferredWillReturn = inferred.willReturn;
	}
	// the result shares the tables of the input and writes the bodies anew
	Module out = preSSA.copyTables();
	out.callees = callees;
	// the signatures keep their parameter types in `extra`; the instruction lists of the pre-SSA functions do not
	// come along
	for (auto& sig : out.signatures) {
		const auto words = std::span<const uint32_t>(preSSA.extra.data() + sig.paramTypes, sig.paramCount);
		sig.paramTypes = out.addExtra(words);
	}
	// the tables of the converter are reused by the next conversion of this thread, unless a big function made them big
	static thread_local FunctionConverter converter;
	constexpr size_t RetainedBytesLimit = size_t(8) << 20;
	converter.bind(preSSA, functions, callees, out, stats, options.optimize);
	try {
		for (uint32_t f = 0; f < preSSA.functions.size(); ++f) {
			if (preSSA.functions[f].level != static_cast<uint8_t>(Level::PreSSA)) {
				throw std::runtime_error("conversion of function #" + std::to_string(f) +
				                         ": it is not a pre-SSA function");
			}
			converter.run(f);
		}
	} catch (...) {
		converter.release();
		throw;
	}
	if (converter.retainedBytes() > RetainedBytesLimit) {
		converter.release();
	}
	out.rebuildIndexes();
	return out;
}

} // namespace nautilus::conversion

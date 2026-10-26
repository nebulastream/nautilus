#include "nautilus/conversion/Conversion.hpp"
#include "nautilus/nir/Builder.hpp"
#include "nautilus/nir/Operands.hpp"
#include <algorithm>
#include <cassert>
#include <functional>
#include <map>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

namespace nautilus::conversion {

using namespace nautilus::nir;

namespace {

[[noreturn]] void fail(uint32_t function, const std::string& message) {
	throw std::runtime_error("conversion of function #" + std::to_string(function) + ": " + message);
}

/// One function: layout (§3.1), Braun's SSA construction with an explicit stack (§3 step 1) and the emission into the
/// output module. Values during the conversion are `Ref`s: a temporary id (an instruction or block parameter this
/// converter made) or a constant reference. Temporaries become instruction ids when the function is emitted.
class FunctionConverter {
public:
	FunctionConverter(const Module& pre, uint32_t function, Module& out, ConversionStatistics& stats)
	    : pre_(pre), f_(function), out_(out), stats_(stats), rec_(pre.functions[function]),
	      insts_(pre.functionInsts(function)), blocks_(pre.functionBlocks(function)) {
	}

	void run();

private:
	// ---- the pre-SSA function ---------------------------------------------------------------------------------

	struct Edge {
		uint32_t from, to;
	};

	std::span<const uint32_t> blockIds(uint32_t b) const {
		return {pre_.extra.data() + blocks_[b].first, blocks_[b].count};
	}
	const Inst& terminator(uint32_t b) const {
		return insts_[blockIds(b).back()];
	}
	void analyze();
	void buildCfg();
	void orderBlocks();

	// ---- SSA construction --------------------------------------------------------------------------------------

	enum class Kind : uint8_t { Inst, Phi, Param };

	struct Temp {
		Kind kind = Kind::Inst;
		Inst inst {};                // Inst: the instruction, operands as Refs
		std::vector<uint32_t> list;  // Inst: the values of a call's argument list
		std::vector<uint32_t> tail;  // Inst: the words of a call's list after the unwind entry
		uint32_t unwind = NO_UNWIND; // Inst: NO_UNWIND, NO_CLEANUP or a pad (padBase + index, resolved when emitted)
		bool usesPad = false;
		uint32_t variable = NONE;  // Phi: the variable
		uint32_t block = NONE;     // Phi, Param: the block
		std::vector<uint32_t> ops; // Phi: one value per incoming edge, in registration order
		TypeId type = TypeId::Void;
		uint32_t edgeTrue = NONE, edgeFalse = NONE; // terminators: the edges
	};

	struct Pad {
		std::vector<std::pair<uint32_t, Ref>> entries; // destructor callee, object address
	};

	uint32_t newTemp(Kind kind, TypeId type, uint32_t block) {
		temps_.push_back(Temp {});
		auto& t = temps_.back();
		t.kind = kind;
		t.type = type;
		t.block = block;
		fwd_.push_back(NONE);
		phiUsers_.resize(temps_.size());
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
	Ref getDef(uint32_t variable, uint32_t block) const {
		auto& defs = currentDef_[block];
		auto it = defs.find(variable);
		return it == defs.end() ? NONE : resolve(it->second);
	}
	void setDef(uint32_t variable, uint32_t block, Ref value) {
		currentDef_[block][variable] = value;
	}

	Ref operand(Ref r, uint32_t block);
	Ref readVariable(uint32_t variable, uint32_t block);
	struct Frame {
		uint32_t phi, block, next;
		std::vector<uint32_t> chain;
	};
	Ref startRead(uint32_t variable, uint32_t block, std::vector<Frame>& stack);
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

	// ---- emission ----------------------------------------------------------------------------------------------

	void emit();
	Ref finalRef(Ref r) const {
		r = resolve(r);
		return isTemp(r) ? finalId_[r] : r;
	}

	const Module& pre_;
	uint32_t f_;
	Module& out_;
	ConversionStatistics& stats_;
	const FunctionRecord& rec_;
	std::span<const Inst> insts_;
	std::span<const BlockRecord> blocks_;

	uint32_t n_ = 0; // blocks
	std::vector<uint8_t> isVariable_;
	std::vector<Edge> edges_;
	std::vector<std::vector<uint32_t>> succEdgesStatic_; // per block: the successors, in terminator order
	std::vector<std::vector<uint32_t>> predsStatic_;
	std::vector<uint32_t> rpo_, rpoIndex_;
	std::vector<uint32_t> layout_, layoutIndex_;

	std::vector<Temp> temps_;
	std::vector<uint32_t> fwd_;
	std::vector<std::vector<uint32_t>> phis_; // per block, in creation order (block 0: the parameters first)
	std::vector<std::vector<uint32_t>> body_; // per block: the temporaries of the instructions, terminator last
	std::vector<std::unordered_map<uint32_t, Ref>> currentDef_;
	std::vector<uint8_t> sealed_;
	std::vector<uint32_t> pending_;
	std::vector<std::vector<uint32_t>> incoming_; // per block: dynamic edge ids in registration order
	std::vector<std::vector<uint32_t>> incomplete_;
	std::vector<Ref> valueOf_; // pre-SSA instruction id -> value, for ids that are never assigned
	std::vector<std::vector<uint32_t>> phiUsers_;
	std::vector<Pad> pads_;
	std::map<std::vector<std::pair<uint32_t, Ref>>, uint32_t> padIndex_;
	std::vector<Edge> dynEdges_;

	std::vector<uint32_t> finalId_;
};

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
	n_ = rec_.blockCount;
	succEdgesStatic_.assign(n_, {});
	predsStatic_.assign(n_, {});
	for (uint32_t b = 0; b < n_; ++b) {
		if (blocks_[b].count == 0) {
			fail(f_, "an empty block");
		}
		const auto& term = terminator(b);
		auto addSucc = [&](uint32_t to) {
			if (to >= n_) {
				fail(f_, "a branch to a block that does not exist");
			}
			succEdgesStatic_[b].push_back(to);
		};
		switch (static_cast<Op>(term.op)) {
		case Op::Br:
			addSucc(term.a);
			break;
		case Op::CondBr: {
			const uint32_t* list = pre_.extra.data() + term.c;
			addSucc(term.b);
			addSucc(list[1 + list[1] + 1]); // [prob, trueArgc, trueArgs..., falseBlk, ...]
			break;
		}
		case Op::Ret:
			break;
		default:
			fail(f_, "a block does not end in a terminator");
		}
	}
	for (uint32_t b = 0; b < n_; ++b) {
		for (auto to : succEdgesStatic_[b]) {
			predsStatic_[to].push_back(b);
		}
	}
}

void FunctionConverter::orderBlocks() {
	// reverse postorder from the entry block
	std::vector<uint8_t> state(n_, 0);
	std::vector<uint32_t> post;
	struct Frame {
		uint32_t block;
		uint32_t next;
	};
	std::vector<Frame> stack {{0, 0}};
	state[0] = 1;
	while (!stack.empty()) {
		auto& top = stack.back();
		const auto& succ = succEdgesStatic_[top.block];
		if (top.next < succ.size()) {
			auto next = succ[top.next++];
			if (state[next] == 0) {
				state[next] = 1;
				stack.push_back({next, 0});
			}
		} else {
			post.push_back(top.block);
			stack.pop_back();
		}
	}
	rpo_.assign(post.rbegin(), post.rend());
	rpoIndex_.assign(n_, NONE);
	for (uint32_t i = 0; i < rpo_.size(); ++i) {
		rpoIndex_[rpo_[i]] = i;
	}
	if (!predsStatic_[0].empty()) {
		for (auto p : predsStatic_[0]) {
			if (rpoIndex_[p] != NONE) {
				fail(f_, "the entry block has a predecessor");
			}
		}
	}

	// dominators (Cooper, Harvey, Kennedy) over the reachable blocks, by reverse postorder index
	std::vector<uint32_t> idom(n_, NONE);
	idom[0] = 0;
	auto intersect = [&](uint32_t a, uint32_t b) {
		while (a != b) {
			while (rpoIndex_[a] > rpoIndex_[b]) {
				a = idom[a];
			}
			while (rpoIndex_[b] > rpoIndex_[a]) {
				b = idom[b];
			}
		}
		return a;
	};
	for (bool changed = true; changed;) {
		changed = false;
		for (size_t i = 1; i < rpo_.size(); ++i) {
			auto b = rpo_[i];
			uint32_t candidate = NONE;
			for (auto p : predsStatic_[b]) {
				if (rpoIndex_[p] == NONE || idom[p] == NONE) {
					continue;
				}
				candidate = candidate == NONE ? p : intersect(p, candidate);
			}
			if (candidate != idom[b]) {
				idom[b] = candidate;
				changed = true;
			}
		}
	}
	auto dominates = [&](uint32_t a, uint32_t b) {
		while (true) {
			if (a == b) {
				return true;
			}
			if (b == 0 || idom[b] == NONE) {
				return false;
			}
			b = idom[b];
		}
	};

	// back edges (the target dominates the source) head the natural loops; retreating edges whose target does not
	// dominate the source belong to irreducible regions, which keep the plain reverse postorder
	std::vector<std::vector<uint32_t>> latches(n_);
	for (auto s : rpo_) {
		for (auto t : succEdgesStatic_[s]) {
			if (rpoIndex_[t] > rpoIndex_[s]) {
				continue;
			}
			if (dominates(t, s)) {
				latches[t].push_back(s);
			} else {
				++stats_.irreducibleRegions;
			}
		}
	}
	std::vector<std::vector<uint32_t>> loopBody(n_); // per header: the other blocks of the loop, in reverse postorder
	for (uint32_t h = 0; h < n_; ++h) {
		if (latches[h].empty()) {
			continue;
		}
		++stats_.loops;
		std::vector<uint8_t> inLoop(n_, 0);
		inLoop[h] = 1;
		std::vector<uint32_t> work;
		for (auto l : latches[h]) {
			if (!inLoop[l]) {
				inLoop[l] = 1;
				work.push_back(l);
			}
		}
		while (!work.empty()) {
			auto b = work.back();
			work.pop_back();
			for (auto p : predsStatic_[b]) {
				if (rpoIndex_[p] != NONE && !inLoop[p]) {
					inLoop[p] = 1;
					work.push_back(p);
				}
			}
		}
		for (auto b : rpo_) {
			if (inLoop[b] && b != h) {
				loopBody[h].push_back(b);
			}
		}
	}

	// layout: reverse postorder, with each natural loop placed contiguously where its header is
	layout_.clear();
	std::vector<uint8_t> placed(n_, 0);
	std::function<void(const std::vector<uint32_t>&)> place = [&](const std::vector<uint32_t>& range) {
		for (auto b : range) {
			if (placed[b]) {
				continue;
			}
			placed[b] = 1;
			layout_.push_back(b);
			if (!latches[b].empty()) {
				place(loopBody[b]);
			}
		}
	};
	place(rpo_);
	layoutIndex_.assign(n_, NONE);
	for (uint32_t i = 0; i < layout_.size(); ++i) {
		layoutIndex_[layout_[i]] = i;
	}
}

// ---- Braun's construction -------------------------------------------------------------------------------------

uint32_t FunctionConverter::newPhi(uint32_t variable, uint32_t block) {
	auto type = static_cast<TypeId>(insts_[variable].type);
	auto id = newTemp(Kind::Phi, type, block);
	temps_[id].variable = variable;
	phis_[block].push_back(id);
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

Ref FunctionConverter::startRead(uint32_t variable, uint32_t block, std::vector<Frame>& stack) {
	std::vector<uint32_t> chain;
	Ref value = NONE;
	for (;;) {
		if (auto d = getDef(variable, block); d != NONE) {
			value = d;
			break;
		}
		if (!sealed_[block]) {
			auto phi = newPhi(variable, block);
			incomplete_[block].push_back(phi);
			setDef(variable, block, phi);
			value = phi;
			break;
		}
		const auto& inc = incoming_[block];
		if (inc.size() == 1) {
			chain.push_back(block);
			block = dynEdges_[inc[0]].from;
			continue;
		}
		if (inc.empty()) {
			fail(f_, "%" + std::to_string(variable) + " is read before it is defined");
		}
		auto phi = newPhi(variable, block);
		setDef(variable, block, phi);
		stack.push_back(Frame {phi, block, 0, std::move(chain)});
		return NONE;
	}
	for (auto b : chain) {
		setDef(variable, b, value);
	}
	return value;
}

Ref FunctionConverter::readVariable(uint32_t variable, uint32_t block) {
	std::vector<Frame> stack;
	if (Ref v = startRead(variable, block, stack); v != NONE) {
		return v;
	}
	Ref delivered = NONE;
	while (!stack.empty()) {
		if (delivered != NONE) {
			auto& parent = stack.back();
			temps_[parent.phi].ops.push_back(delivered);
			if (isTemp(delivered) && temps_[delivered].kind == Kind::Phi) {
				phiUsers_[delivered].push_back(parent.phi);
			}
			++parent.next;
			delivered = NONE;
		}
		auto& f = stack.back();
		const auto count = incoming_[f.block].size();
		if (f.next < count) {
			const auto from = dynEdges_[incoming_[f.block][f.next]].from;
			const auto var = temps_[f.phi].variable;
			if (Ref v = startRead(var, from, stack); v != NONE) {
				delivered = v;
			}
			continue;
		}
		Frame done = std::move(f);
		stack.pop_back();
		const auto var = temps_[done.phi].variable;
		Ref value = tryRemoveTrivialPhi(done.phi);
		setDef(var, done.block, value);
		for (auto b : done.chain) {
			setDef(var, b, value);
		}
		if (stack.empty()) {
			return resolve(value);
		}
		delivered = value;
	}
	fail(f_, "unreachable");
}

Ref FunctionConverter::tryRemoveTrivialPhi(uint32_t phi) {
	std::vector<uint32_t> work {phi};
	Ref result = phi;
	bool first = true;
	while (!work.empty()) {
		auto p = work.back();
		work.pop_back();
		if (fwd_[p] != NONE || temps_[p].kind != Kind::Phi) {
			continue;
		}
		Ref same = NONE;
		bool trivial = true;
		for (auto op : temps_[p].ops) {
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
		++stats_.removedPhis;
		if (first) {
			result = same;
		}
		for (auto user : phiUsers_[p]) {
			if (user != p) {
				work.push_back(user);
			}
		}
		if (isTemp(same) && temps_[same].kind == Kind::Phi) {
			for (auto user : phiUsers_[p]) {
				phiUsers_[same].push_back(user);
			}
		}
		first = false;
	}
	return resolve(result);
}

void FunctionConverter::addPhiOperands(uint32_t phi) {
	// the operands, one per incoming edge, in the order the edges were registered
	const auto block = temps_[phi].block;
	const auto variable = temps_[phi].variable;
	temps_[phi].ops.clear();
	for (size_t i = 0; i < incoming_[block].size(); ++i) {
		auto from = dynEdges_[incoming_[block][i]].from;
		Ref v = readVariable(variable, from);
		temps_[phi].ops.push_back(v);
		if (isTemp(v) && temps_[v].kind == Kind::Phi) {
			phiUsers_[v].push_back(phi);
		}
	}
	tryRemoveTrivialPhi(phi);
}

void FunctionConverter::seal(uint32_t block) {
	sealed_[block] = 1;
	auto incomplete = std::move(incomplete_[block]);
	incomplete_[block].clear();
	for (auto phi : incomplete) {
		addPhiOperands(phi);
	}
}

// ---- instructions -----------------------------------------------------------------------------------------------

uint32_t FunctionConverter::makeEdge(uint32_t from, uint32_t to) {
	dynEdges_.push_back({from, to});
	auto id = static_cast<uint32_t>(dynEdges_.size() - 1);
	incoming_[to].push_back(id);
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
	const uint32_t* list = pre_.extra.data() + cleanupOffset;
	if (list[0] == 0) {
		return NO_CLEANUP;
	}
	std::vector<std::pair<uint32_t, Ref>> entries;
	for (uint32_t i = 0; i < list[0]; ++i) {
		entries.emplace_back(list[1 + 2 * i], operand(list[2 + 2 * i], b));
	}
	return padFor(entries);
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
	auto t = newTemp(Kind::Inst, static_cast<TypeId>(inst.type), b);
	Temp& temp = temps_[t];
	temp.inst = inst;
	const auto* info = coreOpInfo(inst.op);
	if (info == nullptr) {
		fail(f_, "an extension instruction");
	}
	if (info->a == OperandKind::Value || info->a == OperandKind::ValueOpt) {
		temps_[t].inst.a = operand(inst.a, b);
	}
	if (info->b == OperandKind::Value) {
		temps_[t].inst.b = operand(inst.b, b);
	}
	if (info->c == OperandKind::Value) {
		temps_[t].inst.c = operand(inst.c, b);
	}
	switch (op) {
	case Op::Div:
	case Op::Rem:
		if (inst.flags & 2) {
			auto pad = convertUnwind(b, inst.c);
			temps_[t].unwind = pad;
			temps_[t].usesPad = pad != NO_CLEANUP;
		}
		temps_[t].inst.c = 0;
		break;
	case Op::Call:
	case Op::CallIndirect: {
		const uint32_t* list = pre_.extra.data() + inst.c;
		const uint32_t argc = list[0];
		for (uint32_t i = 0; i < argc; ++i) {
			auto value = operand(list[1 + i], b); // may grow temps_
			temps_[t].list.push_back(value);
		}
		const uint32_t unwind = list[1 + argc];
		if (unwind == NO_UNWIND) {
			temps_[t].unwind = NO_UNWIND;
		} else {
			auto pad = convertUnwind(b, unwind);
			temps_[t].unwind = pad;
			temps_[t].usesPad = pad != NO_CLEANUP;
		}
		temps_[t].tail.assign(list + 2 + argc, list + (op == Op::Call ? 3 : 4) + argc);
		temps_[t].inst.c = 0;
		break;
	}
	default:
		break;
	}
	body_[b].push_back(t);
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
	switch (static_cast<Op>(inst.op)) {
	case Op::Ret:
		temps_[t].inst.a = operand(inst.a, b);
		break;
	case Op::Br:
		temps_[t].inst.a = inst.a;
		break;
	case Op::CondBr: {
		const uint32_t* list = pre_.extra.data() + inst.c;
		temps_[t].inst.a = operand(inst.a, b);
		temps_[t].inst.b = inst.b;
		temps_[t].inst.c = list[0]; // the probability
		break;
	}
	default:
		fail(f_, "not a terminator");
	}
	body_[b].push_back(t);
	// register the edges; a block whose last incoming edge this was can be sealed
	std::vector<uint32_t> targets;
	targets.push_back(succEdgesStatic_[b].empty() ? NONE : succEdgesStatic_[b][0]);
	if (static_cast<Op>(inst.op) == Op::CondBr) {
		targets.push_back(succEdgesStatic_[b][1]);
	}
	for (size_t i = 0; i < targets.size(); ++i) {
		if (targets[i] == NONE) {
			continue;
		}
		auto e = makeEdge(b, targets[i]);
		(i == 0 ? temps_[t].edgeTrue : temps_[t].edgeFalse) = e;
	}
	for (auto to : targets) {
		if (to == NONE) {
			continue;
		}
		if (--pending_[to] == 0) {
			seal(to);
		}
	}
}

void FunctionConverter::convertBlock(uint32_t b) {
	const auto ids = blockIds(b);
	for (size_t k = 0; k + 1 < ids.size(); ++k) {
		convertInstruction(b, ids[k], insts_[ids[k]]);
	}
	convertTerminator(b, insts_[ids.back()]);
}

// ---- emission ---------------------------------------------------------------------------------------------------

void FunctionConverter::emit() {
	Builder builder(out_);
	// the final phis of every block, and the ids
	std::vector<std::vector<uint32_t>> finalPhis(n_);
	finalId_.assign(temps_.size(), NONE);
	uint32_t next = 0;
	for (auto b : layout_) {
		for (auto phi : phis_[b]) {
			if (fwd_[phi] != NONE) {
				continue;
			}
			finalPhis[b].push_back(phi);
			finalId_[phi] = next++;
		}
		for (auto t : body_[b]) {
			finalId_[t] = next++;
		}
	}
	const uint32_t blockCount = static_cast<uint32_t>(layout_.size());

	// slot of an incoming edge in its target's phis
	std::vector<uint32_t> edgeSlot(dynEdges_.size(), 0);
	for (uint32_t b = 0; b < n_; ++b) {
		for (uint32_t i = 0; i < incoming_[b].size(); ++i) {
			edgeSlot[incoming_[b][i]] = i;
		}
	}
	auto branchArgs = [&](uint32_t edge) {
		std::vector<uint32_t> args;
		const auto to = dynEdges_[edge].to;
		for (auto phi : finalPhis[to]) {
			const auto& ops = temps_[phi].ops;
			uint32_t slot = edgeSlot[edge];
			args.push_back(slot < ops.size() ? finalRef(ops[slot]) : NONE);
		}
		return args;
	};

	auto signature = rec_.signature;
	builder.beginFunction(rec_.nameId, signature, static_cast<Visibility>(rec_.visibility), Level::SSA, rec_.attrs);
	for (const auto& a : pre_.functionAllocas(f_)) {
		builder.addAlloca(a.size, a.align);
	}
	for (uint32_t li = 0; li < layout_.size(); ++li) {
		const auto b = layout_[li];
		builder.beginBlock(static_cast<uint16_t>(finalPhis[b].size()), 0);
		++stats_.blocks;
		uint32_t position = 0;
		for (auto phi : finalPhis[b]) {
			Ref id = builder.emit(Op::BlockArg, temps_[phi].type, 0, position++);
			assert(id == finalId_[phi]);
			(void) id;
			if (temps_[phi].kind == Kind::Phi) {
				++stats_.phis;
			}
		}
		for (auto t : body_[b]) {
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
				if (!tmp.usesPad) {
					return tmp.unwind;
				}
				return blockCount + tmp.unwind;
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
				std::vector<uint32_t> list;
				list.push_back(static_cast<uint32_t>(temp.list.size()));
				for (auto v : temp.list) {
					list.push_back(finalRef(v));
				}
				list.push_back(unwindWord(temp));
				list.insert(list.end(), temp.tail.begin(), temp.tail.end());
				inst.c = out_.addExtra(list);
				break;
			}
			case Op::Br: {
				auto target = layoutIndex_[inst.a];
				auto args = branchArgs(temp.edgeTrue);
				std::vector<uint32_t> list {static_cast<uint32_t>(args.size())};
				list.insert(list.end(), args.begin(), args.end());
				inst.a = target;
				inst.c = out_.addExtra(list);
				break;
			}
			case Op::CondBr: {
				auto probability = inst.c;
				auto trueArgs = branchArgs(temp.edgeTrue);
				auto falseArgs = branchArgs(temp.edgeFalse);
				std::vector<uint32_t> list {probability, static_cast<uint32_t>(trueArgs.size())};
				list.insert(list.end(), trueArgs.begin(), trueArgs.end());
				list.push_back(layoutIndex_[succEdgesStatic_[b][1]]);
				list.push_back(static_cast<uint32_t>(falseArgs.size()));
				list.insert(list.end(), falseArgs.begin(), falseArgs.end());
				inst.b = layoutIndex_[inst.b];
				inst.c = out_.addExtra(list);
				break;
			}
			default:
				break;
			}
			Ref id = builder.emit(inst);
			assert(id == finalId_[t]);
			(void) id;
			++stats_.instructions;
		}
	}
	// landing pads, in the order of their first reference
	for (const auto& pad : pads_) {
		builder.beginBlock(0, BLOCK_LANDING_PAD);
		++stats_.blocks;
		++stats_.landingPads;
		for (const auto& [callee, address] : pad.entries) {
			uint32_t list[] = {1, finalRef(address), NO_UNWIND, NONE};
			builder.emit(Op::Call, TypeId::Void, 0, callee, 0, out_.addExtra(list));
			++stats_.instructions;
		}
		builder.emit(Op::Resume, TypeId::Void, 0);
		++stats_.instructions;
	}
	builder.endFunction();
	++stats_.functions;
}

void FunctionConverter::run() {
	analyze();
	buildCfg();
	orderBlocks();

	phis_.assign(n_, {});
	body_.assign(n_, {});
	currentDef_.assign(n_, {});
	sealed_.assign(n_, 0);
	pending_.assign(n_, 0);
	incoming_.assign(n_, {});
	incomplete_.assign(n_, {});
	for (auto b : rpo_) {
		for (auto to : succEdgesStatic_[b]) {
			++pending_[to];
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
		phis_[0].push_back(t);
		if (isVariable_[id]) {
			setDef(id, 0, t);
		} else {
			valueOf_[id] = t;
		}
	}
	sealed_[0] = 1;

	for (auto b : layout_) {
		convertBlock(b);
	}
	for (auto b : layout_) {
		if (!sealed_[b]) {
			fail(f_, "block ^b" + std::to_string(b) + " was never sealed");
		}
	}
	emit();
}

} // namespace

Module convert(const Module& preSSA, const ConversionOptions&, ConversionStatistics* statistics) {
	ConversionStatistics local;
	auto& stats = statistics != nullptr ? *statistics : local;
	stats = {};

	Module out = preSSA;
	out.insts.clear();
	out.blocks.clear();
	out.functions.clear();
	out.allocas.clear();
	// the signatures keep their parameter types in `extra`; the instruction lists of the pre-SSA functions do not
	// come along
	out.extra.clear();
	for (auto& sig : out.signatures) {
		const auto words = std::span<const uint32_t>(preSSA.extra.data() + sig.paramTypes, sig.paramCount);
		sig.paramTypes = out.addExtra(words);
	}
	for (uint32_t f = 0; f < preSSA.functions.size(); ++f) {
		if (preSSA.functions[f].level != static_cast<uint8_t>(Level::PreSSA)) {
			throw std::runtime_error("conversion of function #" + std::to_string(f) + ": it is not a pre-SSA function");
		}
		FunctionConverter(preSSA, f, out, stats).run();
	}
	out.rebuildIndexes();
	return out;
}

} // namespace nautilus::conversion

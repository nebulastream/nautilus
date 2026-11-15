#include "nautilus/extensions/ExtensionRegistry.hpp"
#include "nautilus/passes/MutableFunction.hpp"
#include "nautilus/passes/Passes.hpp"
#include <algorithm>
#include <map>
#include <set>
#include <string>
#include <unordered_map>

namespace nautilus::passes {

using nir::CalleeKind;
using nir::NONE;
using nir::Op;
using nir::Ref;

namespace {

constexpr size_t MAX_NOTES = 256;
/// What an imported callee's closure (its body and the internal functions of its exporter that the body reaches) may
/// have when it is copied into the importer.
constexpr size_t MAX_CLOSURE_FUNCTIONS = 16;

bool isOp(const MInst& mi, Op op) {
	return mi.inst.op == static_cast<uint16_t>(op);
}

/// The unwind entry of a may-throw instruction, or null.
uint32_t* unwindSlot(MInst& mi) {
	switch (static_cast<Op>(mi.inst.op)) {
	case Op::Call:
	case Op::CallIndirect:
		return &mi.list[1 + mi.list[0]];
	case Op::Div:
	case Op::Rem:
		return (mi.inst.flags & 2) ? &mi.inst.c : nullptr;
	default:
		return nullptr;
	}
}

/// The strongly connected components of a graph, callees first (Tarjan, iterative).
struct Components {
	std::vector<std::vector<uint32_t>> order; ///< components in the order they complete: a callee before its callers
	std::vector<char> recursive;              ///< per node: in a cycle
};

Components stronglyConnected(const std::vector<std::vector<uint32_t>>& edges) {
	const auto n = static_cast<uint32_t>(edges.size());
	Components result;
	result.recursive.assign(n, 0);
	std::vector<uint32_t> index(n, NONE), low(n, 0), stack;
	std::vector<char> onStack(n, 0);
	uint32_t counter = 0;
	struct Frame {
		uint32_t node;
		size_t next;
	};
	for (uint32_t root = 0; root < n; ++root) {
		if (index[root] != NONE) {
			continue;
		}
		std::vector<Frame> frames {{root, 0}};
		index[root] = low[root] = counter++;
		stack.push_back(root);
		onStack[root] = 1;
		while (!frames.empty()) {
			auto& frame = frames.back();
			const uint32_t v = frame.node;
			if (frame.next < edges[v].size()) {
				const uint32_t w = edges[v][frame.next++];
				if (index[w] == NONE) {
					index[w] = low[w] = counter++;
					stack.push_back(w);
					onStack[w] = 1;
					frames.push_back({w, 0});
				} else if (onStack[w]) {
					low[v] = std::min(low[v], index[w]);
				}
				continue;
			}
			if (low[v] == index[v]) {
				std::vector<uint32_t> component;
				uint32_t w;
				do {
					w = stack.back();
					stack.pop_back();
					onStack[w] = 0;
					component.push_back(w);
				} while (w != v);
				const bool cycle =
				    component.size() > 1 || std::find(edges[v].begin(), edges[v].end(), v) != edges[v].end();
				if (cycle) {
					for (uint32_t c : component) {
						result.recursive[c] = 1;
					}
				}
				result.order.push_back(std::move(component));
			}
			frames.pop_back();
			if (!frames.empty()) {
				low[frames.back().node] = std::min(low[frames.back().node], low[v]);
			}
		}
	}
	return result;
}

/// Copies the closure of an exported function of another module into the importer (13 §4.4.3): the function and the
/// internal functions of its module that it reaches, as internal functions of the importer. A function that uses
/// `bind.addr` stays with its module, and so do extension operations and imported callees (a later version may lift
/// the last two).
class Transplant {
public:
	Transplant(const nir::Module& exporter, std::string exporterName, nir::Module& importer,
	           std::vector<MutableFunction>& functions)
	    : e_(exporter), exporterName_(std::move(exporterName)), m_(importer), functions_(functions) {
	}

	/// The functions to copy for `root`, or why not.
	bool plan(uint32_t root, uint32_t maxInsts, std::string& why) {
		std::set<uint32_t> seen {root};
		std::vector<uint32_t> work {root};
		size_t total = 0;
		while (!work.empty()) {
			const uint32_t f = work.back();
			work.pop_back();
			closure_.push_back(f);
			if (closure_.size() > MAX_CLOSURE_FUNCTIONS) {
				why = "the callee reaches too many functions of its module";
				return false;
			}
			for (const auto& fa : e_.functionAttrs) {
				if (fa.function == f) {
					why = "a function of its closure has function attributes";
					return false;
				}
			}
			total += e_.functions[f].instCount;
			for (const auto& inst : e_.functionInsts(f)) {
				if (inst.op >= nir::FIRST_EXTENSION_OP) {
					why = "its body has an extension operation";
					return false;
				}
				if (inst.op == static_cast<uint16_t>(Op::BindAddr)) {
					why = "its body or a function it calls uses bind.addr, which resolves against its own module";
					return false;
				}
				if (inst.op == static_cast<uint16_t>(Op::Call) || inst.op == static_cast<uint16_t>(Op::FuncAddr)) {
					const auto& callee = e_.callees[inst.a];
					if (callee.kind == static_cast<uint8_t>(CalleeKind::Imported)) {
						why = "its body calls an import of its own module";
						return false;
					}
					if (callee.kind == static_cast<uint8_t>(CalleeKind::Internal) &&
					    seen.insert(callee.target).second) {
						work.push_back(callee.target);
					}
				}
			}
		}
		if (total > static_cast<size_t>(maxInsts) * 4) {
			why = "the callee and the functions it reaches are too large";
			return false;
		}
		return true;
	}

	/// Copies the planned closure; returns the callee index of the root in the importer.
	uint32_t copy(uint32_t root, bool locations, bool regions) {
		uint32_t next = static_cast<uint32_t>(functions_.size());
		for (uint32_t f : closure_) {
			fnIndex_[f] = next++;
		}
		functions_.resize(next);
		for (uint32_t f : closure_) {
			auto fn = decode(e_, f);
			// the debug sections of the exporter are not the importer's: its instructions have no location
			fn.hasLocations = locations;
			fn.hasRegions = regions;
			for (auto& b : fn.blocks) {
				for (auto& mi : b.insts) {
					mi.location = mi.region = NONE;
					remap(mi);
				}
			}
			fn.record.nameId = m_.internName(uniqueName(e_.name(fn.record.nameId)));
			fn.record.signature = signature(fn.record.signature);
			fn.record.visibility = static_cast<uint8_t>(nir::Visibility::Internal);
			functions_[fnIndex_[f]] = std::move(fn);
		}
		const auto& fr = e_.functions[root];
		nir::CalleeRecord rec {};
		rec.kind = static_cast<uint8_t>(CalleeKind::Internal);
		rec.target = fnIndex_.at(root);
		rec.signature = signature(fr.signature);
		rec.attrs = fr.attrs & nir::GUARANTEE_MASK;
		return m_.internCallee(rec);
	}

private:
	std::string uniqueName(std::string_view name) {
		std::string candidate = exporterName_ + "::" + std::string(name);
		auto taken = [&](const std::string& n) {
			for (const auto& f : functions_) {
				if (f.blocks.empty()) {
					continue;
				}
				if (m_.name(f.record.nameId) == n) {
					return true;
				}
			}
			return false;
		};
		std::string unique = candidate;
		for (int i = 1; taken(unique); ++i) {
			unique = candidate + "#" + std::to_string(i);
		}
		return unique;
	}

	uint32_t signature(uint32_t s) {
		const auto& rec = e_.signatures[s];
		std::vector<nir::TypeWord> params(e_.extra.begin() + rec.paramTypes,
		                                  e_.extra.begin() + rec.paramTypes + rec.paramCount);
		return m_.internSignature(rec.result, params);
	}

	uint32_t pointer(uint32_t p) {
		if (p == NONE) {
			return NONE;
		}
		auto rec = e_.pointers[p];
		if (rec.nameId != NONE && rec.nameId != 0) {
			rec.nameId = m_.internName(e_.name(rec.nameId));
		}
		return m_.internPointer(rec);
	}

	Ref constant(Ref r) {
		const auto& rec = e_.consts.record(r);
		return m_.consts.intern(static_cast<nir::TypeId>(rec.type), static_cast<nir::Provenance>(rec.provenance),
		                        rec.bits);
	}

	uint32_t callee(uint32_t c) {
		nir::CalleeRecord rec = e_.callees[c];
		rec.signature = signature(rec.signature);
		switch (static_cast<CalleeKind>(rec.kind)) {
		case CalleeKind::Internal:
			rec.target = fnIndex_.at(rec.target);
			break;
		case CalleeKind::External:
			rec.target = pointer(rec.target);
			break;
		default:
			break; // an intrinsic is identified by the process's registry
		}
		return m_.internCallee(rec);
	}

	void remap(MInst& mi) {
		forEachValue(mi, [&](Ref& r) {
			if (nir::isConstRef(r)) {
				r = constant(r);
			}
		});
		switch (static_cast<Op>(mi.inst.op)) {
		case Op::Call:
			mi.inst.a = callee(mi.inst.a);
			if (uint32_t& thunk = mi.list[2 + mi.list[0]]; thunk != NONE) {
				thunk = pointer(thunk);
			}
			break;
		case Op::CallIndirect:
			mi.inst.b = signature(mi.inst.b);
			if (uint32_t& thunk = mi.list[2 + mi.list[0]]; thunk != NONE) {
				thunk = pointer(thunk);
			}
			break;
		case Op::FuncAddr:
			mi.inst.a = callee(mi.inst.a);
			break;
		case Op::DataAddr: {
			const auto& d = e_.data[mi.inst.a];
			mi.inst.a = m_.internData(std::span<const std::byte>(e_.dataBytes).subspan(d.byteOffset, d.size), d.align);
			break;
		}
		case Op::PtrConst:
			mi.inst.a = pointer(mi.inst.a);
			break;
		default:
			break;
		}
	}

	const nir::Module& e_;
	std::string exporterName_;
	nir::Module& m_;
	std::vector<MutableFunction>& functions_;
	std::vector<uint32_t> closure_;
	std::map<uint32_t, uint32_t> fnIndex_;
};

class InlinePass final : public Pass {
public:
	explicit InlinePass(InlineConfig config) : config_(config) {
	}

	[[nodiscard]] std::string_view name() const override {
		return "inline";
	}

	std::optional<nir::Module> run(const nir::Module& in, PassContext& context) override {
		bool candidate = false;
		for (const auto& inst : in.insts) {
			if (inst.op == static_cast<uint16_t>(Op::Call)) {
				const auto kind = static_cast<CalleeKind>(in.callees[inst.a].kind);
				if (kind == CalleeKind::Internal ||
				    (kind == CalleeKind::Imported && config_.crossModule && context.imports != nullptr)) {
					candidate = true;
					break;
				}
			}
		}
		if (!candidate) {
			return std::nullopt;
		}
		context_ = &context;
		notes_ = 0;
		nir::Module out = copyForRewrite(in);
		out_ = &out;
		in_ = &in;
		functions_.clear();
		functions_.reserve(in.functions.size());
		for (uint32_t f = 0; f < in.functions.size(); ++f) {
			functions_.push_back(decode(in, f));
		}
		hasLocations_ = !functions_.empty() && functions_.front().hasLocations;
		hasRegions_ = !functions_.empty() && functions_.front().hasRegions;
		withAttributes_.clear();
		for (const auto& fa : in.functionAttrs) {
			withAttributes_.insert(fa.function);
		}

		uint64_t changes = 0;
		if (config_.crossModule && context.imports != nullptr) {
			changes += importCallees();
		}

		// callees first
		std::vector<std::vector<uint32_t>> edges(functions_.size());
		for (uint32_t f = 0; f < functions_.size(); ++f) {
			std::set<uint32_t> targets;
			for (const auto& b : functions_[f].blocks) {
				for (const auto& mi : b.insts) {
					if (isOp(mi, Op::Call)) {
						const auto& c = out.callees[mi.inst.a];
						if (c.kind == static_cast<uint8_t>(CalleeKind::Internal)) {
							targets.insert(c.target);
						}
					}
				}
			}
			edges[f].assign(targets.begin(), targets.end());
		}
		auto components = stronglyConnected(edges);
		recursive_ = std::move(components.recursive);
		uint64_t inlined = 0;
		for (const auto& component : components.order) {
			for (uint32_t f : component) {
				inlined += inlineInto(f);
			}
		}
		changes += inlined;
		if (changes == 0) {
			return std::nullopt;
		}
		const bool removed = inlined != 0 && removeUnreachable();
		(void) removed;
		for (const auto& fn : functions_) {
			encode(fn, out);
		}
		context.changes = changes;
		return out;
	}

private:
	bool wantNotes() const {
		return context_->notes != nullptr && notes_ < MAX_NOTES;
	}
	void note(std::string text) {
		if (wantNotes()) {
			context_->notes->push_back(std::move(text));
			++notes_;
		}
	}

	std::string functionName(uint32_t f) const {
		return std::string(out_->name(functions_[f].record.nameId));
	}

	/// Retargets the calls of imported callees whose body can be inlined to copies of that body.
	uint64_t importCallees() {
		struct Imported {
			uint32_t callee = NONE;
			uint32_t attrs = 0;
		};
		// the imported callees the module calls, in order of first use; copying a body grows `functions_`, so the calls
		// are visited only after all copies exist
		std::map<uint32_t, Imported> byCallee;
		std::vector<uint32_t> used;
		for (const auto& fn : functions_) {
			for (const auto& block : fn.blocks) {
				for (const auto& mi : block.insts) {
					if (isOp(mi, Op::Call) &&
					    out_->callees[mi.inst.a].kind == static_cast<uint8_t>(CalleeKind::Imported) &&
					    byCallee.emplace(mi.inst.a, Imported {}).second) {
						used.push_back(mi.inst.a);
					}
				}
			}
		}
		for (uint32_t calleeIndex : used) {
			const nir::CalleeRecord record = out_->callees[calleeIndex];
			auto& imported = byCallee[calleeIndex];
			tryImport(record, imported.callee, imported.attrs);
		}
		uint64_t count = 0;
		for (auto& fn : functions_) {
			for (auto& block : fn.blocks) {
				for (auto& mi : block.insts) {
					if (!isOp(mi, Op::Call)) {
						continue;
					}
					auto it = byCallee.find(mi.inst.a);
					if (it == byCallee.end() || it->second.callee == NONE) {
						continue;
					}
					if (out_->callees[mi.inst.a].kind != static_cast<uint8_t>(CalleeKind::Imported)) {
						continue;
					}
					mi.inst.a = it->second.callee;
					if (nir::attrsImply(it->second.attrs, nir::AttrNoUnwind)) {
						mi.list[1 + mi.list[0]] = nir::NO_UNWIND;
					}
					++count;
				}
			}
		}
		return count;
	}

	void tryImport(const nir::CalleeRecord& callee, uint32_t& result, uint32_t& attrs) {
		const auto& import = in_->imports[callee.target];
		const std::string moduleName(in_->name(import.moduleNameId));
		const std::string functionName(in_->name(import.functionNameId));
		auto skip = [&](const std::string& why) {
			if (wantNotes()) {
				note("not inlined: import " + moduleName + "." + functionName + ": " + why);
			}
		};
		auto exporter = context_->imports->exporter(*in_, callee.target);
		if (!exporter.module) {
			skip("the exporting module keeps no NIR");
			return;
		}
		uint32_t root = NONE;
		for (uint32_t f = 0; f < exporter.module->functions.size(); ++f) {
			if (exporter.module->name(exporter.module->functions[f].nameId) == functionName &&
			    exporter.module->functions[f].visibility == static_cast<uint8_t>(nir::Visibility::Exported)) {
				root = f;
			}
		}
		if (root == NONE) {
			skip("the exporter has no such export");
			return;
		}
		if (exporter.module->functions[root].instCount > config_.maxInsts) {
			skip("too large (" + std::to_string(exporter.module->functions[root].instCount) +
			     " instructions, ir.inline.maxInsts " + std::to_string(config_.maxInsts) + ")");
			return;
		}
		Transplant transplant(*exporter.module, moduleName, *out_, functions_);
		std::string why;
		if (!transplant.plan(root, config_.maxInsts, why)) {
			skip(why);
			return;
		}
		result = transplant.copy(root, hasLocations_, hasRegions_);
		attrs = out_->callees[result].attrs;
		note("import " + moduleName + "." + functionName + ": body copied for inlining");
		if (context_->inlinedImports != nullptr) {
			context_->inlinedImports->push_back({callee.target, moduleName, functionName, exporter.hash});
		}
	}

	/// Inlines the eligible calls of function `f`; returns how many.
	uint64_t inlineInto(uint32_t f) {
		auto& caller = functions_[f];
		size_t size = caller.instCount();
		std::vector<std::pair<uint32_t, Ref>> pending; // call uid, replacement
		uint64_t count = 0;
		for (size_t bi = 0; bi < caller.blocks.size(); ++bi) {
			if (caller.blocks[bi].isPad()) {
				continue;
			}
			for (size_t ii = 0; ii < caller.blocks[bi].insts.size();) {
				const MInst& mi = caller.blocks[bi].insts[ii];
				if (!isOp(mi, Op::Call)) {
					++ii;
					continue;
				}
				const auto& callee = out_->callees[mi.inst.a];
				if (callee.kind != static_cast<uint8_t>(CalleeKind::Internal)) {
					++ii;
					continue;
				}
				const uint32_t g = callee.target;
				std::string why;
				if (g == f || recursive_[g]) {
					why = "recursive";
				} else if (withAttributes_.count(g) != 0) {
					why = "the callee has function attributes";
				} else if (functions_[g].instCount() > config_.maxInsts) {
					why = "too large (" + std::to_string(functions_[g].instCount()) +
					      " instructions, ir.inline.maxInsts " + std::to_string(config_.maxInsts) + ")";
				} else if (size + functions_[g].instCount() > config_.maxCallerInsts) {
					why = "the caller would grow beyond " + std::to_string(config_.maxCallerInsts) + " instructions";
				}
				if (!why.empty()) {
					if (wantNotes()) {
						note("not inlined: " + functionName(f) + " calls " + functionName(g) + ": " + why);
					}
					++ii;
					continue;
				}
				size += functions_[g].instCount();
				if (wantNotes()) {
					note("inlined: " + functionName(g) + " into " + functionName(f) + " (" +
					     std::to_string(functions_[g].instCount()) + " instructions)");
				}
				inlineCall(caller, bi, ii, functions_[g], pending);
				++count;
				// the instruction at `ii` is now the first of the callee's body; scan it
			}
		}
		if (!pending.empty()) {
			std::vector<Ref> subst(caller.nextUid, NONE);
			for (auto [uid, replacement] : pending) {
				subst[uid] = replacement;
			}
			substitute(caller, subst);
		}
		return count;
	}

	void inlineCall(MutableFunction& caller, size_t bi, size_t ii, const MutableFunction& callee,
	                std::vector<std::pair<uint32_t, Ref>>& pending) {
		const MInst call = caller.blocks[bi].insts[ii];
		const uint32_t argc = call.list[0];
		const std::vector<Ref> args(call.list.begin() + 1, call.list.begin() + 1 + argc);
		const uint32_t callUnwind = call.list[1 + argc];
		const bool hasResult = static_cast<nir::TypeId>(call.inst.type) != nir::TypeId::Void;

		const uint32_t uidBase = caller.nextUid;
		caller.nextUid += callee.nextUid;
		const uint32_t blockBase = caller.nextBlockUid;
		caller.nextBlockUid += callee.nextBlockUid;
		const auto allocaBase = static_cast<uint32_t>(caller.allocas.size());
		caller.allocas.insert(caller.allocas.end(), callee.allocas.begin(), callee.allocas.end());

		std::vector<Ref> sub(callee.nextUid);
		for (uint32_t uid = 0; uid < callee.nextUid; ++uid) {
			sub[uid] = uidBase + uid;
		}
		const MBlock& entry = callee.blocks[0];
		const uint16_t params = entry.argCount();
		bool entryHasPredecessors = false;
		size_t returns = 0, normalBlocks = 0;
		for (const auto& b : callee.blocks) {
			normalBlocks += b.isPad() ? 0 : 1;
			for (const auto& mi : b.insts) {
				returns += isOp(mi, Op::Ret) ? 1 : 0;
				if (isOp(mi, Op::Br) && mi.inst.a == entry.uid) {
					entryHasPredecessors = true;
				}
				if (isOp(mi, Op::CondBr) && (mi.inst.b == entry.uid || mi.list[2 + mi.list[1]] == entry.uid)) {
					entryHasPredecessors = true;
				}
			}
		}
		const bool merge = !entryHasPredecessors;
		if (merge) {
			for (uint16_t p = 0; p < params; ++p) {
				sub[entry.insts[p].uid] = args[p];
			}
		}
		const bool straight = merge && normalBlocks == 1 && returns == 1 && isOp(entry.terminator(), Op::Ret);

		// the continuation: the rest of the caller's block after the call
		MBlock cont;
		cont.uid = caller.freshBlockUid();
		std::vector<MInst> tail(std::make_move_iterator(caller.blocks[bi].insts.begin() + ii + 1),
		                        std::make_move_iterator(caller.blocks[bi].insts.end()));
		caller.blocks[bi].insts.resize(ii);

		Ref resultValue = NONE;
		if (hasResult && returns > 1 && !straight) {
			MInst arg;
			arg.inst.op = static_cast<uint16_t>(Op::BlockArg);
			arg.inst.type = call.inst.type;
			arg.inst.a = 0;
			arg.uid = caller.freshUid();
			resultValue = arg.uid;
			cont.insts.push_back(arg);
		}

		auto copyInst = [&](const MInst& src, bool inPad) {
			MInst d = src;
			d.uid = uidBase + src.uid;
			forEachValue(d, [&](Ref& r) {
				if (nir::isValueRef(r)) {
					r = sub[r];
				}
			});
			forEachBlock(d, [&](uint32_t& b) { b += blockBase; });
			if (uint32_t* slot = unwindSlot(d); slot != nullptr && *slot == nir::NO_CLEANUP && !inPad) {
				*slot = callUnwind; // the exception propagates to the caller's handler
			}
			if (isOp(d, Op::Alloca)) {
				d.inst.a += allocaBase;
			}
			return d;
		};
		auto translateTerminator = [&](MInst& d, const MInst& src, bool inPad) {
			if (isOp(src, Op::Ret)) {
				const Ref v = src.inst.a == NONE ? NONE : (nir::isValueRef(src.inst.a) ? sub[src.inst.a] : src.inst.a);
				d = MInst {};
				d.inst.op = static_cast<uint16_t>(Op::Br);
				d.inst.a = cont.uid;
				d.uid = uidBase + src.uid;
				d.location = src.location;
				d.region = src.region;
				if (hasResult && returns > 1) {
					d.list = {1, v};
				} else {
					d.list = {0};
				}
			} else if (isOp(src, Op::Resume) && inPad && callUnwind != nir::NO_UNWIND &&
			           callUnwind != nir::NO_CLEANUP) {
				// the caller's handler continues the unwinding
				d.inst.op = static_cast<uint16_t>(Op::Br);
				d.inst.a = callUnwind;
				d.list = {0};
			}
		};

		auto& blocks = caller.blocks;
		std::vector<MBlock> region, pads;
		// the retained value of the single return, used when no argument carries it
		Ref singleReturn = NONE;
		auto noteReturn = [&](const MInst& src) {
			if (hasResult && returns == 1) {
				singleReturn = nir::isValueRef(src.inst.a) ? sub[src.inst.a] : src.inst.a;
			}
		};

		for (size_t k = 0; k < callee.blocks.size(); ++k) {
			const MBlock& src = callee.blocks[k];
			const bool inPad = src.isPad();
			MBlock dst;
			dst.uid = blockBase + src.uid;
			dst.flags = src.flags;
			const bool intoCaller = k == 0 && merge;
			size_t first = (intoCaller ? params : 0);
			for (size_t i = first; i < src.insts.size(); ++i) {
				const MInst& s = src.insts[i];
				if (i + 1 == src.insts.size() && (isOp(s, Op::Ret) || isOp(s, Op::Resume))) {
					if (isOp(s, Op::Ret)) {
						noteReturn(s);
					}
					if (straight && isOp(s, Op::Ret)) {
						continue; // the caller's block goes on with its tail
					}
					MInst d = copyInst(s, inPad);
					translateTerminator(d, s, inPad);
					dst.insts.push_back(std::move(d));
					continue;
				}
				dst.insts.push_back(copyInst(s, inPad));
			}
			if (intoCaller) {
				auto& b = blocks[bi].insts;
				b.insert(b.end(), std::make_move_iterator(dst.insts.begin()), std::make_move_iterator(dst.insts.end()));
			} else if (inPad) {
				pads.push_back(std::move(dst));
			} else {
				region.push_back(std::move(dst));
			}
		}

		if (!merge) {
			// the callee's entry has predecessors: it keeps its block arguments and the call's arguments flow into them
			MInst br;
			br.inst.op = static_cast<uint16_t>(Op::Br);
			br.inst.a = blockBase + entry.uid;
			br.uid = caller.freshUid();
			br.list.push_back(argc);
			br.list.insert(br.list.end(), args.begin(), args.end());
			blocks[bi].insts.push_back(std::move(br));
		}

		if (straight) {
			auto& b = blocks[bi].insts;
			b.insert(b.end(), std::make_move_iterator(tail.begin()), std::make_move_iterator(tail.end()));
			// the pads are the only blocks to add
		} else {
			cont.insts.insert(cont.insts.end(), std::make_move_iterator(tail.begin()),
			                  std::make_move_iterator(tail.end()));
			region.push_back(std::move(cont));
		}
		blocks.insert(blocks.begin() + bi + 1, std::make_move_iterator(region.begin()),
		              std::make_move_iterator(region.end()));
		blocks.insert(blocks.end(), std::make_move_iterator(pads.begin()), std::make_move_iterator(pads.end()));

		if (hasResult) {
			if (returns > 1 && !straight) {
				pending.emplace_back(call.uid, resultValue);
			} else if (returns == 1) {
				pending.emplace_back(call.uid, singleReturn);
			}
		}
	}

	/// Drops the internal functions that no exported function reaches. Returns whether any went.
	bool removeUnreachable() {
		if (!out_->debugRegions.empty() || !out_->debugFunctions.empty()) {
			return false; // debug sections refer to functions by index; they keep every function
		}
		const auto n = static_cast<uint32_t>(functions_.size());
		std::vector<char> keep(n, 0);
		std::vector<uint32_t> work;
		for (uint32_t f = 0; f < n; ++f) {
			if (functions_[f].record.visibility == static_cast<uint8_t>(nir::Visibility::Exported)) {
				keep[f] = 1;
				work.push_back(f);
			}
		}
		while (!work.empty()) {
			const uint32_t f = work.back();
			work.pop_back();
			for (const auto& b : functions_[f].blocks) {
				for (const auto& mi : b.insts) {
					if (isOp(mi, Op::Call) || isOp(mi, Op::FuncAddr)) {
						const auto& c = out_->callees[mi.inst.a];
						if (c.kind == static_cast<uint8_t>(CalleeKind::Internal) && !keep[c.target]) {
							keep[c.target] = 1;
							work.push_back(c.target);
						}
					}
				}
			}
		}
		if (std::count(keep.begin(), keep.end(), 1) == n) {
			return false;
		}
		std::vector<uint32_t> newIndex(n, NONE);
		uint32_t next = 0;
		for (uint32_t f = 0; f < n; ++f) {
			if (keep[f]) {
				newIndex[f] = next++;
			}
		}
		std::vector<nir::CalleeRecord> callees;
		std::vector<uint32_t> calleeIndex(out_->callees.size(), NONE);
		for (uint32_t c = 0; c < out_->callees.size(); ++c) {
			auto rec = out_->callees[c];
			if (rec.kind == static_cast<uint8_t>(CalleeKind::Internal)) {
				if (!keep[rec.target]) {
					continue;
				}
				rec.target = newIndex[rec.target];
			}
			calleeIndex[c] = static_cast<uint32_t>(callees.size());
			callees.push_back(rec);
		}
		std::vector<MutableFunction> kept;
		for (uint32_t f = 0; f < n; ++f) {
			if (!keep[f]) {
				continue;
			}
			for (auto& b : functions_[f].blocks) {
				for (auto& mi : b.insts) {
					if (isOp(mi, Op::Call) || isOp(mi, Op::FuncAddr)) {
						mi.inst.a = calleeIndex[mi.inst.a];
					}
				}
			}
			kept.push_back(std::move(functions_[f]));
		}
		functions_ = std::move(kept);
		out_->callees = std::move(callees);
		std::vector<nir::FunctionAttrRecord> attrs;
		for (auto rec : out_->functionAttrs) {
			if (rec.function < n && keep[rec.function]) {
				rec.function = newIndex[rec.function];
				attrs.push_back(rec);
			}
		}
		out_->functionAttrs = std::move(attrs);
		out_->rebuildIndexes();
		return true;
	}

	InlineConfig config_;
	PassContext* context_ = nullptr;
	const nir::Module* in_ = nullptr;
	nir::Module* out_ = nullptr;
	std::vector<MutableFunction> functions_;
	std::vector<char> recursive_;
	std::set<uint32_t> withAttributes_;
	size_t notes_ = 0;
	bool hasLocations_ = false;
	bool hasRegions_ = false;
};

} // namespace

std::unique_ptr<Pass> makeInlinePass(InlineConfig config) {
	return std::make_unique<InlinePass>(config);
}

} // namespace nautilus::passes

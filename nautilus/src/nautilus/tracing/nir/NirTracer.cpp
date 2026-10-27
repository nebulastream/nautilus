#include "nautilus/tracing/nir/NirTracer.hpp"
#include "nautilus/exceptions/NotImplementedException.hpp"
#include "nautilus/exceptions/RuntimeException.hpp"
#include "nautilus/logging.hpp"
#include "nautilus/nautilus_function.hpp"
#include "nautilus/nir/Builder.hpp"
#include "nautilus/nir/Operands.hpp"
#include "nautilus/tracing/TracingUtil.hpp"
#include "nautilus/tracing/nir/NirTrace.hpp"
#include <algorithm>
#include <bit>
#include <cassert>
#include <cstring>
#include <fmt/format.h>

namespace nautilus::tracing::nirtrace {

namespace nir = ::nautilus::nir;
using NOp = nir::Op;
using nir::TypeId;

// Thread-local tracer object, like TraceContext's.
static thread_local NirTracer nirTracer;

namespace {

uint64_t literalBits(const ConstantLiteral& value) {
	return std::visit(
	    [](auto v) -> uint64_t {
		    using T = decltype(v);
		    if constexpr (std::is_same_v<T, void*>) {
			    return reinterpret_cast<uintptr_t>(v);
		    } else if constexpr (std::is_same_v<T, float>) {
			    return std::bit_cast<uint32_t>(v);
		    } else if constexpr (std::is_same_v<T, double>) {
			    return std::bit_cast<uint64_t>(v);
		    } else if constexpr (std::is_same_v<T, bool>) {
			    return v ? 1 : 0;
		    } else {
			    return static_cast<uint64_t>(static_cast<std::make_unsigned_t<T>>(v));
		    }
	    },
	    value);
}

uint32_t probabilityWord(double p) {
	if (!(p >= 0.0 && p <= 1.0)) {
		return 0xFFFFFFFFu; // unknown
	}
	return static_cast<uint32_t>(std::llround(p * 0xFFFFFFFEull));
}

uint32_t attrBits(const FunctionAttributes& a) {
	uint32_t bits = 0;
	if (a.modRefInfo == ModRefInfo::NoModRef || a.modRefInfo == ModRefInfo::Ref) {
		bits |= nir::AttrNoArgWrite | nir::AttrNoOtherWrite;
	}
	if (a.modRefInfo == ModRefInfo::NoModRef || a.modRefInfo == ModRefInfo::Mod) {
		bits |= nir::AttrNoArgRead | nir::AttrNoOtherRead;
	}
	if (a.willReturn) {
		bits |= nir::AttrWillReturn;
	}
	if (a.noUnwind) {
		bits |= nir::AttrNoUnwind;
	}
	return bits;
}

} // namespace

// ---- the state of a whole tracing session ------------------------------------------------------------------------

struct NirTracer::SessionState {
	nir::Module module;
	std::list<compiler::CompilableFunction> functionsToTrace;
	/// Definition identity -> the name that definition is traced under (the names of the others are made unique).
	std::unordered_map<const void*, std::string> registeredFunctions;
	std::unordered_set<std::string> usedFunctionNames;
	/// Function name -> index in the module; assigned when a name is first seen, in the order the functions are traced.
	std::unordered_map<std::string, uint32_t> functionIndex;
	std::unordered_set<std::string> traced;
	std::vector<uint32_t> internalCalleeOf; // function index -> callee record, or NONE
	std::vector<uint32_t> internalAttrs;    // function index -> the guarantees every call of it states
	std::vector<uint8_t> internalAttrsSeen;
	std::unordered_map<std::string, uint32_t> bindingIndex;
	bool processLocal = false;
	std::string currentFunctionName;
	SourceLocation currentFunctionLocation;
	uint32_t entryIndex = nir::NONE;

	uint32_t indexOf(const std::string& name) {
		auto [it, inserted] = functionIndex.try_emplace(name, static_cast<uint32_t>(functionIndex.size()));
		if (inserted) {
			internalCalleeOf.push_back(nir::NONE);
			internalAttrs.push_back(0);
			internalAttrsSeen.push_back(0);
		}
		return it->second;
	}

	void noteCallAttributes(uint32_t function, uint32_t bits) {
		if (!internalAttrsSeen[function]) {
			internalAttrsSeen[function] = 1;
			internalAttrs[function] = bits;
		} else {
			internalAttrs[function] &= bits;
		}
	}

	/// Writes the finished trace of function `index` into the module as a pre-SSA function.
	void emitFunction(uint32_t index, const std::string& name, FlatTrace& trace, bool exported);
	/// Gives the callees of internal functions the signatures and attributes their functions ended up with.
	void finish();
};

namespace {

/// The blocks of a trace in reverse postorder from the entry, so that a definition comes before the uses it dominates
/// (13 §4.3); the false target of a branch is visited first, so that the true target is laid out first.
std::vector<uint32_t> layoutBlocks(FlatTrace& trace, std::vector<uint32_t>& newBlockId) {
	const auto n = trace.blocks.size();
	std::vector<uint8_t> state(n, 0);
	std::vector<uint32_t> post;
	struct Frame {
		uint32_t block;
		uint32_t next;
		uint32_t successors[2];
		uint32_t count;
	};
	auto frameFor = [&](uint32_t id) {
		Frame frame {id, 0, {0, 0}, 0};
		const auto& ops = trace.blocks[id].ops;
		if (!ops.empty()) {
			const auto& last = trace.recs[ops.back()];
			if (last.op == static_cast<uint16_t>(NOp::Br)) {
				frame.successors[frame.count++] = last.a;
			} else if (last.op == static_cast<uint16_t>(NOp::CondBr)) {
				frame.successors[frame.count++] = trace.conds[last.c].falseBlock;
				frame.successors[frame.count++] = last.b;
			}
		}
		return frame;
	};
	std::vector<Frame> stack;
	stack.push_back(frameFor(0));
	state[0] = 1;
	while (!stack.empty()) {
		auto& top = stack.back();
		if (top.next < top.count) {
			const auto next = top.successors[top.next++];
			if (next >= n) {
				throw RuntimeException(fmt::format("NIR trace: block id {} is outside the {}-block trace", next, n));
			}
			if (state[next] == 0) {
				state[next] = 1;
				stack.push_back(frameFor(next));
			}
		} else {
			post.push_back(top.block);
			stack.pop_back();
		}
	}
	std::vector<uint32_t> order(post.rbegin(), post.rend());
	newBlockId.assign(n, nir::NONE);
	for (uint32_t i = 0; i < order.size(); ++i) {
		newBlockId[order[i]] = i;
	}
	return order;
}

} // namespace

void NirTracer::SessionState::emitFunction(uint32_t index, const std::string& name, FlatTrace& trace, bool exported) {
	std::vector<uint32_t> newBlockId;
	const auto order = layoutBlocks(trace, newBlockId);

	// the signature: the parameters in position order, and the type of what the function returns
	std::vector<Type> paramTypes;
	for (auto record : trace.parameters) {
		if (record == 0) {
			throw RuntimeException(fmt::format("NIR trace of '{}': a parameter was never registered", name));
		}
		paramTypes.push_back(trace.meta[record].type);
	}
	Type resultType = Type::v;
	for (auto blockId : order) {
		for (auto id : trace.blocks[blockId].ops) {
			if (trace.recs[id].op == static_cast<uint16_t>(NOp::Ret) && trace.meta[id].type != Type::v) {
				resultType = trace.meta[id].type;
			}
		}
	}
	std::vector<nir::TypeWord> params;
	for (auto t : paramTypes) {
		params.push_back(typeWord(t));
	}
	const auto signature = module.internSignature(typeWord(resultType), params);

	nir::Builder b(module);
	const auto functionId =
	    b.beginFunction(module.internName(name), signature,
	                    exported ? nir::Visibility::Exported : nir::Visibility::Internal, nir::Level::PreSSA, 0);
	if (functionId != index) {
		throw RuntimeException(fmt::format("NIR trace of '{}': the function was traced out of order", name));
	}
	for (const auto& spec : trace.allocas) {
		b.addAlloca(spec.size, spec.align);
	}

	// A variable that one path declared and another assigns (a constant reached again on the second path) may be used
	// in a block that the layout places before its declaration. Such a declaration moves to the entry block, and its
	// place keeps an assign of the initial value; the function stays valid since every path that reads it has written
	// it.
	const auto isOp = [&](uint32_t id, NOp op) {
		return trace.recs[id].op == static_cast<uint16_t>(op);
	};
	std::vector<uint32_t> position(trace.recs.size(), 0);
	{
		uint32_t next = 0;
		for (auto blockId : order) {
			for (auto id : trace.blocks[blockId].ops) {
				position[id] = next++;
			}
		}
	}
	std::vector<uint8_t> hoisted(trace.recs.size(), 0);
	std::vector<uint32_t> hoistedVars;
	for (auto blockId : order) {
		for (auto id : trace.blocks[blockId].ops) {
			if (isOp(id, NOp::BlockArg)) {
				continue;
			}
			nir::Inst scan = trace.recs[id];
			auto visit = [&](nir::Ref& r) {
				if (!nir::isConstRef(r) && r < trace.recs.size() && isOp(r, NOp::Var) && position[r] > position[id] &&
				    !hoisted[r]) {
					hoisted[r] = 1;
					hoistedVars.push_back(r);
				}
			};
			if (scan.op == static_cast<uint16_t>(NOp::CondBr)) {
				visit(scan.a);
			} else if (scan.op != static_cast<uint16_t>(NOp::Br)) {
				nir::forEachRef(scan, module.extra.data(), nir::Level::PreSSA, visit);
			}
		}
	}
	std::sort(hoistedVars.begin(), hoistedVars.end());

	// The ids the records get are known up front: the parameters, the hoisted declarations, then the records in layout
	// order.
	std::vector<nir::Ref> newId(trace.recs.size(), nir::NONE);
	std::vector<nir::Ref> slotOf(trace.recs.size(), nir::NONE);
	{
		nir::Ref next = 0;
		for (auto record : trace.parameters) {
			newId[record] = next++;
		}
		for (auto v : hoistedVars) {
			if (trace.recs[v].type == static_cast<uint8_t>(TypeId::Ptr)) {
				++next; // the null pointer it starts with
			}
			newId[v] = next++;
		}
		for (auto blockId : order) {
			for (auto id : trace.blocks[blockId].ops) {
				if (!isOp(id, NOp::BlockArg)) {
					slotOf[id] = next++;
					if (!hoisted[id]) {
						newId[id] = slotOf[id];
					}
				}
			}
		}
	}
	for (uint32_t i = 0; i < order.size(); ++i) {
		b.beginBlock();
		if (i == 0) {
			for (uint32_t p = 0; p < trace.parameters.size(); ++p) {
				const auto record = trace.parameters[p];
				if (b.emit(NOp::BlockArg, typeInfo(paramTypes[p]).id, 0, p) != newId[record]) {
					throw RuntimeException("NIR trace: the instruction numbering of the parameters moved");
				}
			}
			for (auto v : hoistedVars) {
				const auto type = static_cast<TypeId>(trace.recs[v].type);
				nir::Ref init;
				if (type == TypeId::Ptr) {
					init = b.emit(NOp::PtrConst, TypeId::Ptr, 0, nir::NONE);
				} else {
					init = module.consts.intern(type, nir::Provenance::Literal, 0);
				}
				if (b.emit(NOp::Var, type, 0, init) != newId[v]) {
					throw RuntimeException("NIR trace: the instruction numbering of a hoisted variable moved");
				}
			}
		}
		for (auto id : trace.blocks[order[i]].ops) {
			nir::Inst out = trace.recs[id];
			if (out.op == static_cast<uint16_t>(NOp::BlockArg)) {
				continue; // emitted with the entry block
			}
			auto remap = [&](nir::Ref& r) {
				if (nir::isConstRef(r)) {
					return;
				}
				if (r >= newId.size() || newId[r] == nir::NONE) {
					std::string dump;
					for (uint32_t bi = 0; bi < trace.blocks.size(); ++bi) {
						dump += fmt::format("\n  block {} preds:", bi);
						for (auto p : trace.blocks[bi].preds) {
							dump += fmt::format(" {}", p);
						}
						for (auto o : trace.blocks[bi].ops) {
							const auto& rec = trace.recs[o];
							dump += fmt::format("\n    ${} op {} a {} b {} c {}", o, rec.op, rec.a, rec.b, rec.c);
						}
					}
					throw RuntimeException(
					    fmt::format("NIR trace of '{}': ${} is used before its definition{}", name, r, dump));
				}
				r = newId[r];
			};
			if (out.op == static_cast<uint16_t>(NOp::Br)) {
				const uint32_t args[] = {0};
				out.a = newBlockId[out.a];
				out.c = module.addExtra(args);
			} else if (out.op == static_cast<uint16_t>(NOp::CondBr)) {
				const auto& cond = trace.conds[out.c];
				remap(out.a);
				const uint32_t args[] = {cond.probability, 0, newBlockId[cond.falseBlock], 0};
				out.b = newBlockId[out.b];
				out.c = module.addExtra(args);
			} else {
				nir::forEachRef(out, module.extra.data(), nir::Level::PreSSA, remap);
			}
			if (hoisted[id]) {
				// the declaration moved up: here the variable is written with what it was declared to hold
				nir::Inst assign {};
				assign.op = static_cast<uint16_t>(NOp::Assign);
				assign.type = static_cast<uint8_t>(TypeId::Void);
				assign.a = newId[id];
				assign.b = out.a;
				out = assign;
			}
			if (b.emit(out) != slotOf[id]) {
				throw RuntimeException("NIR trace: the instruction numbering of a block moved");
			}
		}
	}
	b.endFunction();
}

void NirTracer::SessionState::finish() {
	for (uint32_t f = 0; f < internalCalleeOf.size(); ++f) {
		if (f >= module.functions.size()) {
			throw RuntimeException("NIR trace: a function was called but never traced");
		}
		module.functions[f].attrs = internalAttrs[f];
		if (internalCalleeOf[f] != nir::NONE) {
			auto& callee = module.callees[internalCalleeOf[f]];
			callee.signature = module.functions[f].signature;
			callee.attrs = internalAttrs[f];
		}
	}
	module.rebuildIndexes();
	if (processLocal) {
		module.module.processToken = nir::currentProcessToken();
	}
}

// ---- construction and the session -------------------------------------------------------------------------------

NirTracer::NirTracer() = default;
NirTracer::~NirTracer() = default;

NirTraceResult NirTracer::Trace(std::list<compiler::CompilableFunction>& functions, const engine::Options& options,
                                common::Arena& arena) {
	return nirTracer.traceSession(functions, options, arena);
}

NirTraceResult NirTracer::traceSession(std::list<compiler::CompilableFunction>& functions,
                                       const engine::Options& options, common::Arena& arena) {
	log::debug("Initialize NIR tracing");
	sessionState_ = std::make_unique<SessionState>();
	arena_ = &arena;
	auto& session = *sessionState_;
	session_ = this;
	parent_ = nullptr;
	stats_ = {};
	session.functionsToTrace = functions;
	// the functions of the list get their indices up front, the entry first; the others are numbered as they are met
	for (const auto& f : functions) {
		session.indexOf(f.getName());
	}
	setActiveTracer(this);
	ActiveTracerGuard activeTracerGuard;
	setFoldStaticConstants(options.getOptionOrDefault("engine.foldStaticConstants", true));

	bool isFirstFunction = true;
	while (!session.functionsToTrace.empty()) {
		auto currentFunction = session.functionsToTrace.front();
		session.functionsToTrace.pop_front();
		if (session.traced.contains(currentFunction.getName())) {
			log::debug("Function '{}' already traced, skipping.", currentFunction.getName());
			continue;
		}
		session.traced.insert(currentFunction.getName());
		const auto index = session.indexOf(currentFunction.getName());
		const bool exported = isFirstFunction;
		isFirstFunction = false;
		session.currentFunctionName = currentFunction.getName();
		session.currentFunctionLocation = currentFunction.getLocation();
		auto wrapperFunc = currentFunction.getFunction();

		FlatTrace trace(session.module);
		auto rootAddress = __builtin_return_address(0);
		auto tr = TagRecorder((TagAddress) rootAddress, arena);
		SymbolicExecutionContext symbolicExecutionContext;
		state.emplace(TraceScope {tr, trace, symbolicExecutionContext, options});

		if (regionState_) {
			regionState_->tagRecorders.clear();
			regionState_->regionMemo.clear();
		}
		entryBlock_ = 0;
		parent_ = nullptr;
		runScope(wrapperFunc);

		state.reset();
		trace.collapseRegionSeams();
		stats_.operations += trace.recs.size() - 1;
		session.emitFunction(index, currentFunction.getName(), trace, exported);
	}
	session.finish();
	NirTraceResult result {std::move(session.module), stats_};
	sessionState_.reset();
	return result;
}

// ---- passes, phases and statistics -------------------------------------------------------------------------------

void NirTracer::resume() {
	staticVars.clear();
	aliveVars.reset();
	if (parent_ != nullptr) {
		activeDestructors = parent_->activeDestructors;
	} else {
		activeDestructors.clear();
	}
	paused_ = false;
}

void NirTracer::beginPass(bool rerun) {
	rerunPass_ = rerun;
	phase_ = rerun ? Phase::Follow : Phase::Record;
	phaseStart_ = passStart_ = std::chrono::steady_clock::now();
}

void NirTracer::accountPhase(std::chrono::steady_clock::time_point now) {
	if (!rerunPass_) {
		return;
	}
	const double ms = std::chrono::duration<double, std::milli>(now - phaseStart_).count();
	auto& stats = session_->stats_;
	switch (phase_) {
	case Phase::Follow:
		stats.rerunFollowMs += ms;
		break;
	case Phase::Record:
		stats.rerunRecordMs += ms;
		break;
	case Phase::Paused:
		stats.rerunPausedMs += ms;
		break;
	case Phase::None:
		break;
	}
}

void NirTracer::enterPhase(Phase phase) {
	if (phase_ == Phase::None || phase_ == phase) {
		return;
	}
	const auto now = std::chrono::steady_clock::now();
	accountPhase(now);
	phase_ = phase;
	phaseStart_ = now;
}

void NirTracer::endPass() {
	const auto now = std::chrono::steady_clock::now();
	accountPhase(now);
	const double ms = std::chrono::duration<double, std::milli>(now - passStart_).count();
	(rerunPass_ ? session_->stats_.rerunMs : session_->stats_.firstRunMs) += ms;
	phase_ = Phase::None;
}

// The tag of an operation is the chain of return addresses between the scope's root and the operation, so it includes
// the address in this function the body is called from; peeling the loop below would give the first pass another
// call site than the later ones (see TraceContext::runScope).
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC push_options
#pragma GCC optimize("no-peel-loops")
#endif
void NirTracer::runScope(std::function<void()>& body) {
	auto& symbolicExecutionContext = state->symbolicExecutionContext;
	auto traceIteration = 0;
#if defined(__clang__)
#pragma clang loop unroll(disable)
#endif
	while (symbolicExecutionContext.shouldContinue()) {
		traceIteration = traceIteration + 1;
		symbolicExecutionContext.next();
		state->trace.setCurrentBlock(entryBlock_);
		resume();
		++session_->stats_.iterations;
		if (parent_ == nullptr) {
			beginPass(traceIteration > 1);
		}
		body();
		if (parent_ == nullptr) {
			endPass();
		}
		if (parent_ != nullptr) {
			traceScopeExit();
		}
		assert(staticVars.empty() && "static variable stack not empty after tracing iteration");
	}
	++session_->stats_.scopes;
	session_->stats_.maxScopeIterations = std::max<uint64_t>(session_->stats_.maxScopeIterations, traceIteration);
}
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC pop_options
#endif

Snapshot NirTracer::recordSnapshot() {
	++session_->stats_.snapshots;
	return {state->tagRecorder.createTag(), hashStaticVector(staticVars) ^ aliveVars.hash()};
}

bool NirTracer::isFollowing() {
	return state->symbolicExecutionContext.getCurrentMode() == SymbolicExecutionContext::MODE::FOLLOW;
}

TypedValueRef& NirTracer::follow() {
	++session_->stats_.replayedOperations;
	result_ = state->trace.consumeOperation();
	return result_;
}

template <typename OnCreation>
TypedValueRef& NirTracer::traceOperation(OnCreation&& onCreation) {
	if (paused_) {
		++session_->stats_.pausedOperations;
		return dummyRef_;
	}
	if (isFollowing()) {
		return follow();
	}
	auto tag = recordSnapshot();
	if (state->trace.checkTag(tag)) {
		return onCreation(tag);
	}
	// already explored from here: enter passive mode
	pause();
	return dummyRef_;
}

// ---- recording ---------------------------------------------------------------------------------------------------

TypedValueRef& NirTracer::finish(uint32_t first, const Snapshot& tag, TypedValueRef result) {
	state->trace.markOperation(first, &tag, result);
	result_ = result;
	return result_;
}

TypedValueRef& NirTracer::finish(uint32_t first, const Snapshot& tag, uint32_t resultRecord, Type resultType) {
	return finish(first, tag, TypedValueRef(resultType == Type::v ? 0 : resultRecord, resultType));
}

nir::Ref NirTracer::emit(NOp op, TypeId type, uint8_t flags, uint32_t a, uint32_t b, uint32_t c) {
	return state->trace.append(nir::Inst {static_cast<uint16_t>(op), static_cast<uint8_t>(type), flags, a, b, c});
}

nir::Ref NirTracer::emitCast(nir::Ref value, uint8_t kind, TypeId to) {
	return emit(NOp::Cast, to, kind, value);
}

nir::Ref NirTracer::emitConversion(const TypedValueRef& source, Type target) {
	auto from = typeInfo(source.type);
	auto to = typeInfo(target);
	const nir::Ref value = source.ref;
	enum : uint8_t { Sext, Zext, Trunc, SiToFp, UiToFp, FpToSi, FpToUi, FpExt, FpTrunc, Bitcast, PtrToInt, IntToPtr };
	auto isInt = [](TypeId t) {
		return nir::isInteger(t);
	};
	auto width = [](TypeId t) {
		return nir::sizeOf(t);
	};
	auto& module = state->trace.module();
	if (from.id == to.id) {
		// the same representation (int32 <-> uint32, ptr <-> ptr): a new variable with the same value
		return emit(NOp::Var, to.id, 0, value);
	}
	if (from.id == TypeId::Bool && isInt(to.id)) {
		return emitCast(value, Zext, to.id);
	}
	if ((isInt(from.id) || from.isFloat) && to.id == TypeId::Bool) {
		// C++ converts to bool by comparing with zero (a NaN is true); truncation to bool is not a NIR cast
		auto zero = module.consts.intern(from.id, nir::Provenance::Literal, 0);
		return emit(NOp::Cmp, TypeId::Bool, 1 /* ne */, value, zero);
	}
	if (from.id == TypeId::Bool && to.isFloat) {
		return emitCast(emitCast(value, Zext, TypeId::I32), UiToFp, to.id);
	}
	if (isInt(from.id) && isInt(to.id)) {
		if (width(to.id) > width(from.id)) {
			return emitCast(value, from.isSigned ? Sext : Zext, to.id);
		}
		return emitCast(value, Trunc, to.id);
	}
	if (isInt(from.id) && to.isFloat) {
		return emitCast(value, from.isSigned ? SiToFp : UiToFp, to.id);
	}
	if (from.isFloat && isInt(to.id)) {
		return emitCast(value, to.isSigned ? FpToSi : FpToUi, to.id);
	}
	if (from.id == TypeId::F32 && to.id == TypeId::F64) {
		return emitCast(value, FpExt, to.id);
	}
	if (from.id == TypeId::F64 && to.id == TypeId::F32) {
		return emitCast(value, FpTrunc, to.id);
	}
	if (from.id == TypeId::Ptr && isInt(to.id)) {
		auto asInt = emitCast(value, PtrToInt, TypeId::I64);
		return to.id == TypeId::I64 ? asInt : emitCast(asInt, Trunc, to.id);
	}
	if (from.id == TypeId::Ptr && to.isFloat) {
		return emitCast(emitCast(value, PtrToInt, TypeId::I64), UiToFp, to.id);
	}
	if (isInt(from.id) && to.id == TypeId::Ptr) {
		auto wide = from.id == TypeId::I64 ? value : emitCast(value, from.isSigned ? Sext : Zext, TypeId::I64);
		return emitCast(wide, IntToPtr, TypeId::Ptr);
	}
	throw RuntimeException(
	    fmt::format("NIR trace: no NIR cast from {} to {}", toString(source.type), toString(target)));
}

nir::Ref NirTracer::emitBinary(NOp op, Type resultType, const TypedValueRef& a, const TypedValueRef& b, uint8_t flags,
                               uint32_t c) {
	auto result = typeInfo(resultType);
	nir::Ref right = b.ref;
	// pointer arithmetic takes a 64-bit byte offset
	if (result.id == TypeId::Ptr && (op == NOp::Add || op == NOp::Sub) && typeInfo(b.type).id != TypeId::I64) {
		right = emitCast(right, typeInfo(b.type).isSigned ? 0 : 1, TypeId::I64);
	}
	return emit(op, result.id, flags, a.ref, right, c);
}

uint32_t NirTracer::pointerEntry(nir::PointerKind kind, const void* address, uint8_t provenance) {
	session_->sessionState_->processLocal = true;
	return state->trace.module().internPointer(
	    nir::PointerRecord {static_cast<uint8_t>(kind), 0, provenance, 0, 0, reinterpret_cast<uintptr_t>(address), 0});
}

uint32_t NirTracer::signatureOf(Type result, const std::vector<TypedValueRef>& arguments) {
	std::vector<nir::TypeWord> params;
	params.reserve(arguments.size());
	for (const auto& a : arguments) {
		params.push_back(typeWord(a.type));
	}
	return state->trace.module().internSignature(typeWord(result), params);
}

uint32_t NirTracer::externalCallee(void* fptn, Type resultType, const std::vector<TypedValueRef>& arguments,
                                   FunctionAttributes fnAttrs) {
	nir::CalleeRecord record {};
	record.kind = static_cast<uint8_t>(nir::CalleeKind::External);
	record.signature = signatureOf(resultType, arguments);
	record.attrs = attrBits(fnAttrs);
	record.target = pointerEntry(nir::PointerKind::Callee, fptn, 0);
	return state->trace.module().internCallee(record);
}

uint32_t NirTracer::internalCallee(uint32_t functionIndex) {
	auto& session = *session_->sessionState_;
	if (session.internalCalleeOf[functionIndex] == nir::NONE) {
		// the signature and the attributes are those of the function, which is only known when it is traced: they are
		// written in when the session ends (SessionState::finish)
		nir::CalleeRecord record {};
		record.kind = static_cast<uint8_t>(nir::CalleeKind::Internal);
		record.target = functionIndex;
		session.module.callees.push_back(record);
		session.internalCalleeOf[functionIndex] = static_cast<uint32_t>(session.module.callees.size() - 1);
	}
	return session.internalCalleeOf[functionIndex];
}

uint32_t NirTracer::cleanupList() {
	auto& module = state->trace.module();
	std::vector<uint32_t> list;
	list.push_back(static_cast<uint32_t>(activeDestructors.size()));
	// destruction order (13 §3.4): the reverse of the order the objects were registered in
	for (auto it = activeDestructors.rbegin(); it != activeDestructors.rend(); ++it) {
		const auto& d = *it;
		nir::CalleeRecord record {};
		record.kind = static_cast<uint8_t>(nir::CalleeKind::External);
		record.target = pointerEntry(nir::PointerKind::Destructor, d.ptr, 0);
		nir::TypeWord param[] = {nir::makeTypeWord(TypeId::Ptr)};
		record.signature = module.internSignature(nir::makeTypeWord(TypeId::Void), param);
		record.attrs = nir::AttrNoUnwind;
		list.push_back(module.internCallee(record));
		list.push_back(d.address.ref);
	}
	return module.addExtra(list);
}

// ---- the traced operations ---------------------------------------------------------------------------------------

TypedValueRef& NirTracer::registerFunctionArgument(Type type, size_t index) {
	if (paused_) {
		++session_->stats_.pausedOperations;
		return dummyRef_;
	}
	if (parent_ != nullptr) {
		throw RuntimeException("Invalid region()" + describeCurrentFunction() +
		                       ": a region body has no arguments of its own.");
	}
	auto& trace = state->trace;
	if (trace.parameters.size() <= index) {
		trace.parameters.resize(index + 1, 0);
	}
	if (trace.parameters[index] == 0) {
		nir::Inst inst {};
		inst.op = static_cast<uint16_t>(NOp::BlockArg);
		inst.type = static_cast<uint8_t>(typeInfo(type).id);
		inst.a = static_cast<uint32_t>(index);
		const auto id = trace.addParameter(inst, type);
		trace.parameters[index] = id;
	}
	result_ = TypedValueRef(trace.parameters[index], type);
	return result_;
}

TypedValueRef& NirTracer::traceConstant(Type type, const ConstantLiteral& value) {
	if (paused_) {
		++session_->stats_.pausedOperations;
		return dummyRef_;
	}
	if (isFollowing()) {
		return follow();
	}
	auto tag = recordSnapshot();
	auto& trace = state->trace;
	auto& module = trace.module();
	const auto existing = trace.tags.find(tag);
	const auto first = trace.nextId();
	const auto info = typeInfo(type);

	nir::Ref created;
	if (info.id == TypeId::Ptr) {
		// a traced pointer is never a constant: it is a ptr.const of a pointers entry (01 §6)
		auto address = std::get<void*>(value);
		if (address == nullptr) {
			created = emit(NOp::PtrConst, TypeId::Ptr, 0, nir::NONE);
		} else {
			const auto entry = pointerEntry(nir::PointerKind::TracedAddress, address,
			                                static_cast<uint8_t>(nir::Provenance::RawProcessValue));
			created = emit(NOp::PtrConst, TypeId::Ptr, 0, entry);
		}
		if (existing != nir::NONE) {
			// the same call site reached on another path: a write onto the variable the first visit made (#95)
			const auto original = TypedValueRef(trace.meta[existing].result, type);
			emit(NOp::Assign, TypeId::Void, 0, original.ref, created);
			return finish(first, tag, original);
		}
		return finish(first, tag, created, type);
	}
	const auto constant = module.consts.intern(info.id, nir::Provenance::Literal, literalBits(value));
	if (existing != nir::NONE) {
		const auto original = TypedValueRef(trace.meta[existing].result, type);
		emit(NOp::Assign, TypeId::Void, 0, original.ref, constant);
		return finish(first, tag, original);
	}
	created = emit(NOp::Var, info.id, 0, constant);
	return finish(first, tag, created, type);
}

TypedValueRef& NirTracer::traceAlloca(size_t size, size_t align) {
	return traceOperation([&, size, align](Snapshot& tag) -> TypedValueRef& {
		auto& trace = state->trace;
		const auto first = trace.nextId();
		const auto index = static_cast<uint32_t>(trace.allocas.size());
		trace.allocas.push_back({static_cast<uint32_t>(size), static_cast<uint32_t>(align)});
		const auto id = emit(NOp::Alloca, TypeId::Ptr, 0, index);
		return finish(first, tag, id, Type::ptr);
	});
}

TypedValueRef& NirTracer::traceCopy(const TypedValueRef& ref) {
	if (paused_) {
		++session_->stats_.pausedOperations;
		return dummyRef_;
	}
	const bool explicitCopy = std::exchange(nextCopyExplicit_, false);
	if (isFollowing()) {
		return follow();
	}
	++(explicitCopy ? session_->stats_.copiesExplicit : session_->stats_.copiesConstruct);
	auto tag = recordSnapshot();
	auto& trace = state->trace;
	const auto first = trace.nextId();
	if (const auto existing = trace.tags.find(tag); existing != nir::NONE) {
		// This copy's call site was already reached by a different path (the other arm of a native `c ? a : b`): the
		// source differs per path, so this is no control-flow merge. It writes the new source onto the variable of the
		// first visit (#95), and the value of the copy stays that variable.
		const auto original = TypedValueRef(trace.meta[existing].result, ref.type);
		emit(NOp::Assign, TypeId::Void, 0, original.ref, ref.ref);
		return finish(first, tag, original);
	}
	const auto id = emit(NOp::Var, typeInfo(ref.type).id, 0, ref.ref);
	return finish(first, tag, id, ref.type);
}

TypedValueRef& NirTracer::traceBinaryOp(Op op, Type resultType, const TypedValueRef& left, const TypedValueRef& right) {
	if (paused_) {
		++session_->stats_.pausedOperations;
		return dummyRef_;
	}
	return traceOperation([&](Snapshot& tag) -> TypedValueRef& {
		auto& trace = state->trace;
		const auto first = trace.nextId();
		auto info = typeInfo(resultType);
		auto cmp = [&](Op comparison) {
			uint8_t predicate = 0;
			bool ordered = false;
			switch (comparison) {
			case EQ:
				break;
			case NEQ:
				predicate = 1;
				break;
			case LT:
				predicate = 2;
				ordered = true;
				break;
			case LTE:
				predicate = 3;
				ordered = true;
				break;
			case GT:
				predicate = 4;
				ordered = true;
				break;
			default:
				predicate = 5;
				ordered = true;
				break;
			}
			auto t = typeInfo(left.type);
			uint8_t flags = predicate;
			if (ordered && nir::isInteger(t.id) && !t.isSigned) {
				flags |= 8;
			}
			return emit(NOp::Cmp, TypeId::Bool, flags, left.ref, right.ref);
		};
		nir::Ref id = 0;
		switch (op) {
		case ADD:
			id = emitBinary(NOp::Add, resultType, left, right);
			break;
		case SUB:
			id = emitBinary(NOp::Sub, resultType, left, right);
			break;
		case MUL:
			id = emitBinary(NOp::Mul, resultType, left, right);
			break;
		case DIV:
		case MOD: {
			const auto nop = op == DIV ? NOp::Div : NOp::Rem;
			if (info.isFloat) {
				id = emitBinary(nop, resultType, left, right);
			} else {
				// an integer division is checked until the conversion proves its divisor safe: it carries the cleanup
				// list of the destructors that are live here (13 §5.10)
				id = emitBinary(nop, resultType, left, right, static_cast<uint8_t>((info.isSigned ? 0 : 1) | 2),
				                cleanupList());
			}
			break;
		}
		case EQ:
		case NEQ:
		case LT:
		case LTE:
		case GT:
		case GTE:
			id = cmp(op);
			break;
		case AND:
			id = emit(NOp::And, TypeId::Bool, 0, left.ref, right.ref);
			break;
		case OR:
			id = emit(NOp::Or, TypeId::Bool, 0, left.ref, right.ref);
			break;
		// val<bool>'s `&`, `|` and `^` combine two values of a 0/1 domain: NIR has no bitwise operation on `bool`
		case BAND:
			id = resultType == Type::b ? emit(NOp::And, TypeId::Bool, 0, left.ref, right.ref)
			                           : emitBinary(NOp::BAnd, resultType, left, right);
			break;
		case BOR:
			id = resultType == Type::b ? emit(NOp::Or, TypeId::Bool, 0, left.ref, right.ref)
			                           : emitBinary(NOp::BOr, resultType, left, right);
			break;
		case BXOR:
			id = resultType == Type::b ? emit(NOp::Cmp, TypeId::Bool, 1 /* ne */, left.ref, right.ref)
			                           : emitBinary(NOp::BXor, resultType, left, right);
			break;
		case LSH:
			id = emitBinary(NOp::Shl, resultType, left, right);
			break;
		case RSH:
			id = emitBinary(NOp::Shr, resultType, left, right, info.isSigned ? 0 : 1);
			break;
		case STORE:
			emit(NOp::Store, TypeId::Void, 0, left.ref, right.ref);
			return finish(first, tag, TypedValueRef(0, Type::v));
		default:
			throw RuntimeException(fmt::format("NIR trace: {} is not a binary operation", toString(op)));
		}
		return finish(first, tag, id, resultType);
	});
}

TypedValueRef& NirTracer::traceUnaryOp(Op op, Type resultType, const TypedValueRef& input) {
	if (paused_) {
		++session_->stats_.pausedOperations;
		return dummyRef_;
	}
	return traceOperation([&](Snapshot& tag) -> TypedValueRef& {
		const auto first = state->trace.nextId();
		nir::Ref id = 0;
		switch (op) {
		case NOT:
			id = emit(NOp::Not, TypeId::Bool, 0, input.ref);
			break;
		case NEGATE: {
			auto t = typeInfo(resultType);
			id = emit(t.isFloat ? NOp::FNeg : NOp::BNot, t.id, 0, input.ref);
			break;
		}
		case CAST:
			id = emitConversion(input, resultType);
			break;
		case LOAD:
			id = emit(NOp::Load, typeInfo(resultType).id, 0, input.ref);
			break;
		default:
			throw RuntimeException(fmt::format("NIR trace: {} is not a unary operation", toString(op)));
		}
		return finish(first, tag, id, resultType);
	});
}

TypedValueRef& NirTracer::traceTernaryOp(Op op, Type resultType, const TypedValueRef& a, const TypedValueRef& b,
                                         const TypedValueRef& c) {
	if (paused_) {
		++session_->stats_.pausedOperations;
		return dummyRef_;
	}
	return traceOperation([&](Snapshot& tag) -> TypedValueRef& {
		if (op != SELECT) {
			throw RuntimeException(fmt::format("NIR trace: {} is not a ternary operation", toString(op)));
		}
		const auto first = state->trace.nextId();
		const auto id = emit(NOp::Select, typeInfo(resultType).id, 0, a.ref, b.ref, c.ref);
		return finish(first, tag, id, resultType);
	});
}

void NirTracer::traceReturnOperation(Type resultType, const TypedValueRef& ref) {
	if (paused_) {
		++session_->stats_.pausedOperations;
		return;
	}
	if (parent_ != nullptr) {
		throw RuntimeException("Invalid region()" + describeCurrentFunction() +
		                       ": a region body returns void and cannot return from the enclosing "
		                       "function; assign to a val<T> captured by reference instead.");
	}
	if (isFollowing()) {
		follow();
		return;
	}
	auto tag = recordSnapshot();
	auto& trace = state->trace;
	const auto first = trace.nextId();
	emit(NOp::Ret, TypeId::Void, 0, ref.type == Type::v ? nir::NONE : ref.ref);
	// the type of what is returned is kept on the record, for the signature
	trace.markOperation(first, &tag, TypedValueRef(0, ref.type == Type::v ? resultType : ref.type));
	if (ref.type == Type::v) {
		trace.meta[first].type = Type::v;
	}
}

void NirTracer::traceAssignment(const TypedValueRef& target, const TypedValueRef& source, Type) {
	if (paused_) {
		++session_->stats_.pausedOperations;
		return;
	}
	if (isFollowing()) {
		follow();
		return;
	}
	++session_->stats_.copiesAssign;
	auto tag = recordSnapshot();
	auto& trace = state->trace;
	// A tag match does not by itself prove this assignment re-enters a loop or an if/else merge: the same call-stack
	// shape can be reached while assigning a different target (two sibling arguments of one call, two slots of an
	// array assigned in one loop, #382). Only an assignment of the same target at the same tag is a re-entry.
	if (const auto existing = trace.tags.find(tag); existing != nir::NONE) {
		const auto& record = trace.recs[existing];
		if (record.op != static_cast<uint16_t>(NOp::Assign) || record.a != target.ref) {
			const auto first = trace.nextId();
			emit(NOp::Assign, TypeId::Void, 0, target.ref, source.ref);
			trace.markOperation(first, &tag, TypedValueRef(0, Type::v));
			return;
		}
	}
	if (!trace.checkTag(tag)) {
		pause();
		return;
	}
	const auto first = trace.nextId();
	emit(NOp::Assign, TypeId::Void, 0, target.ref, source.ref);
	trace.markOperation(first, &tag, TypedValueRef(0, Type::v));
}

bool NirTracer::traceBool(const TypedValueRef& value, double probability) {
	if (paused_) {
		++session_->stats_.pausedOperations;
		// in passive mode, return false to guarantee loop termination
		return false;
	}
	auto& trace = state->trace;
	bool result;
	bool shouldTerminate = false;
	if (state->symbolicExecutionContext.getCurrentMode() == SymbolicExecutionContext::MODE::FOLLOW) {
		++session_->stats_.replayedOperations;
		auto recordResult = state->symbolicExecutionContext.follow();
		result = recordResult.branchDirection;
		shouldTerminate = recordResult.shouldTerminate;
		// the last decision of the path is the one that flips: from here on the pass records
		if (state->symbolicExecutionContext.getCurrentMode() != SymbolicExecutionContext::MODE::FOLLOW) {
			enterPhase(Phase::Record);
		}
	} else {
		auto tag = recordSnapshot();
		if (trace.checkTag(tag)) {
			// two new blocks, one per direction, and the branch that ends the current one
			const auto trueBlock = trace.createBlock();
			trace.blocks[trueBlock].preds.push_back(trace.currentBlock());
			const auto falseBlock = trace.createBlock();
			trace.blocks[falseBlock].preds.push_back(trace.currentBlock());
			const auto condIndex = static_cast<uint32_t>(trace.conds.size());
			trace.conds.push_back({probabilityWord(probability), falseBlock});
			const auto first = trace.nextId();
			emit(NOp::CondBr, TypeId::Void, 0, value.ref, trueBlock, condIndex);
			trace.markOperation(first, &tag, TypedValueRef(0, Type::v));
			auto recordResult = state->symbolicExecutionContext.record(tag);
			result = recordResult.branchDirection;
			shouldTerminate = recordResult.shouldTerminate;
		} else {
			// a control-flow merge or loop: passive mode
			pause();
			return false;
		}
	}
	if (shouldTerminate) {
		pause();
		return false;
	}
	const auto& branch = trace.currentRecord();
	assert(branch.op == static_cast<uint16_t>(NOp::CondBr));
	trace.setCurrentBlock(result ? branch.b : trace.conds[branch.c].falseBlock);
	return result;
}

TypedValueRef& NirTracer::traceCall(void* fptn, Type resultType, const std::vector<TypedValueRef>& arguments,
                                    FunctionAttributes fnAttrs) {
	if (paused_) {
		++session_->stats_.pausedOperations;
		return dummyRef_;
	}
	return traceOperation([&](Snapshot& tag) -> TypedValueRef& {
		const auto first = state->trace.nextId();
		const auto callee = externalCallee(fptn, resultType, arguments, fnAttrs);
		return recordCallRecord(first, tag, callee, false, resultType, arguments, fnAttrs, true, nullptr);
	});
}

TypedValueRef& NirTracer::traceCallWithExceptionHandling(void* fptn, Type resultType,
                                                         const std::vector<TypedValueRef>& arguments,
                                                         FunctionAttributes fnAttrs, void* captureFunc) {
	if (paused_) {
		++session_->stats_.pausedOperations;
		return dummyRef_;
	}
	return traceOperation([&](Snapshot& tag) -> TypedValueRef& {
		const auto first = state->trace.nextId();
		const auto callee = externalCallee(fptn, resultType, arguments, fnAttrs);
		return recordCallRecord(first, tag, callee, true, resultType, arguments, fnAttrs, true, captureFunc);
	});
}

TypedValueRef& NirTracer::traceIndirectCall(const TypedValueRef& fnPtrRef, Type resultType,
                                            const std::vector<TypedValueRef>& arguments, FunctionAttributes fnAttrs,
                                            void* captureFunc) {
	if (paused_) {
		++session_->stats_.pausedOperations;
		return dummyRef_;
	}
	return traceOperation([&](Snapshot& tag) -> TypedValueRef& {
		return recordIndirectCall(tag, false, fnPtrRef, resultType, arguments, fnAttrs, captureFunc);
	});
}

TypedValueRef& NirTracer::traceIndirectCallWithExceptionHandling(const TypedValueRef& fnPtrRef, Type resultType,
                                                                 const std::vector<TypedValueRef>& arguments,
                                                                 FunctionAttributes fnAttrs, void* captureFunc) {
	if (paused_) {
		++session_->stats_.pausedOperations;
		return dummyRef_;
	}
	return traceOperation([&](Snapshot& tag) -> TypedValueRef& {
		return recordIndirectCall(tag, true, fnPtrRef, resultType, arguments, fnAttrs, captureFunc);
	});
}

TypedValueRef& NirTracer::recordCallRecord(uint32_t first, const Snapshot& tag, uint32_t callee, bool exceptionHandling,
                                           Type resultType, const std::vector<TypedValueRef>& arguments,
                                           FunctionAttributes fnAttrs, bool external, void* captureFunc) {
	const bool mayThrow = exceptionHandling && !fnAttrs.noUnwind;
	std::vector<uint32_t> words;
	words.push_back(static_cast<uint32_t>(arguments.size()));
	for (const auto& a : arguments) {
		words.push_back(a.ref);
	}
	uint32_t thunk = nir::NONE;
	if (mayThrow) {
		words.push_back(cleanupList());
		if (external) {
			if (captureFunc == nullptr) {
				throw RuntimeException("NIR trace: a may-throw external call has no capture thunk");
			}
			thunk = pointerEntry(nir::PointerKind::CaptureThunk, captureFunc, 0);
		}
	} else {
		words.push_back(nir::NO_UNWIND);
	}
	words.push_back(thunk);
	const auto id = emit(NOp::Call, typeInfo(resultType).id, 0, callee, 0, state->trace.module().addExtra(words));
	return finish(first, tag, id, resultType);
}

TypedValueRef& NirTracer::recordIndirectCall(const Snapshot& tag, bool exceptionHandling, const TypedValueRef& fnPtrRef,
                                             Type resultType, const std::vector<TypedValueRef>& arguments,
                                             FunctionAttributes fnAttrs, void* captureFunc) {
	auto& trace = state->trace;
	const auto first = trace.nextId();
	const auto signature = signatureOf(resultType, arguments);
	const bool mayThrow = exceptionHandling && !fnAttrs.noUnwind;
	std::vector<uint32_t> words;
	words.push_back(static_cast<uint32_t>(arguments.size()));
	for (const auto& a : arguments) {
		words.push_back(a.ref);
	}
	uint32_t thunk = nir::NONE;
	if (mayThrow) {
		words.push_back(cleanupList());
		if (captureFunc == nullptr) {
			throw RuntimeException("NIR trace: a may-throw indirect call has no capture thunk");
		}
		thunk = pointerEntry(nir::PointerKind::CaptureThunk, captureFunc, 0);
	} else {
		words.push_back(nir::NO_UNWIND);
	}
	words.push_back(thunk);
	words.push_back(attrBits(fnAttrs));
	const auto id =
	    emit(NOp::CallIndirect, typeInfo(resultType).id, 0, fnPtrRef.ref, signature, trace.module().addExtra(words));
	return finish(first, tag, id, resultType);
}

uint32_t NirTracer::nautilusFunctionIndex(const NautilusFunctionDefinition* definition, std::function<void()> fwrapper,
                                          FunctionAttributes fnAttrs, bool takesAddress) {
	auto& session = *session_->sessionState_;
	// the name of the definition, registered for tracing on first sight (two functions of one name get distinct ones)
	std::string name;
	if (const auto it = session.registeredFunctions.find(definition); it != session.registeredFunctions.end()) {
		name = it->second;
	} else {
		name = definition->name();
		if (session.usedFunctionNames.contains(name)) {
			const std::string base = name;
			uint32_t suffix = 1;
			do {
				++suffix;
				name = base + "_" + std::to_string(suffix);
			} while (session.usedFunctionNames.contains(name));
			log::warn("Two distinct NautilusFunctions are named '{}'; tracing the second as '{}'. Give them distinct "
			          "names to keep generated code readable.",
			          base, name);
		}
		session.usedFunctionNames.insert(name);
		session.registeredFunctions.emplace(definition, name);
		session.functionsToTrace.push_back(compiler::CompilableFunction(
		    name, std::move(fwrapper), definition->attributes(), definition, definition->location()));
	}
	const auto index = session.indexOf(name);
	// what every call of the function states is what the function may be assumed to guarantee; taking the address
	// makes no promise
	session.noteCallAttributes(index, takesAddress ? 0u : attrBits(fnAttrs));
	return index;
}

TypedValueRef& NirTracer::traceNautilusCall(const NautilusFunctionDefinition* definition,
                                            std::function<void()> fwrapper, Type resultType,
                                            const std::vector<TypedValueRef>& arguments, FunctionAttributes fnAttrs) {
	if (paused_) {
		++session_->stats_.pausedOperations;
		return dummyRef_;
	}
	const auto function = nautilusFunctionIndex(definition, std::move(fwrapper), fnAttrs, false);
	return traceOperation([&](Snapshot& tag) -> TypedValueRef& {
		const auto first = state->trace.nextId();
		return recordCallRecord(first, tag, internalCallee(function), false, resultType, arguments, fnAttrs, false,
		                        nullptr);
	});
}

TypedValueRef& NirTracer::traceNautilusCallWithExceptionHandling(const NautilusFunctionDefinition* definition,
                                                                 std::function<void()> fwrapper, Type resultType,
                                                                 const std::vector<TypedValueRef>& arguments,
                                                                 FunctionAttributes fnAttrs) {
	if (paused_) {
		++session_->stats_.pausedOperations;
		return dummyRef_;
	}
	const auto function = nautilusFunctionIndex(definition, std::move(fwrapper), fnAttrs, false);
	return traceOperation([&](Snapshot& tag) -> TypedValueRef& {
		const auto first = state->trace.nextId();
		return recordCallRecord(first, tag, internalCallee(function), true, resultType, arguments, fnAttrs, false,
		                        nullptr);
	});
}

TypedValueRef& NirTracer::traceNautilusFunctionPtr(const NautilusFunctionDefinition* definition,
                                                   std::function<void()> fwrapper) {
	if (paused_) {
		++session_->stats_.pausedOperations;
		return dummyRef_;
	}
	const auto function = nautilusFunctionIndex(definition, std::move(fwrapper), {}, true);
	return traceOperation([&](Snapshot& tag) -> TypedValueRef& {
		const auto first = state->trace.nextId();
		const auto id = emit(NOp::FuncAddr, TypeId::Ptr, 0, internalCallee(function));
		return finish(first, tag, id, Type::ptr);
	});
}

TypedValueRef& NirTracer::traceData(std::span<const std::byte> bytes, size_t align) {
	return traceOperation([&](Snapshot& tag) -> TypedValueRef& {
		auto& trace = state->trace;
		const auto first = trace.nextId();
		const auto index = trace.module().internData(bytes, static_cast<uint32_t>(align));
		const auto id = emit(NOp::DataAddr, TypeId::Ptr, 0, index);
		return finish(first, tag, id, Type::ptr);
	});
}

TypedValueRef& NirTracer::traceBinding(std::string_view name, size_t size, size_t align) {
	return traceOperation([&](Snapshot& tag) -> TypedValueRef& {
		auto& trace = state->trace;
		auto& session = *session_->sessionState_;
		const auto first = trace.nextId();
		auto it = session.bindingIndex.find(std::string(name));
		if (it == session.bindingIndex.end()) {
			const auto index = trace.module().addBinding(trace.module().internName(name), TypeId::Void,
			                                             static_cast<uint32_t>(size), static_cast<uint32_t>(align));
			it = session.bindingIndex.emplace(std::string(name), index).first;
		}
		const auto id = emit(NOp::BindAddr, TypeId::Ptr, 0, it->second);
		return finish(first, tag, id, Type::ptr);
	});
}

// ---- destructors, static and alive variables --------------------------------------------------------------------

void NirTracer::registerDestructor(const TypedValueRef& address, void* destructor) {
	activeDestructors.push_back(Destructor {address, destructor});
}

void NirTracer::unregisterDestructor(const TypedValueRef& address) {
	auto it = std::find_if(activeDestructors.rbegin(), activeDestructors.rend(),
	                       [&](const Destructor& destructor) { return destructor.address == address; });
	if (it != activeDestructors.rend()) {
		activeDestructors.erase(std::next(it).base());
	}
}

void NirTracer::transferDestructor(const TypedValueRef& from, const TypedValueRef& to) {
	auto it = std::find_if(activeDestructors.rbegin(), activeDestructors.rend(),
	                       [&](const Destructor& destructor) { return destructor.address == from; });
	if (it != activeDestructors.rend()) {
		it->address = to;
	}
}

void NirTracer::allocateValRef(ValueRef ref) {
	if (paused_) {
		return;
	}
	aliveVars.increment(ref);
}

void NirTracer::freeValRef(ValueRef ref) {
	if (paused_) {
		return;
	}
	if (parent_ != nullptr && !aliveVars.isAlive(ref)) {
		// A value created outside this region and released inside it is counted by the scope that allocated it.
		for (auto* scope = parent_; scope != nullptr; scope = scope->parent_) {
			if (scope->aliveVars.isAlive(ref)) {
				scope->aliveVars.decrement(ref);
				return;
			}
		}
	}
	aliveVars.decrement(ref);
}

void NirTracer::pushStaticVal(void* valPtr, size_t size) {
	// the stack is kept in passive mode too: the destructors of static values pushed before it pop after
	staticVars.emplace_back(valPtr, size);
}

void NirTracer::popStaticVal() {
	staticVars.pop_back();
}

// ---- regions -----------------------------------------------------------------------------------------------------

std::string NirTracer::describeCurrentFunction() const {
	const auto& session = *session_->sessionState_;
	if (session.currentFunctionName.empty()) {
		return {};
	}
	std::string description = " in function '" + session.currentFunctionName + "'";
	if (session.currentFunctionLocation.isKnown()) {
		description += " (registered at " + session.currentFunctionLocation.toString() + ")";
	}
	return description;
}

NirTracer::RegionScopeState& NirTracer::regionState() {
	if (!regionState_) {
		regionState_ = std::make_unique<RegionScopeState>();
	}
	return *regionState_;
}

NirTracer& NirTracer::acquireChildScope() {
	auto& region = regionState();
	if (!region.childScope) {
		region.childScope = std::make_unique<NirTracer>();
	}
	return *region.childScope;
}

void NirTracer::initRegionScope(NirTracer& parent, uint32_t entry, uint32_t exit, TagRecorder& recorder,
                                const RegionAttributes& attributes) {
	parent_ = &parent;
	session_ = parent.session_;
	entryBlock_ = entry;
	paused_ = false;
	staticVars.clear();
	aliveVars.reset();
	auto& region = regionState();
	region.exitBlock = exit;
	region.attributes = attributes;
	region.exitSnapshot.reset();
	region.regionMemo.clear();
	region.symbolicExecutionContext.reset();
	state.emplace(TraceScope {recorder, parent.state->trace, region.symbolicExecutionContext, parent.state->options});
}

void NirTracer::traceScopeExit() {
	if (paused_ || isFollowing()) {
		return;
	}
	auto& region = regionState();
	// Nothing created inside a region may outlive it (docs/region.md).
	if (aliveVars.size() > 0) {
		std::string escaped;
		aliveVars.forEachAlive([&escaped](uint32_t ref, uint32_t) {
			escaped += (escaped.empty() ? "" : ", ") + std::string("$") + std::to_string(ref);
		});
		throw RuntimeException(
		    "Invalid region() " + region.attributes.toString() + describeCurrentFunction() +
		    ": a value created inside the region body outlives it (" + escaped +
		    "). Carry the value out through a val<T> declared outside the region and assigned to inside it.");
	}
	auto snapshot = recordSnapshot();
	if (!region.exitSnapshot.has_value()) {
		region.exitSnapshot = snapshot;
	} else if (*region.exitSnapshot != snapshot) {
		throw RuntimeException(
		    "Invalid region() " + region.attributes.toString() + describeCurrentFunction() +
		    ": the state alive at the end of the region body differs between the paths through it, so what escapes the "
		    "region would depend on which path was explored last. Build the escaping value on every path (assigning to "
		    "a val<T> declared outside the region merges across branches).");
	}
	auto& trace = state->trace;
	if (!trace.checkTag(snapshot)) {
		// this pass's tail was merged with an earlier pass's exit: the pass is over
		pause();
		return;
	}
	trace.blocks[region.exitBlock].preds.push_back(trace.currentBlock());
	trace.addJump(region.exitBlock, &snapshot);
}

void NirTracer::traceRegion(std::function<void()>& regionFunction, const RegionAttributes& attributes) {
	if (paused_) {
		++session_->stats_.pausedOperations;
		return;
	}
	auto& trace = state->trace;
	// a region is an operation to its enclosing scope and a function to its own body: identified at its call site
	auto key = recordSnapshot();

	if (isFollowing()) {
		// memoized replay: the body was fully explored when it was first reached, so it is skipped
		auto& memo = regionState().regionMemo;
		auto memoized = memo.find(key);
		if (memoized == memo.end()) {
			throw RuntimeException("Invalid region() " + attributes.toString() + describeCurrentFunction() +
			                       ": replaying a recorded path reached a region() call site that was not recorded "
			                       "there.");
		}
		trace.setCurrentBlock(memoized->second.exitBlock);
		return;
	}

	if (!trace.checkTag(key)) {
		// the same region call site in the same state: a loop around the region; checkTag has merged
		pause();
		return;
	}

	auto entry = trace.createBlock();
	auto exit = trace.createBlock();
	trace.blocks[entry].preds.push_back(trace.currentBlock());
	trace.addJump(entry, &key);
	trace.setCurrentBlock(entry);

	// the recorder is rooted at this frame's return address: tags in the body are the path from the region entry
	auto& recorder = session_->regionState().tagRecorders.emplace_back(
	    reinterpret_cast<TagAddress>(__builtin_return_address(0)), *session_->arena_);
	auto& child = acquireChildScope();
	child.initRegionScope(*this, entry, exit, recorder, attributes);

	auto regionIndex = trace.addRegion(attributes, entry);
	auto enclosingRegion = trace.setCurrentRegion(regionIndex);
	setActiveTracer(&child);
	try {
		child.runScope(regionFunction);
	} catch (...) {
		setActiveTracer(this);
		trace.setCurrentRegion(enclosingRegion);
		throw;
	}
	setActiveTracer(this);
	trace.setCurrentRegion(enclosingRegion);

	// the body may have released or re-keyed destructors this scope registered; take the updated list back
	activeDestructors = std::move(child.activeDestructors);

	if (trace.blocks[exit].preds.empty()) {
		throw RuntimeException("Invalid region() " + attributes.toString() + describeCurrentFunction() +
		                       ": no path through the region body reached its end, so the enclosing function cannot "
		                       "continue after it.");
	}

	child.state.reset();
	trace.setCurrentBlock(exit);
	regionState().regionMemo[key] = RegionRecord {entry, exit};
}

} // namespace nautilus::tracing::nirtrace

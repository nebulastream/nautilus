#include "nautilus/tracing/nir/NirTrace.hpp"
#include "nautilus/exceptions/RuntimeException.hpp"
#include "nautilus/nir/Builder.hpp"
#include <algorithm>
#include <bit>
#include <cmath>
#include <cstring>
#include <fmt/format.h>
#include <map>
#include <unordered_map>

namespace nautilus::tracing::nirtrace {

namespace nir = ::nautilus::nir;

TypeInfo typeInfo(Type type) {
	switch (type) {
	case Type::v:
		return {nir::TypeId::Void, false, false};
	case Type::b:
		return {nir::TypeId::Bool, false, false};
	case Type::i8:
		return {nir::TypeId::I8, true, false};
	case Type::i16:
		return {nir::TypeId::I16, true, false};
	case Type::i32:
		return {nir::TypeId::I32, true, false};
	case Type::i64:
		return {nir::TypeId::I64, true, false};
	case Type::ui8:
		return {nir::TypeId::I8, false, false};
	case Type::ui16:
		return {nir::TypeId::I16, false, false};
	case Type::ui32:
		return {nir::TypeId::I32, false, false};
	case Type::ui64:
		return {nir::TypeId::I64, false, false};
	case Type::f32:
		return {nir::TypeId::F32, false, true};
	case Type::f64:
		return {nir::TypeId::F64, false, true};
	case Type::ptr:
		return {nir::TypeId::Ptr, false, false};
	}
	throw RuntimeException("NIR trace: unknown type");
}

nir::TypeWord typeWord(Type type) {
	auto info = typeInfo(type);
	switch (info.id) {
	case nir::TypeId::Bool:
		return nir::makeTypeWord(info.id, nir::ExtKind::Zext);
	case nir::TypeId::I8:
	case nir::TypeId::I16:
	case nir::TypeId::I32:
		return nir::makeTypeWord(info.id, info.isSigned ? nir::ExtKind::Sext : nir::ExtKind::Zext);
	default:
		return nir::makeTypeWord(info.id);
	}
}

namespace {

[[noreturn]] void fail(const std::string& function, const std::string& message) {
	throw RuntimeException(fmt::format("NIR trace of '{}': {}", function, message));
}

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
			    // the zero-extended bit pattern of the value's own width
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

/// Everything that is shared by the functions of a module while it is converted.
struct Summary {
	Type result = Type::v;
	std::vector<Type> params;
};

/// The signature of a traced function: the parameters of its entry block and the type of what it returns.
Summary summarize(const ExecutionTrace& trace) {
	Summary summary;
	std::vector<TypedValueRef> parameters;
	if (!trace.blocks.empty()) {
		parameters.assign(trace.blocks[0]->arguments.begin(), trace.blocks[0]->arguments.end());
	}
	std::sort(parameters.begin(), parameters.end());
	for (const auto& p : parameters) {
		summary.params.push_back(p.type);
	}
	for (const auto* block : trace.blocks) {
		for (const auto* op : block->operations) {
			if (op->op == Op::RETURN && !op->input.empty()) {
				summary.result = std::get<TypedValueRef>(op->input[0]).type;
			}
		}
	}
	return summary;
}

struct ModuleState {
	nir::Module m;
	/// the attributes of an internal function: what every call of it states (the guarantees they share)
	std::unordered_map<std::string, uint32_t> internalAttrs;
	std::unordered_map<std::string, Summary> summaries;
	std::unordered_map<std::string, uint32_t> functionIndex;
	std::map<const void*, uint32_t> internalCallee; // NautilusFunctionDefinition identity or name -> callee index
	std::map<std::string, uint32_t> internalCalleeByName;
	bool processLocal = false;
};

class FunctionConverter {
public:
	FunctionConverter(ModuleState& state, const ExecutionTrace& trace, const std::string& name, bool exported)
	    : s_(state), m_(state.m), b_(state.m), trace_(trace), name_(name), exported_(exported) {
	}

	void run();

private:
	using TypeId = nir::TypeId;
	using NOp = nir::Op;

	const Block& block(uint32_t id) const {
		if (id >= trace_.blocks.size()) {
			fail(name_, fmt::format("block id {} is outside the {}-block trace", id, trace_.blocks.size()));
		}
		return *trace_.blocks[id];
	}

	void orderBlocks();
	void collectTypes();
	void emitBlock(uint32_t traceBlock);
	void emitOperation(const TraceOperation& op, uint32_t traceBlock);

	nir::Ref valueOf(const TypedValueRef& ref) const {
		if (ref.ref >= idOf_.size() || idOf_[ref.ref] == nir::NONE) {
			fail(name_, fmt::format("${} is used before its definition", ref.ref));
		}
		return idOf_[ref.ref];
	}
	void define(ValueRef ref, nir::Ref id) {
		if (ref >= idOf_.size()) {
			idOf_.resize(ref + 1, nir::NONE);
		}
		idOf_[ref] = id;
	}
	Type typeOfRef(const TypedValueRef& ref) const {
		return ref.type;
	}

	nir::Ref binary(NOp op, Type resultType, const TypedValueRef& a, const TypedValueRef& b, uint8_t flags = 0,
	                uint32_t c = 0);
	nir::Ref cast(const TypedValueRef& source, Type target);
	nir::Ref emitCast(nir::Ref value, uint8_t kind, TypeId to);
	nir::Ref emitCall(const TraceOperation& op);
	nir::Ref emitIndirectCall(const TraceOperation& op);
	uint32_t cleanupList(const std::vector<FunctionCall::Destructor>& destructors);
	uint32_t calleeFor(const FunctionCall& call, Type resultType);
	uint32_t pointerFor(nir::PointerKind kind, const void* address, uint8_t provenance);
	uint32_t signatureFor(Type result, const std::vector<TypedValueRef>& arguments);
	nir::Ref cmpFlags(Op op, const TypedValueRef& operand, uint8_t& flags);

	ModuleState& s_;
	nir::Module& m_;
	nir::Builder b_;
	const ExecutionTrace& trace_;
	std::string name_;
	bool exported_;

	std::vector<uint32_t> order_;      // trace block ids in layout order
	std::vector<uint32_t> newBlockId_; // trace block id -> NIR block id
	std::vector<nir::Ref> idOf_;       // value ref -> instruction id
	Type returnType_ = Type::v;
};

void FunctionConverter::orderBlocks() {
	// reverse postorder of the control flow graph from the entry block: a definition then comes before the uses it
	// dominates, which is the order in which the instruction ids of a pre-SSA function must be assigned (13 §4.3)
	auto n = trace_.blocks.size();
	std::vector<uint8_t> state(n, 0);
	std::vector<uint32_t> post;
	struct Frame {
		uint32_t block;
		uint32_t next;
		std::vector<uint32_t> successors;
	};
	auto successorsOf = [&](uint32_t id) {
		std::vector<uint32_t> result;
		const auto& ops = block(id).operations;
		if (ops.empty()) {
			return result;
		}
		const auto& last = *ops.back();
		if (last.op == Op::JMP) {
			result.push_back(std::get<BlockRef*>(last.input[0])->block);
		} else if (last.op == Op::CMP) {
			// the false target first, so that the true target is laid out first
			result.push_back(std::get<BlockRef*>(last.input[2])->block);
			result.push_back(std::get<BlockRef*>(last.input[1])->block);
		}
		return result;
	};
	std::vector<Frame> stack;
	stack.push_back({0, 0, successorsOf(0)});
	state[0] = 1;
	while (!stack.empty()) {
		auto& top = stack.back();
		if (top.next < top.successors.size()) {
			auto next = top.successors[top.next++];
			if (next >= n) {
				fail(name_, fmt::format("block id {} is outside the {}-block trace", next, n));
			}
			if (state[next] == 0) {
				state[next] = 1;
				stack.push_back({next, 0, successorsOf(next)});
			}
		} else {
			post.push_back(top.block);
			stack.pop_back();
		}
	}
	order_.assign(post.rbegin(), post.rend());
	newBlockId_.assign(n, nir::NONE);
	for (uint32_t i = 0; i < order_.size(); ++i) {
		newBlockId_[order_[i]] = i;
	}
}

void FunctionConverter::collectTypes() {
	// the result type of the function: the type of what its returns return
	for (uint32_t id : order_) {
		for (const auto* op : block(id).operations) {
			if (op->op == Op::RETURN && !op->input.empty()) {
				returnType_ = std::get<TypedValueRef>(op->input[0]).type;
			}
		}
	}
}

uint32_t FunctionConverter::pointerFor(nir::PointerKind kind, const void* address, uint8_t provenance) {
	s_.processLocal = true;
	return m_.internPointer(
	    nir::PointerRecord {static_cast<uint8_t>(kind), 0, provenance, 0, 0, reinterpret_cast<uintptr_t>(address), 0});
}

uint32_t FunctionConverter::signatureFor(Type result, const std::vector<TypedValueRef>& arguments) {
	std::vector<nir::TypeWord> params;
	params.reserve(arguments.size());
	for (const auto& a : arguments) {
		params.push_back(typeWord(a.type));
	}
	return m_.internSignature(typeWord(result), params);
}

uint32_t FunctionConverter::calleeFor(const FunctionCall& call, Type resultType) {
	nir::CalleeRecord record {};
	record.signature = signatureFor(resultType, call.arguments);
	record.attrs = attrBits(call.fnAttrs);
	if (call.kind == CalleeKind::Internal) {
		auto it = s_.functionIndex.find(call.functionName);
		if (it == s_.functionIndex.end()) {
			fail(name_, "call of the unknown function '" + call.functionName + "'");
		}
		// an internal callee has the signature of its function
		const auto& callee = s_.summaries.at(call.functionName);
		std::vector<nir::TypeWord> params;
		for (auto p : callee.params) {
			params.push_back(typeWord(p));
		}
		record.signature = m_.internSignature(typeWord(callee.result), params);
		record.attrs = s_.internalAttrs.at(call.functionName);
		record.kind = static_cast<uint8_t>(nir::CalleeKind::Internal);
		record.target = it->second;
	} else {
		record.kind = static_cast<uint8_t>(nir::CalleeKind::External);
		record.target = pointerFor(nir::PointerKind::Callee, call.ptr, 0);
	}
	return m_.internCallee(record);
}

uint32_t FunctionConverter::cleanupList(const std::vector<FunctionCall::Destructor>& destructors) {
	std::vector<uint32_t> list;
	list.push_back(static_cast<uint32_t>(destructors.size()));
	for (const auto& d : destructors) {
		nir::CalleeRecord record {};
		record.kind = static_cast<uint8_t>(nir::CalleeKind::External);
		record.target = pointerFor(nir::PointerKind::Destructor, d.ptr, 0);
		nir::TypeWord param[] = {nir::makeTypeWord(TypeId::Ptr)};
		record.signature = m_.internSignature(nir::makeTypeWord(TypeId::Void), param);
		record.attrs = nir::AttrNoUnwind;
		list.push_back(m_.internCallee(record));
		list.push_back(valueOf(d.address));
	}
	return m_.addExtra(list);
}

nir::Ref FunctionConverter::emitCast(nir::Ref value, uint8_t kind, TypeId to) {
	return b_.emit(NOp::Cast, to, kind, value);
}

nir::Ref FunctionConverter::cast(const TypedValueRef& source, Type target) {
	auto from = typeInfo(source.type);
	auto to = typeInfo(target);
	auto value = valueOf(source);
	enum : uint8_t { Sext, Zext, Trunc, SiToFp, UiToFp, FpToSi, FpToUi, FpExt, FpTrunc, Bitcast, PtrToInt, IntToPtr };
	auto isInt = [](TypeId t) {
		return nir::isInteger(t);
	};
	auto width = [](TypeId t) {
		return nir::sizeOf(t);
	};

	if (from.id == to.id) {
		// same representation (int32 <-> uint32, ptr <-> ptr): a new variable with the same value
		return b_.emit(NOp::Var, to.id, 0, value);
	}
	if (from.id == TypeId::Bool && isInt(to.id)) {
		return emitCast(value, Zext, to.id);
	}
	if ((isInt(from.id) || from.isFloat) && to.id == TypeId::Bool) {
		// C++ converts to bool by comparing with zero (a NaN is true); truncation to bool is not a NIR cast
		auto zero = m_.consts.intern(from.id, nir::Provenance::Literal, 0);
		return b_.emit(NOp::Cmp, TypeId::Bool, 1 /* ne */, value, zero);
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
		// the address as a number: ptrtoint, then uitofp
		return emitCast(emitCast(value, PtrToInt, TypeId::I64), UiToFp, to.id);
	}
	if (isInt(from.id) && to.id == TypeId::Ptr) {
		auto wide = from.id == TypeId::I64 ? value : emitCast(value, from.isSigned ? Sext : Zext, TypeId::I64);
		return emitCast(wide, IntToPtr, TypeId::Ptr);
	}
	fail(name_, fmt::format("no NIR cast from {} to {}", toString(source.type), toString(target)));
}

nir::Ref FunctionConverter::binary(NOp op, Type resultType, const TypedValueRef& a, const TypedValueRef& b,
                                   uint8_t flags, uint32_t c) {
	auto result = typeInfo(resultType);
	auto left = valueOf(a);
	auto right = valueOf(b);
	// pointer arithmetic takes a 64-bit byte offset
	if (result.id == TypeId::Ptr && (op == NOp::Add || op == NOp::Sub) && typeInfo(b.type).id != TypeId::I64) {
		right = emitCast(right, typeInfo(b.type).isSigned ? 0 : 1, TypeId::I64);
	}
	return b_.emit(op, result.id, flags, left, right, c);
}

nir::Ref FunctionConverter::emitCall(const TraceOperation& op) {
	const auto& call = *std::get<FunctionCall*>(op.input[0]);
	auto callee = calleeFor(call, op.resultType);
	bool mayThrow = op.op == Op::CALL_WITH_EXCEPTION_HANDLING && !call.fnAttrs.noUnwind;
	std::vector<uint32_t> words;
	words.push_back(static_cast<uint32_t>(call.arguments.size()));
	for (const auto& a : call.arguments) {
		words.push_back(valueOf(a));
	}
	uint32_t thunk = nir::NONE;
	if (mayThrow) {
		words.push_back(cleanupList(call.destructors));
		if (call.kind == CalleeKind::External) {
			if (call.captureFunc == nullptr) {
				fail(name_, "a may-throw external call has no capture thunk");
			}
			thunk = pointerFor(nir::PointerKind::CaptureThunk, call.captureFunc, 0);
		}
	} else {
		words.push_back(nir::NO_UNWIND);
	}
	words.push_back(thunk);
	return b_.emit(NOp::Call, typeInfo(op.resultType).id, 0, callee, 0, m_.addExtra(words));
}

nir::Ref FunctionConverter::emitIndirectCall(const TraceOperation& op) {
	const auto& call = *std::get<IndirectFunctionCall*>(op.input[0]);
	auto sig = signatureFor(op.resultType, call.arguments);
	bool mayThrow = op.op == Op::INDIRECT_CALL_WITH_EXCEPTION_HANDLING && !call.fnAttrs.noUnwind;
	std::vector<uint32_t> words;
	words.push_back(static_cast<uint32_t>(call.arguments.size()));
	for (const auto& a : call.arguments) {
		words.push_back(valueOf(a));
	}
	uint32_t thunk = nir::NONE;
	if (mayThrow) {
		words.push_back(cleanupList(call.destructors));
		if (call.captureFunc == nullptr) {
			fail(name_, "a may-throw indirect call has no capture thunk");
		}
		thunk = pointerFor(nir::PointerKind::CaptureThunk, call.captureFunc, 0);
	} else {
		words.push_back(nir::NO_UNWIND);
	}
	words.push_back(thunk);
	words.push_back(attrBits(call.fnAttrs));
	return b_.emit(NOp::CallIndirect, typeInfo(op.resultType).id, 0, valueOf(call.fnPtr), sig, m_.addExtra(words));
}

nir::Ref FunctionConverter::cmpFlags(Op op, const TypedValueRef& operand, uint8_t& flags) {
	uint8_t predicate = 0;
	bool ordered = false;
	switch (op) {
	case Op::EQ:
		predicate = 0;
		break;
	case Op::NEQ:
		predicate = 1;
		break;
	case Op::LT:
		predicate = 2;
		ordered = true;
		break;
	case Op::LTE:
		predicate = 3;
		ordered = true;
		break;
	case Op::GT:
		predicate = 4;
		ordered = true;
		break;
	case Op::GTE:
		predicate = 5;
		ordered = true;
		break;
	default:
		fail(name_, "not a comparison");
	}
	auto t = typeInfo(operand.type);
	flags = predicate;
	if (ordered && nir::isInteger(t.id) && !t.isSigned) {
		flags |= 8;
	}
	return 0;
}

void FunctionConverter::emitOperation(const TraceOperation& op, uint32_t traceBlock) {
	(void) traceBlock;
	auto in = [&](size_t i) -> const TypedValueRef& {
		return std::get<TypedValueRef>(op.input[i]);
	};
	auto result = [&](nir::Ref id) {
		if (op.resultRef.ref != 0 && op.resultType != Type::v) {
			define(op.resultRef.ref, id);
		}
	};
	switch (op.op) {
	case Op::CONST: {
		auto type = typeInfo(op.resultType);
		const auto& literal = std::get<ConstantLiteral>(op.input[0]);
		const bool known = op.resultRef.ref < idOf_.size() && idOf_[op.resultRef.ref] != nir::NONE;
		nir::Ref value;
		if (type.id == TypeId::Ptr) {
			// a traced pointer is never a constant: it is a ptr.const of a pointers entry (01 §6)
			auto address = std::get<void*>(literal);
			if (address == nullptr) {
				value = b_.emit(NOp::PtrConst, TypeId::Ptr, 0, nir::NONE);
			} else {
				s_.processLocal = true;
				auto entry =
				    m_.internPointer(nir::PointerRecord {static_cast<uint8_t>(nir::PointerKind::TracedAddress), 0,
				                                         static_cast<uint8_t>(nir::Provenance::RawProcessValue), 0, 0,
				                                         reinterpret_cast<uintptr_t>(address), 0});
				value = b_.emit(NOp::PtrConst, TypeId::Ptr, 0, entry);
			}
			if (known) {
				b_.emit(NOp::Assign, TypeId::Void, 0, idOf_[op.resultRef.ref], value);
			} else {
				define(op.resultRef.ref, value);
			}
		} else {
			value = m_.consts.intern(type.id, nir::Provenance::Literal, literalBits(literal));
			if (known) {
				// the same call site reached on another path: a write onto the variable the first visit made (#95)
				b_.emit(NOp::Assign, TypeId::Void, 0, idOf_[op.resultRef.ref], value);
			} else {
				define(op.resultRef.ref, b_.emit(NOp::Var, type.id, 0, value));
			}
		}
		break;
	}
	case Op::ASSIGN: {
		const auto& source = in(0);
		auto target = op.resultRef.ref;
		if (target >= idOf_.size() || idOf_[target] == nir::NONE) {
			// the first write to a fresh ref defines the variable
			define(target, b_.emit(NOp::Var, typeInfo(op.resultType).id, 0, valueOf(source)));
		} else {
			b_.emit(NOp::Assign, TypeId::Void, 0, idOf_[target], valueOf(source));
		}
		break;
	}
	case Op::ADD:
		result(binary(NOp::Add, op.resultType, in(0), in(1)));
		break;
	case Op::SUB:
		result(binary(NOp::Sub, op.resultType, in(0), in(1)));
		break;
	case Op::MUL:
		result(binary(NOp::Mul, op.resultType, in(0), in(1)));
		break;
	case Op::DIV:
	case Op::MOD: {
		auto t = typeInfo(op.resultType);
		auto nop = op.op == Op::DIV ? NOp::Div : NOp::Rem;
		if (t.isFloat) {
			result(binary(nop, op.resultType, in(0), in(1)));
		} else {
			// integer division is checked: it records the cleanup list of the live destructors, none here (13 §5.10)
			uint32_t empty[] = {0};
			result(binary(nop, op.resultType, in(0), in(1), static_cast<uint8_t>((t.isSigned ? 0 : 1) | 2),
			              m_.addExtra(empty)));
		}
		break;
	}
	case Op::EQ:
	case Op::NEQ:
	case Op::LT:
	case Op::LTE:
	case Op::GT:
	case Op::GTE: {
		uint8_t flags = 0;
		cmpFlags(op.op, in(0), flags);
		result(b_.emit(NOp::Cmp, TypeId::Bool, flags, valueOf(in(0)), valueOf(in(1))));
		break;
	}
	case Op::NOT:
		result(b_.emit(NOp::Not, TypeId::Bool, 0, valueOf(in(0))));
		break;
	case Op::AND:
		result(b_.emit(NOp::And, TypeId::Bool, 0, valueOf(in(0)), valueOf(in(1))));
		break;
	case Op::OR:
		result(b_.emit(NOp::Or, TypeId::Bool, 0, valueOf(in(0)), valueOf(in(1))));
		break;
	case Op::NEGATE: {
		auto t = typeInfo(op.resultType);
		result(b_.emit(t.isFloat ? NOp::FNeg : NOp::BNot, t.id, 0, valueOf(in(0))));
		break;
	}
	// val<bool>'s `&`, `|` and `^` combine two values of a 0/1 domain: NIR has no bitwise operation on `bool`, so they
	// are the logical ones (`xor` is "differs", `cmp.ne`)
	case Op::BAND:
		result(op.resultType == Type::b ? b_.emit(NOp::And, TypeId::Bool, 0, valueOf(in(0)), valueOf(in(1)))
		                                : binary(NOp::BAnd, op.resultType, in(0), in(1)));
		break;
	case Op::BOR:
		result(op.resultType == Type::b ? b_.emit(NOp::Or, TypeId::Bool, 0, valueOf(in(0)), valueOf(in(1)))
		                                : binary(NOp::BOr, op.resultType, in(0), in(1)));
		break;
	case Op::BXOR:
		result(op.resultType == Type::b ? b_.emit(NOp::Cmp, TypeId::Bool, 1 /* ne */, valueOf(in(0)), valueOf(in(1)))
		                                : binary(NOp::BXor, op.resultType, in(0), in(1)));
		break;
	case Op::LSH:
		result(binary(NOp::Shl, op.resultType, in(0), in(1)));
		break;
	case Op::RSH:
		result(binary(NOp::Shr, op.resultType, in(0), in(1), typeInfo(op.resultType).isSigned ? 0 : 1));
		break;
	case Op::CAST:
		result(cast(in(0), op.resultType));
		break;
	case Op::SELECT:
		result(b_.emit(NOp::Select, typeInfo(op.resultType).id, 0, valueOf(in(0)), valueOf(in(1)), valueOf(in(2))));
		break;
	case Op::LOAD:
		result(b_.emit(NOp::Load, typeInfo(op.resultType).id, 0, valueOf(in(0))));
		break;
	case Op::STORE:
		b_.emit(NOp::Store, TypeId::Void, 0, valueOf(in(0)), valueOf(in(1)));
		break;
	case Op::ALLOCA:
		result(b_.emit(NOp::Alloca, TypeId::Ptr, 0, std::get<AllocaIndex>(op.input[0])));
		break;
	case Op::CALL:
	case Op::CALL_WITH_EXCEPTION_HANDLING: {
		auto id = emitCall(op);
		result(id);
		break;
	}
	case Op::INDIRECT_CALL:
	case Op::INDIRECT_CALL_WITH_EXCEPTION_HANDLING: {
		auto id = emitIndirectCall(op);
		result(id);
		break;
	}
	case Op::FUNC_ADDR: {
		const auto& call = *std::get<FunctionCall*>(op.input[0]);
		result(b_.emit(NOp::FuncAddr, TypeId::Ptr, 0, calleeFor(call, Type::ptr)));
		break;
	}
	case Op::JMP: {
		const auto& target = *std::get<BlockRef*>(op.input[0]);
		uint32_t args[] = {0};
		b_.emit(NOp::Br, TypeId::Void, 0, newBlockId_[target.block], 0, m_.addExtra(args));
		break;
	}
	case Op::CMP: {
		const auto& t = *std::get<BlockRef*>(op.input[1]);
		const auto& f = *std::get<BlockRef*>(op.input[2]);
		uint32_t args[] = {probabilityWord(std::get<BranchProbability>(op.input[3])), 0, newBlockId_[f.block], 0};
		b_.emit(NOp::CondBr, TypeId::Void, 0, valueOf(in(0)), newBlockId_[t.block], m_.addExtra(args));
		break;
	}
	case Op::RETURN:
		b_.emit(NOp::Ret, TypeId::Void, 0, op.input.empty() ? nir::NONE : valueOf(in(0)));
		break;
	case Op::FREE:
		fail(name_, "FREE has no NIR counterpart");
	}
}

void FunctionConverter::emitBlock(uint32_t traceBlock) {
	for (const auto* op : block(traceBlock).operations) {
		emitOperation(*op, traceBlock);
	}
}

void FunctionConverter::run() {
	if (trace_.blocks.empty()) {
		fail(name_, "the trace has no blocks");
	}
	orderBlocks();
	collectTypes();

	const auto& arguments = trace_.blocks[0]->arguments;
	std::vector<TypedValueRef> parameters(arguments.begin(), arguments.end());
	std::sort(parameters.begin(), parameters.end());
	idOf_.assign(trace_.lastValueRef + 1, nir::NONE);

	auto signature = signatureFor(returnType_, parameters);
	auto nameId = m_.internName(name_);
	uint32_t attrs = 0;
	if (auto it = s_.internalAttrs.find(name_); it != s_.internalAttrs.end()) {
		attrs = it->second;
	}
	b_.beginFunction(nameId, signature, exported_ ? nir::Visibility::Exported : nir::Visibility::Internal,
	                 nir::Level::PreSSA, attrs);
	for (const auto& spec : trace_.allocaSpecs) {
		b_.addAlloca(static_cast<uint32_t>(spec.size), static_cast<uint32_t>(spec.align));
	}
	for (uint32_t i = 0; i < order_.size(); ++i) {
		b_.beginBlock();
		if (i == 0) {
			for (uint32_t p = 0; p < parameters.size(); ++p) {
				auto id = b_.emit(NOp::BlockArg, typeInfo(parameters[p].type).id, 0, p);
				define(parameters[p].ref, id);
			}
		}
		emitBlock(order_[i]);
	}
	b_.endFunction();
}

} // namespace

nir::Module toPreSSA(const TraceModule& module, const Options& options) {
	(void) options;
	ModuleState state;
	auto names = module.getFunctionNames();
	auto isEntry = [&](const std::string& name) {
		const auto& attributes = module.getFunctionAttributes(name);
		auto it = attributes.find("entry");
		return it != attributes.end() && it->second == "true";
	};
	std::sort(names.begin(), names.end(), [&](const std::string& a, const std::string& b) {
		bool ea = isEntry(a), eb = isEntry(b);
		return ea != eb ? ea : a < b;
	});
	for (uint32_t i = 0; i < names.size(); ++i) {
		state.functionIndex[names[i]] = i;
		state.summaries[names[i]] = summarize(*module.getFunction(names[i]));
	}
	// the attributes of an internal function are what its call sites state; the pre-SSA module has no inference yet
	for (const auto& name : names) {
		for (const auto* block : module.getFunction(name)->blocks) {
			for (const auto* op : block->operations) {
				if ((op->op == Op::CALL || op->op == Op::CALL_WITH_EXCEPTION_HANDLING || op->op == Op::FUNC_ADDR) &&
				    std::get<FunctionCall*>(op->input[0])->kind == CalleeKind::Internal) {
					const auto& call = *std::get<FunctionCall*>(op->input[0]);
					auto bits = op->op == Op::FUNC_ADDR ? 0u : attrBits(call.fnAttrs);
					auto [it, inserted] = state.internalAttrs.emplace(call.functionName, bits);
					if (!inserted) {
						it->second &= bits;
					}
				}
			}
		}
	}
	for (const auto& name : names) {
		FunctionConverter(state, *module.getFunction(name), name, isEntry(name)).run();
	}
	if (state.processLocal) {
		state.m.module.processToken = nir::currentProcessToken();
	}
	return std::move(state.m);
}

nir::Module toPreSSA(const ExecutionTrace& trace, std::string_view name, const Options& options) {
	(void) options;
	ModuleState state;
	state.functionIndex[std::string(name)] = 0;
	state.summaries[std::string(name)] = summarize(trace);
	FunctionConverter(state, trace, std::string(name), true).run();
	if (state.processLocal) {
		state.m.module.processToken = nir::currentProcessToken();
	}
	return std::move(state.m);
}

} // namespace nautilus::tracing::nirtrace

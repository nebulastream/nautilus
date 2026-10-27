#include "nautilus/compiler/NirToIR.hpp"
#include "nautilus/common/FunctionAttributes.hpp"
#include "nautilus/compiler/backends/nir/NirBackend.hpp"
#include "nautilus/compiler/ir/IRGraph.hpp"
#include "nautilus/compiler/ir/blocks/BasicBlock.hpp"
#include "nautilus/compiler/ir/blocks/BasicBlockArgument.hpp"
#include "nautilus/compiler/ir/operations/AllocaOperation.hpp"
#include "nautilus/compiler/ir/operations/ArithmeticOperations/AddOperation.hpp"
#include "nautilus/compiler/ir/operations/ArithmeticOperations/DivOperation.hpp"
#include "nautilus/compiler/ir/operations/ArithmeticOperations/ModOperation.hpp"
#include "nautilus/compiler/ir/operations/ArithmeticOperations/MulOperation.hpp"
#include "nautilus/compiler/ir/operations/ArithmeticOperations/SubOperation.hpp"
#include "nautilus/compiler/ir/operations/BinaryOperations/BinaryCompOperation.hpp"
#include "nautilus/compiler/ir/operations/BinaryOperations/NegateOperation.hpp"
#include "nautilus/compiler/ir/operations/BinaryOperations/ShiftOperation.hpp"
#include "nautilus/compiler/ir/operations/BranchOperation.hpp"
#include "nautilus/compiler/ir/operations/CallOperation.hpp"
#include "nautilus/compiler/ir/operations/CastOperation.hpp"
#include "nautilus/compiler/ir/operations/ConstBooleanOperation.hpp"
#include "nautilus/compiler/ir/operations/ConstFloatOperation.hpp"
#include "nautilus/compiler/ir/operations/ConstIntOperation.hpp"
#include "nautilus/compiler/ir/operations/ConstPtrOperation.hpp"
#include "nautilus/compiler/ir/operations/FunctionAddressOfOperation.hpp"
#include "nautilus/compiler/ir/operations/FunctionOperation.hpp"
#include "nautilus/compiler/ir/operations/IfOperation.hpp"
#include "nautilus/compiler/ir/operations/IndirectCallOperation.hpp"
#include "nautilus/compiler/ir/operations/LoadOperation.hpp"
#include "nautilus/compiler/ir/operations/LogicalOperations/AndOperation.hpp"
#include "nautilus/compiler/ir/operations/LogicalOperations/CompareOperation.hpp"
#include "nautilus/compiler/ir/operations/LogicalOperations/NotOperation.hpp"
#include "nautilus/compiler/ir/operations/LogicalOperations/OrOperation.hpp"
#include "nautilus/compiler/ir/operations/ReturnOperation.hpp"
#include "nautilus/compiler/ir/operations/SelectOperation.hpp"
#include "nautilus/compiler/ir/operations/StoreOperation.hpp"
#include "nautilus/exceptions/RuntimeException.hpp"
#include "nautilus/extensions/ExtensionRegistry.hpp"
#include "nautilus/tracing/FunctionNameCache.hpp"
#include <array>
#include <bit>
#include <fmt/format.h>
#include <map>
#include <numeric>
#include <tuple>
#include <unordered_map>

namespace nautilus::compiler {

namespace {

namespace nir = ::nautilus::nir;
using ir::BasicBlock;
using ir::Operation;
using ir::OperationIdentifier;
using nir::Op;
using nir::Ref;
using nir::TypeId;

[[noreturn]] void fail(const std::string& function, const std::string& message) {
	throw RuntimeException(fmt::format("NIR to IR bridge, function '{}': {}", function, message));
}

/// The signedness a 0.3 integer stamp carries and NIR does not.
enum class Sign : uint8_t { None, Signed, Unsigned };

Sign signOf(Type type) {
	switch (type) {
	case Type::i8:
	case Type::i16:
	case Type::i32:
	case Type::i64:
		return Sign::Signed;
	case Type::ui8:
	case Type::ui16:
	case Type::ui32:
	case Type::ui64:
		return Sign::Unsigned;
	default:
		return Sign::None;
	}
}

/// The 0.3 stamp of a NIR type. An integer without a sign preference is unsigned: it wraps without undefined behavior
/// in the C++ backend.
Type stampOf(TypeId id, Sign sign) {
	const bool isSigned = sign == Sign::Signed;
	switch (id) {
	case TypeId::Void:
		return Type::v;
	case TypeId::Bool:
		return Type::b;
	case TypeId::I8:
		return isSigned ? Type::i8 : Type::ui8;
	case TypeId::I16:
		return isSigned ? Type::i16 : Type::ui16;
	case TypeId::I32:
		return isSigned ? Type::i32 : Type::ui32;
	case TypeId::I64:
		return isSigned ? Type::i64 : Type::ui64;
	case TypeId::F32:
		return Type::f32;
	case TypeId::F64:
		return Type::f64;
	case TypeId::Ptr:
		return Type::ptr;
	default:
		throw RuntimeException("NIR to IR bridge: an extension type has no 0.3 counterpart");
	}
}

Type withSign(Type type, Sign sign) {
	switch (type) {
	case Type::i8:
	case Type::ui8:
		return sign == Sign::Signed ? Type::i8 : Type::ui8;
	case Type::i16:
	case Type::ui16:
		return sign == Sign::Signed ? Type::i16 : Type::ui16;
	case Type::i32:
	case Type::ui32:
		return sign == Sign::Signed ? Type::i32 : Type::ui32;
	case Type::i64:
	case Type::ui64:
		return sign == Sign::Signed ? Type::i64 : Type::ui64;
	default:
		return type;
	}
}

/// The sign a type word of a signature states for an integer that crosses a native boundary (13 §4.5.1).
Sign wordSign(nir::TypeWord word) {
	switch (nir::extKindOf(word)) {
	case nir::ExtKind::Sext:
		return Sign::Signed;
	case nir::ExtKind::Zext:
		return Sign::Unsigned;
	default:
		return Sign::None;
	}
}

FunctionAttributes attributesOf(uint32_t bits) {
	FunctionAttributes attributes;
	const bool noRead =
	    (bits & (nir::AttrNoArgRead | nir::AttrNoOtherRead)) == (nir::AttrNoArgRead | nir::AttrNoOtherRead);
	const bool noWrite =
	    (bits & (nir::AttrNoArgWrite | nir::AttrNoOtherWrite)) == (nir::AttrNoArgWrite | nir::AttrNoOtherWrite);
	if (noRead && noWrite) {
		attributes.modRefInfo = ModRefInfo::NoModRef;
	} else if (noWrite) {
		attributes.modRefInfo = ModRefInfo::Ref;
	} else if (noRead) {
		attributes.modRefInfo = ModRefInfo::Mod;
	} else {
		attributes.modRefInfo = ModRefInfo::ModRef;
	}
	attributes.willReturn = (bits & nir::AttrWillReturn) != 0;
	attributes.noUnwind = (bits & nir::AttrNoUnwind) != 0;
	return attributes;
}

/// What is shared by the functions of a module while its IR is built.
struct ModuleBridge {
	const nir::Module& m;
	ir::IRGraph& graph;
	common::Arena& arena;
	nirbackend::HostEnvironment host {};
	bool resolveNames = false;
	bool demangle = true;
	/// NIR function index -> function table id of its definition.
	std::vector<ir::FunctionId> internalIds;
	/// NIR callee index -> function table id, interned at the first call site.
	std::vector<ir::FunctionId> calleeIds;

	ModuleBridge(const nir::Module& module, ir::IRGraph& g, const engine::Options& options,
	             const nirbackend::HostEnvironment* hostEnvironment)
	    : m(module), graph(g), arena(g.getArena()) {
		if (hostEnvironment != nullptr) {
			host = *hostEnvironment;
		}
		// names never affect the generated code, so they are resolved (dladdr is slow) only when someone will read them
		const bool inspected = options.getOptionOrDefault("debug", false) ||
		                       options.getOptionOrDefault("perf", false) ||
		                       options.getOptionOrDefault("perf.sample", false);
		resolveNames = options.getOptionOrDefault("engine.resolveFunctionNames", inspected);
		demangle = options.getOptionOrDefault("engine.demangleFunctionNames", true);
	}

	void* internalKey(uint32_t function) const {
		return const_cast<void*>(static_cast<const void*>(&m.functions[function]));
	}

	void* addressOf(const nir::CalleeRecord& callee) const {
		switch (static_cast<nir::CalleeKind>(callee.kind)) {
		case nir::CalleeKind::External:
			return nirbackend::resolvePointer(m, callee.target, host);
		case nir::CalleeKind::Intrinsic: {
			const auto* info = ext::ExtensionRegistry::instance().intrinsic(callee.target);
			if (info == nullptr) {
				throw RuntimeException("NIR to IR bridge: an intrinsic callee that is not registered");
			}
			return info->fallback;
		}
		case nir::CalleeKind::Internal:
			return internalKey(callee.target);
		case nir::CalleeKind::Imported:
			break;
		}
		throw RuntimeException("NIR to IR bridge: imported callees come with modules and linking (M4.6)");
	}

	/// Interns callee `index` in the function table of the graph, with the types of its first call site.
	ir::FunctionId calleeId(uint32_t index, Type resultType, const std::vector<Type>& paramTypes) {
		auto& id = calleeIds[index];
		if (id != ir::INVALID_FUNCTION_ID) {
			return id;
		}
		const auto& callee = m.callees[index];
		if (static_cast<nir::CalleeKind>(callee.kind) == nir::CalleeKind::Internal) {
			return id = internalIds[callee.target];
		}
		ir::CalleeDescriptor descriptor;
		descriptor.kind = ir::CalleeDescriptor::Kind::External;
		descriptor.key = addressOf(callee);
		descriptor.resultType = resultType;
		descriptor.paramTypes = paramTypes;
		descriptor.attrs = attributesOf(callee.attrs);
		if (resolveNames) {
			const auto& resolved = tracing::resolveFunctionName(descriptor.key);
			descriptor.mangledName = resolved.mangled;
			descriptor.demangledName = demangle ? resolved.demangled : resolved.mangled;
		}
		return id = graph.internCallee(descriptor);
	}
};

class FunctionBridge {
public:
	FunctionBridge(ModuleBridge& module, uint32_t function)
	    : s_(module), m_(module.m), f_(function), record_(m_.functions[function]), insts_(m_.functionInsts(function)),
	      blocks_(m_.functionBlocks(function)), name_(std::string(m_.name(record_.nameId))) {
	}

	ir::FunctionOperation* build();

private:
	// ---- signedness inference ---------------------------------------------------------------------------------------

	uint32_t find(uint32_t x) {
		while (parent_[x] != x) {
			parent_[x] = parent_[parent_[x]];
			x = parent_[x];
		}
		return x;
	}
	void unite(uint32_t x, uint32_t y) {
		x = find(x);
		y = find(y);
		if (x != y) {
			parent_[y] = x;
		}
	}
	bool isIntValue(Ref r) const {
		return nir::isValueRef(r) && nir::isInteger(static_cast<TypeId>(insts_[r].type));
	}
	void uniteRefs(Ref x, Ref y) {
		if (isIntValue(x) && isIntValue(y)) {
			unite(x, y);
		}
	}
	void vote(Ref r, Sign sign) {
		if (sign != Sign::None && isIntValue(r)) {
			votes_.push_back({r, sign});
		}
	}
	TypeId typeOfRef(Ref r) const {
		return nir::isConstRef(r) ? static_cast<TypeId>(m_.consts.record(r).type) : static_cast<TypeId>(insts_[r].type);
	}
	/// The sign the class of value `r` was planned with.
	Sign planSign(Ref r) {
		if (!isIntValue(r)) {
			return Sign::None;
		}
		return planned_[find(r)] ? Sign::Signed : Sign::Unsigned;
	}
	void plan();

	// ---- values -----------------------------------------------------------------------------------------------------

	uint32_t newId() {
		return nextId_++;
	}
	OperationIdentifier identifier(uint32_t id) const {
		return OperationIdentifier {id};
	}
	Operation* constant(Ref r, Sign want);
	Operation* cast(Operation* value, Type target);
	/// The operand `r`, as an integer of sign `want`: a constant is created with it, a value of another sign is cast.
	Operation* use(Ref r, Sign want = Sign::None);
	/// The sign a call argument or result has: what the signature states, else what the value's class was planned with.
	Sign signatureSign(nir::TypeWord word, Ref r) {
		const auto stated = wordSign(word);
		return stated != Sign::None ? stated : planSign(r);
	}

	template <class OpT>
	Operation* binary(uint32_t id, const nir::Inst& inst, Sign sign);
	void emit(uint32_t id, const nir::Inst& inst);
	void emitCast(uint32_t id, const nir::Inst& inst);
	void emitCall(uint32_t id, const nir::Inst& inst);
	void emitCallIndirect(uint32_t id, const nir::Inst& inst);
	void emitTerminator(const nir::Inst& inst);

	struct Cleanup {
		uint32_t callee;
		Ref object;
	};
	/// The destructor calls a landing pad runs, in the order they run.
	const std::vector<Cleanup>& padCleanups(uint32_t pad);
	template <class Destructor>
	std::vector<Destructor> destructorsOf(uint32_t unwind);
	void define(uint32_t id, Operation* value) {
		values_[id] = value;
	}

	ModuleBridge& s_;
	const nir::Module& m_;
	uint32_t f_;
	const nir::FunctionRecord& record_;
	std::span<const nir::Inst> insts_;
	std::span<const nir::BlockRecord> blocks_;
	std::string name_;

	std::vector<uint32_t> parent_;
	std::vector<std::pair<Ref, Sign>> votes_;
	std::vector<uint8_t> planned_; // per root: 1 signed
	std::vector<Ref> returns_;
	Sign returnSign_ = Sign::Unsigned;

	std::vector<Operation*> values_;
	std::vector<BasicBlock*> irBlocks_;
	std::vector<BasicBlock*> order_;
	BasicBlock* cur_ = nullptr;
	BasicBlock* returnBlock_ = nullptr;
	TypeId returnType_ = TypeId::Void;
	uint32_t nextId_ = 0;
	std::map<std::tuple<BasicBlock*, uint32_t, uint8_t>, Operation*> constants_;
	std::map<std::tuple<BasicBlock*, Operation*, Type>, Operation*> casts_;
	std::map<uint32_t, std::vector<Cleanup>> pads_;
};

void FunctionBridge::plan() {
	const auto n = static_cast<uint32_t>(insts_.size());
	parent_.resize(n);
	std::iota(parent_.begin(), parent_.end(), 0u);
	const auto& signature = m_.signatures[record_.signature];
	returnType_ = nir::typeOf(signature.result);
	Ref firstReturn = nir::NONE;

	auto blockArgOf = [&](uint32_t block, uint32_t position) {
		return blocks_[block].first + position;
	};
	auto sigVotes = [&](const nir::SignatureRecord& sig, const uint32_t* list) {
		for (uint32_t i = 0; i < sig.paramCount; ++i) {
			vote(list[1 + i], wordSign(m_.paramType(sig, i)));
		}
	};

	for (uint32_t id = 0; id < n; ++id) {
		const auto& inst = insts_[id];
		const auto op = static_cast<Op>(inst.op);
		switch (op) {
		case Op::Add:
		case Op::Sub:
		case Op::Mul:
		case Op::BAnd:
		case Op::BOr:
		case Op::BXor:
		case Op::Shl:
			uniteRefs(id, inst.a);
			uniteRefs(id, inst.b);
			break;
		case Op::Div:
		case Op::Rem:
		case Op::Shr:
			uniteRefs(id, inst.a);
			uniteRefs(id, inst.b);
			if (isIntValue(id)) {
				vote(id, (inst.flags & 1) ? Sign::Unsigned : Sign::Signed);
			}
			break;
		case Op::BNot:
			uniteRefs(id, inst.a);
			break;
		case Op::Select:
			uniteRefs(id, inst.b);
			uniteRefs(id, inst.c);
			break;
		case Op::Cmp:
			uniteRefs(inst.a, inst.b);
			if (nir::isInteger(typeOfRef(inst.a))) {
				const uint8_t predicate = inst.flags & 7;
				const auto sign = (inst.flags & 8) ? Sign::Unsigned : (predicate >= 2 ? Sign::Signed : Sign::None);
				vote(inst.a, sign);
				vote(inst.b, sign);
			}
			break;
		case Op::Cast:
			switch (inst.flags & 15) {
			case 0: // sext
			case 3: // sitofp
				vote(inst.a, Sign::Signed);
				break;
			case 1: // zext
			case 4: // uitofp
				vote(inst.a, Sign::Unsigned);
				break;
			case 5: // fptosi
				vote(id, Sign::Signed);
				break;
			case 6: // fptoui
				vote(id, Sign::Unsigned);
				break;
			default:
				break;
			}
			break;
		case Op::BlockArg:
			// the parameters of the function state their sign in the signature
			if (id < blocks_[0].first + blocks_[0].argCount) {
				vote(id, wordSign(m_.paramType(signature, inst.a)));
			}
			break;
		case Op::Br: {
			const uint32_t* list = m_.extra.data() + inst.c;
			for (uint32_t i = 0; i < list[0]; ++i) {
				uniteRefs(blockArgOf(inst.a, i), list[1 + i]);
			}
			break;
		}
		case Op::CondBr: {
			const uint32_t* list = m_.extra.data() + inst.c;
			const uint32_t trueCount = list[1];
			for (uint32_t i = 0; i < trueCount; ++i) {
				uniteRefs(blockArgOf(inst.b, i), list[2 + i]);
			}
			const uint32_t falseBlock = list[2 + trueCount];
			const uint32_t falseCount = list[3 + trueCount];
			for (uint32_t i = 0; i < falseCount; ++i) {
				uniteRefs(blockArgOf(falseBlock, i), list[4 + trueCount + i]);
			}
			break;
		}
		case Op::Call: {
			const auto& callee = m_.callees[inst.a];
			const auto& sig = m_.signatures[callee.signature];
			sigVotes(sig, m_.extra.data() + inst.c);
			vote(id, wordSign(sig.result));
			break;
		}
		case Op::CallIndirect: {
			const auto& sig = m_.signatures[inst.b];
			sigVotes(sig, m_.extra.data() + inst.c);
			vote(id, wordSign(sig.result));
			break;
		}
		case Op::Ret:
			if (inst.a != nir::NONE) {
				returns_.push_back(inst.a);
				if (firstReturn == nir::NONE && isIntValue(inst.a)) {
					firstReturn = inst.a;
				} else {
					uniteRefs(firstReturn, inst.a);
				}
				vote(inst.a, wordSign(signature.result));
			}
			break;
		default:
			break;
		}
	}

	std::vector<std::array<uint32_t, 2>> counts(n, {0, 0});
	for (const auto& [ref, sign] : votes_) {
		++counts[find(ref)][sign == Sign::Signed ? 0 : 1];
	}
	planned_.assign(n, 0);
	for (uint32_t id = 0; id < n; ++id) {
		planned_[id] = counts[id][0] > counts[id][1] ? 1 : 0;
	}
	// what the function returns: the sign of its signature, else the one its returned values were planned with
	returnSign_ = wordSign(signature.result);
	if (returnSign_ == Sign::None) {
		returnSign_ = firstReturn != nir::NONE ? planSign(firstReturn) : Sign::Unsigned;
	}
}

Operation* FunctionBridge::constant(Ref r, Sign want) {
	const auto index = nir::constIndexOf(r);
	const auto& record = m_.consts.at(index);
	const auto type = static_cast<TypeId>(record.type);
	if (want == Sign::None) {
		want = Sign::Unsigned;
	}
	const auto key = std::make_tuple(
	    cur_, index, static_cast<uint8_t>(type == TypeId::Bool || !nir::isInteger(type) ? Sign::None : want));
	if (auto it = constants_.find(key); it != constants_.end()) {
		return it->second;
	}
	Operation* result = nullptr;
	switch (type) {
	case TypeId::Bool:
		result = cur_->addOperation<ir::ConstBooleanOperation>(identifier(newId()), record.bits != 0);
		break;
	case TypeId::F32:
		result = cur_->addOperation<ir::ConstFloatOperation>(
		    identifier(newId()), static_cast<double>(std::bit_cast<float>(static_cast<uint32_t>(record.bits))),
		    Type::f32);
		break;
	case TypeId::F64:
		result = cur_->addOperation<ir::ConstFloatOperation>(identifier(newId()), std::bit_cast<double>(record.bits),
		                                                     Type::f64);
		break;
	case TypeId::I8:
	case TypeId::I16:
	case TypeId::I32:
	case TypeId::I64: {
		const auto stamp = stampOf(type, want);
		const auto bits = static_cast<unsigned>(nir::sizeOf(type) * 8);
		int64_t value = static_cast<int64_t>(record.bits);
		if (want == Sign::Signed && bits < 64) {
			// the value as a signed integer of its own width
			const uint64_t sign = 1ull << (bits - 1);
			value = static_cast<int64_t>((record.bits ^ sign) - sign);
		}
		result = cur_->addOperation<ir::ConstIntOperation>(identifier(newId()), value, stamp);
		break;
	}
	default:
		fail(name_, "a constant of an extension type has no 0.3 counterpart");
	}
	constants_.emplace(key, result);
	return result;
}

Operation* FunctionBridge::cast(Operation* value, Type target) {
	const auto key = std::make_tuple(cur_, value, target);
	if (auto it = casts_.find(key); it != casts_.end()) {
		return it->second;
	}
	auto* result = cur_->addOperation<ir::CastOperation>(identifier(newId()), value, target);
	casts_.emplace(key, result);
	return result;
}

Operation* FunctionBridge::use(Ref r, Sign want) {
	if (nir::isConstRef(r)) {
		return constant(r, want);
	}
	auto* value = values_[r];
	if (value == nullptr) {
		fail(name_, fmt::format("%{} is used before its definition", r));
	}
	if (want != Sign::None) {
		const auto have = signOf(value->getStamp());
		if (have != Sign::None && have != want) {
			return cast(value, withSign(value->getStamp(), want));
		}
	}
	return value;
}

template <class OpT>
Operation* FunctionBridge::binary(uint32_t id, const nir::Inst& inst, Sign sign) {
	auto* left = use(inst.a, sign);
	auto* right = use(inst.b, sign);
	return cur_->addOperation<OpT>(identifier(id), left, right);
}

const std::vector<FunctionBridge::Cleanup>& FunctionBridge::padCleanups(uint32_t pad) {
	if (auto it = pads_.find(pad); it != pads_.end()) {
		return it->second;
	}
	std::vector<Cleanup> cleanups;
	// a pad runs its destructor calls, then resumes or continues in another pad
	uint32_t block = pad;
	for (bool done = false; !done;) {
		const auto& record = blocks_[block];
		done = true;
		for (uint32_t i = 0; i < record.count; ++i) {
			const auto& inst = insts_[record.first + i];
			const auto op = static_cast<Op>(inst.op);
			if (op == Op::Call) {
				cleanups.push_back({inst.a, m_.extra[inst.c + 1]});
			} else if (op == Op::Br) {
				block = inst.a;
				done = false;
			}
		}
	}
	return pads_.emplace(pad, std::move(cleanups)).first->second;
}

template <class Destructor>
std::vector<Destructor> FunctionBridge::destructorsOf(uint32_t unwind) {
	std::vector<Destructor> destructors;
	if (unwind == nir::NO_UNWIND || unwind == nir::NO_CLEANUP) {
		return destructors;
	}
	// 0.3 lists destructors in construction order and runs them in reverse
	const auto& cleanups = padCleanups(unwind);
	destructors.reserve(cleanups.size());
	for (auto it = cleanups.rbegin(); it != cleanups.rend(); ++it) {
		const auto& callee = m_.callees[it->callee];
		destructors.push_back(Destructor {
		    .address = use(it->object), .functionSymbol = {}, .functionName = {}, .functionPtr = s_.addressOf(callee)});
	}
	return destructors;
}

void FunctionBridge::emitCall(uint32_t id, const nir::Inst& inst) {
	const auto& callee = m_.callees[inst.a];
	const uint32_t* list = m_.extra.data() + inst.c;
	const uint32_t argc = list[0];
	const uint32_t unwind = list[1 + argc];
	const uint32_t thunk = list[2 + argc];
	const auto& sig = m_.signatures[callee.signature];

	std::vector<Operation*> arguments;
	std::vector<Type> parameterTypes;
	arguments.reserve(argc);
	for (uint32_t i = 0; i < argc; ++i) {
		arguments.push_back(use(list[1 + i], signatureSign(m_.paramType(sig, i), list[1 + i])));
		parameterTypes.push_back(arguments.back()->getStamp());
	}
	const auto resultId = static_cast<TypeId>(inst.type);
	const auto resultType = stampOf(resultId, signatureSign(sig.result, id));
	const auto calleeId = s_.calleeId(inst.a, resultType, parameterTypes);
	const bool internal = static_cast<nir::CalleeKind>(callee.kind) == nir::CalleeKind::Internal;
	const bool exceptionHandling = unwind != nir::NO_UNWIND;
	void* captureFunc = thunk == nir::NONE ? nullptr : nirbackend::resolvePointer(m_, thunk, s_.host);
	auto destructors = destructorsOf<ir::CallOperation::Destructor>(unwind);
	const auto& target = s_.graph.getFunctionTarget(calleeId);

	auto* call = cur_->addOperation<ir::CallOperation>(target.getName().getMangled(), target.getName().get(),
	                                                   s_.addressOf(callee), identifier(id), arguments, resultType,
	                                                   attributesOf(callee.attrs), calleeId, std::move(destructors),
	                                                   exceptionHandling, captureFunc, internal);
	if (resultType != Type::v) {
		define(id, call);
	}
}

void FunctionBridge::emitCallIndirect(uint32_t id, const nir::Inst& inst) {
	const uint32_t* list = m_.extra.data() + inst.c;
	const uint32_t argc = list[0];
	const uint32_t unwind = list[1 + argc];
	const uint32_t thunk = list[2 + argc];
	const uint32_t attrs = list[3 + argc];
	const auto& sig = m_.signatures[inst.b];

	auto* pointer = use(inst.a);
	std::vector<Operation*> arguments;
	arguments.reserve(argc);
	for (uint32_t i = 0; i < argc; ++i) {
		arguments.push_back(use(list[1 + i], signatureSign(m_.paramType(sig, i), list[1 + i])));
	}
	const auto resultType = stampOf(static_cast<TypeId>(inst.type), signatureSign(sig.result, id));
	void* captureFunc = thunk == nir::NONE ? nullptr : nirbackend::resolvePointer(m_, thunk, s_.host);
	auto* call = cur_->addOperation<ir::IndirectCallOperation>(
	    identifier(id), pointer, arguments, resultType, attributesOf(attrs),
	    destructorsOf<ir::IndirectCallOperation::Destructor>(unwind), unwind != nir::NO_UNWIND, captureFunc);
	if (resultType != Type::v) {
		define(id, call);
	}
}

void FunctionBridge::emitCast(uint32_t id, const nir::Inst& inst) {
	const auto kind = inst.flags & 15;
	const auto to = static_cast<TypeId>(inst.type);
	const auto from = typeOfRef(inst.a);
	Operation* result = nullptr;
	switch (kind) {
	case 0: { // sext
		if (from == TypeId::Bool) {
			// 0 or -1: 0.3 converts a bool to 0 or 1
			auto* condition = use(inst.a);
			const auto stamp = stampOf(to, Sign::Signed);
			auto* ones = cur_->addOperation<ir::ConstIntOperation>(identifier(newId()), -1, stamp);
			auto* zero = cur_->addOperation<ir::ConstIntOperation>(identifier(newId()), 0, stamp);
			result = cur_->addOperation<ir::SelectOperation>(identifier(id), condition, ones, zero, stamp);
			break;
		}
		result =
		    cur_->addOperation<ir::CastOperation>(identifier(id), use(inst.a, Sign::Signed), stampOf(to, planSign(id)));
		break;
	}
	case 1: // zext
		result = cur_->addOperation<ir::CastOperation>(identifier(id),
		                                               from == TypeId::Bool ? use(inst.a) : use(inst.a, Sign::Unsigned),
		                                               stampOf(to, planSign(id)));
		break;
	case 2: // trunc
		result = cur_->addOperation<ir::CastOperation>(identifier(id), use(inst.a), stampOf(to, planSign(id)));
		break;
	case 3: // sitofp
		result =
		    cur_->addOperation<ir::CastOperation>(identifier(id), use(inst.a, Sign::Signed), stampOf(to, Sign::None));
		break;
	case 4: // uitofp
		result =
		    cur_->addOperation<ir::CastOperation>(identifier(id), use(inst.a, Sign::Unsigned), stampOf(to, Sign::None));
		break;
	case 5: // fptosi
		result = cur_->addOperation<ir::CastOperation>(identifier(id), use(inst.a), stampOf(to, Sign::Signed));
		break;
	case 6: // fptoui
		result = cur_->addOperation<ir::CastOperation>(identifier(id), use(inst.a), stampOf(to, Sign::Unsigned));
		break;
	case 7: // fpext
	case 8: // fptrunc
		result = cur_->addOperation<ir::CastOperation>(identifier(id), use(inst.a), stampOf(to, Sign::None));
		break;
	case 10: // ptrtoint
		result = cur_->addOperation<ir::CastOperation>(identifier(id), use(inst.a), stampOf(to, planSign(id)));
		break;
	case 11: // inttoptr
		result = cur_->addOperation<ir::CastOperation>(identifier(id), use(inst.a), Type::ptr);
		break;
	default:
		fail(name_, "the cast kind 'bitcast' has no 0.3 counterpart");
	}
	define(id, result);
}

void FunctionBridge::emitTerminator(const nir::Inst& inst) {
	auto blockArgSign = [&](uint32_t block, uint32_t position) {
		return signOf(irBlocks_[block]->getArguments()[position]->getStamp());
	};
	switch (static_cast<Op>(inst.op)) {
	case Op::Br: {
		const uint32_t* list = m_.extra.data() + inst.c;
		std::vector<Operation*> arguments;
		for (uint32_t i = 0; i < list[0]; ++i) {
			arguments.push_back(use(list[1 + i], blockArgSign(inst.a, i)));
		}
		cur_->addNextBlock(irBlocks_[inst.a], arguments);
		break;
	}
	case Op::CondBr: {
		const uint32_t* list = m_.extra.data() + inst.c;
		const uint32_t trueCount = list[1];
		const uint32_t falseBlock = list[2 + trueCount];
		const uint32_t falseCount = list[3 + trueCount];
		const double probability = list[0] == 0xFFFFFFFFu ? 0.5 : static_cast<double>(list[0]) / 0xFFFFFFFEull;
		auto* condition = use(inst.a);
		auto* branch = s_.arena.create<ir::IfOperation>(s_.arena, condition, probability);
		branch->getTrueBlockInvocation().setBlock(irBlocks_[inst.b]);
		branch->getFalseBlockInvocation().setBlock(irBlocks_[falseBlock]);
		for (uint32_t i = 0; i < trueCount; ++i) {
			branch->getTrueBlockInvocation().addArgument(s_.arena, use(list[2 + i], blockArgSign(inst.b, i)));
		}
		for (uint32_t i = 0; i < falseCount; ++i) {
			branch->getFalseBlockInvocation().addArgument(s_.arena,
			                                              use(list[4 + trueCount + i], blockArgSign(falseBlock, i)));
		}
		cur_->addOperation(branch);
		break;
	}
	case Op::Ret: {
		if (returnBlock_ != nullptr) {
			// several returns branch to one block that takes the result
			if (inst.a == nir::NONE) {
				cur_->addNextBlock(returnBlock_, {});
			} else {
				Operation* const arguments[] = {use(inst.a, returnSign_)};
				cur_->addNextBlock(returnBlock_, arguments);
			}
		} else if (inst.a == nir::NONE) {
			cur_->addOperation<ir::ReturnOperation>();
		} else {
			cur_->addOperation<ir::ReturnOperation>(use(inst.a, returnSign_));
		}
		break;
	}
	default:
		fail(name_, "a landing pad terminator in a regular block");
	}
}

void FunctionBridge::emit(uint32_t id, const nir::Inst& inst) {
	const auto op = static_cast<Op>(inst.op);
	const auto type = static_cast<TypeId>(inst.type);
	const bool isInteger = nir::isInteger(type);
	switch (op) {
	case Op::Add:
	case Op::Sub: {
		if (type == TypeId::Ptr) {
			auto* address = use(inst.a);
			auto* offset = use(inst.b, planSign(inst.b));
			define(id, op == Op::Add ? static_cast<Operation*>(
			                               cur_->addOperation<ir::AddOperation>(identifier(id), address, offset))
			                         : cur_->addOperation<ir::SubOperation>(identifier(id), address, offset));
			break;
		}
		const auto sign = isInteger ? planSign(id) : Sign::None;
		define(id, op == Op::Add ? binary<ir::AddOperation>(id, inst, sign) : binary<ir::SubOperation>(id, inst, sign));
		break;
	}
	case Op::Mul:
		define(id, binary<ir::MulOperation>(id, inst, isInteger ? planSign(id) : Sign::None));
		break;
	case Op::Div:
	case Op::Rem: {
		// 0.3 divides without the check of D10: the checked bit and the landing pad are dropped
		const auto sign = isInteger ? ((inst.flags & 1) ? Sign::Unsigned : Sign::Signed) : Sign::None;
		define(id, op == Op::Div ? binary<ir::DivOperation>(id, inst, sign) : binary<ir::ModOperation>(id, inst, sign));
		break;
	}
	case Op::FNeg:
		define(id, cur_->addOperation<ir::NegateOperation>(identifier(id), use(inst.a)));
		break;
	case Op::And:
		define(id, binary<ir::AndOperation>(id, inst, Sign::None));
		break;
	case Op::Or:
		define(id, binary<ir::OrOperation>(id, inst, Sign::None));
		break;
	case Op::Not:
		define(id, cur_->addOperation<ir::NotOperation>(identifier(id), use(inst.a)));
		break;
	case Op::BAnd:
	case Op::BOr:
	case Op::BXor: {
		const auto sign = planSign(id);
		auto* left = use(inst.a, sign);
		auto* right = use(inst.b, sign);
		const auto kind = op == Op::BAnd  ? ir::BinaryCompOperation::BAND
		                  : op == Op::BOr ? ir::BinaryCompOperation::BOR
		                                  : ir::BinaryCompOperation::XOR;
		define(id, cur_->addOperation<ir::BinaryCompOperation>(identifier(id), left, right, kind));
		break;
	}
	case Op::BNot:
		define(id, cur_->addOperation<ir::NegateOperation>(identifier(id), use(inst.a, planSign(id))));
		break;
	case Op::Shl:
	case Op::Shr: {
		const auto sign = op == Op::Shr ? ((inst.flags & 1) ? Sign::Unsigned : Sign::Signed) : planSign(id);
		auto* left = use(inst.a, sign);
		auto* right = use(inst.b, sign);
		define(id, cur_->addOperation<ir::ShiftOperation>(
		               identifier(id), left, right, op == Op::Shl ? ir::ShiftOperation::LS : ir::ShiftOperation::RS));
		break;
	}
	case Op::Cmp: {
		const auto operandType = typeOfRef(inst.a);
		const uint8_t predicate = inst.flags & 7;
		auto sign = Sign::None;
		if (nir::isInteger(operandType)) {
			sign = (inst.flags & 8) ? Sign::Unsigned : (predicate >= 2 ? Sign::Signed : planSign(inst.a));
			if (sign == Sign::None) {
				sign = planSign(inst.b);
			}
		}
		auto* left = use(inst.a, sign);
		auto* right = use(inst.b, sign);
		define(id, cur_->addOperation<ir::CompareOperation>(identifier(id), left, right,
		                                                    static_cast<ir::CompareOperation::Comparator>(predicate)));
		break;
	}
	case Op::Select: {
		const auto sign = isInteger ? planSign(id) : Sign::None;
		auto* condition = use(inst.a);
		auto* whenTrue = use(inst.b, sign);
		auto* whenFalse = use(inst.c, sign);
		define(id, cur_->addOperation<ir::SelectOperation>(identifier(id), condition, whenTrue, whenFalse,
		                                                   stampOf(type, sign)));
		break;
	}
	case Op::Cast:
		emitCast(id, inst);
		break;
	case Op::Load:
		define(id, cur_->addOperation<ir::LoadOperation>(identifier(id), use(inst.a),
		                                                 stampOf(type, isInteger ? planSign(id) : Sign::None)));
		break;
	case Op::Store: {
		auto* address = use(inst.a);
		auto* value = use(inst.b, planSign(inst.b));
		cur_->addOperation<ir::StoreOperation>(value, address);
		break;
	}
	case Op::Alloca:
		define(id, cur_->addOperation<ir::AllocaOperation>(identifier(id), inst.a));
		break;
	case Op::Call:
		emitCall(id, inst);
		break;
	case Op::CallIndirect:
		emitCallIndirect(id, inst);
		break;
	case Op::FuncAddr: {
		const auto& callee = m_.callees[inst.a];
		const auto& sig = m_.signatures[callee.signature];
		std::vector<Type> parameterTypes;
		for (uint32_t i = 0; i < sig.paramCount; ++i) {
			parameterTypes.push_back(stampOf(nir::typeOf(m_.paramType(sig, i)), wordSign(m_.paramType(sig, i))));
		}
		const auto resultType = stampOf(nir::typeOf(sig.result), wordSign(sig.result));
		const auto calleeId = s_.calleeId(inst.a, resultType, parameterTypes);
		const auto& target = s_.graph.getFunctionTarget(calleeId);
		define(id,
		       cur_->addOperation<ir::FunctionAddressOfOperation>(target.getName().getMangled(), target.getName().get(),
		                                                          s_.addressOf(callee), identifier(id), calleeId));
		break;
	}
	case Op::PtrConst: {
		void* address = inst.a == nir::NONE ? nullptr : nirbackend::resolvePointer(m_, inst.a, s_.host);
		define(id, cur_->addOperation<ir::ConstPtrOperation>(identifier(id), address));
		break;
	}
	case Op::BindAddr:
	case Op::DataAddr:
		fail(name_, "bind.addr and data.addr have no 0.3 counterpart");
	case Op::Br:
	case Op::CondBr:
	case Op::Ret:
		emitTerminator(inst);
		break;
	case Op::BlockArg:
		break; // created with its block
	default:
		fail(name_, fmt::format("the operation {} has no 0.3 counterpart", inst.op));
	}
}

ir::FunctionOperation* FunctionBridge::build() {
	if (record_.level != static_cast<uint8_t>(nir::Level::SSA)) {
		fail(name_, "the function is not an SSA function");
	}
	plan();
	nextId_ = static_cast<uint32_t>(insts_.size());
	values_.assign(insts_.size(), nullptr);
	auto& arena = s_.arena;
	const auto blockCount = static_cast<uint32_t>(blocks_.size());

	// the returned value of a function with several returns goes through one block
	uint32_t returns = 0;
	for (uint32_t b = 0; b < blockCount; ++b) {
		if ((blocks_[b].flags & nir::BLOCK_LANDING_PAD) == 0 && blocks_[b].count != 0 &&
		    static_cast<Op>(insts_[blocks_[b].first + blocks_[b].count - 1].op) == Op::Ret) {
			++returns;
		}
	}

	// blocks and their arguments; landing pads are not blocks of 0.3 (the destructors go with the calls)
	irBlocks_.assign(blockCount, nullptr);
	std::vector<Type> parameters;
	for (uint32_t b = 0; b < blockCount; ++b) {
		if (blocks_[b].flags & nir::BLOCK_LANDING_PAD) {
			continue;
		}
		std::vector<ir::BasicBlockArgument*> arguments;
		for (uint32_t a = 0; a < blocks_[b].argCount; ++a) {
			const auto id = blocks_[b].first + a;
			const auto type = static_cast<TypeId>(insts_[id].type);
			auto sign = planSign(id);
			if (b == 0) {
				sign = signatureSign(m_.paramType(m_.signatures[record_.signature], a), id);
			}
			auto* argument = arena.create<ir::BasicBlockArgument>(identifier(id), stampOf(type, sign));
			arguments.push_back(argument);
			values_[id] = argument;
			if (b == 0) {
				parameters.push_back(argument->getStamp());
			}
		}
		irBlocks_[b] = arena.create<BasicBlock>(arena, ir::BlockIdentifier {b}, std::move(arguments));
		order_.push_back(irBlocks_[b]);
	}

	Type outputType = stampOf(returnType_, returnSign_);
	if (returns > 1) {
		std::vector<ir::BasicBlockArgument*> arguments;
		if (returnType_ != TypeId::Void) {
			arguments.push_back(arena.create<ir::BasicBlockArgument>(identifier(newId()), outputType));
		}
		returnBlock_ = arena.create<BasicBlock>(arena, ir::BlockIdentifier {blockCount}, std::move(arguments));
		if (returnType_ == TypeId::Void) {
			returnBlock_->addOperation<ir::ReturnOperation>();
		} else {
			returnBlock_->addOperation<ir::ReturnOperation>(returnBlock_->getArguments().front());
		}
	}

	for (uint32_t b = 0; b < blockCount; ++b) {
		if (irBlocks_[b] == nullptr) {
			continue;
		}
		cur_ = irBlocks_[b];
		for (uint32_t i = blocks_[b].argCount; i < blocks_[b].count; ++i) {
			const auto id = blocks_[b].first + i;
			emit(id, insts_[id]);
		}
	}
	if (returnBlock_ != nullptr) {
		order_.push_back(returnBlock_);
	}

	std::vector<ir::AllocaSpec> allocas;
	for (const auto& a : m_.functionAllocas(f_)) {
		allocas.push_back({a.size, a.align});
	}
	std::unordered_map<std::string, std::string> attributes;
	if (record_.visibility == static_cast<uint8_t>(nir::Visibility::Exported)) {
		attributes.emplace("entry", "true");
	}
	return arena.create<ir::FunctionOperation>(name_, std::move(order_), std::vector<Type> {},
	                                           std::vector<std::string> {}, outputType, std::move(allocas),
	                                           std::move(attributes));
}

} // namespace

void buildIRFromNir(const nir::Module& module, ir::IRGraph& graph, const engine::Options& options,
                    const nirbackend::HostEnvironment* host) {
	ModuleBridge bridge(module, graph, options, host);
	bridge.calleeIds.assign(module.callees.size(), ir::INVALID_FUNCTION_ID);
	// every function has its entry in the function table before any call refers to it
	bridge.internalIds.resize(module.functions.size());
	for (uint32_t f = 0; f < module.functions.size(); ++f) {
		ir::CalleeDescriptor descriptor;
		descriptor.kind = ir::CalleeDescriptor::Kind::Internal;
		descriptor.key = bridge.internalKey(f);
		descriptor.customName = std::string(module.name(module.functions[f].nameId));
		bridge.internalIds[f] = graph.internCallee(descriptor);
	}
	for (uint32_t f = 0; f < module.functions.size(); ++f) {
		auto* function = FunctionBridge(bridge, f).build();
		graph.addFunctionOperation(function);
		graph.defineFunction(bridge.internalIds[f], function);
		graph.getFunctionTableMut()
		    .getMut(bridge.internalIds[f])
		    .setDerivedAttributes(attributesOf(module.functions[f].attrs));
	}
}

} // namespace nautilus::compiler

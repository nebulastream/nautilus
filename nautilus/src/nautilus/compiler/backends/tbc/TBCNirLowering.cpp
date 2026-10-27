#include "nautilus/compiler/backends/tbc/TBCNirLowering.hpp"
#include "nautilus/compiler/backends/NativeClosure.hpp"
#include "nautilus/compiler/backends/nir/NirRuntime.hpp"
#include "nautilus/exceptions/NotImplementedException.hpp"
#include "nautilus/exceptions/RuntimeException.hpp"
#include "nautilus/extensions/ExtensionRegistry.hpp"
#include "nautilus/nir/Inst.hpp"
#include <cstring>
#include <functional>
#include <map>
#include <unordered_map>

namespace nautilus::compiler::tbc {

using nir::CalleeKind;
using N = nir::Op;
using nir::TypeId;

Type tbcTypeOfWord(nir::TypeWord word) {
	const auto kind = nir::extKindOf(word);
	switch (nir::typeOf(word)) {
	case TypeId::Void:
		return Type::v;
	case TypeId::Bool:
		return Type::b;
	case TypeId::I8:
		return kind == nir::ExtKind::Zext ? Type::ui8 : Type::i8;
	case TypeId::I16:
		return kind == nir::ExtKind::Zext ? Type::ui16 : Type::i16;
	case TypeId::I32:
		return kind == nir::ExtKind::Zext ? Type::ui32 : Type::i32;
	case TypeId::I64:
		return Type::i64;
	case TypeId::F32:
		return Type::f32;
	case TypeId::F64:
		return Type::f64;
	case TypeId::Ptr:
		return Type::ptr;
	default:
		throw NotImplementedException(
		    "tbc: extension types in a signature are lowered with the extension opcode (M4.1)");
	}
}

namespace {

constexpr uint32_t kUnbound = ~0u;

/// Index into the 10-type family order i8, i16, i32, i64, ui8, ui16, ui32, ui64, f32, f64. `bool` runs as ui8 and `ptr`
/// as i64 (ui64 where it is ordered or converted, since a pointer orders as unsigned, 13 §5.4).
int familyIndex(TypeId t, bool isUnsigned) {
	const int u = isUnsigned ? 4 : 0;
	switch (t) {
	case TypeId::I8:
		return 0 + u;
	case TypeId::I16:
		return 1 + u;
	case TypeId::I32:
		return 2 + u;
	case TypeId::I64:
	case TypeId::Ptr:
		return 3 + u;
	case TypeId::Bool:
		return 4;
	case TypeId::F32:
		return 8;
	case TypeId::F64:
		return 9;
	default:
		throw NotImplementedException("tbc: an extension type in an operation (M4.1)");
	}
}

Op family(Op base, int index) {
	return static_cast<Op>(opIndex(base) + index);
}

unsigned bitWidth(TypeId t) {
	return nir::sizeOf(t) * 8;
}

uint64_t widthMask(unsigned bits) {
	return bits == 64 ? ~uint64_t {0} : (uint64_t {1} << bits) - 1;
}

class Lowering {
public:
	Lowering(const nir::Module& module, uint32_t function, TBCProgram& program, TBCFunction& out,
	         const nirbackend::HostEnvironment& host)
	    : m_(module), f_(function), rec_(module.functions[function]), insts_(module.functionInsts(function)),
	      blocks_(module.functionBlocks(function)), program_(program), out_(out), host_(host) {
	}

	void run() {
		const uint32_t blockCount = static_cast<uint32_t>(blocks_.size());
		regOf_.assign(insts_.size(), kUnbound);
		blockLabel_.resize(blockCount);
		argRegs_.resize(blockCount);
		for (uint32_t b = 0; b < blockCount; ++b) {
			blockLabel_[b] = newLabel();
		}

		// registers of the block arguments; those of the entry block are the function's parameters
		for (uint32_t b = 0; b < blockCount; ++b) {
			for (uint32_t k = 0; k < blocks_[b].argCount; ++k) {
				const auto reg = newRegister();
				regOf_[blocks_[b].first + k] = reg;
				argRegs_[b].push_back(reg);
			}
		}
		out_.argRegs = argRegs_[0];

		// one pinned register per alloca, laid out with its alignment; the pointers are computed at frame-push time
		uint32_t allocaOffset = 0;
		for (const auto& alloca : m_.functionAllocas(f_)) {
			const uint32_t align = alloca.align == 0 ? 1 : alloca.align;
			if (align > 16) {
				throw NotImplementedException("tbc: alloca alignment > 16 is not supported");
			}
			allocaOffset = (allocaOffset + align - 1) & ~(align - 1);
			const auto reg = newRegister();
			out_.allocaRegs.emplace_back(reg, allocaOffset);
			allocaRegs_.push_back(reg);
			allocaOffset += alloca.size;
		}
		out_.allocaBytes = allocaOffset;

		for (uint32_t b = 0; b < blockCount; ++b) {
			lowerBlock(b);
		}
		emitColdCode();
		if (exitLabel_ != kUnbound) {
			bind(exitLabel_);
			emit(Op::RET, kNoReg);
		}
		patchFixups();
		finalize();
	}

private:
	const nir::Module& m_;
	uint32_t f_;
	const nir::FunctionRecord& rec_;
	std::span<const nir::Inst> insts_;
	std::span<const nir::BlockRecord> blocks_;
	TBCProgram& program_;
	TBCFunction& out_;
	const nirbackend::HostEnvironment& host_;

	uint32_t nextRegister_ = 0;
	std::vector<uint32_t> regOf_; // value id -> register
	std::vector<uint32_t> blockLabel_;
	std::vector<std::vector<uint16_t>> argRegs_;
	std::vector<uint16_t> allocaRegs_;
	std::vector<std::pair<uint16_t, uint64_t>> constInits_;
	std::map<std::pair<uint64_t, uint32_t>, uint16_t> constRegs_; // (bits, constant pool index or ~0) -> register
	std::vector<uint16_t> scratch_;

	// ---- code and labels --------------------------------------------------------------------------------------

	struct Fixup {
		uint32_t at;
		uint32_t label;
	};
	std::vector<int32_t> labelPos_;
	std::vector<Fixup> fixups_;
	std::vector<std::function<void()>> cold_;
	uint32_t exitLabel_ = kUnbound;
	uint32_t nextBlock_ = kUnbound; // the block emitted next, for fall-through

	uint32_t newLabel() {
		labelPos_.push_back(-1);
		return static_cast<uint32_t>(labelPos_.size() - 1);
	}
	void bind(uint32_t label) {
		labelPos_[label] = static_cast<int32_t>(out_.code.size());
	}
	uint32_t exitLabel() {
		if (exitLabel_ == kUnbound) {
			exitLabel_ = newLabel();
		}
		return exitLabel_;
	}

	void emit(Op op, uint16_t a = 0, uint16_t b = 0, uint16_t c = 0) {
		out_.code.push_back(Instr {opIndex(op), a, b, c});
	}
	/// An instruction whose b|c words hold the offset of a label, patched when all labels are bound.
	void emitBranch(Op op, uint16_t a, uint32_t label) {
		fixups_.push_back({static_cast<uint32_t>(out_.code.size()), label});
		emit(op, a);
	}
	void jump(uint32_t label) {
		emitBranch(Op::JMP, 0, label);
	}

	void patchFixups() {
		for (const auto& fixup : fixups_) {
			const int32_t target = labelPos_[fixup.label];
			if (target < 0) {
				throw RuntimeException("tbc: a branch to a label that was never bound");
			}
			const auto packed = static_cast<uint32_t>(target - static_cast<int32_t>(fixup.at));
			out_.code[fixup.at].b = static_cast<uint16_t>(packed & 0xFFFF);
			out_.code[fixup.at].c = static_cast<uint16_t>(packed >> 16);
		}
	}

	// ---- registers and constants ------------------------------------------------------------------------------

	uint16_t newRegister() {
		if (nextRegister_ >= kNoReg) {
			throw RuntimeException("tbc: function requires more than 65534 registers");
		}
		return static_cast<uint16_t>(nextRegister_++);
	}

	/// A pinned register whose slot holds `bits` from the first instruction on.
	uint16_t constSlot(uint64_t bits) {
		auto key = std::make_pair(bits, ~0u);
		auto it = constRegs_.find(key);
		if (it != constRegs_.end()) {
			return it->second;
		}
		const auto reg = newRegister();
		constInits_.emplace_back(reg, bits);
		constRegs_.emplace(key, reg);
		return reg;
	}

	uint16_t scratch(unsigned index) {
		while (scratch_.size() <= index) {
			scratch_.push_back(newRegister());
		}
		return scratch_[index];
	}

	TypeId typeOfRef(nir::Ref r) const {
		return nir::isConstRef(r) ? static_cast<TypeId>(m_.consts.record(r).type) : static_cast<TypeId>(insts_[r].type);
	}

	/// The register that holds the value of `r`.
	uint16_t reg(nir::Ref r) {
		if (nir::isConstRef(r)) {
			return constSlot(m_.consts.record(r).bits);
		}
		const auto reg = regOf_[r];
		if (reg == kUnbound) {
			throw RuntimeException("tbc: value %" + std::to_string(r) + " is used before it is lowered");
		}
		return static_cast<uint16_t>(reg);
	}

	uint16_t defineResult(uint32_t id) {
		const auto r = newRegister();
		regOf_[id] = r;
		return r;
	}

	// ---- blocks -----------------------------------------------------------------------------------------------

	void lowerBlock(uint32_t b) {
		bind(blockLabel_[b]);
		nextBlock_ = b + 1 < blocks_.size() ? b + 1 : kUnbound;
		const auto& block = blocks_[b];
		for (uint32_t k = block.argCount; k < block.count; ++k) {
			lowerInstruction(block.first + k);
		}
	}

	void lowerInstruction(uint32_t id) {
		const nir::Inst& inst = insts_[id];
		const auto op = static_cast<N>(inst.op);
		switch (op) {
		case N::Add:
		case N::Sub:
		case N::Mul:
			return lowerArithmetic(op, id, inst);
		case N::Div:
		case N::Rem:
			return lowerDivision(op, id, inst);
		case N::FNeg: {
			const auto src = reg(inst.a);
			const auto dst = defineResult(id);
			emit(static_cast<TypeId>(inst.type) == TypeId::F32 ? Op::NEG_f32 : Op::NEG_f64, dst, src);
			return;
		}
		case N::And:
		case N::Or: {
			const auto l = reg(inst.a), r = reg(inst.b);
			const auto dst = defineResult(id);
			emit(op == N::And ? Op::AND_b : Op::OR_b, dst, l, r);
			return;
		}
		case N::Not: {
			const auto src = reg(inst.a);
			const auto dst = defineResult(id);
			emit(Op::NOT_b, dst, src);
			return;
		}
		case N::BAnd:
		case N::BOr:
		case N::BXor: {
			const auto base = op == N::BAnd ? Op::BAND_i8 : op == N::BOr ? Op::BOR_i8 : Op::BXOR_i8;
			const auto l = reg(inst.a), r = reg(inst.b);
			const auto dst = defineResult(id);
			emit(family(base, familyIndex(static_cast<TypeId>(inst.type), false)), dst, l, r);
			return;
		}
		case N::BNot: {
			const auto src = reg(inst.a);
			const auto dst = defineResult(id);
			emit(Op::BNOT_i64, dst, src); // on the whole slot: consumers read their exact width
			return;
		}
		case N::Shl:
		case N::Shr:
			return lowerShift(op, id, inst);
		case N::Cmp:
			return lowerCompare(id, inst);
		case N::Select: {
			const auto cond = reg(inst.a), t = reg(inst.b), f = reg(inst.c);
			const auto dst = defineResult(id);
			emit(Op::SELECT, dst, cond, t);
			out_.code.push_back(Instr {opIndex(Op::TRAP), f, 0, 0}); // the operand word is never dispatched
			return;
		}
		case N::Cast:
			return lowerCast(id, inst);
		case N::Load: {
			const auto address = reg(inst.a);
			const auto dst = defineResult(id);
			emit(memoryOp(Op::LOAD_i8, static_cast<TypeId>(inst.type)), dst, address);
			return;
		}
		case N::Store: {
			const auto address = reg(inst.a), value = reg(inst.b);
			emit(memoryOp(Op::STORE_i8, typeOfRef(inst.b)), address, value);
			return;
		}
		case N::Alloca:
			regOf_[id] = allocaRegs_.at(inst.a);
			return;
		case N::Call:
			return lowerCall(id, inst);
		case N::CallIndirect:
			return lowerCallIndirect(id, inst);
		case N::FuncAddr:
			return lowerFuncAddr(id, inst);
		case N::BindAddr: {
			void* address = host_.bindingAddress ? host_.bindingAddress(m_, inst.a) : nullptr;
			if (address == nullptr) {
				throw RuntimeException("tbc: the host supplies no address for binding #b" + std::to_string(inst.a));
			}
			regOf_[id] = constSlot(reinterpret_cast<uint64_t>(address));
			return;
		}
		case N::DataAddr: {
			const auto& data = m_.data.at(inst.a);
			const auto* address = m_.dataBytes.data() + data.byteOffset;
			regOf_[id] = constSlot(reinterpret_cast<uint64_t>(address));
			return;
		}
		case N::PtrConst: {
			void* address = inst.a == nir::NONE ? nullptr : nirbackend::resolvePointer(m_, inst.a, host_);
			regOf_[id] = constSlot(reinterpret_cast<uint64_t>(address));
			return;
		}
		case N::Br: {
			const uint32_t* list = m_.extra.data() + inst.c;
			parallelCopy(inst.a, std::span<const uint32_t>(list + 1, list[0]));
			if (inst.a != nextBlock_) {
				jump(blockLabel_[inst.a]);
			}
			return;
		}
		case N::CondBr:
			return lowerCondBr(inst);
		case N::Ret:
			emit(Op::RET, inst.a == nir::NONE ? kNoReg : reg(inst.a));
			return;
		case N::Resume:
			jump(exitLabel());
			return;
		default:
			throw NotImplementedException("tbc: extension operations are lowered with the extension opcode (M4.1)");
		}
	}

	// ---- arithmetic -------------------------------------------------------------------------------------------

	void lowerArithmetic(N op, uint32_t id, const nir::Inst& inst) {
		const Op base = op == N::Add ? Op::ADD_i8 : op == N::Sub ? Op::SUB_i8 : Op::MUL_i8;
		const auto l = reg(inst.a), r = reg(inst.b);
		const auto dst = defineResult(id);
		emit(family(base, familyIndex(static_cast<TypeId>(inst.type), false)), dst, l, r);
	}

	void lowerCompare(uint32_t id, const nir::Inst& inst) {
		const unsigned predicate = inst.flags & 7;
		const bool isUnsigned = (inst.flags & 8) != 0;
		const auto type = typeOfRef(inst.a);
		// a pointer orders as unsigned 64-bit; equality does not care
		const bool unsignedOrder = isUnsigned || (type == TypeId::Ptr && predicate >= 2);
		const auto index = familyIndex(type, unsignedOrder);
		const auto l = reg(inst.a), r = reg(inst.b);
		const auto dst = defineResult(id);
		emit(static_cast<Op>(opIndex(Op::EQ_i8) + static_cast<int>(predicate) * 10 + index), dst, l, r);
	}

	void lowerShift(N op, uint32_t id, const nir::Inst& inst) {
		const auto type = static_cast<TypeId>(inst.type);
		const bool isUnsigned = op == N::Shr && (inst.flags & 1) != 0;
		const auto index = familyIndex(type, isUnsigned);
		const unsigned bits = bitWidth(type);
		const auto value = reg(inst.a);
		// the amount is taken modulo the bit width (D10)
		uint16_t amount;
		if (nir::isConstRef(inst.b)) {
			amount = constSlot(m_.consts.record(inst.b).bits & (bits - 1));
		} else {
			amount = scratch(0);
			emit(family(Op::BAND_i8, index), amount, reg(inst.b), constSlot(bits - 1));
		}
		const auto dst = defineResult(id);
		emit(family(op == N::Shl ? Op::SHL_i8 : Op::SHR_i8, index), dst, value, amount);
	}

	/// Where an exception raised at an instruction with unwind entry `unwind` continues: its landing pad, or the exit
	/// of the function.
	uint32_t unwindLabel(uint32_t unwind) {
		return unwind == nir::NO_CLEANUP ? exitLabel() : blockLabel_.at(unwind);
	}

	void lowerDivision(N op, uint32_t id, const nir::Inst& inst) {
		const auto type = static_cast<TypeId>(inst.type);
		if (nir::isFloat(type)) {
			const auto l = reg(inst.a), r = reg(inst.b);
			const auto dst = defineResult(id);
			if (op == N::Div) {
				emit(family(Op::DIV_i8, familyIndex(type, false)), dst, l, r);
			} else {
				emitHelperCall(
				    type == TypeId::F32 ? reinterpret_cast<void*>(&nirbackend::nautilus_rt_fmod_f32)
				                        : reinterpret_cast<void*>(&nirbackend::nautilus_rt_fmod_f64),
				    {type == TypeId::F32 ? Type::f32 : Type::f64, type == TypeId::F32 ? Type::f32 : Type::f64}, {l, r},
				    type == TypeId::F32 ? Type::f32 : Type::f64, dst);
			}
			return;
		}
		const bool isUnsigned = (inst.flags & 1) != 0;
		const bool checked = (inst.flags & 2) != 0;
		const auto index = familyIndex(type, isUnsigned);
		const unsigned bits = bitWidth(type);
		const auto l = reg(inst.a), r = reg(inst.b);
		if (checked) {
			// the divisor is zero, or the signed minimum is divided by -1: take the cold path that throws (13 §5.10)
			const auto zero = scratch(0);
			emit(family(Op::EQ_i8, index), zero, r, constSlot(0));
			uint16_t bad = zero;
			if (!isUnsigned) {
				const auto isMin = scratch(1), isMinusOne = scratch(2);
				emit(family(Op::EQ_i8, index), isMin, l, constSlot(uint64_t {1} << (bits - 1)));
				emit(family(Op::EQ_i8, index), isMinusOne, r, constSlot(widthMask(bits)));
				emit(Op::AND_b, isMin, isMin, isMinusOne);
				emit(Op::OR_b, zero, zero, isMin);
			}
			const auto cold = newLabel();
			emitBranch(Op::CJMP, bad, cold);
			const auto target = unwindLabel(inst.c);
			cold_.push_back([this, cold, target]() {
				bind(cold);
				emitThrowArith();
				jump(target);
			});
		}
		const auto dst = defineResult(id);
		emit(family(op == N::Div ? Op::DIV_i8 : Op::MOD_i8, index), dst, l, r);
	}

	/// The call of `nautilus_rt_throw_arith` through its capture thunk, which stores the exception for the pad.
	void emitThrowArith() {
		CallSite site;
		site.target = nirbackend::throwArithThunk();
		site.returnType = Type::v;
		site.argTypes = {Type::ptr};
		site.argRegs = {constSlot(reinterpret_cast<uint64_t>(nirbackend::throwArithTarget()))};
		emit(Op::CALL_EXT, kNoReg, addCallSite(std::move(site)));
	}

	// ---- casts ------------------------------------------------------------------------------------------------

	void lowerCast(uint32_t id, const nir::Inst& inst) {
		const auto kind = inst.flags & 15;
		const auto from = typeOfRef(inst.a);
		const auto to = static_cast<TypeId>(inst.type);
		const auto src = reg(inst.a);
		const auto dst = defineResult(id);
		auto castOp = [&](int srcIndex, int dstIndex) {
			emit(static_cast<Op>(opIndex(Op::CAST_i8_i8) + srcIndex * 10 + dstIndex), dst, src);
		};
		switch (kind) {
		case 0: // sext
			if (from == TypeId::Bool) {
				// 0 or -1: zero-extend, then negate
				const auto widened = scratch(0);
				emit(static_cast<Op>(opIndex(Op::CAST_ui8_i8) + familyIndex(to, false)), widened, src);
				emit(family(Op::SUB_i8, familyIndex(to, false)), dst, constSlot(0), widened);
				return;
			}
			castOp(familyIndex(from, false), familyIndex(to, false));
			return;
		case 1: // zext
			castOp(familyIndex(from, true), familyIndex(to, true));
			return;
		case 2: // trunc
			castOp(familyIndex(from, false), familyIndex(to, false));
			return;
		case 3: // sitofp
			castOp(familyIndex(from, false), familyIndex(to, false));
			return;
		case 4: // uitofp
			castOp(familyIndex(from, true), familyIndex(to, false));
			return;
		case 5:   // fptosi
		case 6: { // fptoui: saturating, through a helper (D11)
			const bool isSigned = kind == 5;
			const unsigned bits = bitWidth(to);
			void* helper = nirbackend::floatToIntHelper(isSigned, bits, bitWidth(from));
			if (helper == nullptr) {
				throw NotImplementedException("tbc: unsupported float to integer cast");
			}
			const auto resultType = bits == 8    ? (isSigned ? Type::i8 : Type::ui8)
			                        : bits == 16 ? (isSigned ? Type::i16 : Type::ui16)
			                        : bits == 32 ? (isSigned ? Type::i32 : Type::ui32)
			                                     : (isSigned ? Type::i64 : Type::ui64);
			// the result register was defined above; the call writes it
			CallSite site;
			site.target = helper;
			site.returnType = resultType;
			site.argTypes = {from == TypeId::F32 ? Type::f32 : Type::f64};
			site.argRegs = {src};
			emit(Op::CALL_EXT, dst, addCallSite(std::move(site)));
			return;
		}
		case 7: // fpext
		case 8: // fptrunc
			castOp(familyIndex(from, false), familyIndex(to, false));
			return;
		case 9:  // bitcast
		case 10: // ptrtoint
		case 11: // inttoptr
			emit(Op::MOV, dst, src);
			return;
		default:
			throw RuntimeException("tbc: an invalid cast kind");
		}
	}

	// ---- memory -----------------------------------------------------------------------------------------------

	Op memoryOp(Op base, TypeId type) const {
		if (type == TypeId::Bool) {
			return family(base, 10);
		}
		return family(base, familyIndex(type, false));
	}

	// ---- control flow -----------------------------------------------------------------------------------------

	/// Copies the arguments of an edge into the parameter registers of `target`: a parallel copy, since an argument may
	/// be a parameter of the target itself.
	void parallelCopy(uint32_t target, std::span<const uint32_t> args) {
		std::vector<std::pair<uint16_t, uint16_t>> pairs; // destination, source
		for (size_t i = 0; i < args.size(); ++i) {
			const uint16_t dst = argRegs_[target][i];
			const uint16_t src = reg(args[i]);
			if (dst != src) {
				pairs.emplace_back(dst, src);
			}
		}
		emitParallelCopy(std::move(pairs));
	}

	void emitParallelCopy(std::vector<std::pair<uint16_t, uint16_t>> pairs) {
		std::unordered_map<uint16_t, int> pendingReads;
		for (auto& p : pairs) {
			++pendingReads[p.second];
		}
		std::vector<char> done(pairs.size(), 0);
		size_t remaining = pairs.size();
		while (remaining > 0) {
			bool progressed = false;
			for (size_t i = 0; i < pairs.size(); ++i) {
				if (done[i]) {
					continue;
				}
				const auto [dst, src] = pairs[i];
				if (pendingReads[dst] == 0) {
					emit(Op::MOV, dst, src);
					done[i] = 1;
					--remaining;
					progressed = true;
					--pendingReads[src];
				}
			}
			if (progressed) {
				continue;
			}
			// a cycle: stash one destination in a temporary
			for (size_t i = 0; i < pairs.size(); ++i) {
				if (done[i]) {
					continue;
				}
				const uint16_t dst = pairs[i].first;
				const uint16_t temp = newRegister();
				emit(Op::MOV, temp, dst);
				for (size_t j = 0; j < pairs.size(); ++j) {
					if (!done[j] && pairs[j].second == dst) {
						pairs[j].second = temp;
					}
				}
				pendingReads[dst] = 0;
				break;
			}
		}
	}

	void lowerCondBr(const nir::Inst& inst) {
		const uint32_t* list = m_.extra.data() + inst.c;
		const uint32_t trueBlock = inst.b;
		const uint32_t trueArgc = list[1];
		std::span<const uint32_t> trueArgs(list + 2, trueArgc);
		const uint32_t falseBlock = list[2 + trueArgc];
		const uint32_t falseArgc = list[3 + trueArgc];
		std::span<const uint32_t> falseArgs(list + 4 + trueArgc, falseArgc);
		const auto cond = reg(inst.a);

		if (trueArgc == 0 && falseArgc == 0) {
			emitBranch(Op::CJMP, cond, blockLabel_[trueBlock]);
			if (falseBlock != nextBlock_) {
				jump(blockLabel_[falseBlock]);
			}
			return;
		}
		// arguments to pass: the true edge gets a stub after the function body, the false edge falls through
		const auto stub = newLabel();
		emitBranch(Op::CJMP, cond, stub);
		parallelCopy(falseBlock, falseArgs);
		if (falseBlock != nextBlock_) {
			jump(blockLabel_[falseBlock]);
		}
		// the stub copies after the code of all blocks, so it is lowered later but reads registers fixed now
		std::vector<std::pair<uint16_t, uint16_t>> pairs;
		for (size_t i = 0; i < trueArgs.size(); ++i) {
			pairs.emplace_back(argRegs_[trueBlock][i], reg(trueArgs[i]));
		}
		cold_.push_back([this, stub, pairs = std::move(pairs), trueBlock]() mutable {
			bind(stub);
			std::erase_if(pairs, [](const auto& p) { return p.first == p.second; });
			emitParallelCopy(std::move(pairs));
			jump(blockLabel_[trueBlock]);
		});
	}

	void emitColdCode() {
		// a cold block may add more cold blocks only through newLabel, never through cold_
		for (size_t i = 0; i < cold_.size(); ++i) {
			cold_[i]();
		}
	}

	// ---- calls ------------------------------------------------------------------------------------------------

	uint16_t addCallSite(CallSite&& site) {
		if (program_.callsites.size() >= kNoReg) {
			throw RuntimeException("tbc: too many call sites in program");
		}
		program_.callsites.emplace_back(std::move(site));
		return static_cast<uint16_t>(program_.callsites.size() - 1);
	}

	/// A call of a native helper that cannot throw.
	void emitHelperCall(void* target, std::vector<Type> argTypes, std::vector<uint16_t> argRegs, Type returnType,
	                    uint16_t dst) {
		CallSite site;
		site.target = target;
		site.returnType = returnType;
		site.argTypes = std::move(argTypes);
		site.argRegs = std::move(argRegs);
		emit(Op::CALL_EXT, dst, addCallSite(std::move(site)));
	}

	struct CallArgs {
		std::vector<uint16_t> regs;
		uint32_t unwind;
		uint32_t thunk;
	};

	CallArgs callArguments(const uint32_t* list) {
		CallArgs a;
		const uint32_t argc = list[0];
		for (uint32_t i = 0; i < argc; ++i) {
			a.regs.push_back(reg(list[1 + i]));
		}
		a.unwind = list[1 + argc];
		a.thunk = list[2 + argc];
		return a;
	}

	CallSite siteFor(uint32_t signature, std::vector<uint16_t> argRegs) {
		const auto& sig = m_.signatures[signature];
		CallSite site;
		site.returnType = tbcTypeOfWord(sig.result);
		for (uint32_t p = 0; p < sig.paramCount; ++p) {
			site.argTypes.push_back(tbcTypeOfWord(m_.paramType(sig, p)));
		}
		site.argRegs = std::move(argRegs);
		return site;
	}

	/// After a call that may throw: continue at the pad, or leave the function, if an exception is pending.
	void emitCheckPending(uint32_t unwind) {
		if (unwind != nir::NO_UNWIND) {
			emitBranch(Op::CHECK_PENDING, 0, unwindLabel(unwind));
		}
	}

	uint16_t resultRegister(uint32_t id, const nir::Inst& inst) {
		return static_cast<TypeId>(inst.type) == TypeId::Void ? kNoReg : defineResult(id);
	}

	void lowerCall(uint32_t id, const nir::Inst& inst) {
		const auto& callee = m_.callees.at(inst.a);
		auto args = callArguments(m_.extra.data() + inst.c);
		auto site = siteFor(callee.signature, args.regs);
		const auto dst = resultRegister(id, inst);

		switch (static_cast<CalleeKind>(callee.kind)) {
		case CalleeKind::Internal: {
			site.internalFnIdx = callee.target;
			emit(Op::CALL, dst, static_cast<uint16_t>(callee.target), addCallSite(std::move(site)));
			emitCheckPending(args.unwind);
			return;
		}
		case CalleeKind::External:
			return emitNativeCall(std::move(site), nirbackend::resolvePointer(m_, callee.target, host_), args, dst);
		case CalleeKind::Intrinsic: {
			const auto* info = ext::ExtensionRegistry::instance().intrinsic(callee.target);
			if (info == nullptr) {
				throw RuntimeException("tbc: an intrinsic callee that is not registered");
			}
			// no backend lowering yet: call the fallback, through the intrinsic's capture thunk if it may throw
			if (args.unwind != nir::NO_UNWIND) {
				args.thunk = nir::NONE;
			}
			return emitNativeCall(std::move(site), info->fallback, args, dst, info->captureThunk);
		}
		case CalleeKind::Imported:
			throw NotImplementedException("tbc: imported callees come with modules and linking (M4.6)");
		}
	}

	void emitNativeCall(CallSite site, void* target, const CallArgs& args, uint16_t dst,
	                    void* intrinsicThunk = nullptr) {
		if (args.unwind == nir::NO_UNWIND) {
			site.target = target;
			emit(Op::CALL_EXT, dst, addCallSite(std::move(site)));
			return;
		}
		// a call that may throw goes through the capture thunk, a real C++ frame, so that the exception is caught
		// before it crosses the dyncall trampoline; the target becomes the thunk's first argument
		void* thunk = intrinsicThunk;
		if (thunk == nullptr) {
			if (args.thunk == nir::NONE) {
				throw RuntimeException("tbc: a call that may throw names no capture thunk");
			}
			thunk = nirbackend::resolvePointer(m_, args.thunk, host_);
		}
		site.argTypes.insert(site.argTypes.begin(), Type::ptr);
		site.argRegs.insert(site.argRegs.begin(), constSlot(reinterpret_cast<uint64_t>(target)));
		site.target = thunk;
		emit(Op::CALL_EXT, dst, addCallSite(std::move(site)));
		emitCheckPending(args.unwind);
	}

	void lowerCallIndirect(uint32_t id, const nir::Inst& inst) {
		const uint32_t* list = m_.extra.data() + inst.c;
		auto args = callArguments(list);
		auto site = siteFor(inst.b, args.regs);
		const auto target = reg(inst.a);
		const auto dst = resultRegister(id, inst);
		if (args.unwind == nir::NO_UNWIND) {
			emit(Op::CALL_IND, dst, target, addCallSite(std::move(site)));
			return;
		}
		if (args.thunk == nir::NONE) {
			throw RuntimeException("tbc: an indirect call that may throw names no capture thunk");
		}
		site.argTypes.insert(site.argTypes.begin(), Type::ptr);
		site.argRegs.insert(site.argRegs.begin(), target);
		site.target = nirbackend::resolvePointer(m_, args.thunk, host_);
		emit(Op::CALL_EXT, dst, addCallSite(std::move(site)));
		emitCheckPending(args.unwind);
	}

	void lowerFuncAddr(uint32_t id, const nir::Inst& inst) {
		const auto& callee = m_.callees.at(inst.a);
		uint64_t address = 0;
		switch (static_cast<CalleeKind>(callee.kind)) {
		case CalleeKind::Internal:
			// internal functions have no machine code; the native closure built for them is a real C function
			address = reinterpret_cast<uint64_t>(program_.closures.at(callee.target)->code());
			break;
		case CalleeKind::External:
			address = reinterpret_cast<uint64_t>(nirbackend::resolvePointer(m_, callee.target, host_));
			break;
		case CalleeKind::Imported:
			throw NotImplementedException("tbc: imported callees come with modules and linking (M4.6)");
		case CalleeKind::Intrinsic:
			throw RuntimeException("tbc: an intrinsic has no address");
		}
		regOf_[id] = constSlot(address);
	}

	// ---- the frame --------------------------------------------------------------------------------------------

	void finalize() {
		out_.regSlots = nextRegister_;
		out_.initImage.assign(nextRegister_, 0);
		for (const auto& [r, bits] : constInits_) {
			out_.initImage[r] = bits;
		}
		// +2 slack slots so the alloca area can be 16-aligned at runtime
		const uint32_t allocaSlots = out_.allocaBytes > 0 ? (out_.allocaBytes + 7) / 8 + 2 : 0;
		out_.frameSlots = 3 + nextRegister_ + allocaSlots;
	}
};

} // namespace

void TBCNirLowering::lower(const nir::Module& module, uint32_t function, TBCProgram& program, TBCFunction& out,
                           const nirbackend::HostEnvironment& host) {
	Lowering(module, function, program, out, host).run();
}

} // namespace nautilus::compiler::tbc

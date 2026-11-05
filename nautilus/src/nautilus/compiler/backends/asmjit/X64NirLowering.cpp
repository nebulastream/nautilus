#include "nautilus/CompilationStatistics.hpp"
#include "nautilus/common/ExceptionTransport.hpp"
#include "nautilus/compiler/DumpHandler.hpp"
#include "nautilus/compiler/backends/asmjit/AsmJitNirLowering.hpp"
#include "nautilus/compiler/backends/asmjit/X64PostRAPeepholePass.hpp"
#include "nautilus/compiler/backends/asmjit/intrinsics/AsmJitBackendIntrinsic.hpp"
#include "nautilus/compiler/backends/nir/NirRuntime.hpp"
#include "nautilus/exceptions/NotImplementedException.hpp"
#include "nautilus/exceptions/RuntimeException.hpp"
#include "nautilus/extensions/ExtensionRegistry.hpp"
#include "nautilus/nir/Inst.hpp"
#include "nautilus/nir/Operands.hpp"
#include <algorithm>
#include <asmjit/x86.h>
#include <bit>
#include <chrono>
#include <cstring>
#include <functional>
#include <optional>
#include <span>
#include <stdexcept>

// The x86-64 lowering of SSA NIR (docs/plans/nautilus-0.4/05-backends.md §4, M4.4): the instruction selection of the
// 0.3 lowering over AsmJit's `Compiler` and register allocator, reading the module's flat arrays instead of an
// `IRGraph`. Every per-value side table is an array indexed by the value id of the function.
//
// Integers live in 64-bit virtual registers and are not kept extended: an operation on a narrower type leaves garbage
// above its width, and each consumer that reads more than the low bits (division, right shifts, widening casts, calls,
// returns) extends what it needs. `bool` is a byte that holds 0 or 1.

namespace nautilus::compiler::asmjit {

using namespace ::asmjit;
using namespace ::asmjit::x86;
using nir::CalleeKind;
using N = nir::Op;
using nir::Ref;
using nir::TypeId;

namespace {

class ThrowOnError : public ::asmjit::ErrorHandler {
public:
	void handleError(::asmjit::Error err, const char* message, ::asmjit::BaseEmitter* /*origin*/) override {
		fprintf(stderr, "[AsmJit] Error %u: %s\n", err, message ? message : "(null)");
		throw std::runtime_error(std::string("AsmJit error: ") + (message ? message : "unknown"));
	}
};

bool isFp(TypeId t) {
	return t == TypeId::F32 || t == TypeId::F64;
}

/// The width of a value in bits; a `bool` is one byte.
unsigned bitsOf(TypeId t) {
	switch (t) {
	case TypeId::Bool:
	case TypeId::I8:
		return 8;
	case TypeId::I16:
		return 16;
	case TypeId::I32:
	case TypeId::F32:
		return 32;
	default:
		return 64;
	}
}

/// The width an operation on a value of `t` is carried out in.
unsigned opBitsOf(TypeId t) {
	return bitsOf(t) == 64 ? 64 : 32;
}

Gp view(const Gp& r, unsigned bits) {
	switch (bits) {
	case 8:
		return r.r8();
	case 16:
		return r.r16();
	case 32:
		return r.r32();
	default:
		return r.r64();
	}
}

::asmjit::TypeId asmjitType(nir::TypeWord word) {
	const auto kind = nir::extKindOf(word);
	switch (nir::typeOf(word)) {
	case TypeId::Void:
		return ::asmjit::TypeId::kVoid;
	case TypeId::Bool:
		return ::asmjit::TypeId::kUInt8;
	case TypeId::I8:
		return kind == nir::ExtKind::Zext ? ::asmjit::TypeId::kUInt8 : ::asmjit::TypeId::kInt8;
	case TypeId::I16:
		return kind == nir::ExtKind::Zext ? ::asmjit::TypeId::kUInt16 : ::asmjit::TypeId::kInt16;
	case TypeId::I32:
		return kind == nir::ExtKind::Zext ? ::asmjit::TypeId::kUInt32 : ::asmjit::TypeId::kInt32;
	case TypeId::I64:
		return ::asmjit::TypeId::kInt64;
	case TypeId::F32:
		return ::asmjit::TypeId::kFloat32;
	case TypeId::F64:
		return ::asmjit::TypeId::kFloat64;
	case TypeId::Ptr:
		return ::asmjit::TypeId::kUIntPtr;
	default:
		throw NotImplementedException("asmjit: extension types in a signature are lowered with the extension opcode");
	}
}

/// log2(`value`) for 1, 2, 4 and 8, the scales of an x86 address.
std::optional<uint32_t> addressScaleShift(uint64_t value) {
	switch (value) {
	case 1:
		return 0;
	case 2:
		return 1;
	case 4:
		return 2;
	case 8:
		return 3;
	default:
		return std::nullopt;
	}
}

/// `base + offset + disp`, where offset is a plain 64-bit value or `scaledInput << shift` computed by `offset`.
struct AddressParts {
	Ref base = nir::NONE;
	Ref offset = nir::NONE;      ///< NONE when the address is `base + disp`
	Ref scaledInput = nir::NONE; ///< x when `offset` computes `x << shift`
	uint32_t shift = 0;
	int32_t disp = 0;
};

constexpr uint8_t kConstValue = 1;  ///< a constant that no code defines: consumers fold or rematerialise it
constexpr uint8_t kAddressPart = 2; ///< an address computation that every consumer folds into its memory operand
constexpr uint8_t kScaledIndex = 4; ///< a scaled index that every address add folds

/// The state of the function being lowered, in arrays indexed by value id (the position of the instruction within the
/// function) or block id.
struct Fn {
	uint32_t index = 0;
	std::span<const nir::Inst> insts;
	std::span<const nir::BlockRecord> blocks;
	std::vector<BaseReg> regs;
	std::vector<uint32_t> useCount;
	std::vector<uint8_t> lastUse; ///< bit 0 / bit 1: operand a / b is not used again later in the block
	std::vector<uint32_t> blockOf;
	std::vector<uint32_t> addressUses; ///< uses as the address of a load or store in the block that defines the value
	std::vector<uint32_t> scaledUses;  ///< uses as the scaled index of a foldable address add in the same block
	std::vector<uint8_t> deferred;
	std::vector<uint64_t> constBits;
	std::vector<uint32_t> soleUser;    ///< for a value with one use: the instruction that uses it
	std::vector<uint8_t> fusedLoad;    ///< a cast whose load already widened the value
	std::vector<uint8_t> fusedCompare; ///< a `condbr` whose compare was not materialised
	std::vector<Label> labels;
	std::vector<uint32_t> resolved; ///< a branch to block b goes to resolved[b]: blocks that only jump are threaded
	std::vector<uint8_t> threaded;  ///< blocks that are not emitted because every branch to them goes past them
	std::vector<std::optional<Mem>> allocas;
	std::vector<std::function<void()>> cold;
	Label exitLabel;
	uint32_t block = 0;
	uint32_t nextBlock = nir::NONE;
	bool inlined = false;
	std::optional<AsmReg> inlineResult;
};

class Lowerer {
public:
	Lowerer(const nir::Module& module, CodeHolder& code, const engine::Options& options,
	        CompilationStatistics* statistics, const AsmJitIntrinsicManager& intrinsics,
	        const nirbackend::HostEnvironment& host)
	    : m_(module), cc(&code), intrinsics_(intrinsics), host_(host), statistics_(statistics) {
		if (options.getOptionOrDefault<bool>("asmjit.enablePostRAPeephole", true)) {
			cc.addPassT<X64PostRAPeepholePass>(statistics);
		}
		branchFusion_ = options.getOptionOrDefault<bool>("asmjit.enableBranchFusion", true);
		constFolding_ = options.getOptionOrDefault<bool>("asmjit.enableConstFolding", true);
		selectCmov_ = options.getOptionOrDefault<bool>("asmjit.enableSelectCmov", true);
		lazyNarrowing_ = options.getOptionOrDefault<bool>("asmjit.enableLazyNarrowing", true);
		addressFusion_ = options.getOptionOrDefault<bool>("asmjit.enableAddressFusion", true);
		inlining_ = options.getOptionOrDefault<bool>("asmjit.enableInlining", true);
		loopRotation_ = options.getOptionOrDefault<bool>("asmjit.enableLoopRotation", true);
		inliningMaxOperations_ =
		    static_cast<size_t>(options.getOptionOrDefault<int>("asmjit.inliningMaxOperations", 24));
	}

	void run(std::string* irDump) {
		if (m_.functions.empty()) {
			throw std::runtime_error("AsmJit: no functions found in the module");
		}
		// Pass 1: every function gets its node, so that calls and function addresses can refer to it in any order.
		nodes_.resize(m_.functions.size());
		for (uint32_t i = 0; i < m_.functions.size(); ++i) {
			const auto& record = m_.functions[i];
			if (record.level != static_cast<uint8_t>(nir::Level::SSA)) {
				throw RuntimeException("asmjit: function #" + std::to_string(i) + " is not an SSA function");
			}
			nodes_[i] = cc.newFunc(signatureOf(m_.signatures[record.signature]));
			nodesByName_[std::string(m_.name(record.nameId))] = nodes_[i];
		}
		const auto loweringStart = std::chrono::steady_clock::now();
		inlinable_.assign(m_.functions.size(), -1);
		// Pass 2: the bodies.
		for (uint32_t i = 0; i < m_.functions.size(); ++i) {
			lowerFunction(i);
		}

		if (statistics_ != nullptr && branchFusion_) {
			statistics_->add("asmjit.lowering.fusedBranches", fusedBranches_);
		}
		if (statistics_ != nullptr && constFolding_) {
			statistics_->add("asmjit.lowering.foldedImmediates", foldedImmediates_);
		}
		if (statistics_ != nullptr && addressFusion_) {
			statistics_->add("asmjit.lowering.fusedAddresses", fusedAddresses_);
		}
		if (statistics_ != nullptr && inlining_) {
			statistics_->add("asmjit.lowering.inlinedCalls", inlinedCalls_);
		}
		if (statistics_ != nullptr && branchFusion_ && loopRotation_) {
			statistics_->add("asmjit.lowering.rotatedLoops", rotatedLoops_);
		}

		// The builder node list, formatted while the virtual registers are still there.
		if (irDump != nullptr) {
			::asmjit::String sb;
			::asmjit::FormatOptions formatOptions;
			::asmjit::Formatter::formatNodeList(sb, formatOptions, &cc);
			irDump->assign(sb.data(), sb.size());
		}
		const auto finalizeStart = std::chrono::steady_clock::now();
		cc.finalize();
		if (statistics_ != nullptr) {
			// the builder nodes (this file), then AsmJit's register allocation, the peephole pass and the encoding
			statistics_->set("asmjit.lowering.ms",
			                 std::chrono::duration<double, std::milli>(finalizeStart - loweringStart).count());
			statistics_->recordTimingMs("asmjit.finalize.ms", finalizeStart);
		}
	}

	const std::unordered_map<std::string, FuncNode*>& functionNodes() const {
		return nodesByName_;
	}

private:
	const nir::Module& m_;
	x86::Compiler cc;
	const AsmJitIntrinsicManager& intrinsics_;
	const nirbackend::HostEnvironment& host_;
	CompilationStatistics* statistics_;

	std::vector<FuncNode*> nodes_;
	std::unordered_map<std::string, FuncNode*> nodesByName_;
	std::vector<int8_t> inlinable_;
	Fn* f_ = nullptr;

	bool branchFusion_ = true, constFolding_ = true, selectCmov_ = true, lazyNarrowing_ = true, addressFusion_ = true,
	     inlining_ = true, loopRotation_ = true;
	size_t inliningMaxOperations_ = 24;
	int64_t fusedBranches_ = 0, foldedImmediates_ = 0, fusedAddresses_ = 0, inlinedCalls_ = 0, rotatedLoops_ = 0;

	Fn& F() {
		return *f_;
	}

	// ---- signatures and types ---------------------------------------------------------------------------------

	FuncSignature signatureOf(const nir::SignatureRecord& sig, bool withTarget = false) const {
		FuncSignature out;
		out.setRet(asmjitType(sig.result));
		if (withTarget) {
			out.addArg(::asmjit::TypeId::kUIntPtr);
		}
		for (uint32_t p = 0; p < sig.paramCount; ++p) {
			out.addArg(asmjitType(m_.paramType(sig, p)));
		}
		return out;
	}

	TypeId typeOfRef(Ref r) const {
		return nir::isConstRef(r) ? static_cast<TypeId>(m_.consts.record(r).type)
		                          : static_cast<TypeId>(f_->insts[r].type);
	}

	// ---- constants and registers ------------------------------------------------------------------------------

	std::optional<uint64_t> constOf(Ref r) const {
		if (nir::isConstRef(r)) {
			return m_.consts.record(r).bits;
		}
		if (nir::isValueRef(r) && (f_->deferred[r] & kConstValue) != 0) {
			return f_->constBits[r];
		}
		return std::nullopt;
	}

	/// The immediate form of constant `r` for an instruction that works on `bits` bits, if it has one: a 64-bit
	/// instruction takes a sign-extended 32-bit immediate, narrower ones the low bits.
	std::optional<int32_t> immFor(Ref r, unsigned bits) {
		if (!constFolding_) {
			return std::nullopt;
		}
		const auto c = constOf(r);
		if (!c.has_value()) {
			return std::nullopt;
		}
		switch (bits) {
		case 8:
			return static_cast<int32_t>(static_cast<int8_t>(*c));
		case 16:
			return static_cast<int32_t>(static_cast<int16_t>(*c));
		case 32:
			return static_cast<int32_t>(static_cast<uint32_t>(*c));
		default: {
			const auto s = static_cast<int64_t>(*c);
			if (s >= INT32_MIN && s <= INT32_MAX) {
				return static_cast<int32_t>(s);
			}
			return std::nullopt;
		}
		}
	}

	Gp gpConst(uint64_t bits) {
		Gp g = cc.newInt64();
		cc.mov(g, Imm(static_cast<int64_t>(bits)));
		return g;
	}

	Mem floatConst(uint64_t bits, TypeId t) {
		if (t == TypeId::F32) {
			return cc.newFloatConst(ConstPoolScope::kLocal, std::bit_cast<float>(static_cast<uint32_t>(bits)));
		}
		return cc.newDoubleConst(ConstPoolScope::kLocal, std::bit_cast<double>(bits));
	}

	Xmm newXmm(TypeId t) {
		return t == TypeId::F32 ? cc.newXmmSs() : cc.newXmmSd();
	}

	/// The register of integer, pointer or bool value `r`; a constant is rematerialised into a fresh register.
	Gp gp(Ref r) {
		if (const auto c = constOf(r)) {
			return gpConst(*c);
		}
		return regOf(r).as<Gp>();
	}

	/// The register of float value `r`; a constant is loaded from the constant pool into a fresh register.
	Xmm xmm(Ref r) {
		const auto t = typeOfRef(r);
		if (const auto c = constOf(r)) {
			Xmm x = newXmm(t);
			if (*c == 0) {
				cc.xorps(x, x);
			} else if (t == TypeId::F32) {
				cc.movss(x, floatConst(*c, t));
			} else {
				cc.movsd(x, floatConst(*c, t));
			}
			return x;
		}
		return regOf(r).as<Xmm>();
	}

	AsmReg reg(Ref r) {
		return isFp(typeOfRef(r)) ? AsmReg(xmm(r)) : AsmReg(gp(r));
	}

	const BaseReg& regOf(Ref r) {
		if (!nir::isValueRef(r) || r >= F().regs.size() || !F().regs[r].isValid()) {
			throw RuntimeException("asmjit: value %" + std::to_string(r) + " is used before it is lowered");
		}
		return F().regs[r];
	}

	Gp defGp(uint32_t id) {
		Gp g = cc.newInt64();
		F().regs[id] = g;
		return g;
	}

	Xmm defXmm(uint32_t id, TypeId t) {
		Xmm x = newXmm(t);
		F().regs[id] = x;
		return x;
	}

	/// A virtual register is created for a value when it is first needed; only a value that is defined and dead within
	/// one block, with a single use, hands its register on to the result of the instruction that uses it.
	bool canReuse(Ref r) const {
		if (!nir::isValueRef(r) || (f_->inlined && f_->insts[r].op == static_cast<uint16_t>(N::BlockArg))) {
			return false;
		}
		return f_->deferred[r] == 0 && f_->useCount[r] == 1 && f_->blockOf[r] == f_->block && f_->regs[r].isValid();
	}

	/// The destination register for value `id`, the register of `candidate` when that dies here.
	Gp destGp(uint32_t id, Ref candidate, bool* reused = nullptr) {
		if (canReuse(candidate) && f_->regs[candidate].isGp()) {
			F().regs[id] = F().regs[candidate];
			if (reused != nullptr) {
				*reused = true;
			}
			return F().regs[id].as<Gp>();
		}
		if (reused != nullptr) {
			*reused = false;
		}
		return defGp(id);
	}

	Xmm destXmm(uint32_t id, Ref candidate, TypeId t, bool* reused = nullptr) {
		if (canReuse(candidate) && f_->regs[candidate].isVec() && typeOfRef(candidate) == t) {
			F().regs[id] = F().regs[candidate];
			if (reused != nullptr) {
				*reused = true;
			}
			return F().regs[id].as<Xmm>();
		}
		if (reused != nullptr) {
			*reused = false;
		}
		return defXmm(id, t);
	}

	void bindResult(uint32_t id, const AsmReg& r) {
		F().regs[id] = std::holds_alternative<Gp>(r) ? BaseReg(std::get<Gp>(r)) : BaseReg(std::get<Xmm>(r));
	}

	/// The register of block argument `k` of block `b`.
	BaseReg argRegOf(uint32_t b, uint32_t k) {
		const auto id = F().blocks[b].first + k;
		if (!F().regs[id].isValid()) {
			const auto t = static_cast<TypeId>(F().insts[id].type);
			F().regs[id] = isFp(t) ? BaseReg(newXmm(t)) : BaseReg(cc.newInt64());
		}
		return F().regs[id];
	}

	// ---- analysis ---------------------------------------------------------------------------------------------

	void prepare(Fn& f, uint32_t function) {
		f.index = function;
		f.insts = m_.functionInsts(function);
		f.blocks = m_.functionBlocks(function);
		const auto n = f.insts.size();
		f.regs.assign(n, BaseReg());
		f.useCount.assign(n, 0);
		f.lastUse.assign(n, 0);
		f.blockOf.assign(n, 0);
		f.addressUses.assign(n, 0);
		f.scaledUses.assign(n, 0);
		f.deferred.assign(n, 0);
		f.constBits.assign(n, 0);
		f.fusedCompare.assign(n, 0);
		f.fusedLoad.assign(n, 0);
		f.soleUser.assign(n, nir::NONE);
		const auto& record = m_.functions[function];
		f.allocas.assign(record.allocaCount, std::nullopt);
		for (uint32_t b = 0; b < f.blocks.size(); ++b) {
			for (uint32_t i = 0; i < f.blocks[b].count; ++i) {
				f.blockOf[f.blocks[b].first + i] = b;
			}
		}
		for (uint32_t id = 0; id < n; ++id) {
			nir::forEachRef(f.insts[id], m_.extra.data(), nir::Level::SSA, [&](const uint32_t& ref) {
				if (nir::isValueRef(ref) && ref < n) {
					f.soleUser[ref] = ++f.useCount[ref] == 1 ? id : nir::NONE;
				}
			});
		}
		// a block without arguments that holds nothing but a jump without arguments is skipped: branches go to the
		// block that it jumps to (`if (c) {...}` without else leaves such a block on the false edge)
		f.resolved.resize(f.blocks.size());
		f.threaded.assign(f.blocks.size(), 0);
		const auto onlyJumps = [&](uint32_t b) {
			const auto& block = f.blocks[b];
			if (b == 0 || block.argCount != 0 || block.count != 1 || (block.flags & nir::BLOCK_LANDING_PAD) != 0) {
				return false;
			}
			const auto& only = f.insts[block.first];
			return static_cast<N>(only.op) == N::Br && m_.extra[only.c] == 0;
		};
		for (uint32_t b = 0; b < f.blocks.size(); ++b) {
			uint32_t target = b;
			for (int steps = 0; steps < 16 && onlyJumps(target); ++steps) {
				target = f.insts[f.blocks[target].first].a;
				if (steps == 15) {
					target = b; // a cycle of empty blocks: leave it alone
				}
			}
			f.resolved[b] = onlyJumps(target) ? b : target;
			f.threaded[b] = f.resolved[b] != b ? 1 : 0;
		}
		// per block, from the end: the operands that are not used again later in the block
		std::vector<uint32_t> seenIn(n, nir::NONE);
		for (uint32_t b = 0; b < f.blocks.size(); ++b) {
			for (uint32_t k = f.blocks[b].count; k-- > 0;) {
				const uint32_t id = f.blocks[b].first + k;
				const auto& inst = f.insts[id];
				if (nir::isValueRef(inst.a) && inst.a < n && seenIn[inst.a] != b) {
					f.lastUse[id] |= 1;
				}
				if (nir::isValueRef(inst.b) && inst.b < n && seenIn[inst.b] != b) {
					f.lastUse[id] |= 2;
				}
				nir::forEachRef(inst, m_.extra.data(), nir::Level::SSA, [&](const uint32_t& ref) {
					if (nir::isValueRef(ref) && ref < n) {
						seenIn[ref] = b;
					}
				});
			}
		}
	}

	/// Counts the uses that fold into addresses; needs `f_` to be `f`, because matching an address reads constants.
	void analyzeAddresses() {
		if (!addressFusion_) {
			return;
		}
		auto& f = F();
		for (uint32_t id = 0; id < f.insts.size(); ++id) {
			const auto& inst = f.insts[id];
			switch (static_cast<N>(inst.op)) {
			case N::Load:
			case N::Store:
				if (nir::isValueRef(inst.a) && f.blockOf[inst.a] == f.blockOf[id]) {
					++f.addressUses[inst.a];
				}
				break;
			case N::Add:
				if (const auto parts = matchAddress(id)) {
					if (parts->scaledInput != nir::NONE && f.blockOf[parts->offset] == f.blockOf[id]) {
						++f.scaledUses[parts->offset];
					}
				}
				break;
			default:
				break;
			}
		}
	}

	std::optional<std::pair<Ref, uint32_t>> scaledOf(Ref candidate) {
		if (!nir::isValueRef(candidate)) {
			return std::nullopt;
		}
		const auto& c = F().insts[candidate];
		if (static_cast<TypeId>(c.type) != TypeId::I64) {
			return std::nullopt;
		}
		if (static_cast<N>(c.op) == N::Mul) {
			for (int side = 0; side < 2; ++side) {
				const Ref x = side == 0 ? c.a : c.b, k = side == 0 ? c.b : c.a;
				const auto value = constOf(k);
				if (!value.has_value() || constOf(x).has_value()) {
					continue;
				}
				if (const auto shift = addressScaleShift(*value)) {
					return std::make_pair(x, *shift);
				}
			}
		} else if (static_cast<N>(c.op) == N::Shl) {
			const auto count = constOf(c.b);
			if (count.has_value() && *count <= 3 && !constOf(c.a).has_value()) {
				return std::make_pair(c.a, static_cast<uint32_t>(*count));
			}
		}
		return std::nullopt;
	}

	/// Structural match of the add `id` as an x86 address; nullopt when it is not a pointer or 64-bit add that fits.
	std::optional<AddressParts> matchAddress(uint32_t id) {
		if (!addressFusion_) {
			return std::nullopt;
		}
		const auto& add = F().insts[id];
		const auto type = static_cast<TypeId>(add.type);
		if (type != TypeId::Ptr && type != TypeId::I64) {
			return std::nullopt;
		}
		Ref base = add.a, offset = add.b;
		if (constOf(base).has_value() && constOf(offset).has_value()) {
			return std::nullopt;
		}
		AddressParts parts;
		if (const auto disp = immFor(offset, 64)) {
			parts.base = base;
			parts.disp = *disp;
			// an integer add of a doubled value and a constant is one `lea [x + x + disp]`
			if (type != TypeId::Ptr) {
				if (const auto scaled = scaledOf(base); scaled.has_value() && scaled->second == 1) {
					parts.base = nir::NONE;
					parts.offset = base;
					parts.scaledInput = scaled->first;
					parts.shift = 1;
				}
			}
			return parts;
		}
		// for an integer add either side may be the scaled one
		if (type != TypeId::Ptr && !scaledOf(offset).has_value() && scaledOf(base).has_value()) {
			std::swap(base, offset);
		}
		parts.base = base;
		parts.offset = offset;
		if (const auto scaled = scaledOf(offset)) {
			parts.scaledInput = scaled->first;
			parts.shift = scaled->second;
		}
		return parts;
	}

	bool isAddressPartDeferred(Ref r) const {
		return nir::isValueRef(r) && (f_->deferred[r] & kAddressPart) != 0;
	}
	bool isScaledIndexDeferred(Ref r) const {
		return nir::isValueRef(r) && (f_->deferred[r] & kScaledIndex) != 0;
	}

	Mem memFromParts(const AddressParts& parts) {
		if (parts.base == nir::NONE) {
			// `x * 2 + disp`: the doubling as base and index when folded, otherwise the doubled value plus the
			// displacement
			if (isScaledIndexDeferred(parts.offset)) {
				auto x = gp(parts.scaledInput);
				return x86::ptr(x, x, 0, parts.disp);
			}
			return x86::ptr(gp(parts.offset), parts.disp);
		}
		auto base = gp(parts.base);
		if (parts.offset == nir::NONE) {
			return x86::ptr(base, parts.disp);
		}
		if (parts.scaledInput != nir::NONE && isScaledIndexDeferred(parts.offset)) {
			return x86::ptr(base, gp(parts.scaledInput), parts.shift, parts.disp);
		}
		return x86::ptr(base, gp(parts.offset), 0, parts.disp);
	}

	/// The memory operand for the address of a load or a store.
	Mem memOperand(Ref address, uint32_t size) {
		Mem mem;
		if (isAddressPartDeferred(address)) {
			mem = memFromParts(*matchAddress(address));
			++fusedAddresses_;
		} else {
			mem = x86::ptr(gp(address));
		}
		mem.setSize(size);
		return mem;
	}

	// ---- functions --------------------------------------------------------------------------------------------

	void lowerFunction(uint32_t function) {
		Fn fn;
		prepare(fn, function);
		f_ = &fn;
		analyzeAddresses();
		const auto& record = m_.functions[function];
		cc.addFunc(nodes_[function]);

		fn.labels.reserve(fn.blocks.size());
		for (size_t b = 0; b < fn.blocks.size(); ++b) {
			fn.labels.push_back(cc.newLabel());
		}

		// the parameters: the arguments of the entry block, each copied out of its ABI register
		for (uint32_t k = 0; k < fn.blocks[0].argCount; ++k) {
			const auto id = fn.blocks[0].first + k;
			const auto t = static_cast<TypeId>(fn.insts[id].type);
			if (isFp(t)) {
				Xmm abi = newXmm(t);
				nodes_[function]->setArg(k, abi);
				Xmm value = defXmm(id, t);
				cc.movaps(value, abi);
			} else {
				Gp abi = cc.newInt64();
				nodes_[function]->setArg(k, abi);
				Gp value = defGp(id);
				cc.mov(value, abi);
			}
		}

		for (uint32_t b = 0; b < fn.blocks.size(); ++b) {
			lowerBlock(b);
		}
		// cold code: failed checks and rare paths, after the body
		for (size_t i = 0; i < fn.cold.size(); ++i) {
			fn.cold[i]();
		}
		if (fn.exitLabel.isValid()) {
			cc.bind(fn.exitLabel);
			emitDefaultReturn(nir::typeOf(m_.signatures[record.signature].result));
		}
		cc.endFunc();
		f_ = nullptr;
	}

	void lowerBlock(uint32_t b) {
		auto& f = F();
		f.block = b;
		if (f.threaded[b] != 0) {
			return;
		}
		f.nextBlock = b + 1;
		while (f.nextBlock < f.blocks.size() && f.threaded[f.nextBlock] != 0) {
			++f.nextBlock;
		}
		if (f.nextBlock >= f.blocks.size()) {
			f.nextBlock = nir::NONE;
		}
		cc.bind(f.labels[b]);
		const auto& block = f.blocks[b];
		for (uint32_t k = block.argCount; k < block.count; ++k) {
			lowerInstruction(block.first + k);
		}
	}

	void emitDefaultReturn(TypeId t) {
		if (t == TypeId::Void) {
			cc.ret();
		} else if (isFp(t)) {
			Xmm x = newXmm(t);
			cc.xorps(x, x);
			cc.ret(x);
		} else {
			Gp g = cc.newInt64();
			cc.xor_(g, g);
			cc.ret(g);
		}
	}

	Label exitLabel() {
		if (!F().exitLabel.isValid()) {
			F().exitLabel = cc.newLabel();
		}
		return F().exitLabel;
	}

	/// Where an exception raised at an instruction with unwind entry `unwind` continues: its landing pad, or the exit.
	Label unwindLabel(uint32_t unwind) {
		return unwind == nir::NO_CLEANUP ? exitLabel() : F().labels.at(unwind);
	}

	// ---- instructions -----------------------------------------------------------------------------------------

	void lowerInstruction(uint32_t id) {
		const nir::Inst& inst = F().insts[id];
		const auto op = static_cast<N>(inst.op);
		switch (op) {
		case N::Add:
		case N::Sub:
		case N::Mul:
			return lowerArithmetic(op, id, inst);
		case N::Div:
		case N::Rem:
			return lowerDivision(op, id, inst);
		case N::FNeg:
			return lowerFNeg(id, inst);
		case N::And:
		case N::Or:
		case N::BAnd:
		case N::BOr:
		case N::BXor:
			return lowerBitwise(op, id, inst);
		case N::Not:
		case N::BNot:
			return lowerNot(op, id, inst);
		case N::Shl:
		case N::Shr:
			return lowerShift(op, id, inst);
		case N::Cmp:
			return lowerCompare(id, inst);
		case N::Select:
			return lowerSelect(id, inst);
		case N::Cast:
			return lowerCast(id, inst);
		case N::Load:
			return lowerLoad(id, inst);
		case N::Store:
			return lowerStore(inst);
		case N::Alloca:
			return lowerAlloca(id, inst);
		case N::Call:
			return lowerCall(id, inst);
		case N::CallIndirect:
			return lowerCallIndirect(id, inst);
		case N::FuncAddr:
			return lowerFuncAddr(id, inst);
		case N::BindAddr: {
			void* address = host_.bindingAddress ? host_.bindingAddress(m_, inst.a) : nullptr;
			if (address == nullptr) {
				throw RuntimeException("asmjit: the host supplies no address for binding #b" + std::to_string(inst.a));
			}
			return deferConstant(id, reinterpret_cast<uint64_t>(address));
		}
		case N::DataAddr: {
			const auto& data = m_.data.at(inst.a);
			return deferConstant(id, reinterpret_cast<uint64_t>(m_.dataBytes.data() + data.byteOffset));
		}
		case N::PtrConst: {
			void* address = inst.a == nir::NONE ? nullptr : nirbackend::resolvePointer(m_, inst.a, host_);
			return deferConstant(id, reinterpret_cast<uint64_t>(address));
		}
		case N::Br:
			return lowerBr(inst);
		case N::CondBr:
			return lowerCondBr(id, inst);
		case N::Ret:
			return lowerRet(inst);
		case N::Resume:
			cc.jmp(exitLabel());
			return;
		default:
			throw NotImplementedException("asmjit: extension operations are lowered with the extension opcode (M4.4)");
		}
	}

	/// A value that is a constant address: nothing is emitted, each consumer takes the address as an immediate or
	/// rematerialises it.
	void deferConstant(uint32_t id, uint64_t bits) {
		F().deferred[id] |= kConstValue;
		F().constBits[id] = bits;
	}

	/// Re-extends the narrow result of an operation that leaves garbage above its width, when lazy narrowing is off.
	void narrowResult(const Gp& g, TypeId t) {
		if (lazyNarrowing_) {
			return;
		}
		switch (bitsOf(t)) {
		case 8:
			cc.movzx(g.r32(), g.r8());
			break;
		case 16:
			cc.movzx(g.r32(), g.r16());
			break;
		case 32:
			cc.mov(g.r32(), g.r32());
			break;
		default:
			break;
		}
	}

	// ---- arithmetic -------------------------------------------------------------------------------------------

	void lowerFloatBinary(N op, uint32_t id, const nir::Inst& inst) {
		const auto t = static_cast<TypeId>(inst.type);
		const bool single = t == TypeId::F32;
		bool reused = false;
		Xmm d = destXmm(id, inst.a, t, &reused);
		if (!reused) {
			cc.movaps(d, xmm(inst.a));
		}
		const auto c = constOf(inst.b);
		// a constant operand is read straight from the constant pool
		const bool fromPool = c.has_value() && *c != 0;
		if (fromPool) {
			const Mem constMem = floatConst(*c, t);
			switch (op) {
			case N::Add:
				single ? cc.addss(d, constMem) : cc.addsd(d, constMem);
				break;
			case N::Sub:
				single ? cc.subss(d, constMem) : cc.subsd(d, constMem);
				break;
			case N::Mul:
				single ? cc.mulss(d, constMem) : cc.mulsd(d, constMem);
				break;
			default:
				single ? cc.divss(d, constMem) : cc.divsd(d, constMem);
				break;
			}
			return;
		}
		const Xmm r = xmm(inst.b);
		switch (op) {
		case N::Add:
			single ? cc.addss(d, r) : cc.addsd(d, r);
			break;
		case N::Sub:
			single ? cc.subss(d, r) : cc.subsd(d, r);
			break;
		case N::Mul:
			single ? cc.mulss(d, r) : cc.mulsd(d, r);
			break;
		default:
			single ? cc.divss(d, r) : cc.divsd(d, r);
			break;
		}
	}

	void lowerArithmetic(N op, uint32_t id, const nir::Inst& inst) {
		const auto t = static_cast<TypeId>(inst.type);
		if (isFp(t)) {
			return lowerFloatBinary(op, id, inst);
		}
		if (op == N::Add && (t == TypeId::Ptr || t == TypeId::I64)) {
			if (const auto parts = matchAddress(id)) {
				// every consumer folds it into its own memory operand: nothing to emit
				if (addressFusion_ && F().useCount[id] > 0 && F().addressUses[id] == F().useCount[id]) {
					F().deferred[id] |= kAddressPart;
					return;
				}
			}
			if (const auto parts = matchAddress(id);
			    parts.has_value() && ((parts->scaledInput != nir::NONE && isScaledIndexDeferred(parts->offset)) ||
			                          parts->base == nir::NONE)) {
				// a scaled index or a doubled value: one three-operand lea computes it; the register of an operand
				// that dies here takes the result. (A plain `a + b` or `a + c` takes the general path below: where an
				// operand dies the register allocator coalesces the two-address form, which a lea would not.)
				Ref dying = nir::NONE;
				for (const Ref candidate :
				     {parts->base, parts->scaledInput != nir::NONE && isScaledIndexDeferred(parts->offset)
				                       ? parts->scaledInput
				                       : parts->offset}) {
					if (dying == nir::NONE && canReuse(candidate)) {
						dying = candidate;
					}
				}
				const Mem mem = memFromParts(*parts);
				Gp d = destGp(id, dying);
				cc.lea(d, mem);
				if (parts->offset == nir::NONE) {
					++foldedImmediates_;
				}
				return;
			}
		}
		if (op == N::Mul || op == N::Shl) {
			// a scaled address index every consumer folds into its address
			if (addressFusion_ && F().useCount[id] > 0 && F().scaledUses[id] == F().useCount[id]) {
				F().deferred[id] |= kScaledIndex;
				return;
			}
		}
		// the operation runs on the whole register, whatever the width: the bits above it are never read, and a
		// 64-bit `add` of a loop counter is measurably faster than the 32-bit one (main's lowering never used it);
		// immediates are those of the type's width
		const unsigned ow = 64, iw = opBitsOf(t);
		Ref a = inst.a, b = inst.b;
		const bool commutative = op != N::Sub;
		if (commutative && !constOf(b).has_value() && constOf(a).has_value()) {
			std::swap(a, b);
		}
		if (op == N::Mul) {
			if (const auto imm = immFor(b, iw)) {
				return lowerMulImmediate(id, a, *imm, ow);
			}
		}
		// an operand that stays alive cannot take the result: `a + b` and `a + c` are one `lea`, which saves the copy
		// that `mov d, a; add d, b` needs (for the sum of two 32-bit values the upper half is garbage anyway)
		// (an operand that dies here stays two-address: the register allocator coalesces `mov d, a; add d, b` into one
		// `add` when a is dead afterwards, as in `s = s + v` of a loop, which a `lea` would not)
		if (op != N::Mul && !canReuse(a) && !constOf(a).has_value() &&
		    ((F().lastUse[id] >> (a == inst.a ? 0 : 1)) & 1) == 0) {
			const auto imm = immFor(b, iw);
			const int64_t disp = imm.has_value() ? (op == N::Sub ? -static_cast<int64_t>(*imm) : *imm) : 0;
			if (disp >= INT32_MIN && disp <= INT32_MAX && (imm.has_value() || op == N::Add)) {
				Gp av = gp(a);
				const Mem mem = imm.has_value() ? x86::ptr(av, static_cast<int32_t>(disp)) : x86::ptr(av, gp(b));
				Gp d = defGp(id);
				cc.lea(d, mem);
				if (imm.has_value()) {
					++foldedImmediates_;
				}
				narrowResult(d, t);
				return;
			}
		}
		bool reused = false;
		Gp d = destGp(id, a, &reused);
		if (!reused) {
			cc.mov(d, gp(a));
		}
		const auto dv = view(d, ow);
		if (const auto imm = immFor(b, iw)) {
			op == N::Add ? cc.add(dv, *imm) : op == N::Sub ? cc.sub(dv, *imm) : cc.imul(dv, dv, *imm);
			++foldedImmediates_;
		} else {
			const auto bv = view(gp(b), ow);
			op == N::Add ? cc.add(dv, bv) : op == N::Sub ? cc.sub(dv, bv) : cc.imul(dv, bv);
		}
		narrowResult(d, t);
	}

	void lowerMulImmediate(uint32_t id, Ref a, int32_t imm, unsigned ow) {
		const auto t = static_cast<TypeId>(F().insts[id].type);
		Gp src = gp(a);
		Gp d = destGp(id, a);
		// strength-reduce the 3-cycle imul: a power of two is a shift, and 3/5/9 are one lea (the low bits agree)
		if (imm > 1 && (imm & (imm - 1)) == 0) {
			cc.mov(d, src);
			cc.shl(view(d, ow), static_cast<uint32_t>(std::countr_zero(static_cast<uint32_t>(imm))));
		} else if (imm == 3 || imm == 5 || imm == 9) {
			cc.lea(view(d, ow), x86::ptr(view(src, 64), view(src, 64),
			                             static_cast<uint32_t>(std::countr_zero(static_cast<uint32_t>(imm - 1)))));
		} else {
			cc.imul(view(d, ow), view(src, ow), imm);
		}
		++foldedImmediates_;
		narrowResult(d, t);
	}

	void lowerFNeg(uint32_t id, const nir::Inst& inst) {
		const auto t = static_cast<TypeId>(inst.type);
		// flip the sign bit with a mask, which is right for zero and NaN too
		bool reused = false;
		Xmm d = destXmm(id, inst.a, t, &reused);
		if (!reused) {
			cc.movaps(d, xmm(inst.a));
		}
		Xmm mask = newXmm(t);
		if (t == TypeId::F32) {
			cc.movss(mask, floatConst(0x80000000u, t));
			cc.xorps(d, mask);
		} else {
			cc.movsd(mask, floatConst(0x8000000000000000ULL, t));
			cc.xorpd(d, mask);
		}
	}

	void lowerBitwise(N op, uint32_t id, const nir::Inst& inst) {
		const auto t = static_cast<TypeId>(inst.type);
		const unsigned ow = 64, iw = opBitsOf(t);
		Ref a = inst.a, b = inst.b;
		if (!constOf(b).has_value() && constOf(a).has_value()) {
			std::swap(a, b);
		}
		bool reused = false;
		Gp d = destGp(id, a, &reused);
		if (!reused) {
			cc.mov(d, gp(a));
		}
		const auto dv = view(d, ow);
		const bool isAnd = op == N::And || op == N::BAnd;
		const bool isOr = op == N::Or || op == N::BOr;
		if (const auto imm = immFor(b, iw)) {
			isAnd ? cc.and_(dv, *imm) : isOr ? cc.or_(dv, *imm) : cc.xor_(dv, *imm);
			++foldedImmediates_;
		} else {
			const auto bv = view(gp(b), ow);
			isAnd ? cc.and_(dv, bv) : isOr ? cc.or_(dv, bv) : cc.xor_(dv, bv);
		}
	}

	void lowerNot(N op, uint32_t id, const nir::Inst& inst) {
		const auto t = static_cast<TypeId>(inst.type);
		const unsigned ow = 64;
		bool reused = false;
		Gp d = destGp(id, inst.a, &reused);
		if (!reused) {
			cc.mov(d, gp(inst.a));
		}
		if (op == N::Not) {
			// logical not of a bool: flips the low bit
			cc.xor_(view(d, 32), 1);
		} else {
			cc.not_(view(d, ow));
			narrowResult(d, t);
		}
	}

	// ---- shifts -----------------------------------------------------------------------------------------------

	void lowerShift(N op, uint32_t id, const nir::Inst& inst) {
		const auto t = static_cast<TypeId>(inst.type);
		const unsigned bits = bitsOf(t);
		const unsigned ow = opBitsOf(t);
		if (op == N::Shl && addressFusion_ && F().useCount[id] > 0 && F().scaledUses[id] == F().useCount[id]) {
			F().deferred[id] |= kScaledIndex;
			return;
		}
		const bool isLeft = op == N::Shl;
		const bool isUnsigned = !isLeft && (inst.flags & 1) != 0;
		// a right shift pulls the bits above the width down, so the value is extended to the width of the shift first
		bool reused = false;
		Gp d = destGp(id, inst.a, &reused);
		if (!isLeft && bits < 32) {
			const auto src = gp(inst.a);
			if (isUnsigned) {
				bits == 8 ? cc.movzx(d.r32(), src.r8()) : cc.movzx(d.r32(), src.r16());
			} else {
				bits == 8 ? cc.movsx(d.r32(), src.r8()) : cc.movsx(d.r32(), src.r16());
			}
		} else if (!reused) {
			cc.mov(d, gp(inst.a));
		}
		const auto dv = view(d, ow);
		if (const auto c = constOf(inst.b)) {
			// the amount is taken modulo the width (D10)
			const auto count = static_cast<uint32_t>(*c & (bits - 1));
			if (count != 0) {
				isLeft ? cc.shl(dv, count) : isUnsigned ? cc.shr(dv, count) : cc.sar(dv, count);
				++foldedImmediates_;
			}
		} else {
			Gp amount = gp(inst.b);
			if (bits < 32) {
				// the hardware masks the count modulo 32 for these registers, not modulo the width
				Gp masked = cc.newInt32();
				cc.mov(masked, view(amount, 32));
				cc.and_(masked, bits - 1);
				amount = masked.r64();
			}
			// x86 masks the count of 32- and 64-bit shifts by itself (mod 32, mod 64), as the width of the value
			isLeft ? cc.shl(dv, amount.r8()) : isUnsigned ? cc.shr(dv, amount.r8()) : cc.sar(dv, amount.r8());
		}
		if (isLeft) {
			narrowResult(d, t);
		}
	}

	// ---- integer division -------------------------------------------------------------------------------------

	void lowerDivision(N op, uint32_t id, const nir::Inst& inst) {
		const auto t = static_cast<TypeId>(inst.type);
		if (isFp(t)) {
			if (op == N::Div) {
				return lowerFloatBinary(N::Div, id, inst);
			}
			// float rem is fmod, a call of a runtime helper
			Xmm d = defXmm(id, t);
			FuncSignature sig;
			sig.setRet(asmjitType(nir::makeTypeWord(t)));
			sig.addArg(asmjitType(nir::makeTypeWord(t)));
			sig.addArg(asmjitType(nir::makeTypeWord(t)));
			Xmm a = xmm(inst.a), b = xmm(inst.b);
			InvokeNode* node = nullptr;
			void* helper = t == TypeId::F32 ? reinterpret_cast<void*>(&nirbackend::nautilus_rt_fmod_f32)
			                                : reinterpret_cast<void*>(&nirbackend::nautilus_rt_fmod_f64);
			cc.invoke(&node, reinterpret_cast<uint64_t>(helper), sig);
			node->setArg(0, a);
			node->setArg(1, b);
			node->setRet(0, d);
			return;
		}
		const bool isUnsigned = (inst.flags & 1) != 0;
		const bool checked = (inst.flags & 2) != 0;
		const unsigned bits = bitsOf(t);
		const unsigned ow = opBitsOf(t);
		const bool isDiv = op == N::Div;
		Gp left = gp(inst.a), right = gp(inst.b);
		if (checked) {
			// the divisor is zero, or the signed minimum is divided by -1: a compare and a branch to a cold block that
			// throws (13 §5.10)
			Label cold = cc.newLabel();
			cc.test(view(right, bits), view(right, bits));
			cc.jz(cold);
			if (!isUnsigned) {
				Label ok = cc.newLabel();
				cc.cmp(view(right, bits), -1);
				cc.jne(ok);
				// a - 1 overflows exactly for the minimum of the type
				cc.cmp(view(left, bits), 1);
				cc.jo(cold);
				cc.bind(ok);
			}
			const auto target = unwindLabel(inst.c);
			F().cold.push_back([this, cold, target]() {
				cc.bind(cold);
				emitThrowArith();
				cc.jmp(target);
			});
		}
		// the division works on 32 or 64 bits: narrower operands are extended first
		Gp quot = cc.newInt64(), rem = cc.newInt64();
		Gp divisor = right;
		if (bits < 32) {
			Gp extended = cc.newInt64();
			if (isUnsigned) {
				bits == 8 ? cc.movzx(quot.r32(), left.r8()) : cc.movzx(quot.r32(), left.r16());
				bits == 8 ? cc.movzx(extended.r32(), right.r8()) : cc.movzx(extended.r32(), right.r16());
			} else {
				bits == 8 ? cc.movsx(quot.r32(), left.r8()) : cc.movsx(quot.r32(), left.r16());
				bits == 8 ? cc.movsx(extended.r32(), right.r8()) : cc.movsx(extended.r32(), right.r16());
			}
			divisor = extended;
		} else {
			cc.mov(quot, left);
		}
		if (isUnsigned) {
			cc.xor_(rem.r32(), rem.r32());
			cc.div(view(rem, ow), view(quot, ow), view(divisor, ow));
		} else if (ow == 32) {
			cc.cdq(rem.r32(), quot.r32());
			cc.idiv(rem.r32(), quot.r32(), divisor.r32());
		} else {
			cc.cqo(rem.r64(), quot.r64());
			cc.idiv(rem.r64(), quot.r64(), divisor.r64());
		}
		F().regs[id] = isDiv ? quot : rem;
		narrowResult(F().regs[id].as<Gp>(), t);
	}

	/// The call of `nautilus_rt_throw_arith` through its capture thunk, which stores the exception for the pad.
	void emitThrowArith() {
		FuncSignature sig;
		sig.setRet(::asmjit::TypeId::kVoid);
		sig.addArg(::asmjit::TypeId::kUIntPtr);
		Gp target = cc.newIntPtr();
		cc.mov(target, reinterpret_cast<uint64_t>(nirbackend::throwArithTarget()));
		InvokeNode* node = nullptr;
		cc.invoke(&node, reinterpret_cast<uint64_t>(nirbackend::throwArithThunk()), sig);
		node->setArg(0, target);
	}

	// ---- compare and select -----------------------------------------------------------------------------------

	static unsigned swapPredicate(unsigned p) {
		switch (p) {
		case 2:
			return 4;
		case 3:
			return 5;
		case 4:
			return 2;
		case 5:
			return 3;
		default:
			return p;
		}
	}

	static CondCode integerCondition(unsigned predicate, bool isUnsigned) {
		switch (predicate) {
		case 0:
			return CondCode::kEqual;
		case 1:
			return CondCode::kNotEqual;
		case 2:
			return isUnsigned ? CondCode::kUnsignedLT : CondCode::kSignedLT;
		case 3:
			return isUnsigned ? CondCode::kUnsignedLE : CondCode::kSignedLE;
		case 4:
			return isUnsigned ? CondCode::kUnsignedGT : CondCode::kSignedGT;
		case 5:
			return isUnsigned ? CondCode::kUnsignedGE : CondCode::kSignedGE;
		default:
			throw RuntimeException("asmjit: an invalid compare predicate");
		}
	}

	/// The flag-setting half of an integer compare at its own width; returns the condition under which it holds.
	CondCode emitIntegerCompare(const nir::Inst& cmp) {
		unsigned predicate = cmp.flags & 7;
		const auto type = typeOfRef(cmp.a);
		const bool isUnsigned = (cmp.flags & 8) != 0 || type == TypeId::Ptr;
		const unsigned bits = bitsOf(type);
		Ref l = cmp.a, r = cmp.b;
		// a constant on the left goes to the right, with the swapped predicate
		if (constOf(l).has_value() && !constOf(r).has_value()) {
			std::swap(l, r);
			predicate = swapPredicate(predicate);
		}
		const auto left = view(gp(l), bits);
		if (const auto c = constOf(r); c.has_value() && constFolding_) {
			const uint64_t mask = bits == 64 ? ~uint64_t {0} : (uint64_t {1} << bits) - 1;
			if ((*c & mask) == 0) {
				// `cmp x, 0` is `test x, x`: the same flags for every condition that is read here
				cc.test(left, left);
				++foldedImmediates_;
				return integerCondition(predicate, isUnsigned);
			}
			if (const auto imm = immFor(r, bits)) {
				cc.cmp(left, *imm);
				++foldedImmediates_;
				return integerCondition(predicate, isUnsigned);
			}
		}
		cc.cmp(left, view(gp(r), bits));
		return integerCondition(predicate, isUnsigned);
	}

	/// Whether the compare `id` can hand its flags to the `condbr` that follows it.
	bool isFusibleCompare(uint32_t id) const {
		const auto& f = *f_;
		const auto& cmp = f.insts[id];
		if (!branchFusion_ || id + 1 >= f.insts.size() || f.blockOf[id + 1] != f.blockOf[id]) {
			return false;
		}
		const auto& next = f.insts[id + 1];
		// float compares need the parity flag on top of the condition and stay on the materialising path
		return static_cast<N>(next.op) == N::CondBr && next.a == id && f.useCount[id] == 1 && !isFp(typeOfRef(cmp.a));
	}

	void lowerCompare(uint32_t id, const nir::Inst& inst) {
		if (isFusibleCompare(id)) {
			F().fusedCompare[id + 1] = 1;
			return;
		}
		const auto type = typeOfRef(inst.a);
		if (!isFp(type)) {
			const auto cond = emitIntegerCompare(inst);
			Gp d = destGp(id, inst.a);
			cc.set(cond, d.r8());
			cc.movzx(d.r32(), d.r8());
			return;
		}
		const unsigned predicate = inst.flags & 7;
		const bool single = type == TypeId::F32;
		Xmm left = xmm(inst.a), right = xmm(inst.b);
		auto ucomi = [&](const Xmm& a, const Xmm& b) {
			single ? cc.ucomiss(a, b) : cc.ucomisd(a, b);
		};
		Gp d = defGp(id);
		const auto d8 = d.r8();
		// ucomis sets ZF, PF and CF for an unordered result. `eq` and `ne` need the parity flag on top of ZF; `lt` and
		// `le` compare with the operands swapped and read "above", which is false when unordered
		switch (predicate) {
		case 0: {
			ucomi(left, right);
			cc.sete(d8);
			Gp parity = cc.newInt64();
			cc.setnp(parity.r8());
			cc.and_(d8, parity.r8());
			break;
		}
		case 1: {
			ucomi(left, right);
			cc.setne(d8);
			Gp parity = cc.newInt64();
			cc.setp(parity.r8());
			cc.or_(d8, parity.r8());
			break;
		}
		case 2:
			ucomi(right, left);
			cc.seta(d8);
			break;
		case 3:
			ucomi(right, left);
			cc.setae(d8);
			break;
		case 4:
			ucomi(left, right);
			cc.seta(d8);
			break;
		case 5:
			ucomi(left, right);
			cc.setae(d8);
			break;
		default:
			throw RuntimeException("asmjit: an invalid compare predicate");
		}
		cc.movzx(d.r32(), d8);
	}

	void lowerSelect(uint32_t id, const nir::Inst& inst) {
		const auto t = static_cast<TypeId>(inst.type);
		Gp cond = gp(inst.a);
		if (selectCmov_ && !isFp(t)) {
			// both values are computed already: a cmov replaces the two-way branch and its misprediction risk
			const unsigned ow = opBitsOf(t);
			Gp falseValue = gp(inst.c);
			bool reused = false;
			Gp d = destGp(id, inst.b, &reused);
			if (!reused) {
				cc.mov(d, gp(inst.b));
			}
			cc.test(cond.r8(), cond.r8());
			cc.cmovz(view(d, ow), view(falseValue, ow));
			return;
		}
		Label falsePath = cc.newLabel(), done = cc.newLabel();
		AsmReg d = isFp(t) ? AsmReg(defXmm(id, t)) : AsmReg(defGp(id));
		cc.test(cond.r8(), cond.r8());
		cc.jz(falsePath);
		emitMove(d, inst.b);
		cc.jmp(done);
		cc.bind(falsePath);
		emitMove(d, inst.c);
		cc.bind(done);
	}

	/// `dst = value of src`: a register move, or the constant loaded directly.
	void emitMove(const AsmReg& dst, Ref src) {
		if (std::holds_alternative<Xmm>(dst)) {
			const auto d = std::get<Xmm>(dst);
			const auto c = constOf(src);
			if (c.has_value()) {
				const auto t = typeOfRef(src);
				if (*c == 0) {
					cc.xorps(d, d);
				} else if (t == TypeId::F32) {
					cc.movss(d, floatConst(*c, t));
				} else {
					cc.movsd(d, floatConst(*c, t));
				}
			} else {
				cc.movaps(d, regOf(src).as<Xmm>());
			}
		} else {
			const auto d = std::get<Gp>(dst);
			if (const auto c = constOf(src)) {
				cc.mov(d, Imm(static_cast<int64_t>(*c)));
			} else {
				cc.mov(d, regOf(src).as<Gp>());
			}
		}
	}

	// ---- casts ------------------------------------------------------------------------------------------------

	void lowerCast(uint32_t id, const nir::Inst& inst) {
		if (F().fusedLoad[id] != 0) {
			return;
		}
		const auto kind = inst.flags & 15;
		const auto from = typeOfRef(inst.a);
		const auto to = static_cast<TypeId>(inst.type);
		switch (kind) {
		case 0: { // sext
			Gp src = gp(inst.a);
			Gp d = destGp(id, inst.a);
			switch (bitsOf(from)) {
			case 8:
				if (from == TypeId::Bool) {
					// 0 or -1
					cc.movzx(d.r32(), src.r8());
					cc.neg(d.r64());
				} else {
					cc.movsx(d.r64(), src.r8());
				}
				break;
			case 16:
				cc.movsx(d.r64(), src.r16());
				break;
			case 32:
				cc.movsxd(d.r64(), src.r32());
				break;
			default:
				cc.mov(d, src);
				break;
			}
			return;
		}
		case 1: { // zext
			Gp src = gp(inst.a);
			Gp d = destGp(id, inst.a);
			switch (bitsOf(from)) {
			case 8:
				cc.movzx(d.r32(), src.r8());
				break;
			case 16:
				cc.movzx(d.r32(), src.r16());
				break;
			case 32:
				cc.mov(d.r32(), src.r32());
				break;
			default:
				cc.mov(d, src);
				break;
			}
			return;
		}
		case 2: { // trunc: the low bits stay where they are
			bool reused = false;
			Gp d = destGp(id, inst.a, &reused);
			if (!reused) {
				cc.mov(d, gp(inst.a));
			}
			narrowResult(d, to);
			return;
		}
		case 3: // sitofp
		case 4: // uitofp
			return lowerIntToFloat(id, inst, kind == 3);
		case 5: // fptosi
		case 6: // fptoui
			return lowerFloatToInt(id, inst, kind == 5);
		case 7: { // fpext
			Xmm src = xmm(inst.a);
			Xmm d = defXmm(id, to);
			cc.cvtss2sd(d, src);
			return;
		}
		case 8: { // fptrunc
			Xmm src = xmm(inst.a);
			Xmm d = defXmm(id, to);
			cc.cvtsd2ss(d, src);
			return;
		}
		case 9: { // bitcast
			if (isFp(to)) {
				Gp src = gp(inst.a);
				Xmm d = defXmm(id, to);
				to == TypeId::F32 ? cc.movd(d, src.r32()) : cc.movq(d, src.r64());
			} else if (isFp(from)) {
				Xmm src = xmm(inst.a);
				Gp d = defGp(id);
				from == TypeId::F32 ? cc.movd(d.r32(), src) : cc.movq(d.r64(), src);
			} else {
				bool reused = false;
				Gp d = destGp(id, inst.a, &reused);
				if (!reused) {
					cc.mov(d, gp(inst.a));
				}
			}
			return;
		}
		case 10:   // ptrtoint
		case 11: { // inttoptr
			bool reused = false;
			Gp d = destGp(id, inst.a, &reused);
			if (!reused) {
				cc.mov(d, gp(inst.a));
			}
			return;
		}
		default:
			throw RuntimeException("asmjit: an invalid cast kind");
		}
	}

	void lowerIntToFloat(uint32_t id, const nir::Inst& inst, bool isSigned) {
		const auto from = typeOfRef(inst.a);
		const auto to = static_cast<TypeId>(inst.type);
		const bool single = to == TypeId::F32;
		Gp src = gp(inst.a);
		Gp wide = src;
		Xmm d = defXmm(id, to);
		const unsigned bits = bitsOf(from);
		// the conversion reads 32 or 64 bits: narrower values are extended, an unsigned 32-bit one to 64
		if (bits < 32 || (bits == 32 && !isSigned)) {
			wide = cc.newInt64();
			if (bits == 8) {
				isSigned ? cc.movsx(wide.r32(), src.r8()) : cc.movzx(wide.r32(), src.r8());
			} else if (bits == 16) {
				isSigned ? cc.movsx(wide.r32(), src.r16()) : cc.movzx(wide.r32(), src.r16());
			} else {
				cc.mov(wide.r32(), src.r32());
			}
		}
		if (bits < 32) {
			single ? cc.cvtsi2ss(d, wide.r32()) : cc.cvtsi2sd(d, wide.r32());
			return;
		}
		if (bits == 32) {
			if (isSigned) {
				single ? cc.cvtsi2ss(d, wide.r32()) : cc.cvtsi2sd(d, wide.r32());
			} else {
				single ? cc.cvtsi2ss(d, wide.r64()) : cc.cvtsi2sd(d, wide.r64());
			}
			return;
		}
		if (isSigned) {
			single ? cc.cvtsi2ss(d, src.r64()) : cc.cvtsi2sd(d, src.r64());
			return;
		}
		// unsigned 64-bit: cvtsi2s* reads the source as signed, so values from 2^63 on halve first, keeping the low
		// bit, and double after
		Label done = cc.newLabel(), big = cc.newLabel();
		Gp tmp = cc.newInt64(), lowBit = cc.newInt64();
		cc.test(src, src);
		cc.js(big);
		single ? cc.cvtsi2ss(d, src.r64()) : cc.cvtsi2sd(d, src.r64());
		cc.jmp(done);
		cc.bind(big);
		cc.mov(tmp, src);
		cc.mov(lowBit, src);
		cc.and_(lowBit, 1);
		cc.shr(tmp, 1);
		cc.or_(tmp, lowBit);
		if (single) {
			cc.cvtsi2ss(d, tmp.r64());
			cc.addss(d, d);
		} else {
			cc.cvtsi2sd(d, tmp.r64());
			cc.addsd(d, d);
		}
		cc.bind(done);
	}

	void ucomis(bool single, const Xmm& a, const Xmm& b) {
		single ? cc.ucomiss(a, b) : cc.ucomisd(a, b);
	}

	/// Saturating float to integer conversion (D11): the truncating `cvtt` plus a fixup. The instruction yields the
	/// "integer indefinite" (the minimum) for NaN and every value out of range; a compare with 1 overflows exactly for
	/// the minimum, and only then does a cold block tell NaN (0), too large (maximum) and too small (the minimum
	/// that is already there) apart.
	void lowerFloatToInt(uint32_t id, const nir::Inst& inst, bool isSigned) {
		const auto from = typeOfRef(inst.a);
		const auto to = static_cast<TypeId>(inst.type);
		const bool single = from == TypeId::F32;
		const unsigned bits = bitsOf(to);
		Xmm x = xmm(inst.a);
		Gp d = defGp(id);
		auto cvtt = [&](const Gp& g) {
			single ? cc.cvttss2si(g, x) : cc.cvttsd2si(g, x);
		};
		// the conversion is carried out in 64 bits for every unsigned result and for 64-bit ones, else in 32
		const bool wide = !isSigned || bits == 64;
		const unsigned convBits = wide ? 64 : 32;
		const Gp dc = view(d, convBits);

		if (!isSigned && bits == 64) {
			// below 2^63 the signed conversion is exact; from there subtract 2^63 in the float domain (exact),
			// convert and set bit 63 back
			Label done = cc.newLabel(), big = cc.newLabel(), negative = cc.newLabel(), max = cc.newLabel();
			const Mem threshold = single ? cc.newFloatConst(ConstPoolScope::kLocal, 9223372036854775808.0f)
			                             : cc.newDoubleConst(ConstPoolScope::kLocal, 9223372036854775808.0);
			single ? cc.ucomiss(x, threshold) : cc.ucomisd(x, threshold);
			cc.jae(big);
			cvtt(d);
			cc.test(d, d);
			cc.js(negative);
			cc.bind(done);
			F().cold.push_back([=, this]() {
				// NaN and everything below -1
				cc.bind(negative);
				cc.xor_(d.r32(), d.r32());
				cc.jmp(done);
				cc.bind(big);
				Xmm adjusted = single ? cc.newXmmSs() : cc.newXmmSd();
				single ? cc.movss(adjusted, x) : cc.movsd(adjusted, x);
				single ? cc.subss(adjusted, threshold) : cc.subsd(adjusted, threshold);
				single ? cc.cvttss2si(d, adjusted) : cc.cvttsd2si(d, adjusted);
				cc.test(d, d);
				cc.js(max);
				cc.btc(d, 63);
				cc.jmp(done);
				// 2^64 and more
				cc.bind(max);
				cc.mov(d, Imm(-1));
				cc.jmp(done);
			});
			return;
		}

		cvtt(dc);
		Label cold = cc.newLabel(), back = cc.newLabel();
		cc.cmp(dc, 1);
		cc.jo(cold);
		cc.bind(back);
		F().cold.push_back([=, this]() {
			cc.bind(cold);
			Label nan = cc.newLabel(), positive = cc.newLabel();
			ucomis(single, x, x);
			cc.jp(nan);
			Xmm zero = single ? cc.newXmmSs() : cc.newXmmSd();
			cc.xorps(zero, zero);
			ucomis(single, x, zero);
			cc.ja(positive);
			cc.jmp(back); // below the range: the minimum is there already
			cc.bind(positive);
			convBits == 64 ? cc.mov(d, Imm(INT64_MAX)) : cc.mov(d.r32(), Imm(INT32_MAX));
			cc.jmp(back);
			cc.bind(nan);
			cc.xor_(d.r32(), d.r32());
			cc.jmp(back);
		});

		if (bits >= 32 && (isSigned || bits == 64)) {
			return;
		}
		// the range of a narrower result: clamp with conditional moves
		Gp limit = cc.newInt64();
		if (isSigned) {
			const int32_t hi = bits == 8 ? 127 : 32767, lo = bits == 8 ? -128 : -32768;
			cc.mov(limit.r32(), hi);
			cc.cmp(d.r32(), hi);
			cc.cmovg(d.r32(), limit.r32());
			cc.mov(limit.r32(), lo);
			cc.cmp(d.r32(), lo);
			cc.cmovl(d.r32(), limit.r32());
		} else {
			// negative values (and NaN, already 0) clamp to 0, large ones to the maximum of the width
			Gp zero = cc.newInt64();
			cc.xor_(zero.r32(), zero.r32());
			cc.test(d, d);
			cc.cmovs(d, zero);
			const uint64_t maxValue = bits == 8 ? 0xFF : bits == 16 ? 0xFFFF : 0xFFFFFFFFu;
			cc.mov(limit.r32(), static_cast<uint32_t>(maxValue));
			cc.cmp(d, limit);
			cc.cmova(d, limit);
		}
	}

	// ---- memory -----------------------------------------------------------------------------------------------

	void lowerLoad(uint32_t id, const nir::Inst& inst) {
		const auto t = static_cast<TypeId>(inst.type);
		if (isFp(t)) {
			const auto mem = memOperand(inst.a, t == TypeId::F32 ? 4 : 8);
			Xmm d = defXmm(id, t);
			t == TypeId::F32 ? cc.movss(d, mem) : cc.movsd(d, mem);
			return;
		}
		const unsigned bits = bitsOf(t);
		const auto mem = memOperand(inst.a, bits / 8);
		Gp d = canReuse(inst.a) && F().regs[inst.a].isGp() && !isAddressPartDeferred(inst.a) ? destGp(id, inst.a)
		                                                                                     : defGp(id);
		// the load and the widening of its value are one movsx or movzx from memory (the extension is the load's only
		// use, in the same block, so its value is the same wherever it is computed)
		if (bits < 64 && F().soleUser[id] != nir::NONE && F().blockOf[F().soleUser[id]] == F().blockOf[id] &&
		    t != TypeId::Bool) {
			const uint32_t castId = F().soleUser[id];
			const auto& cast = F().insts[castId];
			const auto kind = cast.flags & 15;
			if (static_cast<N>(cast.op) == N::Cast && cast.a == id && (kind == 0 || kind == 1) &&
			    bitsOf(static_cast<TypeId>(cast.type)) > bits) {
				F().regs[castId] = d;
				F().fusedLoad[castId] = 1;
				if (kind == 0) {
					bits == 32 ? cc.movsxd(d.r64(), mem) : cc.movsx(d.r64(), mem);
				} else {
					bits == 32 ? cc.mov(d.r32(), mem) : cc.movzx(d.r32(), mem);
				}
				return;
			}
		}
		switch (bits) {
		case 8:
			cc.movzx(d.r32(), mem);
			break;
		case 16:
			cc.movzx(d.r32(), mem);
			break;
		case 32:
			cc.mov(d.r32(), mem);
			break;
		default:
			cc.mov(d.r64(), mem);
			break;
		}
	}

	void lowerStore(const nir::Inst& inst) {
		const auto t = typeOfRef(inst.b);
		if (isFp(t)) {
			Xmm v = xmm(inst.b);
			const auto mem = memOperand(inst.a, t == TypeId::F32 ? 4 : 8);
			t == TypeId::F32 ? cc.movss(mem, v) : cc.movsd(mem, v);
			return;
		}
		const unsigned bits = bitsOf(t);
		if (const auto imm = immFor(inst.b, bits)) {
			const auto mem = memOperand(inst.a, bits / 8);
			cc.mov(mem, *imm);
			++foldedImmediates_;
			return;
		}
		Gp v = gp(inst.b);
		const auto mem = memOperand(inst.a, bits / 8);
		cc.mov(mem, view(v, bits));
	}

	void lowerAlloca(uint32_t id, const nir::Inst& inst) {
		auto& slot = F().allocas.at(inst.a);
		if (!slot.has_value()) {
			const auto& record = m_.functionAllocas(F().index)[inst.a];
			slot = cc.newStack(record.size, std::max<uint32_t>(record.align, 1));
		}
		Gp d = defGp(id);
		cc.lea(d, *slot);
	}

	// ---- control flow -----------------------------------------------------------------------------------------

	struct Copy {
		BaseReg dst;
		Ref src;
	};

	/// Copies the arguments of an edge into the argument registers of `target`: a parallel copy, since an argument may
	/// be an argument of the target itself.
	void emitEdge(uint32_t target, std::span<const uint32_t> args) {
		std::vector<Copy> copies;
		copies.reserve(args.size());
		for (size_t i = 0; i < args.size(); ++i) {
			copies.push_back({argRegOf(target, static_cast<uint32_t>(i)), args[i]});
		}
		emitParallelCopy(std::move(copies));
	}

	void emitMoveRegs(const BaseReg& dst, const BaseReg& src) {
		if (dst.isGp()) {
			cc.mov(dst.as<Gp>(), src.as<Gp>());
		} else {
			cc.movaps(dst.as<Xmm>(), src.as<Xmm>());
		}
	}

	void emitParallelCopy(std::vector<Copy> copies) {
		struct Move {
			BaseReg dst;
			BaseReg src;
		};
		std::vector<Move> moves;
		std::vector<Copy> constants;
		for (auto& copy : copies) {
			if (constOf(copy.src).has_value()) {
				constants.push_back(copy);
			} else {
				const auto& src = regOf(copy.src);
				if (src.id() != copy.dst.id()) {
					moves.push_back({copy.dst, src});
				}
			}
		}
		std::vector<char> done(moves.size(), 0);
		size_t remaining = moves.size();
		auto pendingReads = [&](uint32_t regId) {
			int n = 0;
			for (size_t i = 0; i < moves.size(); ++i) {
				if (!done[i] && moves[i].src.id() == regId) {
					++n;
				}
			}
			return n;
		};
		while (remaining > 0) {
			bool progressed = false;
			for (size_t i = 0; i < moves.size(); ++i) {
				if (done[i] || pendingReads(moves[i].dst.id()) != 0) {
					continue;
				}
				emitMoveRegs(moves[i].dst, moves[i].src);
				done[i] = 1;
				--remaining;
				progressed = true;
			}
			if (progressed) {
				continue;
			}
			// a cycle: stash one destination in a temporary, the readers of it read the temporary
			for (size_t i = 0; i < moves.size(); ++i) {
				if (done[i]) {
					continue;
				}
				const BaseReg dst = moves[i].dst;
				BaseReg temp = dst.isGp() ? BaseReg(cc.newInt64()) : BaseReg(cc.newXmmSd());
				emitMoveRegs(temp, dst);
				for (size_t j = 0; j < moves.size(); ++j) {
					if (!done[j] && moves[j].src.id() == dst.id()) {
						moves[j].src = temp;
					}
				}
				break;
			}
		}
		// constants last: nothing reads a destination any more
		for (const auto& copy : constants) {
			const auto bits = *constOf(copy.src);
			if (copy.dst.isGp()) {
				cc.mov(copy.dst.as<Gp>(), Imm(static_cast<int64_t>(bits)));
				continue;
			}
			const auto t = typeOfRef(copy.src);
			const Xmm x = copy.dst.as<Xmm>();
			if (bits == 0) {
				cc.xorps(x, x);
			} else if (t == TypeId::F32) {
				cc.movss(x, floatConst(bits, t));
			} else {
				cc.movsd(x, floatConst(bits, t));
			}
		}
	}

	void lowerBr(const nir::Inst& inst) {
		auto& f = F();
		const uint32_t* list = m_.extra.data() + inst.c;
		const uint32_t target = f.resolved[inst.a];
		emitEdge(target, std::span<const uint32_t>(list + 1, list[0]));
		if (tryRotateLoop(target)) {
			return;
		}
		if (target != f.nextBlock) {
			cc.jmp(f.labels[target]);
		}
	}

	/// A loop's back edge normally jumps to the header, which tests the exit condition and branches into the body: two
	/// jumps per iteration, one of them taken. When the header holds nothing but that test (a compare feeding its
	/// `condbr`, no arguments on either edge), the test is repeated at the end of the back edge instead, jumping
	/// straight into the body (taken) or out of the loop. The header still handles the first entry. The compare reads
	/// the header's argument registers, which the edge's copies have just written.
	bool tryRotateLoop(uint32_t header) {
		auto& f = F();
		if (!branchFusion_ || !loopRotation_ || header >= f.block || f.inlined) {
			return false;
		}
		const auto& block = f.blocks[header];
		if (block.count != block.argCount + 2u) {
			return false;
		}
		const uint32_t cmpId = block.first + block.argCount, brId = cmpId + 1;
		const auto& cmp = f.insts[cmpId];
		const auto& br = f.insts[brId];
		if (static_cast<N>(cmp.op) != N::Cmp || static_cast<N>(br.op) != N::CondBr || br.a != cmpId ||
		    f.useCount[cmpId] != 1 || isFp(typeOfRef(cmp.a))) {
			return false;
		}
		const uint32_t* list = m_.extra.data() + br.c;
		const uint32_t trueBlock = f.resolved[br.b], trueArgc = list[1];
		if (trueArgc != 0) {
			return false;
		}
		const uint32_t falseBlock = f.resolved[list[2]], falseArgc = list[3];
		if (falseArgc != 0) {
			return false;
		}
		const auto cond = emitIntegerCompare(cmp);
		cc.j(cond, f.labels[trueBlock]);
		cc.jmp(f.labels[falseBlock]);
		++rotatedLoops_;
		return true;
	}

	void lowerCondBr(uint32_t id, const nir::Inst& inst) {
		auto& f = F();
		const uint32_t* list = m_.extra.data() + inst.c;
		const uint32_t trueBlock = f.resolved[inst.b];
		const uint32_t trueArgc = list[1];
		const std::span<const uint32_t> trueArgs(list + 2, trueArgc);
		const uint32_t falseBlock = f.resolved[list[2 + trueArgc]];
		const uint32_t falseArgc = list[3 + trueArgc];
		const std::span<const uint32_t> falseArgs(list + 4 + trueArgc, falseArgc);

		if (const auto c = constOf(inst.a)) {
			// a constant condition is an unconditional edge
			const bool value = (*c & 1) != 0;
			emitEdge(value ? trueBlock : falseBlock, value ? trueArgs : falseArgs);
			const auto target = value ? trueBlock : falseBlock;
			if (target != f.nextBlock) {
				cc.jmp(f.labels[target]);
			}
			return;
		}
		CondCode cond;
		if (f.fusedCompare[id] != 0) {
			cond = emitIntegerCompare(f.insts[inst.a]);
			++fusedBranches_;
		} else {
			Gp c = gp(inst.a);
			cc.test(c.r8(), c.r8());
			cond = CondCode::kNotZero;
		}

		if (trueArgc == 0 && falseArgc == 0) {
			if (trueBlock == f.nextBlock) {
				cc.j(negateCond(cond), f.labels[falseBlock]);
			} else {
				cc.j(cond, f.labels[trueBlock]);
				if (falseBlock != f.nextBlock) {
					cc.jmp(f.labels[falseBlock]);
				}
			}
			return;
		}
		// edges with arguments: the copies sit after the conditional jump, so they cannot disturb the flags
		Label elsePath = cc.newLabel();
		cc.j(negateCond(cond), elsePath);
		emitEdge(trueBlock, trueArgs);
		cc.jmp(f.labels[trueBlock]);
		cc.bind(elsePath);
		emitEdge(falseBlock, falseArgs);
		if (falseBlock != f.nextBlock) {
			cc.jmp(f.labels[falseBlock]);
		}
	}

	void lowerRet(const nir::Inst& inst) {
		auto& f = F();
		if (f.inlined) {
			if (inst.a != nir::NONE) {
				const auto t = typeOfRef(inst.a);
				if (isFp(t)) {
					Xmm r = newXmm(t);
					emitMove(AsmReg(r), inst.a);
					f.inlineResult = AsmReg(r);
				} else {
					Gp r = cc.newInt64();
					emitMove(AsmReg(r), inst.a);
					f.inlineResult = AsmReg(r);
				}
			}
			return;
		}
		if (inst.a == nir::NONE) {
			cc.ret();
			return;
		}
		const auto result = m_.signatures[m_.functions[f.index].signature].result;
		const auto t = nir::typeOf(result);
		if (isFp(t)) {
			cc.ret(xmm(inst.a));
			return;
		}
		cc.ret(extendedForAbi(inst.a, result));
	}

	/// The register of integer value `ref` extended to the full register as the extension kind of `word` says, for
	/// a value that crosses a native boundary (13 §4.5.1); the other types keep their register.
	/// The bit pattern of constant `bits` of the type `word` as it crosses a native boundary.
	static uint64_t abiBits(uint64_t bits, nir::TypeWord word) {
		const auto t = nir::typeOf(word);
		if (nir::extKindOf(word) == nir::ExtKind::Sext && bitsOf(t) < 64) {
			const unsigned w = bitsOf(t);
			bits = static_cast<uint64_t>(static_cast<int64_t>(bits << (64 - w)) >> (64 - w));
		}
		return bits;
	}

	Gp extendedForAbi(Ref ref, nir::TypeWord word) {
		const auto t = nir::typeOf(word);
		const auto kind = nir::extKindOf(word);
		if (const auto c = constOf(ref)) {
			return gpConst(abiBits(*c, word));
		}
		Gp g = gp(ref);
		if (bitsOf(t) == 64 || (t == TypeId::I32 && kind == nir::ExtKind::None) || nir::isFloat(t)) {
			return g;
		}
		Gp e = cc.newInt64();
		const bool sign = kind == nir::ExtKind::Sext;
		switch (bitsOf(t)) {
		case 8:
			sign ? cc.movsx(e.r64(), g.r8()) : cc.movzx(e.r32(), g.r8());
			break;
		case 16:
			sign ? cc.movsx(e.r64(), g.r16()) : cc.movzx(e.r32(), g.r16());
			break;
		default:
			sign ? cc.movsxd(e.r64(), g.r32()) : cc.mov(e.r32(), g.r32());
			break;
		}
		return e;
	}

	// ---- calls ------------------------------------------------------------------------------------------------

	struct CallRecord {
		std::span<const uint32_t> args;
		uint32_t unwind;
		uint32_t thunk;
	};

	CallRecord callRecord(const uint32_t* list) const {
		const uint32_t argc = list[0];
		return {std::span<const uint32_t>(list + 1, argc), list[1 + argc], list[2 + argc]};
	}

	/// After a call that may throw: continue at the pad, or leave the function, if an exception is pending.
	void emitCheckPending(uint32_t unwind) {
		if (unwind == nir::NO_UNWIND) {
			return;
		}
		const Label target = unwindLabel(unwind);
		FuncSignature frameSig;
		frameSig.setRet(::asmjit::TypeId::kUIntPtr);
		InvokeNode* frameInvoke = nullptr;
		cc.invoke(&frameInvoke, reinterpret_cast<uint64_t>(&currentExceptionFrame), frameSig);
		Gp frame = cc.newIntPtr();
		frameInvoke->setRet(0, frame);
		// the frame is never null inside a function that the engine invoked: its wrapper pushes one
		Gp pending = cc.newIntPtr();
		cc.mov(pending, x86::qword_ptr(frame, offsetof(ExceptionFrame, pending)));
		cc.test(pending, pending);
		cc.jne(target);
	}

	/// An argument of a call. (A constant could be an immediate of the invoke node, but that only moves the move into
	/// the register allocator: callgrind shows the same total.)
	struct CallArg {
		std::optional<AsmReg> reg;
	};

	/// The arguments of a call, the integers extended by the extension kinds of the signature when the callee is
	/// native.
	std::vector<CallArg> callArguments(const nir::SignatureRecord& sig, std::span<const uint32_t> args, bool native) {
		std::vector<CallArg> out;
		out.reserve(args.size());
		for (size_t i = 0; i < args.size(); ++i) {
			const auto word = m_.paramType(sig, static_cast<uint32_t>(i));
			CallArg arg;
			if (nir::isFloat(nir::typeOf(word))) {
				arg.reg = xmm(args[i]);
			} else {
				arg.reg = native ? extendedForAbi(args[i], word) : gp(args[i]);
			}
			out.push_back(std::move(arg));
		}
		return out;
	}

	/// The argument registers of an intrinsic call (a handler reads registers): constants are materialised.
	std::vector<AsmReg> argumentRegisters(const nir::SignatureRecord& sig, std::span<const uint32_t> args) {
		std::vector<AsmReg> regs;
		regs.reserve(args.size());
		for (size_t i = 0; i < args.size(); ++i) {
			const auto word = m_.paramType(sig, static_cast<uint32_t>(i));
			if (nir::isFloat(nir::typeOf(word))) {
				regs.emplace_back(xmm(args[i]));
			} else {
				regs.emplace_back(extendedForAbi(args[i], word));
			}
		}
		return regs;
	}

	void setInvokeArguments(InvokeNode* node, size_t first, const std::vector<CallArg>& args) {
		for (size_t i = 0; i < args.size(); ++i) {
			if (std::holds_alternative<Xmm>(*args[i].reg)) {
				node->setArg(first + i, std::get<Xmm>(*args[i].reg));
			} else {
				node->setArg(first + i, std::get<Gp>(*args[i].reg));
			}
		}
	}

	void bindCallResult(InvokeNode* node, uint32_t id, TypeId type) {
		if (type == TypeId::Void) {
			return;
		}
		if (isFp(type)) {
			Xmm r = defXmm(id, type);
			node->setRet(0, r);
		} else {
			Gp r = defGp(id);
			node->setRet(0, r);
		}
	}

	void lowerCall(uint32_t id, const nir::Inst& inst) {
		const auto& callee = m_.callees.at(inst.a);
		const auto& sig = m_.signatures[callee.signature];
		const auto record = callRecord(m_.extra.data() + inst.c);
		const auto type = nir::typeOf(sig.result);
		switch (static_cast<CalleeKind>(callee.kind)) {
		case CalleeKind::Internal: {
			if (tryInline(callee.target, id, inst, record)) {
				return;
			}
			const auto args = callArguments(sig, record.args, false);
			InvokeNode* node = nullptr;
			cc.invoke(&node, nodes_.at(callee.target)->label(), signatureOf(sig));
			setInvokeArguments(node, 0, args);
			bindCallResult(node, id, type);
			emitCheckPending(record.unwind);
			return;
		}
		case CalleeKind::External: {
			void* target = nirbackend::resolvePointer(m_, callee.target, host_);
			void* thunk = nullptr;
			if (record.unwind != nir::NO_UNWIND) {
				if (record.thunk == nir::NONE) {
					throw RuntimeException("asmjit: a call that may throw names no capture thunk");
				}
				thunk = nirbackend::resolvePointer(m_, record.thunk, host_);
			}
			return emitNativeCall(sig, target, thunk, record, id, type);
		}
		case CalleeKind::Intrinsic: {
			const auto* info = ext::ExtensionRegistry::instance().intrinsic(callee.target);
			if (info == nullptr) {
				throw RuntimeException("asmjit: an intrinsic callee that is not registered");
			}
			if (const auto handler =
			        intrinsics_.getIntrinsic(ir::IntrinsicRegistry::instance().lookup(info->fallback))) {
				const auto regs = argumentRegisters(sig, record.args);
				IntrinsicCallContext ctx {cc, regs, std::nullopt};
				if ((*handler)(ctx)) {
					if (ctx.result.has_value()) {
						bindResult(id, *ctx.result);
					}
					return;
				}
			}
			// no lowering: call the fallback, through the intrinsic's capture thunk if it may throw
			void* thunk = record.unwind != nir::NO_UNWIND ? info->captureThunk : nullptr;
			if (record.unwind != nir::NO_UNWIND && thunk == nullptr) {
				throw RuntimeException("asmjit: an intrinsic that may throw has no capture thunk");
			}
			return emitNativeCall(sig, info->fallback, thunk, record, id, type);
		}
		case CalleeKind::Imported:
			throw NotImplementedException("asmjit: imported callees come with modules and linking (M4.6)");
		}
	}

	/// A call of a native function; one that may throw goes through a capture thunk, a real C++ frame, so that the
	/// exception is caught before it crosses the JIT frame, with the target as the thunk's first argument.
	void emitNativeCall(const nir::SignatureRecord& sig, void* target, void* thunk, const CallRecord& record,
	                    uint32_t id, TypeId type) {
		const auto args = callArguments(sig, record.args, true);
		InvokeNode* node = nullptr;
		if (thunk != nullptr) {
			Gp targetReg = cc.newIntPtr();
			cc.mov(targetReg, reinterpret_cast<uint64_t>(target));
			cc.invoke(&node, reinterpret_cast<uint64_t>(thunk), signatureOf(sig, true));
			node->setArg(0, targetReg);
			setInvokeArguments(node, 1, args);
		} else {
			cc.invoke(&node, reinterpret_cast<uint64_t>(target), signatureOf(sig));
			setInvokeArguments(node, 0, args);
		}
		bindCallResult(node, id, type);
		emitCheckPending(record.unwind);
	}

	void lowerCallIndirect(uint32_t id, const nir::Inst& inst) {
		const auto& sig = m_.signatures[inst.b];
		const auto record = callRecord(m_.extra.data() + inst.c);
		const auto type = nir::typeOf(sig.result);
		Gp target = gp(inst.a);
		const auto args = callArguments(sig, record.args, true);
		InvokeNode* node = nullptr;
		if (record.unwind != nir::NO_UNWIND) {
			if (record.thunk == nir::NONE) {
				throw RuntimeException("asmjit: an indirect call that may throw names no capture thunk");
			}
			void* thunk = nirbackend::resolvePointer(m_, record.thunk, host_);
			cc.invoke(&node, reinterpret_cast<uint64_t>(thunk), signatureOf(sig, true));
			node->setArg(0, target);
			setInvokeArguments(node, 1, args);
		} else {
			cc.invoke(&node, target, signatureOf(sig));
			setInvokeArguments(node, 0, args);
		}
		bindCallResult(node, id, type);
		emitCheckPending(record.unwind);
	}

	void lowerFuncAddr(uint32_t id, const nir::Inst& inst) {
		const auto& callee = m_.callees.at(inst.a);
		switch (static_cast<CalleeKind>(callee.kind)) {
		case CalleeKind::Internal: {
			// the JIT function's own address, through a RIP-relative lea resolved when the code is finalized
			Gp d = defGp(id);
			cc.lea(d, x86::ptr(nodes_.at(callee.target)->label()));
			return;
		}
		case CalleeKind::External:
			return deferConstant(id, reinterpret_cast<uint64_t>(nirbackend::resolvePointer(m_, callee.target, host_)));
		case CalleeKind::Imported:
			throw NotImplementedException("asmjit: imported callees come with modules and linking (M4.6)");
		case CalleeKind::Intrinsic:
			throw RuntimeException("asmjit: an intrinsic has no address");
		}
	}

	// ---- inlining ---------------------------------------------------------------------------------------------
	// A small leaf callee costs far more in call overhead (argument shuffling, the call/ret pair, the caller's values
	// forced into callee-saved registers) than its body, so a body of one block of plain value operations is lowered
	// in place. The callee is still compiled for its other callers and as an export.

	bool isInlinable(uint32_t callee) {
		if (callee == F().index || F().inlined) {
			return false;
		}
		auto& cached = inlinable_[callee];
		if (cached >= 0) {
			return cached != 0;
		}
		cached = 0;
		const auto& record = m_.functions[callee];
		if (record.blockCount != 1 || record.allocaCount != 0) {
			return false;
		}
		const auto insts = m_.functionInsts(callee);
		const auto& block = m_.functionBlocks(callee)[0];
		if (insts.size() <= block.argCount || insts.size() - block.argCount > inliningMaxOperations_ ||
		    static_cast<N>(insts.back().op) != N::Ret) {
			return false;
		}
		for (size_t i = block.argCount; i + 1 < insts.size(); ++i) {
			const auto& inst = insts[i];
			switch (static_cast<N>(inst.op)) {
			case N::Div:
			case N::Rem:
				// a checked division has a cold path with an unwind target of its own
				if ((inst.flags & 2) != 0) {
					return false;
				}
				break;
			case N::Add:
			case N::Sub:
			case N::Mul:
			case N::FNeg:
			case N::And:
			case N::Or:
			case N::Not:
			case N::BAnd:
			case N::BOr:
			case N::BXor:
			case N::BNot:
			case N::Shl:
			case N::Shr:
			case N::Cmp:
			case N::Select:
			case N::Cast:
			case N::Load:
			case N::Store:
			case N::DataAddr:
			case N::PtrConst:
				break;
			default:
				return false;
			}
		}
		cached = 1;
		return true;
	}

	bool tryInline(uint32_t calleeIndex, uint32_t id, const nir::Inst& inst, const CallRecord& record) {
		if (!inlining_ || record.unwind != nir::NO_UNWIND || !isInlinable(calleeIndex)) {
			return false;
		}
		const auto& block = m_.functionBlocks(calleeIndex)[0];
		if (block.argCount != record.args.size()) {
			return false;
		}
		Fn callee;
		Fn* caller = f_;
		prepare(callee, calleeIndex);
		callee.inlined = true;
		callee.nextBlock = nir::NONE;
		// the parameters read the caller's registers, or are constants the body folds
		for (uint32_t k = 0; k < block.argCount; ++k) {
			const auto pid = block.first + k;
			const Ref arg = record.args[k];
			if (const auto c = constOf(arg); c.has_value() && !isFp(typeOfRef(arg))) {
				callee.deferred[pid] |= kConstValue;
				callee.constBits[pid] = *c;
			} else if (isFp(typeOfRef(arg))) {
				callee.regs[pid] = xmm(arg);
			} else {
				callee.regs[pid] = gp(arg);
			}
		}
		f_ = &callee;
		analyzeAddresses();
		for (uint32_t k = block.argCount; k + 1 < block.count; ++k) {
			lowerInstruction(block.first + k);
		}
		lowerRet(callee.insts.back());
		auto result = std::move(callee.inlineResult);
		f_ = caller;
		if (result.has_value() && static_cast<TypeId>(inst.type) != TypeId::Void) {
			bindResult(id, *result);
		}
		++inlinedCalls_;
		return true;
	}
};

} // namespace

NirLowerResult lowerNirModule(const nir::Module& module, ::asmjit::JitRuntime& runtime, const engine::Options& options,
                              const DumpHandler& dumpHandler, CompilationStatistics* statistics,
                              const nirbackend::HostEnvironment& host) {
	CodeHolder code;
	code.init(runtime.environment(), runtime.cpuFeatures());
	ThrowOnError errHandler;
	code.setErrorHandler(&errHandler);

	// Only pay the formatting/logging cost when the corresponding dump is requested.
	const bool dumpAsmjitIR = dumpHandler.shouldDump("after_asmjit_generation");
	const bool dumpAssembly = dumpHandler.shouldDump("after_asmjit_assembly");
	StringLogger asmLogger;
	if (dumpAssembly) {
		code.setLogger(&asmLogger);
	}

	// Gated by `asmjit.enableIntrinsics` (default true); when off the manager stays empty and every intrinsic call is
	// the ordinary call of its fallback.
	AsmJitIntrinsicManager intrinsicManager;
	if (options.getOptionOrDefault<bool>("asmjit.enableIntrinsics", true)) {
		AsmJitIntrinsicPluginRegistry::instance().registerAllIntrinsics(intrinsicManager);
	}

	Lowerer lowerer(module, code, options, statistics, intrinsicManager, host);
	std::string asmjitIR;
	lowerer.run(dumpAsmjitIR ? &asmjitIR : nullptr);

	// label offsets while the code is still in the CodeHolder (before the runtime takes it)
	std::unordered_map<std::string, uint64_t> offsets;
	for (const auto& [name, node] : lowerer.functionNodes()) {
		offsets[name] = code.labelOffset(node->label());
	}
	const uint64_t codeSize = code.codeSize();

	void* basePtr = nullptr;
	if (runtime.add(&basePtr, &code)) {
		return {};
	}
	NirLowerResult result;
	result.basePtr = basePtr;
	result.codeSize = codeSize;
	for (const auto& [name, offset] : offsets) {
		result.jitPtrs[name] = reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(basePtr) + offset);
	}
	result.asmjitIR = std::move(asmjitIR);
	if (dumpAssembly) {
		result.assembly.assign(asmLogger.data(), asmLogger.dataSize());
	}
	return result;
}

} // namespace nautilus::compiler::asmjit

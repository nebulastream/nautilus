#include "nautilus/CompilationStatistics.hpp"
#include "nautilus/common/ExceptionTransport.hpp"
#include "nautilus/compiler/DumpHandler.hpp"
#include "nautilus/compiler/backends/asmjit/A64PostRAPeepholePass.hpp"
#include "nautilus/compiler/backends/asmjit/AsmJitNirLowering.hpp"
#include "nautilus/compiler/backends/asmjit/intrinsics/AsmJitBackendIntrinsic.hpp"
#include "nautilus/compiler/backends/nir/NirRuntime.hpp"
#include "nautilus/exceptions/NotImplementedException.hpp"
#include "nautilus/exceptions/RuntimeException.hpp"
#include "nautilus/extensions/ExtensionRegistry.hpp"
#include "nautilus/nir/Inst.hpp"
#include "nautilus/nir/Operands.hpp"
#include <algorithm>
#include <asmjit/a64.h>
#include <asmjit/arm/armutils.h>
#include <bit>
#include <chrono>
#include <cstring>
#include <functional>
#include <optional>
#include <span>
#include <stdexcept>

// The AArch64 lowering of SSA NIR (docs/plans/nautilus-0.4/05-backends.md §4, M4.4): the counterpart of
// X64NirLowering.cpp over AsmJit's a64 `Compiler`. Integers live in 64-bit virtual registers and are not kept extended:
// an operation on a type of at most 32 bits runs on the W view and leaves its upper bits as they come out, and each
// consumer that needs more (compares and divisions of 8 and 16 bits, widening casts, calls, returns) extends what it
// needs. `bool` is kept clean (a W register holding 0 or 1), since `cbnz` reads it.

namespace nautilus::compiler::asmjit {

using namespace ::asmjit;
using namespace ::asmjit::a64;
using nir::CalleeKind;
using N = nir::Op;
using nir::Ref;
using nir::TypeId;
using CC = arm::CondCode;

namespace {

class ThrowOnError : public ::asmjit::ErrorHandler {
public:
	void handleError(::asmjit::Error err, const char* message, ::asmjit::BaseEmitter* /*origin*/) override {
		fprintf(stderr, "[AsmJit/A64] Error %u: %s\n", err, message ? message : "(null)");
		throw std::runtime_error(std::string("AsmJit/A64 error: ") + (message ? message : "unknown"));
	}
};

bool isFp(TypeId t) {
	return t == TypeId::F32 || t == TypeId::F64;
}

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

Gp view(const Gp& r, unsigned opBits) {
	if (opBits == 64) {
		return r.x();
	}
	return r.w();
}

uint64_t maskOf(unsigned bits) {
	return bits >= 64 ? ~uint64_t {0} : (uint64_t {1} << bits) - 1;
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

/// What a compare leaves for a branch to read: the flags under a condition, or a register that is compared with zero.
struct Test {
	std::optional<Gp> zeroReg;
	bool equalZero = false;
	CC cond = CC::kAL;
};

/// The state of the function being lowered, in arrays indexed by value id or block id.
struct Fn {
	uint32_t index = 0;
	std::span<const nir::Inst> insts;
	std::span<const nir::BlockRecord> blocks;
	std::vector<BaseReg> regs;
	std::vector<uint32_t> useCount;
	std::vector<uint32_t> blockOf;
	std::vector<uint32_t> addressUses; ///< uses as the address of a load or store that the address fits
	std::vector<uint32_t> scaledUses;  ///< uses as the scaled index of a foldable address add in the same block
	std::vector<uint8_t> deferred;
	std::vector<uint64_t> constBits;
	std::vector<uint8_t> fusedCompare; ///< a `condbr` whose compare was not materialised
	std::vector<uint8_t> shared;       ///< a block that is lowered twice (tail duplication): its values share registers
	std::vector<Label> labels;
	std::vector<std::optional<Mem>> allocas;
	std::vector<std::function<void()>> cold;
	Label exitLabel;
	uint32_t block = 0;
	uint32_t nextBlock = nir::NONE;
	bool inlined = false;
	bool duplicating = false;
	std::optional<AsmReg> inlineResult;
};

class Lowerer {
public:
	Lowerer(const nir::Module& module, CodeHolder& code, const engine::Options& options,
	        CompilationStatistics* statistics, const AsmJitIntrinsicManager& intrinsics,
	        const nirbackend::HostEnvironment& host)
	    : m_(module), cc(&code), intrinsics_(intrinsics), host_(host), statistics_(statistics) {
		if (options.getOptionOrDefault<bool>("asmjit.enablePostRAPeephole", true)) {
			cc.addPassT<A64PostRAPeepholePass>(statistics);
		}
		branchFusion_ = options.getOptionOrDefault<bool>("asmjit.enableBranchFusion", true);
		constFolding_ = options.getOptionOrDefault<bool>("asmjit.enableConstFolding", true);
		selectCmov_ = options.getOptionOrDefault<bool>("asmjit.enableSelectCmov", true);
		lazyNarrowing_ = options.getOptionOrDefault<bool>("asmjit.enableLazyNarrowing", true);
		addressFusion_ = options.getOptionOrDefault<bool>("asmjit.enableAddressFusion", true);
		inlining_ = options.getOptionOrDefault<bool>("asmjit.enableInlining", true);
		loopRotation_ = options.getOptionOrDefault<bool>("asmjit.enableLoopRotation", true);
		tailDuplication_ = options.getOptionOrDefault<bool>("asmjit.enableTailDuplication", true);
		inliningMaxOperations_ =
		    static_cast<size_t>(options.getOptionOrDefault<int>("asmjit.inliningMaxOperations", 24));
	}

	void run(std::string* irDump) {
		if (m_.functions.empty()) {
			throw std::runtime_error("AsmJit: no functions found in the module");
		}
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
		if (statistics_ != nullptr && tailDuplication_) {
			statistics_->add("asmjit.lowering.duplicatedTails", duplicatedTails_);
		}

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
	a64::Compiler cc;
	const AsmJitIntrinsicManager& intrinsics_;
	const nirbackend::HostEnvironment& host_;
	CompilationStatistics* statistics_;

	std::vector<FuncNode*> nodes_;
	std::unordered_map<std::string, FuncNode*> nodesByName_;
	std::vector<int8_t> inlinable_;
	Fn* f_ = nullptr;

	bool branchFusion_ = true, constFolding_ = true, selectCmov_ = true, lazyNarrowing_ = true, addressFusion_ = true,
	     inlining_ = true, loopRotation_ = true, tailDuplication_ = true;
	size_t inliningMaxOperations_ = 24;
	int64_t fusedBranches_ = 0, foldedImmediates_ = 0, fusedAddresses_ = 0, inlinedCalls_ = 0, rotatedLoops_ = 0,
	        duplicatedTails_ = 0;

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

	Gp gpConst(uint64_t bits) {
		Gp g = cc.newInt64();
		cc.mov(g, Imm(bits));
		return g;
	}

	Vec newVec(TypeId t) {
		return t == TypeId::F32 ? cc.newVecS() : cc.newVecD();
	}

	/// A float constant, built through a general register (`fmov` takes W for S and X for D).
	void loadFloatConst(const Vec& v, uint64_t bits, TypeId t) {
		if (t == TypeId::F32) {
			Gp tmp = cc.newUInt32();
			cc.mov(tmp, static_cast<uint32_t>(bits));
			cc.fmov(v, tmp);
		} else {
			Gp tmp = cc.newUInt64();
			cc.mov(tmp, bits);
			cc.fmov(v, tmp);
		}
	}

	Gp gp(Ref r) {
		if (const auto c = constOf(r)) {
			return gpConst(*c);
		}
		return regOf(r).as<Gp>();
	}

	/// The value of `r` as a register that is zero when the constant is: the zero register stands in for it.
	Gp gpOrZero(Ref r) {
		if (const auto c = constOf(r); c.has_value() && *c == 0 && constFolding_) {
			return Gp(xzr);
		}
		return gp(r);
	}

	Vec vec(Ref r) {
		const auto t = typeOfRef(r);
		if (const auto c = constOf(r)) {
			Vec v = newVec(t);
			loadFloatConst(v, *c, t);
			return v;
		}
		return regOf(r).as<Vec>();
	}

	const BaseReg& regOf(Ref r) {
		if (!nir::isValueRef(r) || r >= F().regs.size() || !F().regs[r].isValid()) {
			throw RuntimeException("asmjit: value %" + std::to_string(r) + " is used before it is lowered");
		}
		return F().regs[r];
	}

	/// The register of value `id`: a value of a block that is lowered twice keeps the register of its first lowering.
	Gp defGp(uint32_t id) {
		if (F().regs[id].isValid()) {
			return F().regs[id].as<Gp>();
		}
		Gp g = cc.newInt64();
		F().regs[id] = g;
		return g;
	}

	Vec defVec(uint32_t id, TypeId t) {
		if (F().regs[id].isValid()) {
			return F().regs[id].as<Vec>();
		}
		Vec v = newVec(t);
		F().regs[id] = v;
		return v;
	}

	bool canReuse(Ref r) const {
		if (!nir::isValueRef(r) || f_->shared[f_->block] != 0 ||
		    (f_->inlined && f_->insts[r].op == static_cast<uint16_t>(N::BlockArg))) {
			return false;
		}
		return f_->deferred[r] == 0 && f_->useCount[r] == 1 && f_->blockOf[r] == f_->block && f_->regs[r].isValid();
	}

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

	Vec destVec(uint32_t id, Ref candidate, TypeId t, bool* reused = nullptr) {
		if (canReuse(candidate) && f_->regs[candidate].isVec() && typeOfRef(candidate) == t) {
			F().regs[id] = F().regs[candidate];
			if (reused != nullptr) {
				*reused = true;
			}
			return F().regs[id].as<Vec>();
		}
		if (reused != nullptr) {
			*reused = false;
		}
		return defVec(id, t);
	}

	void bindResult(uint32_t id, const AsmReg& r) {
		F().regs[id] = std::holds_alternative<Gp>(r) ? BaseReg(std::get<Gp>(r)) : BaseReg(std::get<Vec>(r));
	}

	BaseReg argRegOf(uint32_t b, uint32_t k) {
		const auto id = F().blocks[b].first + k;
		if (!F().regs[id].isValid()) {
			const auto t = static_cast<TypeId>(F().insts[id].type);
			F().regs[id] = isFp(t) ? BaseReg(newVec(t)) : BaseReg(cc.newInt64());
		}
		return F().regs[id];
	}

	// ---- immediates -------------------------------------------------------------------------------------------

	static bool isAddSubImm(uint64_t value) {
		return arm::Utils::isAddSubImm(value);
	}

	/// dst = src + value (or - value) at `ow` bits, with the immediate form when the constant encodes.
	void emitAddConst(const Gp& dst, const Gp& src, uint64_t value, bool subtract, unsigned ow) {
		const uint64_t mask = maskOf(ow);
		const uint64_t magnitude = (subtract ? (uint64_t {0} - value) : value) & mask;
		const uint64_t negated = (uint64_t {0} - magnitude) & mask;
		if (isAddSubImm(magnitude)) {
			cc.add(dst, src, Imm(magnitude));
			++foldedImmediates_;
		} else if (isAddSubImm(negated)) {
			cc.sub(dst, src, Imm(negated));
			++foldedImmediates_;
		} else {
			Gp tmp = cc.newInt64();
			cc.mov(tmp, Imm(magnitude));
			cc.add(dst, src, view(tmp, ow));
		}
	}

	// ---- analysis ---------------------------------------------------------------------------------------------

	void prepare(Fn& f, uint32_t function) {
		f.index = function;
		f.insts = m_.functionInsts(function);
		f.blocks = m_.functionBlocks(function);
		const auto n = f.insts.size();
		f.regs.assign(n, BaseReg());
		f.useCount.assign(n, 0);
		f.blockOf.assign(n, 0);
		f.addressUses.assign(n, 0);
		f.scaledUses.assign(n, 0);
		f.deferred.assign(n, 0);
		f.constBits.assign(n, 0);
		f.fusedCompare.assign(n, 0);
		f.shared.assign(f.blocks.size(), 0);
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
					++f.useCount[ref];
				}
			});
		}
	}

	static uint32_t accessSize(TypeId t) {
		return std::max<uint32_t>(nir::sizeOf(t), 1);
	}

	/// Counts the uses that fold into addresses; needs `f_` to be the function.
	void analyzeAddresses() {
		if (!addressFusion_) {
			return;
		}
		auto& f = F();
		for (uint32_t id = 0; id < f.insts.size(); ++id) {
			const auto& inst = f.insts[id];
			switch (static_cast<N>(inst.op)) {
			case N::Load:
			case N::Store: {
				if (!nir::isValueRef(inst.a) || f.blockOf[inst.a] != f.blockOf[id] ||
				    static_cast<N>(f.insts[inst.a].op) != N::Add) {
					break;
				}
				// the address must fit the access: a signed 9-bit or scaled unsigned 12-bit displacement, or an index
				// shifted by the access size or not at all
				const auto t = static_cast<N>(inst.op) == N::Load ? static_cast<TypeId>(inst.type) : typeOfRef(inst.b);
				if (const auto parts = matchAddress(inst.a);
				    parts.has_value() && fitsAccess(*parts, accessSize(t)) &&
				    inst.a != (static_cast<N>(inst.op) == N::Store ? inst.b : nir::NONE)) {
					++f.addressUses[inst.a];
				}
				break;
			}
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

	static bool fitsAccess(const AddressParts& parts, uint32_t size) {
		if (parts.offset == nir::NONE) {
			const int64_t disp = parts.disp;
			return (disp >= -256 && disp <= 255) || (disp >= 0 && disp % size == 0 && disp / size < 4096);
		}
		return parts.scaledInput == nir::NONE || parts.shift == 0 || (parts.shift <= 3 && (1u << parts.shift) == size);
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
				if (value.has_value() && !constOf(x).has_value() && *value > 0 && std::has_single_bit(*value)) {
					return std::make_pair(x, static_cast<uint32_t>(std::countr_zero(*value)));
				}
			}
		} else if (static_cast<N>(c.op) == N::Shl) {
			const auto count = constOf(c.b);
			if (count.has_value() && *count <= 63 && !constOf(c.a).has_value()) {
				return std::make_pair(c.a, static_cast<uint32_t>(*count));
			}
		}
		return std::nullopt;
	}

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
		if (const auto disp = constOf(offset); disp.has_value() && constFolding_) {
			const auto s = static_cast<int64_t>(*disp);
			if (s < INT32_MIN || s > INT32_MAX || constOf(base).has_value()) {
				return std::nullopt;
			}
			parts.base = base;
			parts.disp = static_cast<int32_t>(s);
			return parts;
		}
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

	bool isScaledIndexDeferred(Ref r) const {
		return nir::isValueRef(r) && (f_->deferred[r] & kScaledIndex) != 0;
	}
	bool isAddressPartDeferred(Ref r) const {
		return nir::isValueRef(r) && (f_->deferred[r] & kAddressPart) != 0;
	}

	Mem memFromParts(const AddressParts& parts) {
		auto base = gp(parts.base);
		if (parts.offset == nir::NONE) {
			return a64::ptr(base, parts.disp);
		}
		if (parts.scaledInput != nir::NONE && isScaledIndexDeferred(parts.offset)) {
			auto index = gp(parts.scaledInput);
			return parts.shift == 0 ? a64::ptr(base, index) : a64::ptr(base, index, arm::lsl(parts.shift));
		}
		return a64::ptr(base, gp(parts.offset));
	}

	Mem memOperand(Ref address) {
		if (isAddressPartDeferred(address)) {
			++fusedAddresses_;
			return memFromParts(*matchAddress(address));
		}
		return a64::ptr(gp(address));
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

		for (uint32_t k = 0; k < fn.blocks[0].argCount; ++k) {
			const auto id = fn.blocks[0].first + k;
			const auto t = static_cast<TypeId>(fn.insts[id].type);
			if (isFp(t)) {
				Vec abi = newVec(t);
				nodes_[function]->setArg(k, abi);
				Vec value = defVec(id, t);
				cc.fmov(value, abi);
			} else {
				Gp abi = cc.newInt64();
				nodes_[function]->setArg(k, abi);
				Gp value = defGp(id);
				cc.mov(value, abi);
				if (t == TypeId::Bool) {
					// AAPCS64 leaves the bits above a narrow argument unspecified; a bool is tested whole
					cc.uxtb(value.w(), value.w());
				}
			}
		}

		for (uint32_t b = 0; b < fn.blocks.size(); ++b) {
			lowerBlock(b);
		}
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
		f.nextBlock = b + 1 < f.blocks.size() ? b + 1 : nir::NONE;
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
			// A64 only encodes `fmov Sd, Wn` and `fmov Dd, Xn`: the general register matches the width
			Vec v = newVec(t);
			Gp tmp = t == TypeId::F32 ? cc.newUInt32() : cc.newUInt64();
			cc.mov(tmp, 0);
			cc.fmov(v, tmp);
			cc.ret(v);
		} else {
			Gp g = cc.newInt64();
			cc.mov(g, 0);
			cc.ret(g);
		}
	}

	Label exitLabel() {
		if (!F().exitLabel.isValid()) {
			F().exitLabel = cc.newLabel();
		}
		return F().exitLabel;
	}

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
		case N::FNeg: {
			const auto t = static_cast<TypeId>(inst.type);
			bool reused = false;
			Vec d = destVec(id, inst.a, t, &reused);
			cc.fneg(d, vec(inst.a));
			return;
		}
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
			cc.b(exitLabel());
			return;
		default:
			throw NotImplementedException("asmjit: extension operations are lowered with the extension opcode (M4.4)");
		}
	}

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
			cc.uxtb(g.w(), g.w());
			break;
		case 16:
			cc.uxth(g.w(), g.w());
			break;
		default:
			break;
		}
	}

	/// `dst` (W) = the value of narrow `src` extended to 32 bits.
	void extendNarrow(const Gp& dst, const Gp& src, unsigned bits, bool isSigned) {
		if (bits == 8) {
			isSigned ? cc.sxtb(dst.x(), src.w()) : cc.uxtb(dst.w(), src.w());
		} else {
			isSigned ? cc.sxth(dst.x(), src.w()) : cc.uxth(dst.w(), src.w());
		}
	}

	// ---- arithmetic -------------------------------------------------------------------------------------------

	void lowerArithmetic(N op, uint32_t id, const nir::Inst& inst) {
		const auto t = static_cast<TypeId>(inst.type);
		if (isFp(t)) {
			bool reused = false;
			Vec d = destVec(id, inst.a, t, &reused);
			const Vec a = vec(inst.a), b = vec(inst.b);
			op == N::Add ? cc.fadd(d, a, b) : op == N::Sub ? cc.fsub(d, a, b) : cc.fmul(d, a, b);
			return;
		}
		if (op == N::Add && (t == TypeId::Ptr || t == TypeId::I64)) {
			if (const auto parts = matchAddress(id)) {
				if (addressFusion_ && F().useCount[id] > 0 && F().addressUses[id] == F().useCount[id]) {
					F().deferred[id] |= kAddressPart;
					return;
				}
				return emitAddressAdd(id, *parts);
			}
		}
		if (op == N::Mul && addressFusion_ && F().useCount[id] > 0 && F().scaledUses[id] == F().useCount[id]) {
			F().deferred[id] |= kScaledIndex;
			return;
		}
		const unsigned ow = opBitsOf(t);
		Ref a = inst.a, b = inst.b;
		if (op != N::Sub && !constOf(b).has_value() && constOf(a).has_value()) {
			std::swap(a, b);
		}
		Gp d = destGp(id, a);
		const auto dv = view(d, ow);
		if (op == N::Mul) {
			if (const auto c = constOf(b);
			    c.has_value() && constFolding_ && *c > 1 && std::has_single_bit(*c) && (*c & maskOf(ow)) == *c) {
				cc.lsl(dv, view(gp(a), ow), Imm(std::countr_zero(*c)));
				++foldedImmediates_;
				narrowResult(d, t);
				return;
			}
			cc.mul(dv, view(gp(a), ow), view(gp(b), ow));
			narrowResult(d, t);
			return;
		}
		if (const auto c = constOf(b); c.has_value() && constFolding_) {
			emitAddConst(dv, view(gp(a), ow), *c, op == N::Sub, ow);
		} else if (op == N::Add) {
			cc.add(dv, view(gp(a), ow), view(gp(b), ow));
		} else {
			cc.sub(dv, view(gp(a), ow), view(gp(b), ow));
		}
		narrowResult(d, t);
	}

	/// A materialised address: one add with a shifted index, plus a constant.
	void emitAddressAdd(uint32_t id, const AddressParts& parts) {
		// the register of an operand that dies here takes the result
		Ref dying = nir::NONE;
		for (const Ref candidate :
		     {parts.base, parts.scaledInput != nir::NONE && isScaledIndexDeferred(parts.offset) ? parts.scaledInput
		                                                                                        : parts.offset}) {
			if (dying == nir::NONE && canReuse(candidate)) {
				dying = candidate;
			}
		}
		Gp d = destGp(id, dying);
		Gp base = gp(parts.base);
		if (parts.offset == nir::NONE) {
			emitAddConst(d.x(), base.x(), static_cast<uint64_t>(static_cast<int64_t>(parts.disp)), false, 64);
			return;
		}
		if (parts.scaledInput != nir::NONE && isScaledIndexDeferred(parts.offset)) {
			cc.add(d.x(), base.x(), gp(parts.scaledInput).x(), arm::lsl(parts.shift));
		} else {
			cc.add(d.x(), base.x(), gp(parts.offset).x());
		}
	}

	static bool isLogicalImm(uint64_t value, unsigned ow) {
		return arm::Utils::isLogicalImm(value & maskOf(ow), ow);
	}

	void lowerBitwise(N op, uint32_t id, const nir::Inst& inst) {
		const auto t = static_cast<TypeId>(inst.type);
		const unsigned ow = opBitsOf(t);
		Ref a = inst.a, b = inst.b;
		if (!constOf(b).has_value() && constOf(a).has_value()) {
			std::swap(a, b);
		}
		Gp d = destGp(id, a);
		const auto dv = view(d, ow);
		const auto av = view(gp(a), ow);
		const bool isAnd = op == N::And || op == N::BAnd;
		const bool isOr = op == N::Or || op == N::BOr;
		if (const auto c = constOf(b); c.has_value() && constFolding_ && isLogicalImm(*c, ow)) {
			const Imm imm(*c & maskOf(ow));
			isAnd ? cc.and_(dv, av, imm) : isOr ? cc.orr(dv, av, imm) : cc.eor(dv, av, imm);
			++foldedImmediates_;
		} else {
			const auto bv = view(gp(b), ow);
			isAnd ? cc.and_(dv, av, bv) : isOr ? cc.orr(dv, av, bv) : cc.eor(dv, av, bv);
		}
	}

	void lowerNot(N op, uint32_t id, const nir::Inst& inst) {
		const auto t = static_cast<TypeId>(inst.type);
		const unsigned ow = opBitsOf(t);
		Gp d = destGp(id, inst.a);
		if (op == N::Not) {
			cc.eor(d.w(), gp(inst.a).w(), Imm(1));
		} else {
			cc.mvn(view(d, ow), view(gp(inst.a), ow));
			narrowResult(d, t);
		}
	}

	// ---- shifts -----------------------------------------------------------------------------------------------

	void lowerShift(N op, uint32_t id, const nir::Inst& inst) {
		const auto t = static_cast<TypeId>(inst.type);
		const unsigned bits = bitsOf(t);
		const unsigned ow = opBitsOf(t);
		const bool isLeft = op == N::Shl;
		if (isLeft && addressFusion_ && F().useCount[id] > 0 && F().scaledUses[id] == F().useCount[id]) {
			F().deferred[id] |= kScaledIndex;
			return;
		}
		const bool isUnsigned = !isLeft && (inst.flags & 1) != 0;
		Gp d = destGp(id, inst.a);
		const auto dv = view(d, ow);
		Gp src = gp(inst.a);
		if (!isLeft && bits < 32) {
			// a right shift pulls the bits above the width down: extend first
			extendNarrow(d, src, bits, !isUnsigned);
			src = d;
		}
		if (const auto c = constOf(inst.b)) {
			const auto count = static_cast<uint32_t>(*c & (bits - 1));
			if (count == 0) {
				if (isLeft || bits >= 32) {
					cc.mov(dv, view(src, ow));
				}
			} else {
				isLeft       ? cc.lsl(dv, view(src, ow), Imm(count))
				: isUnsigned ? cc.lsr(dv, view(src, ow), Imm(count))
				             : cc.asr(dv, view(src, ow), Imm(count));
				++foldedImmediates_;
			}
		} else {
			Gp amount = gp(inst.b);
			if (bits < 32) {
				// the register forms take the count modulo 32, not modulo the width
				Gp masked = cc.newInt64();
				cc.and_(masked.w(), amount.w(), Imm(bits - 1));
				amount = masked;
			}
			// the register forms take the count modulo the register width, which is the width of 32- and 64-bit values
			isLeft       ? cc.lsl(dv, view(src, ow), view(amount, ow))
			: isUnsigned ? cc.lsr(dv, view(src, ow), view(amount, ow))
			             : cc.asr(dv, view(src, ow), view(amount, ow));
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
				bool reused = false;
				Vec d = destVec(id, inst.a, t, &reused);
				cc.fdiv(d, vec(inst.a), vec(inst.b));
				return;
			}
			Vec d = defVec(id, t);
			FuncSignature sig;
			sig.setRet(asmjitType(nir::makeTypeWord(t)));
			sig.addArg(asmjitType(nir::makeTypeWord(t)));
			sig.addArg(asmjitType(nir::makeTypeWord(t)));
			Vec a = vec(inst.a), b = vec(inst.b);
			void* helper = t == TypeId::F32 ? reinterpret_cast<void*>(&nirbackend::nautilus_rt_fmod_f32)
			                                : reinterpret_cast<void*>(&nirbackend::nautilus_rt_fmod_f64);
			Gp target = cc.newIntPtr();
			cc.mov(target, reinterpret_cast<uint64_t>(helper));
			InvokeNode* node = nullptr;
			cc.invoke(&node, target, sig);
			node->setArg(0, a);
			node->setArg(1, b);
			node->setRet(0, d);
			return;
		}
		const bool isUnsigned = (inst.flags & 1) != 0;
		const bool checked = (inst.flags & 2) != 0;
		const unsigned bits = bitsOf(t);
		const unsigned ow = opBitsOf(t);
		// the operands of the division, narrow ones extended
		Gp left = gp(inst.a), right = gp(inst.b);
		if (bits < 32) {
			Gp l = cc.newInt64(), r = cc.newInt64();
			extendNarrow(l, left, bits, !isUnsigned);
			extendNarrow(r, right, bits, !isUnsigned);
			left = l;
			right = r;
		}
		if (checked) {
			// the divisor is zero, or the signed minimum is divided by -1: compare and branch to a cold block that
			// throws (13 §5.10)
			Label cold = cc.newLabel();
			cc.cbz(view(right, ow), cold);
			if (!isUnsigned) {
				Label ok = cc.newLabel();
				cc.cmn(view(right, ow), Imm(1));
				cc.b(CC::kNE, ok);
				if (bits >= 32) {
					// a - 1 overflows exactly for the minimum of the type
					cc.cmp(view(left, ow), Imm(1));
					cc.b(CC::kVS, cold);
				} else {
					cc.cmn(view(left, ow), Imm(uint64_t {1} << (bits - 1)));
					cc.b(CC::kEQ, cold);
				}
				cc.bind(ok);
			}
			const auto target = unwindLabel(inst.c);
			F().cold.push_back([this, cold, target]() {
				cc.bind(cold);
				emitThrowArith();
				cc.b(target);
			});
		}
		Gp d = defGp(id);
		if (op == N::Div) {
			isUnsigned ? cc.udiv(view(d, ow), view(left, ow), view(right, ow))
			           : cc.sdiv(view(d, ow), view(left, ow), view(right, ow));
		} else {
			// there is no remainder instruction: dividend - quotient * divisor
			Gp q = cc.newInt64();
			isUnsigned ? cc.udiv(view(q, ow), view(left, ow), view(right, ow))
			           : cc.sdiv(view(q, ow), view(left, ow), view(right, ow));
			cc.msub(view(d, ow), view(q, ow), view(right, ow), view(left, ow));
		}
		narrowResult(d, t);
	}

	void emitThrowArith() {
		FuncSignature sig;
		sig.setRet(::asmjit::TypeId::kVoid);
		sig.addArg(::asmjit::TypeId::kUIntPtr);
		Gp target = cc.newIntPtr();
		cc.mov(target, reinterpret_cast<uint64_t>(nirbackend::throwArithTarget()));
		Gp thunk = cc.newIntPtr();
		cc.mov(thunk, reinterpret_cast<uint64_t>(nirbackend::throwArithThunk()));
		InvokeNode* node = nullptr;
		cc.invoke(&node, thunk, sig);
		node->setArg(0, target);
	}

	// ---- compare and select -----------------------------------------------------------------------------------

	static CC integerCondition(unsigned predicate, bool isUnsigned) {
		switch (predicate) {
		case 0:
			return CC::kEQ;
		case 1:
			return CC::kNE;
		case 2:
			return isUnsigned ? CC::kLO : CC::kLT;
		case 3:
			return isUnsigned ? CC::kLS : CC::kLE;
		case 4:
			return isUnsigned ? CC::kHI : CC::kGT;
		case 5:
			return isUnsigned ? CC::kHS : CC::kGE;
		default:
			throw RuntimeException("asmjit: an invalid compare predicate");
		}
	}

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

	/// The compare of `cmp`: flags under a condition, or a register tested against zero for an `eq`/`ne` with zero.
	Test emitIntegerCompare(const nir::Inst& cmp, bool allowZeroTest) {
		unsigned predicate = cmp.flags & 7;
		const auto type = typeOfRef(cmp.a);
		const bool isUnsigned = (cmp.flags & 8) != 0 || type == TypeId::Ptr;
		const unsigned bits = bitsOf(type);
		const unsigned ow = opBitsOf(type);
		Ref l = cmp.a, r = cmp.b;
		if (constOf(l).has_value() && !constOf(r).has_value()) {
			std::swap(l, r);
			predicate = swapPredicate(predicate);
		}
		// an operand of 8 or 16 bits is extended to 32 first; the extension follows the order of the compare
		const bool signedOrder = (predicate >= 2) && !isUnsigned;
		Gp left = gp(l);
		if (bits < 32 && type != TypeId::Bool) {
			Gp e = cc.newInt64();
			extendNarrow(e, left, bits, signedOrder);
			left = e;
		} else if (type == TypeId::Bool) {
			// bools are kept clean
		}
		const auto c = constOf(r);
		if (allowZeroTest && c.has_value() && constFolding_ && (*c & maskOf(bits)) == 0 &&
		    (predicate == 0 || predicate == 1)) {
			Test t;
			t.zeroReg = view(left, ow);
			t.equalZero = predicate == 0;
			++foldedImmediates_;
			return t;
		}
		Test t;
		t.cond = integerCondition(predicate, isUnsigned);
		if (c.has_value() && constFolding_) {
			// the constant as the extended compare sees it
			uint64_t k = *c & maskOf(bits);
			if (signedOrder && bits < 32 && (k >> (bits - 1)) != 0) {
				k |= maskOf(32) & ~maskOf(bits);
			}
			k &= maskOf(ow);
			const uint64_t negated = (uint64_t {0} - k) & maskOf(ow);
			if (isAddSubImm(k)) {
				cc.cmp(view(left, ow), Imm(k));
				++foldedImmediates_;
				return t;
			}
			if (k != 0 && isAddSubImm(negated) && (predicate == 0 || predicate == 1)) {
				// `cmn x, #-k` sets the flags that decide equality like `cmp x, #k`
				cc.cmn(view(left, ow), Imm(negated));
				++foldedImmediates_;
				return t;
			}
		}
		Gp right = gp(r);
		if (bits < 32 && type != TypeId::Bool) {
			Gp e = cc.newInt64();
			extendNarrow(e, right, bits, signedOrder);
			right = e;
		}
		cc.cmp(view(left, ow), view(right, ow));
		return t;
	}

	void emitBranchOn(const Test& test, bool jumpIfTrue, const Label& target) {
		if (test.zeroReg.has_value()) {
			const bool branchIfZero = test.equalZero == jumpIfTrue;
			branchIfZero ? cc.cbz(*test.zeroReg, target) : cc.cbnz(*test.zeroReg, target);
			return;
		}
		cc.b(jumpIfTrue ? test.cond : arm::negateCond(test.cond), target);
	}

	bool isFusibleCompare(uint32_t id) const {
		const auto& f = *f_;
		const auto& cmp = f.insts[id];
		if (!branchFusion_ || id + 1 >= f.insts.size() || f.blockOf[id + 1] != f.blockOf[id]) {
			return false;
		}
		const auto& next = f.insts[id + 1];
		return static_cast<N>(next.op) == N::CondBr && next.a == id && f.useCount[id] == 1 && !isFp(typeOfRef(cmp.a));
	}

	void lowerCompare(uint32_t id, const nir::Inst& inst) {
		if (isFusibleCompare(id)) {
			F().fusedCompare[id + 1] = 1;
			return;
		}
		const auto type = typeOfRef(inst.a);
		const unsigned predicate = inst.flags & 7;
		if (isFp(type)) {
			cc.fcmp(vec(inst.a), vec(inst.b));
			// fcmp sets NZCV so that these conditions give C++'s semantics for unordered operands
			CC cond;
			switch (predicate) {
			case 0:
				cond = CC::kEQ;
				break;
			case 1:
				cond = CC::kNE;
				break;
			case 2:
				cond = CC::kMI;
				break;
			case 3:
				cond = CC::kLS;
				break;
			case 4:
				cond = CC::kGT;
				break;
			case 5:
				cond = CC::kGE;
				break;
			default:
				throw RuntimeException("asmjit: an invalid compare predicate");
			}
			Gp d = defGp(id);
			cc.cset(d.w(), Imm(static_cast<uint32_t>(cond)));
			return;
		}
		const auto test = emitIntegerCompare(inst, false);
		Gp d = destGp(id, inst.a);
		cc.cset(d.w(), Imm(static_cast<uint32_t>(test.cond)));
	}

	void lowerSelect(uint32_t id, const nir::Inst& inst) {
		const auto t = static_cast<TypeId>(inst.type);
		Gp cond = gp(inst.a);
		if (selectCmov_) {
			cc.cmp(cond.w(), Imm(0));
			const Imm ne(static_cast<uint32_t>(CC::kNE));
			if (isFp(t)) {
				Vec d = defVec(id, t);
				cc.fcsel(d, vec(inst.b), vec(inst.c), ne);
			} else {
				const unsigned ow = opBitsOf(t);
				Gp d = defGp(id);
				cc.csel(view(d, ow), view(gpOrZero(inst.b), ow), view(gpOrZero(inst.c), ow), ne);
			}
			return;
		}
		Label falsePath = cc.newLabel(), done = cc.newLabel();
		AsmReg d = isFp(t) ? AsmReg(defVec(id, t)) : AsmReg(defGp(id));
		cc.cbz(cond.w(), falsePath);
		emitMove(d, inst.b);
		cc.b(done);
		cc.bind(falsePath);
		emitMove(d, inst.c);
		cc.bind(done);
	}

	void emitMove(const AsmReg& dst, Ref src) {
		if (std::holds_alternative<Vec>(dst)) {
			const auto d = std::get<Vec>(dst);
			if (const auto c = constOf(src)) {
				loadFloatConst(d, *c, typeOfRef(src));
			} else {
				cc.fmov(d, regOf(src).as<Vec>());
			}
		} else {
			const auto d = std::get<Gp>(dst);
			if (const auto c = constOf(src)) {
				cc.mov(d, Imm(*c));
			} else {
				cc.mov(d, regOf(src).as<Gp>());
			}
		}
	}

	// ---- casts ------------------------------------------------------------------------------------------------

	void lowerCast(uint32_t id, const nir::Inst& inst) {
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
					cc.sub(d.x(), Gp(xzr), src.x());
				} else {
					cc.sxtb(d.x(), src.w());
				}
				break;
			case 16:
				cc.sxth(d.x(), src.w());
				break;
			case 32:
				cc.sxtw(d.x(), src.w());
				break;
			default:
				cc.mov(d.x(), src.x());
				break;
			}
			return;
		}
		case 1: { // zext
			Gp src = gp(inst.a);
			Gp d = destGp(id, inst.a);
			switch (bitsOf(from)) {
			case 8:
				cc.uxtb(d.w(), src.w());
				break;
			case 16:
				cc.uxth(d.w(), src.w());
				break;
			case 32:
				cc.mov(d.w(), src.w());
				break;
			default:
				cc.mov(d.x(), src.x());
				break;
			}
			return;
		}
		case 2: { // trunc
			bool reused = false;
			Gp d = destGp(id, inst.a, &reused);
			if (!reused) {
				cc.mov(d.x(), gp(inst.a).x());
			}
			narrowResult(d, to);
			return;
		}
		case 3:
		case 4:
			return lowerIntToFloat(id, inst, kind == 3);
		case 5:
		case 6:
			return lowerFloatToInt(id, inst, kind == 5);
		case 7:
		case 8: {
			Vec src = vec(inst.a);
			Vec d = defVec(id, to);
			cc.fcvt(d, src);
			return;
		}
		case 9: { // bitcast
			if (isFp(to)) {
				Gp src = gp(inst.a);
				Vec d = defVec(id, to);
				if (to == TypeId::F32) {
					cc.fmov(d, src.w());
				} else {
					cc.fmov(d, src.x());
				}
			} else if (isFp(from)) {
				Vec src = vec(inst.a);
				Gp d = defGp(id);
				if (from == TypeId::F32) {
					cc.fmov(d.w(), src);
				} else {
					cc.fmov(d.x(), src);
				}
			} else {
				bool reused = false;
				Gp d = destGp(id, inst.a, &reused);
				if (!reused) {
					cc.mov(d.x(), gp(inst.a).x());
				}
			}
			return;
		}
		case 10:
		case 11: {
			bool reused = false;
			Gp d = destGp(id, inst.a, &reused);
			if (!reused) {
				cc.mov(d.x(), gp(inst.a).x());
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
		const unsigned bits = bitsOf(from);
		Gp src = gp(inst.a);
		Vec d = defVec(id, to);
		if (bits < 64) {
			// the source is extended to a register of its own: scvtf/ucvtf read the whole register
			Gp wide = cc.newInt64();
			if (bits == 8) {
				isSigned ? cc.sxtb(wide.x(), src.w()) : cc.uxtb(wide.w(), src.w());
			} else if (bits == 16) {
				isSigned ? cc.sxth(wide.x(), src.w()) : cc.uxth(wide.w(), src.w());
			} else {
				isSigned ? cc.sxtw(wide.x(), src.w()) : cc.mov(wide.w(), src.w());
			}
			src = wide;
		}
		isSigned ? cc.scvtf(d, src.x()) : cc.ucvtf(d, src.x());
	}

	/// fcvtzs and fcvtzu saturate and map NaN to 0 themselves (D11); results narrower than 32 bits are clamped.
	void lowerFloatToInt(uint32_t id, const nir::Inst& inst, bool isSigned) {
		const auto to = static_cast<TypeId>(inst.type);
		const unsigned bits = bitsOf(to);
		Vec x = vec(inst.a);
		Gp d = defGp(id);
		const unsigned ow = bits == 64 ? 64 : 32;
		isSigned ? cc.fcvtzs(view(d, ow), x) : cc.fcvtzu(view(d, ow), x);
		if (bits >= 32) {
			return;
		}
		Gp limit = cc.newInt64();
		if (isSigned) {
			const uint32_t hi = bits == 8 ? 127 : 32767;
			const uint64_t lo = bits == 8 ? 0xFFFFFF80u : 0xFFFF8000u;
			cc.mov(limit.w(), hi);
			cc.cmp(d.w(), limit.w());
			cc.csel(d.w(), d.w(), limit.w(), Imm(static_cast<uint32_t>(CC::kLE)));
			cc.mov(limit.w(), static_cast<uint32_t>(lo));
			cc.cmp(d.w(), limit.w());
			cc.csel(d.w(), d.w(), limit.w(), Imm(static_cast<uint32_t>(CC::kGE)));
		} else {
			cc.mov(limit.w(), bits == 8 ? 0xFFu : 0xFFFFu);
			cc.cmp(d.w(), limit.w());
			cc.csel(d.w(), d.w(), limit.w(), Imm(static_cast<uint32_t>(CC::kLS)));
		}
	}

	// ---- memory -----------------------------------------------------------------------------------------------

	void lowerLoad(uint32_t id, const nir::Inst& inst) {
		const auto t = static_cast<TypeId>(inst.type);
		const auto mem = memOperand(inst.a);
		if (isFp(t)) {
			Vec d = defVec(id, t);
			cc.ldr(d, mem);
			return;
		}
		Gp d = defGp(id);
		switch (bitsOf(t)) {
		case 8:
			cc.ldrb(d.w(), mem);
			break;
		case 16:
			cc.ldrh(d.w(), mem);
			break;
		case 32:
			cc.ldr(d.w(), mem);
			break;
		default:
			cc.ldr(d.x(), mem);
			break;
		}
	}

	void lowerStore(const nir::Inst& inst) {
		const auto t = typeOfRef(inst.b);
		if (isFp(t)) {
			Vec v = vec(inst.b);
			cc.str(v, memOperand(inst.a));
			return;
		}
		Gp v = gpOrZero(inst.b);
		const auto mem = memOperand(inst.a);
		switch (bitsOf(t)) {
		case 8:
			cc.strb(v.w(), mem);
			break;
		case 16:
			cc.strh(v.w(), mem);
			break;
		case 32:
			cc.str(v.w(), mem);
			break;
		default:
			cc.str(v.x(), mem);
			break;
		}
	}

	void lowerAlloca(uint32_t id, const nir::Inst& inst) {
		auto& slot = F().allocas.at(inst.a);
		if (!slot.has_value()) {
			const auto& record = m_.functionAllocas(F().index)[inst.a];
			slot = cc.newStack(record.size, std::max<uint32_t>(record.align, 1));
		}
		Gp d = defGp(id);
		cc.loadAddressOf(d, *slot);
	}

	// ---- control flow -----------------------------------------------------------------------------------------

	struct Copy {
		BaseReg dst;
		Ref src;
	};

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
			cc.mov(dst.as<Gp>().x(), src.as<Gp>().x());
		} else {
			cc.fmov(dst.as<Vec>(), src.as<Vec>());
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
			for (size_t i = 0; i < moves.size(); ++i) {
				if (done[i]) {
					continue;
				}
				const BaseReg dst = moves[i].dst;
				BaseReg temp = dst.isGp() ? BaseReg(cc.newInt64()) : BaseReg(cc.newVecD());
				emitMoveRegs(temp, dst);
				for (size_t j = 0; j < moves.size(); ++j) {
					if (!done[j] && moves[j].src.id() == dst.id()) {
						moves[j].src = temp;
					}
				}
				break;
			}
		}
		for (const auto& copy : constants) {
			const auto bits = *constOf(copy.src);
			if (copy.dst.isGp()) {
				cc.mov(copy.dst.as<Gp>().x(), Imm(bits));
			} else {
				loadFloatConst(copy.dst.as<Vec>(), bits, typeOfRef(copy.src));
			}
		}
	}

	void lowerBr(const nir::Inst& inst) {
		auto& f = F();
		const uint32_t* list = m_.extra.data() + inst.c;
		const uint32_t target = inst.a;
		emitEdge(target, std::span<const uint32_t>(list + 1, list[0]));
		if (tryRotateLoop(target) || tryDuplicateTail(target)) {
			return;
		}
		if (target != f.nextBlock) {
			cc.b(f.labels[target]);
		}
	}

	/// See X64NirLowering.cpp: the header's test repeated at the end of the back edge.
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
		const uint32_t trueBlock = br.b, trueArgc = list[1];
		if (trueArgc != 0) {
			return false;
		}
		const uint32_t falseBlock = list[2], falseArgc = list[3];
		if (falseArgc != 0) {
			return false;
		}
		const auto test = emitIntegerCompare(cmp, true);
		emitBranchOn(test, true, f.labels[trueBlock]);
		cc.b(f.labels[falseBlock]);
		++rotatedLoops_;
		return true;
	}

	/// A diamond's arms both branch to the merge block, and only one of them can fall into it; the other pays a taken
	/// branch, on top of the one that entered it. When the merge block is small and ends in an unconditional branch,
	/// that arm lowers a copy of it instead. The copy writes the registers of the block's own lowering (a block that is
	/// lowered twice defines its values into the same registers, and reuses none), so everything that follows sees what
	/// it expects.
	bool tryDuplicateTail(uint32_t target) {
		auto& f = F();
		if (!tailDuplication_ || f.duplicating || f.inlined || target <= f.block || target == f.nextBlock) {
			return false;
		}
		const auto& block = f.blocks[target];
		constexpr uint32_t maxTailOperations = 6;
		if (block.count <= block.argCount || block.count - block.argCount > maxTailOperations + 1) {
			return false;
		}
		const auto& last = f.insts[block.first + block.count - 1];
		if (static_cast<N>(last.op) != N::Br) {
			return false;
		}
		for (uint32_t k = block.argCount; k + 1 < block.count; ++k) {
			switch (static_cast<N>(f.insts[block.first + k].op)) {
			case N::Add:
			case N::Sub:
			case N::Mul:
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
			case N::Load:
			case N::Cast:
				break;
			default:
				return false;
			}
		}
		f.shared[target] = 1;
		f.duplicating = true;
		for (uint32_t k = block.argCount; k < block.count; ++k) {
			lowerInstruction(block.first + k);
		}
		f.duplicating = false;
		++duplicatedTails_;
		return true;
	}

	void lowerCondBr(uint32_t id, const nir::Inst& inst) {
		auto& f = F();
		const uint32_t* list = m_.extra.data() + inst.c;
		const uint32_t trueBlock = inst.b;
		const uint32_t trueArgc = list[1];
		const std::span<const uint32_t> trueArgs(list + 2, trueArgc);
		const uint32_t falseBlock = list[2 + trueArgc];
		const uint32_t falseArgc = list[3 + trueArgc];
		const std::span<const uint32_t> falseArgs(list + 4 + trueArgc, falseArgc);

		if (const auto c = constOf(inst.a)) {
			const bool value = (*c & 1) != 0;
			emitEdge(value ? trueBlock : falseBlock, value ? trueArgs : falseArgs);
			const auto target = value ? trueBlock : falseBlock;
			if (target != f.nextBlock) {
				cc.b(f.labels[target]);
			}
			return;
		}
		Test test;
		if (f.fusedCompare[id] != 0) {
			test = emitIntegerCompare(f.insts[inst.a], true);
			++fusedBranches_;
		} else {
			test.zeroReg = gp(inst.a).w();
			test.equalZero = false;
		}

		if (trueArgc == 0 && falseArgc == 0) {
			if (trueBlock == f.nextBlock) {
				emitBranchOn(test, false, f.labels[falseBlock]);
			} else {
				emitBranchOn(test, true, f.labels[trueBlock]);
				if (falseBlock != f.nextBlock) {
					cc.b(f.labels[falseBlock]);
				}
			}
			return;
		}
		// edges with arguments: the copies follow the branch, and moves leave the flags alone
		Label elsePath = cc.newLabel();
		emitBranchOn(test, false, elsePath);
		emitEdge(trueBlock, trueArgs);
		cc.b(f.labels[trueBlock]);
		cc.bind(elsePath);
		emitEdge(falseBlock, falseArgs);
		if (falseBlock != f.nextBlock) {
			cc.b(f.labels[falseBlock]);
		}
	}

	void lowerRet(const nir::Inst& inst) {
		auto& f = F();
		if (f.inlined) {
			if (inst.a != nir::NONE) {
				const auto t = typeOfRef(inst.a);
				if (isFp(t)) {
					Vec r = newVec(t);
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
		if (isFp(nir::typeOf(result))) {
			cc.ret(vec(inst.a));
			return;
		}
		cc.ret(extendedForAbi(inst.a, result));
	}

	/// The register of integer value `ref` extended to the full register as the extension kind of `word` says, for a
	/// value that crosses a native boundary (13 §4.5.1).
	Gp extendedForAbi(Ref ref, nir::TypeWord word) {
		const auto t = nir::typeOf(word);
		const auto kind = nir::extKindOf(word);
		if (const auto c = constOf(ref)) {
			uint64_t bits = *c;
			if (kind == nir::ExtKind::Sext && bitsOf(t) < 64) {
				const unsigned w = bitsOf(t);
				bits = static_cast<uint64_t>(static_cast<int64_t>(bits << (64 - w)) >> (64 - w));
			}
			return gpConst(bits);
		}
		Gp g = gp(ref);
		if (bitsOf(t) == 64 || (t == TypeId::I32 && kind == nir::ExtKind::None) || nir::isFloat(t)) {
			return g;
		}
		Gp e = cc.newInt64();
		const bool sign = kind == nir::ExtKind::Sext;
		switch (bitsOf(t)) {
		case 8:
			sign ? cc.sxtb(e.x(), g.w()) : cc.uxtb(e.w(), g.w());
			break;
		case 16:
			sign ? cc.sxth(e.x(), g.w()) : cc.uxth(e.w(), g.w());
			break;
		default:
			sign ? cc.sxtw(e.x(), g.w()) : cc.mov(e.w(), g.w());
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

	void emitCheckPending(uint32_t unwind) {
		if (unwind == nir::NO_UNWIND) {
			return;
		}
		const Label target = unwindLabel(unwind);
		FuncSignature frameSig;
		frameSig.setRet(::asmjit::TypeId::kUIntPtr);
		InvokeNode* frameInvoke = nullptr;
		Gp frameFn = cc.newIntPtr();
		cc.mov(frameFn, reinterpret_cast<uint64_t>(&currentExceptionFrame));
		cc.invoke(&frameInvoke, frameFn, frameSig);
		Gp frame = cc.newIntPtr();
		frameInvoke->setRet(0, frame);
		Gp pending = cc.newIntPtr();
		cc.ldr(pending, a64::ptr(frame, static_cast<int32_t>(offsetof(ExceptionFrame, pending))));
		cc.cbnz(pending, target);
	}

	std::vector<AsmReg> argumentRegisters(const nir::SignatureRecord& sig, std::span<const uint32_t> args,
	                                      bool native) {
		std::vector<AsmReg> regs;
		regs.reserve(args.size());
		for (size_t i = 0; i < args.size(); ++i) {
			const auto word = m_.paramType(sig, static_cast<uint32_t>(i));
			if (nir::isFloat(nir::typeOf(word))) {
				regs.emplace_back(vec(args[i]));
			} else {
				regs.emplace_back(native ? extendedForAbi(args[i], word) : gp(args[i]));
			}
		}
		return regs;
	}

	void setInvokeArguments(InvokeNode* node, size_t first, const std::vector<AsmReg>& regs) {
		for (size_t i = 0; i < regs.size(); ++i) {
			if (std::holds_alternative<Vec>(regs[i])) {
				node->setArg(first + i, std::get<Vec>(regs[i]));
			} else {
				node->setArg(first + i, std::get<Gp>(regs[i]));
			}
		}
	}

	void bindCallResult(InvokeNode* node, uint32_t id, TypeId type) {
		if (type == TypeId::Void) {
			return;
		}
		if (isFp(type)) {
			Vec r = defVec(id, type);
			node->setRet(0, r);
		} else {
			Gp r = defGp(id);
			node->setRet(0, r);
			if (type == TypeId::Bool) {
				// the callee guarantees the low byte only
				cc.uxtb(r.w(), r.w());
			}
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
			const auto regs = argumentRegisters(sig, record.args, false);
			// blr takes a register: the label's address is built with adr
			Gp target = cc.newIntPtr();
			cc.adr(target, nodes_.at(callee.target)->label());
			InvokeNode* node = nullptr;
			cc.invoke(&node, target, signatureOf(sig));
			setInvokeArguments(node, 0, regs);
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
				const auto regs = argumentRegisters(sig, record.args, true);
				IntrinsicCallContext ctx {cc, regs, std::nullopt};
				if ((*handler)(ctx)) {
					if (ctx.result.has_value()) {
						bindResult(id, *ctx.result);
					}
					return;
				}
			}
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

	void emitNativeCall(const nir::SignatureRecord& sig, void* target, void* thunk, const CallRecord& record,
	                    uint32_t id, TypeId type) {
		const auto regs = argumentRegisters(sig, record.args, true);
		InvokeNode* node = nullptr;
		if (thunk != nullptr) {
			Gp targetReg = cc.newIntPtr();
			cc.mov(targetReg, reinterpret_cast<uint64_t>(target));
			Gp thunkReg = cc.newIntPtr();
			cc.mov(thunkReg, reinterpret_cast<uint64_t>(thunk));
			cc.invoke(&node, thunkReg, signatureOf(sig, true));
			node->setArg(0, targetReg);
			setInvokeArguments(node, 1, regs);
		} else {
			Gp targetReg = cc.newIntPtr();
			cc.mov(targetReg, reinterpret_cast<uint64_t>(target));
			cc.invoke(&node, targetReg, signatureOf(sig));
			setInvokeArguments(node, 0, regs);
		}
		bindCallResult(node, id, type);
		emitCheckPending(record.unwind);
	}

	void lowerCallIndirect(uint32_t id, const nir::Inst& inst) {
		const auto& sig = m_.signatures[inst.b];
		const auto record = callRecord(m_.extra.data() + inst.c);
		const auto type = nir::typeOf(sig.result);
		Gp target = gp(inst.a);
		const auto regs = argumentRegisters(sig, record.args, true);
		InvokeNode* node = nullptr;
		if (record.unwind != nir::NO_UNWIND) {
			if (record.thunk == nir::NONE) {
				throw RuntimeException("asmjit: an indirect call that may throw names no capture thunk");
			}
			Gp thunkReg = cc.newIntPtr();
			cc.mov(thunkReg, reinterpret_cast<uint64_t>(nirbackend::resolvePointer(m_, record.thunk, host_)));
			cc.invoke(&node, thunkReg, signatureOf(sig, true));
			node->setArg(0, target);
			setInvokeArguments(node, 1, regs);
		} else {
			cc.invoke(&node, target, signatureOf(sig));
			setInvokeArguments(node, 0, regs);
		}
		bindCallResult(node, id, type);
		emitCheckPending(record.unwind);
	}

	void lowerFuncAddr(uint32_t id, const nir::Inst& inst) {
		const auto& callee = m_.callees.at(inst.a);
		switch (static_cast<CalleeKind>(callee.kind)) {
		case CalleeKind::Internal: {
			Gp d = defGp(id);
			cc.adr(d, nodes_.at(callee.target)->label());
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

	// ---- inlining (see X64NirLowering.cpp) --------------------------------------------------------------------

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
		for (uint32_t k = 0; k < block.argCount; ++k) {
			const auto pid = block.first + k;
			const Ref arg = record.args[k];
			if (const auto c = constOf(arg); c.has_value() && !isFp(typeOfRef(arg))) {
				callee.deferred[pid] |= kConstValue;
				callee.constBits[pid] = *c;
			} else if (isFp(typeOfRef(arg))) {
				callee.regs[pid] = vec(arg);
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

	const bool dumpAsmjitIR = dumpHandler.shouldDump("after_asmjit_generation");
	const bool dumpAssembly = dumpHandler.shouldDump("after_asmjit_assembly");
	StringLogger asmLogger;
	if (dumpAssembly) {
		code.setLogger(&asmLogger);
	}

	AsmJitIntrinsicManager intrinsicManager;
	if (options.getOptionOrDefault<bool>("asmjit.enableIntrinsics", true)) {
		AsmJitIntrinsicPluginRegistry::instance().registerAllIntrinsics(intrinsicManager);
	}

	Lowerer lowerer(module, code, options, statistics, intrinsicManager, host);
	std::string asmjitIR;
	lowerer.run(dumpAsmjitIR ? &asmjitIR : nullptr);

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

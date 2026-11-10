#include "nautilus/compiler/backends/mlir/MLIRNirLowering.hpp"
#include "nautilus/compiler/backends/mlir/LLVMBackendHooks.hpp"
#include "nautilus/compiler/backends/nir/NirRuntime.hpp"
#include "nautilus/exceptions/NotImplementedException.hpp"
#include "nautilus/exceptions/RuntimeException.hpp"
#include "nautilus/extensions/ExtensionRegistry.hpp"
#include "nautilus/nir/Inst.hpp"
#include "nautilus/tracing/FunctionNameCache.hpp"
#include <cstdio>
#include <llvm/ADT/APFloat.h>
#include <llvm/ADT/APInt.h>
#include <llvm/ADT/SmallVector.h>
#include <map>
#include <mlir/Dialect/Arith/IR/Arith.h>
#include <mlir/Dialect/ControlFlow/IR/ControlFlow.h>
#include <mlir/Dialect/ControlFlow/IR/ControlFlowOps.h>
#include <mlir/Dialect/Func/IR/FuncOps.h>
#include <mlir/Dialect/LLVMIR/LLVMDialect.h>
#include <mlir/Dialect/LLVMIR/LLVMTypes.h>
#include <mlir/IR/Builders.h>
#include <mlir/IR/BuiltinAttributes.h>
#include <mlir/IR/Verifier.h>
#include <set>
#include <string>
#include <unordered_map>

namespace nautilus::compiler::mlir {

NirExtensionHooks& NirExtensionHooks::instance() {
	static NirExtensionHooks hooks;
	return hooks;
}

void NirExtensionHooks::registerType(nir::TypeId type, TypeMapper mapper) {
	types_[static_cast<uint8_t>(type)] = std::move(mapper);
}

void NirExtensionHooks::registerOp(uint16_t op, OpLowering lowering) {
	ops_[op] = std::move(lowering);
}

const NirExtensionHooks::TypeMapper* NirExtensionHooks::type(nir::TypeId type) const {
	auto it = types_.find(static_cast<uint8_t>(type));
	return it == types_.end() ? nullptr : &it->second;
}

const NirExtensionHooks::OpLowering* NirExtensionHooks::op(uint16_t op) const {
	auto it = ops_.find(op);
	return it == ops_.end() ? nullptr : &it->second;
}

namespace {

namespace M = ::mlir;
using N = nir::Op;
using nir::CalleeKind;
using nir::TypeId;

/// Whether the host C ABI makes the caller extend a narrow integer argument (see the 0.3 lowering): then the entry of
/// a function can trust it. AAPCS64 leaves the upper bits unspecified.
constexpr bool callerExtendsNarrowArguments() {
#if defined(__aarch64__) && !defined(__APPLE__)
	return false;
#else
	return true;
#endif
}

std::string hexAddress(const void* address) {
	char buffer[32];
	std::snprintf(buffer, sizeof(buffer), "%llx",
	              static_cast<unsigned long long>(reinterpret_cast<uintptr_t>(address)));
	return buffer;
}

/// How a call to a native function reaches its target and what its declaration promises.
struct ExternalKey {
	void* address;
	uint32_t signature;
	uint32_t attrs;
	bool operator<(const ExternalKey& o) const {
		return std::tie(address, signature, attrs) < std::tie(o.address, o.signature, o.attrs);
	}
};

class Lowering {
public:
	Lowering(M::MLIRContext& context, const nir::Module& module, const nirbackend::HostEnvironment& host,
	         const engine::Options& options, const MLIRIntrinsicManager* intrinsics)
	    : ctx_(context), m_(module), host_(host), intrinsics_(intrinsics),
	      builderOwner_(std::make_unique<M::OpBuilder>(&context)), builder_(*builderOwner_),
	      loc_(M::UnknownLoc::get(&context)) {
		const bool inspected = options.getOptionOrDefault("debug", false) ||
		                       options.getOptionOrDefault("perf", false) ||
		                       options.getOptionOrDefault("perf.sample", false);
		resolveNames_ = options.getOptionOrDefault("engine.resolveFunctionNames", inspected);
		demangle_ = options.getOptionOrDefault("engine.demangleFunctionNames", true);
		inlineInvokeCalls_ = options.getOptionOrDefault("mlir.inline_invoke_calls", false);
		normalizeNames_ = options.getOptionOrDefault("engine.normalizeFunctionNames", false);
	}

	MLIRNirModule run() {
		for (uint32_t f = 0; f < m_.functions.size(); ++f) {
			if (m_.functions[f].level != static_cast<uint8_t>(nir::Level::SSA)) {
				throw RuntimeException("mlir: function #" + std::to_string(f) + " is not an SSA function");
			}
		}
		ctx_.loadDialect<M::arith::ArithDialect, M::cf::ControlFlowDialect, M::LLVM::LLVMDialect,
		                 M::func::FuncDialect>();
		module_ = M::ModuleOp::create(loc_);
		builder_.setInsertionPointToEnd(module_.getBody());

		// every function exists before any body is lowered: functions call each other regardless of order
		for (uint32_t f = 0; f < m_.functions.size(); ++f) {
			declareFunction(f);
		}
		for (uint32_t f = 0; f < m_.functions.size(); ++f) {
			lowerFunction(f);
		}
		if (M::failed(M::verify(module_))) {
			module_.emitError("module verification error");
			module_->erase();
			throw RuntimeException("mlir: verification of the lowered module failed");
		}
		MLIRNirModule result;
		result.module = M::OwningOpRef<M::ModuleOp>(module_);
		result.proxySymbols = std::move(proxySymbols_);
		result.proxyAddresses = std::move(proxyAddresses_);
		return result;
	}

private:
	M::MLIRContext& ctx_;
	const nir::Module& m_;
	const nirbackend::HostEnvironment& host_;
	const MLIRIntrinsicManager* intrinsics_;
	std::unique_ptr<M::OpBuilder> builderOwner_;
	M::OpBuilder& builder_;
	M::Location loc_;
	M::ModuleOp module_;
	bool resolveNames_ = false;
	bool demangle_ = true;
	bool inlineInvokeCalls_ = false;
	bool normalizeNames_ = false;
	uint32_t normalizedNames_ = 0;

	std::vector<M::func::FuncOp> functions_;
	std::map<ExternalKey, std::string> externalNames_;
	std::unordered_map<std::string, ExternalKey> usedExternalNames_;
	std::vector<std::string> proxySymbols_;
	std::vector<void*> proxyAddresses_;

	// per function
	uint32_t f_ = 0;
	uint32_t curBlock_ = 0;
	std::span<const nir::Inst> insts_;
	std::span<const nir::BlockRecord> blocks_;
	std::vector<M::Value> values_;
	std::vector<M::Block*> mlirBlocks_;
	std::vector<M::Block*> landingPads_;
	std::vector<M::Value> allocaSlots_;
	bool personalitySet_ = false;

	// ---- types ----------------------------------------------------------------------------------------------

	M::Type ptrType() {
		return M::LLVM::LLVMPointerType::get(&ctx_);
	}

	M::Type exceptionType() {
		return M::LLVM::LLVMStructType::getLiteral(&ctx_, {ptrType(), builder_.getI32Type()});
	}

	M::Type mlirType(TypeId t) {
		switch (t) {
		case TypeId::Bool:
			return builder_.getI1Type();
		case TypeId::I8:
			return builder_.getI8Type();
		case TypeId::I16:
			return builder_.getI16Type();
		case TypeId::I32:
			return builder_.getI32Type();
		case TypeId::I64:
			return builder_.getI64Type();
		case TypeId::F32:
			return builder_.getF32Type();
		case TypeId::F64:
			return builder_.getF64Type();
		case TypeId::Ptr:
			return ptrType();
		default:
			if (const auto* mapper = NirExtensionHooks::instance().type(t)) {
				return (*mapper)(ctx_);
			}
			throw NotImplementedException("mlir: no mapping for the extension type #" +
			                              std::to_string(static_cast<unsigned>(t)));
		}
	}

	/// The result types of a signature: empty for `void`.
	llvm::SmallVector<M::Type> resultTypes(const nir::SignatureRecord& sig) {
		llvm::SmallVector<M::Type> types;
		if (nir::typeOf(sig.result) != TypeId::Void) {
			types.push_back(mlirType(nir::typeOf(sig.result)));
		}
		return types;
	}

	llvm::SmallVector<M::Type> paramTypes(const nir::SignatureRecord& sig) {
		llvm::SmallVector<M::Type> types;
		for (uint32_t p = 0; p < sig.paramCount; ++p) {
			types.push_back(mlirType(nir::typeOf(m_.paramType(sig, p))));
		}
		return types;
	}

	/// The ABI attribute of a narrow integer that crosses a native boundary. Types of 32 bits and wider occupy a full
	/// register already and need none as an argument (`narrowOnly`); a result states its extension kind.
	static const char* extensionAttribute(nir::TypeWord word, bool narrowOnly = true) {
		if (narrowOnly && nir::typeOf(word) == TypeId::I32) {
			return nullptr;
		}
		switch (nir::extKindOf(word)) {
		case nir::ExtKind::Zext:
			return "llvm.zeroext";
		case nir::ExtKind::Sext:
			return "llvm.signext";
		case nir::ExtKind::None:
			break;
		}
		return nullptr;
	}

	// ---- values ---------------------------------------------------------------------------------------------

	TypeId typeOfRef(nir::Ref r) const {
		return nir::isConstRef(r) ? static_cast<TypeId>(m_.consts.record(r).type) : static_cast<TypeId>(insts_[r].type);
	}

	M::Value constantOf(TypeId type, uint64_t bits) {
		const auto mt = mlirType(type);
		switch (type) {
		case TypeId::Bool:
			return M::arith::ConstantOp::create(builder_, loc_, mt, builder_.getIntegerAttr(mt, bits & 1));
		case TypeId::I8:
		case TypeId::I16:
		case TypeId::I32:
		case TypeId::I64: {
			const unsigned width = nir::sizeOf(type) * 8;
			const uint64_t mask = width == 64 ? ~uint64_t {0} : (uint64_t {1} << width) - 1;
			return M::arith::ConstantOp::create(builder_, loc_, mt,
			                                    builder_.getIntegerAttr(mt, llvm::APInt(width, bits & mask)));
		}
		case TypeId::F32: {
			// the bit pattern, so that NaN payloads and the sign of zero survive
			llvm::APFloat value(llvm::APFloat::IEEEsingle(), llvm::APInt(32, bits & 0xFFFFFFFFull));
			return M::arith::ConstantOp::create(builder_, loc_, mt, builder_.getFloatAttr(mt, value));
		}
		case TypeId::F64: {
			llvm::APFloat value(llvm::APFloat::IEEEdouble(), llvm::APInt(64, bits));
			return M::arith::ConstantOp::create(builder_, loc_, mt, builder_.getFloatAttr(mt, value));
		}
		default:
			throw NotImplementedException("mlir: a constant of an extension type");
		}
	}

	M::Value val(nir::Ref r) {
		if (nir::isConstRef(r)) {
			const auto& record = m_.consts.record(r);
			return constantOf(static_cast<TypeId>(record.type), record.bits);
		}
		const auto value = values_[r];
		if (!value) {
			throw RuntimeException("mlir: value %" + std::to_string(r) + " is used before it is defined");
		}
		return value;
	}

	M::Value intConstant(TypeId type, int64_t value) {
		return constantOf(type, static_cast<uint64_t>(value));
	}

	M::Value pointerConstant(const void* address) {
		auto constInt = M::arith::ConstantOp::create(
		    builder_, loc_, builder_.getI64Type(),
		    builder_.getIntegerAttr(builder_.getI64Type(), static_cast<int64_t>(reinterpret_cast<intptr_t>(address))));
		return M::LLVM::IntToPtrOp::create(builder_, loc_, ptrType(), M::ValueRange(constInt));
	}

	void bind(uint32_t id, M::Value value) {
		values_[id] = value;
	}

	// ---- functions and externals ----------------------------------------------------------------------------

	static std::string functionSymbol(const nir::Module& m, uint32_t f) {
		return std::string(m.name(m.functions[f].nameId));
	}

	void declareFunction(uint32_t f) {
		const auto& record = m_.functions[f];
		const auto& sig = m_.signatures[record.signature];
		auto type = builder_.getFunctionType(paramTypes(sig), resultTypes(sig));
		auto function = M::func::FuncOp::create(builder_, loc_, functionSymbol(m_, f), type);
		// the type of the C prototype: where the ABI has the caller extend a narrow argument, say so, so that the
		// callee does not extend it again; the result is extended by the callee.
		for (uint32_t p = 0; p < sig.paramCount; ++p) {
			if (const char* attribute = extensionAttribute(m_.paramType(sig, p))) {
				if (callerExtendsNarrowArguments()) {
					function.setArgAttr(p, attribute, M::UnitAttr::get(&ctx_));
				}
			}
		}
		if (nir::typeOf(sig.result) != TypeId::Void) {
			if (const char* attribute = extensionAttribute(sig.result, false)) {
				function.setResultAttr(0, attribute, M::UnitAttr::get(&ctx_));
			}
		}
		functions_.push_back(function);
	}

	static M::LLVM::ModRefInfo modRefOf(uint32_t attrs) {
		using MR = M::LLVM::ModRefInfo;
		const bool noRead = (attrs & nir::AttrNoArgRead) != 0 && (attrs & nir::AttrNoOtherRead) != 0;
		const bool noWrite = (attrs & nir::AttrNoArgWrite) != 0 && (attrs & nir::AttrNoOtherWrite) != 0;
		if (noRead && noWrite) {
			return MR::NoModRef;
		}
		if (noWrite) {
			return MR::Ref;
		}
		if (noRead) {
			return MR::Mod;
		}
		return MR::ModRef;
	}

	/// The name under which the native function at `address` is called in the module.
	std::string externalName(void* address, const ExternalKey& key, uint32_t calleeIndex) {
		std::string name;
		if (inlineInvokeCalls_) {
			// the inlining plugin matches registered bitcode on the name it returns for the address
			if (const auto& hook = getLLVMBackendHooks().callNameOverride) {
				if (auto overridden = hook(address)) {
					name = *overridden;
				}
			}
		}
		if (name.empty() && normalizeNames_) {
			// stable names (`runtimeFunc0`, ...), in the order of the first call, for reference IR
			name = "runtimeFunc" + std::to_string(normalizedNames_++);
		}
		if (name.empty() && resolveNames_) {
			const auto& resolved = tracing::resolveFunctionName(address);
			name = demangle_ ? resolved.demangled : resolved.mangled;
		}
		if (name.empty()) {
			name = "nautilus_ext_" + hexAddress(address);
		}
		// the same name may denote different targets or signatures: keep every declaration its own symbol
		auto used = usedExternalNames_.find(name);
		if (used != usedExternalNames_.end() &&
		    (used->second.address != key.address || used->second.signature != key.signature ||
		     used->second.attrs != key.attrs)) {
			name += "_" + hexAddress(address) + "_c" + std::to_string(calleeIndex);
		}
		usedExternalNames_.emplace(name, key);
		return name;
	}

	/// Declares (once) the native function at `address` with the signature of callee `calleeIndex`, registers it with
	/// the JIT and returns its symbol.
	std::string declareExternal(void* address, uint32_t signatureIndex, uint32_t attrs, uint32_t calleeIndex,
	                            const std::string& forcedName = {}) {
		const ExternalKey key {address, signatureIndex, attrs & nir::GUARANTEE_MASK};
		if (auto it = externalNames_.find(key); it != externalNames_.end()) {
			return it->second;
		}
		const auto& sig = m_.signatures[signatureIndex];
		const auto name = forcedName.empty() ? externalName(address, key, calleeIndex) : forcedName;
		externalNames_.emplace(key, name);

		M::OpBuilder::InsertionGuard guard(builder_);
		builder_.setInsertionPointToStart(module_.getBody());
		auto type = builder_.getFunctionType(paramTypes(sig), resultTypes(sig));
		auto function = M::func::FuncOp::create(builder_, loc_, name, type);
		function.setPrivate();
		// several C ABIs make the caller extend a narrow argument and the native callee relies on it; the result
		// gets no attribute because not every ABI guarantees that the callee extends it
		for (uint32_t p = 0; p < sig.paramCount; ++p) {
			if (const char* attribute = extensionAttribute(m_.paramType(sig, p))) {
				function.setArgAttr(p, attribute, M::UnitAttr::get(&ctx_));
			}
		}
		const auto guarantees = attrs & nir::GUARANTEE_MASK;
		const auto mr = modRefOf(guarantees);
		function->setDiscardableAttr("memory_effects", M::LLVM::MemoryEffectsAttr::get(&ctx_, mr, mr, mr, mr, mr, mr));
		llvm::SmallVector<M::Attribute> passthrough;
		if ((guarantees & nir::AttrWillReturn) != 0) {
			passthrough.push_back(M::StringAttr::get(&ctx_, "willreturn"));
		}
		if ((guarantees & nir::AttrNoUnwind) != 0) {
			passthrough.push_back(M::StringAttr::get(&ctx_, "nounwind"));
		}
		if (!passthrough.empty()) {
			function->setDiscardableAttr("passthrough", M::ArrayAttr::get(&ctx_, passthrough));
		}
		proxySymbols_.push_back(name);
		proxyAddresses_.push_back(address);
		return name;
	}

	// ---- exceptions -----------------------------------------------------------------------------------------

	bool isPad(uint32_t unwind) const {
		return unwind != nir::NO_UNWIND && unwind != nir::NO_CLEANUP;
	}

	void declarePersonality() {
		if (!personalitySet_) {
			functions_[f_]->setDiscardableAttr("personality", M::FlatSymbolRefAttr::get(&ctx_, "__gxx_personality_v0"));
			personalitySet_ = true;
		}
		if (!module_.lookupSymbol<M::LLVM::LLVMFuncOp>("__gxx_personality_v0")) {
			M::OpBuilder::InsertionGuard guard(builder_);
			builder_.setInsertionPointToStart(module_.getBody());
			auto personalityType = M::LLVM::LLVMFunctionType::get(builder_.getI32Type(), {}, true);
			M::LLVM::LLVMFuncOp::create(builder_, module_.getLoc(), "__gxx_personality_v0", personalityType);
		}
	}

	/// The landing-pad block that enters NIR pad `pad`: `llvm.landingpad` (cleanup only), then the body of the pad,
	/// which receives the exception as a block argument. A pad is also entered by `br` from another pad, which is why
	/// the pad body itself is not the unwind destination.
	M::Block* landingPad(uint32_t pad) {
		if (landingPads_[pad] != nullptr) {
			return landingPads_[pad];
		}
		declarePersonality();
		M::OpBuilder::InsertionGuard guard(builder_);
		auto* block = new M::Block();
		functions_[f_].getBody().push_back(block);
		builder_.setInsertionPointToStart(block);
		auto pad0 = M::LLVM::LandingpadOp::create(builder_, loc_, exceptionType(), true, M::ValueRange {});
		M::cf::BranchOp::create(builder_, loc_, mlirBlocks_[pad], M::ValueRange {pad0.getResult()});
		landingPads_[pad] = block;
		return block;
	}

	/// A call of a function symbol: an `invoke` when an exception continues at a pad, a plain call otherwise (the
	/// exception then propagates natively through this frame).
	M::Value emitCall(const std::string& symbol, llvm::ArrayRef<M::Type> results, llvm::ArrayRef<M::Value> args,
	                  uint32_t unwind) {
		if (isPad(unwind)) {
			auto* current = builder_.getInsertionBlock();
			auto* region = current->getParent();
			auto* normal = new M::Block();
			region->getBlocks().insert(std::next(M::Region::iterator(current)), normal);
			auto* unwindBlock = landingPad(unwind);
			auto invoke = M::LLVM::InvokeOp::create(builder_, loc_, results, M::FlatSymbolRefAttr::get(&ctx_, symbol),
			                                        args, normal, M::ValueRange {}, unwindBlock, M::ValueRange {});
			builder_.setInsertionPointToStart(normal);
			return results.empty() ? M::Value() : invoke.getResult();
		}
		auto call = M::func::CallOp::create(builder_, loc_, symbol, M::TypeRange(results), M::ValueRange(args));
		return results.empty() ? M::Value() : call.getResult(0);
	}

	/// The ABI attributes of the arguments of a native call through a pointer: the extension of narrow integers (what
	/// `declareExternal` puts on the parameters of a declaration), after an optional leading pointer. Null if there is
	/// none.
	M::ArrayAttr nativeArgumentAttributes(const nir::SignatureRecord& sig, bool leadingPointer) {
		llvm::SmallVector<M::Attribute> perArgument;
		bool any = false;
		if (leadingPointer) {
			perArgument.push_back(builder_.getDictionaryAttr({}));
		}
		for (uint32_t p = 0; p < sig.paramCount; ++p) {
			if (const char* attribute = extensionAttribute(m_.paramType(sig, p))) {
				perArgument.push_back(builder_.getDictionaryAttr(
				    llvm::ArrayRef<M::NamedAttribute> {builder_.getNamedAttr(attribute, M::UnitAttr::get(&ctx_))}));
				any = true;
			} else {
				perArgument.push_back(builder_.getDictionaryAttr({}));
			}
		}
		return any ? builder_.getArrayAttr(perArgument) : M::ArrayAttr {};
	}

	/// A call of the function at address `callee`. A `leading` argument is passed before `args` (the target of a
	/// thunk); `nativeAbi` states the extension of narrow arguments, for a callee that is native code.
	M::Value emitIndirectCall(M::Value callee, const nir::SignatureRecord& sig, llvm::ArrayRef<M::Value> args,
	                          uint32_t unwind, M::Value leading = M::Value(), bool nativeAbi = false) {
		const auto results = resultTypes(sig);
		M::Type resultType = results.empty() ? static_cast<M::Type>(M::LLVM::LLVMVoidType::get(&ctx_)) : results[0];
		llvm::SmallVector<M::Type> parameters;
		llvm::SmallVector<M::Value> operands;
		operands.push_back(callee);
		if (leading) {
			parameters.push_back(ptrType());
			operands.push_back(leading);
		}
		for (const auto type : paramTypes(sig)) {
			parameters.push_back(type);
		}
		operands.append(args.begin(), args.end());
		auto functionType = M::LLVM::LLVMFunctionType::get(resultType, parameters);
		const M::ArrayAttr argumentAttributes =
		    nativeAbi ? nativeArgumentAttributes(sig, static_cast<bool>(leading)) : M::ArrayAttr {};
		if (isPad(unwind)) {
			auto* current = builder_.getInsertionBlock();
			auto* region = current->getParent();
			auto* normal = new M::Block();
			region->getBlocks().insert(std::next(M::Region::iterator(current)), normal);
			auto* unwindBlock = landingPad(unwind);
			auto invoke = M::LLVM::InvokeOp::create(builder_, loc_, results,
			                                        /*var_callee_type=*/M::TypeAttr {},
			                                        /*callee=*/M::FlatSymbolRefAttr {}, operands,
			                                        /*arg_attrs=*/argumentAttributes,
			                                        /*res_attrs=*/M::ArrayAttr {},
			                                        /*normalDestOperands=*/M::ValueRange {},
			                                        /*unwindDestOperands=*/M::ValueRange {},
			                                        /*branch_weights=*/M::DenseI32ArrayAttr {},
			                                        /*CConv=*/M::LLVM::CConvAttr {},
			                                        /*op_bundle_operands=*/llvm::ArrayRef<M::ValueRange> {},
			                                        /*op_bundle_tags=*/M::ArrayAttr {}, normal, unwindBlock);
			builder_.setInsertionPointToStart(normal);
			return results.empty() ? M::Value() : invoke.getResult();
		}
		auto call = M::LLVM::CallOp::create(builder_, loc_, functionType, operands);
		if (argumentAttributes) {
			call.setArgAttrsAttr(argumentAttributes);
		}
		return results.empty() ? M::Value() : call.getResult();
	}

	/// A call of an exported function of another module (13 §5.7): the entry point of the exporter's current code is
	/// read from the import slot with an acquire load. The exporter's tier decides whether it throws natively or
	/// captures into the exception frame of the thread, and that can change under this code, so a call that may throw
	/// goes through the import's frame thunk: a C++ frame that gives a capturing exporter a frame to write to and
	/// rethrows from it, so that this code only ever sees a native exception.
	M::Value emitImportedCall(const nir::CalleeRecord& callee, const nir::SignatureRecord& sig,
	                          llvm::ArrayRef<M::Value> args, uint32_t unwind) {
		if (host_.imports == nullptr) {
			throw RuntimeException("mlir: the module calls an imported function but was not linked");
		}
		auto slot = pointerConstant(host_.imports->address(callee.target));
		M::Value entry = M::LLVM::LoadOp::create(
		    builder_, loc_, ptrType(), slot,
		    /*alignment=*/sizeof(void*), /*isVolatile=*/false, /*isNonTemporal=*/false,
		    /*isInvariant=*/false, /*isInvariantGroup=*/false, M::LLVM::AtomicOrdering::acquire);
		if (unwind != nir::NO_UNWIND) {
			if (void* thunk = host_.imports->frameThunk(callee.target)) {
				return emitIndirectCall(pointerConstant(thunk), sig, args, unwind, entry, true);
			}
		}
		return emitIndirectCall(entry, sig, args, unwind, M::Value(), true);
	}

	// ---- instructions ---------------------------------------------------------------------------------------

	static unsigned bitWidth(TypeId t) {
		return nir::sizeOf(t) * 8;
	}

	llvm::SmallVector<M::Value> argumentValues(const uint32_t* list, uint32_t first, uint32_t count) {
		llvm::SmallVector<M::Value> values;
		for (uint32_t i = 0; i < count; ++i) {
			values.push_back(val(list[first + i]));
		}
		return values;
	}

	/// The edge arguments of a branch into block `target`: its block arguments, or the exception for a pad.
	llvm::SmallVector<M::Value> edgeArguments(uint32_t target, const uint32_t* list, uint32_t count) {
		if ((blocks_[target].flags & nir::BLOCK_LANDING_PAD) != 0) {
			return {mlirBlocks_[curBlock_]->getArgument(0)};
		}
		return argumentValues(list, 0, count);
	}

	M::Value currentException() {
		return mlirBlocks_[curBlock_]->getArgument(0);
	}

	void lowerInstruction(uint32_t id) {
		const nir::Inst& inst = insts_[id];
		const auto op = static_cast<N>(inst.op);
		const auto type = static_cast<TypeId>(inst.type);
		switch (op) {
		case N::Add:
			return bindBinary<M::arith::AddIOp, M::arith::AddFOp>(id, inst);
		case N::Sub:
			return bindBinary<M::arith::SubIOp, M::arith::SubFOp>(id, inst);
		case N::Mul:
			return bindBinary<M::arith::MulIOp, M::arith::MulFOp>(id, inst);
		case N::Div:
		case N::Rem:
			return lowerDivision(id, inst);
		case N::FNeg:
			bind(id, M::arith::NegFOp::create(builder_, loc_, val(inst.a)));
			return;
		case N::And:
		case N::BAnd:
			bind(id, M::arith::AndIOp::create(builder_, loc_, val(inst.a), val(inst.b)));
			return;
		case N::Or:
		case N::BOr:
			bind(id, M::arith::OrIOp::create(builder_, loc_, val(inst.a), val(inst.b)));
			return;
		case N::BXor:
			bind(id, M::arith::XOrIOp::create(builder_, loc_, val(inst.a), val(inst.b)));
			return;
		case N::Not:
			bind(id, M::arith::XOrIOp::create(builder_, loc_, val(inst.a), intConstant(TypeId::Bool, 1)));
			return;
		case N::BNot:
			bind(id, M::arith::XOrIOp::create(builder_, loc_, val(inst.a), intConstant(type, -1)));
			return;
		case N::Shl:
		case N::Shr:
			return lowerShift(id, inst);
		case N::Cmp:
			return lowerCompare(id, inst);
		case N::Select:
			bind(id, M::arith::SelectOp::create(builder_, loc_, val(inst.a), val(inst.b), val(inst.c)));
			return;
		case N::Cast:
			return lowerCast(id, inst);
		case N::Load:
			return lowerLoad(id, inst);
		case N::Store:
			return lowerStore(inst);
		case N::Alloca:
			bind(id, allocaSlots_.at(inst.a));
			return;
		case N::Call:
			return lowerCall(id, inst);
		case N::CallIndirect: {
			const uint32_t* list = m_.extra.data() + inst.c;
			const auto& sig = m_.signatures[inst.b];
			const auto callee = val(inst.a);
			const auto args = argumentValues(list, 1, list[0]);
			const auto result = emitIndirectCall(callee, sig, args, list[1 + list[0]]);
			if (result) {
				bind(id, result);
			}
			return;
		}
		case N::FuncAddr:
			return lowerFuncAddr(id, inst);
		case N::BindAddr: {
			void* address = host_.bindingAddress ? host_.bindingAddress(m_, inst.a) : nullptr;
			if (address == nullptr) {
				throw RuntimeException("mlir: the host supplies no address for binding #b" + std::to_string(inst.a));
			}
			bind(id, pointerConstant(address));
			return;
		}
		case N::DataAddr: {
			const auto& data = m_.data.at(inst.a);
			bind(id, pointerConstant(m_.dataBytes.data() + data.byteOffset));
			return;
		}
		case N::PtrConst: {
			void* address = inst.a == nir::NONE ? nullptr : nirbackend::resolvePointer(m_, inst.a, host_);
			bind(id, pointerConstant(address));
			return;
		}
		case N::Br: {
			const uint32_t* list = m_.extra.data() + inst.c;
			const auto args = edgeArguments(inst.a, list + 1, list[0]);
			M::cf::BranchOp::create(builder_, loc_, mlirBlocks_[inst.a], args);
			return;
		}
		case N::CondBr:
			return lowerCondBr(inst);
		case N::Ret:
			if (inst.a == nir::NONE) {
				M::func::ReturnOp::create(builder_, loc_, M::ValueRange {});
			} else {
				M::func::ReturnOp::create(builder_, loc_, val(inst.a));
			}
			return;
		case N::Resume:
			M::LLVM::ResumeOp::create(builder_, loc_, currentException());
			return;
		default:
			return lowerExtension(id, inst);
		}
	}

	void lowerExtension(uint32_t id, const nir::Inst& inst) {
		const auto* lowering = NirExtensionHooks::instance().op(inst.op);
		if (lowering == nullptr) {
			throw NotImplementedException("mlir: the extension operation " + std::to_string(inst.op) +
			                              " has no lowering");
		}
		llvm::SmallVector<M::Value> operands;
		for (const nir::Ref ref : {inst.a, inst.b, inst.c}) {
			if (ref != nir::NONE && (nir::isConstRef(ref) || ref < insts_.size())) {
				operands.push_back(val(ref));
			}
		}
		const auto result = (*lowering)(builder_, loc_, m_, inst, operands);
		if (result) {
			bind(id, result);
		}
	}

	/// `ptr` arithmetic (13 §5.1): the `i64` byte offset is added to or subtracted from the address.
	void lowerPointerArithmetic(uint32_t id, const nir::Inst& inst) {
		auto offset = val(inst.b);
		if (static_cast<N>(inst.op) == N::Sub) {
			offset = M::arith::SubIOp::create(builder_, loc_, intConstant(TypeId::I64, 0), offset);
		}
		bind(id, M::LLVM::GEPOp::create(builder_, loc_, ptrType(), builder_.getI8Type(), val(inst.a),
		                                llvm::ArrayRef<M::Value> {offset}));
	}

	template <typename IntOp, typename FloatOp>
	void bindBinary(uint32_t id, const nir::Inst& inst) {
		if (static_cast<TypeId>(inst.type) == TypeId::Ptr) {
			return lowerPointerArithmetic(id, inst);
		}
		if (nir::isFloat(static_cast<TypeId>(inst.type))) {
			bind(id, FloatOp::create(builder_, loc_, val(inst.a), val(inst.b)));
		} else {
			bind(id, IntOp::create(builder_, loc_, val(inst.a), val(inst.b)));
		}
	}

	void lowerShift(uint32_t id, const nir::Inst& inst) {
		const auto type = static_cast<TypeId>(inst.type);
		// the amount is taken modulo the bit width (D10); LLVM would give poison for a larger one
		auto amount = M::arith::AndIOp::create(builder_, loc_, val(inst.b), intConstant(type, bitWidth(type) - 1));
		if (static_cast<N>(inst.op) == N::Shl) {
			bind(id, M::arith::ShLIOp::create(builder_, loc_, val(inst.a), amount));
		} else if ((inst.flags & 1) != 0) {
			bind(id, M::arith::ShRUIOp::create(builder_, loc_, val(inst.a), amount));
		} else {
			bind(id, M::arith::ShRSIOp::create(builder_, loc_, val(inst.a), amount));
		}
	}

	void lowerCompare(uint32_t id, const nir::Inst& inst) {
		const unsigned predicate = inst.flags & 7;
		if (predicate > 5) {
			throw RuntimeException("mlir: an invalid compare predicate");
		}
		const auto type = typeOfRef(inst.a);
		const bool isUnsigned = (inst.flags & 8) != 0;
		if (nir::isFloat(type)) {
			// ordered comparisons are false for NaN; `!=` is true for it, as in C++
			static const M::arith::CmpFPredicate floatPredicates[] = {
			    M::arith::CmpFPredicate::OEQ, M::arith::CmpFPredicate::UNE, M::arith::CmpFPredicate::OLT,
			    M::arith::CmpFPredicate::OLE, M::arith::CmpFPredicate::OGT, M::arith::CmpFPredicate::OGE};
			bind(id, M::arith::CmpFOp::create(builder_, loc_, floatPredicates[predicate], val(inst.a), val(inst.b)));
			return;
		}
		const bool signedOrder = nir::isInteger(type) && !isUnsigned;
		static const M::arith::CmpIPredicate signedPredicates[] = {
		    M::arith::CmpIPredicate::eq,  M::arith::CmpIPredicate::ne,  M::arith::CmpIPredicate::slt,
		    M::arith::CmpIPredicate::sle, M::arith::CmpIPredicate::sgt, M::arith::CmpIPredicate::sge};
		static const M::arith::CmpIPredicate unsignedPredicates[] = {
		    M::arith::CmpIPredicate::eq,  M::arith::CmpIPredicate::ne,  M::arith::CmpIPredicate::ult,
		    M::arith::CmpIPredicate::ule, M::arith::CmpIPredicate::ugt, M::arith::CmpIPredicate::uge};
		if (type == TypeId::Ptr) {
			static const M::LLVM::ICmpPredicate pointerPredicates[] = {
			    M::LLVM::ICmpPredicate::eq,  M::LLVM::ICmpPredicate::ne,  M::LLVM::ICmpPredicate::ult,
			    M::LLVM::ICmpPredicate::ule, M::LLVM::ICmpPredicate::ugt, M::LLVM::ICmpPredicate::uge};
			bind(id, M::LLVM::ICmpOp::create(builder_, loc_, pointerPredicates[predicate], val(inst.a), val(inst.b)));
			return;
		}
		bind(id, M::arith::CmpIOp::create(builder_, loc_,
		                                  signedOrder ? signedPredicates[predicate] : unsignedPredicates[predicate],
		                                  val(inst.a), val(inst.b)));
	}

	/// Throws `nautilus::ArithmeticException` when `failed` holds: a cold block calls the runtime helper like any
	/// call that may throw (13 §5.10), through an `invoke` when the exception continues at a pad.
	void emitArithmeticCheck(M::Value failed, uint32_t unwind) {
		auto* current = builder_.getInsertionBlock();
		auto* region = current->getParent();
		auto* continuation = new M::Block();
		region->getBlocks().insert(std::next(M::Region::iterator(current)), continuation);
		auto* cold = new M::Block();
		region->push_back(cold);
		auto branch =
		    M::cf::CondBranchOp::create(builder_, loc_, failed, cold, M::ValueRange {}, continuation, M::ValueRange {});
		branch.setWeights(llvm::ArrayRef<int32_t> {1, 2000});

		builder_.setInsertionPointToStart(cold);
		const auto symbol = declareThrowArith();
		if (isPad(unwind)) {
			auto* unreachable = new M::Block();
			region->push_back(unreachable);
			auto* unwindBlock = landingPad(unwind);
			M::LLVM::InvokeOp::create(builder_, loc_, llvm::ArrayRef<M::Type> {},
			                          M::FlatSymbolRefAttr::get(&ctx_, symbol), M::ValueRange {}, unreachable,
			                          M::ValueRange {}, unwindBlock, M::ValueRange {});
			builder_.setInsertionPointToStart(unreachable);
		} else {
			M::func::CallOp::create(builder_, loc_, symbol, M::TypeRange {}, M::ValueRange {});
		}
		M::LLVM::UnreachableOp::create(builder_, loc_);
		builder_.setInsertionPointToStart(continuation);
	}

	std::string declareThrowArith() {
		const std::string name = "nautilus_rt_throw_arith";
		if (!module_.lookupSymbol<M::func::FuncOp>(name)) {
			M::OpBuilder::InsertionGuard guard(builder_);
			builder_.setInsertionPointToStart(module_.getBody());
			auto function = M::func::FuncOp::create(builder_, loc_, name, builder_.getFunctionType({}, {}));
			function.setPrivate();
			function->setDiscardableAttr("passthrough", M::ArrayAttr::get(&ctx_, {M::StringAttr::get(&ctx_, "noreturn"),
			                                                                      M::StringAttr::get(&ctx_, "cold")}));
			proxySymbols_.push_back(name);
			proxyAddresses_.push_back(nirbackend::throwArithTarget());
		}
		return name;
	}

	void lowerDivision(uint32_t id, const nir::Inst& inst) {
		const auto type = static_cast<TypeId>(inst.type);
		const bool isDiv = static_cast<N>(inst.op) == N::Div;
		const auto a = val(inst.a);
		const auto b = val(inst.b);
		if (nir::isFloat(type)) {
			if (isDiv) {
				bind(id, M::arith::DivFOp::create(builder_, loc_, a, b));
			} else {
				bind(id, M::arith::RemFOp::create(builder_, loc_, a, b));
			}
			return;
		}
		const bool isUnsigned = (inst.flags & 1) != 0;
		const bool checked = (inst.flags & 2) != 0;
		if (checked) {
			// the divisor is zero, or the signed minimum is divided by -1 (D10)
			M::Value failed =
			    M::arith::CmpIOp::create(builder_, loc_, M::arith::CmpIPredicate::eq, b, intConstant(type, 0));
			if (!isUnsigned) {
				const auto bits = bitWidth(type);
				const int64_t minimum = bits == 64 ? INT64_MIN : -(int64_t {1} << (bits - 1));
				auto isMinimum = M::arith::CmpIOp::create(builder_, loc_, M::arith::CmpIPredicate::eq, a,
				                                          intConstant(type, minimum));
				auto isMinusOne =
				    M::arith::CmpIOp::create(builder_, loc_, M::arith::CmpIPredicate::eq, b, intConstant(type, -1));
				auto overflow = M::arith::AndIOp::create(builder_, loc_, isMinimum, isMinusOne);
				failed = M::arith::OrIOp::create(builder_, loc_, failed, overflow);
			}
			emitArithmeticCheck(failed, inst.c);
		}
		if (isDiv) {
			bind(id, isUnsigned ? static_cast<M::Value>(M::arith::DivUIOp::create(builder_, loc_, a, b))
			                    : static_cast<M::Value>(M::arith::DivSIOp::create(builder_, loc_, a, b)));
		} else {
			bind(id, isUnsigned ? static_cast<M::Value>(M::arith::RemUIOp::create(builder_, loc_, a, b))
			                    : static_cast<M::Value>(M::arith::RemSIOp::create(builder_, loc_, a, b)));
		}
	}

	/// `llvm.fptosi.sat` / `llvm.fptoui.sat`: NaN gives 0, a value out of range the minimum or the maximum (D11).
	M::Value saturatingConversion(bool isSigned, TypeId to, TypeId from, M::Value source) {
		const std::string name = std::string("llvm.") + (isSigned ? "fptosi" : "fptoui") + ".sat.i" +
		                         std::to_string(bitWidth(to)) + (from == TypeId::F32 ? ".f32" : ".f64");
		auto call = M::LLVM::CallIntrinsicOp::create(builder_, loc_, llvm::ArrayRef<M::Type> {mlirType(to)},
		                                             builder_.getStringAttr(name), M::ValueRange {source},
		                                             M::LLVM::FastmathFlagsAttr {});
		return call.getResult(0);
	}

	void lowerCast(uint32_t id, const nir::Inst& inst) {
		const auto kind = inst.flags & 15;
		const auto from = typeOfRef(inst.a);
		const auto to = static_cast<TypeId>(inst.type);
		const auto source = val(inst.a);
		const auto target = mlirType(to);
		switch (kind) {
		case 0: // sext
			bind(id, M::arith::ExtSIOp::create(builder_, loc_, target, source));
			return;
		case 1: // zext
			bind(id, M::arith::ExtUIOp::create(builder_, loc_, target, source));
			return;
		case 2: // trunc
			bind(id, M::arith::TruncIOp::create(builder_, loc_, target, source));
			return;
		case 3: // sitofp
			bind(id, M::arith::SIToFPOp::create(builder_, loc_, target, source));
			return;
		case 4: // uitofp
			bind(id, M::arith::UIToFPOp::create(builder_, loc_, target, source));
			return;
		case 5: // fptosi
		case 6: // fptoui
			bind(id, saturatingConversion(kind == 5, to, from, source));
			return;
		case 7: // fpext
			bind(id, M::arith::ExtFOp::create(builder_, loc_, target, source));
			return;
		case 8: // fptrunc
			bind(id, M::arith::TruncFOp::create(builder_, loc_, target, source));
			return;
		case 9: // bitcast
			bind(id, M::arith::BitcastOp::create(builder_, loc_, target, source));
			return;
		case 10: // ptrtoint
			bind(id, M::LLVM::PtrToIntOp::create(builder_, loc_, target, source));
			return;
		case 11: // inttoptr
			bind(id, M::LLVM::IntToPtrOp::create(builder_, loc_, target, source));
			return;
		default:
			throw RuntimeException("mlir: an invalid cast kind");
		}
	}

	/// The alignment a load or store states: 0 is the natural alignment of the type, k in 1..13 is 2^(k-1) bytes.
	static uint64_t alignmentOf(unsigned flags) {
		const auto code = flags & 15;
		return code == 0 ? 0 : uint64_t {1} << (code - 1);
	}

	void lowerLoad(uint32_t id, const nir::Inst& inst) {
		const auto type = static_cast<TypeId>(inst.type);
		const auto alignment = alignmentOf(inst.flags);
		if (type == TypeId::Bool) {
			// a bool occupies a byte in memory
			auto raw = M::LLVM::LoadOp::create(builder_, loc_, builder_.getI8Type(), val(inst.a));
			if (alignment != 0) {
				raw.setAlignment(alignment);
			}
			bind(id, M::arith::CmpIOp::create(builder_, loc_, M::arith::CmpIPredicate::ne, raw,
			                                  intConstant(TypeId::I8, 0)));
			return;
		}
		auto load = M::LLVM::LoadOp::create(builder_, loc_, mlirType(type), val(inst.a));
		if (alignment != 0) {
			load.setAlignment(alignment);
		}
		bind(id, load);
	}

	void lowerStore(const nir::Inst& inst) {
		const auto alignment = alignmentOf(inst.flags);
		auto value = val(inst.b);
		if (typeOfRef(inst.b) == TypeId::Bool) {
			value = M::arith::ExtUIOp::create(builder_, loc_, builder_.getI8Type(), value);
		}
		auto store = M::LLVM::StoreOp::create(builder_, loc_, value, val(inst.a));
		if (alignment != 0) {
			store.setAlignment(alignment);
		}
	}

	void lowerCondBr(const nir::Inst& inst) {
		const uint32_t* list = m_.extra.data() + inst.c;
		const uint32_t probability = list[0];
		const uint32_t trueArgc = list[1];
		const uint32_t falseBlock = list[2 + trueArgc];
		const uint32_t falseArgc = list[3 + trueArgc];
		const auto condition = val(inst.a);
		const auto trueArgs = argumentValues(list, 2, trueArgc);
		const auto falseArgs = argumentValues(list, 4 + trueArgc, falseArgc);
		auto branch = M::cf::CondBranchOp::create(builder_, loc_, condition, mlirBlocks_[inst.b], trueArgs,
		                                          mlirBlocks_[falseBlock], falseArgs);
		// the branch weights only when the probability is not close to neutral
		if (probability != nir::NONE) {
			const double p = static_cast<double>(probability) / 4294967294.0;
			if (p < 0.4 || p > 0.6) {
				branch.setWeights(
				    llvm::ArrayRef<int32_t> {static_cast<int32_t>(p * 1000), static_cast<int32_t>((1.0 - p) * 1000)});
			}
		}
	}

	/// Expands the call with the handler registered for the native function at `address`, if there is one.
	bool tryHandler(uint32_t id, void* address, llvm::ArrayRef<M::Value> args, llvm::ArrayRef<M::Type> results) {
		if (intrinsics_ == nullptr) {
			return false;
		}
		const auto* handler = intrinsics_->getNirIntrinsic(address);
		if (handler == nullptr) {
			return false;
		}
		NirIntrinsicCall call {builderOwner_, loc_, args, results.empty() ? M::Type() : results[0], M::Value()};
		if (!(*handler)(call)) {
			return false;
		}
		if (call.result) {
			bind(id, call.result);
		}
		return true;
	}

	void lowerCall(uint32_t id, const nir::Inst& inst) {
		const auto& callee = m_.callees.at(inst.a);
		const auto& sig = m_.signatures[callee.signature];
		const uint32_t* list = m_.extra.data() + inst.c;
		const uint32_t argc = list[0];
		const uint32_t unwind = list[1 + argc];
		const auto args = argumentValues(list, 1, argc);
		const auto results = resultTypes(sig);
		std::string symbol;
		switch (static_cast<CalleeKind>(callee.kind)) {
		case CalleeKind::Internal:
			symbol = functionSymbol(m_, callee.target);
			break;
		case CalleeKind::External: {
			void* address = nirbackend::resolvePointer(m_, callee.target, host_);
			// a handler registered by the address of a native function expands calls of it, whether or not the
			// extension registry knows the function as an intrinsic
			if (tryHandler(id, address, args, results)) {
				return;
			}
			symbol = declareExternal(address, callee.signature, callee.attrs, inst.a);
			break;
		}
		case CalleeKind::Intrinsic: {
			const auto* info = ext::ExtensionRegistry::instance().intrinsic(callee.target);
			if (info == nullptr) {
				throw RuntimeException("mlir: an intrinsic callee that is not registered");
			}
			if (tryHandler(id, info->fallback, args, results)) {
				return;
			}
			// no lowering: call the fallback
			symbol = declareExternal(info->fallback, callee.signature, callee.attrs, inst.a);
			break;
		}
		case CalleeKind::Imported: {
			const auto result = emitImportedCall(callee, sig, args, unwind);
			if (result) {
				bind(id, result);
			}
			return;
		}
		}
		const auto result = emitCall(symbol, results, args, unwind);
		if (result) {
			bind(id, result);
		}
	}

	void lowerFuncAddr(uint32_t id, const nir::Inst& inst) {
		const auto& callee = m_.callees.at(inst.a);
		switch (static_cast<CalleeKind>(callee.kind)) {
		case CalleeKind::External:
			bind(id, pointerConstant(nirbackend::resolvePointer(m_, callee.target, host_)));
			return;
		case CalleeKind::Internal: {
			// the function is a func.func of this module: `func.constant` of it, behind a helper function so that
			// convert-func-to-llvm turns it into the address of the llvm.func
			const auto name = functionSymbol(m_, callee.target);
			auto functionType = functions_[callee.target].getFunctionType();
			const auto helper = "__nautilus_fptr_" + name;
			if (!module_.lookupSymbol<M::func::FuncOp>(helper)) {
				M::OpBuilder::InsertionGuard guard(builder_);
				builder_.setInsertionPointToStart(module_.getBody());
				auto helperType = builder_.getFunctionType({}, {functionType});
				auto helperFunction = M::func::FuncOp::create(builder_, module_.getLoc(), helper, helperType);
				helperFunction.setPrivate();
				auto* entry = helperFunction.addEntryBlock();
				builder_.setInsertionPointToStart(entry);
				auto constant = M::func::ConstantOp::create(builder_, module_.getLoc(), functionType, name);
				M::func::ReturnOp::create(builder_, module_.getLoc(), constant.getResult());
			}
			auto call = M::func::CallOp::create(builder_, loc_, helper, M::TypeRange {functionType}, M::ValueRange {});
			auto cast = M::UnrealizedConversionCastOp::create(builder_, loc_, ptrType(), call.getResult(0));
			bind(id, cast.getResult(0));
			return;
		}
		case CalleeKind::Imported:
			// the stub that calls through the import slot (13 §5.7) is not built by any backend yet; nothing in the C++
			// front end takes the address of an imported function
			throw NotImplementedException("mlir: the address of an imported function (funcaddr) is not supported");
		case CalleeKind::Intrinsic:
			throw RuntimeException("mlir: an intrinsic has no address");
		}
	}

	// ---- functions ------------------------------------------------------------------------------------------

	void lowerFunction(uint32_t f) {
		f_ = f;
		insts_ = m_.functionInsts(f);
		blocks_ = m_.functionBlocks(f);
		values_.assign(insts_.size(), M::Value());
		landingPads_.assign(blocks_.size(), nullptr);
		allocaSlots_.clear();
		personalitySet_ = false;
		auto function = functions_[f];
		auto& region = function.getBody();

		auto* entry = function.addEntryBlock();
		mlirBlocks_.assign(blocks_.size(), nullptr);
		mlirBlocks_[0] = entry;
		for (uint32_t p = 0; p < blocks_[0].argCount; ++p) {
			values_[blocks_[0].first + p] = entry->getArgument(p);
		}
		for (uint32_t b = 1; b < blocks_.size(); ++b) {
			auto* block = new M::Block();
			region.push_back(block);
			mlirBlocks_[b] = block;
			if ((blocks_[b].flags & nir::BLOCK_LANDING_PAD) != 0) {
				block->addArgument(exceptionType(), loc_);
				continue;
			}
			for (uint32_t i = 0; i < blocks_[b].argCount; ++i) {
				const auto id = blocks_[b].first + i;
				values_[id] = block->addArgument(mlirType(static_cast<TypeId>(insts_[id].type)), loc_);
			}
		}

		// the static stack slots, in the prologue
		builder_.setInsertionPointToStart(entry);
		auto i8 = builder_.getI8Type();
		for (const auto& slot : m_.functionAllocas(f)) {
			auto size = M::LLVM::ConstantOp::create(builder_, loc_, builder_.getI64Type(),
			                                        builder_.getI64IntegerAttr(std::max<uint32_t>(slot.size, 1)));
			allocaSlots_.push_back(
			    M::LLVM::AllocaOp::create(builder_, loc_, ptrType(), i8, size, std::max<uint32_t>(slot.align, 1)));
		}

		for (uint32_t b = 0; b < blocks_.size(); ++b) {
			curBlock_ = b;
			builder_.setInsertionPointToEnd(mlirBlocks_[b]);
			const auto& block = blocks_[b];
			for (uint32_t k = block.argCount; k < block.count; ++k) {
				lowerInstruction(block.first + k);
			}
		}
	}
};

} // namespace

MLIRNirModule lowerNirToMLIR(M::MLIRContext& context, const nir::Module& module,
                             const nirbackend::HostEnvironment& host, const engine::Options& options,
                             const MLIRIntrinsicManager* intrinsics) {
	return Lowering(context, module, host, options, intrinsics).run();
}

} // namespace nautilus::compiler::mlir

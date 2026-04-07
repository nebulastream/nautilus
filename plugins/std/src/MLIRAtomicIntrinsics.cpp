
#include "MLIRAtomicIntrinsics.hpp"
#include "nautilus/compiler/backends/mlir/intrinsics/MLIRBackendIntrinsic.hpp"
#include <atomic>
#include <cstdint>
#include <functional>
#include <mlir/Dialect/LLVMIR/LLVMAttrs.h>
#include <mlir/Dialect/LLVMIR/LLVMDialect.h>
#include <mlir/Dialect/LLVMIR/LLVMTypes.h>

// Forward-declare thunks defined in atomic.cpp.  Their addresses are the keys the
// MLIR intrinsic registry uses to substitute the runtime call with a real LLVM atomic
// instruction.
extern "C" {

#define NAUTILUS_DECL_PER_TYPE(T)                                                                                      \
	T nautilus_atomic_load_##T##_seqcst(std::atomic<T>*);                                                              \
	T nautilus_atomic_load_##T##_acquire(std::atomic<T>*);                                                             \
	T nautilus_atomic_load_##T##_relaxed(std::atomic<T>*);                                                             \
	void nautilus_atomic_store_##T##_seqcst(std::atomic<T>*, T);                                                       \
	void nautilus_atomic_store_##T##_release(std::atomic<T>*, T);                                                      \
	void nautilus_atomic_store_##T##_relaxed(std::atomic<T>*, T);                                                      \
	T nautilus_atomic_exchange_##T##_seqcst(std::atomic<T>*, T);                                                       \
	T nautilus_atomic_exchange_##T##_relaxed(std::atomic<T>*, T);                                                      \
	T nautilus_atomic_fetch_add_##T##_seqcst(std::atomic<T>*, T);                                                      \
	T nautilus_atomic_fetch_add_##T##_relaxed(std::atomic<T>*, T);                                                     \
	T nautilus_atomic_fetch_sub_##T##_seqcst(std::atomic<T>*, T);                                                      \
	T nautilus_atomic_fetch_sub_##T##_relaxed(std::atomic<T>*, T);                                                     \
	T nautilus_atomic_fetch_and_##T##_seqcst(std::atomic<T>*, T);                                                      \
	T nautilus_atomic_fetch_and_##T##_relaxed(std::atomic<T>*, T);                                                     \
	T nautilus_atomic_fetch_or_##T##_seqcst(std::atomic<T>*, T);                                                       \
	T nautilus_atomic_fetch_or_##T##_relaxed(std::atomic<T>*, T);                                                      \
	T nautilus_atomic_fetch_xor_##T##_seqcst(std::atomic<T>*, T);                                                      \
	T nautilus_atomic_fetch_xor_##T##_relaxed(std::atomic<T>*, T);                                                     \
	T nautilus_atomic_ref_load_##T##_seqcst(T*);                                                                       \
	T nautilus_atomic_ref_load_##T##_acquire(T*);                                                                      \
	T nautilus_atomic_ref_load_##T##_relaxed(T*);                                                                      \
	void nautilus_atomic_ref_store_##T##_seqcst(T*, T);                                                                \
	void nautilus_atomic_ref_store_##T##_release(T*, T);                                                               \
	void nautilus_atomic_ref_store_##T##_relaxed(T*, T);                                                               \
	T nautilus_atomic_ref_exchange_##T##_seqcst(T*, T);                                                                \
	T nautilus_atomic_ref_exchange_##T##_relaxed(T*, T);                                                               \
	T nautilus_atomic_ref_fetch_add_##T##_seqcst(T*, T);                                                               \
	T nautilus_atomic_ref_fetch_add_##T##_relaxed(T*, T);                                                              \
	T nautilus_atomic_ref_fetch_sub_##T##_seqcst(T*, T);                                                               \
	T nautilus_atomic_ref_fetch_sub_##T##_relaxed(T*, T);                                                              \
	T nautilus_atomic_ref_fetch_and_##T##_seqcst(T*, T);                                                               \
	T nautilus_atomic_ref_fetch_and_##T##_relaxed(T*, T);                                                              \
	T nautilus_atomic_ref_fetch_or_##T##_seqcst(T*, T);                                                                \
	T nautilus_atomic_ref_fetch_or_##T##_relaxed(T*, T);                                                               \
	T nautilus_atomic_ref_fetch_xor_##T##_seqcst(T*, T);                                                               \
	T nautilus_atomic_ref_fetch_xor_##T##_relaxed(T*, T);

NAUTILUS_DECL_PER_TYPE(int32_t)
NAUTILUS_DECL_PER_TYPE(int64_t)
NAUTILUS_DECL_PER_TYPE(uint32_t)
NAUTILUS_DECL_PER_TYPE(uint64_t)
#undef NAUTILUS_DECL_PER_TYPE

void nautilus_atomic_thread_fence_seqcst();
void nautilus_atomic_thread_fence_acquire();
void nautilus_atomic_thread_fence_release();
void nautilus_atomic_thread_fence_acq_rel();
void nautilus_atomic_thread_fence_relaxed();

} // extern "C"

namespace nautilus::compiler::mlir {

namespace {

using ::mlir::LLVM::AtomicBinOp;
using ::mlir::LLVM::AtomicOrdering;

using TypeFactory = std::function<::mlir::Type(::mlir::OpBuilder&)>;

::mlir::Type i32(::mlir::OpBuilder& b) {
	return b.getI32Type();
}
::mlir::Type i64(::mlir::OpBuilder& b) {
	return b.getI64Type();
}

IntrinsicFunction makeAtomicLoadLowering(AtomicOrdering ord, TypeFactory tyFn) {
	return [ord, tyFn](std::unique_ptr<::mlir::OpBuilder>& b, const compiler::ir::ProxyCallOperation* call,
	                   MLIRLoweringProvider::ValueFrame& frame) -> bool {
		auto ptr = frame.getValue(call->getInputArguments().at(0)->getIdentifier());
		auto resultTy = tyFn(*b);
		auto op = b->create<::mlir::LLVM::LoadOp>(b->getUnknownLoc(), resultTy, ptr,
		                                          /*alignment=*/0, /*isVolatile=*/false, /*isNonTemporal=*/false,
		                                          /*isInvariant=*/false, /*isInvariantGroup=*/false, ord);
		frame.setValue(call->getIdentifier(), op);
		return true;
	};
}

IntrinsicFunction makeAtomicStoreLowering(AtomicOrdering ord) {
	return [ord](std::unique_ptr<::mlir::OpBuilder>& b, const compiler::ir::ProxyCallOperation* call,
	             MLIRLoweringProvider::ValueFrame& frame) -> bool {
		auto ptr = frame.getValue(call->getInputArguments().at(0)->getIdentifier());
		auto value = frame.getValue(call->getInputArguments().at(1)->getIdentifier());
		b->create<::mlir::LLVM::StoreOp>(b->getUnknownLoc(), value, ptr,
		                                 /*alignment=*/0, /*isVolatile=*/false, /*isNonTemporal=*/false,
		                                 /*isInvariantGroup=*/false, ord);
		return true;
	};
}

IntrinsicFunction makeAtomicRMWLowering(AtomicBinOp binOp, AtomicOrdering ord) {
	return [binOp, ord](std::unique_ptr<::mlir::OpBuilder>& b, const compiler::ir::ProxyCallOperation* call,
	                    MLIRLoweringProvider::ValueFrame& frame) -> bool {
		auto ptr = frame.getValue(call->getInputArguments().at(0)->getIdentifier());
		auto value = frame.getValue(call->getInputArguments().at(1)->getIdentifier());
		auto op = b->create<::mlir::LLVM::AtomicRMWOp>(b->getUnknownLoc(), binOp, ptr, value, ord);
		frame.setValue(call->getIdentifier(), op);
		return true;
	};
}

IntrinsicFunction makeFenceLowering(AtomicOrdering ord) {
	return [ord](std::unique_ptr<::mlir::OpBuilder>& b, const compiler::ir::ProxyCallOperation* /*call*/,
	             MLIRLoweringProvider::ValueFrame& /*frame*/) -> bool {
		b->create<::mlir::LLVM::FenceOp>(b->getUnknownLoc(), ord);
		return true;
	};
}

class MLIRAtomicIntrinsicPlugin : public MLIRIntrinsicPlugin {
public:
	void registerIntrinsics(MLIRIntrinsicManager& manager) override;
	~MLIRAtomicIntrinsicPlugin() override = default;
};

void MLIRAtomicIntrinsicPlugin::registerIntrinsics(MLIRIntrinsicManager& manager) {
#define ADD(target, fn) manager.addIntrinsic(reinterpret_cast<void*>(target), fn)

#define ADD_TYPE(T, tyFn)                                                                                              \
	ADD(&nautilus_atomic_load_##T##_seqcst, makeAtomicLoadLowering(AtomicOrdering::seq_cst, tyFn));                    \
	ADD(&nautilus_atomic_load_##T##_acquire, makeAtomicLoadLowering(AtomicOrdering::acquire, tyFn));                   \
	ADD(&nautilus_atomic_load_##T##_relaxed, makeAtomicLoadLowering(AtomicOrdering::monotonic, tyFn));                 \
	ADD(&nautilus_atomic_store_##T##_seqcst, makeAtomicStoreLowering(AtomicOrdering::seq_cst));                        \
	ADD(&nautilus_atomic_store_##T##_release, makeAtomicStoreLowering(AtomicOrdering::release));                       \
	ADD(&nautilus_atomic_store_##T##_relaxed, makeAtomicStoreLowering(AtomicOrdering::monotonic));                     \
	ADD(&nautilus_atomic_exchange_##T##_seqcst, makeAtomicRMWLowering(AtomicBinOp::xchg, AtomicOrdering::seq_cst));    \
	ADD(&nautilus_atomic_exchange_##T##_relaxed, makeAtomicRMWLowering(AtomicBinOp::xchg, AtomicOrdering::monotonic)); \
	ADD(&nautilus_atomic_fetch_add_##T##_seqcst, makeAtomicRMWLowering(AtomicBinOp::add, AtomicOrdering::seq_cst));    \
	ADD(&nautilus_atomic_fetch_add_##T##_relaxed, makeAtomicRMWLowering(AtomicBinOp::add, AtomicOrdering::monotonic)); \
	ADD(&nautilus_atomic_fetch_sub_##T##_seqcst, makeAtomicRMWLowering(AtomicBinOp::sub, AtomicOrdering::seq_cst));    \
	ADD(&nautilus_atomic_fetch_sub_##T##_relaxed, makeAtomicRMWLowering(AtomicBinOp::sub, AtomicOrdering::monotonic)); \
	ADD(&nautilus_atomic_fetch_and_##T##_seqcst, makeAtomicRMWLowering(AtomicBinOp::_and, AtomicOrdering::seq_cst));   \
	ADD(&nautilus_atomic_fetch_and_##T##_relaxed,                                                                      \
	    makeAtomicRMWLowering(AtomicBinOp::_and, AtomicOrdering::monotonic));                                          \
	ADD(&nautilus_atomic_fetch_or_##T##_seqcst, makeAtomicRMWLowering(AtomicBinOp::_or, AtomicOrdering::seq_cst));     \
	ADD(&nautilus_atomic_fetch_or_##T##_relaxed, makeAtomicRMWLowering(AtomicBinOp::_or, AtomicOrdering::monotonic));  \
	ADD(&nautilus_atomic_fetch_xor_##T##_seqcst, makeAtomicRMWLowering(AtomicBinOp::_xor, AtomicOrdering::seq_cst));   \
	ADD(&nautilus_atomic_fetch_xor_##T##_relaxed, makeAtomicRMWLowering(AtomicBinOp::_xor, AtomicOrdering::monotonic));

#define ADD_REF_TYPE(T, tyFn)                                                                                          \
	ADD(&nautilus_atomic_ref_load_##T##_seqcst, makeAtomicLoadLowering(AtomicOrdering::seq_cst, tyFn));                \
	ADD(&nautilus_atomic_ref_load_##T##_acquire, makeAtomicLoadLowering(AtomicOrdering::acquire, tyFn));               \
	ADD(&nautilus_atomic_ref_load_##T##_relaxed, makeAtomicLoadLowering(AtomicOrdering::monotonic, tyFn));             \
	ADD(&nautilus_atomic_ref_store_##T##_seqcst, makeAtomicStoreLowering(AtomicOrdering::seq_cst));                    \
	ADD(&nautilus_atomic_ref_store_##T##_release, makeAtomicStoreLowering(AtomicOrdering::release));                   \
	ADD(&nautilus_atomic_ref_store_##T##_relaxed, makeAtomicStoreLowering(AtomicOrdering::monotonic));                 \
	ADD(&nautilus_atomic_ref_exchange_##T##_seqcst,                                                                    \
	    makeAtomicRMWLowering(AtomicBinOp::xchg, AtomicOrdering::seq_cst));                                            \
	ADD(&nautilus_atomic_ref_exchange_##T##_relaxed,                                                                   \
	    makeAtomicRMWLowering(AtomicBinOp::xchg, AtomicOrdering::monotonic));                                          \
	ADD(&nautilus_atomic_ref_fetch_add_##T##_seqcst,                                                                   \
	    makeAtomicRMWLowering(AtomicBinOp::add, AtomicOrdering::seq_cst));                                             \
	ADD(&nautilus_atomic_ref_fetch_add_##T##_relaxed,                                                                  \
	    makeAtomicRMWLowering(AtomicBinOp::add, AtomicOrdering::monotonic));                                           \
	ADD(&nautilus_atomic_ref_fetch_sub_##T##_seqcst,                                                                   \
	    makeAtomicRMWLowering(AtomicBinOp::sub, AtomicOrdering::seq_cst));                                             \
	ADD(&nautilus_atomic_ref_fetch_sub_##T##_relaxed,                                                                  \
	    makeAtomicRMWLowering(AtomicBinOp::sub, AtomicOrdering::monotonic));                                           \
	ADD(&nautilus_atomic_ref_fetch_and_##T##_seqcst,                                                                   \
	    makeAtomicRMWLowering(AtomicBinOp::_and, AtomicOrdering::seq_cst));                                            \
	ADD(&nautilus_atomic_ref_fetch_and_##T##_relaxed,                                                                  \
	    makeAtomicRMWLowering(AtomicBinOp::_and, AtomicOrdering::monotonic));                                          \
	ADD(&nautilus_atomic_ref_fetch_or_##T##_seqcst, makeAtomicRMWLowering(AtomicBinOp::_or, AtomicOrdering::seq_cst)); \
	ADD(&nautilus_atomic_ref_fetch_or_##T##_relaxed,                                                                   \
	    makeAtomicRMWLowering(AtomicBinOp::_or, AtomicOrdering::monotonic));                                           \
	ADD(&nautilus_atomic_ref_fetch_xor_##T##_seqcst,                                                                   \
	    makeAtomicRMWLowering(AtomicBinOp::_xor, AtomicOrdering::seq_cst));                                            \
	ADD(&nautilus_atomic_ref_fetch_xor_##T##_relaxed,                                                                  \
	    makeAtomicRMWLowering(AtomicBinOp::_xor, AtomicOrdering::monotonic));

	ADD_TYPE(int32_t, i32)
	ADD_TYPE(int64_t, i64)
	ADD_TYPE(uint32_t, i32)
	ADD_TYPE(uint64_t, i64)
	ADD_REF_TYPE(int32_t, i32)
	ADD_REF_TYPE(int64_t, i64)
	ADD_REF_TYPE(uint32_t, i32)
	ADD_REF_TYPE(uint64_t, i64)
#undef ADD_TYPE
#undef ADD_REF_TYPE

	ADD(&nautilus_atomic_thread_fence_seqcst, makeFenceLowering(AtomicOrdering::seq_cst));
	ADD(&nautilus_atomic_thread_fence_acquire, makeFenceLowering(AtomicOrdering::acquire));
	ADD(&nautilus_atomic_thread_fence_release, makeFenceLowering(AtomicOrdering::release));
	ADD(&nautilus_atomic_thread_fence_acq_rel, makeFenceLowering(AtomicOrdering::acq_rel));
	ADD(&nautilus_atomic_thread_fence_relaxed, makeFenceLowering(AtomicOrdering::monotonic));
#undef ADD
}

} // namespace

void RegisterMLIRAtomicIntrinsicPlugin() {
	MLIRIntrinsicPluginRegistry::instance().addPlugin(std::make_shared<MLIRAtomicIntrinsicPlugin>());
}

} // namespace nautilus::compiler::mlir

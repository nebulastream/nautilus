#include "MLIRPrefetchIntrinsics.hpp"
#include "nautilus/compiler/backends/mlir/intrinsics/MLIRBackendIntrinsic.hpp"
#include "prefetch_impl.hpp"
#include <mlir/Dialect/LLVMIR/LLVMDialect.h>

namespace nautilus::compiler::mlir {

// ============================================================================
// MLIR Prefetch Intrinsic Plugin
//
// Intercepts the nautilus_prefetch_*_impl function pointers and replaces the
// call with LLVM's `llvm.prefetch` intrinsic (mlir::LLVM::Prefetch), so the
// MLIR/LLVM pipeline emits a native prefetch instruction instead of a real
// function call.
//
// `llvm.prefetch(ptr addr, i32 rw, i32 locality, i32 cacheType)` takes three
// compile-time-constant i32 immediates:
//   rw:        0 = read, 1 = write
//   locality:  0 (no reuse expected) .. 3 (high reuse expected)
//   cacheType: 0 = instruction cache, 1 = data cache (always 1 here, since
//              nautilus's prefetch() only ever targets data)
// ============================================================================

class MLIRPrefetchIntrinsicPlugin : public MLIRIntrinsicPlugin {
public:
	void registerIntrinsics(MLIRIntrinsicManager& manager) override;
	~MLIRPrefetchIntrinsicPlugin() override = default;
};

namespace {

constexpr int32_t kDataCache = 1;

template <int32_t Rw, int32_t Locality>
bool prefetchIntrinsic(std::unique_ptr<::mlir::OpBuilder>& builder, const compiler::ir::CallOperation* call,
                       MLIRLoweringProvider::ValueFrame& frame) {
	auto addr = frame.getValue(call->getInputArguments()[0]->getIdentifier());
	::mlir::LLVM::Prefetch::create(*builder, builder->getUnknownLoc(), addr, builder->getI32IntegerAttr(Rw),
	                               builder->getI32IntegerAttr(Locality), builder->getI32IntegerAttr(kDataCache));
	// llvm.prefetch has no result, so there is nothing to bind in the frame.
	return true;
}

} // namespace

void MLIRPrefetchIntrinsicPlugin::registerIntrinsics(MLIRIntrinsicManager& manager) {
	using namespace nautilus::detail;
	manager.addIntrinsic(reinterpret_cast<void*>(&nautilus_prefetch_read_none), prefetchIntrinsic<0, 0>);
	manager.addIntrinsic(reinterpret_cast<void*>(&nautilus_prefetch_read_low), prefetchIntrinsic<0, 1>);
	manager.addIntrinsic(reinterpret_cast<void*>(&nautilus_prefetch_read_moderate), prefetchIntrinsic<0, 2>);
	manager.addIntrinsic(reinterpret_cast<void*>(&nautilus_prefetch_read_high), prefetchIntrinsic<0, 3>);
	manager.addIntrinsic(reinterpret_cast<void*>(&nautilus_prefetch_write_none), prefetchIntrinsic<1, 0>);
	manager.addIntrinsic(reinterpret_cast<void*>(&nautilus_prefetch_write_low), prefetchIntrinsic<1, 1>);
	manager.addIntrinsic(reinterpret_cast<void*>(&nautilus_prefetch_write_moderate), prefetchIntrinsic<1, 2>);
	manager.addIntrinsic(reinterpret_cast<void*>(&nautilus_prefetch_write_high), prefetchIntrinsic<1, 3>);
}

void RegisterMLIRPrefetchIntrinsicPlugin() {
	MLIRIntrinsicPluginRegistry::instance().addPlugin(std::make_shared<MLIRPrefetchIntrinsicPlugin>());
}

} // namespace nautilus::compiler::mlir

#pragma once

#include "../GPULoweringProviderBase.hpp"

namespace nautilus::compiler::metal {

/// Result of Metal lowering: separate device (MSL) and host (C++) code.
struct MetalLoweringResult {
	std::string deviceCode; // .metal kernel source
	std::string hostCode;   // .cpp host source with Metal API dispatch
};

class MetalLoweringProvider {
public:
	static MetalLoweringResult lower(std::shared_ptr<ir::IRGraph> ir, const engine::Options& options);

private:
	/// Generates MSL kernel code.
	class DeviceContext : public gpu::GPULoweringProviderBase<DeviceContext> {
	public:
		explicit DeviceContext(std::shared_ptr<ir::IRGraph> ir);
		Code process();

		static std::string getType(const Type& stamp);
		void processOperation(ir::Operation* operation, short block, gpu::RegisterFrame& frame);
		void processGPUOperation(ir::Operation* opt, short block, gpu::RegisterFrame& frame);

	private:
		void registerGPUIntrinsics();
		void processProxyCall(ir::ProxyCallOperation* opt, short block, gpu::RegisterFrame& frame);

		/// Returns the pointer operations in `func` that address threadgroup
		/// (shared) memory: seeded from sharedArray (nautilus_gpu_shared_alloc)
		/// results and propagated through pointer arithmetic, casts, and
		/// block-argument phis to a fixpoint.
		std::unordered_set<const ir::Operation*> analyzeThreadgroupPtrs(const ir::FunctionOperation& func);

		/// Retags pointer declarations and casts in one function's generated MSL
		/// with the correct address space: `threadgroup` for variables in
		/// `tgVars`, `device` otherwise.
		static std::string rewriteAddressSpaces(const std::string& text, const std::unordered_set<std::string>& tgVars);
	};

	/// Generates C++ host code with Metal API dispatch for kernel launches.
	class HostContext : public gpu::GPULoweringProviderBase<HostContext> {
	public:
		explicit HostContext(std::shared_ptr<ir::IRGraph> ir);
		Code process();

		static std::string getType(const Type& stamp);
		void processOperation(ir::Operation* operation, short block, gpu::RegisterFrame& frame);
		void processGPUOperation(ir::Operation* opt, short block, gpu::RegisterFrame& frame);

	private:
		void registerHostIntrinsics();
		void processProxyCall(ir::ProxyCallOperation* opt, short block, gpu::RegisterFrame& frame);
	};
};

} // namespace nautilus::compiler::metal

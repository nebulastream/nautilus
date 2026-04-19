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
	};

	/// Generates C++ host code with Metal API dispatch for kernel launches.
	class HostContext : public gpu::GPULoweringProviderBase<HostContext> {
	public:
		explicit HostContext(std::shared_ptr<ir::IRGraph> ir);
		Code process(int bufferSize = 4096);

		static std::string getType(const Type& stamp);
		void processOperation(ir::Operation* operation, short block, gpu::RegisterFrame& frame);
		void processGPUOperation(ir::Operation* opt, short block, gpu::RegisterFrame& frame);

	private:
		void registerHostIntrinsics();
		void processProxyCall(ir::ProxyCallOperation* opt, short block, gpu::RegisterFrame& frame);
	};
};

} // namespace nautilus::compiler::metal

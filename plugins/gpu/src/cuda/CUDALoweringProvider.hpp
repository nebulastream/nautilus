#pragma once

#include "../GPULoweringProviderBase.hpp"

namespace nautilus::compiler::cuda {

class CUDALoweringProvider {
public:
	static std::string lower(std::shared_ptr<ir::IRGraph> ir, const engine::Options& options);

private:
	class LoweringContext : public gpu::GPULoweringProviderBase<LoweringContext> {
	public:
		explicit LoweringContext(std::shared_ptr<ir::IRGraph> ir, const engine::Options& options);
		Code process();

		// Called by base class via CRTP
		static std::string getType(const Type& stamp);
		void processOperation(ir::Operation* operation, short block, gpu::RegisterFrame& frame);
		void processGPUOperation(ir::Operation* opt, short block, gpu::RegisterFrame& frame);

	private:
		const engine::Options& options;

		void registerGPUIntrinsics();
		void processProxyCall(ir::ProxyCallOperation* opt, short block, gpu::RegisterFrame& frame);
		void processIndirectCall(ir::IndirectCallOperation* opt, short block, gpu::RegisterFrame& frame);
		void processFunctionAddressOf(ir::FunctionAddressOfOperation* opt, short block, gpu::RegisterFrame& frame);
	};
};

} // namespace nautilus::compiler::cuda

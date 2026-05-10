
#include "CUDACompilationBackend.hpp"
#include "CUDACompiler.hpp"
#include "CUDAExecutable.hpp"
#include "CUDALoweringProvider.hpp"

namespace nautilus::compiler::cuda {

std::unique_ptr<Executable> CUDACompilationBackend::compile(const std::shared_ptr<ir::IRGraph>& ir,
                                                            const DumpHandler& dumpHandler,
                                                            const engine::Options& options,
                                                            CompilationStatistics* /*statistics*/) const {
	auto code = CUDALoweringProvider::lower(ir, options);
	dumpHandler.dump("after_cuda_generation", ".cu", [&]() { return code; });

	std::string identifier = "nautilus_cuda_" + ir->getId();

	auto lib = CUDACompiler::compile(identifier, code, options);
	return std::make_unique<CUDAExecutable>(lib);
}

} // namespace nautilus::compiler::cuda

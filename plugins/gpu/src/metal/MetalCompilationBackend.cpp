
#include "MetalCompilationBackend.hpp"
#include "MetalCompiler.hpp"
#include "MetalExecutable.hpp"
#include "MetalHostCompiler.hpp"
#include "MetalLoweringProvider.hpp"
#include <string>

namespace nautilus::compiler::metal {

std::unique_ptr<Executable> MetalCompilationBackend::compile(const std::shared_ptr<ir::IRGraph>& ir,
                                                             const DumpHandler& dumpHandler,
                                                             const engine::Options& options,
                                                             CompilationStatistics* /*statistics*/) const {
	auto result = MetalLoweringProvider::lower(ir, options);
	dumpHandler.dump("after_metal_generation", ".metal", [&]() { return result.deviceCode; });
	dumpHandler.dump("after_metal_host_generation", ".mm", [&]() { return result.hostCode; });

	std::string identifier = "nautilus_metal_" + ir->getId();

	// 1. Compile device code (.metal -> .metallib)
	auto metallibPath = MetalCompiler::compile(identifier, result.deviceCode);

	// 2. Inject the metallib path into the host code
	auto hostCode = result.hostCode;
	auto pos = hostCode.find("__METALLIB_PATH__");
	if (pos != std::string::npos) {
		hostCode.replace(pos, 17, metallibPath);
	}

	// 3. Compile host code as Objective-C++ with Metal framework
	auto hostLib = MetalHostCompiler::compile(identifier, hostCode);

	// 4. Return executable owning both the host library and metallib
	return std::make_unique<MetalExecutable>(hostLib, metallibPath);
}

} // namespace nautilus::compiler::metal



#include <iostream>
#include <nautilus/compiler/backends/bc/BCInterpreter.hpp>
#include <nautilus/compiler/backends/bc/BCInterpreterBackend.hpp>
#include <nautilus/compiler/backends/bc/BCLoweringProvider.hpp>
#include <nautilus/compiler/backends/bc/ByteCode.hpp>
namespace nautilus::compiler::bc {

std::unique_ptr<Executable> BCInterpreterBackend::compile(const std::shared_ptr<ir::IRGraph>& ir,
                                                          const DumpHandler& dumpHandler, const engine::Options&) {
	auto result = BCLoweringProvider().lower(ir);
	auto code = std::get<0>(result);
	dumpHandler.dump("after_bc_generation", "bc", [&]() { return code.toString(); });

	return std::make_unique<BCInterpreter>(std::get<0>(result), std::get<1>(result));
}

} // namespace nautilus::compiler::bc

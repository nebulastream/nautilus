

#include <nautilus/compiler/backends/bc/BCInterpreter.hpp>
#include <nautilus/compiler/backends/bc/BCInterpreterBackend.hpp>
#include <nautilus/compiler/backends/bc/BCLoweringProvider.hpp>
#include <nautilus/compiler/backends/bc/ByteCode.hpp>

namespace nautilus::compiler::bc {

std::unique_ptr<Executable> BCInterpreterBackend::compile(std::shared_ptr<ir::IRGraph> ir) {

	auto result = BCLoweringProvider().lower(ir);

	auto code = std::get<0>(result);
	// dumpHelper.dump("3. ByteCode.bc", [&]() {
	//     return code.toString();
	// });
	return std::make_unique<BCInterpreter>(std::get<0>(result), std::get<1>(result));
}

} // namespace nautilus::compiler::bc

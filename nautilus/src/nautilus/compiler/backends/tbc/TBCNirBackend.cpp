#include "nautilus/compiler/backends/tbc/TBCNirBackend.hpp"
#include "nautilus/CompilationStatistics.hpp"
#include "nautilus/compiler/backends/tbc/TBCClosure.hpp"
#include "nautilus/compiler/backends/tbc/TBCExecutable.hpp"
#include "nautilus/compiler/backends/tbc/TBCNirLowering.hpp"
#include "nautilus/compiler/backends/tbc/TBCProgramSetup.hpp"
#include "nautilus/exceptions/RuntimeException.hpp"
#include <chrono>

namespace nautilus::compiler::tbc {

std::unique_ptr<Executable> TBCNirBackend::compile(std::shared_ptr<const nir::Module> modulePtr,
                                                   const nirbackend::CompileContext& context) const {
	const auto backendStart = std::chrono::steady_clock::now();
	const nir::Module& module = *modulePtr;
	if (context.cancellation != nullptr && context.cancellation->cancelled()) {
		return nullptr;
	}

	auto program = std::make_shared<TBCProgram>();
	program->owner = modulePtr;
	configureProgram(*program, context.options);

	// every signature and the function index exist before any function is lowered
	program->functions.resize(module.functions.size());
	for (uint32_t i = 0; i < module.functions.size(); ++i) {
		const auto& record = module.functions[i];
		if (record.level != static_cast<uint8_t>(nir::Level::SSA)) {
			throw RuntimeException("tbc: function #" + std::to_string(i) + " is not an SSA function");
		}
		auto& function = program->functions[i];
		function.name = std::string(module.name(record.nameId));
		program->functionIndex[function.name] = i;
		const auto& signature = module.signatures[record.signature];
		function.returnType = tbcTypeOfWord(signature.result);
		for (uint32_t p = 0; p < signature.paramCount; ++p) {
			function.argTypes.push_back(tbcTypeOfWord(module.paramType(signature, p)));
		}
	}
	createClosures(*program);

	int64_t totalInstructions = 0;
	int64_t maxRegisters = 0;
	for (uint32_t i = 0; i < module.functions.size(); ++i) {
		if (context.cancellation != nullptr && context.cancellation->cancelled()) {
			return nullptr;
		}
		auto& function = program->functions[i];
		TBCNirLowering::lower(module, i, *program, function, context.host);
		totalInstructions += static_cast<int64_t>(function.code.size());
		maxRegisters = std::max<int64_t>(maxRegisters, function.regSlots);
	}

	context.dumpHandler.dump("after_tbc_generation", "tbc", [&program]() {
		std::string out;
		for (const auto& function : program->functions) {
			out += function.toString();
			out += "\n";
		}
		return out;
	});

	finishProgram(*program, context.options, context.statistics, totalInstructions, maxRegisters, backendStart);
	return std::make_unique<TBCExecutable>(std::move(program), nirbackend::functionsNeedingCapture(module));
}

} // namespace nautilus::compiler::tbc

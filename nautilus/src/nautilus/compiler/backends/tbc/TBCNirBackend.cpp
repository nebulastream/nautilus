#include "nautilus/compiler/backends/tbc/TBCNirBackend.hpp"
#include "nautilus/CompilationStatistics.hpp"
#include "nautilus/compiler/backends/tbc/TBCClosure.hpp"
#include "nautilus/compiler/backends/tbc/TBCExecutable.hpp"
#include "nautilus/compiler/backends/tbc/TBCNirLowering.hpp"
#include "nautilus/compiler/backends/tbc/TBCProgramSetup.hpp"
#include "nautilus/exceptions/RuntimeException.hpp"
#include <chrono>

namespace nautilus::compiler::tbc {

namespace {

/// The functions that must run under an exception frame: those with a call that may throw or a checked division, and
/// those that reach one through internal calls. Their entry points are reported as captured-host-rethrow.
std::unordered_set<std::string> functionsNeedingCapture(const nir::Module& module) {
	const auto count = static_cast<uint32_t>(module.functions.size());
	std::vector<uint8_t> direct(count, 0);
	std::vector<std::vector<uint32_t>> callees(count);
	for (uint32_t f = 0; f < count; ++f) {
		for (const auto& inst : module.functionInsts(f)) {
			const auto op = static_cast<nir::Op>(inst.op);
			if (op == nir::Op::Div || op == nir::Op::Rem) {
				direct[f] = direct[f] || (inst.flags & 2) != 0;
			} else if (op == nir::Op::Call || op == nir::Op::CallIndirect) {
				const uint32_t* list = module.extra.data() + inst.c;
				const uint32_t argc = list[0];
				direct[f] = direct[f] || list[1 + argc] != nir::NO_UNWIND;
				if (op == nir::Op::Call &&
				    module.callees[inst.a].kind == static_cast<uint8_t>(nir::CalleeKind::Internal)) {
					callees[f].push_back(module.callees[inst.a].target);
				}
			}
		}
	}
	std::vector<uint8_t> needs = direct;
	for (bool changed = true; changed;) {
		changed = false;
		for (uint32_t f = 0; f < count; ++f) {
			if (!needs[f]) {
				for (auto callee : callees[f]) {
					if (needs[callee]) {
						needs[f] = 1;
						changed = true;
						break;
					}
				}
			}
		}
	}
	std::unordered_set<std::string> names;
	for (uint32_t f = 0; f < count; ++f) {
		if (needs[f]) {
			names.insert(std::string(module.name(module.functions[f].nameId)));
		}
	}
	return names;
}

} // namespace

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
	return std::make_unique<TBCExecutable>(std::move(program), functionsNeedingCapture(module));
}

} // namespace nautilus::compiler::tbc

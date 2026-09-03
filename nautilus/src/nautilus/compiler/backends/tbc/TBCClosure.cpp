
#include "nautilus/compiler/backends/tbc/TBCClosure.hpp"
#include "nautilus/compiler/backends/NativeClosure.hpp"
#include "nautilus/compiler/backends/tbc/TBCInterpreter.hpp"
#include <vector>

namespace nautilus::compiler::tbc {

namespace {

/// Closure body for one tbc function: pulls the incoming arguments into raw
/// register slots and enters the VM. `invoke` forks to stitched code by itself
/// when the program was JIT-compiled, so this one body serves both execution
/// modes. The typed argument decoding and return writeback live in the shared
/// NativeClosure component.
uint64_t closureBody(ClosureArgs& args, void* userdata) {
	const auto* binding = static_cast<const TBCClosureBinding*>(userdata);
	const auto& argTypes = binding->program->functions[binding->functionIndex].argTypes;
	const size_t argCount = argTypes.size();

	// Arity is almost always small; keep those calls allocation-free and fall
	// back to the heap only for signatures that exceed the inline capacity.
	constexpr size_t inlineCapacity = 16;
	uint64_t inlineSlots[inlineCapacity];
	std::vector<uint64_t> heapSlots;
	uint64_t* slots = inlineSlots;
	if (argCount > inlineCapacity) {
		heapSlots.resize(argCount);
		slots = heapSlots.data();
	}
	for (size_t i = 0; i < argCount; ++i) {
		slots[i] = args.next(argTypes[i]);
	}
	return invoke(*binding->program, binding->functionIndex, slots, argCount);
}

} // namespace

void createClosures(TBCProgram& program) {
	// Sized once and never resized: each closure stores the address of its
	// binding, so those addresses must stay stable for the program's lifetime.
	program.closureBindings.resize(program.functions.size());
	program.closures.reserve(program.functions.size());
	for (size_t i = 0; i < program.functions.size(); ++i) {
		const auto& function = program.functions[i];
		auto& binding = program.closureBindings[i];
		binding.program = &program;
		binding.functionIndex = static_cast<uint32_t>(i);
		program.closures.push_back(
		    std::make_unique<NativeClosure>(function.argTypes, function.returnType, closureBody, &binding));
	}
}

} // namespace nautilus::compiler::tbc

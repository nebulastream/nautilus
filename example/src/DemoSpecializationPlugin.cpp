// DemoSpecializationPlugin.cpp — showcase the `nautilus-specialization` plugin.
//
// The specialization plugin adds two optimizer hints:
//
//   nautilus_assume(val<bool> cond)
//       Promise to the optimizer that `cond` is true at this program point.
//       If the condition does not hold at runtime, behavior is undefined.
//
//   nautilus_assume_aligned(val<void*> ptr, int alignment)
//       Promise that `ptr` is aligned to `alignment` bytes.
//
// Both hints are plain no-ops unless the MLIR backend is used with the
// intrinsic pass enabled (`mlir.enableIntrinsics = true`). Static init in
// the linked `nautilus-specialization` library registers the intrinsic
// plugin with the MLIR backend automatically.
//
// This demo re-implements conditionalSum (from DemoJit.cpp) twice — once
// plain, once annotated with assume hints — and dumps the generated LLVM IR
// for both. Users can diff the two dumps to see the optimizer exploiting
// the invariants.

#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <nautilus/Engine.hpp>
#include <nautilus/specialization/assume.hpp>
#include <nautilus/val.hpp>

using namespace nautilus;

val<int32_t> conditionalSumPlain(val<int32_t> size, val<bool*> mask, val<int32_t*> array) {
	val<int32_t> sum = 0;
	for (val<int32_t> i = 0; i < size; i++) {
		if (mask[i]) {
			sum += array[i];
		}
	}
	return sum;
}

val<int32_t> conditionalSumHinted(val<int32_t> size, val<bool*> mask, val<int32_t*> array) {
	// Promise: size is strictly positive, so the loop body executes at least once.
	nautilus_assume(size > 0);
	// Promise: both buffers are 32-byte aligned. The caller allocates them with
	// std::aligned_alloc(32, ...), so this invariant is upheld at runtime.
	nautilus_assume_aligned((val<void*>) mask, 32);
	nautilus_assume_aligned((val<void*>) array, 32);

	val<int32_t> sum = 0;
	for (val<int32_t> i = 0; i < size; i++) {
		if (mask[i]) {
			sum += array[i];
		}
	}
	return sum;
}

int main(int, char*[]) {
	engine::Options options;
	// Use the MLIR backend so the intrinsic hints have an effect. If MLIR is
	// not compiled in, fall through — the hints still compile and run as
	// no-ops under the other backends.
	options.setOption("engine.backend", std::string("mlir"));
	options.setOption("mlir.enableIntrinsics", true);
	// Dump the generated LLVM IR for both variants so users can diff them.
	options.setOption("dump.after_llvm_generation", true);

	auto engine = engine::NautilusEngine(options);
	std::cout << "Backend: " << engine.getNameOfBackend() << "\n\n";

	std::cout << "=== Compiling conditionalSumPlain (no hints) ===\n";
	auto plainFn = engine.registerFunction(conditionalSumPlain);
	std::cout << "\n=== Compiling conditionalSumHinted (with assume + assume_aligned) ===\n";
	auto hintedFn = engine.registerFunction(conditionalSumHinted);

	// Allocate 32-byte-aligned buffers so the alignment promise holds.
	constexpr size_t N = 4;
	constexpr size_t ALIGN = 32;
	auto* mask = static_cast<bool*>(std::aligned_alloc(ALIGN, ((sizeof(bool) * N + ALIGN - 1) / ALIGN) * ALIGN));
	auto* array = static_cast<int32_t*>(std::aligned_alloc(ALIGN, ((sizeof(int32_t) * N + ALIGN - 1) / ALIGN) * ALIGN));
	mask[0] = true;
	mask[1] = true;
	mask[2] = false;
	mask[3] = true;
	array[0] = 1;
	array[1] = 2;
	array[2] = 3;
	array[3] = 4;

	auto plainResult = plainFn(static_cast<int32_t>(N), mask, array);
	auto hintedResult = hintedFn(static_cast<int32_t>(N), mask, array);

	std::cout << "\n=== Results ===\n";
	std::cout << "conditionalSumPlain  = " << plainResult << "\n";
	std::cout << "conditionalSumHinted = " << hintedResult << "\n";
	std::cout << "(Both should be 7.)\n";

	std::free(mask);
	std::free(array);
	return 0;
}

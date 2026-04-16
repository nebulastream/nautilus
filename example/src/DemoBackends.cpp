// DemoBackends.cpp — compile the same nautilus function with every backend
// that was enabled at CMake configure time, run it, and print a short
// wall-clock comparison.
//
// Backends selected via options.setOption("engine.backend", "<name>"):
//   "mlir"   — MLIR → LLVM → native (primary, usually fastest)
//   "cpp"    — C++ source generation + system compiler
//   "bc"     — in-process bytecode interpreter
//   "asmjit" — direct x86-64 / ARM64 assembly via asmjit (experimental)
//
// The baseline DemoJit.cpp leaves this option unset, which picks up the
// tiered JIT compiler default — see nautilus/src/nautilus/compiler/Engine.cpp.

#include <chrono>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <nautilus/Engine.hpp>
#include <nautilus/val.hpp>
#include <string>
#include <vector>

using namespace nautilus;

// Sum of squares from 0 to n-1. Simple, loop-heavy, easy to benchmark.
val<int64_t> sumOfSquares(val<int32_t> n) {
	val<int64_t> sum = 0;
	for (val<int32_t> i = 0; i < n; i++) {
		val<int64_t> v = i;
		sum += v * v;
	}
	return sum;
}

static void runWith(const std::string& backendName, int32_t n) {
	engine::Options options;
	options.setOption("engine.backend", backendName);
	// Force single-tier compilation so each run uses exactly the requested
	// backend, instead of the tiered strategy switching between them.
	options.setOption("engine.compilationStrategy", std::string("legacy"));
	// Enable the per-compile statistics report.
	options.setOption("engine.logStatistics", true);
	auto engine = engine::NautilusEngine(options);

	// Measure compile time + first call separately from warm calls.
	auto compileStart = std::chrono::steady_clock::now();
	auto fn = engine.registerFunction(sumOfSquares);
	auto compileEnd = std::chrono::steady_clock::now();

	int64_t result = 0;
	auto runStart = std::chrono::steady_clock::now();
	for (int i = 0; i < 100; i++) {
		result += fn(n);
	}
	auto runEnd = std::chrono::steady_clock::now();

	auto compileMs = std::chrono::duration<double, std::milli>(compileEnd - compileStart).count();
	auto runUs = std::chrono::duration<double, std::micro>(runEnd - runStart).count();

	std::cout << std::left << std::setw(10) << backendName << "  compile=" << std::fixed << std::setprecision(2)
	          << std::setw(8) << compileMs << " ms   100x run=" << std::setw(8) << runUs
	          << " us   result=" << (result / 100) << "\n";
}

int main(int, char*[]) {
	constexpr int32_t N = 10'000;
	std::cout << "Benchmarking sumOfSquares(" << N << ") across compiled-in backends:\n\n";

	// Each backend is only linked in when its CMake flag is ON.
#ifdef ENABLE_MLIR_BACKEND
	runWith("mlir", N);
#endif
#ifdef ENABLE_C_BACKEND
	runWith("cpp", N);
#endif
#ifdef ENABLE_BC_BACKEND
	runWith("bc", N);
#endif
#ifdef ENABLE_ASMJIT_BACKEND
	runWith("asmjit", N);
#endif

	std::cout << "\n(Per-phase compilation stats are logged above by the nautilus library)\n";
	std::cout << "\nExpected result: " << ([] {
		int64_t s = 0;
		for (int64_t i = 0; i < N; i++)
			s += i * i;
		return s;
	})() << "\n";
	return 0;
}

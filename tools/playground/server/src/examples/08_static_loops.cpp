// title: Static loops: unrolling at trace time
// description: static_val loop bounds are plain C++ values — the tracer unrolls the loop, so the IR has no branches at all.
#include <nautilus/Engine.hpp>
#include <nautilus/static.hpp>
#include <nautilus/val.hpp>

using namespace nautilus;

void playground_register(engine::NautilusModule& m) {
	// A regular val<> loop stays a loop: the trace records blocks, a
	// condition, and a back edge (compare with the "Loops" example).
	m.registerFunction<val<int32_t>(val<int32_t>)>("dynamicSum", [](val<int32_t> n) {
		val<int32_t> sum = 0;
		for (val<int32_t> i = 0; i < n; i++) {
			sum += i;
		}
		return sum;
	});

	// A static_val loop is invisible to the tracer: the loop runs in plain
	// C++ *during tracing* and each iteration's body is recorded once.
	// The result is straight-line IR — 8 additions, no blocks, no branches.
	// Check the execution trace and the CFG: there is nothing to jump to.
	m.registerFunction<val<int32_t>(val<int32_t>)>("unrolledPolynomial", [](val<int32_t> x) {
		// Coefficients are ordinary C++ data — never traced, folded straight
		// into the recorded operations as constants.
		const int32_t coefficients[] = {5, -3, 0, 2, 1, -7, 4, 6};

		// Horner's rule, fully unrolled: result = ((c0*x + c1)*x + c2)...
		val<int32_t> result = 0;
		for (static_val<int> i = 0; i < 8; i = i + 1) {
			result = result * x + coefficients[i];
		}
		return result;
	});
}

// Smoke fixture: constant-fed branch — after constant folding, the compare
// becomes a constant and ConstantBranchFolding removes the dead arm, so the
// per-pass tabs show visible IR changes.
#include <nautilus/Engine.hpp>
#include <nautilus/val.hpp>

using namespace nautilus;

void playground_register(engine::NautilusModule& m) {
	m.registerFunction<val<int32_t>(val<int32_t>)>("constantBranch", [](val<int32_t> x) {
		val<int32_t> threshold = 2 + 3; // folds to a constant
		val<int32_t> result = 0;
		if (threshold > 4) { // always true after folding
			result = x + 1;
		} else {
			result = x - 1;
		}
		return result;
	});
}

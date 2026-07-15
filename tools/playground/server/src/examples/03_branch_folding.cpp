// title: Constant branch folding
// description: The condition folds to a constant and a whole branch dies — watch it disappear in the pass tabs.
#include <nautilus/Engine.hpp>
#include <nautilus/val.hpp>

using namespace nautilus;

void playground_register(engine::NautilusModule& m) {
	m.registerFunction<val<int32_t>(val<int32_t>)>("constantBranch", [](val<int32_t> x) {
		val<int32_t> threshold = 2 + 3; // folds to a constant
		val<int32_t> result = 0;
		if (threshold > 4) { // always true after constant folding
			result = x + 1;
		} else {
			result = x - 1; // dead — removed by ConstantBranchFolding
		}
		return result;
	});
}

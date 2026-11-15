// EXPECT-SUCCESS
#include <nautilus/Engine.hpp>

nautilus::val<int> fine(nautilus::val<int> x) {
	return x + 1;
}

void f(const nautilus::engine::NautilusEngine& engine) {
	engine.registerFunction(fine);
}

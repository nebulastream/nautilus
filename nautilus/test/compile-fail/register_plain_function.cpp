// EXPECT-ERROR: registerFunction
#include <nautilus/Engine.hpp>

int plain(int x) {
	return x;
}

void f(const nautilus::engine::NautilusEngine& engine) {
	engine.registerFunction(plain);
}

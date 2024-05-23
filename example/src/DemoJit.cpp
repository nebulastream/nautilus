#include <nautilus/Engine.hpp>
#include <nautilus/val.hpp>
#include <sstream>

using namespace nautilus;

val<int64_t> int64AddExpression(val<int64_t> x) {
	val<int64_t> y = (int64_t) 7;
	return x + y;
}

int main(int, char* []) {
	engine::Options options;
	options.setOption("engine.backend", "mlir");
	auto engine = engine::NautilusEngine(options);
	auto function = engine.registerFunction(int64AddExpression);
	auto result = function(42LL);
	std::cout << "Result: " << result << std::endl;
	return 0;
}
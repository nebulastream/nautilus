#pragma once
#include <functional>
#include <string>
#include <string_view>

namespace nautilus::compiler {

class CompilableFunction {
	using wrapper_function = std::function<void()>;

public:
	CompilableFunction(std::string_view name, wrapper_function function) : name(name), function(function) {
	}

	const std::string& getName() const {
		return name;
	}
	wrapper_function& getFunction() {
		return function;
	}
	const wrapper_function& getFunction() const {
		return function;
	}

private:
	std::string name;
	wrapper_function function;
};

} // namespace nautilus::compiler

#pragma once
#include <functional>
#include <string>
#include <string_view>
#include <unordered_map>

namespace nautilus::compiler {

class CompilableFunction {
	using wrapper_function = std::function<void()>;

public:
	CompilableFunction(std::string_view name, wrapper_function function,
	                   std::unordered_map<std::string, std::string> attributes = {})
	    : name(name), function(function), attributes(std::move(attributes)) {
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
	const std::unordered_map<std::string, std::string>& getAttributes() const {
		return attributes;
	}

private:
	std::string name;
	wrapper_function function;
	std::unordered_map<std::string, std::string> attributes;
};

} // namespace nautilus::compiler

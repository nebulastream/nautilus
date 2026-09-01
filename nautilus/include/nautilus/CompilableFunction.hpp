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
	                   std::unordered_map<std::string, std::string> attributes = {}, const void* definition = nullptr)
	    : name(name), function(function), attributes(std::move(attributes)), definition(definition) {
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

	/// Identity of the NautilusFunctionDefinition this function was traced
	/// from, or nullptr for the entry function (which nobody calls).
	///
	/// The function table binds a traced body to the id minted at its call
	/// sites through this pointer. Binding by name instead would reintroduce
	/// the aliasing bug the table removes: two distinct NautilusFunctions may
	/// share a name.
	const void* getDefinition() const {
		return definition;
	}

private:
	std::string name;
	wrapper_function function;
	std::unordered_map<std::string, std::string> attributes;
	const void* definition = nullptr;
};

} // namespace nautilus::compiler

#pragma once
#include "nautilus/common/RegionAttributes.hpp"
#include "nautilus/tracing/Types.hpp"
#include <functional>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace nautilus::compiler {

class CompilableFunction {
	using wrapper_function = std::function<void()>;

public:
	struct Signature {
		Type returnType;
		std::vector<Type> argumentTypes;

		bool operator==(const Signature&) const = default;
	};

	CompilableFunction(std::string_view name, wrapper_function function,
	                   std::unordered_map<std::string, std::string> attributes = {}, const void* definition = nullptr,
	                   SourceLocation location = {}, std::optional<Signature> signature = std::nullopt)
	    : name(name), function(std::move(function)), attributes(std::move(attributes)), definition(definition),
	      location(location), signature(std::move(signature)) {
	}

	CompilableFunction(std::string_view name, wrapper_function function,
	                   std::unordered_map<std::string, std::string> attributes, std::optional<Signature> signature)
	    : CompilableFunction(name, std::move(function), std::move(attributes), nullptr, {}, std::move(signature)) {
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
	const std::optional<Signature>& getSignature() const {
		return signature;
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

	/// Where this function was registered (docs/engine.md), or an unknown
	/// location for a function queued without one.
	const SourceLocation& getLocation() const {
		return location;
	}

private:
	std::string name;
	wrapper_function function;
	std::unordered_map<std::string, std::string> attributes;
	const void* definition = nullptr;
	SourceLocation location;
	std::optional<Signature> signature;
};

} // namespace nautilus::compiler

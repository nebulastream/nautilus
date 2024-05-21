#pragma once

#include <iostream>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>

namespace nautilus::engine {

using OptionValue = std::variant<int, double, std::string, bool>;

class Options {

public:
	// Set option value
	template <typename T>
	void setOption(const std::string& name, const T& value) {
		options[name] = value;
	}

	template <typename T>
	T getOptionOrDefault(const std::string& name, T defaultValue) const {
		if (options.contains(name)) {
			const OptionValue& val = options.at(name);
			if (std::holds_alternative<T>(val)) {
				return std::get<T>(val);
			}
		}
		return defaultValue;
	}

private:
	std::unordered_map<std::string, OptionValue> options;
};
} // namespace nautilus::engine

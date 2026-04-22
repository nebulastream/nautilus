#pragma once

#include <iostream>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

// Forward-declared so the Nautilus public header does not pull in LLVM.
// Only the MLIR backend dereferences these pointers; if MLIR is disabled,
// the list simply stays empty.
namespace llvm {
class JITEventListener;
} // namespace llvm

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

	// Register a JITEventListener to attach to the MLIR backend's JIT.
	// The caller retains ownership; the listener must outlive every
	// Executable produced by an Engine configured with this Options.
	void addMLIRJitEventListener(llvm::JITEventListener* listener) {
		if (listener != nullptr) {
			mlir_jit_event_listeners_.push_back(listener);
		}
	}

	const std::vector<llvm::JITEventListener*>& getMLIRJitEventListeners() const {
		return mlir_jit_event_listeners_;
	}

private:
	std::unordered_map<std::string, OptionValue> options;
	std::vector<llvm::JITEventListener*> mlir_jit_event_listeners_;
};
} // namespace nautilus::engine

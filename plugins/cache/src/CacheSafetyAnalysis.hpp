#pragma once

#include "nautilus/compiler/ir/passes/IRPass.hpp"
#include <string>
#include <vector>

namespace nautilus::compiler::ir {

class CacheScalarValidationPass final : public IRPass {
public:
	struct Result {
		bool certified = false;
		std::string rejection;
	};

	bool apply(IRGraph& ir) override;

	std::string getName() const override {
		return "cacheScalarValidation";
	}

	[[nodiscard]] const Result& getResult() const {
		return result;
	}

private:
	Result result;
};

class PointerRelocatabilityPass final : public IRPass {
public:
	struct Result {
		bool relocatable = false;
		std::string rejection;
	};

	explicit PointerRelocatabilityPass(std::vector<std::string> exports);

	bool apply(IRGraph& ir) override;

	std::string getName() const override {
		return "pointerRelocatability";
	}

	[[nodiscard]] const Result& getResult() const {
		return result;
	}

private:
	std::vector<std::string> exports;
	Result result;
};

} // namespace nautilus::compiler::ir

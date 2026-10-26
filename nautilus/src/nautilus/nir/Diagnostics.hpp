#pragma once

#include <cstdint>
#include <string>
#include <string_view>

namespace nautilus::nir {

/// The stable diagnostic codes of 13-nir-spec.md §6.6.
enum class DiagCode : uint16_t {
#define NIR_DIAG(id, code, rule, check) id,
#include "nautilus/nir/Diagnostics.def"
#undef NIR_DIAG
};

/// "NIR-V001"
std::string_view codeName(DiagCode c);
/// The spec rule the code checks, "§6.1.1".
std::string_view codeRule(DiagCode c);
/// One-line description of the check.
std::string_view codeCheck(DiagCode c);

/// A diagnostic names the code, the function, and the instruction or block id as printed in the text form (§9).
/// Fields that do not apply are NONE.
struct Diagnostic {
	DiagCode code;
	uint32_t function;
	uint32_t inst;
	uint32_t block;
	std::string message;

	/// "NIR-V005: function 'f', %3: message"
	std::string str(std::string_view functionName = {}) const;
};

} // namespace nautilus::nir

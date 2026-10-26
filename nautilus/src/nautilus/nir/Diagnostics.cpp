#include "nautilus/nir/Diagnostics.hpp"
#include "nautilus/nir/Inst.hpp"

namespace nautilus::nir {

namespace {
struct Entry {
	std::string_view code, rule, check;
};
constexpr Entry entries[] = {
#define NIR_DIAG(id, code, rule, check) {code, rule, check},
#include "nautilus/nir/Diagnostics.def"
#undef NIR_DIAG
};
} // namespace

std::string_view codeName(DiagCode c) {
	return entries[static_cast<size_t>(c)].code;
}
std::string_view codeRule(DiagCode c) {
	return entries[static_cast<size_t>(c)].rule;
}
std::string_view codeCheck(DiagCode c) {
	return entries[static_cast<size_t>(c)].check;
}

std::string Diagnostic::str(std::string_view functionName) const {
	std::string loc;
	auto add = [&](const std::string& part) {
		loc += loc.empty() ? part : ", " + part;
	};
	if (function != NONE) {
		add("function " +
		    (functionName.empty() ? "#" + std::to_string(function) : "'" + std::string(functionName) + "'"));
	}
	if (inst != NONE) {
		add("%" + std::to_string(inst));
	}
	if (block != NONE) {
		add("^b" + std::to_string(block));
	}
	std::string s(codeName(code));
	s += ": ";
	if (!loc.empty()) {
		s += loc + ": ";
	}
	return s + message;
}

} // namespace nautilus::nir

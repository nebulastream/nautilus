#include "nautilus/compiler/ir/FunctionName.hpp"
#include <cctype>

namespace nautilus::compiler::ir {

namespace {

/// Upper bound on a sanitised identifier.  A demangled template instantiation
/// can run to hundreds of characters; the table uniquifies whatever survives
/// the truncation, so shortening can never produce a collision it does not
/// then resolve.
constexpr size_t MAX_IDENTIFIER_LENGTH = 128;

bool isIdentifierChar(unsigned char c) {
	return std::isalnum(c) != 0 || c == '_';
}

} // namespace

std::string sanitizeIdentifier(const std::string& seed) {
	std::string result;
	result.reserve(seed.size() < MAX_IDENTIFIER_LENGTH ? seed.size() : MAX_IDENTIFIER_LENGTH);

	bool lastWasUnderscore = false;
	for (const char rawChar : seed) {
		if (result.size() >= MAX_IDENTIFIER_LENGTH) {
			break;
		}
		const auto c = static_cast<unsigned char>(rawChar);
		if (isIdentifierChar(c)) {
			result.push_back(rawChar);
			lastWasUnderscore = (rawChar == '_');
			continue;
		}
		// Collapse runs of illegal characters into a single separator, so
		// "add(int, int)" becomes "add_int_int" rather than "add_int__int_".
		if (!lastWasUnderscore && !result.empty()) {
			result.push_back('_');
			lastWasUnderscore = true;
		}
	}

	// Trim the trailing separator a signature's closing bracket leaves behind.
	while (!result.empty() && result.back() == '_') {
		result.pop_back();
	}

	if (result.empty()) {
		return result;
	}

	// An identifier may not start with a digit in any of the languages we
	// emit; prefixing keeps the readable part intact.
	if (std::isdigit(static_cast<unsigned char>(result.front())) != 0) {
		result.insert(result.begin(), '_');
	}
	return result;
}

} // namespace nautilus::compiler::ir

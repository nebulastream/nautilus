#pragma once

#include <string>

namespace nautilus::testing {

/**
 * @brief Replaces every external-function reference (`@"symbol" "name"`)
 * in an IR dump with fixed `@"<symbol>" "<name>"` placeholders.
 *
 * The IR writer prints the mangled symbol and demangled name recorded at
 * trace time. Both depend on the build: functions that are not exported
 * from the binary are recorded as raw addresses, and lambda or template
 * names vary between compilers. Normalizing them keeps the checked-in
 * reference dumps deterministic while the rest of the IR text is compared
 * exactly. Trace-format dumps contain no `@"` sequence, so applying the
 * normalization unconditionally is safe.
 */
inline std::string normalizeExternalFunctionReferences(std::string text) {
	std::string out;
	out.reserve(text.size());
	size_t pos = 0;
	// Consumes a `"..."` literal (with backslash escapes) starting at
	// `from`; returns the position past the closing quote or npos.
	auto skipQuoted = [&text](size_t from) -> size_t {
		if (from >= text.size() || text[from] != '"') {
			return std::string::npos;
		}
		for (size_t i = from + 1; i < text.size() && text[i] != '\n'; ++i) {
			if (text[i] == '\\') {
				++i;
			} else if (text[i] == '"') {
				return i + 1;
			}
		}
		return std::string::npos;
	};
	while (pos < text.size()) {
		const size_t at = text.find("@\"", pos);
		if (at == std::string::npos) {
			out.append(text, pos, std::string::npos);
			break;
		}
		const size_t symbolEnd = skipQuoted(at + 1);
		if (symbolEnd == std::string::npos || symbolEnd >= text.size() || text[symbolEnd] != ' ') {
			out.append(text, pos, at + 2 - pos);
			pos = at + 2;
			continue;
		}
		const size_t nameEnd = skipQuoted(symbolEnd + 1);
		if (nameEnd == std::string::npos) {
			out.append(text, pos, at + 2 - pos);
			pos = at + 2;
			continue;
		}
		out.append(text, pos, at - pos);
		out.append("@\"<symbol>\" \"<name>\"");
		pos = nameEnd;
	}
	return out;
}

} // namespace nautilus::testing

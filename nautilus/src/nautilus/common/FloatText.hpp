#pragma once

#include <charconv>
#include <cstdlib>
#include <string>
#include <string_view>

namespace nautilus {

/// Parses a decimal or hexadecimal floating-point literal with round-to-nearest-even, the whole text or nothing.
/// `hex` selects the form after a `0x` prefix, which the caller has removed (`std::from_chars` convention).
///
/// Uses `std::from_chars` where the standard library has the floating-point overloads (`__cpp_lib_to_chars`); on the
/// others (libc++ before LLVM 20, which includes older Apple SDKs) it uses `strtod`/`strtof`, which give the same
/// result for finite literals in the "C" locale.
template <class F>
bool parseFloatText(std::string_view text, F& out, bool hex = false) {
	if (text.empty()) {
		return false;
	}
#if defined(__cpp_lib_to_chars)
	auto r = std::from_chars(text.data(), text.data() + text.size(), out,
	                         hex ? std::chars_format::hex : std::chars_format::general);
	return r.ec == std::errc() && r.ptr == text.data() + text.size();
#else
	std::string s = hex ? "0x" + std::string(text) : std::string(text);
	char* end = nullptr;
	if constexpr (sizeof(F) == sizeof(float)) {
		out = std::strtof(s.c_str(), &end);
	} else {
		out = std::strtod(s.c_str(), &end);
	}
	return end == s.c_str() + s.size() && s.find_first_of(" \t\n") == std::string::npos;
#endif
}

} // namespace nautilus

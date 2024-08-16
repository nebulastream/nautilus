#include "nautilus/std/string.hpp"
#include "nautilus/function.hpp"

namespace nautilus {
template class val<std::basic_string<char, std::char_traits<char>>>;
template class val<std::basic_string<char16_t, std::char_traits<char16_t>>>;
template class val<std::basic_string<char32_t, std::char_traits<char32_t>>>;
template class val<std::basic_string<wchar_t, std::char_traits<wchar_t>>>;
} // namespace nautilus

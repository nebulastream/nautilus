#include "nautilus/std/string_view.h"
#include "nautilus/function.hpp"

namespace nautilus {

template class val<std::basic_string_view<char, std::char_traits<char>>>;

// template
// class val<std::basic_string_view<char8_t, std::char_traits<char8_t>>>;

template class val<std::basic_string_view<char16_t, std::char_traits<char16_t>>>;

template class val<std::basic_string_view<char32_t, std::char_traits<char32_t>>>;

template class val<std::basic_string_view<wchar_t, std::char_traits<wchar_t>>>;
} // namespace nautilus

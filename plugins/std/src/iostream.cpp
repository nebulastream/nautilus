
#include "nautilus/std/iostream.h"
#include "iostream"

namespace nautilus {

std::basic_ostream<char, std::char_traits<char>>* getCout() noexcept {
	return &std::cout;
}

std::basic_ostream<char, std::char_traits<char>>* getCerr() noexcept {
	return &std::cerr;
}

} // namespace nautilus

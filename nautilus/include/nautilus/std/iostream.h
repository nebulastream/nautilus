#pragma once
#include "nautilus/function.hpp"
#include "ostream.hpp"
namespace nautilus {

std::basic_ostream<char, std::char_traits<char>>* getCout();
std::basic_ostream<char, std::char_traits<char>>* getCerr();

class cout_helper {
public:
	template <class T>
	val<std::basic_ostream<char, std::char_traits<char>>> operator<<(T&& value) {
		auto stream = val<std::basic_ostream<char, std::char_traits<char>>>(invoke(getCout));
		stream << value;
		return stream;
	}
};

class cerr_helper {
public:
	template <class T>
	val<std::basic_ostream<char, std::char_traits<char>>> operator<<(T&& value) {
		auto stream = val<std::basic_ostream<char, std::char_traits<char>>>(invoke(getCerr));
		stream << value;
		return stream;
	}
};


[[maybe_unused]] static cout_helper cout;
[[maybe_unused]] static cerr_helper cerr;

} // namespace nautilus

#pragma once

#include "nautilus/val_ptr.hpp"
#include <string>

namespace nautilus {
template <class CharT, class Traits>
class val<std::basic_string<CharT, Traits>> {
	using base_type = std::basic_string<CharT, Traits>;
	using value_type = base_type::value_type;
	using size_type = base_type::size_type;
	using const_reference = base_type::const_reference;
	using iterator = base_type::iterator;
	using const_iterator = base_type::const_iterator;
	using reference = base_type::reference;
	using pointer = base_type::pointer;
	using const_pointer = base_type::const_pointer;

public:
	val<std::basic_string<CharT, Traits>>(val<const CharT*> s) : data_ptr(nullptr) {
		data_ptr = invoke(+[](const CharT* s) -> base_type* { return new base_type(s); }, s);
	}

	~val<std::basic_string<CharT, Traits>>() {
		invoke(+[](base_type* ptr) -> void { delete ptr; }, data_ptr);
	}

private:
	val<base_type*> data_ptr;
};
} // namespace nautilus

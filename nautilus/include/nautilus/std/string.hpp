#pragma once

#include "nautilus/val_ptr.hpp"
#include "nautilus/std/string_view.hpp"
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
	val<std::basic_string<CharT, Traits>>(val<std::basic_string<CharT, Traits>*> str) : data_ptr(str) {
	}
	val<std::basic_string<CharT, Traits>>(val<const CharT*> s) : data_ptr(nullptr) {
		data_ptr = invoke(+[](const CharT* s) -> base_type* { return new base_type(s); }, s);
	}

	val<CharT> at(val<size_type> pos) {
		return invoke(+[](base_type* ptr, size_type p) -> CharT { return ptr->at(p); }, data_ptr, pos);
	}

	/**
	 * Returns a reference to the character at specified location pos if pos < size(),
	 * or a reference to CharT() if pos == size().
	 * No bounds checking is performed.
	 * If pos > size(), the behavior is undefined.
	 */
	val<CharT> operator[](val<size_type> pos) {
		return invoke(+[](base_type* ptr, size_type p) -> CharT { return ptr->operator[](p); }, data_ptr, pos);
	}

	val<CharT> front() {
		return invoke(+[](base_type* ptr) -> CharT { return ptr->front(); }, data_ptr);
	}

	val<CharT> back() {
		return invoke(+[](base_type* ptr) -> CharT { return ptr->back(); }, data_ptr);
	}

	val<CharT*> data() {
		return invoke(+[](base_type* ptr) -> CharT* { return ptr->data(); }, data_ptr);
	}

	/**
	 * Returns a pointer to a null-terminated character array with data equivalent to those stored in the string.
	 * The pointer is such that the range [c_str(), c_str() + size()]
	 * is valid and the values in it correspond to the values stored in the string with an additional null
	 * character after the last position.
	 */
	val<const CharT*> c_str() const {
		return invoke(+[](base_type* ptr) -> const CharT* { return ptr->c_str(); }, data_ptr);
	}

	operator val<std::basic_string_view<CharT, Traits>>() const {
		return val<std::basic_string_view<CharT, Traits>>(c_str());
	}

	val<bool> empty() const {
		return invoke(+[](base_type* ptr) -> bool { return ptr->empty(); }, data_ptr);
	}

	/**
	 * Returns the number of CharT elements in the string, i.e. std::distance(begin(), end()).
	 * @return
	 */
	val<size_type> size() const {
		return invoke(+[](base_type* ptr) -> size_type { return ptr->size(); }, data_ptr);
	}

	/**
	 * Returns the number of CharT elements in the string, i.e. std::distance(begin(), end()).
	 * @return
	 */
	val<size_type> length() const {
		return invoke(+[](base_type* ptr) -> size_type { return ptr->length(); }, data_ptr);
	}

	/**
	 * Returns the maximum number of characters
	 * @return
	 */
	val<size_type> max_size() const {
		return invoke(+[](base_type* ptr) -> size_type { return ptr->max_size(); }, data_ptr);
	}



	/**
	 * Informs a std::basic_string object of a planned change in size, so that it can manage the storage allocation appropriately.
	 * @param new_cap
	 */
	void reserve(val<size_type> new_cap) const {
		return invoke(+[](base_type* ptr, size_type s) -> void { return ptr->reserve(s); }, data_ptr, new_cap);
	}

	/**
	 * Returns the number of characters that can be held in currently allocated storage.
	 * @return
	 */
	val<size_type> capacity() const {
		return invoke(+[](base_type* ptr) -> size_type { return ptr->capacity(); }, data_ptr);
	}

	~val<std::basic_string<CharT, Traits>>() {
		invoke(+[](base_type* ptr) -> void { delete ptr; }, data_ptr);
	}

private:
	val<base_type*> data_ptr;
};
} // namespace nautilus

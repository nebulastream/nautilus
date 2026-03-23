#pragma once

#include "nautilus/std/string_view.h"
#include "nautilus/val_ptr.hpp"
#include <string>

namespace nautilus {
template <class CharT, class Traits>
class val<std::basic_string<CharT, Traits>> {
	using base_type = std::basic_string<CharT, Traits>;
	using value_type = typename base_type::value_type;
	using size_type = typename base_type::size_type;
	using const_reference = typename base_type::const_reference;
	using iterator = typename base_type::iterator;
	using const_iterator = typename base_type::const_iterator;
	using reference = typename base_type::reference;
	using pointer = typename base_type::pointer;
	using const_pointer = typename base_type::const_pointer;

public:
	val() : data_ptr(nullptr) {
		data_ptr = invoke(+[]() -> base_type* { return new base_type(); });
	}
	val(val<std::basic_string<CharT, Traits>*> str) : data_ptr(str) {
	}
	val(const val<const CharT*>& s) : data_ptr(nullptr) {
		data_ptr = invoke(
		    +[](const CharT* s) -> base_type* { return new base_type(s); }, s);
	}

	val(const CharT* s) : val(val<const CharT*>(s)) {
	}

	val<std::basic_string<CharT, Traits>>& operator=(const val<std::basic_string<CharT, Traits>>& other) {
		this->data_ptr = other.data_ptr;
		return *this;
	}

	val<CharT> at(val<size_type> pos) {
		return invoke(
		    +[](base_type* ptr, size_type p) -> CharT { return ptr->at(p); }, data_ptr, pos);
	}

	/**
	 * Returns a reference to the character at specified location pos if pos < size(),
	 * or a reference to CharT() if pos == size().
	 * No bounds checking is performed.
	 * If pos > size(), the behavior is undefined.
	 */
	val<CharT> operator[](val<size_type> pos) {
		return invoke(
		    +[](base_type* ptr, size_type p) -> CharT { return ptr->operator[](p); }, data_ptr, pos);
	}

	val<CharT> front() {
		return invoke(
		    +[](base_type* ptr) -> CharT { return ptr->front(); }, data_ptr);
	}

	val<CharT> back() {
		return invoke(
		    +[](base_type* ptr) -> CharT { return ptr->back(); }, data_ptr);
	}

	val<CharT*> data() {
		return invoke(
		    +[](base_type* ptr) -> CharT* { return ptr->data(); }, data_ptr);
	}

	/**
	 * Returns a pointer to a null-terminated character array with data equivalent to those stored in the string.
	 * The pointer is such that the range [c_str(), c_str() + size()]
	 * is valid and the values in it correspond to the values stored in the string with an additional null
	 * character after the last position.
	 */
	val<const CharT*> c_str() const {
		return invoke(
		    +[](base_type* ptr) -> const CharT* { return ptr->c_str(); }, data_ptr);
	}

	operator val<std::basic_string_view<CharT, Traits>>() const {
		return val<std::basic_string_view<CharT, Traits>>(c_str());
	}

	val<bool> empty() const {
		return invoke(
		    +[](base_type* ptr) -> bool { return ptr->empty(); }, data_ptr);
	}

	/**
	 * Returns the number of CharT elements in the string, i.e. std::distance(begin(), end()).
	 * @return
	 */
	val<size_type> size() const {
		return invoke(
		    +[](base_type* ptr) -> size_type { return ptr->size(); }, data_ptr);
	}

	/**
	 * Returns the number of CharT elements in the string, i.e. std::distance(begin(), end()).
	 * @return
	 */
	val<size_type> length() const {
		return invoke(
		    +[](base_type* ptr) -> size_type { return ptr->length(); }, data_ptr);
	}

	/**
	 * Returns the maximum number of characters
	 * @return
	 */
	val<size_type> max_size() const {
		return invoke(
		    +[](base_type* ptr) -> size_type { return ptr->max_size(); }, data_ptr);
	}

	/**
	 * Informs a std::basic_string object of a planned change in size, so that it can manage the storage allocation
	 * appropriately.
	 * @param new_cap
	 */
	void reserve(val<size_type> new_cap) const {
		return invoke(
		    +[](base_type* ptr, size_type s) -> void { return ptr->reserve(s); }, data_ptr, new_cap);
	}

	/**
	 * Returns the number of characters that can be held in currently allocated storage.
	 * @return
	 */
	val<size_type> capacity() const {
		return invoke(
		    +[](base_type* ptr) -> size_type { return ptr->capacity(); }, data_ptr);
	}

	/**
	 * Removes all characters from the string as if by executing erase(begin(), end()).
	 * All pointers, references, and iterators are invalidated.
	 * @return
	 */
	void clear() const {
		invoke(
		    +[](base_type* ptr) -> void { ptr->clear(); }, data_ptr);
	}

	/**
	 * Inserts characters into the string.
	 */
	auto& insert(val<size_type> index, val<size_type> count, val<CharT> ch) const {
		invoke(
		    +[](base_type* ptr, size_type index, size_type count, CharT ch) -> void { ptr->insert(index, count, ch); },
		    data_ptr, index, count, ch);
		return *this;
	}

	/**
	 * Inserts characters into the string.
	 */
	auto& insert(val<size_type> index, val<const CharT*> s) const {
		invoke(
		    +[](base_type* ptr, size_type index, const CharT* s) -> void { ptr->insert(index, s); }, data_ptr, index,
		    s);
		return *this;
	}

	/**
	 * Appends additional characters to the string.
	 */
	auto& append(val<size_type> count, val<CharT> ch) {
		invoke(
		    +[](base_type* ptr, size_type count, CharT ch) -> void { ptr->append(count, ch); }, data_ptr, count, ch);
		return *this;
	}

	/**
	 * Appends additional characters to the string.
	 */
	auto& append(val<std::basic_string<CharT, Traits>>& str) {
		invoke(
		    +[](base_type* ptr, base_type* other) -> void { ptr->append(*other); }, data_ptr, str.data_ptr);
		return *this;
	}

	/**
	 * Appends additional characters to the string.
	 */
	auto& append(const val<std::basic_string<CharT, Traits>>& str, const val<size_type>& pos,
	             const val<size_type>& count) {
		invoke(
		    +[](base_type* ptr, base_type* other, size_type pos, size_type count) -> void {
			    ptr->append(*other, pos, count);
		    },
		    data_ptr, str.data_ptr, pos, count);
		return *this;
	}

	/**
	 * Appends additional characters to the string.
	 */
	auto& append(const val<std::basic_string<CharT, Traits>>& str, const val<size_type>& count) {
		invoke(
		    +[](base_type* ptr, base_type* other, size_type count) -> void { ptr->append(*other, count); }, data_ptr,
		    str.data_ptr, count);
		return *this;
	}

	/**
	 * Appends additional characters to the string.
	 */
	auto& append(const val<CharT*>& s, const val<size_type>& count) {
		invoke(
		    +[](base_type* ptr, CharT* s, size_type count) -> void { ptr->append(s, count); }, data_ptr, s, count);
		return *this;
	}

	/**
	 * Appends additional characters to the string.
	 */
	auto& append(const val<const CharT*>& s) {
		invoke(
		    +[](base_type* ptr, const CharT* s) -> void { ptr->append(s); }, data_ptr, s);
		return *this;
	}

	/**
	 * Appends additional characters to the string.
	 */
	auto& operator+=(const val<std::basic_string<CharT, Traits>>& str) {
		invoke(
		    +[](base_type* ptr, base_type* s) -> void { ptr->operator+=(*s); }, data_ptr, str.data_ptr);
		return *this;
	}

	/**
	 * Appends additional characters to the string.
	 */
	auto& operator+=(const val<const CharT*>& s) {
		invoke(
		    +[](base_type* ptr, const CharT* s) -> void { ptr->operator+=(s); }, data_ptr, s);
		return *this;
	}

	auto& operator+=(const val<CharT*>& s) {
		invoke(
		    +[](base_type* ptr, const CharT* s) -> void { ptr->operator+=(s); }, data_ptr, s);
		return *this;
	}

	auto& operator+=(const CharT* s) {
		return this->operator+=(val<const CharT*>(s));
	}

	/**
	 * Copies a substring [pos, pos + count) to character string pointed to by dest. If the requested substring lasts
	 * past the end of the string, or if count == npos, the copied substring is [pos, size()).
	 */
	val<size_type> copy(const val<CharT*>& dest, const val<size_type>& count, const val<size_type>& pos = 0) {
		return invoke(
		    +[](base_type* ptr, CharT* dest, size_type count, size_type pos) -> size_type {
			    return ptr->copy(dest, count, pos);
		    },
		    data_ptr, dest, count, pos);
	}

	/**
	 * Returns a newly constructed string object with its value initialized to a copy of a substring of this string.
	 */
	val<std::basic_string<CharT, Traits>> substr(val<size_type> pos = 0, val<size_type> count = base_type::npos) {
		auto ptr = invoke(
		    +[](base_type* ptr, size_type pos, size_type count) -> base_type* {
			    return new base_type(ptr->substr(pos, count));
		    },
		    data_ptr, pos, count);
		return val<std::basic_string<CharT, Traits>>(ptr);
	}

	/**
	 * Finds the first substring equal to the given character sequence.
	 */
	val<size_type> find(const val<const CharT*>& s, val<size_type> pos = 0) const {
		return invoke(
		    +[](base_type* ptr, const CharT* s, size_type pos) -> size_type { return ptr->find(s, pos); }, data_ptr, s,
		    pos);
	}

	val<size_type> find(val<CharT> ch, val<size_type> pos = 0) const {
		return invoke(
		    +[](base_type* ptr, CharT ch, size_type pos) -> size_type { return ptr->find(ch, pos); }, data_ptr, ch,
		    pos);
	}

	/**
	 * Finds the last substring equal to the given character sequence.
	 */
	val<size_type> rfind(const val<const CharT*>& s, val<size_type> pos = base_type::npos) const {
		return invoke(
		    +[](base_type* ptr, const CharT* s, size_type pos) -> size_type { return ptr->rfind(s, pos); }, data_ptr, s,
		    pos);
	}

	val<size_type> rfind(val<CharT> ch, val<size_type> pos = base_type::npos) const {
		return invoke(
		    +[](base_type* ptr, CharT ch, size_type pos) -> size_type { return ptr->rfind(ch, pos); }, data_ptr, ch,
		    pos);
	}

	/**
	 * Finds the first character equal to one of the characters in the given character sequence.
	 */
	val<size_type> find_first_of(const val<const CharT*>& s, val<size_type> pos = 0) const {
		return invoke(
		    +[](base_type* ptr, const CharT* s, size_type pos) -> size_type { return ptr->find_first_of(s, pos); },
		    data_ptr, s, pos);
	}

	val<size_type> find_first_of(val<CharT> ch, val<size_type> pos = 0) const {
		return invoke(
		    +[](base_type* ptr, CharT ch, size_type pos) -> size_type { return ptr->find_first_of(ch, pos); }, data_ptr,
		    ch, pos);
	}

	/**
	 * Finds the last character equal to one of the characters in the given character sequence.
	 */
	val<size_type> find_last_of(const val<const CharT*>& s, val<size_type> pos = base_type::npos) const {
		return invoke(
		    +[](base_type* ptr, const CharT* s, size_type pos) -> size_type { return ptr->find_last_of(s, pos); },
		    data_ptr, s, pos);
	}

	val<size_type> find_last_of(val<CharT> ch, val<size_type> pos = base_type::npos) const {
		return invoke(
		    +[](base_type* ptr, CharT ch, size_type pos) -> size_type { return ptr->find_last_of(ch, pos); }, data_ptr,
		    ch, pos);
	}

	/**
	 * Finds the first character not equal to any of the characters in the given character sequence.
	 */
	val<size_type> find_first_not_of(const val<const CharT*>& s, val<size_type> pos = 0) const {
		return invoke(
		    +[](base_type* ptr, const CharT* s, size_type pos) -> size_type { return ptr->find_first_not_of(s, pos); },
		    data_ptr, s, pos);
	}

	val<size_type> find_first_not_of(val<CharT> ch, val<size_type> pos = 0) const {
		return invoke(
		    +[](base_type* ptr, CharT ch, size_type pos) -> size_type { return ptr->find_first_not_of(ch, pos); },
		    data_ptr, ch, pos);
	}

	/**
	 * Finds the last character not equal to any of the characters in the given character sequence.
	 */
	val<size_type> find_last_not_of(const val<const CharT*>& s, val<size_type> pos = base_type::npos) const {
		return invoke(
		    +[](base_type* ptr, const CharT* s, size_type pos) -> size_type { return ptr->find_last_not_of(s, pos); },
		    data_ptr, s, pos);
	}

	val<size_type> find_last_not_of(val<CharT> ch, val<size_type> pos = base_type::npos) const {
		return invoke(
		    +[](base_type* ptr, CharT ch, size_type pos) -> size_type { return ptr->find_last_not_of(ch, pos); },
		    data_ptr, ch, pos);
	}

	/**
	 * Removes count characters starting at index.
	 */
	auto& erase(val<size_type> index = 0, val<size_type> count = base_type::npos) {
		invoke(
		    +[](base_type* ptr, size_type index, size_type count) -> void { ptr->erase(index, count); }, data_ptr, index,
		    count);
		return *this;
	}

	/**
	 * Replaces the characters in the range [pos, pos + count) with the given string.
	 */
	auto& replace(val<size_type> pos, val<size_type> count, const val<const CharT*>& s) {
		invoke(
		    +[](base_type* ptr, size_type pos, size_type count, const CharT* s) -> void { ptr->replace(pos, count, s); },
		    data_ptr, pos, count, s);
		return *this;
	}

	/**
	 * Compares two strings.
	 */
	val<int> compare(const val<std::basic_string<CharT, Traits>>& str) const {
		return invoke(
		    +[](base_type* ptr, base_type* other) -> int { return ptr->compare(*other); }, data_ptr, str.data_ptr);
	}

	val<int> compare(const val<const CharT*>& s) const {
		return invoke(
		    +[](base_type* ptr, const CharT* s) -> int { return ptr->compare(s); }, data_ptr, s);
	}

#ifdef __cpp_lib_starts_ends_with
	val<bool> starts_with(const val<const CharT*>& s) const {
		return invoke(
		    +[](base_type* ptr, const CharT* s) -> bool { return ptr->starts_with(s); }, data_ptr, s);
	}

	val<bool> starts_with(val<CharT> ch) const {
		return invoke(
		    +[](base_type* ptr, CharT ch) -> bool { return ptr->starts_with(ch); }, data_ptr, ch);
	}

	val<bool> ends_with(const val<const CharT*>& s) const {
		return invoke(
		    +[](base_type* ptr, const CharT* s) -> bool { return ptr->ends_with(s); }, data_ptr, s);
	}

	val<bool> ends_with(val<CharT> ch) const {
		return invoke(
		    +[](base_type* ptr, CharT ch) -> bool { return ptr->ends_with(ch); }, data_ptr, ch);
	}
#endif

#ifdef __cpp_lib_string_contains
	val<bool> contains(const val<const CharT*>& s) const {
		return invoke(
		    +[](base_type* ptr, const CharT* s) -> bool { return ptr->contains(s); }, data_ptr, s);
	}

	val<bool> contains(val<CharT> ch) const {
		return invoke(
		    +[](base_type* ptr, CharT ch) -> bool { return ptr->contains(ch); }, data_ptr, ch);
	}
#endif

	void push_back(val<CharT> ch) {
		invoke(
		    +[](base_type* ptr, CharT ch) -> void { ptr->push_back(ch); }, data_ptr, ch);
	}

	void pop_back() {
		invoke(
		    +[](base_type* ptr) -> void { ptr->pop_back(); }, data_ptr);
	}

	void resize(val<size_type> count) {
		invoke(
		    +[](base_type* ptr, size_type count) -> void { ptr->resize(count); }, data_ptr, count);
	}

	void resize(val<size_type> count, val<CharT> ch) {
		invoke(
		    +[](base_type* ptr, size_type count, CharT ch) -> void { ptr->resize(count, ch); }, data_ptr, count, ch);
	}

	~val() {
		invoke(
		    +[](base_type* ptr) -> void {
			    if (ptr != nullptr) {
				    delete ptr;
			    }
		    },
		    data_ptr);
	}

	val<base_type*> release() {
		auto ptr = std::move(data_ptr);
		data_ptr = nullptr;
		return ptr;
	}

private:
	val<base_type*> data_ptr;
};

} // namespace nautilus

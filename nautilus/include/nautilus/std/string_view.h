#pragma once

#include "nautilus/val_ptr.hpp"
#include <string_view>

namespace nautilus {

template <class CharT, class Traits>
class val<std::basic_string_view<CharT, Traits>> {
public:
	using base_type = std::basic_string_view<CharT, Traits>;
	using value_type = typename base_type::value_type;
	using size_type = typename base_type::size_type;
	using const_reference = typename base_type::const_reference;
	using iterator = typename base_type::iterator;
	using const_iterator = typename base_type::const_iterator;
	using reference = typename base_type::reference;
	using pointer = typename base_type::pointer;
	using const_pointer = typename base_type::const_pointer;

	val() : data_ptr(nullptr) {
		auto string_vew = new std::basic_string_view<CharT, Traits>();
		data_ptr = val<base_type*>(string_vew);
	}

	val(val<const CharT*> s, val<size_type> count) : data_ptr(nullptr) {
		// call new on val<base_type *>
		auto raw_s = details::RawValueResolver<const CharT*>::getRawValue(s);
		auto raw_count = details::RawValueResolver<size_type>::getRawValue(count);
		auto string_vew = new std::basic_string_view<CharT, Traits>(raw_s, raw_count);
		data_ptr = val<base_type*>(string_vew);
	}

	val(val<const CharT*> s) : data_ptr(nullptr) {
		// call new on val<base_type *>
		// auto raw_s = details::RawValueResolver<typename std::remove_cvref_t<decltype((s))>::raw_type>::getRawValue(s);
		// auto string_vew = new std::basic_string_view<CharT, Traits>(raw_s);
		auto ptr = invoke(
		    +[](const CharT* s) -> auto { return new std::basic_string_view<CharT, Traits>(s); }, s);
		data_ptr = val<base_type*>(ptr);
	}

	val<const_iterator> begin() const {
		return invoke(
		    +[](std::basic_string_view<CharT, Traits>* ptr) { return ptr->begin(); }, data_ptr);
	}

	val<const_iterator> cbegin() const {
		return invoke(
		    +[](std::basic_string_view<CharT, Traits>* ptr) { return ptr->cbegin(); }, data_ptr);
	}

	val<const_iterator> end() const {
		return invoke(
		    +[](std::basic_string_view<CharT, Traits>* ptr) { return ptr->end(); }, data_ptr);
	}

	val<const_iterator> cend() const {
		return invoke(
		    +[](std::basic_string_view<CharT, Traits>* ptr) { return ptr->cend(); }, data_ptr);
	}

	val<value_type> operator[](val<size_type> pos) const {
		return invoke(
		    +[](std::basic_string_view<CharT, Traits>* ptr, size_type pos) -> value_type {
			    return ptr->operator[](pos);
		    },
		    data_ptr, pos);
	}

	const val<value_type> at(val<size_type> index) const {
		return invoke(
		    +[](std::basic_string_view<CharT, Traits>* ptr, size_type index) -> value_type { return ptr->at(index); },
		    data_ptr, index);
	}

	val<size_type> size() const noexcept {
		return invoke(
		    +[](std::basic_string_view<CharT, Traits>* ptr) { return ptr->size(); }, data_ptr);
	}

	val<size_type> length() const noexcept {
		return invoke(
		    +[](std::basic_string_view<CharT, Traits>* ptr) { return ptr->length(); }, data_ptr);
	}

	val<size_type> max_size() const noexcept {
		return invoke(
		    +[](std::basic_string_view<CharT, Traits>* ptr) { return ptr->max_size(); }, data_ptr);
	}

	val<bool> empty() const noexcept {
		return invoke(
		    +[](std::basic_string_view<CharT, Traits>* ptr) { return ptr->empty(); }, data_ptr);
	}

	val<value_type> front() const {
		return invoke(
		    +[](std::basic_string_view<CharT, Traits>* ptr) -> value_type { return ptr->front(); }, data_ptr);
	}

	val<value_type> back() const {
		return invoke(
		    +[](std::basic_string_view<CharT, Traits>* ptr) -> value_type { return ptr->back(); }, data_ptr);
	}

	val<const_pointer> data() const {
		return invoke(
		    +[](std::basic_string_view<CharT, Traits>* ptr) { return ptr->data(); }, data_ptr);
	}

	void remove_prefix(val<size_type> n) {
		invoke(
		    +[](std::basic_string_view<CharT, Traits>* ptr, size_type n) -> void { ptr->remove_prefix(n); }, data_ptr,
		    n);
	}

	void remove_suffix(val<size_type> n) {
		invoke(
		    +[](std::basic_string_view<CharT, Traits>* ptr, size_type n) -> void { ptr->remove_suffix(n); }, data_ptr,
		    n);
	}

	void swap(val<std::basic_string_view<CharT, Traits>>& v) {
		invoke(
		    +[](std::basic_string_view<CharT, Traits>* ptr, std::basic_string_view<CharT, Traits>* other) -> void {
			    ptr->swap(*other);
		    },
		    data_ptr, v.data_ptr);
	}

	val<size_type> copy(val<CharT*> dest, val<size_type> count, val<size_type> pos = 0) {
		return invoke(
		    +[](std::basic_string_view<CharT, Traits>* ptr, CharT* dest, size_type count, size_type pos) -> size_type {
			    return ptr->copy(dest, count, pos);
		    },
		    data_ptr, dest, count, pos);
	}

	/*
	val<std::basic_string_view<CharT, Traits>> substr(val<size_type> count, val<size_type> pos = 0) {
	    return invoke(+[](std::basic_string_view<CharT, Traits> *ptr,
	               size_type count, size_type pos) -> std::basic_string_view<CharT, Traits> {
	      return ptr->substr(count, pos);
	    }, data_ptr, count, pos);
	}

	val<std::basic_string_view<CharT, Traits>> substr(val<size_type> count, val<size_type> pos = 0) {
	    return invoke(+[](std::basic_string_view<CharT, Traits> *ptr,
	                      size_type count, size_type pos) -> std::basic_string_view<CharT, Traits> {
	        return ptr->substr(count, pos);
	    }, data_ptr, count, pos);
	}*/

	val<int> compare(val<std::basic_string_view<CharT, Traits>>& v) {
		return invoke(
		    +[](std::basic_string_view<CharT, Traits>* ptr, std::basic_string_view<CharT, Traits>* other) -> int {
			    return ptr->compare(*other);
		    },
		    data_ptr, v.data_ptr);
	}

	val<int> compare(val<size_type> pos1, val<size_type> count1, val<std::basic_string_view<CharT, Traits>>& v) {
		return invoke(
		    +[](std::basic_string_view<CharT, Traits>* ptr, size_type pos1, size_t count1,
		        std::basic_string_view<CharT, Traits>* v) -> int { return ptr->compare(pos1, count1, *v); },
		    data_ptr, pos1, count1, v.data_ptr);
	}

	val<int> compare(val<size_type> pos1, val<size_type> count1, val<std::basic_string_view<CharT, Traits>>& v,
	                 val<size_type> pos2, val<size_type> count2) {
		return invoke(
		    +[](std::basic_string_view<CharT, Traits>* ptr, size_type pos1, size_t count1,
		        std::basic_string_view<CharT, Traits>* v, size_type pos2,
		        size_t count2) -> int { return ptr->compare(pos1, count1, *v, pos2, count2); },
		    data_ptr, pos1, count1, v.data_ptr, pos2, count2);
	}

	val<int> compare(val<const CharT*> s) {
		return invoke(
		    +[](std::basic_string_view<CharT, Traits>* ptr, const CharT* s) -> int { return ptr->compare(s); },
		    data_ptr, s);
	}

	val<int> compare(val<size_type> pos1, val<size_type> count1, val<const CharT*> s) {
		return invoke(
		    +[](std::basic_string_view<CharT, Traits>* ptr, size_type pos1, size_t count1, const CharT* s) -> int {
			    return ptr->compare(pos1, count1, s);
		    },
		    data_ptr, pos1, count1, s);
	}

	val<int> compare(val<size_type> pos1, val<size_type> count1, val<const CharT*> s, val<size_type> pos2,
	                 val<size_type> count2) {
		return invoke(
		    +[](std::basic_string_view<CharT, Traits>* ptr, size_type pos1, size_t count1, const CharT* s,
		        size_type pos2, size_t count2) -> int { return ptr->compare(pos1, count1, s, pos2, count2); },
		    data_ptr, pos1, count1, s, pos2, count2);
	}
#ifdef __cpp_lib_starts_ends_with
	val<bool> start_with(val<std::basic_string_view<CharT, Traits>>& v) {
		return invoke(
		    +[](std::basic_string_view<CharT, Traits>* ptr, std::basic_string_view<CharT, Traits>* v) -> bool {
			    return ptr->starts_with(*v);
		    },
		    data_ptr, v.data_ptr);
	}

	val<bool> start_with(val<const CharT*> s) {
		return invoke(
		    +[](std::basic_string_view<CharT, Traits>* ptr, const CharT* s) -> bool { return ptr->starts_with(s); },
		    data_ptr, s);
	}

	val<bool> start_with(val<CharT> s) {
		return invoke(
		    +[](std::basic_string_view<CharT, Traits>* ptr, CharT s) -> bool { return ptr->starts_with(s); }, data_ptr,
		    s);
	}

	val<bool> end_with(val<std::basic_string_view<CharT, Traits>>& v) {
		return invoke(
		    +[](std::basic_string_view<CharT, Traits>* ptr, std::basic_string_view<CharT, Traits>* v) -> bool {
			    return ptr->ends_with(*v);
		    },
		    data_ptr, v.data_ptr);
	}

	val<bool> end_with(val<const CharT*> s) {
		return invoke(
		    +[](std::basic_string_view<CharT, Traits>* ptr, const CharT* s) -> bool { return ptr->ends_with(s); },
		    data_ptr, s);
	}

	val<bool> end_with(val<CharT> s) {
		return invoke(
		    +[](std::basic_string_view<CharT, Traits>* ptr, CharT s) -> bool { return ptr->ends_with(s); }, data_ptr,
		    s);
	}
#endif

#ifdef __cpp_lib_string_contains
	val<bool> contains(val<std::basic_string_view<CharT, Traits>>& v) {
		return invoke(
		    +[](std::basic_string_view<CharT, Traits>* ptr, std::basic_string_view<CharT, Traits>* v) -> bool {
			    return ptr->contains(*v);
		    },
		    data_ptr, v.data_ptr);
	}

	val<bool> contains(val<const CharT*> s) {
		return invoke(
		    +[](std::basic_string_view<CharT, Traits>* ptr, const CharT* s) -> bool { return ptr->contains(s); },
		    data_ptr, s);
	}

	val<bool> contains(val<CharT> s) {
		return invoke(
		    +[](std::basic_string_view<CharT, Traits>* ptr, CharT s) -> bool { return ptr->contains(s); }, data_ptr, s);
	}
#endif
private:
	val<base_type*> data_ptr;
};

// using string_view = val<std::basic_string_view<char, std::char_traits<char>>>;
// using u8string_view = val<std::basic_string_view<char8_t, std::char_traits<char8_t>>>;
// using u16string_view = std::basic_string_view<char16_t, std::char_traits<char16_t>>;
//  using u32string_view = std::basic_string_view<char32_t, std::char_traits<char32_t>>;
// using wstring_view = std::basic_string_view<wchar_t, std::char_traits<wchar_t>>;
} // namespace nautilus

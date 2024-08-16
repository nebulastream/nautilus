
#include "nautilus/function.hpp"
#include "ostream.hpp"
#include "sstream"
#pragma once
namespace nautilus {

/**
 * The class template std::basic_stringstream implements input and output operations on string based streams.
 * It effectively stores an instance of std::basic_string and performs the input and output operations on it.
 * At the low level, the class essentially wraps a raw string device implementation of std::basic_stringbuf into
 * a higher-level interface of std::basic_iostream.
 * The complete interface to unique std::basic_stringbuf members is provided.
 * @tparam CharT
 * @tparam Traits
 * @tparam Allocator
 */
template <class CharT, class Traits, class Allocator>
class val<std::basic_stringstream<CharT, Traits, Allocator>> : public val<std::basic_ostream<CharT, Traits>> {
public:
	val() : val<std::basic_ostream<CharT, Traits>>(nullptr) {
		auto data_ptr = invoke(+[]() -> auto* {
			auto sb = new std::basic_stringstream<CharT, Traits, Allocator>();
			return dynamic_cast<std::basic_ostream<CharT, Traits>*>(sb);
		});
		this->stream = data_ptr;
	}
	/*
	val<std::basic_string_view<CharT, Traits>> view() {
		auto view_ptr = invoke(
		    +[](std::basic_ostream<CharT, Traits>* ptr) -> const char* {
			    auto sb = dynamic_cast<std::basic_stringstream<CharT, Traits, Allocator>*>(ptr);
			    auto view = sb->view();
			    auto data = view.data();
			    return data;
		    },
		    this->stream);
		return val<std::basic_string_view<CharT, Traits>>(view_ptr);
	};
	 */

	val<std::basic_string<CharT, Traits>> str() {
		auto str_ptr = invoke(
		    +[](std::basic_ostream<CharT, Traits>* ptr) -> auto {
			    auto sb = dynamic_cast<std::basic_stringstream<CharT, Traits, Allocator>*>(ptr);
			    auto str = new std::basic_string<CharT, Traits>(sb->str());
			    return str;
		    },
		    this->stream);
		return val<std::basic_string<CharT, Traits>>(str_ptr);
	};

	~val() {
		invoke(
		    +[](std::basic_ostream<CharT, Traits>* ptr) -> void { delete ptr; }, this->stream);
	}
};

typedef val<std::basic_stringstream<char>> stringstream;
} // namespace nautilus

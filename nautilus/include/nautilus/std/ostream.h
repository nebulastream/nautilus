#pragma once
#include "nautilus/val_ptr.hpp"
#include <ostream>
#include <string>
namespace nautilus {

template <class _CharT, class _Traits>
void callEndl(std::basic_ostream<_CharT, _Traits>* ptr) {
	std::endl(*ptr);
}
template <class _CharT, class _Traits>
val<std::basic_ostream<_CharT, _Traits>>& endl(val<std::basic_ostream<_CharT, _Traits>>& __os) {
	invoke(callEndl<_CharT, _Traits>, __os.stream);
	return __os;
}

template <class _CharT, class _Traits>
void callEnds(std::basic_ostream<_CharT, _Traits>* ptr) {
	std::ends(*ptr);
}

template <class _CharT, class _Traits>
val<std::basic_ostream<_CharT, _Traits>>& ends(val<std::basic_ostream<_CharT, _Traits>>& __os) {
	invoke(callEnds<_CharT, _Traits>, __os.stream);
	return __os;
}

template <class _CharT, class _Traits>
void callFlush(std::basic_ostream<_CharT, _Traits>* ptr) {
	std::flush(*ptr);
}

template <class _CharT, class _Traits>
val<std::basic_ostream<_CharT, _Traits>>& flush(val<std::basic_ostream<_CharT, _Traits>>& __os) {
	invoke(callFlush<_CharT, _Traits>, __os.stream);
	return __os;
}

template <class CharT, class Traits>
class val<std::basic_ostream<CharT, Traits>> {
public:
	explicit val(val<std::basic_ostream<CharT, Traits>*> stream) : stream(stream) {};
	template <class _CharT, class _Traits>
	val(val<std::basic_ostream<_CharT, _Traits>>& other) : stream(other.stream) {
	}

	template <class T>
	val<std::basic_ostream<CharT, Traits>>& operator<<(val<T>& value) {
		invoke(pipe<T>, stream, value);
		return *this;
	}

	val<std::basic_ostream<CharT, Traits>>& operator<<(const char* value) {
		invoke(pipe<const char*>, stream, val<const char*>(value));
		return *this;
	}

	val<std::basic_ostream<CharT, Traits>>& put(val<CharT> ch) {
		invoke(put, stream, ch);
		return *this;
	}

	val<std::basic_ostream<CharT, Traits>>& write(val<const CharT*> s, val<size_t> count) {
		invoke(write, stream, s, count);
		return *this;
	}

	val<std::basic_ostream<CharT, Traits>>& tellp() {
		invoke(tellp, stream);
		return *this;
	}

	val<std::basic_ostream<CharT, Traits>>& seekp(val<typename Traits::pos_type> pos) {
		invoke(seekp, stream, pos);
		return *this;
	}

	auto& operator<<(val<std::basic_ostream<CharT, Traits>>& (*func)(val<std::basic_ostream<CharT, Traits>>&) ) {
		func(*this);
		return *this;
	}

	val<std::basic_ostream<CharT, Traits>*> stream;

private:
	template <class T>
	static void pipe(std::basic_ostream<CharT, Traits>* ptr, T value) {
		auto& s = *ptr;
		s << value;
	}

	static void flash(std::basic_ostream<CharT, Traits>* ptr) {
		ptr->flush();
	}

	static void put(std::basic_ostream<CharT, Traits>* ptr, CharT ch) {
		ptr->put(ch);
	}
	static void write(std::basic_ostream<CharT, Traits>* ptr, const CharT* ch, size_t count) {
		ptr->write(ch, count);
	}

	static void tellp(std::basic_ostream<CharT, Traits>* ptr) {
		ptr->tellp();
	}
	static void seekp(std::basic_ostream<CharT, Traits>* ptr, typename Traits::pos_type pos) {
		ptr->seekp(pos);
	}
};

} // namespace nautilus

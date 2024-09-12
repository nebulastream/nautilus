#pragma once

#include "nautilus/common/traceing.hpp"
#include <iostream>
#include <iterator>
#include <type_traits>
#include <vector>

namespace nautilus {

template <typename T>
    requires std::is_integral_v<T>
class alignas(64) static_val {
public:
	using raw_type = T;

	static_val() {
		tracing::pushStaticVal(&value);
	}

	static_val(T v) : value(v) {
		tracing::pushStaticVal((void*) &value);
	}

	static_val(const static_val& other) : static_val((T) other) {
	}

	~static_val() {
		tracing::popStaticVal();
	}

	static_val& operator=(const static_val& other) {
		*this = (T) other;
		return *this;
	}

	template <typename OT>
	static_val(const static_val<OT>& other) : static_val((T) (OT) other) {
	}

	template <typename OT>
	static_val& operator=(const static_val<OT>& other) {
		*this = (T) (OT) other;
		return *this;
	}

	operator T() const {
		return value;
	}

	const T& operator=(const T& t) {
		value = t;
		return t;
	}

	const auto& operator++() {
		++value;
		return *this;
	}

	auto operator++(int) {
		auto temp = *this;
		++value;
		return temp;
	}

	auto& operator--() {
		--value;
		return *this;
	}

	auto operator--(int) {
		auto temp = *this;
		--value;
		return temp;
	}

	bool operator<(const T& other) const {
		return value < other;
	}

	bool operator<=(const T& other) const {
		return value <= other;
	}

	bool operator>=(const T& other) const {
		return value >= other;
	}

	bool operator==(const T& other) const {
		return value == other;
	}

	bool operator>(const T& other) const {
		return value > other;
	}

	template <typename Arg>
	static_val<T> operator+(const Arg& other) const {
		return value + other;
	}

private:
	T value;
};

template <typename Iterator>
class static_iterator {
public:
	using iterator_category = typename std::iterator_traits<Iterator>::iterator_category;
	using value_type = typename std::iterator_traits<Iterator>::value_type;
	using difference_type = typename std::iterator_traits<Iterator>::difference_type;
	using pointer = typename std::iterator_traits<Iterator>::pointer;
	using reference = typename std::iterator_traits<Iterator>::reference;

	explicit static_iterator(Iterator it) : val(0), m_iterator(it) {
	}

	reference operator*() {
		val = val + 1;
		return *m_iterator;
	}

	pointer operator->() {
		val = val + 1;
		return std::addressof(operator*());
	}

	static_iterator& operator++() {
		val = val + 1;
		++m_iterator;
		return *this;
	}

	static_iterator operator++(int) {
		val = val + 1;
		static_iterator tmp = *this;
		++(*this);
		return tmp;
	}

	bool operator==(const static_iterator& other) {
		val = val + 1;
		return m_iterator == other.m_iterator;
	}

	bool operator!=(const static_iterator& other) const {
		return !(*this == other);
	}

private:
	static_val<int64_t> val;
	Iterator m_iterator;
};

template <typename Container>
class static_iterable {
public:
	static_iterable(Container& container) : m_container(container) {
	}

	auto begin() {
		return static_iterator<decltype(m_container.begin())>(m_container.begin());
	}

	auto end() {
		return static_iterator<decltype(m_container.end())>(m_container.end());
	}

private:
	Container& m_container;
};

} // namespace nautilus

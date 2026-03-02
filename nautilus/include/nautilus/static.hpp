#pragma once
#include "nautilus/config.hpp"
#ifdef ENABLE_TRACING
#include "nautilus/tracing/TracingUtil.hpp"
#endif
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
#ifdef ENABLE_TRACING
		tracing::pushStaticVal(&value);
		owns_trace = true;
#endif
	}

	static_val(T v) : value(v) {
#ifdef ENABLE_TRACING
		tracing::pushStaticVal((void*) &value);
		owns_trace = true;
#endif
	}

	// Copy constructor: carries the value but does NOT push to tracing stack.
	// The number of copies is ABI/platform-dependent (calling convention,
	// copy elision), so suppressing tracing here avoids platform-divergent
	// push/pop imbalances.
	static_val(const static_val& other) : value((T) other) {
		// owns_trace stays false — no push, no matching pop needed
	}

	// Move constructor: transfers ownership of the trace slot so the
	// destructor of the moved-from object does not pop.
	static_val(static_val&& other) noexcept : value(other.value) {
#ifdef ENABLE_TRACING
		owns_trace = other.owns_trace;
		other.owns_trace = false;
#endif
	}

	~static_val() {
#ifdef ENABLE_TRACING
		if (owns_trace) {
			tracing::popStaticVal();
		}
#endif
	}

	// Copy-assign: update value only, tracing ownership does not change.
	static_val& operator=(const static_val& other) {
		value = (T) other;
		return *this;
	}

	// Move-assign: transfer value and tracing ownership.
	static_val& operator=(static_val&& other) noexcept {
		value = other.value;
#ifdef ENABLE_TRACING
		// If we currently own a trace slot, pop it before stealing other's.
		if (owns_trace) {
			tracing::popStaticVal();
		}
		owns_trace = other.owns_trace;
		other.owns_trace = false;
#endif
		return *this;
	}

	template <typename OT>
	static_val(const static_val<OT>& other) : value((T) (OT) other) {
		// Cross-type copy: same rationale as copy constructor — no trace push.
	}

	template <typename OT>
	static_val& operator=(const static_val<OT>& other) {
		value = (T) (OT) other;
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

	T operator++(int) {
		T temp = value;
		++value;
		return temp;
	}

	auto& operator--() {
		--value;
		return *this;
	}

	T operator--(int) {
		T temp = value;
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
	bool operator!=(const T& other) const {
		return value != other;
	}
	bool operator>(const T& other) const {
		return value > other;
	}

	template <typename Arg>
	    requires std::is_integral_v<Arg>
	T operator+(const Arg& other) const {
		return value + static_cast<T>(other);
	}

private:
	T value {};
#ifdef ENABLE_TRACING
	bool owns_trace = false;
#endif
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

	friend bool operator==(const static_iterator& lhs, const static_iterator& rhs) {
		return lhs.m_iterator == rhs.m_iterator;
	}

	friend bool operator!=(const static_iterator& lhs, const static_iterator& rhs) {
		return lhs.m_iterator != rhs.m_iterator;
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

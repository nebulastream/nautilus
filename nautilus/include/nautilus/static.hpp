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
		tracing::pushStaticVal(&value, sizeof(T));
		owns_trace = true;
#endif
	}

	static_val(T v) : value(v) {
#ifdef ENABLE_TRACING
		tracing::pushStaticVal((void*) &value, sizeof(T));
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

// Forward declaration for use in static_iterator friendship.
template <typename Iterator>
class static_sentinel;

// Iterator wrapper that holds a static_val counter so the tracer sees a unique
// snapshot hash on every step, enabling full loop unrolling at trace time.
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

	// Dereference is side-effect-free; the counter advances only in operator++.
	reference operator*() const {
		return *m_iterator;
	}

	pointer operator->() const {
		return std::addressof(*m_iterator);
	}

	// Advance: increment the static counter (new snapshot hash) and the underlying iterator.
	static_iterator& operator++() {
		++val;
		++m_iterator;
		return *this;
	}

	// Standard post-increment: copy current state, advance *this, return old state.
	static_iterator operator++(int) {
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

	friend bool operator==(const static_iterator& it, const static_sentinel<Iterator>& s);
	friend bool operator!=(const static_iterator& it, const static_sentinel<Iterator>& s);

private:
	static_val<int64_t> val;
	Iterator m_iterator;
	friend class static_sentinel<Iterator>;
};

// Sentinel for end(): stores only the end iterator without a static_val,
// avoiding a spurious constant in the snapshot hash.
template <typename Iterator>
class static_sentinel {
public:
	explicit static_sentinel(Iterator it) : m_end(it) {
	}

	friend bool operator==(const static_iterator<Iterator>& it, const static_sentinel& s) {
		return it.m_iterator == s.m_end;
	}

	friend bool operator!=(const static_iterator<Iterator>& it, const static_sentinel& s) {
		return it.m_iterator != s.m_end;
	}

private:
	Iterator m_end;
};

// Wraps any container (or C-style array via std::begin/end) so that a range-for
// loop over it is fully unrolled at trace time.
template <typename Container>
class static_iterable {
public:
	explicit static_iterable(Container& container) : m_container(container) {
	}

	auto begin() {
		using std::begin;
		auto it = begin(m_container);
		return static_iterator<decltype(it)>(it);
	}

	auto end() {
		using std::end;
		auto it = end(m_container);
		return static_sentinel<decltype(it)>(it);
	}

private:
	Container& m_container;
};

// --- enumerate support ---

template <typename Iterator, typename IndexType = int64_t>
class enumerate_sentinel;

// Iterator that exposes the static loop counter alongside each element.
// Structured-binding friendly: `for (auto [idx, elem] : static_enumerable(c))`.
// `idx` is `static_val<IndexType>&` — a reference into the iterator, valid for
// the lifetime of the loop body.
template <typename Iterator, typename IndexType = int64_t>
class enumerate_iterator {
public:
	using value_type = typename std::iterator_traits<Iterator>::value_type;
	using reference = typename std::iterator_traits<Iterator>::reference;

	struct value_ref {
		static_val<IndexType>& index;
		reference element;
	};

	explicit enumerate_iterator(Iterator it) : index(0), m_iterator(it) {
	}

	value_ref operator*() {
		return {index, *m_iterator};
	}

	enumerate_iterator& operator++() {
		++index;
		++m_iterator;
		return *this;
	}

	enumerate_iterator operator++(int) {
		enumerate_iterator tmp = *this;
		++(*this);
		return tmp;
	}

	friend bool operator==(const enumerate_iterator& lhs, const enumerate_iterator& rhs) {
		return lhs.m_iterator == rhs.m_iterator;
	}

	friend bool operator!=(const enumerate_iterator& lhs, const enumerate_iterator& rhs) {
		return lhs.m_iterator != rhs.m_iterator;
	}

	friend bool operator==(const enumerate_iterator& it, const enumerate_sentinel<Iterator, IndexType>& s);
	friend bool operator!=(const enumerate_iterator& it, const enumerate_sentinel<Iterator, IndexType>& s);

private:
	static_val<IndexType> index;
	Iterator m_iterator;
	friend class enumerate_sentinel<Iterator, IndexType>;
};

template <typename Iterator, typename IndexType>
class enumerate_sentinel {
public:
	explicit enumerate_sentinel(Iterator it) : m_end(it) {
	}

	friend bool operator==(const enumerate_iterator<Iterator, IndexType>& it, const enumerate_sentinel& s) {
		return it.m_iterator == s.m_end;
	}

	friend bool operator!=(const enumerate_iterator<Iterator, IndexType>& it, const enumerate_sentinel& s) {
		return it.m_iterator != s.m_end;
	}

private:
	Iterator m_end;
};

// Like static_iterable but also exposes the iteration index as a static_val.
template <typename Container, typename IndexType = int64_t>
class static_enumerable {
public:
	explicit static_enumerable(Container& container) : m_container(container) {
	}

	auto begin() {
		using std::begin;
		auto it = begin(m_container);
		return enumerate_iterator<decltype(it), IndexType>(it);
	}

	auto end() {
		using std::end;
		auto it = end(m_container);
		return enumerate_sentinel<decltype(it), IndexType>(it);
	}

private:
	Container& m_container;
};

} // namespace nautilus

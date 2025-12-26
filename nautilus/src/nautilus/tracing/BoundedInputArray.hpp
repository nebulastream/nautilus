#pragma once

#include <array>
#include <cstdint>
#include <iterator>

namespace nautilus::tracing {

/**
 * @brief Zero-overhead container wrapping a fixed-size array with a runtime count.
 *
 * This container provides iterator access to only the valid elements (0..count-1)
 * while storing all elements in a stack-allocated array.
 *
 * @tparam T Element type
 * @tparam N Maximum capacity
 */
template <typename T, size_t N>
class BoundedInputArray {
public:
	using value_type = T;
	using size_type = uint8_t;
	using reference = T&;
	using const_reference = const T&;
	using pointer = T*;
	using const_pointer = const T*;

	// Iterator that only iterates over valid elements [0, count_)
	class iterator {
	public:
		using iterator_category = std::random_access_iterator_tag;
		using value_type = T;
		using difference_type = std::ptrdiff_t;
		using pointer = T*;
		using reference = T&;

		iterator(pointer ptr) : ptr_(ptr) {
		}

		reference operator*() const {
			return *ptr_;
		}
		pointer operator->() const {
			return ptr_;
		}

		iterator& operator++() {
			++ptr_;
			return *this;
		}
		iterator operator++(int) {
			iterator tmp = *this;
			++ptr_;
			return tmp;
		}

		iterator& operator--() {
			--ptr_;
			return *this;
		}
		iterator operator--(int) {
			iterator tmp = *this;
			--ptr_;
			return tmp;
		}

		iterator& operator+=(difference_type n) {
			ptr_ += n;
			return *this;
		}
		iterator& operator-=(difference_type n) {
			ptr_ -= n;
			return *this;
		}

		iterator operator+(difference_type n) const {
			return iterator(ptr_ + n);
		}
		iterator operator-(difference_type n) const {
			return iterator(ptr_ - n);
		}

		difference_type operator-(const iterator& other) const {
			return ptr_ - other.ptr_;
		}

		reference operator[](difference_type n) const {
			return ptr_[n];
		}

		bool operator==(const iterator& other) const {
			return ptr_ == other.ptr_;
		}
		bool operator!=(const iterator& other) const {
			return ptr_ != other.ptr_;
		}
		bool operator<(const iterator& other) const {
			return ptr_ < other.ptr_;
		}
		bool operator<=(const iterator& other) const {
			return ptr_ <= other.ptr_;
		}
		bool operator>(const iterator& other) const {
			return ptr_ > other.ptr_;
		}
		bool operator>=(const iterator& other) const {
			return ptr_ >= other.ptr_;
		}

	private:
		pointer ptr_;
	};

	class const_iterator {
	public:
		using iterator_category = std::random_access_iterator_tag;
		using value_type = T;
		using difference_type = std::ptrdiff_t;
		using pointer = const T*;
		using reference = const T&;

		const_iterator(pointer ptr) : ptr_(ptr) {
		}

		reference operator*() const {
			return *ptr_;
		}
		pointer operator->() const {
			return ptr_;
		}

		const_iterator& operator++() {
			++ptr_;
			return *this;
		}
		const_iterator operator++(int) {
			const_iterator tmp = *this;
			++ptr_;
			return tmp;
		}

		const_iterator& operator--() {
			--ptr_;
			return *this;
		}
		const_iterator operator--(int) {
			const_iterator tmp = *this;
			--ptr_;
			return tmp;
		}

		const_iterator& operator+=(difference_type n) {
			ptr_ += n;
			return *this;
		}
		const_iterator& operator-=(difference_type n) {
			ptr_ -= n;
			return *this;
		}

		const_iterator operator+(difference_type n) const {
			return const_iterator(ptr_ + n);
		}
		const_iterator operator-(difference_type n) const {
			return const_iterator(ptr_ - n);
		}

		difference_type operator-(const const_iterator& other) const {
			return ptr_ - other.ptr_;
		}

		reference operator[](difference_type n) const {
			return ptr_[n];
		}

		bool operator==(const const_iterator& other) const {
			return ptr_ == other.ptr_;
		}
		bool operator!=(const const_iterator& other) const {
			return ptr_ != other.ptr_;
		}
		bool operator<(const const_iterator& other) const {
			return ptr_ < other.ptr_;
		}
		bool operator<=(const const_iterator& other) const {
			return ptr_ <= other.ptr_;
		}
		bool operator>(const const_iterator& other) const {
			return ptr_ > other.ptr_;
		}
		bool operator>=(const const_iterator& other) const {
			return ptr_ >= other.ptr_;
		}

	private:
		pointer ptr_;
	};

	// Constructors
	BoundedInputArray() : data_(), count_(0) {
	}

	BoundedInputArray(std::array<T, N>&& arr, size_type count) : data_(std::move(arr)), count_(count) {
	}

	// Default copy/move semantics
	BoundedInputArray(const BoundedInputArray&) = default;
	BoundedInputArray& operator=(const BoundedInputArray&) = default;
	BoundedInputArray(BoundedInputArray&&) = default;
	BoundedInputArray& operator=(BoundedInputArray&&) = default;

	// Element access
	reference operator[](size_type pos) {
		return data_[pos];
	}
	const_reference operator[](size_type pos) const {
		return data_[pos];
	}

	// Iterators - only iterate over valid elements
	iterator begin() {
		return iterator(data_.data());
	}
	const_iterator begin() const {
		return const_iterator(data_.data());
	}
	const_iterator cbegin() const {
		return const_iterator(data_.data());
	}

	iterator end() {
		return iterator(data_.data() + count_);
	}
	const_iterator end() const {
		return const_iterator(data_.data() + count_);
	}
	const_iterator cend() const {
		return const_iterator(data_.data() + count_);
	}

	// Capacity
	size_type size() const {
		return count_;
	}
	constexpr size_type capacity() const {
		return N;
	}
	bool empty() const {
		return count_ == 0;
	}

	// Access to underlying data
	pointer data() {
		return data_.data();
	}
	const_pointer data() const {
		return data_.data();
	}

	// Access to underlying array (for compatibility)
	std::array<T, N>& array() {
		return data_;
	}
	const std::array<T, N>& array() const {
		return data_;
	}

private:
	std::array<T, N> data_;
	size_type count_;
};

} // namespace nautilus::tracing

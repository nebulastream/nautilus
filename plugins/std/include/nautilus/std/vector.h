#pragma once

#include "nautilus/val_ptr.hpp"
#include <cstdint>
#include <cstring>
#include <vector>

namespace nautilus {

namespace detail {

/// Layout of std::vector's three internal pointer fields. All major
/// standard libraries (libstdc++, libc++, MSVC STL) store the data
/// pointer, finish pointer, and end-of-storage pointer as the first three
/// pointer-sized members. We probe the byte offsets at static-init time to
/// avoid hard-coding the layout.
struct vector_layout_offsets {
	std::size_t start_offset = 0;
	std::size_t finish_offset = sizeof(void*);
	std::size_t end_offset = 2 * sizeof(void*);
};

template <typename T, typename Allocator>
inline vector_layout_offsets probe_vector_layout() {
	vector_layout_offsets layout;
	std::vector<T, Allocator> v;
	v.reserve(4);
	v.push_back(T {});
	const auto* start_p = static_cast<const void*>(v.data());
	const auto* finish_p = static_cast<const void*>(v.data() + 1);
	const auto* end_p = static_cast<const void*>(v.data() + 4);
	const auto* base = reinterpret_cast<const std::byte*>(&v);
	for (std::size_t i = 0; i + sizeof(void*) <= sizeof(v); i += alignof(void*)) {
		void* slot;
		std::memcpy(&slot, base + i, sizeof(void*));
		if (slot == start_p) {
			layout.start_offset = i;
		} else if (slot == finish_p) {
			layout.finish_offset = i;
		} else if (slot == end_p) {
			layout.end_offset = i;
		}
	}
	return layout;
}

template <typename T, typename Allocator>
inline const vector_layout_offsets vector_layout = probe_vector_layout<T, Allocator>();

/// Backwards-compatible alias for the data-pointer offset.
template <typename T, typename Allocator>
inline const std::size_t vector_data_offset = vector_layout<T, Allocator>.start_offset;

} // namespace detail

template <class T, class Allocator>
class val<std::vector<T, Allocator>> {
	using base_type = std::vector<T, Allocator>;
	using value_type = typename base_type::value_type;
	using size_type = typename base_type::size_type;

	/// Loads the vector's internal data pointer via direct memory access at
	/// the probed byte offset. Emits a LOAD op instead of an opaque invoke()
	/// call. Goes through `val<void*>` intermediates because some backends
	/// (bc on macOS) reject direct ptr→ptr casts between unrelated types.
	val<T*> load_data_ptr() {
		auto byte_ptr = static_cast<val<uint8_t*>>(static_cast<val<void*>>(data_ptr));
		auto data_field_byte_ptr = byte_ptr + val<uint64_t>(detail::vector_layout<T, Allocator>.start_offset);
		auto data_field_ptr = static_cast<val<T**>>(static_cast<val<void*>>(data_field_byte_ptr));
		return *data_field_ptr;
	}

	/// Loads the vector's `_M_finish` pointer at the probed byte offset.
	val<T*> load_finish_ptr() {
		auto byte_ptr = static_cast<val<uint8_t*>>(static_cast<val<void*>>(data_ptr));
		auto field_byte_ptr = byte_ptr + val<uint64_t>(detail::vector_layout<T, Allocator>.finish_offset);
		auto field_ptr = static_cast<val<T**>>(static_cast<val<void*>>(field_byte_ptr));
		return *field_ptr;
	}

	/// Loads the vector's `_M_end_of_storage` pointer at the probed byte offset.
	val<T*> load_end_ptr() {
		auto byte_ptr = static_cast<val<uint8_t*>>(static_cast<val<void*>>(data_ptr));
		auto field_byte_ptr = byte_ptr + val<uint64_t>(detail::vector_layout<T, Allocator>.end_offset);
		auto field_ptr = static_cast<val<T**>>(static_cast<val<void*>>(field_byte_ptr));
		return *field_ptr;
	}

public:
	val() : data_ptr(nullptr) {
		data_ptr = invoke(+[]() -> base_type* { return new base_type(); });
	}

	val(val<std::vector<T, Allocator>*> vec) : data_ptr(vec) {
	}

	/// Move-assignment: take ownership of `other`'s underlying vector and
	/// delete the previously owned one. Required to avoid leaks reported by
	/// LeakSanitizer in CI when reassigning a default-constructed vector.
	val<std::vector<T, Allocator>>& operator=(val<std::vector<T, Allocator>>& other) {
		invoke(
		    +[](base_type* old_ptr) -> void {
			    if (old_ptr != nullptr) {
				    delete old_ptr;
			    }
		    },
		    this->data_ptr);
		this->data_ptr = other.data_ptr;
		other.data_ptr = nullptr;
		return *this;
	}

	// Element access — direct memory access (no invoke).
	// - arithmetic T: returns val<T&>, supporting both `vec[i]` reads and `vec[i] = x` writes.
	// - pointer T: returns val<T> (load only). vec[i] = x for pointers is intentionally unsupported.
	// - class T: returns val<T*>.

	auto operator[](val<size_type> pos) {
		auto data = load_data_ptr();
		if constexpr (std::is_class_v<T>) {
			auto byte_data = static_cast<val<uint8_t*>>(static_cast<val<void*>>(data));
			auto elem_byte_ptr = byte_data + pos * val<uint64_t>(sizeof(T));
			return static_cast<val<T*>>(static_cast<val<void*>>(elem_byte_ptr));
		} else if constexpr (std::is_arithmetic_v<T>) {
			return data[pos]; // val<T&> — supports load and store
		} else {
			return val<T>(data[pos]);
		}
	}

	auto front() {
		auto data = load_data_ptr();
		if constexpr (std::is_class_v<T>) {
			return data;
		} else if constexpr (std::is_arithmetic_v<T>) {
			return *data;
		} else {
			return val<T>(*data);
		}
	}

	auto back() {
		auto data = load_data_ptr();
		auto last_index = size() - val<size_type>(1);
		if constexpr (std::is_class_v<T>) {
			auto byte_data = static_cast<val<uint8_t*>>(static_cast<val<void*>>(data));
			auto elem_byte_ptr = byte_data + last_index * val<uint64_t>(sizeof(T));
			return static_cast<val<T*>>(static_cast<val<void*>>(elem_byte_ptr));
		} else if constexpr (std::is_arithmetic_v<T>) {
			return data[last_index];
		} else {
			return val<T>(data[last_index]);
		}
	}

	// at() is bounds-checked — keep the invoke() call so std::out_of_range is thrown correctly.

	auto at(val<size_type> pos) {
		if constexpr (std::is_class_v<T>) {
			return invoke(
			    +[](base_type* ptr, size_type p) -> T* { return &ptr->at(p); }, data_ptr, pos);
		} else {
			return invoke(
			    +[](base_type* ptr, size_type p) -> T { return ptr->at(p); }, data_ptr, pos);
		}
	}

	val<T*> data() {
		return load_data_ptr();
	}

	// Capacity — direct memory access. size/capacity are computed from the
	// distance between the internal pointers; empty is a pointer compare.

	val<bool> empty() {
		return load_data_ptr() == load_finish_ptr();
	}

	val<size_type> size() {
		auto start = static_cast<val<uint64_t>>(load_data_ptr());
		auto finish = static_cast<val<uint64_t>>(load_finish_ptr());
		return val<size_type>((finish - start) / val<uint64_t>(sizeof(T)));
	}

	val<size_type> capacity() {
		auto start = static_cast<val<uint64_t>>(load_data_ptr());
		auto end = static_cast<val<uint64_t>>(load_end_ptr());
		return val<size_type>((end - start) / val<uint64_t>(sizeof(T)));
	}

	val<size_type> max_size() const {
		return invoke(
		    +[](base_type* ptr) -> size_type { return ptr->max_size(); }, data_ptr);
	}

	void reserve(val<size_type> new_cap) {
		invoke(
		    +[](base_type* ptr, size_type cap) -> void { ptr->reserve(cap); }, data_ptr, new_cap);
	}

	void shrink_to_fit() {
		invoke(
		    +[](base_type* ptr) -> void { ptr->shrink_to_fit(); }, data_ptr);
	}

	// Modifiers

	void clear() {
		invoke(
		    +[](base_type* ptr) -> void { ptr->clear(); }, data_ptr);
	}

	void push_back(val<T> value)
	    requires(!std::is_class_v<T>)
	{
		invoke(
		    +[](base_type* ptr, T v) -> void { ptr->push_back(v); }, data_ptr, value);
	}

	void push_back(val<T*> value_ptr)
	    requires(std::is_class_v<T>)
	{
		invoke(
		    +[](base_type* ptr, T* v) -> void { ptr->push_back(*v); }, data_ptr, value_ptr);
	}

	void pop_back() {
		invoke(
		    +[](base_type* ptr) -> void { ptr->pop_back(); }, data_ptr);
	}

	void resize(val<size_type> count) {
		invoke(
		    +[](base_type* ptr, size_type c) -> void { ptr->resize(c); }, data_ptr, count);
	}

	void resize(val<size_type> count, val<T> value)
	    requires(!std::is_class_v<T>)
	{
		invoke(
		    +[](base_type* ptr, size_type c, T v) -> void { ptr->resize(c, v); }, data_ptr, count, value);
	}

	void swap(val<std::vector<T, Allocator>>& other) {
		invoke(
		    +[](base_type* a, base_type* b) -> void { a->swap(*b); }, data_ptr, other.data_ptr);
	}

	val<bool> equals(val<std::vector<T, Allocator>>& other) {
		return invoke(
		    +[](base_type* a, base_type* b) -> bool { return *a == *b; }, data_ptr, other.data_ptr);
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

	// --- Iterator support ---
	//
	// A random-access iterator that wraps a `val<T*>` cursor into the
	// vector's contiguous storage. It mirrors the semantics of a normal C++
	// `std::vector<T>::iterator`: increment, decrement, random-access
	// arithmetic, dereference, and comparison are all supported, so it can be
	// used in ordinary `for (auto it = v.begin(); it != v.end(); ++it)` style
	// loops as well as range-for.
	//
	// Dereference mirrors `operator[]`:
	//   - arithmetic T  -> val<T&> (load + store)
	//   - class T       -> val<T*>
	//   - pointer T     -> val<T> (load only)
	class iterator {
	public:
		iterator(val<T*> cursor) : cursor_(cursor) {
		}

		auto operator*() {
			if constexpr (std::is_class_v<T>) {
				return cursor_;
			} else if constexpr (std::is_arithmetic_v<T>) {
				return cursor_[val<size_type>(0)];
			} else {
				return val<T>(cursor_[val<size_type>(0)]);
			}
		}

		auto operator[](val<size_type> n) {
			auto tmp = *this + n;
			return *tmp;
		}

		iterator& operator++() {
			cursor_ += val<size_type>(1);
			return *this;
		}

		iterator operator++(int) {
			iterator tmp = *this;
			++(*this);
			return tmp;
		}

		iterator& operator--() {
			cursor_ = cursor_ - val<size_type>(1);
			return *this;
		}

		iterator operator--(int) {
			iterator tmp = *this;
			--(*this);
			return tmp;
		}

		iterator operator+(val<size_type> n) {
			val<T*> tmp = cursor_;
			return iterator(tmp + n);
		}

		iterator operator-(val<size_type> n) {
			val<T*> tmp = cursor_;
			return iterator(tmp - n);
		}

		iterator& operator+=(val<size_type> n) {
			cursor_ = cursor_ + n;
			return *this;
		}

		iterator& operator-=(val<size_type> n) {
			cursor_ = cursor_ - n;
			return *this;
		}

		val<size_type> operator-(const iterator& other) const {
			auto a = static_cast<val<uint64_t>>(cursor_);
			auto b = static_cast<val<uint64_t>>(other.cursor_);
			return val<size_type>((a - b) / val<uint64_t>(sizeof(T)));
		}

		val<bool> operator==(const iterator& other) const {
			return cursor_ == other.cursor_;
		}
		val<bool> operator!=(const iterator& other) const {
			return cursor_ != other.cursor_;
		}
		val<bool> operator<(const iterator& other) const {
			return static_cast<val<uint64_t>>(cursor_) < static_cast<val<uint64_t>>(other.cursor_);
		}
		val<bool> operator>(const iterator& other) const {
			return static_cast<val<uint64_t>>(cursor_) > static_cast<val<uint64_t>>(other.cursor_);
		}
		val<bool> operator<=(const iterator& other) const {
			return static_cast<val<uint64_t>>(cursor_) <= static_cast<val<uint64_t>>(other.cursor_);
		}
		val<bool> operator>=(const iterator& other) const {
			return static_cast<val<uint64_t>>(cursor_) >= static_cast<val<uint64_t>>(other.cursor_);
		}

		val<T*> base() const {
			return cursor_;
		}

	private:
		val<T*> cursor_;
	};

	iterator begin() {
		return iterator(load_data_ptr());
	}

	iterator end() {
		return iterator(load_finish_ptr());
	}

private:
	val<base_type*> data_ptr;
};

} // namespace nautilus

#pragma once

#include "nautilus/val_ptr.hpp"
#include <cstdint>
#include <cstring>
#include <vector>

namespace nautilus {

namespace detail {

/// Byte offset of the data pointer (`_M_start` / `__begin_` / `_Myfirst`)
/// inside a std::vector. All major standard libraries store the data
/// pointer as one of the first pointer-sized members; we probe the offset
/// at static-init time to avoid hard-coding the layout.
template <typename T, typename Allocator>
inline std::size_t probe_vector_data_offset() {
	std::vector<T, Allocator> v;
	v.reserve(1);
	v.push_back(T {});
	const auto* data = static_cast<const void*>(v.data());
	const auto* base = reinterpret_cast<const std::byte*>(&v);
	for (std::size_t i = 0; i + sizeof(void*) <= sizeof(v); i += alignof(void*)) {
		void* slot;
		std::memcpy(&slot, base + i, sizeof(void*));
		if (slot == data) {
			return i;
		}
	}
	return 0;
}

template <typename T, typename Allocator>
inline const std::size_t vector_data_offset = probe_vector_data_offset<T, Allocator>();

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
		auto data_field_byte_ptr = byte_ptr + val<uint64_t>(detail::vector_data_offset<T, Allocator>);
		auto data_field_ptr = static_cast<val<T**>>(static_cast<val<void*>>(data_field_byte_ptr));
		return *data_field_ptr;
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
		if constexpr (std::is_class_v<T>) {
			return invoke(
			    +[](base_type* ptr) -> T* { return &ptr->back(); }, data_ptr);
		} else {
			return invoke(
			    +[](base_type* ptr) -> T { return ptr->back(); }, data_ptr);
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
		return invoke(
		    +[](base_type* ptr) -> bool { return ptr->empty(); }, data_ptr);
	}

	val<size_type> size() {
		return invoke(
		    +[](base_type* ptr) -> size_type { return ptr->size(); }, data_ptr);
	}

	val<size_type> capacity() {
		return invoke(
		    +[](base_type* ptr) -> size_type { return ptr->capacity(); }, data_ptr);
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

private:
	val<base_type*> data_ptr;
};

} // namespace nautilus

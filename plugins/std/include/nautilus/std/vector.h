#pragma once

#include "nautilus/val_ptr.hpp"
#include <cstdint>
#include <cstring>
#include <vector>

namespace nautilus {

namespace detail {

/// Probes the byte offset of the data pointer field within std::vector<T>.
///
/// All major standard library implementations (libstdc++, libc++, MSVC STL)
/// store the data pointer as the first member of the vector layout, so this
/// is typically 0. We probe at static-init time to avoid hard-coding the
/// assumption.
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
	// Fall back to 0 — all known stdlib implementations match this.
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

	/// Loads the underlying data pointer of the vector via direct memory access
	/// at the probed layout offset. This becomes a single traced load operation
	/// that the JIT can optimize, instead of an opaque invoke() call to data().
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

	val<std::vector<T, Allocator>>& operator=(const val<std::vector<T, Allocator>>& other) {
		this->data_ptr = other.data_ptr;
		return *this;
	}

	// Element access — direct memory access (no invoke).
	// Returns val<T> for fundamental/pointer types, val<T*> for class types.

	auto operator[](val<size_type> pos) {
		auto data = load_data_ptr();
		if constexpr (std::is_class_v<T>) {
			// Byte arithmetic for struct element access (val<Struct*>::operator[] is not provided).
			auto byte_data = static_cast<val<uint8_t*>>(static_cast<val<void*>>(data));
			auto elem_byte_ptr = byte_data + pos * val<uint64_t>(sizeof(T));
			return static_cast<val<T*>>(static_cast<val<void*>>(elem_byte_ptr));
		} else {
			// data[pos] returns val<T&>; convert to val<T> to perform a load.
			return val<T>(data[pos]);
		}
	}

	auto front() {
		auto data = load_data_ptr();
		if constexpr (std::is_class_v<T>) {
			return data;
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

	// Capacity

	val<bool> empty() const {
		return invoke(
		    +[](base_type* ptr) -> bool { return ptr->empty(); }, data_ptr);
	}

	val<size_type> size() const {
		return invoke(
		    +[](base_type* ptr) -> size_type { return ptr->size(); }, data_ptr);
	}

	void reserve(val<size_type> new_cap) {
		invoke(
		    +[](base_type* ptr, size_type cap) -> void { ptr->reserve(cap); }, data_ptr, new_cap);
	}

	val<size_type> capacity() const {
		return invoke(
		    +[](base_type* ptr) -> size_type { return ptr->capacity(); }, data_ptr);
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

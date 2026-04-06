#pragma once

#include "nautilus/val_ptr.hpp"
#include <vector>

namespace nautilus {
template <class T, class Allocator>
class val<std::vector<T, Allocator>> {
	using base_type = std::vector<T, Allocator>;
	using value_type = typename base_type::value_type;
	using size_type = typename base_type::size_type;

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

	// Element access — returns val<T> for fundamental/pointer types, val<T*> for class types

	auto at(val<size_type> pos) {
		if constexpr (std::is_class_v<T>) {
			return invoke(
			    +[](base_type* ptr, size_type p) -> T* { return &ptr->at(p); }, data_ptr, pos);
		} else {
			return invoke(
			    +[](base_type* ptr, size_type p) -> T { return ptr->at(p); }, data_ptr, pos);
		}
	}

	auto operator[](val<size_type> pos) {
		if constexpr (std::is_class_v<T>) {
			return invoke(
			    +[](base_type* ptr, size_type p) -> T* { return &ptr->operator[](p); }, data_ptr, pos);
		} else {
			return invoke(
			    +[](base_type* ptr, size_type p) -> T { return ptr->operator[](p); }, data_ptr, pos);
		}
	}

	auto front() {
		if constexpr (std::is_class_v<T>) {
			return invoke(
			    +[](base_type* ptr) -> T* { return &ptr->front(); }, data_ptr);
		} else {
			return invoke(
			    +[](base_type* ptr) -> T { return ptr->front(); }, data_ptr);
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

	val<T*> data() {
		return invoke(
		    +[](base_type* ptr) -> T* { return ptr->data(); }, data_ptr);
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

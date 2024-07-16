#pragma once

#include <nautilus/Engine.hpp>
#include <nautilus/std/cstring.h>

namespace nautilus {

val<int32_t> load(val<int32_t*> array, val<int32_t> index) {
	val<int32_t> value = array[index];
	return value;
}

int32_t globalPtr;
val<int32_t> loadConst() {
	val<int32_t> value = *val<int32_t*>(&globalPtr);
	return value;
}

val<int32_t> castVoidPtr(val<void*> array) {
	auto intPtr = static_cast<val<int32_t*>>(array);
	return intPtr[0];
}

template <typename A, typename B>
val<B> castPtrAndGetValue(val<A*> array) {
	auto intPtr = static_cast<val<B*>>(array);
	return intPtr[0];
}

val<int32_t> sumArray(val<int32_t*> array, val<int32_t> length) {
	val<int32_t> sum = val<int32_t>(0);
	for (val<int32_t> i = 0; i < length; i = i + 1) {
		val<int32_t> value = array[i];
		sum = sum + value;
	}
	return sum;
}

template <typename T>
void addArray(val<T*> array, val<T*> array2, val<T> length) {
	for (val<T> i = 0; i < length; i = i + 1) {
		val<T> left = array[i];
		val<T> right = array2[i];
		array[i] = left + right;
	}
}

void callMemcpy(val<int32_t*> src, val<int32_t*> dest) {
	memcpy(dest, src, 0);
}

class BaseClass {};

class CustomClass : public BaseClass {
public:
	int x;
};

val<int32_t> passCustomClass(val<CustomClass*> customClassPtr) {
	return invoke<>(+[](CustomClass* ptr) { return ptr->x; }, customClassPtr);
}

val<int32_t> castCustomClass(val<BaseClass*> voidPtr) {
	// cast base struct to custom struct
	auto resultPtr = static_cast<val<CustomClass*>>(voidPtr);
	return invoke<>(+[](CustomClass* ptr) { return ptr->x; }, resultPtr);
}

struct CustomStruct2 {
	int x;
};

/*
 * Specialize the pointer type to implement some custom wrapper functions
 */
template <>
class val<CustomStruct2*> : public base_ptr_val<CustomStruct2*> {
public:
	using base_ptr_val<CustomStruct2*>::base_ptr_val;
	val<int32_t> getX() {
		return invoke<>(+[](CustomStruct2* ptr) { return ptr->x; }, *this);
	}
};

val<int32_t> specializeType(val<CustomStruct2*> customClassPtr) {
	return customClassPtr.getX();
}

class WrapperType {
public:
	val<int32_t> add() {
		return a.getX() + b.getX();
	};
	val<CustomStruct2*> a;
	val<CustomStruct2*> b;
};

val<int32_t> useWrapper(val<CustomStruct2*> a, val<CustomStruct2*> b) {
	WrapperType wrapper = {a, b};
	return wrapper.add();
}

} // namespace nautilus

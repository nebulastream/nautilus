#pragma once

#include <cstdint>
#include <nautilus/Engine.hpp>
#include <nautilus/std/cstring.h>
#include <nautilus/val.hpp>
#include <nautilus/val_ptr.hpp>

namespace nautilus {

inline val<int32_t> load(val<int32_t*> array, val<int32_t> index) {
	val<int32_t> value = array[index];
	return value;
}

inline int32_t globalPtr;
inline val<int32_t> loadConst() {
	val<int32_t> value = *val<int32_t*>(&globalPtr);
	return value;
}

inline val<int32_t> castVoidPtr(val<void*> array) {
	auto intPtr = static_cast<val<int32_t*>>(array);
	return intPtr[0];
}

inline val<int32_t> ptrAssignment(val<int32_t*> ptr, val<int32_t*> ptr2) {
	auto tempPtr = ptr;
	ptr = ptr2;
	return val<int32_t>(*ptr) + val<int32_t>(*tempPtr);
}

template <typename T>
val<bool> isNullptr(val<T*> ptr) {
	return ptr == nullptr;
}

template <typename T>
val<bool> isNotNullptr(val<T*> ptr) {
	return ptr != nullptr;
}

template <typename T>
val<bool> ptrEquals(val<T*> left, val<T*> right) {
	return left == right;
}

template <typename T>
val<bool> ptrNotEquals(val<T*> left, val<T*> right) {
	return left != right;
}

template <typename T>
val<bool> ptrLessThan(val<T*> left, val<T*> right) {
	return left < right;
}

template <typename T>
val<bool> ptrLessThanEquals(val<T*> left, val<T*> right) {
	return left <= right;
}

template <typename T>
val<bool> ptrGreaterThan(val<T*> left, val<T*> right) {
	return left > right;
}

template <typename T>
val<bool> ptrGreaterThanEquals(val<T*> left, val<T*> right) {
	return left >= right;
}

template <typename A, typename B>
val<B> castPtrAndGetValue(val<A*> array) {
	auto intPtr = static_cast<val<B*>>(array);
	return intPtr[0];
}

template <typename A, typename B, typename C>
val<C> divAWithB(val<A> a, val<B> b) {
	return a / b;
}

template <typename A, typename B, typename C>
val<C> mulAWithB(val<A> a, val<B> b) {
	return a * b;
}

template <typename A, typename B, typename C>
val<C> subAWithB(val<A> a, val<B> b) {
	return a - b;
}

template <typename A, typename B, typename C>
val<C> addAWithB(val<A> a, val<B> b) {
	return a + b;
}

inline val<int32_t> pointerAdd(val<int32_t*> ptr, val<int32_t> offset) {
	auto result = ptr + offset;
	return *result;
}

inline val<int32_t> pointerSub(val<int32_t*> ptr, val<int32_t> offset) {
	auto result = ptr - offset;
	return *result;
}

inline val<int32_t> pointerAddConst(val<int32_t*> ptr) {
	auto result = ptr + 2;
	return *result;
}

inline val<int32_t> pointerSubConst(val<int32_t*> ptr) {
	auto result = ptr - 2;
	return *result;
}

inline val<int32_t> sumArray(val<int32_t*> array, val<int32_t> length) {
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

inline void callMemcpy(val<int32_t*> src, val<int32_t*> dest) {
	memcpy(dest, src, 0);
}

inline val<int32_t> pointerAddNegativeOffset(val<int32_t*> ptr, val<int32_t> offset) {
	auto result = ptr - offset;
	return *result;
}

inline val<bool> pointerLessThanAfterAdd(val<int32_t*> base, val<int32_t> offset) {
	auto advanced = base + offset;
	return base < advanced;
}

inline val<bool> pointerGreaterThanAfterAdd(val<int32_t*> base, val<int32_t> offset) {
	auto advanced = base + offset;
	return advanced > base;
}

inline val<int32_t> pointerRoundTrip(val<int32_t*> ptr, val<int32_t> offset) {
	auto advanced = ptr + offset;
	auto back = advanced - offset;
	return *back;
}

inline val<int32_t> pointerMultiStep(val<int32_t*> ptr) {
	auto p1 = ptr + 1;
	auto p2 = p1 + 1;
	auto p3 = p2 + 1;
	return *p3;
}

class BaseClass {};

class CustomClass : public BaseClass {
public:
	int x;
	int y;
	int z;
};

inline val<CustomClass*> customPointerAdd(val<CustomClass*> customClassPtr, val<int32_t> offset) {
	auto result = customClassPtr + offset;
	return result;
}

template <typename PtrElem, typename OffsetType>
val<PtrElem> pointerAddInt(val<PtrElem*> ptr, val<OffsetType> offset) {
	auto result = ptr + offset;
	return *result;
}

template <typename PtrElem, typename OffsetType>
val<PtrElem> pointerSubInt(val<PtrElem*> ptr, val<OffsetType> offset) {
	auto result = ptr - offset;
	return *result;
}

template <typename PtrElem>
val<PtrElem> pointerAddConstInt(val<PtrElem*> ptr) {
	auto result = ptr + 2;
	return *result;
}

template <typename PtrElem>
val<PtrElem> pointerSubConstInt(val<PtrElem*> ptr) {
	auto result = ptr - 2;
	return *result;
}

template <typename PtrElem, typename OffsetType>
val<PtrElem> pointerPlusAssign(val<PtrElem*> ptr, val<OffsetType> offset) {
	ptr += offset;
	return *ptr;
}

template <typename PtrElem, typename OffsetType>
val<PtrElem> pointerMinusAssign(val<PtrElem*> ptr, val<OffsetType> offset) {
	ptr -= offset;
	return *ptr;
}

template <typename PtrElem>
val<PtrElem> pointerPreIncrement(val<PtrElem*> ptr) {
	++ptr;
	return *ptr;
}

inline val<int32_t> passCustomClass(val<CustomClass*> customClassPtr) {
	return invoke<>(+[](CustomClass* ptr) { return ptr->x; }, customClassPtr);
}

inline val<int32_t> castCustomClass(val<BaseClass*> voidPtr) {
	// cast base struct to custom struct
	auto resultPtr = static_cast<val<CustomClass*>>(voidPtr);
	return invoke<>(+[](CustomClass* ptr) { return ptr->x; }, resultPtr);
}

struct FieldStruct {
	int x;
};

inline val<int32_t> getField(val<FieldStruct*> ptr) {
	// cast base struct to custom struct
	val<int32_t> intValue = ptr.get(&FieldStruct::x);
	val<int32_t&> intRef = ptr.get(&FieldStruct::x);
	return intValue + intRef;
}

inline void setFieldConst(val<FieldStruct*> ptr) {
	ptr.set(&FieldStruct::x, 42);
}

inline void setFieldIndirect(val<FieldStruct*> ptr, val<int32_t> x) {
	ptr.set(&FieldStruct::x, x);
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

inline val<int32_t> specializeType(val<CustomStruct2*> customClassPtr) {
	return customClassPtr.getX();
}

class WrapperType {
public:
	val<int32_t> add() {
		return a.getX() + b.getX();
	}
	val<CustomStruct2*> a;
	val<CustomStruct2*> b;
};

inline val<int32_t> useWrapper(val<CustomStruct2*> a, val<CustomStruct2*> b) {
	WrapperType wrapper = {a, b};
	return wrapper.add();
}

} // namespace nautilus

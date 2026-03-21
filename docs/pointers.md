# Pointers and Memory

## Overview

Nautilus supports pointer operations through `val<T*>`. Pointers can be used for array access, dereferencing, and memory operations. The tracer captures all pointer-based reads and writes so that the compiled code correctly handles memory access patterns.

## Creating Pointer Values

Pointer values can be created from raw pointers or by taking the address of an existing variable.

```cpp
// From a raw pointer
int32_t* raw_ptr = new int32_t[10];
val<int32_t*> ptr = raw_ptr;

// From address-of
int32_t globalVar;
val<int32_t*> ptr = &globalVar;
// Inside traced code:
val<int32_t> value = *val<int32_t*>(&globalVar);
```

## Array Indexing

The `[]` operator provides pointer-based array access, just like raw C++ pointers.

```cpp
val<int32_t> load(val<int32_t*> array, val<int32_t> index) {
    val<int32_t> value = array[index];
    return value;
}
```

A more complete example using a mask array for conditional summation:

```cpp
val<int32_t> conditionalSum(val<int32_t> size, val<bool*> mask, val<int32_t*> array) {
    val<int32_t> sum = 0;
    for (val<int32_t> i = 0; i < size; i++) {
        if (mask[i]) {
            sum += array[i];
        }
    }
    return sum;
}
```

## Dereferencing

Use the `*` operator to read the value that a pointer points to.

```cpp
val<int32_t> value = *ptr;
```

## Writing Through Pointers

Assign to a dereferenced pointer to perform a store operation.

```cpp
void writeToPtr(val<int32_t*> x) {
    val<int32_t> val = *x;
    if (val > 10) {
        *x = 1;
        return;
    }
    *x = 42;
}
```

## Pointer Comparisons

Pointers support null checks, equality comparisons, and ordering comparisons.

```cpp
// Null checks
val<bool> isNull = (ptr == nullptr);
val<bool> isNotNull = (ptr != nullptr);

// Pointer equality
val<bool> equal = (ptr1 == ptr2);
val<bool> notEqual = (ptr1 != ptr2);

// Pointer ordering
val<bool> less = (ptr1 < ptr2);
val<bool> lessEq = (ptr1 <= ptr2);
val<bool> greater = (ptr1 > ptr2);
val<bool> greaterEq = (ptr1 >= ptr2);
```

## Void Pointer Casting

Use `static_cast` to cast a `val<void*>` to a typed pointer before accessing the underlying data.

```cpp
val<int32_t> castVoidPtr(val<void*> data) {
    auto intPtr = static_cast<val<int32_t*>>(data);
    return intPtr[0];
}
```

## Pointer Assignment

Pointers can be reassigned. Copies of a pointer value before reassignment retain the original address.

```cpp
val<int32_t> ptrAssignment(val<int32_t*> ptr, val<int32_t*> ptr2) {
    auto tempPtr = ptr;
    ptr = ptr2;
    return val<int32_t>(*ptr) + val<int32_t>(*tempPtr);
}
```

## Compound Assignment Through References

When the `[]` operator returns a reference, compound assignment operators work as expected.

```cpp
// These work with pointer-based array access
array[i] += value;
array[i] -= value;
array[i] *= value;
```

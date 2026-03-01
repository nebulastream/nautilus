
#pragma once

#include "nautilus/function.hpp"
#include <cstdint>
#include <nautilus/Engine.hpp>
#include <nautilus/std/cstring.h>
#include <nautilus/val.hpp>
#include <nautilus/val_ptr.hpp>
#include <nautilus/val_std.hpp>

namespace nautilus {

struct Test {
	int32_t a;
	int32_t b;
};

void addFunc(Test* res, int32_t a, int32_t b) {
	res->a = a + b;
}

// Struct with non-trivial default and parameterised constructors.
// is_trivially_default_constructible = false → invoke(construct, ...) is traced.
struct TestWithCtor {
	int32_t a;
	int32_t b;
	TestWithCtor() : a(0), b(0) {
	}
	TestWithCtor(int32_t a, int32_t b) : a(a), b(b) {
	}
};

// Struct with a non-trivial destructor.
// is_trivially_destructible = false  → invoke(destruct, ...) is traced.
// is_trivially_copyable    = false  → copies use copy_construct / copy_assign via invoke().
struct TestWithDtor {
	int32_t value;
	TestWithDtor() : value(0) {
	}
	TestWithDtor(const TestWithDtor&) = default;
	TestWithDtor& operator=(const TestWithDtor&) = default;
	~TestWithDtor() {
	}
};

// --- Default constructor tests ---

// Default-construct (trivial ctor elided), set field a, read it back.
val<int32_t> constructAndAccess() {
	val<Test> value;
	value.set(&Test::a, 42);
	return value.get(&Test::a);
}

// Default-construct (trivial ctor elided), set both fields, return their sum.
val<int32_t> constructSetBothFields() {
	val<Test> value;
	value.set(&Test::a, 10);
	value.set(&Test::b, 32);
	return value.get(&Test::a) + value.get(&Test::b);
}

// Default-construct via non-trivial ctor (invoke traced), set fields, read them back.
val<int32_t> constructNonTrivialDefault() {
	val<TestWithCtor> value;
	value.set(&TestWithCtor::a, 7);
	return value.get(&TestWithCtor::a);
}

val<int32_t> constructAndCall(val<int32_t> x) {
	val<Test> value;
	invoke(addFunc, &value, x, x);
	return value.get(&Test::a);
}

// --- Parameterised constructor test ---

// Construct with two val<int32_t> arguments; invoke(construct_with<int32_t,int32_t>,...) is traced.
val<int32_t> constructWithArgs(val<int32_t> x, val<int32_t> y) {
	val<TestWithCtor> value(x, y);
	return value.get(&TestWithCtor::a) + value.get(&TestWithCtor::b);
}

// --- Copy constructor and copy assignment tests ---

// Copy-construct: trivially-copyable Test → invoke(trivial_copy, ...) is traced.
// A const-ref is used so that the non-template copy constructor beats the variadic template
// in overload resolution (both become equally-good exact matches; non-template wins the tie).
val<int32_t> copyConstruct() {
	val<Test> original;
	original.set(&Test::a, 42);
	original.set(&Test::b, 10);
	const val<Test>& ref = original;
	val<Test> copy(ref);
	return copy.get(&Test::a) + copy.get(&Test::b);
}

// Copy-assign: trivially-copyable Test → invoke(trivial_copy, ...) is traced.
val<int32_t> copyAssign() {
	val<Test> original;
	original.set(&Test::a, 42);
	val<Test> other;
	other = original;
	return other.get(&Test::a);
}

// Copy-construct of non-trivially-copyable type → invoke(copy_construct, ...) is traced.
val<int32_t> copyConstructNonTrivial() {
	val<TestWithDtor> original;
	original.set(&TestWithDtor::value, 99);
	const val<TestWithDtor>& ref = original;
	val<TestWithDtor> copy(ref);
	return copy.get(&TestWithDtor::value);
}

// --- Destructor test ---

// Non-trivial destructor: invoke(destruct, ...) is recorded when value goes out of scope.
val<int32_t> nonTrivialDestructor() {
	val<TestWithDtor> value;
	value.set(&TestWithDtor::value, 42);
	return value.get(&TestWithDtor::value);
}

// --- Loop tests ---

// val<Test> lives outside the loop; field a is incremented on every iteration.
val<int32_t> modifyInLoop(val<int32_t> count) {
	val<Test> value;
	value.set(&Test::a, 0);
	for (val<int32_t> i = 0; i < count; i = i + 1) {
		value.set(&Test::a, value.get(&Test::a) + 1);
	}
	return value.get(&Test::a);
}

// A fresh copy of base is made inside each loop iteration (alloca + trivial_copy per iteration).
val<int32_t> copyInLoop(val<int32_t> count) {
	val<Test> base;
	base.set(&Test::a, 10);
	const val<Test>& base_ref = base;
	val<int32_t> sum = 0;
	for (val<int32_t> i = 0; i < count; i = i + 1) {
		val<Test> copy(base_ref);
		sum = sum + copy.get(&Test::a);
	}
	return sum;
}

} // namespace nautilus

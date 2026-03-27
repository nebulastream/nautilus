
#pragma once

#include "nautilus/function.hpp"
#include <cstdint>
#include <nautilus/Engine.hpp>
#include <nautilus/nautilus_function.hpp>
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

// Structs with mixed-alignment members to exercise field_offset padding.
// Layout: [i8][3 pad][i32][i64] = 16 bytes, alignof = 8
struct MixedAlign {
	int8_t small;
	int32_t medium;
	int64_t large;
};

// Layout: [i64][i8][1 pad][i16][4 pad] = 16 bytes, alignof = 8
struct ReversePadding {
	int64_t first;
	int8_t second;
	int16_t third;
};

// Layout: [double][i8][7 pad] = 16 bytes, alignof = 8
struct DoubleAndByte {
	double value;
	int8_t flag;
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

// --- Nested function + struct tests ---

// Helper: takes a Test* and increments field a by a given amount.
val<int32_t> readFieldA(val<Test*> ptr) {
	return ptr.get(&Test::a);
}

static auto nautilusReadFieldA = NautilusFunction {"readFieldA", readFieldA};

void incrementFieldA(val<Test*> ptr, val<int32_t> amount) {
	ptr.set(&Test::a, ptr.get(&Test::a) + amount);
}

static auto nautilusIncrementFieldA = NautilusFunction {"incrementFieldA", incrementFieldA};

// Struct modified in a loop via a nested NautilusFunction call.
val<int32_t> modifyStructInLoopWithNestedCall(val<int32_t> count) {
	val<Test> value;
	value.set(&Test::a, 0);
	for (val<int32_t> i = 0; i < count; i = i + 1) {
		nautilusIncrementFieldA(&value, val<int32_t>(1));
	}
	return nautilusReadFieldA(&value);
}

// Struct constructed inside loop body, passed to nested function each iteration.
val<int32_t> constructStructInLoopWithNestedCall(val<int32_t> count) {
	val<int32_t> sum = 0;
	for (val<int32_t> i = 0; i < count; i = i + 1) {
		val<Test> tmp;
		tmp.set(&Test::a, i);
		sum = sum + nautilusReadFieldA(&tmp);
	}
	return sum;
}

// Helper: creates a Test with both fields set and returns a + b.
val<int32_t> sumFields(val<Test*> ptr) {
	return ptr.get(&Test::a) + ptr.get(&Test::b);
}

static auto nautilusSumFields = NautilusFunction {"sumFields", sumFields};

// Multiple nested function calls on the same struct inside a loop.
val<int32_t> multipleNestedCallsInLoop(val<int32_t> count) {
	val<Test> value;
	value.set(&Test::a, 0);
	value.set(&Test::b, 100);
	for (val<int32_t> i = 0; i < count; i = i + 1) {
		nautilusIncrementFieldA(&value, nautilusSumFields(&value));
	}
	return nautilusReadFieldA(&value);
}

// --- Struct in conditional return paths ---

// Construct a struct inside a conditional branch that also returns.
// The ALLOCA and RETURN end up in the same non-initial block.
// When hoistAllocaOperations() erases the ALLOCA, the returnRef's
// operationIndex becomes stale (points OOB or to the wrong op).
val<int32_t> structInConditionalReturn(val<int32_t> x) {
	if (x > 0) {
		val<Test> value;
		value.set(&Test::a, x);
		value.set(&Test::b, 10);
		return value.get(&Test::a) + value.get(&Test::b);
	}
	return x;
}

// Two structs constructed in different branches, both returning.
val<int32_t> structInBothBranches(val<int32_t> x) {
	if (x > 0) {
		val<Test> a;
		a.set(&Test::a, x);
		return a.get(&Test::a);
	} else {
		val<Test> b;
		b.set(&Test::a, 42);
		return b.get(&Test::a);
	}
}

// Struct constructed in a nested function called from a conditional return path.
val<int32_t> structInNestedConditionalReturn(val<int32_t> x) {
	if (x > 0) {
		val<Test> value;
		value.set(&Test::a, x);
		return nautilusReadFieldA(&value);
	}
	return val<int32_t>(0);
}

// --- Mixed-alignment struct tests ---

// Set and read all three fields with different sizes/alignments.
val<int64_t> mixedAlignSetAll(val<int8_t> s, val<int32_t> m, val<int64_t> l) {
	val<MixedAlign> value;
	value.set(&MixedAlign::small, s);
	value.set(&MixedAlign::medium, m);
	value.set(&MixedAlign::large, l);
	val<int8_t> rs = value.get(&MixedAlign::small);
	val<int32_t> rm = value.get(&MixedAlign::medium);
	val<int64_t> rl = value.get(&MixedAlign::large);
	return static_cast<val<int64_t>>(rs) + static_cast<val<int64_t>>(rm) + rl;
}

// Verify that writing one field does not clobber an adjacent padded field.
val<int64_t> mixedAlignNoClobber() {
	val<MixedAlign> value;
	value.set(&MixedAlign::large, val<int64_t>(1000));
	value.set(&MixedAlign::small, val<int8_t>(1));
	value.set(&MixedAlign::medium, val<int32_t>(100));
	return value.get(&MixedAlign::large);
}

// Reverse-ordered padding: i64, i8, i16 with internal padding.
val<int64_t> reversePaddingAccess(val<int64_t> a, val<int8_t> b, val<int16_t> c) {
	val<ReversePadding> value;
	value.set(&ReversePadding::first, a);
	value.set(&ReversePadding::second, b);
	value.set(&ReversePadding::third, c);
	val<int64_t> r1 = value.get(&ReversePadding::first);
	val<int8_t> r2 = value.get(&ReversePadding::second);
	val<int16_t> r3 = value.get(&ReversePadding::third);
	return r1 + static_cast<val<int64_t>>(r2) + static_cast<val<int64_t>>(r3);
}

// Double + int8_t with trailing padding.
val<double> doubleAndByteAccess(val<double> d, val<int8_t> f) {
	val<DoubleAndByte> value;
	value.set(&DoubleAndByte::value, d);
	value.set(&DoubleAndByte::flag, f);
	val<double> rd = value.get(&DoubleAndByte::value);
	val<int8_t> rf = value.get(&DoubleAndByte::flag);
	return rd + static_cast<val<double>>(rf);
}

// Mixed-alignment struct modified in a loop.
val<int64_t> mixedAlignModifyInLoop(val<int32_t> count) {
	val<MixedAlign> value;
	value.set(&MixedAlign::small, val<int8_t>(0));
	value.set(&MixedAlign::medium, val<int32_t>(0));
	value.set(&MixedAlign::large, val<int64_t>(0));
	for (val<int32_t> i = 0; i < count; i = i + 1) {
		val<int8_t> curS = value.get(&MixedAlign::small);
		value.set(&MixedAlign::small, static_cast<val<int8_t>>(curS + val<int8_t>(1)));
		val<int32_t> curM = value.get(&MixedAlign::medium);
		value.set(&MixedAlign::medium, curM + val<int32_t>(10));
		val<int64_t> curL = value.get(&MixedAlign::large);
		value.set(&MixedAlign::large, curL + val<int64_t>(100));
	}
	val<int8_t> fs = value.get(&MixedAlign::small);
	val<int32_t> fm = value.get(&MixedAlign::medium);
	val<int64_t> fl = value.get(&MixedAlign::large);
	return static_cast<val<int64_t>>(fs) + static_cast<val<int64_t>>(fm) + fl;
}

// Mixed-alignment struct in a conditional return path (tombstone regression).
val<int64_t> mixedAlignConditionalReturn(val<int64_t> x) {
	if (x > 0) {
		val<MixedAlign> value;
		value.set(&MixedAlign::small, val<int8_t>(1));
		value.set(&MixedAlign::medium, val<int32_t>(2));
		value.set(&MixedAlign::large, x);
		val<int8_t> rs = value.get(&MixedAlign::small);
		val<int32_t> rm = value.get(&MixedAlign::medium);
		val<int64_t> rl = value.get(&MixedAlign::large);
		return static_cast<val<int64_t>>(rs) + static_cast<val<int64_t>>(rm) + rl;
	}
	return x;
}

// --- Multi-struct, multi-loop tests ---

// Multiple sequential loops, each constructing a different struct type inside.
// Produces many ALLOCAs across different blocks, stressing the tombstone mechanism.
val<int64_t> multiStructMultiLoop(val<int32_t> count) {
	val<int64_t> result = 0;

	// Loop 1: MixedAlign struct per iteration
	for (val<int32_t> i = 0; i < count; i = i + 1) {
		val<MixedAlign> ma;
		ma.set(&MixedAlign::small, val<int8_t>(1));
		ma.set(&MixedAlign::medium, val<int32_t>(10));
		ma.set(&MixedAlign::large, val<int64_t>(100));
		result = result + ma.get(&MixedAlign::large);
	}

	// Loop 2: ReversePadding struct per iteration
	for (val<int32_t> i = 0; i < count; i = i + 1) {
		val<ReversePadding> rp;
		rp.set(&ReversePadding::first, val<int64_t>(200));
		rp.set(&ReversePadding::second, val<int8_t>(2));
		rp.set(&ReversePadding::third, val<int16_t>(20));
		result = result + rp.get(&ReversePadding::first);
	}

	// Loop 3: plain Test struct per iteration
	for (val<int32_t> i = 0; i < count; i = i + 1) {
		val<Test> t;
		t.set(&Test::a, val<int32_t>(30));
		t.set(&Test::b, val<int32_t>(3));
		val<int32_t> sum = t.get(&Test::a) + t.get(&Test::b);
		result = result + static_cast<val<int64_t>>(sum);
	}

	return result;
}

// Two different struct types allocated in the same loop body.
val<int64_t> twoStructsSameLoop(val<int32_t> count) {
	val<int64_t> result = 0;
	for (val<int32_t> i = 0; i < count; i = i + 1) {
		val<MixedAlign> ma;
		ma.set(&MixedAlign::large, val<int64_t>(10));

		val<ReversePadding> rp;
		rp.set(&ReversePadding::first, val<int64_t>(20));

		result = result + ma.get(&MixedAlign::large) + rp.get(&ReversePadding::first);
	}
	return result;
}

// Struct allocated outside the loop plus a different struct inside the loop.
val<int64_t> outerAndInnerStructLoop(val<int32_t> count) {
	val<MixedAlign> outer;
	outer.set(&MixedAlign::large, val<int64_t>(0));

	for (val<int32_t> i = 0; i < count; i = i + 1) {
		val<Test> inner;
		inner.set(&Test::a, val<int32_t>(5));
		val<int64_t> cur = outer.get(&MixedAlign::large);
		val<int32_t> innerA = inner.get(&Test::a);
		outer.set(&MixedAlign::large, cur + static_cast<val<int64_t>>(innerA));
	}
	return outer.get(&MixedAlign::large);
}

// Multiple structs in conditional branches with a loop — combines the
// tombstone regression path with multiple ALLOCAs per branch.
val<int64_t> multiStructConditionalLoop(val<int32_t> count, val<int64_t> selector) {
	if (selector > 0) {
		val<int64_t> result = 0;
		for (val<int32_t> i = 0; i < count; i = i + 1) {
			val<MixedAlign> ma;
			ma.set(&MixedAlign::large, val<int64_t>(7));
			val<ReversePadding> rp;
			rp.set(&ReversePadding::first, val<int64_t>(3));
			result = result + ma.get(&MixedAlign::large) + rp.get(&ReversePadding::first);
		}
		return result;
	}
	return selector;
}

// --- Alloca merge bug: different-sized structs with non-overlapping lifetimes ---

struct Small {
	int32_t a;
	int32_t b;
}; // 8 bytes

struct Large {
	int64_t data[8];
}; // 64 bytes

void fillSmall(Small* s) {
	s->a = 42;
	s->b = 10;
}

void fillLarge(Large* l) {
	for (int i = 0; i < 8; i++) {
		l->data[i] = i + 1;
	}
}

int64_t sumLarge(Large* l) {
	int64_t sum = 0;
	for (int i = 0; i < 8; i++) {
		sum += l->data[i];
	}
	return sum;
}

// val<Small> goes out of scope before val<Large> is created.
// LLVM sees non-overlapping lifetimes and may merge both allocas,
// using the smaller size. fillLarge then writes 64 bytes into an
// 8-byte slot — stack corruption.
val<int64_t> allocaMergeBug() {
	val<int32_t> smallResult;
	{
		val<Small> s;
		invoke(fillSmall, &s);
		smallResult = s.get(&Small::a);
	}
	val<Large> l;
	invoke(fillLarge, &l);
	val<int64_t> largeSum = invoke(sumLarge, &l);
	return largeSum + static_cast<val<int64_t>>(smallResult);
}

// Same pattern but in a loop: each iteration constructs Small then Large.
val<int64_t> allocaMergeInLoop(val<int32_t> count) {
	val<int64_t> result = 0;
	for (val<int32_t> i = 0; i < count; i = i + 1) {
		val<int32_t> tmp;
		{
			val<Small> s;
			invoke(fillSmall, &s);
			tmp = s.get(&Small::a);
		}
		val<Large> l;
		invoke(fillLarge, &l);
		val<int64_t> largeSum = invoke(sumLarge, &l);
		result = result + largeSum + static_cast<val<int64_t>>(tmp);
	}
	return result;
}

} // namespace nautilus

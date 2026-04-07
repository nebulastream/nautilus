
#include "ExecutionTest.hpp"
#include "nautilus/Engine.hpp"
#include "nautilus/std/vector.h"
#include "nautilus/val_std.hpp"
#include <catch2/catch_all.hpp>

namespace nautilus::engine {

// --- Test structs ---

struct Point {
	int32_t x;
	int32_t y;
};

struct Pair {
	double first;
	double second;
	Pair() : first(0), second(0) {
	}
	Pair(double f, double s) : first(f), second(s) {
	}
};

// --- Element access (fundamental) ---

val<int32_t> vectorPushBackAndRead(val<int32_t> x) {
	val<std::vector<int32_t>> vec;
	vec.push_back(x);
	vec.push_back(val<int32_t>(42));
	return vec[val<size_t>(0)] + vec[val<size_t>(1)];
}

val<int32_t> vectorAt(val<int32_t> x) {
	val<std::vector<int32_t>> vec;
	vec.push_back(val<int32_t>(10));
	vec.push_back(x);
	vec.push_back(val<int32_t>(30));
	return vec.at(val<size_t>(1));
}

val<int32_t> vectorFrontBack(val<int32_t> x) {
	val<std::vector<int32_t>> vec;
	vec.push_back(val<int32_t>(10));
	vec.push_back(x);
	vec.push_back(val<int32_t>(30));
	return vec.front() + vec.back();
}

val<int32_t> vectorDataPtr(val<int32_t> x) {
	val<std::vector<int32_t>> vec;
	vec.push_back(x);
	vec.push_back(val<int32_t>(50));
	vec.push_back(val<int32_t>(60));
	auto ptr = vec.data();
	return ptr[val<size_t>(0)] + ptr[val<size_t>(1)] + ptr[val<size_t>(2)];
}

// --- Capacity ---

val<bool> vectorEmpty() {
	val<std::vector<int32_t>> vec;
	return vec.empty();
}

val<size_t> vectorNotEmptyAfterPush() {
	val<std::vector<int32_t>> vec;
	vec.push_back(val<int32_t>(1));
	return vec.size();
}

val<bool> vectorEmptyAfterClear() {
	val<std::vector<int32_t>> vec;
	vec.push_back(val<int32_t>(1));
	vec.push_back(val<int32_t>(2));
	vec.clear();
	return vec.empty();
}

val<size_t> vectorSize(val<int32_t> x) {
	val<std::vector<int32_t>> vec;
	vec.push_back(x);
	vec.push_back(val<int32_t>(2));
	vec.push_back(val<int32_t>(3));
	return vec.size();
}

val<bool> vectorReserveCapacity() {
	val<std::vector<int32_t>> vec;
	vec.reserve(val<size_t>(100));
	auto cap = vec.capacity();
	return cap >= val<size_t>(100);
}

// --- Modifiers ---

val<int32_t> vectorClear(val<int32_t> x) {
	val<std::vector<int32_t>> vec;
	vec.push_back(x);
	vec.clear();
	vec.push_back(val<int32_t>(99));
	return vec[val<size_t>(0)];
}

val<int32_t> vectorPopBack() {
	val<std::vector<int32_t>> vec;
	vec.push_back(val<int32_t>(1));
	vec.push_back(val<int32_t>(2));
	vec.push_back(val<int32_t>(3));
	vec.pop_back();
	return vec.back();
}

val<size_t> vectorResizeGrow() {
	val<std::vector<int32_t>> vec;
	vec.push_back(val<int32_t>(1));
	vec.resize(val<size_t>(5));
	return vec.size();
}

val<int32_t> vectorResizeGrowReadsDefault() {
	val<std::vector<int32_t>> vec;
	vec.push_back(val<int32_t>(42));
	vec.resize(val<size_t>(3));
	return vec[val<size_t>(1)] + vec[val<size_t>(2)];
}

val<size_t> vectorResizeShrink() {
	val<std::vector<int32_t>> vec;
	vec.push_back(val<int32_t>(1));
	vec.push_back(val<int32_t>(2));
	vec.push_back(val<int32_t>(3));
	vec.push_back(val<int32_t>(4));
	vec.resize(val<size_t>(2));
	return vec.size();
}

val<int32_t> vectorResizeWithValue(val<int32_t> fill) {
	val<std::vector<int32_t>> vec;
	vec.resize(val<size_t>(3), fill);
	return vec[val<size_t>(0)] + vec[val<size_t>(1)] + vec[val<size_t>(2)];
}

// --- Ownership & interop ---

val<std::vector<int32_t>*> vectorReturnPtr(val<int32_t> x) {
	val<std::vector<int32_t>> vec;
	vec.push_back(x);
	vec.push_back(val<int32_t>(100));
	return vec.release();
}

val<int32_t> vectorFromPtr(val<std::vector<int32_t>*> vec_ptr) {
	val<std::vector<int32_t>> vec(vec_ptr);
	auto result = vec[val<size_t>(0)] + vec[val<size_t>(1)];
	vec.release();
	return result;
}

// --- Assignment ---

val<int32_t> vectorAssign(val<int32_t> x) {
	val<std::vector<int32_t>> vec;
	vec.push_back(x);
	vec.push_back(val<int32_t>(20));
	val<std::vector<int32_t>> vec2;
	vec2 = vec;
	return vec2[val<size_t>(0)] + vec2[val<size_t>(1)];
}

// --- Subscript assignment (val<T&>) ---

val<int32_t> vectorAssignElement(val<int32_t> x) {
	val<std::vector<int32_t>> vec;
	vec.push_back(val<int32_t>(1));
	vec.push_back(val<int32_t>(2));
	vec[val<size_t>(1)] = x;
	return vec[val<size_t>(1)];
}

val<int32_t> vectorAssignOverwrite(val<int32_t> x) {
	val<std::vector<int32_t>> vec;
	vec.push_back(val<int32_t>(99));
	vec[val<size_t>(0)] = x;
	return vec.front();
}

val<double> vectorAssignDouble(val<double> x) {
	val<std::vector<double>> vec;
	vec.push_back(val<double>(0.0));
	vec.push_back(val<double>(0.0));
	vec[val<size_t>(0)] = x;
	vec[val<size_t>(1)] = x + val<double>(1.0);
	return vec[val<size_t>(0)] + vec[val<size_t>(1)];
}

// --- Optimized capacity ops ---

val<int32_t> vectorAssignFront(val<int32_t> x) {
	val<std::vector<int32_t>> vec;
	vec.push_back(val<int32_t>(1));
	vec.push_back(val<int32_t>(2));
	vec.front() = x;
	return vec[val<size_t>(0)];
}

val<int32_t> vectorAssignBack(val<int32_t> x) {
	val<std::vector<int32_t>> vec;
	vec.push_back(val<int32_t>(1));
	vec.push_back(val<int32_t>(2));
	vec.back() = x;
	return vec[val<size_t>(1)];
}

val<int32_t> vectorAssignViaData(val<int32_t> x) {
	val<std::vector<int32_t>> vec;
	vec.push_back(val<int32_t>(0));
	vec.push_back(val<int32_t>(0));
	vec.push_back(val<int32_t>(0));
	auto ptr = vec.data();
	ptr[val<size_t>(0)] = x;
	ptr[val<size_t>(1)] = x + val<int32_t>(1);
	ptr[val<size_t>(2)] = x + val<int32_t>(2);
	return vec[val<size_t>(0)] + vec[val<size_t>(1)] + vec[val<size_t>(2)];
}

val<int8_t> vectorAssignInt8(val<int8_t> x) {
	val<std::vector<int8_t>> vec;
	vec.push_back(val<int8_t>(0));
	vec[val<size_t>(0)] = x;
	return vec[val<size_t>(0)];
}

val<int64_t> vectorAssignInt64(val<int64_t> x) {
	val<std::vector<int64_t>> vec;
	vec.push_back(val<int64_t>(0));
	vec[val<size_t>(0)] = x;
	return vec[val<size_t>(0)];
}

val<float> vectorAssignFloat(val<float> x) {
	val<std::vector<float>> vec;
	vec.push_back(val<float>(0.0f));
	vec[val<size_t>(0)] = x;
	return vec[val<size_t>(0)];
}

// --- shrink_to_fit verification (capacity actually shrinks) ---

val<bool> vectorShrinkToFitActual() {
	val<std::vector<int32_t>> vec;
	vec.reserve(val<size_t>(1000));
	vec.push_back(val<int32_t>(1));
	auto cap_before = vec.capacity();
	vec.shrink_to_fit();
	auto cap_after = vec.capacity();
	return cap_after < cap_before;
}

// --- equals returning false ---

val<bool> vectorEqualsFalse() {
	val<std::vector<int32_t>> a;
	a.push_back(val<int32_t>(1));
	a.push_back(val<int32_t>(2));
	val<std::vector<int32_t>> b;
	b.push_back(val<int32_t>(1));
	b.push_back(val<int32_t>(3));
	return a.equals(b);
}

val<bool> vectorEqualsDifferentSize() {
	val<std::vector<int32_t>> a;
	a.push_back(val<int32_t>(1));
	val<std::vector<int32_t>> b;
	b.push_back(val<int32_t>(1));
	b.push_back(val<int32_t>(2));
	return a.equals(b);
}

// --- swap with pointer element type ---

val<int32_t> vectorSwapPtrs(val<int32_t*> a, val<int32_t*> b) {
	val<std::vector<int32_t*>> v1;
	v1.push_back(a);
	val<std::vector<int32_t*>> v2;
	v2.push_back(b);
	v1.swap(v2);
	return *v1[val<size_t>(0)] + *v2[val<size_t>(0)];
}

// --- Boundary sizes ---

val<size_t> vectorReserveZero() {
	val<std::vector<int32_t>> vec;
	vec.reserve(val<size_t>(0));
	return vec.size();
}

val<size_t> vectorResizeZero() {
	val<std::vector<int32_t>> vec;
	vec.push_back(val<int32_t>(1));
	vec.push_back(val<int32_t>(2));
	vec.resize(val<size_t>(0));
	return vec.size();
}

val<size_t> vectorSizeAfterPushPop() {
	val<std::vector<int32_t>> vec;
	vec.push_back(val<int32_t>(1));
	vec.push_back(val<int32_t>(2));
	vec.push_back(val<int32_t>(3));
	vec.pop_back();
	return vec.size();
}

val<bool> vectorCapacityAfterReserve() {
	val<std::vector<int32_t>> vec;
	vec.reserve(val<size_t>(50));
	return vec.capacity() >= val<size_t>(50);
}

// --- New methods ---

val<bool> vectorMaxSize() {
	val<std::vector<int32_t>> vec;
	return vec.max_size() > val<size_t>(0);
}

val<bool> vectorShrinkToFit() {
	val<std::vector<int32_t>> vec;
	vec.reserve(val<size_t>(100));
	vec.push_back(val<int32_t>(1));
	vec.shrink_to_fit();
	return vec.capacity() < val<size_t>(100);
}

val<int32_t> vectorSwap(val<int32_t> x) {
	val<std::vector<int32_t>> a;
	a.push_back(x);
	a.push_back(val<int32_t>(2));
	val<std::vector<int32_t>> b;
	b.push_back(val<int32_t>(100));
	a.swap(b);
	return a[val<size_t>(0)] + b[val<size_t>(0)];
}

val<bool> vectorEquals() {
	val<std::vector<int32_t>> a;
	a.push_back(val<int32_t>(1));
	a.push_back(val<int32_t>(2));
	val<std::vector<int32_t>> b;
	b.push_back(val<int32_t>(1));
	b.push_back(val<int32_t>(2));
	return a.equals(b);
}

// --- Fundamental element types ---

val<double> vectorDouble(val<double> x) {
	val<std::vector<double>> vec;
	vec.push_back(x);
	vec.push_back(val<double>(3.14));
	return vec[val<size_t>(0)] + vec[val<size_t>(1)];
}

val<int64_t> vectorInt64(val<int64_t> x) {
	val<std::vector<int64_t>> vec;
	vec.push_back(x);
	vec.push_back(val<int64_t>(1000000000000LL));
	return vec[val<size_t>(0)] + vec[val<size_t>(1)];
}

val<float> vectorFloat(val<float> x) {
	val<std::vector<float>> vec;
	vec.push_back(x);
	vec.push_back(val<float>(2.5f));
	return vec.front() + vec.back();
}

val<int8_t> vectorInt8(val<int8_t> x) {
	val<std::vector<int8_t>> vec;
	vec.push_back(x);
	vec.push_back(val<int8_t>(10));
	return vec[val<size_t>(0)] + vec[val<size_t>(1)];
}

// --- Combined operations ---

val<int32_t> vectorBuildAndSum(val<int32_t> x) {
	val<std::vector<int32_t>> vec;
	vec.reserve(val<size_t>(4));
	vec.push_back(x);
	vec.push_back(x + val<int32_t>(1));
	vec.push_back(x + val<int32_t>(2));
	vec.push_back(x + val<int32_t>(3));
	return vec[val<size_t>(0)] + vec[val<size_t>(1)] + vec[val<size_t>(2)] + vec[val<size_t>(3)];
}

val<size_t> vectorPopToEmpty() {
	val<std::vector<int32_t>> vec;
	vec.push_back(val<int32_t>(1));
	vec.push_back(val<int32_t>(2));
	vec.pop_back();
	vec.pop_back();
	return vec.size();
}

val<int32_t> vectorClearAndRefill(val<int32_t> x) {
	val<std::vector<int32_t>> vec;
	vec.push_back(val<int32_t>(100));
	vec.push_back(val<int32_t>(200));
	vec.clear();
	vec.push_back(x);
	return vec.front();
}

// --- Pointer element types ---

val<int32_t> vectorOfPtrsReadThrough(val<int32_t*> a, val<int32_t*> b) {
	val<std::vector<int32_t*>> vec;
	vec.push_back(a);
	vec.push_back(b);
	auto first = vec[val<size_t>(0)];
	auto second = vec[val<size_t>(1)];
	return *first + *second;
}

val<size_t> vectorOfPtrsSize(val<int32_t*> a) {
	val<std::vector<int32_t*>> vec;
	vec.push_back(a);
	vec.push_back(a);
	vec.push_back(a);
	return vec.size();
}

val<int32_t> vectorOfPtrsFrontBack(val<int32_t*> a, val<int32_t*> b, val<int32_t*> c) {
	val<std::vector<int32_t*>> vec;
	vec.push_back(a);
	vec.push_back(b);
	vec.push_back(c);
	return *vec.front() + *vec.back();
}

val<int32_t> vectorOfPtrsPopBack(val<int32_t*> a, val<int32_t*> b) {
	val<std::vector<int32_t*>> vec;
	vec.push_back(a);
	vec.push_back(b);
	vec.pop_back();
	return *vec.back();
}

val<double> vectorOfDoublePtrs(val<double*> a, val<double*> b) {
	val<std::vector<double*>> vec;
	vec.push_back(a);
	vec.push_back(b);
	return *vec[val<size_t>(0)] + *vec[val<size_t>(1)];
}

// --- Struct pointer element types ---

val<int32_t> vectorOfStructPtrs(val<Point*> a, val<Point*> b) {
	val<std::vector<Point*>> vec;
	vec.push_back(a);
	vec.push_back(b);
	auto first = vec[val<size_t>(0)];
	auto second = vec[val<size_t>(1)];
	return first.get(&Point::x) + second.get(&Point::y);
}

val<size_t> vectorOfStructPtrsSize(val<Point*> a) {
	val<std::vector<Point*>> vec;
	vec.push_back(a);
	vec.push_back(a);
	return vec.size();
}

val<int32_t> vectorOfStructPtrsFrontBack(val<Point*> a, val<Point*> b, val<Point*> c) {
	val<std::vector<Point*>> vec;
	vec.push_back(a);
	vec.push_back(b);
	vec.push_back(c);
	auto front = vec.front();
	auto back = vec.back();
	return front.get(&Point::x) + back.get(&Point::y);
}

val<int32_t> vectorOfStructPtrsAt(val<Point*> a, val<Point*> b) {
	val<std::vector<Point*>> vec;
	vec.push_back(a);
	vec.push_back(b);
	auto elem = vec.at(val<size_t>(1));
	return elem.get(&Point::x) + elem.get(&Point::y);
}

val<int32_t> vectorOfStructPtrsPopBack(val<Point*> a, val<Point*> b) {
	val<std::vector<Point*>> vec;
	vec.push_back(a);
	vec.push_back(b);
	vec.pop_back();
	auto elem = vec.back();
	return elem.get(&Point::x) + elem.get(&Point::y);
}

val<double> vectorOfPairPtrs(val<Pair*> a, val<Pair*> b) {
	val<std::vector<Pair*>> vec;
	vec.push_back(a);
	vec.push_back(b);
	auto first = vec[val<size_t>(0)];
	auto second = vec[val<size_t>(1)];
	return first.get(&Pair::first) + second.get(&Pair::second);
}

// --- Struct value types (operator[], at, front, back return val<T*> for class types) ---

val<int32_t> vectorOfStructValues(val<int32_t> x, val<int32_t> y) {
	val<std::vector<Point>> vec;
	val<Point> p;
	p.set(&Point::x, x);
	p.set(&Point::y, y);
	vec.push_back(&p);
	auto elem = vec[val<size_t>(0)];
	return elem.get(&Point::x) + elem.get(&Point::y);
}

val<int32_t> vectorOfStructValuesAt(val<int32_t> x, val<int32_t> y) {
	val<std::vector<Point>> vec;
	val<Point> p;
	p.set(&Point::x, x);
	p.set(&Point::y, y);
	vec.push_back(&p);
	auto elem = vec.at(val<size_t>(0));
	return elem.get(&Point::x) + elem.get(&Point::y);
}

val<size_t> vectorOfStructValuesSize(val<int32_t> x) {
	val<std::vector<Point>> vec;
	val<Point> p1;
	p1.set(&Point::x, x);
	p1.set(&Point::y, val<int32_t>(0));
	vec.push_back(&p1);
	val<Point> p2;
	p2.set(&Point::x, val<int32_t>(0));
	p2.set(&Point::y, x);
	vec.push_back(&p2);
	return vec.size();
}

val<int32_t> vectorOfStructValuesFrontBack(val<int32_t> x, val<int32_t> y) {
	val<std::vector<Point>> vec;
	val<Point> p1;
	p1.set(&Point::x, x);
	p1.set(&Point::y, val<int32_t>(0));
	vec.push_back(&p1);
	val<Point> p2;
	p2.set(&Point::x, val<int32_t>(0));
	p2.set(&Point::y, y);
	vec.push_back(&p2);
	return vec.front().get(&Point::x) + vec.back().get(&Point::y);
}

val<int32_t> vectorOfStructValuesClear(val<int32_t> x) {
	val<std::vector<Point>> vec;
	val<Point> p;
	p.set(&Point::x, val<int32_t>(999));
	p.set(&Point::y, val<int32_t>(999));
	vec.push_back(&p);
	vec.clear();
	p.set(&Point::x, x);
	p.set(&Point::y, x);
	vec.push_back(&p);
	auto elem = vec[val<size_t>(0)];
	return elem.get(&Point::x) + elem.get(&Point::y);
}

val<double> vectorOfPairValues(val<double> f, val<double> s) {
	val<std::vector<Pair>> vec;
	val<Pair> p(f, s);
	vec.push_back(&p);
	auto elem = vec[val<size_t>(0)];
	return elem.get(&Pair::first) + elem.get(&Pair::second);
}

val<int32_t> vectorOfStructValuesMultiple(val<int32_t> x) {
	val<std::vector<Point>> vec;
	val<Point> p1;
	p1.set(&Point::x, x);
	p1.set(&Point::y, x + val<int32_t>(1));
	vec.push_back(&p1);
	val<Point> p2;
	p2.set(&Point::x, x + val<int32_t>(2));
	p2.set(&Point::y, x + val<int32_t>(3));
	vec.push_back(&p2);
	auto e0 = vec[val<size_t>(0)];
	auto e1 = vec[val<size_t>(1)];
	return e0.get(&Point::x) + e0.get(&Point::y) + e1.get(&Point::x) + e1.get(&Point::y);
}

void runVectorTest(engine::NautilusEngine& engine, const std::string& /*backend*/ = "") {
	// Element access (fundamental)
	SECTION("vectorPushBackAndRead") {
		auto f = engine.registerFunction(vectorPushBackAndRead);
		REQUIRE(f(10) == 52);
	}
	SECTION("vectorAt") {
		auto f = engine.registerFunction(vectorAt);
		REQUIRE(f(77) == 77);
	}
	SECTION("vectorFrontBack") {
		auto f = engine.registerFunction(vectorFrontBack);
		REQUIRE(f(20) == 40);
	}
	SECTION("vectorDataPtr") {
		auto f = engine.registerFunction(vectorDataPtr);
		REQUIRE(f(5) == 115);
	}

	// Capacity
	SECTION("vectorEmpty") {
		auto f = engine.registerFunction(vectorEmpty);
		REQUIRE(f() == true);
	}
	SECTION("vectorNotEmptyAfterPush") {
		auto f = engine.registerFunction(vectorNotEmptyAfterPush);
		REQUIRE(f() == 1);
	}
	SECTION("vectorEmptyAfterClear") {
		auto f = engine.registerFunction(vectorEmptyAfterClear);
		REQUIRE(f() == true);
	}
	SECTION("vectorSize") {
		auto f = engine.registerFunction(vectorSize);
		REQUIRE(f(1) == 3);
	}
	SECTION("vectorReserveCapacity") {
		auto f = engine.registerFunction(vectorReserveCapacity);
		REQUIRE(f() == true);
	}

	// Modifiers
	SECTION("vectorClear") {
		auto f = engine.registerFunction(vectorClear);
		REQUIRE(f(7) == 99);
	}
	SECTION("vectorPopBack") {
		auto f = engine.registerFunction(vectorPopBack);
		REQUIRE(f() == 2);
	}
	SECTION("vectorResizeGrow") {
		auto f = engine.registerFunction(vectorResizeGrow);
		REQUIRE(f() == 5);
	}
	SECTION("vectorResizeGrowReadsDefault") {
		auto f = engine.registerFunction(vectorResizeGrowReadsDefault);
		REQUIRE(f() == 0);
	}
	SECTION("vectorResizeShrink") {
		auto f = engine.registerFunction(vectorResizeShrink);
		REQUIRE(f() == 2);
	}
	SECTION("vectorResizeWithValue") {
		auto f = engine.registerFunction(vectorResizeWithValue);
		REQUIRE(f(7) == 21);
	}

	// Ownership & interop
	SECTION("vectorReturnPtr") {
		auto f = engine.registerFunction(vectorReturnPtr);
		auto* vec = f(42);
		REQUIRE(vec->size() == 2);
		REQUIRE((*vec)[0] == 42);
		REQUIRE((*vec)[1] == 100);
		delete vec;
	}
	SECTION("vectorFromPtr") {
		auto f = engine.registerFunction(vectorFromPtr);
		auto vec = std::vector<int32_t> {5, 15};
		REQUIRE(f(&vec) == 20);
	}

	// Assignment
	SECTION("vectorAssign") {
		auto f = engine.registerFunction(vectorAssign);
		REQUIRE(f(10) == 30);
	}
	SECTION("vectorAssignElement") {
		auto f = engine.registerFunction(vectorAssignElement);
		REQUIRE(f(77) == 77);
	}
	SECTION("vectorAssignOverwrite") {
		auto f = engine.registerFunction(vectorAssignOverwrite);
		REQUIRE(f(42) == 42);
	}
	SECTION("vectorAssignDouble") {
		auto f = engine.registerFunction(vectorAssignDouble);
		REQUIRE(f(1.5) == Catch::Approx(4.0));
	}
	SECTION("vectorAssignFront") {
		auto f = engine.registerFunction(vectorAssignFront);
		REQUIRE(f(99) == 99);
	}
	SECTION("vectorAssignBack") {
		auto f = engine.registerFunction(vectorAssignBack);
		REQUIRE(f(99) == 99);
	}
	SECTION("vectorAssignViaData") {
		auto f = engine.registerFunction(vectorAssignViaData);
		REQUIRE(f(10) == 33);
	}
	SECTION("vectorAssignInt8") {
		auto f = engine.registerFunction(vectorAssignInt8);
		REQUIRE(f(static_cast<int8_t>(42)) == 42);
	}
	SECTION("vectorAssignInt64") {
		auto f = engine.registerFunction(vectorAssignInt64);
		REQUIRE(f(1234567890123LL) == 1234567890123LL);
	}
	SECTION("vectorAssignFloat") {
		auto f = engine.registerFunction(vectorAssignFloat);
		REQUIRE(f(2.5f) == Catch::Approx(2.5f));
	}
	SECTION("vectorShrinkToFitActual") {
		auto f = engine.registerFunction(vectorShrinkToFitActual);
		REQUIRE(f() == true);
	}
	SECTION("vectorEqualsFalse") {
		auto f = engine.registerFunction(vectorEqualsFalse);
		REQUIRE(f() == false);
	}
	SECTION("vectorEqualsDifferentSize") {
		auto f = engine.registerFunction(vectorEqualsDifferentSize);
		REQUIRE(f() == false);
	}
	SECTION("vectorSwapPtrs") {
		auto f = engine.registerFunction(vectorSwapPtrs);
		int32_t a = 10, b = 20;
		REQUIRE(f(&a, &b) == 30);
	}
	SECTION("vectorReserveZero") {
		auto f = engine.registerFunction(vectorReserveZero);
		REQUIRE(f() == 0);
	}
	SECTION("vectorResizeZero") {
		auto f = engine.registerFunction(vectorResizeZero);
		REQUIRE(f() == 0);
	}
	// Optimized capacity
	SECTION("vectorSizeAfterPushPop") {
		auto f = engine.registerFunction(vectorSizeAfterPushPop);
		REQUIRE(f() == 2);
	}
	SECTION("vectorCapacityAfterReserve") {
		auto f = engine.registerFunction(vectorCapacityAfterReserve);
		REQUIRE(f() == true);
	}

	// New methods
	SECTION("vectorMaxSize") {
		auto f = engine.registerFunction(vectorMaxSize);
		REQUIRE(f() == true);
	}
	SECTION("vectorShrinkToFit") {
		auto f = engine.registerFunction(vectorShrinkToFit);
		REQUIRE(f() == true);
	}
	SECTION("vectorSwap") {
		auto f = engine.registerFunction(vectorSwap);
		REQUIRE(f(5) == 105);
	}
	SECTION("vectorEquals") {
		auto f = engine.registerFunction(vectorEquals);
		REQUIRE(f() == true);
	}

	// Fundamental element types
	SECTION("vectorDouble") {
		auto f = engine.registerFunction(vectorDouble);
		REQUIRE(f(1.0) == Catch::Approx(4.14));
	}
	SECTION("vectorInt64") {
		auto f = engine.registerFunction(vectorInt64);
		REQUIRE(f(42LL) == 1000000000042LL);
	}
	SECTION("vectorFloat") {
		auto f = engine.registerFunction(vectorFloat);
		REQUIRE(f(1.5f) == Catch::Approx(4.0f));
	}
	SECTION("vectorInt8") {
		auto f = engine.registerFunction(vectorInt8);
		REQUIRE(f(static_cast<int8_t>(5)) == 15);
	}

	// Combined operations
	SECTION("vectorBuildAndSum") {
		auto f = engine.registerFunction(vectorBuildAndSum);
		REQUIRE(f(10) == 46);
	}
	SECTION("vectorPopToEmpty") {
		auto f = engine.registerFunction(vectorPopToEmpty);
		REQUIRE(f() == 0);
	}
	SECTION("vectorClearAndRefill") {
		auto f = engine.registerFunction(vectorClearAndRefill);
		REQUIRE(f(55) == 55);
	}

	// Pointer element types
	SECTION("vectorOfPtrsReadThrough") {
		auto f = engine.registerFunction(vectorOfPtrsReadThrough);
		int32_t a = 10, b = 20;
		REQUIRE(f(&a, &b) == 30);
	}
	SECTION("vectorOfPtrsSize") {
		auto f = engine.registerFunction(vectorOfPtrsSize);
		int32_t a = 1;
		REQUIRE(f(&a) == 3);
	}
	SECTION("vectorOfPtrsFrontBack") {
		auto f = engine.registerFunction(vectorOfPtrsFrontBack);
		int32_t a = 10, b = 20, c = 30;
		REQUIRE(f(&a, &b, &c) == 40);
	}
	SECTION("vectorOfPtrsPopBack") {
		auto f = engine.registerFunction(vectorOfPtrsPopBack);
		int32_t a = 100, b = 200;
		REQUIRE(f(&a, &b) == 100);
	}
	SECTION("vectorOfDoublePtrs") {
		auto f = engine.registerFunction(vectorOfDoublePtrs);
		double a = 1.5, b = 2.5;
		REQUIRE(f(&a, &b) == Catch::Approx(4.0));
	}

	// Struct pointer element types
	SECTION("vectorOfStructPtrs") {
		auto f = engine.registerFunction(vectorOfStructPtrs);
		Point a = {10, 20}, b = {30, 40};
		REQUIRE(f(&a, &b) == 50);
	}
	SECTION("vectorOfStructPtrsSize") {
		auto f = engine.registerFunction(vectorOfStructPtrsSize);
		Point a = {1, 2};
		REQUIRE(f(&a) == 2);
	}
	SECTION("vectorOfStructPtrsFrontBack") {
		auto f = engine.registerFunction(vectorOfStructPtrsFrontBack);
		Point a = {10, 20}, b = {30, 40}, c = {50, 60};
		REQUIRE(f(&a, &b, &c) == 70);
	}
	SECTION("vectorOfStructPtrsAt") {
		auto f = engine.registerFunction(vectorOfStructPtrsAt);
		Point a = {1, 2}, b = {30, 40};
		REQUIRE(f(&a, &b) == 70);
	}
	SECTION("vectorOfStructPtrsPopBack") {
		auto f = engine.registerFunction(vectorOfStructPtrsPopBack);
		Point a = {10, 20}, b = {30, 40};
		REQUIRE(f(&a, &b) == 30);
	}
	SECTION("vectorOfPairPtrs") {
		auto f = engine.registerFunction(vectorOfPairPtrs);
		Pair a = {1.5, 2.5}, b = {3.5, 4.5};
		REQUIRE(f(&a, &b) == Catch::Approx(6.0));
	}

	// Struct value element types
	SECTION("vectorOfStructValues") {
		auto f = engine.registerFunction(vectorOfStructValues);
		REQUIRE(f(10, 20) == 30);
	}
	SECTION("vectorOfStructValuesAt") {
		auto f = engine.registerFunction(vectorOfStructValuesAt);
		REQUIRE(f(10, 20) == 30);
	}
	SECTION("vectorOfStructValuesSize") {
		auto f = engine.registerFunction(vectorOfStructValuesSize);
		REQUIRE(f(5) == 2);
	}
	SECTION("vectorOfStructValuesFrontBack") {
		auto f = engine.registerFunction(vectorOfStructValuesFrontBack);
		REQUIRE(f(10, 20) == 30);
	}
	SECTION("vectorOfStructValuesClear") {
		auto f = engine.registerFunction(vectorOfStructValuesClear);
		REQUIRE(f(7) == 14);
	}
	SECTION("vectorOfPairValues") {
		auto f = engine.registerFunction(vectorOfPairValues);
		REQUIRE(f(1.5, 2.5) == Catch::Approx(4.0));
	}
	SECTION("vectorOfStructValuesMultiple") {
		auto f = engine.registerFunction(vectorOfStructValuesMultiple);
		// x=10: 10+11+12+13 = 46
		REQUIRE(f(10) == 46);
	}
}

val<int32_t> vectorAtOutOfRange(val<size_t> idx) {
	val<std::vector<int32_t>> vec;
	vec.push_back(val<int32_t>(1));
	vec.push_back(val<int32_t>(2));
	return vec.at(idx);
}

TEST_CASE("VectorTest - Interpreter") {
	auto engine = nautilus::testing::makeEngine("interpreter");
	runVectorTest(engine);

	SECTION("vectorAtOutOfRange") {
		auto f = engine.registerFunction(vectorAtOutOfRange);
		REQUIRE(f(0) == 1);
		REQUIRE(f(1) == 2);
		REQUIRE_THROWS_AS(f(5), std::out_of_range);
	}
}

#ifdef ENABLE_TRACING
TEST_CASE("VectorTest - Compiler") {
	const std::vector<std::string> traceModes = {"exceptionBasedTracing", "lazyTracing"};
	for (const auto& backend : nautilus::testing::availableBackends()) {
		for (const auto& traceMode : traceModes) {
			DYNAMIC_SECTION(backend + "_" + traceMode) {
				auto engine = nautilus::testing::makeEngine(
				    backend, [&](engine::Options& options) { options.setOption("engine.traceMode", traceMode); });
				runVectorTest(engine, backend);
			}
		}
	}
}
#endif
} // namespace nautilus::engine

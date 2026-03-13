
#include "nautilus/val.hpp"
#include "nautilus/val_func.hpp"
#include <catch2/catch_all.hpp>

namespace nautilus {

static int32_t addHelper(int32_t x, int32_t y) {
	return x + y;
}

static int32_t mulHelper(int32_t x, int32_t y) {
	return x * y;
}

static int32_t negHelper(int32_t x) {
	return -x;
}

using BinFn = int32_t (*)(int32_t, int32_t);
using UnaryFn = int32_t (*)(int32_t);

TEST_CASE("FunctionPtr Val Test") {
	SECTION("default-construct is null") {
		val<BinFn> fn;
		REQUIRE((fn == nullptr) == true);
	}

	SECTION("nullptr-construct is null") {
		val<BinFn> fn(nullptr);
		REQUIRE((fn == nullptr) == true);
	}

	SECTION("construct from raw ptr is not null") {
		val<BinFn> fn(addHelper);
		REQUIRE((fn != nullptr) == true);
	}

	SECTION("operator bool - null is false") {
		val<BinFn> fn(nullptr);
		REQUIRE(!static_cast<bool>(fn));
	}

	SECTION("operator bool - non-null is true") {
		val<BinFn> fn(addHelper);
		REQUIRE(static_cast<bool>(fn));
	}

	SECTION("copy constructor preserves pointer") {
		val<BinFn> original(addHelper);
		val<BinFn> copy(original);
		REQUIRE((copy == original) == true);
	}

	SECTION("copy assignment preserves pointer") {
		val<BinFn> a(addHelper);
		val<BinFn> b(mulHelper);
		b = a;
		REQUIRE((b == a) == true);
	}

	SECTION("equality - same pointer") {
		val<BinFn> a(addHelper);
		val<BinFn> b(addHelper);
		REQUIRE((a == b) == true);
	}

	SECTION("equality - different pointers") {
		val<BinFn> a(addHelper);
		val<BinFn> b(mulHelper);
		REQUIRE((a == b) == false);
	}

	SECTION("inequality - same pointer") {
		val<BinFn> a(addHelper);
		val<BinFn> b(addHelper);
		REQUIRE((a != b) == false);
	}

	SECTION("inequality - different pointers") {
		val<BinFn> a(addHelper);
		val<BinFn> b(mulHelper);
		REQUIRE((a != b) == true);
	}

	SECTION("equality with nullptr - null") {
		val<BinFn> fn(nullptr);
		REQUIRE((fn == nullptr) == true);
	}

	SECTION("equality with nullptr - non-null") {
		val<BinFn> fn(addHelper);
		REQUIRE((fn == nullptr) == false);
	}

	SECTION("inequality with nullptr - null") {
		val<BinFn> fn(nullptr);
		REQUIRE((fn != nullptr) == false);
	}

	SECTION("inequality with nullptr - non-null") {
		val<BinFn> fn(addHelper);
		REQUIRE((fn != nullptr) == true);
	}

	SECTION("conversion to val<void*>") {
		val<BinFn> fn(addHelper);
		val<void*> vp = fn;
		REQUIRE((vp != nullptr) == true);
	}

	SECTION("null conversion to val<void*>") {
		val<BinFn> fn(nullptr);
		val<void*> vp = fn;
		REQUIRE((vp == nullptr) == true);
	}

	SECTION("indirect call - addition") {
		val<BinFn> fn(addHelper);
		val<int32_t> result = fn(val<int32_t>(3), val<int32_t>(4));
		REQUIRE(result == 7);
	}

	SECTION("indirect call - multiplication") {
		val<BinFn> fn(mulHelper);
		val<int32_t> result = fn(val<int32_t>(3), val<int32_t>(4));
		REQUIRE(result == 12);
	}

	SECTION("indirect call through copy") {
		val<BinFn> original(addHelper);
		val<BinFn> copy(original);
		val<int32_t> result = copy(val<int32_t>(10), val<int32_t>(5));
		REQUIRE(result == 15);
	}

	SECTION("make_fn_val from raw ptr") {
		auto fn = make_fn_val(addHelper);
		val<int32_t> result = fn(val<int32_t>(6), val<int32_t>(7));
		REQUIRE(result == 13);
	}

	SECTION("make_fn_val from non-capturing lambda") {
		auto fn = make_fn_val(+[](int32_t x, int32_t y) { return x - y; });
		val<int32_t> result = fn(val<int32_t>(10), val<int32_t>(3));
		REQUIRE(result == 7);
	}

	SECTION("different signatures - unary function") {
		val<UnaryFn> fn(negHelper);
		REQUIRE((fn != nullptr) == true);
		val<int32_t> result = fn(val<int32_t>(5));
		REQUIRE(result == -5);
	}

	SECTION("assign nullptr clears pointer") {
		val<BinFn> fn(addHelper);
		fn = val<BinFn>(nullptr);
		REQUIRE((fn == nullptr) == true);
	}

	SECTION("reassign to different function") {
		val<BinFn> fn(addHelper);
		fn = val<BinFn>(mulHelper);
		val<int32_t> result = fn(val<int32_t>(3), val<int32_t>(4));
		REQUIRE(result == 12);
	}
}

} // namespace nautilus

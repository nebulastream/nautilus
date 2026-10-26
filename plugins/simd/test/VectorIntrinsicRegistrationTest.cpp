#include "nautilus/extensions/ExtensionRegistry.hpp"
#include "nautilus/extensions/IntrinsicRegistration.hpp"
#include <catch2/catch_test_macros.hpp>
#include <string>

using namespace nautilus;

TEST_CASE("the vector intrinsics are registered without a backend", "[simd][intrinsic]") {
	auto& registry = ext::ExtensionRegistry::instance();
	size_t found = 0;
	for (auto type : {"f32x4", "f32x8", "f32x16", "f64x2", "f64x4", "f64x8", "i32x4", "i32x8", "i32x16", "i64x2",
	                  "i64x4", "i64x8"}) {
		for (auto op : {"load", "store", "add", "div", "reduce_max", "blend", "gather", "compress_store", "shuffle"}) {
			auto name = std::string("simd.") + op + "_" + type;
			INFO(name);
			auto id = registry.findIntrinsic(name);
			REQUIRE(id.has_value());
			auto* info = registry.intrinsic(*id);
			CHECK((info->attrs & ext::AttrNoUnwind) != 0);
			// vectors live in thread-local storage behind a pointer: never claimed pure
			CHECK((info->attrs & ext::AttrNoOtherRead) == 0);
			CHECK(registry.findIntrinsicByFallback(info->fallback) == id);
			++found;
		}
	}
	CHECK(found == 108);
	CHECK(registry.findIntrinsic("simd.fma_f64x4").has_value());
	CHECK(registry.findIntrinsic("simd.lshr_i64x2").has_value());
	CHECK_FALSE(registry.findIntrinsic("simd.fma_i32x4").has_value());
}

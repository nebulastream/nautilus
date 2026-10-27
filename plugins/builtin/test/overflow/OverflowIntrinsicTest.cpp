#include "nautilus/extensions/ExtensionRegistry.hpp"
#include "nautilus/extensions/IntrinsicRegistration.hpp"
#include <catch2/catch_test_macros.hpp>
#include <string>

using namespace nautilus;

TEST_CASE("the overflow and int128 intrinsics are registered without a backend", "[builtin][intrinsic]") {
	auto& registry = ext::ExtensionRegistry::instance();
	for (auto op : {"add", "sub", "mul"}) {
		for (auto width : {"i32", "i64", "i128"}) {
			auto name = std::string("builtin.") + op + "_overflow_" + width;
			INFO(name);
			auto id = registry.findIntrinsic(name);
			REQUIRE(id.has_value());
			auto* info = registry.intrinsic(*id);
			// bool result, three parameters
			REQUIRE(info->signature.size() == 4);
			CHECK(nir::typeOf(info->signature[0]) == nir::TypeId::Bool);
			CHECK((info->attrs & ext::AttrNoUnwind) != 0);
			CHECK(registry.findIntrinsicByFallback(info->fallback) == id);
		}
	}
	for (auto name : {"make", "add", "sub", "mul", "div", "rem", "and", "or", "xor", "neg", "not", "shl", "shr", "eq",
	                  "lt", "low", "high", "load", "store"}) {
		auto full = std::string("builtin.int128_") + name;
		INFO(full);
		auto id = registry.findIntrinsic(full);
		REQUIRE(id.has_value());
		auto* info = registry.intrinsic(*id);
		CHECK((info->attrs & ext::AttrNoUnwind) != 0);
		// pointers to thread-local storage: never claimed pure
		CHECK((info->attrs & ext::AttrNoOtherRead) == 0);
	}
}

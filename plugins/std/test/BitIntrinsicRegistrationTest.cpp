#include "nautilus/extensions/ExtensionRegistry.hpp"
#include "nautilus/extensions/IntrinsicRegistration.hpp"
#include <catch2/catch_test_macros.hpp>
#include <nautilus/std/bit.h>

using namespace nautilus;

TEST_CASE("the bit intrinsics are registered without a backend", "[std][intrinsic]") {
	auto& registry = ext::ExtensionRegistry::instance();
	size_t found = 0;
	for (auto op : {"countl_zero", "countr_zero", "popcount", "rotl", "rotr"}) {
		for (auto width : {"u8", "u16", "u32", "u64"}) {
			auto name = std::string("std.") + op + "_" + width;
			INFO(name);
			auto id = registry.findIntrinsic(name);
			REQUIRE(id.has_value());
			auto* info = registry.intrinsic(*id);
			// result and parameters have the width of the operation, narrow ones zero-extended
			auto expectedWidth = std::string(width) == "u8"    ? nir::TypeId::I8
			                     : std::string(width) == "u16" ? nir::TypeId::I16
			                     : std::string(width) == "u32" ? nir::TypeId::I32
			                                                   : nir::TypeId::I64;
			for (auto word : info->signature) {
				CHECK(nir::typeOf(word) == expectedWidth);
				CHECK(nir::isValidTypeWord(word));
			}
			CHECK((info->attrs & ext::AttrNoUnwind) != 0);
			CHECK(registry.findIntrinsicByFallback(info->fallback) == id);
			++found;
		}
	}
	CHECK(found == 20);
	for (auto name : {"std.byteswap_u16", "std.byteswap_u32", "std.byteswap_u64"}) {
		CHECK(registry.findIntrinsic(name).has_value());
	}
}

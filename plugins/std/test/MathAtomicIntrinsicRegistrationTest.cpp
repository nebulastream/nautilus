#include "nautilus/extensions/ExtensionRegistry.hpp"
#include "nautilus/extensions/IntrinsicRegistration.hpp"
#include <catch2/catch_test_macros.hpp>
#include <cmath>
#include <string>

using namespace nautilus;

TEST_CASE("the math intrinsics are registered without a backend", "[std][intrinsic]") {
	auto& registry = ext::ExtensionRegistry::instance();
	for (auto name : {"sin", "cos", "sqrt", "floor", "fabs", "exp2", "log10", "nearbyint"}) {
		for (auto suffix : {"_f32", "_f64"}) {
			auto full = std::string("std.") + name + suffix;
			INFO(full);
			auto id = registry.findIntrinsic(full);
			REQUIRE(id.has_value());
			auto* info = registry.intrinsic(*id);
			REQUIRE(info->signature.size() == 2);
			CHECK(nir::typeOf(info->signature[0]) ==
			      (std::string(suffix) == "_f32" ? nir::TypeId::F32 : nir::TypeId::F64));
			CHECK((info->attrs & ext::AttrNoArgWrite) != 0);
			CHECK((info->attrs & ext::AttrNoUnwind) != 0);
		}
	}
	for (auto name : {"std.pow_f64", "std.atan2_f32", "std.copysign_f64"}) {
		auto id = registry.findIntrinsic(name);
		REQUIRE(id.has_value());
		CHECK(registry.intrinsic(*id)->signature.size() == 3);
	}
	auto fma = registry.findIntrinsic("std.fma_f64");
	REQUIRE(fma.has_value());
	CHECK(registry.intrinsic(*fma)->signature.size() == 4);
	auto lround = registry.findIntrinsic("std.lround_f64");
	REQUIRE(lround.has_value());
	CHECK(nir::typeOf(registry.intrinsic(*lround)->signature[0]) == nir::TypeId::I64);
	CHECK(registry.findIntrinsic("std.abs_i32").has_value());
	CHECK(registry.findIntrinsic("std.abs_i64").has_value());
}

TEST_CASE("the atomic intrinsics are registered and conservative", "[std][intrinsic]") {
	auto& registry = ext::ExtensionRegistry::instance();
	size_t found = 0;
	for (auto type : {"int32_t", "int64_t", "uint32_t", "uint64_t"}) {
		for (auto op : {"load_%_seqcst", "load_%_acquire", "store_%_release", "exchange_%_relaxed",
		                "fetch_add_%_seqcst", "fetch_xor_%_relaxed", "cmpxchg_strong_%_seqcst"}) {
			std::string name = std::string("std.atomic_") + op;
			name.replace(name.find('%'), 1, type);
			INFO(name);
			auto id = registry.findIntrinsic(name);
			REQUIRE(id.has_value());
			auto* info = registry.intrinsic(*id);
			CHECK((info->attrs & ext::AttrNoUnwind) != 0);
			// an atomic touches memory: it must not claim to be pure
			CHECK((info->attrs & ext::AttrNoArgWrite) == 0);
			CHECK((info->attrs & ext::AttrNoArgRead) == 0);
			CHECK(nir::typeOf(info->signature[1]) == nir::TypeId::Ptr);
			++found;
		}
	}
	CHECK(found == 28);
}

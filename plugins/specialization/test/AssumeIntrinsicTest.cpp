#include "nautilus/extensions/ExtensionRegistry.hpp"
#include "nautilus/extensions/IntrinsicRegistration.hpp"
#include <catch2/catch_test_macros.hpp>
#include <nautilus/specialization/assume.hpp>

using namespace nautilus;

// the registrar sits in assume.cpp; referencing its public function is what links the file, as in any real user
void (*volatile assumeAnchor)(val<bool>) = &nautilus_assume;

TEST_CASE("the assume intrinsics are registered without a backend", "[specialization][intrinsic]") {
	auto& registry = ext::ExtensionRegistry::instance();
	struct Expected {
		const char* name;
		size_t params;
	};
	for (auto e : {Expected {"specialization.assume", 1}, Expected {"specialization.assume_aligned", 2},
	               Expected {"specialization.assume_constant", 2}, Expected {"specialization.assume_range", 3},
	               Expected {"specialization.assume_nonzero", 1}, Expected {"specialization.assume_separate_storage", 2}}) {
		INFO(e.name);
		auto id = registry.findIntrinsic(e.name);
		REQUIRE(id.has_value());
		auto* info = registry.intrinsic(*id);
		CHECK(info->signature.size() == e.params + 1);
		CHECK(info->signature[0] == nir::makeTypeWord(nir::TypeId::Void));
		CHECK((info->attrs & ext::AttrNoUnwind) != 0);
		CHECK(registry.findIntrinsicByFallback(info->fallback) == id);
	}
}

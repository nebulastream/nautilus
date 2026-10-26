#include "nautilus/extensions/ExtensionRegistry.hpp"
#include "nautilus/extensions/IntrinsicRegistration.hpp"
#include <catch2/catch_test_macros.hpp>
#include <nautilus/builtin/prefetch/prefetch.hpp>

using namespace nautilus;

// the registry is frozen by the first reader (an engine); registration has to be complete before that, in every build
TEST_CASE("the prefetch intrinsics are registered without a backend", "[builtin][intrinsic]") {
	auto& registry = ext::ExtensionRegistry::instance();
	for (auto rw : {"read", "write"}) {
		for (auto locality : {"none", "low", "moderate", "high"}) {
			auto name = std::string("builtin.prefetch_") + rw + "_" + locality;
			INFO(name);
			auto id = registry.findIntrinsic(name);
			REQUIRE(id.has_value());
			auto* info = registry.intrinsic(*id);
			CHECK(info->signature == std::vector<uint32_t> {nir::makeTypeWord(nir::TypeId::Void),
			                                                nir::makeTypeWord(nir::TypeId::Ptr)});
			CHECK((info->attrs & ext::AttrNoUnwind) != 0);
			CHECK(info->captureThunk == nullptr);
			REQUIRE(info->fallback != nullptr);
			// the fallback is callable and is what a traced invoke of it resolves to
			reinterpret_cast<void (*)(const void*)>(info->fallback)(&name);
			CHECK(registry.findIntrinsicByFallback(info->fallback) == id);
		}
	}
}

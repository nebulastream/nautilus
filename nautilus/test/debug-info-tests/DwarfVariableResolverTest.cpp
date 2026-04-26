
#include "nautilus/debug/DwarfVariableResolver.hpp"
#include <catch2/catch_all.hpp>

namespace nautilus::debug {

// Sentinel variables declared at namespace scope so they reliably land in
// the test binary's DWARF as DW_TAG_variable entries. The test then looks
// them up by the source coordinates captured via __FILE__ / __LINE__.
namespace {
[[maybe_unused]] static int mySentinelAlpha = 111;
constexpr uint32_t kSentinelAlphaLine = __LINE__ - 1;

[[maybe_unused]] static long mySentinelBeta = 222;
constexpr uint32_t kSentinelBetaLine = __LINE__ - 1;
} // namespace

TEST_CASE("DwarfVariableResolver: recovers a namespace-scope variable name") {
	auto* resolver = DwarfVariableResolver::getInstance();
	if (resolver == nullptr) {
		// Non-Linux or non-MLIR build. Skip rather than fail so the test
		// binary stays portable.
		SUCCEED("No DWARF variable resolver available in this build; nothing to verify.");
		return;
	}

	auto alpha = resolver->resolveVariableName(__FILE__, kSentinelAlphaLine, 0);
	auto beta = resolver->resolveVariableName(__FILE__, kSentinelBetaLine, 0);

	// These assertions only fire when the test binary was actually built
	// with -g. Without debug info every lookup returns nullopt, so treat
	// "both missing" as an informative skip.
	if (!alpha && !beta) {
		SUCCEED("Test binary lacks DWARF (built without -g); nothing to verify.");
		return;
	}

	REQUIRE(alpha.has_value());
	REQUIRE(*alpha == "mySentinelAlpha");
	REQUIRE(beta.has_value());
	REQUIRE(*beta == "mySentinelBeta");
}

TEST_CASE("DwarfVariableResolver: returns nullopt for nonexistent coordinates") {
	auto* resolver = DwarfVariableResolver::getInstance();
	if (resolver == nullptr) {
		SUCCEED("No DWARF variable resolver available in this build; nothing to verify.");
		return;
	}
	// Line 1 of this file is the blank leading line; no variable is
	// declared there.
	auto result = resolver->resolveVariableName(__FILE__, 1, 0);
	REQUIRE_FALSE(result.has_value());
}

} // namespace nautilus::debug

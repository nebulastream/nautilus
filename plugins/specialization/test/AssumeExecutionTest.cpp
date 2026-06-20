#include "ExecutionTest.hpp"
#include "nautilus/Engine.hpp"
#include "nautilus/specialization/plugin.hpp"
#include <catch2/catch_all.hpp>

TEST_CASE("Specialization plugin: linkable", "[specialization][smoke]") {
	SUCCEED("nautilus-specialization plugin is built and linkable");
}

#if defined(ENABLE_MLIR_BACKEND) && !defined(__APPLE__)
#include "nautilus/specialization/assume.hpp"

namespace nautilus::engine {

static void throwExceptions() {
	throw std::runtime_error("error");
}

val<int32_t> cfWithAssume(val<int32_t> a) {
	nautilus_assume(a > 10);
	if (a > 10) {
		return a + 10;
	} else {
		invoke<void>(throwExceptions);
		return 42;
	}
}

val<int> cfWithAssumeAlignment(val<int32_t*> a) {
	nautilus_assume_aligned(a, 256);
	return *a + 10;
}

val<int32_t> cfWithAssumeNoalias(val<int32_t*> a, val<int32_t*> b) {
	nautilus_assume_noalias(a, b);
	*a = 1;
	*b = 2;
	// With the noalias assumption LLVM may fold this load to the constant 1.
	return *a;
}

TEST_CASE("MLIR Intrinsic Function Test") {
	for (auto useIntrinsics : {false, true}) {
		DYNAMIC_SECTION("useIntrinsics:" << useIntrinsics) {
			auto engine = nautilus::testing::makeEngine("mlir", [&](engine::Options& options) {
				options.setOption("dump.after_mlir_generation", true);
				options.setOption("dump.console", true);
				options.setOption("mlir.enableIntrinsics", useIntrinsics);
				options.setOption("mlir.enableMultithreading", false);
			});

			SECTION("cfWithAssume") {
				auto f = engine.registerFunction(cfWithAssume);
				if (useIntrinsics) {
					REQUIRE(f(42) == 52);
					// the compiled version with intrinsics should optimize away the else branch
					REQUIRE(f(0) == 10);
				} else {
					REQUIRE(f(42) == 52);
					REQUIRE_THROWS(f(0));
				}
			}

			SECTION("cfWithAssumeNoalias") {
				auto f = engine.registerFunction(cfWithAssumeNoalias);
				// Distinct, non-aliasing pointers: result is always the value stored through `a`.
				int32_t x = 0;
				int32_t y = 0;
				REQUIRE(f(&x, &y) == 1);
				REQUIRE(x == 1);
				REQUIRE(y == 2);
			}
		}
	}
}

} // namespace nautilus::engine
#endif

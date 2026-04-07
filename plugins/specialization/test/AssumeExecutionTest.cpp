#include "ExecutionTest.hpp"
#include "nautilus/Engine.hpp"
#include <catch2/catch_all.hpp>

TEST_CASE("Specialization plugin: linkable", "[specialization][smoke]") {
	SUCCEED("nautilus-specialization plugin is built and linkable");
}

#if defined(ENABLE_MLIR_BACKEND) && !defined(__APPLE__)
#include "MLIRAssumeIntrinsics.hpp"
#include "nautilus/specialization/assume.hpp"

namespace {
struct SpecializationIntrinsicRegistrar {
	SpecializationIntrinsicRegistrar() {
		nautilus::compiler::mlir::RegisterMLIRAssumeIntrinsicPlugin();
	}
};
static SpecializationIntrinsicRegistrar registrar_;
} // namespace

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

val<int64_t> cfWithAssumeConstant(val<int64_t> a) {
	nautilus_assume_constant(a, 7);
	if (a == 7) {
		return a * 3;
	} else {
		invoke<void>(throwExceptions);
		return 42;
	}
}

val<int64_t> cfWithAssumeRange(val<int64_t> a) {
	nautilus_assume_range(a, 0, 100);
	if (a >= 0 && a <= 100) {
		return a + 1;
	} else {
		invoke<void>(throwExceptions);
		return 42;
	}
}

val<int64_t> cfWithAssumeNonzero(val<int64_t> a) {
	nautilus_assume_nonzero(a);
	if (a != 0) {
		return a + 5;
	} else {
		invoke<void>(throwExceptions);
		return 42;
	}
}

val<int> cfWithAssumeAlignment(val<int32_t*> a) {
	nautilus_assume_aligned(a, 256);
	return *a + 10;
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
			SECTION("cfWithAssumeConstant") {
				auto f = engine.registerFunction(cfWithAssumeConstant);
				if (useIntrinsics) {
					REQUIRE(f(7) == 21);
					// with intrinsics, the optimizer folds `a` to the asserted constant 7,
					// so even passing 0 returns 7 * 3 = 21 (undefined behavior is exploited)
					REQUIRE(f(0) == 21);
				} else {
					REQUIRE(f(7) == 21);
					REQUIRE_THROWS(f(0));
				}
			}
			SECTION("cfWithAssumeRange") {
				auto f = engine.registerFunction(cfWithAssumeRange);
				if (useIntrinsics) {
					REQUIRE(f(50) == 51);
					REQUIRE(f(-1) == 0);
				} else {
					REQUIRE(f(50) == 51);
					REQUIRE_THROWS(f(-1));
				}
			}
			SECTION("cfWithAssumeNonzero") {
				auto f = engine.registerFunction(cfWithAssumeNonzero);
				if (useIntrinsics) {
					REQUIRE(f(3) == 8);
					REQUIRE(f(0) == 5);
				} else {
					REQUIRE(f(3) == 8);
					REQUIRE_THROWS(f(0));
				}
			}
		}
	}
}

} // namespace nautilus::engine
#endif

#include "ExecutionTest.hpp"
#include "nautilus/Engine.hpp"
#include <catch2/catch_all.hpp>
#include <cstdint>
#include <functional>

#ifdef ENABLE_TRACING
#include "nautilus/specialization/specialize.hpp"
#include "nautilus/specialization/specialized_function.hpp"

namespace nautilus::engine {

// Backend-parameterized behavioural tests for SpecializedNautilusFunction.
//
// These tests mirror the patterns used by the generic execution tests in
// `nautilus/test/common/ExecutionTest.hpp`: every test case iterates over all
// enabled code-gen backends via `forEachBackend`, plus the interpreter when
// applicable. IR-shape inspection (cond_br placement, dispatcher symbol names)
// stays in `SpecializationTest.cpp` since it is inherently MLIR-only.
//
// Backend variations / notes:
// - The interpreter (`engine.Compilation=false`) executes the dispatcher
//   lambda directly without compiling. The traced if/else still evaluates as
//   regular C++ control flow, so both the specialized and generic branches
//   produce correct results. Numeric outputs match the compiled backends.
// - All compiled backends (mlir / cpp / bc / asmjit) go through the same
//   tracing dispatcher path and must produce identical numeric results.

namespace {

val<int64_t> specBackendPow(val<int64_t> base, val<int64_t> exp) {
	val<int64_t> r = 1;
	for (val<int64_t> i = 0; i < exp; i = i + 1) {
		r = r * base;
	}
	return r;
}

val<int32_t> specBackendMulAdd(val<int32_t> x, val<int32_t> k) {
	return x * k + k;
}

} // namespace

TEST_CASE("SpecializedNautilusFunction backends: empty profile / generic path") {
	nautilus::testing::forEachBackend(
	    [](engine::NautilusEngine& engine) {
		    SpecializedNautilusFunction<val<int64_t>(val<int64_t>, val<int64_t>), 1> spec {"spec_be_empty",
		                                                                                   specBackendPow};
		    std::function<val<int64_t>(val<int64_t>, val<int64_t>)> wrapper = [&](val<int64_t> b, val<int64_t> e) {
			    return spec(b, e);
		    };
		    auto fn = engine.registerFunction(wrapper);
		    REQUIRE(fn(2, 5) == 32);
		    REQUIRE(fn(3, 4) == 81);
		    REQUIRE(fn(7, 0) == 1);
		    REQUIRE(fn(-2, 3) == -8);
	    },
	    /*include_interpreter=*/true);
}

TEST_CASE("SpecializedNautilusFunction backends: stable profile dispatches both branches") {
	nautilus::testing::forEachBackend(
	    [](engine::NautilusEngine& engine) {
		    SpecializedNautilusFunction<val<int64_t>(val<int64_t>, val<int64_t>), 1> spec {"spec_be_stable",
		                                                                                   specBackendPow};
		    for (int i = 0; i < 2048; ++i) {
			    ValueProfile<int64_t>::profile(&spec.profile<0>(), 5);
		    }
		    REQUIRE(spec.profile<0>().isStable());
		    std::function<val<int64_t>(val<int64_t>, val<int64_t>)> wrapper = [&](val<int64_t> b, val<int64_t> e) {
			    return spec(b, e);
		    };
		    auto fn = engine.registerFunction(wrapper);
		    // Specialized branch (exp == 5).
		    REQUIRE(fn(2, 5) == 32);
		    REQUIRE(fn(3, 5) == 243);
		    REQUIRE(fn(-2, 5) == -32);
		    REQUIRE(fn(1, 5) == 1);
		    // Generic branch (exp != 5).
		    REQUIRE(fn(2, 10) == 1024);
		    REQUIRE(fn(3, 4) == 81);
		    REQUIRE(fn(5, 0) == 1);
		    REQUIRE(fn(7, 1) == 7);
	    },
	    /*include_interpreter=*/true);
}

TEST_CASE("SpecializedNautilusFunction backends: multi-index specialization") {
	nautilus::testing::forEachBackend(
	    [](engine::NautilusEngine& engine) {
		    SpecializedNautilusFunction<val<int64_t>(val<int64_t>, val<int64_t>), 0, 1> spec {"spec_be_multi",
		                                                                                      specBackendPow};
		    for (int i = 0; i < 2048; ++i) {
			    ValueProfile<int64_t>::profile(&spec.profile<0>(), 2);
			    ValueProfile<int64_t>::profile(&spec.profile<1>(), 5);
		    }
		    REQUIRE(spec.profile<0>().isStable());
		    REQUIRE(spec.profile<1>().isStable());
		    std::function<val<int64_t>(val<int64_t>, val<int64_t>)> wrapper = [&](val<int64_t> b, val<int64_t> e) {
			    return spec(b, e);
		    };
		    auto fn = engine.registerFunction(wrapper);
		    // Both match -> specialized.
		    REQUIRE(fn(2, 5) == 32);
		    // Partial / no matches -> generic.
		    REQUIRE(fn(2, 4) == 16);
		    REQUIRE(fn(3, 5) == 243);
		    REQUIRE(fn(4, 3) == 64);
		    REQUIRE(fn(7, 0) == 1);
	    },
	    /*include_interpreter=*/true);
}

TEST_CASE("SpecializedNautilusFunction backends: edge dominant value zero") {
	nautilus::testing::forEachBackend(
	    [](engine::NautilusEngine& engine) {
		    SpecializedNautilusFunction<val<int64_t>(val<int64_t>, val<int64_t>), 1> spec {"spec_be_zero",
		                                                                                   specBackendPow};
		    for (int i = 0; i < 2048; ++i) {
			    ValueProfile<int64_t>::profile(&spec.profile<0>(), 0);
		    }
		    REQUIRE(spec.profile<0>().isStable());
		    REQUIRE(spec.profile<0>().dominant() == 0);
		    std::function<val<int64_t>(val<int64_t>, val<int64_t>)> wrapper = [&](val<int64_t> b, val<int64_t> e) {
			    return spec(b, e);
		    };
		    auto fn = engine.registerFunction(wrapper);
		    // Specialized branch (exp == 0): pow returns 1 for any base.
		    REQUIRE(fn(2, 0) == 1);
		    REQUIRE(fn(99, 0) == 1);
		    // Generic branch.
		    REQUIRE(fn(2, 5) == 32);
	    },
	    /*include_interpreter=*/true);
}

TEST_CASE("SpecializedNautilusFunction backends: edge dominant value negative (int32)") {
	nautilus::testing::forEachBackend(
	    [](engine::NautilusEngine& engine) {
		    SpecializedNautilusFunction<val<int32_t>(val<int32_t>, val<int32_t>), 1> spec {"spec_be_neg",
		                                                                                   specBackendMulAdd};
		    for (int i = 0; i < 2048; ++i) {
			    ValueProfile<int32_t>::profile(&spec.profile<0>(), -3);
		    }
		    REQUIRE(spec.profile<0>().isStable());
		    std::function<val<int32_t>(val<int32_t>, val<int32_t>)> wrapper = [&](val<int32_t> x, val<int32_t> k) {
			    return spec(x, k);
		    };
		    auto fn = engine.registerFunction(wrapper);
		    // Specialized branch (k == -3): x * -3 + -3.
		    REQUIRE(fn(10, -3) == -33);
		    REQUIRE(fn(0, -3) == -3);
		    // Generic branch.
		    REQUIRE(fn(4, 5) == 25);
	    },
	    /*include_interpreter=*/true);
}

TEST_CASE("SpecializedNautilusFunction backends: multiple call sites in same parent") {
	nautilus::testing::forEachBackend(
	    [](engine::NautilusEngine& engine) {
		    SpecializedNautilusFunction<val<int64_t>(val<int64_t>, val<int64_t>), 1> spec {"spec_be_multi_call",
		                                                                                   specBackendPow};
		    for (int i = 0; i < 2048; ++i) {
			    ValueProfile<int64_t>::profile(&spec.profile<0>(), 3);
		    }
		    REQUIRE(spec.profile<0>().isStable());
		    std::function<val<int64_t>(val<int64_t>, val<int64_t>)> wrapper = [&](val<int64_t> a, val<int64_t> b) {
			    // Two call sites: both must dedup to the same dispatcher symbol.
			    auto x = spec(a, 3); // specialized
			    auto y = spec(b, 4); // generic
			    return x + y;
		    };
		    auto fn = engine.registerFunction(wrapper);
		    // 2^3 + 5^4 = 8 + 625 = 633
		    REQUIRE(fn(2, 5) == 633);
		    // 4^3 + 2^4 = 64 + 16 = 80
		    REQUIRE(fn(4, 2) == 80);
	    },
	    /*include_interpreter=*/true);
}

TEST_CASE("SpecializedNautilusFunction backends: profile drives stabilization through compiled calls") {
	// Drives the profile via real compiled calls (rather than priming it
	// directly), ensuring the profile-update proxy call lowers correctly on
	// each backend.
	nautilus::testing::forEachBackend(
	    [](engine::NautilusEngine& engine) {
		    SpecializedNautilusFunction<val<int64_t>(val<int64_t>, val<int64_t>), 1> spec {"spec_be_drive",
		                                                                                   specBackendPow};
		    std::function<val<int64_t>(val<int64_t>, val<int64_t>)> wrapper = [&](val<int64_t> b, val<int64_t> e) {
			    return spec(b, e);
		    };
		    auto fn = engine.registerFunction(wrapper);
		    REQUIRE_FALSE(spec.profile<0>().isStable());
		    for (int i = 0; i < 2048; ++i) {
			    REQUIRE(fn(2, 5) == 32);
		    }
		    REQUIRE(spec.profile<0>().isStable());
		    REQUIRE(spec.profile<0>().dominant() == 5);
	    },
	    /*include_interpreter=*/true);
}

} // namespace nautilus::engine
#endif

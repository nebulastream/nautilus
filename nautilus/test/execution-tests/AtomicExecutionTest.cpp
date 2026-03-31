#include "AtomicFunctions.hpp"
#include "nautilus/Engine.hpp"
#include <catch2/catch_all.hpp>

namespace nautilus::engine {

void atomicTest(engine::NautilusEngine& engine) {

	SECTION("atomic_load i32") {
		auto f = engine.registerFunction(atomicLoadI32);
		int32_t value = 42;
		REQUIRE(f(&value) == 42);
	}

	SECTION("atomic_store i32") {
		auto f = engine.registerFunction(atomicStoreI32);
		int32_t value = 0;
		f(&value, (int32_t) 99);
		REQUIRE(value == 99);
	}

	SECTION("atomic_load i64") {
		auto f = engine.registerFunction(atomicLoadI64);
		int64_t value = 1234567890LL;
		REQUIRE(f(&value) == 1234567890LL);
	}

	SECTION("atomic_store i64") {
		auto f = engine.registerFunction(atomicStoreI64);
		int64_t value = 0;
		f(&value, (int64_t) 9876543210LL);
		REQUIRE(value == 9876543210LL);
	}

	SECTION("atomic_load then atomic_store roundtrip i32") {
		auto fStore = engine.registerFunction(atomicStoreI32);
		auto fLoad = engine.registerFunction(atomicLoadI32);
		int32_t value = 0;
		fStore(&value, (int32_t) 77);
		REQUIRE(fLoad(&value) == 77);
	}

	SECTION("atomic_fetch_add i32") {
		auto f = engine.registerFunction(atomicFetchAddI32);
		int32_t value = 10;
		auto old = f(&value, (int32_t) 5);
		REQUIRE(old == 10);
		REQUIRE(value == 15);
	}

	SECTION("atomic_fetch_add i64") {
		auto f = engine.registerFunction(atomicFetchAddI64);
		int64_t value = 100;
		auto old = f(&value, (int64_t) 50);
		REQUIRE(old == 100);
		REQUIRE(value == 150);
	}

	SECTION("atomic_fetch_sub i32") {
		auto f = engine.registerFunction(atomicFetchSubI32);
		int32_t value = 20;
		auto old = f(&value, (int32_t) 7);
		REQUIRE(old == 20);
		REQUIRE(value == 13);
	}

	SECTION("atomic_exchange i32") {
		auto f = engine.registerFunction(atomicExchangeI32);
		int32_t value = 42;
		auto old = f(&value, (int32_t) 99);
		REQUIRE(old == 42);
		REQUIRE(value == 99);
	}

	SECTION("atomic_exchange i64") {
		auto f = engine.registerFunction(atomicExchangeI64);
		int64_t value = 1000;
		auto old = f(&value, (int64_t) 2000);
		REQUIRE(old == 1000);
		REQUIRE(value == 2000);
	}

	SECTION("atomic_compare_exchange i32 success") {
		auto f = engine.registerFunction(atomicCasI32);
		int32_t value = 42;
		int32_t expected = 42;
		auto success = f(&value, &expected, (int32_t) 99);
		REQUIRE(success == true);
		REQUIRE(value == 99);
	}

	SECTION("atomic_compare_exchange i32 failure") {
		auto f = engine.registerFunction(atomicCasI32);
		int32_t value = 42;
		int32_t expected = 10; // wrong expectation
		auto success = f(&value, &expected, (int32_t) 99);
		REQUIRE(success == false);
		REQUIRE(value == 42);
		REQUIRE(expected == 42); // expected updated to actual
	}

	SECTION("atomic_compare_exchange i64 success") {
		auto f = engine.registerFunction(atomicCasI64);
		int64_t value = 1000;
		int64_t expected = 1000;
		auto success = f(&value, &expected, (int64_t) 2000);
		REQUIRE(success == true);
		REQUIRE(value == 2000);
	}

	SECTION("atomic accumulate") {
		auto f = engine.registerFunction(atomicAccumulate);
		int32_t value = 100;
		auto result = f(&value, (int32_t) 25);
		REQUIRE(result == 125);
		REQUIRE(value == 125);
	}
}

TEST_CASE("Atomic Interpreter Test") {
	engine::Options options;
	options.setOption("engine.Compilation", false);
	auto engine = engine::NautilusEngine(options);
	atomicTest(engine);
}

#ifdef ENABLE_TRACING
TEST_CASE("Atomic Compiler Test") {
	std::vector<std::string> backends = {};
#ifdef ENABLE_MLIR_BACKEND
	backends.emplace_back("mlir");
#endif
#ifdef ENABLE_C_BACKEND
	backends.emplace_back("cpp");
#endif
#ifdef ENABLE_BC_BACKEND
	backends.emplace_back("bc");
#endif
#ifdef ENABLE_ASMJIT_BACKEND
	backends.emplace_back("asmjit");
#endif
	for (auto& backend : backends) {
		DYNAMIC_SECTION(backend) {
			engine::Options options;
			options.setOption("engine.backend", backend);
			auto engine = engine::NautilusEngine(options);
			atomicTest(engine);
		}
	}
}
#endif
} // namespace nautilus::engine

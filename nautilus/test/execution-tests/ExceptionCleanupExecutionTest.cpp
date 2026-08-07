#include "ExecutionTest.hpp"
#include "nautilus/Engine.hpp"
#include "nautilus/function.hpp"
#include "nautilus/val_func.hpp"
#include "nautilus/val_std.hpp"
#include <catch2/catch_test_macros.hpp>
#include <stdexcept>
#include <vector>

namespace nautilus::testing {

namespace {

thread_local std::vector<int32_t> destructionOrder;
thread_local int32_t postThrowCalls;

struct CleanupValue {
	explicit CleanupValue(int32_t id) noexcept : id(id) {
	}
	~CleanupValue() noexcept {
		destructionOrder.push_back(id);
	}
	int32_t id;
};

void throwRuntime(int32_t value) {
	throw std::runtime_error("runtime failure " + std::to_string(value));
}

void recordPostThrowCall() noexcept {
	++postThrowCalls;
}

val<int32_t> cleanupTwoValuesThenThrow(val<int32_t> value) {
	val<CleanupValue> first(val<int32_t> {1});
	val<CleanupValue> second(val<int32_t> {2});
	invoke(throwRuntime, value);
	return value;
}

val<int32_t> throwWithoutCleanup(val<int32_t> value) {
	invoke(throwRuntime, value);
	invoke(recordPostThrowCall);
	return value;
}

struct ThrowingConstructionValue {
	explicit ThrowingConstructionValue(int32_t id) : id(id) {
		if (id == 2) {
			throw std::runtime_error("constructor failure");
		}
	}
	~ThrowingConstructionValue() noexcept {
		destructionOrder.push_back(id);
	}
	int32_t id;
};

val<int32_t> cleanupEarlierValueWhenConstructionThrows() {
	val<ThrowingConstructionValue> first(val<int32_t> {1});
	val<ThrowingConstructionValue> second(val<int32_t> {2});
	return 0;
}

val<int32_t> cleanupThenIndirectThrow(val<void (*)(int32_t)> function, val<int32_t> value) {
	val<CleanupValue> cleanup(val<int32_t> {9});
	function(value);
	return value;
}

} // namespace

TEST_CASE("exceptions without cleanup state return directly to the host") {
	forEachBackend([](auto& engine) {
		auto function = engine.registerFunction(throwWithoutCleanup);
		postThrowCalls = 0;

		try {
			(void) function(7);
			FAIL("expected exception");
		} catch (const std::runtime_error& exception) {
			REQUIRE(std::string(exception.what()) == "runtime failure 7");
		} catch (...) {
			FAIL("unexpected exception type");
		}
		REQUIRE(postThrowCalls == 0);
	});
}

TEST_CASE("a throwing constructor cleans earlier values but not its own storage") {
	forEachBackend([](auto& engine) {
		auto function = engine.registerFunction(cleanupEarlierValueWhenConstructionThrows);
		destructionOrder.clear();

		try {
			(void) function();
			FAIL("expected exception");
		} catch (const std::runtime_error& exception) {
			REQUIRE(std::string(exception.what()) == "constructor failure");
		} catch (...) {
			FAIL("unexpected exception type");
		}
		REQUIRE(destructionOrder == std::vector<int32_t> {1});
	});
}

TEST_CASE("indirect throwing calls clean active values") {
	forEachBackend([](auto& engine) {
		auto function = engine.registerFunction(cleanupThenIndirectThrow);
		destructionOrder.clear();

		try {
			(void) function(throwRuntime, 11);
			FAIL("expected exception");
		} catch (const std::runtime_error& exception) {
			REQUIRE(std::string(exception.what()) == "runtime failure 11");
		} catch (...) {
			FAIL("unexpected exception type");
		}
		REQUIRE(destructionOrder == std::vector<int32_t> {9});
	});
}

TEST_CASE("all compiled backends unwind active struct values in reverse order") {
	forEachBackendWithTraceMode([](auto& engine) {
		auto function = engine.registerFunction(cleanupTwoValuesThenThrow);
		destructionOrder.clear();

		try {
			(void) function(42);
			FAIL("expected exception");
		} catch (const std::runtime_error& exception) {
			REQUIRE(std::string(exception.what()) == "runtime failure 42");
		} catch (const std::exception& exception) {
			FAIL("unexpected std::exception: " << exception.what());
		} catch (...) {
			FAIL("unexpected non-standard exception");
		}
		REQUIRE(destructionOrder == std::vector<int32_t> {2, 1});
	});
}

} // namespace nautilus::testing

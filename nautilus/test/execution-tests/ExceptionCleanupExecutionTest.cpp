#include "ExecutionTest.hpp"
#include "nautilus/Engine.hpp"
#include "nautilus/function.hpp"
#include "nautilus/nautilus_function.hpp"
#include "nautilus/val_func.hpp"
#include "nautilus/val_std.hpp"
#include <catch2/catch_test_macros.hpp>
#include <filesystem>
#include <fstream>
#include <set>
#include <sstream>
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

val<int32_t> nestedCleanupThenThrow(val<int32_t> value) {
	val<CleanupValue> inner(val<int32_t> {2});
	invoke(throwRuntime, value);
	return value;
}

auto nestedThrowingFunction = NautilusFunction {"nestedCleanupThenThrow", nestedCleanupThenThrow};

val<int32_t> cleanupAcrossNestedThrow(val<int32_t> value) {
	val<CleanupValue> outer(val<int32_t> {1});
	return nestedThrowingFunction(value);
}

std::set<std::filesystem::path> currentDumpDirectories() {
	auto result = std::set<std::filesystem::path> {};
	const auto root = std::filesystem::temp_directory_path() / "dump";
	if (std::filesystem::exists(root)) {
		for (const auto& entry : std::filesystem::directory_iterator(root)) {
			result.insert(entry.path());
		}
	}
	return result;
}

std::string readNewDump(const std::set<std::filesystem::path>& previous, const std::string& fileName,
                        const std::string& marker = "nestedCleanupThenThrow") {
	const auto root = std::filesystem::temp_directory_path() / "dump";
	if (!std::filesystem::exists(root)) {
		return {};
	}
	for (const auto& directory : std::filesystem::directory_iterator(root)) {
		if (previous.contains(directory.path())) {
			continue;
		}
		for (const auto& entry : std::filesystem::recursive_directory_iterator(directory)) {
			if (!entry.is_regular_file() || entry.path().filename() != fileName) {
				continue;
			}
			std::ifstream stream(entry.path());
			auto content = std::stringstream {};
			content << stream.rdbuf();
			if (content.str().find(marker) != std::string::npos) {
				return content.str();
			}
		}
	}
	return {};
}

std::string llvmFunctionBody(const std::string& module, const std::string& functionName) {
	const auto name = "@" + functionName + "(";
	auto begin = size_t {0};
	while ((begin = module.find("define ", begin)) != std::string::npos) {
		const auto headerEnd = module.find('{', begin);
		const auto namePosition = module.find(name, begin);
		if (headerEnd != std::string::npos && namePosition != std::string::npos && namePosition < headerEnd) {
			const auto end = module.find("\n}", headerEnd);
			if (end != std::string::npos) {
				return module.substr(begin, end + 2 - begin);
			}
			return {};
		}
		if (headerEnd == std::string::npos) {
			break;
		}
		begin = headerEnd + 1;
	}
	return {};
}

size_t countOccurrences(const std::string& value, const std::string& needle) {
	auto count = size_t {0};
	auto position = size_t {0};
	while ((position = value.find(needle, position)) != std::string::npos) {
		++count;
		position += needle.size();
	}
	return count;
}

std::string lineContaining(const std::string& value, const std::string& needle) {
	const auto position = value.find(needle);
	if (position == std::string::npos) {
		return {};
	}
	const auto begin = value.rfind('\n', position);
	const auto end = value.find('\n', position);
	const auto lineBegin = begin == std::string::npos ? 0 : begin + 1;
	return value.substr(lineBegin, end == std::string::npos ? std::string::npos : end - lineBegin);
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

TEST_CASE("nested Nautilus frames unwind their active struct values") {
	forEachBackendWithTraceMode([](auto& engine) {
		auto function = engine.registerFunction(cleanupAcrossNestedThrow);
		destructionOrder.clear();

		try {
			(void) function(42);
			FAIL("expected exception");
		} catch (const std::runtime_error& exception) {
			REQUIRE(std::string(exception.what()) == "runtime failure 42");
		} catch (...) {
			FAIL("unexpected exception type");
		}
		REQUIRE(destructionOrder == std::vector<int32_t> {2, 1});
	});
}

#ifdef ENABLE_MLIR_BACKEND
TEST_CASE("MLIR keeps potentially throwing calls without cleanup as calls") {
	const auto previousDumps = currentDumpDirectories();
	auto engine =
	    makeEngine("mlir", [](engine::Options& options) { options.setOption("dump.before_llvm_optimization", true); });
	auto function = engine.registerFunction(throwWithoutCleanup);
	REQUIRE_THROWS_AS((void) function(7), std::runtime_error);

	const auto llvm = readNewDump(previousDumps, "before_llvm_optimization.ll", "throwRuntime");
	REQUIRE_FALSE(llvm.empty());
	const auto execute = llvmFunctionBody(llvm, "execute");
	const auto throwingCall = lineContaining(execute, "throwRuntime");
	REQUIRE(throwingCall.find("call") != std::string::npos);
	REQUIRE(throwingCall.find("invoke") == std::string::npos);
	REQUIRE(execute.find("landingpad") == std::string::npos);
}

TEST_CASE("MLIR materializes nested cleanup frames before LLVM inlining") {
	const auto previousDumps = currentDumpDirectories();
	auto engine = makeEngine("mlir", [](engine::Options& options) {
		options.setOption("dump.before_llvm_optimization", true);
		options.setOption("dump.after_llvm_generation", true);
	});
	auto function = engine.registerFunction(cleanupAcrossNestedThrow);
	destructionOrder.clear();
	REQUIRE_THROWS_AS((void) function(42), std::runtime_error);
	REQUIRE(destructionOrder == std::vector<int32_t> {2, 1});

	const auto beforeOptimization = readNewDump(previousDumps, "before_llvm_optimization.ll");
	const auto afterOptimization = readNewDump(previousDumps, "after_llvm_generation.ll");
	REQUIRE_FALSE(beforeOptimization.empty());
	REQUIRE_FALSE(afterOptimization.empty());

	const auto beforeExecute = llvmFunctionBody(beforeOptimization, "execute");
	const auto beforeNested = llvmFunctionBody(beforeOptimization, "nestedCleanupThenThrow");
	REQUIRE(lineContaining(beforeExecute, "@nestedCleanupThenThrow").find("invoke") != std::string::npos);
	REQUIRE(countOccurrences(beforeExecute, "landingpad") >= 1);
	REQUIRE(lineContaining(beforeNested, "throwRuntime").find("invoke") != std::string::npos);
	REQUIRE(countOccurrences(beforeNested, "landingpad") >= 1);

	const auto afterExecute = llvmFunctionBody(afterOptimization, "execute");
	REQUIRE(afterExecute.find("@nestedCleanupThenThrow") == std::string::npos);
	REQUIRE(afterExecute.find("throwRuntime") != std::string::npos);
	REQUIRE(countOccurrences(afterExecute, "landingpad") >= 1);
	REQUIRE(countOccurrences(afterExecute, "@__nautilus_cleanup_dtor_") == 2);
}
#endif

} // namespace nautilus::testing

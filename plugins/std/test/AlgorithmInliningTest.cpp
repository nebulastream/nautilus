// Verifies that the runtime functions behind `nautilus/std/algorithm.h` are
// picked up by the `nautilus-inlining` plugin: the Clang pass registers their
// bitcode, and the MLIR backend inlines them into the generated code when
// `mlir.inline_invoke_calls` is enabled.

#include "ExecutionTest.hpp"
#include "nautilus/Engine.hpp"
#include "nautilus/compiler/backends/mlir/LLVMBackendHooks.hpp"
#include "nautilus/inline.hpp"
#include "nautilus/std/algorithm.h"
#include <catch2/catch_all.hpp>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <string>
#include <vector>

namespace nautilus::engine {

#if defined(ENABLE_TRACING) && defined(ENABLE_MLIR_BACKEND)

// Mirrors the condition in plugins/inlining/test/InliningExecutionTest.cpp.
#if defined(__clang__) && __clang_major__ >= 19 && __clang_major__ < 22 && !defined(__aarch64__) && !defined(__arm__)
#define NAUTILUS_INLINING_EXPECTED_ACTIVE 1
#else
#define NAUTILUS_INLINING_EXPECTED_ACTIVE 0
#endif

val<int32_t> inlinedNthElement(val<int32_t*> array, val<size_t> size, val<size_t> n) {
	auto nth = array + n;
	nautilus::nth_element(array, nth, array + size);
	return *nth;
}

void inlinedSortDescending(val<int32_t*> array, val<size_t> size) {
	nautilus::sort<std::greater<>>(array, array + size);
}

val<int32_t> inlinedPartialSortMinMax(val<int32_t*> array, val<size_t> size) {
	nautilus::partial_sort(array, array + val<size_t>(1), array + size);
	auto max = nautilus::max_element(array, array + size);
	return *max - *array;
}

template <typename Fn>
static bool hasBitcode(Fn* fn) {
	return InlineFunctionRegistry::instance().containsFunctionBitcode(reinterpret_cast<void*>(fn));
}

TEST_CASE("Algorithm runtime functions are registered for inlining", "[std][inlining]") {
	// Taking the addresses instantiates the runtime functions in this TU, so the
	// Clang pass sees their definitions.
	const bool nth = hasBitcode(&nautilus::detail::nth_element_impl<int32_t, std::less<>>);
	const bool sort = hasBitcode(&nautilus::detail::sort_impl<int32_t, std::greater<>>);
	const bool partialSort = hasBitcode(&nautilus::detail::partial_sort_impl<int32_t, std::less<>>);
	const bool maxElement = hasBitcode(&nautilus::detail::max_element_impl<int32_t, std::less<>>);
#if NAUTILUS_INLINING_EXPECTED_ACTIVE
	REQUIRE(nth);
	REQUIRE(sort);
	REQUIRE(partialSort);
	REQUIRE(maxElement);
#else
	(void) nth;
	(void) sort;
	(void) partialSort;
	(void) maxElement;
#endif
}

TEST_CASE("Algorithm calls execute with inlining enabled", "[std][inlining]") {
	auto engine = nautilus::testing::makeEngine("mlir", [](engine::Options& opts) {
		opts.setOption("engine.Compilation", true);
		opts.setOption("mlir.enableMultithreading", false);
		opts.setOption("mlir.inline_invoke_calls", true);
	});

	SECTION("nth_element") {
		auto f = engine.registerFunction(inlinedNthElement);
		const std::vector<int32_t> input = {8, 3, 7, 1, 9, 2, 6, 4, 5};
		for (size_t n = 0; n < input.size(); n++) {
			auto values = input;
			REQUIRE(f(values.data(), values.size(), n) == static_cast<int32_t>(n + 1));
		}
	}
	SECTION("sort descending") {
		auto f = engine.registerFunction(inlinedSortDescending);
		std::vector<int32_t> values = {5, -3, 9, 0, 2};
		f(values.data(), values.size());
		REQUIRE(values == std::vector<int32_t> {9, 5, 2, 0, -3});
	}
	SECTION("partial_sort and max_element") {
		auto f = engine.registerFunction(inlinedPartialSortMinMax);
		std::vector<int32_t> values = {4, -7, 12, 0, 3};
		REQUIRE(f(values.data(), values.size()) == 19);
	}
}

TEST_CASE("Algorithm calls are inlined into the generated LLVM IR", "[std][inlining]") {
	if (!hasBitcode(&nautilus::detail::nth_element_impl<int32_t, std::less<>>)) {
		SKIP("Clang inlining pass plugin is not active on this host.");
	}

	auto dumpPath = std::filesystem::temp_directory_path() / "nautilus-std-algorithm-inlining-ir";
	std::filesystem::remove_all(dumpPath);
	std::filesystem::create_directories(dumpPath);

	engine::Options options;
	options.setOption("engine.backend", "mlir");
	options.setOption("dump.after_llvm_generation", true);
	options.setOption("dump.all", true);
	options.setOption("dump.console", false);
	options.setOption("dump.path", dumpPath.string());
	options.setOption("mlir.enableMultithreading", false);
	options.setOption("mlir.inline_invoke_calls", true);
	auto engine = engine::NautilusEngine(options);
	auto function = engine.registerFunction(inlinedNthElement);
	auto generatedLLVMFile = function.getExecutable()->getGeneratedFile("after_llvm_generation");
	REQUIRE_FALSE(generatedLLVMFile.empty());
	std::ifstream in(std::string {generatedLLVMFile});
	REQUIRE(in.good());
	const std::string ir((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
	std::filesystem::remove_all(dumpPath);

	// Without inlining, the invoke() lowers to a call of the runtime function,
	// named after its address. After inlining, the IR contains its body instead.
	// Calls to libstdc++ internals used by the body may remain.
	const auto& hooks = nautilus::compiler::mlir::getLLVMBackendHooks();
	REQUIRE(static_cast<bool>(hooks.callNameOverride));
	const auto implName =
	    hooks.callNameOverride(reinterpret_cast<void*>(&nautilus::detail::nth_element_impl<int32_t, std::less<>>));
	REQUIRE(implName.has_value());
	INFO(ir);
	REQUIRE(ir.find("@\"" + *implName + "\"") == std::string::npos);
	REQUIRE(ir.find("nth_element_impl") != std::string::npos);

	std::vector<int32_t> values = {8, 3, 7, 1, 9, 2, 6, 4, 5};
	REQUIRE(function(values.data(), values.size(), (size_t) 4) == 5);
}

#endif

} // namespace nautilus::engine

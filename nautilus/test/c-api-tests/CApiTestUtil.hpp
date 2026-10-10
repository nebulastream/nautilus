#pragma once

#include "IRCApiPrograms.h"
#include "nautilus/c/engine.h"
#include "nautilus/c/ir.h"
#include <catch2/catch_all.hpp>
#include <cstdint>
#include <memory>
#include <string>

namespace nautilus::testing {

struct GraphDeleter {
	void operator()(NautilusIROpaqueGraph* graph) const {
		nautilus_ir_graph_dispose(graph);
	}
};
using Graph = std::unique_ptr<NautilusIROpaqueGraph, GraphDeleter>;

struct ExecutableDeleter {
	void operator()(NautilusIROpaqueExecutable* executable) const {
		nautilus_ir_executable_dispose(executable);
	}
};
using Executable = std::unique_ptr<NautilusIROpaqueExecutable, ExecutableDeleter>;

inline std::string takeString(char* str) {
	std::string result = str != nullptr ? str : "";
	nautilus_ir_string_dispose(str);
	return result;
}

inline std::string lastError() {
	const char* error = nautilus_ir_get_last_error();
	return error != nullptr ? error : "";
}

inline int64_t externalHelper(int64_t a, int64_t b) {
	return a * 10 + b;
}

/// Builds every test program into one graph, so the cross-function paths
/// (internal calls, the function table) are exercised by every backend.
inline Graph buildAll() {
	Graph graph(nautilus_ir_graph_create("c-api-test"));
	REQUIRE(graph);
	REQUIRE(build_add(graph.get()) == 0);
	REQUIRE(build_sum_loop(graph.get()) == 0);
	REQUIRE(build_max(graph.get()) == 0);
	REQUIRE(build_factorial(graph.get()) == 0);
	REQUIRE(build_call_external(graph.get(), &externalHelper) == 0);
	REQUIRE(build_memory(graph.get()) == 0);
	REQUIRE(build_float(graph.get()) == 0);
	return graph;
}

template <typename F>
F function(NautilusIRExecutableRef executable, const char* name) {
	auto* fn = nautilus_ir_executable_get_function(executable, name);
	INFO(lastError());
	REQUIRE(fn != nullptr);
	return reinterpret_cast<F>(fn);
}

/// Runs every program buildAll() builds against @p executable.
inline void checkAllPrograms(NautilusIRExecutableRef executable) {
	auto add = function<int64_t (*)(int64_t, int64_t)>(executable, "add");
	REQUIRE(add(40, 2) == 42);
	REQUIRE(add(-5, 3) == -2);

	auto sumTo = function<int64_t (*)(int64_t)>(executable, "sum_to");
	REQUIRE(sumTo(0) == 0);
	REQUIRE(sumTo(10) == 45);
	REQUIRE(sumTo(1000) == 499500);

	auto maxIf = function<int32_t (*)(int32_t, int32_t)>(executable, "max_if");
	auto maxSelect = function<int32_t (*)(int32_t, int32_t)>(executable, "max_select");
	REQUIRE(maxIf(3, 7) == 7);
	REQUIRE(maxIf(9, -1) == 9);
	REQUIRE(maxSelect(3, 7) == 7);
	REQUIRE(maxSelect(9, -1) == 9);

	auto factorial = function<int64_t (*)(int64_t)>(executable, "factorial");
	REQUIRE(factorial(1) == 1);
	REQUIRE(factorial(10) == 3628800);

	auto callExternal = function<int64_t (*)(int64_t)>(executable, "call_external");
	REQUIRE(callExternal(4) == 42);

	auto storeThrough = function<void (*)(int64_t*, int64_t)>(executable, "store_through");
	int64_t out = 0;
	storeThrough(&out, 41);
	REQUIRE(out == 42);

	auto scale = function<double (*)(int32_t)>(executable, "scale");
	REQUIRE(scale(5) == 2.5);
}

} // namespace nautilus::testing

#include "IRCApiPrograms.h"
#include "nautilus/c/ir.h"
#include <catch2/catch_all.hpp>
#include <cstdint>
#include <memory>
#include <string>

namespace {

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

std::string takeString(char* str) {
	std::string result = str != nullptr ? str : "";
	nautilus_ir_string_dispose(str);
	return result;
}

std::string lastError() {
	const char* error = nautilus_ir_get_last_error();
	return error != nullptr ? error : "";
}

int64_t externalHelper(int64_t a, int64_t b) {
	return a * 10 + b;
}

/// Builds every test program into one graph, so the cross-function paths
/// (internal calls, the function table) are exercised by every backend.
Graph buildAll() {
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

} // namespace

TEST_CASE("C IR API: built graph verifies and prints") {
	auto graph = buildAll();
	char* errors = nullptr;
	const int status = nautilus_ir_graph_verify(graph.get(), &errors);
	INFO(takeString(errors));
	REQUIRE(status == 0);

	const auto text = takeString(nautilus_ir_graph_to_string(graph.get()));
	REQUIRE(text.find("sum_to") != std::string::npos);
	REQUIRE(text.find("factorial") != std::string::npos);
	REQUIRE(nautilus_ir_graph_get_function_count(graph.get()) == 8);
}

TEST_CASE("C IR API: compiled functions run on every available backend") {
	for (const char* backend : {"bc", "tbc", "cpp", "asmjit", "mlir"}) {
		if (!nautilus_ir_backend_is_available(backend)) {
			continue;
		}
		DYNAMIC_SECTION("backend " << backend) {
			auto graph = buildAll();
			Executable executable(nautilus_ir_graph_compile(graph.get(), backend, nullptr));
			INFO(lastError());
			REQUIRE(executable);

			auto add = function<int64_t (*)(int64_t, int64_t)>(executable.get(), "add");
			REQUIRE(add(40, 2) == 42);
			REQUIRE(add(-5, 3) == -2);

			auto sumTo = function<int64_t (*)(int64_t)>(executable.get(), "sum_to");
			REQUIRE(sumTo(0) == 0);
			REQUIRE(sumTo(10) == 45);
			REQUIRE(sumTo(1000) == 499500);

			auto maxIf = function<int32_t (*)(int32_t, int32_t)>(executable.get(), "max_if");
			auto maxSelect = function<int32_t (*)(int32_t, int32_t)>(executable.get(), "max_select");
			REQUIRE(maxIf(3, 7) == 7);
			REQUIRE(maxIf(9, -1) == 9);
			REQUIRE(maxSelect(3, 7) == 7);
			REQUIRE(maxSelect(9, -1) == 9);

			auto factorial = function<int64_t (*)(int64_t)>(executable.get(), "factorial");
			REQUIRE(factorial(1) == 1);
			REQUIRE(factorial(10) == 3628800);

			auto callExternal = function<int64_t (*)(int64_t)>(executable.get(), "call_external");
			REQUIRE(callExternal(4) == 42);

			auto storeThrough = function<void (*)(int64_t*, int64_t)>(executable.get(), "store_through");
			int64_t out = 0;
			storeThrough(&out, 41);
			REQUIRE(out == 42);

			auto scale = function<double (*)(int32_t)>(executable.get(), "scale");
			REQUIRE(scale(5) == 2.5);
		}
	}
}

TEST_CASE("C IR API: inspection walks functions, blocks and operations") {
	auto graph = buildAll();
	auto* fn = nautilus_ir_graph_get_function_by_name(graph.get(), "sum_to");
	REQUIRE(fn != nullptr);
	REQUIRE(std::string(nautilus_ir_function_get_name(fn)) == "sum_to");
	REQUIRE(nautilus_ir_function_get_return_type(fn) == NAUTILUS_IR_TYPE_I64);
	REQUIRE(nautilus_ir_function_get_block_count(fn) == 4);

	auto* entry = nautilus_ir_function_get_block(fn, 0);
	REQUIRE(nautilus_ir_block_get_argument_count(entry) == 1);
	REQUIRE(nautilus_ir_value_get_kind(nautilus_ir_block_get_argument(entry, 0)) == NAUTILUS_IR_OP_BLOCK_ARGUMENT);

	auto* zero = nautilus_ir_block_get_operation(entry, 0);
	REQUIRE(nautilus_ir_value_get_kind(zero) == NAUTILUS_IR_OP_CONST_INT);
	int64_t constant = -1;
	REQUIRE(nautilus_ir_value_get_const_int(zero, &constant) == 0);
	REQUIRE(constant == 0);
	double notAFloat = 0;
	REQUIRE(nautilus_ir_value_get_const_float(zero, &notAFloat) != 0);

	auto* branch = nautilus_ir_block_get_terminator(entry);
	REQUIRE(branch != nullptr);
	REQUIRE(nautilus_ir_value_get_kind(branch) == NAUTILUS_IR_OP_BRANCH);
	REQUIRE(nautilus_ir_value_get_successor_count(branch) == 1);
	auto* header = nautilus_ir_value_get_successor(branch, 0);
	REQUIRE(header == nautilus_ir_function_get_block(fn, 1));
	REQUIRE(nautilus_ir_value_get_successor_argument_count(branch, 0) == 3);
	REQUIRE(nautilus_ir_value_get_successor_argument(branch, 0, 0) == zero);
	REQUIRE(nautilus_ir_value_get_operand_count(branch) == 3);
	REQUIRE(nautilus_ir_value_get_operand(branch, 2) == nautilus_ir_block_get_argument(entry, 0));

	auto* ifOp = nautilus_ir_block_get_terminator(header);
	REQUIRE(nautilus_ir_value_get_kind(ifOp) == NAUTILUS_IR_OP_IF);
	REQUIRE(nautilus_ir_value_get_successor_count(ifOp) == 2);
	double probability = 0;
	REQUIRE(nautilus_ir_value_get_branch_probability(ifOp, &probability) == 0);
	REQUIRE(probability == 0.9);

	auto* compare = nautilus_ir_value_get_operand(ifOp, 0);
	REQUIRE(nautilus_ir_value_get_kind(compare) == NAUTILUS_IR_OP_COMPARE);
	REQUIRE(nautilus_ir_value_get_type(compare) == NAUTILUS_IR_TYPE_BOOL);
	NautilusIRComparator comparator = NAUTILUS_IR_CMP_EQ;
	REQUIRE(nautilus_ir_value_get_comparator(compare, &comparator) == 0);
	REQUIRE(comparator == NAUTILUS_IR_CMP_LT);

	// The recursive call resolves to factorial's own table entry.
	auto* factorial = nautilus_ir_graph_get_function_by_name(graph.get(), "factorial");
	const auto factorialId = nautilus_ir_function_get_callee(graph.get(), factorial);
	REQUIRE(nautilus_ir_graph_get_callee_linkage(graph.get(), factorialId) == NAUTILUS_IR_LINKAGE_INTERNAL);
	REQUIRE(nautilus_ir_graph_get_callee_function(graph.get(), factorialId) == factorial);
	auto* recurse = nautilus_ir_function_get_block(factorial, 2);
	bool sawCall = false;
	for (size_t i = 0; i < nautilus_ir_block_get_operation_count(recurse); ++i) {
		auto* op = nautilus_ir_block_get_operation(recurse, i);
		if (nautilus_ir_value_get_kind(op) == NAUTILUS_IR_OP_CALL) {
			NautilusIRCalleeId callee = NAUTILUS_IR_INVALID_CALLEE;
			REQUIRE(nautilus_ir_value_get_callee(op, &callee) == 0);
			REQUIRE(callee == factorialId);
			sawCall = true;
		}
	}
	REQUIRE(sawCall);

	// The external callee keeps its address.
	auto* callExternal = nautilus_ir_graph_get_function_by_name(graph.get(), "call_external");
	auto* call = nautilus_ir_block_get_operation(nautilus_ir_function_get_block(callExternal, 0), 1);
	NautilusIRCalleeId externalId = NAUTILUS_IR_INVALID_CALLEE;
	REQUIRE(nautilus_ir_value_get_callee(call, &externalId) == 0);
	REQUIRE(nautilus_ir_graph_get_callee_linkage(graph.get(), externalId) == NAUTILUS_IR_LINKAGE_EXTERNAL);
	REQUIRE(nautilus_ir_graph_get_callee_address(graph.get(), externalId) == reinterpret_cast<void*>(&externalHelper));

	auto* storeThrough = nautilus_ir_graph_get_function_by_name(graph.get(), "store_through");
	REQUIRE(nautilus_ir_function_get_stack_slot_count(storeThrough) == 1);
	size_t size = 0;
	size_t align = 0;
	REQUIRE(nautilus_ir_function_get_stack_slot(storeThrough, 0, &size, &align) == 0);
	REQUIRE(size == sizeof(int64_t));
	REQUIRE(align == alignof(int64_t));
}

TEST_CASE("C IR API: optimize rewrites the graph in place") {
	Graph graph(nautilus_ir_graph_create("optimize"));
	auto* fb = nautilus_ir_function_builder_create(graph.get(), "folded", NAUTILUS_IR_TYPE_I32);
	auto* entry = nautilus_ir_function_builder_add_block(fb, nullptr, 0);
	auto* two = nautilus_ir_build_const_int(fb, entry, 2, NAUTILUS_IR_TYPE_I32);
	auto* three = nautilus_ir_build_const_int(fb, entry, 3, NAUTILUS_IR_TYPE_I32);
	nautilus_ir_build_return(fb, entry, nautilus_ir_build_binary(fb, entry, NAUTILUS_IR_BINARY_MUL, two, three));
	auto* fn = nautilus_ir_function_builder_finish(fb);
	REQUIRE(fn != nullptr);
	REQUIRE(nautilus_ir_block_get_operation_count(entry) == 4);

	REQUIRE(nautilus_ir_graph_optimize(graph.get(), NAUTILUS_IR_OPTIMIZE_FULL, nullptr) == 0);
	auto* optimizedEntry = nautilus_ir_function_get_block(fn, 0);
	REQUIRE(nautilus_ir_block_get_operation_count(optimizedEntry) == 2);
	int64_t folded = 0;
	REQUIRE(nautilus_ir_value_get_const_int(nautilus_ir_block_get_operation(optimizedEntry, 0), &folded) == 0);
	REQUIRE(folded == 6);

	// An optimized graph is closed for new functions.
	REQUIRE(nautilus_ir_function_builder_create(graph.get(), "late", NAUTILUS_IR_TYPE_VOID) == nullptr);
	REQUIRE(lastError().find("optimized") != std::string::npos);
}

TEST_CASE("C IR API: invalid construction is reported, not crashed on") {
	Graph graph(nautilus_ir_graph_create("errors"));
	auto* fb = nautilus_ir_function_builder_create(graph.get(), "f", NAUTILUS_IR_TYPE_I64);
	REQUIRE(fb != nullptr);
	REQUIRE(nautilus_ir_function_builder_create(graph.get(), "f", NAUTILUS_IR_TYPE_I64) == nullptr);
	REQUIRE(lastError().find("already exists") != std::string::npos);

	NautilusIRType params[] = {NAUTILUS_IR_TYPE_I64, NAUTILUS_IR_TYPE_BOOL};
	auto* entry = nautilus_ir_function_builder_add_block(fb, params, 2);
	auto* next = nautilus_ir_function_builder_add_block(fb, params, 1);
	auto* i64 = nautilus_ir_block_get_argument(entry, 0);
	auto* flag = nautilus_ir_block_get_argument(entry, 1);

	SECTION("type mismatches") {
		REQUIRE(nautilus_ir_build_const_int(fb, entry, 1, NAUTILUS_IR_TYPE_F64) == nullptr);
		REQUIRE(nautilus_ir_build_binary(fb, entry, NAUTILUS_IR_BINARY_LOGICAL_AND, i64, flag) == nullptr);
		REQUIRE(nautilus_ir_build_load(fb, entry, i64, NAUTILUS_IR_TYPE_I64) == nullptr);
		REQUIRE(nautilus_ir_build_return(fb, entry, flag) == nullptr);
		REQUIRE(lastError().find("return type") != std::string::npos);
	}

	SECTION("branch arity must match the target") {
		REQUIRE(nautilus_ir_build_branch(fb, entry, next, nullptr, 0) == nullptr);
		REQUIRE(lastError().find("argument count") != std::string::npos);
		NautilusIRValueRef wrongType[] = {flag};
		REQUIRE(nautilus_ir_build_branch(fb, entry, next, wrongType, 1) == nullptr);
		NautilusIRValueRef ok[] = {i64};
		REQUIRE(nautilus_ir_build_branch(fb, entry, next, ok, 1) != nullptr);
		// Nothing goes after a terminator.
		REQUIRE(nautilus_ir_build_const_int(fb, entry, 1, NAUTILUS_IR_TYPE_I64) == nullptr);
		REQUIRE(lastError().find("terminator") != std::string::npos);
	}

	SECTION("unfinished blocks fail finish and consume the builder") {
		REQUIRE(nautilus_ir_function_builder_finish(fb) == nullptr);
		REQUIRE(lastError().find("terminator") != std::string::npos);
		REQUIRE(nautilus_ir_graph_compile(graph.get(), "bc", nullptr) == nullptr);
	}

	SECTION("blocks of another builder are rejected") {
		auto* other = nautilus_ir_function_builder_create(graph.get(), "g", NAUTILUS_IR_TYPE_VOID);
		REQUIRE(nautilus_ir_build_return(other, entry, nullptr) == nullptr);
		REQUIRE(lastError().find("does not belong") != std::string::npos);
	}

	SECTION("unknown backends are rejected") {
		REQUIRE(nautilus_ir_build_return(fb, entry, i64) != nullptr);
		NautilusIRValueRef ok[] = {i64};
		(void) ok;
		REQUIRE(nautilus_ir_build_return(fb, next, nautilus_ir_block_get_argument(next, 0)) != nullptr);
		REQUIRE(nautilus_ir_function_builder_finish(fb) != nullptr);
		REQUIRE(nautilus_ir_graph_compile(graph.get(), "no-such-backend", nullptr) == nullptr);
		REQUIRE(lastError().find("not available") != std::string::npos);
	}
}

#include "CApiTestUtil.hpp"
#include "nautilus/c/ir.h"
#include <catch2/catch_all.hpp>
#include <cstdint>
#include <string>

using namespace nautilus::testing;

namespace {

NautilusIRFunctionRef findFunction(NautilusIRGraphRef graph, const char* name) {
	NautilusIRFunctionRef function = nullptr;
	REQUIRE(nautilus_ir_graph_find_function(graph, str(name), &function) == NAUTILUS_OK);
	return function;
}

std::vector<NautilusIRValueRef> operations(NautilusIRBlockRef block) {
	return collect<NautilusIRValueRef>([&](NautilusIRValueRef* out, size_t capacity) {
		return nautilus_ir_block_get_operations(block, out, capacity);
	});
}

std::vector<NautilusIRBlockRef> blocks(NautilusIRFunctionRef function) {
	return collect<NautilusIRBlockRef>([&](NautilusIRBlockRef* out, size_t capacity) {
		return nautilus_ir_function_get_blocks(function, out, capacity);
	});
}

} // namespace

TEST_CASE("C IR API: version and conventions") {
	REQUIRE(nautilus_c_api_version() == NAUTILUS_C_API_VERSION);
	REQUIRE(toStd(nautilus_ir_type_name(NAUTILUS_IR_TYPE_I64)) == "i64");
	REQUIRE(nautilus_ir_type_name(99).length == 0);

	// Strings are (pointer, length): a name need not be NUL-terminated.
	Graph graph(nautilus_ir_graph_create(str("strings")));
	const char name[] = {'s', 'u', 'm', 'X'};
	auto* fb = nautilus_ir_function_builder_create(graph.get(), NautilusStringRef {name, 3}, NAUTILUS_IR_TYPE_VOID);
	REQUIRE(fb != nullptr);
	auto* entry = nautilus_ir_function_builder_add_block(fb, nullptr, 0);
	REQUIRE(nautilus_ir_build_return(fb, entry, nullptr) != nullptr);
	auto* fn = nautilus_ir_function_builder_finish(fb);
	REQUIRE(toStd(nautilus_ir_function_get_name(fn)) == "sum");
	// Borrowed output strings are also NUL-terminated.
	REQUIRE(std::string(nautilus_ir_function_get_name(fn).data) == "sum");
}

TEST_CASE("C IR API: built graph verifies and prints") {
	auto graph = buildAll();
	NautilusString diagnostics {nullptr, 0};
	const auto status = nautilus_ir_graph_verify(graph.get(), &diagnostics);
	INFO(lastError());
	REQUIRE(status == NAUTILUS_OK);
	REQUIRE(diagnostics.data == nullptr);

	NautilusString text {nullptr, 0};
	REQUIRE(nautilus_ir_graph_to_string(graph.get(), &text) == NAUTILUS_OK);
	const auto dump = take(text);
	REQUIRE(dump.find("sum_to") != std::string::npos);
	REQUIRE(dump.find("factorial") != std::string::npos);
	REQUIRE(nautilus_ir_graph_get_functions(graph.get(), nullptr, 0) == 8);
}

TEST_CASE("C IR API: compiled functions run on every available backend") {
	for (const char* backend : {"bc", "tbc", "cpp", "asmjit", "mlir"}) {
		if (!nautilus_ir_backend_is_available(str(backend))) {
			continue;
		}
		DYNAMIC_SECTION("backend " << backend) {
			auto graph = buildAll();
			Executable executable(nautilus_ir_graph_compile(graph.get(), str(backend), nullptr));
			INFO(lastError());
			REQUIRE(executable);
			checkAllPrograms(executable.get());
		}
	}
}

TEST_CASE("C IR API: inspection walks functions, blocks and operations") {
	auto graph = buildAll();
	auto* fn = findFunction(graph.get(), "sum_to");
	REQUIRE(toStd(nautilus_ir_function_get_name(fn)) == "sum_to");
	REQUIRE(nautilus_ir_function_get_return_type(fn) == NAUTILUS_IR_TYPE_I64);
	const auto fnBlocks = blocks(fn);
	REQUIRE(fnBlocks.size() == 4);

	auto* entry = nautilus_ir_function_get_entry_block(fn);
	REQUIRE(entry == fnBlocks[0]);
	REQUIRE(nautilus_ir_block_get_arguments(entry, nullptr, 0) == 1);
	REQUIRE(nautilus_ir_value_get_kind(nautilus_ir_block_get_argument(entry, 0)) == NAUTILUS_IR_OP_BLOCK_ARGUMENT);

	// A partial copy-out fills what fits and still reports the total.
	NautilusIRValueRef first = nullptr;
	REQUIRE(nautilus_ir_block_get_operations(entry, &first, 1) == 2);
	REQUIRE(nautilus_ir_value_get_kind(first) == NAUTILUS_IR_OP_CONST_INT);
	int64_t constant = -1;
	REQUIRE(nautilus_ir_value_get_const_int(first, &constant) == NAUTILUS_OK);
	REQUIRE(constant == 0);
	double notAFloat = 0;
	REQUIRE(nautilus_ir_value_get_const_float(first, &notAFloat) == NAUTILUS_ERROR_INVALID_ARGUMENT);
	REQUIRE(nautilus_last_error_code() == NAUTILUS_ERROR_INVALID_ARGUMENT);

	auto* branch = nautilus_ir_block_get_terminator(entry);
	REQUIRE(nautilus_ir_value_get_kind(branch) == NAUTILUS_IR_OP_BRANCH);
	REQUIRE(nautilus_ir_value_is_terminator(branch));
	REQUIRE(nautilus_ir_value_get_operands(branch, nullptr, 0) == 0);
	NautilusIRBlockRef header = nullptr;
	REQUIRE(nautilus_ir_value_get_successors(branch, &header, 1) == 1);
	REQUIRE(header == fnBlocks[1]);
	const auto passed = collect<NautilusIRValueRef>([&](NautilusIRValueRef* out, size_t capacity) {
		return nautilus_ir_value_get_successor_arguments(branch, 0, out, capacity);
	});
	REQUIRE(passed.size() == 3);
	REQUIRE(passed[0] == first);
	REQUIRE(passed[2] == nautilus_ir_block_get_argument(entry, 0));
	REQUIRE(nautilus_ir_value_get_successor_arguments(branch, 1, nullptr, 0) == 0);
	REQUIRE(nautilus_last_error_code() == NAUTILUS_ERROR_INVALID_ARGUMENT);

	auto* ifOp = nautilus_ir_block_get_terminator(header);
	REQUIRE(nautilus_ir_value_get_kind(ifOp) == NAUTILUS_IR_OP_IF);
	const auto successors = collect<NautilusIRBlockRef>([&](NautilusIRBlockRef* out, size_t capacity) {
		return nautilus_ir_value_get_successors(ifOp, out, capacity);
	});
	REQUIRE(successors == std::vector<NautilusIRBlockRef> {fnBlocks[2], fnBlocks[3]});
	double probability = 0;
	REQUIRE(nautilus_ir_value_get_branch_probability(ifOp, &probability) == NAUTILUS_OK);
	REQUIRE(probability == 0.9);

	NautilusIRValueRef compare = nullptr;
	REQUIRE(nautilus_ir_value_get_operands(ifOp, &compare, 1) == 1);
	REQUIRE(nautilus_ir_value_get_kind(compare) == NAUTILUS_IR_OP_COMPARE);
	REQUIRE(nautilus_ir_value_get_type(compare) == NAUTILUS_IR_TYPE_BOOL);
	NautilusIRComparator comparator = NAUTILUS_IR_CMP_EQ;
	REQUIRE(nautilus_ir_value_get_comparator(compare, &comparator) == NAUTILUS_OK);
	REQUIRE(comparator == NAUTILUS_IR_CMP_LT);

	// The recursive call resolves to factorial's own table entry.
	auto* factorial = findFunction(graph.get(), "factorial");
	const auto factorialId = nautilus_ir_function_get_callee(graph.get(), factorial);
	NautilusIRCalleeInfo info {};
	REQUIRE(nautilus_ir_graph_get_callee_info(graph.get(), factorialId, &info) == NAUTILUS_OK);
	REQUIRE(info.linkage == NAUTILUS_IR_LINKAGE_INTERNAL);
	REQUIRE(info.function == factorial);
	REQUIRE(info.address == nullptr);
	bool sawCall = false;
	for (auto* op : operations(blocks(factorial)[2])) {
		if (nautilus_ir_value_get_kind(op) == NAUTILUS_IR_OP_CALL) {
			NautilusIRCalleeId callee = NAUTILUS_IR_INVALID_CALLEE;
			REQUIRE(nautilus_ir_value_get_callee(op, &callee) == NAUTILUS_OK);
			REQUIRE(callee == factorialId);
			sawCall = true;
		}
	}
	REQUIRE(sawCall);

	// The external callee keeps its address and name.
	auto* call = operations(nautilus_ir_function_get_entry_block(findFunction(graph.get(), "call_external")))[1];
	NautilusIRCalleeId externalId = NAUTILUS_IR_INVALID_CALLEE;
	REQUIRE(nautilus_ir_value_get_callee(call, &externalId) == NAUTILUS_OK);
	REQUIRE(nautilus_ir_graph_get_callee_info(graph.get(), externalId, &info) == NAUTILUS_OK);
	REQUIRE(info.linkage == NAUTILUS_IR_LINKAGE_EXTERNAL);
	REQUIRE(info.address == reinterpret_cast<void*>(&externalHelper));
	REQUIRE(info.function == nullptr);
	REQUIRE(nautilus_ir_graph_get_callee_info(graph.get(), 12345, &info) == NAUTILUS_ERROR_NOT_FOUND);

	auto* storeThrough = findFunction(graph.get(), "store_through");
	const auto slots = collect<NautilusIRStackSlot>([&](NautilusIRStackSlot* out, size_t capacity) {
		return nautilus_ir_function_get_stack_slots(storeThrough, out, capacity);
	});
	REQUIRE(slots.size() == 1);
	REQUIRE(slots[0].size == sizeof(int64_t));
	REQUIRE(slots[0].align == alignof(int64_t));

	NautilusIRFunctionRef missing = nullptr;
	REQUIRE(nautilus_ir_graph_find_function(graph.get(), str("missing"), &missing) == NAUTILUS_ERROR_NOT_FOUND);
	REQUIRE(missing == nullptr);
}

TEST_CASE("C IR API: optimize rewrites the graph in place") {
	Graph graph(nautilus_ir_graph_create(str("optimize")));
	auto* fb = nautilus_ir_function_builder_create(graph.get(), str("folded"), NAUTILUS_IR_TYPE_I32);
	REQUIRE(nautilus_ir_function_builder_set_attribute(fb, str("entry"), str("true")) == NAUTILUS_OK);
	auto* entry = nautilus_ir_function_builder_add_block(fb, nullptr, 0);
	auto* two = nautilus_ir_build_const_int(fb, entry, 2, NAUTILUS_IR_TYPE_I32);
	auto* three = nautilus_ir_build_const_int(fb, entry, 3, NAUTILUS_IR_TYPE_I32);
	nautilus_ir_build_return(fb, entry, nautilus_ir_build_binary(fb, entry, NAUTILUS_IR_BINARY_MUL, two, three));
	auto* fn = nautilus_ir_function_builder_finish(fb);
	REQUIRE(fn != nullptr);
	REQUIRE(operations(entry).size() == 4);

	NautilusString attribute {nullptr, 0};
	REQUIRE(nautilus_ir_function_get_attribute(fn, str("entry"), &attribute) == NAUTILUS_OK);
	REQUIRE(take(attribute) == "true");
	REQUIRE(nautilus_ir_function_get_attribute(fn, str("nope"), &attribute) == NAUTILUS_ERROR_NOT_FOUND);

	REQUIRE(nautilus_ir_graph_optimize(graph.get(), NAUTILUS_IR_OPTIMIZE_FULL, nullptr) == NAUTILUS_OK);
	const auto optimized = operations(nautilus_ir_function_get_entry_block(fn));
	REQUIRE(optimized.size() == 2);
	int64_t folded = 0;
	REQUIRE(nautilus_ir_value_get_const_int(optimized[0], &folded) == NAUTILUS_OK);
	REQUIRE(folded == 6);

	// An optimized graph is closed for new functions.
	REQUIRE(nautilus_ir_function_builder_create(graph.get(), str("late"), NAUTILUS_IR_TYPE_VOID) == nullptr);
	REQUIRE(nautilus_last_error_code() == NAUTILUS_ERROR_INVALID_STATE);
}

TEST_CASE("C IR API: invalid construction is reported with a status") {
	Graph graph(nautilus_ir_graph_create(str("errors")));
	auto* fb = nautilus_ir_function_builder_create(graph.get(), str("f"), NAUTILUS_IR_TYPE_I64);
	REQUIRE(fb != nullptr);
	REQUIRE(nautilus_ir_function_builder_create(graph.get(), str("f"), NAUTILUS_IR_TYPE_I64) == nullptr);
	REQUIRE(nautilus_last_error_code() == NAUTILUS_ERROR_INVALID_STATE);
	REQUIRE(lastError().find("already exists") != std::string::npos);

	NautilusIRType params[] = {NAUTILUS_IR_TYPE_I64, NAUTILUS_IR_TYPE_BOOL};
	auto* entry = nautilus_ir_function_builder_add_block(fb, params, 2);
	auto* next = nautilus_ir_function_builder_add_block(fb, params, 1);
	auto* i64 = nautilus_ir_block_get_argument(entry, 0);
	auto* flag = nautilus_ir_block_get_argument(entry, 1);

	SECTION("type mismatches") {
		REQUIRE(nautilus_ir_build_const_int(fb, entry, 1, NAUTILUS_IR_TYPE_F64) == nullptr);
		REQUIRE(nautilus_last_error_code() == NAUTILUS_ERROR_TYPE_MISMATCH);
		REQUIRE(nautilus_ir_build_binary(fb, entry, NAUTILUS_IR_BINARY_LOGICAL_AND, i64, flag) == nullptr);
		REQUIRE(nautilus_ir_build_load(fb, entry, i64, NAUTILUS_IR_TYPE_I64) == nullptr);
		REQUIRE(nautilus_ir_build_return(fb, entry, flag) == nullptr);
		REQUIRE(nautilus_last_error_code() == NAUTILUS_ERROR_TYPE_MISMATCH);
		REQUIRE(lastError().find("return type") != std::string::npos);
	}

	SECTION("invalid enumeration values are rejected, not undefined") {
		REQUIRE(nautilus_ir_build_const_int(fb, entry, 1, 1000) == nullptr);
		REQUIRE(nautilus_last_error_code() == NAUTILUS_ERROR_INVALID_ARGUMENT);
		REQUIRE(nautilus_ir_build_binary(fb, entry, 1000, i64, i64) == nullptr);
		REQUIRE(nautilus_last_error_code() == NAUTILUS_ERROR_INVALID_ARGUMENT);
		auto attributes = nautilus_ir_function_attributes_default();
		attributes.flags = 1u << 31;
		NautilusIRCalleeId id = NAUTILUS_IR_INVALID_CALLEE;
		REQUIRE(nautilus_ir_graph_declare_external_function(
		            graph.get(), str("x"), str("x"), reinterpret_cast<void*>(&externalHelper), NAUTILUS_IR_TYPE_I64,
		            nullptr, 0, attributes, &id) == NAUTILUS_ERROR_INVALID_ARGUMENT);
		REQUIRE(id == NAUTILUS_IR_INVALID_CALLEE);
	}

	SECTION("branch arity must match the target") {
		REQUIRE(nautilus_ir_build_branch(fb, entry, next, nullptr, 0) == nullptr);
		REQUIRE(lastError().find("argument count") != std::string::npos);
		NautilusIRValueRef wrongType[] = {flag};
		REQUIRE(nautilus_ir_build_branch(fb, entry, next, wrongType, 1) == nullptr);
		REQUIRE(nautilus_last_error_code() == NAUTILUS_ERROR_TYPE_MISMATCH);
		NautilusIRValueRef ok[] = {i64};
		REQUIRE(nautilus_ir_build_branch(fb, entry, next, ok, 1) != nullptr);
		// Nothing goes after a terminator.
		REQUIRE(nautilus_ir_build_const_int(fb, entry, 1, NAUTILUS_IR_TYPE_I64) == nullptr);
		REQUIRE(nautilus_last_error_code() == NAUTILUS_ERROR_INVALID_STATE);
	}

	SECTION("unfinished blocks fail finish and consume the builder") {
		REQUIRE(nautilus_ir_function_builder_finish(fb) == nullptr);
		REQUIRE(nautilus_last_error_code() == NAUTILUS_ERROR_INVALID_STATE);
		REQUIRE(lastError().find("terminator") != std::string::npos);
		// The graph has no pending builder, and an empty graph compiles.
		Executable executable(nautilus_ir_graph_compile(graph.get(), str("bc"), nullptr));
		INFO(lastError());
		REQUIRE(executable);
	}

	SECTION("a disposed builder no longer blocks compilation") {
		REQUIRE(nautilus_ir_graph_compile(graph.get(), str("bc"), nullptr) == nullptr);
		REQUIRE(nautilus_last_error_code() == NAUTILUS_ERROR_INVALID_STATE);
		nautilus_ir_function_builder_dispose(fb);
		Executable executable(nautilus_ir_graph_compile(graph.get(), str("bc"), nullptr));
		INFO(lastError());
		REQUIRE(executable);
	}

	SECTION("calls to an abandoned function fail verification") {
		NautilusIRCalleeId abandoned = NAUTILUS_IR_INVALID_CALLEE;
		REQUIRE(nautilus_ir_function_builder_get_callee(fb, &abandoned) == NAUTILUS_OK);
		auto* caller = nautilus_ir_function_builder_create(graph.get(), str("caller"), NAUTILUS_IR_TYPE_I64);
		auto* callerEntry = nautilus_ir_function_builder_add_block(caller, nullptr, 0);
		auto* one = nautilus_ir_build_const_int(caller, callerEntry, 1, NAUTILUS_IR_TYPE_I64);
		auto* yes = nautilus_ir_build_const_bool(caller, callerEntry, true);
		NautilusIRValueRef args[] = {one, yes};
		auto* result = nautilus_ir_build_call(caller, callerEntry, abandoned, args, 2);
		REQUIRE(result != nullptr);
		REQUIRE(nautilus_ir_build_return(caller, callerEntry, result) != nullptr);
		REQUIRE(nautilus_ir_function_builder_finish(caller) != nullptr);
		nautilus_ir_function_builder_dispose(fb);
		REQUIRE(nautilus_ir_graph_compile(graph.get(), str("bc"), nullptr) == nullptr);
		REQUIRE(nautilus_last_error_code() == NAUTILUS_ERROR_VERIFICATION_FAILED);
	}

	SECTION("blocks of another builder are rejected") {
		auto* other = nautilus_ir_function_builder_create(graph.get(), str("g"), NAUTILUS_IR_TYPE_VOID);
		REQUIRE(nautilus_ir_build_return(other, entry, nullptr) == nullptr);
		REQUIRE(nautilus_last_error_code() == NAUTILUS_ERROR_INVALID_ARGUMENT);
		REQUIRE(lastError().find("does not belong") != std::string::npos);
	}

	SECTION("unknown backends are reported as unavailable") {
		nautilus_ir_function_builder_dispose(fb);
		REQUIRE(nautilus_ir_graph_compile(graph.get(), str("no-such-backend"), nullptr) == nullptr);
		REQUIRE(nautilus_last_error_code() == NAUTILUS_ERROR_UNAVAILABLE);
	}

	SECTION("missing out-parameters are rejected") {
		REQUIRE(nautilus_ir_function_builder_get_callee(fb, nullptr) == NAUTILUS_ERROR_INVALID_ARGUMENT);
		REQUIRE(nautilus_ir_graph_to_string(graph.get(), nullptr) == NAUTILUS_ERROR_INVALID_ARGUMENT);
	}
}

TEST_CASE("C IR API: options and executables report lookups") {
	Options options(nautilus_options_create());
	REQUIRE(nautilus_options_set_bool(options.get(), str("dump.all"), false) == NAUTILUS_OK);
	REQUIRE(nautilus_options_set_int(options.get(), str("ir.maxPipelineIterations"), 2) == NAUTILUS_OK);
	REQUIRE(nautilus_options_set_string(options.get(), str(""), str("x")) == NAUTILUS_ERROR_INVALID_ARGUMENT);

	auto graph = buildAll();
	Executable executable(nautilus_ir_graph_compile(graph.get(), str("bc"), options.get()));
	REQUIRE(executable);
	void* fn = nullptr;
	REQUIRE(nautilus_executable_get_function(executable.get(), str("missing"), &fn) == NAUTILUS_ERROR_NOT_FOUND);
	REQUIRE(fn == nullptr);
}

#include "nautilus/c_api/nautilus_c_api.h"
#include <catch2/catch_all.hpp>
#include <cstdint>

TEST_CASE("C API IR Builder - Create Module", "[c_api][ir_builder]") {
	auto* module = nautilus_ir_module_create();
	REQUIRE(module != nullptr);
	nautilus_ir_module_destroy(module);
}

TEST_CASE("C API IR Builder - Create Function", "[c_api][ir_builder]") {
	auto* module = nautilus_ir_module_create();
	REQUIRE(module != nullptr);

	auto* function = nautilus_ir_module_create_function(module, "test_function");
	REQUIRE(function != nullptr);

	nautilus_ir_module_destroy(module);
}

TEST_CASE("C API IR Builder - Get Function Body", "[c_api][ir_builder]") {
	auto* module = nautilus_ir_module_create();
	REQUIRE(module != nullptr);

	auto* function = nautilus_ir_module_create_function(module, "test_function");
	REQUIRE(function != nullptr);

	auto* block = nautilus_ir_function_get_body(function);
	REQUIRE(block != nullptr);

	nautilus_ir_block_destroy(block);
	nautilus_ir_module_destroy(module);
}

TEST_CASE("C API IR Builder - Create Constant", "[c_api][ir_builder]") {
	auto* module = nautilus_ir_module_create();
	REQUIRE(module != nullptr);

	auto* function = nautilus_ir_module_create_function(module, "test_function");
	REQUIRE(function != nullptr);

	auto* block = nautilus_ir_function_get_body(function);
	REQUIRE(block != nullptr);

	auto* const_op = nautilus_int_64(42);
	REQUIRE(const_op != nullptr);

	nautilus_ir_operation_destroy(const_op);
	nautilus_ir_block_destroy(block);
	nautilus_ir_module_destroy(module);
}

TEST_CASE("C API IR Builder - Create Add Operation", "[c_api][ir_builder]") {
	auto* module = nautilus_ir_module_create();
	REQUIRE(module != nullptr);

	auto* function = nautilus_ir_module_create_function(module, "test_function");
	REQUIRE(function != nullptr);

	auto* block = nautilus_ir_function_get_body(function);
	REQUIRE(block != nullptr);

	auto* left = nautilus_int_64(10);
	REQUIRE(left != nullptr);

	auto* right = nautilus_int_64(32);
	REQUIRE(right != nullptr);

	auto* add_op = nautilus_add(left, right);
	REQUIRE(add_op != nullptr);

	nautilus_ir_operation_destroy(add_op);
	nautilus_ir_operation_destroy(right);
	nautilus_ir_operation_destroy(left);
	nautilus_ir_block_destroy(block);
	nautilus_ir_module_destroy(module);
}

TEST_CASE("C API IR Builder - Create Return Operation", "[c_api][ir_builder]") {
	auto* module = nautilus_ir_module_create();
	REQUIRE(module != nullptr);

	auto* function = nautilus_ir_module_create_function(module, "test_function");
	REQUIRE(function != nullptr);

	auto* block = nautilus_ir_function_get_body(function);
	REQUIRE(block != nullptr);

	auto* value = nautilus_int_64(42);
	REQUIRE(value != nullptr);

	auto* ret_op = nautilus_return(value);
	REQUIRE(ret_op != nullptr);

	nautilus_ir_operation_destroy(ret_op);
	nautilus_ir_operation_destroy(value);
	nautilus_ir_block_destroy(block);
	nautilus_ir_module_destroy(module);
}

TEST_CASE("C API IR Builder - Build Simple Function", "[c_api][ir_builder]") {
	// Create a function that returns 10 + 32 = 42
	auto* module = nautilus_ir_module_create();
	REQUIRE(module != nullptr);

	auto* function = nautilus_ir_module_create_function(module, "add_function");
	REQUIRE(function != nullptr);

	auto* block = nautilus_ir_function_get_body(function);
	REQUIRE(block != nullptr);

	// Create constants
	auto* left = nautilus_int_64(10);
	REQUIRE(left != nullptr);

	auto* right = nautilus_int_64(32);
	REQUIRE(right != nullptr);

	// Add them
	auto* sum = nautilus_add(left, right);
	REQUIRE(sum != nullptr);

	// Return the result
	auto* ret = nautilus_return(sum);
	REQUIRE(ret != nullptr);

	// Clean up operation wrappers (the actual operations are owned by the function)
	nautilus_ir_operation_destroy(ret);
	nautilus_ir_operation_destroy(sum);
	nautilus_ir_operation_destroy(right);
	nautilus_ir_operation_destroy(left);
	nautilus_ir_block_destroy(block);
	nautilus_ir_module_destroy(module);
}

TEST_CASE("C API JIT - Create and Destroy Engine", "[c_api][jit]") {
	auto* engine = nautilus_jit_create();
	REQUIRE(engine != nullptr);
	nautilus_jit_destroy(engine);
}

TEST_CASE("C API JIT - Compile Simple Function", "[c_api][jit][!mayfail]") {
	// Create a function that returns 10 + 32 = 42
	auto* module = nautilus_ir_module_create();
	REQUIRE(module != nullptr);

	auto* function = nautilus_ir_module_create_function(module, "execute");
	REQUIRE(function != nullptr);

	auto* block = nautilus_ir_function_get_body(function);
	REQUIRE(block != nullptr);

	// Create constants
	auto* left = nautilus_int_64(10);
	auto* right = nautilus_int_64(32);

	// Add them
	auto* sum = nautilus_add(left, right);

	// Return the result
	auto* ret = nautilus_return(sum);

	// Clean up operation wrappers
	nautilus_ir_operation_destroy(ret);
	nautilus_ir_operation_destroy(sum);
	nautilus_ir_operation_destroy(right);
	nautilus_ir_operation_destroy(left);
	nautilus_ir_block_destroy(block);

	// Create JIT engine
	auto* engine = nautilus_jit_create();
	REQUIRE(engine != nullptr);

	// Compile the module
	auto* executable = nautilus_jit_compile(engine, module);
	REQUIRE(executable != nullptr);

	// Get function pointer
	auto* func_ptr = nautilus_executable_get_function_ptr(executable, "execute");
	REQUIRE(func_ptr != nullptr);

	// Call the function
	typedef int64_t (*func_t)();
	auto func = reinterpret_cast<func_t>(func_ptr);
	auto result = func();
	REQUIRE(result == 42);

	// Clean up
	nautilus_executable_destroy(executable);
	nautilus_jit_destroy(engine);
	nautilus_ir_module_destroy(module);
}


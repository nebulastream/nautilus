#include "nautilus/c_api/nautilus_c_api.h"
#include <catch2/catch_all.hpp>

// ---- Constant Creation Tests ----

TEST_CASE("C API IR Builder - Boolean Constants", "[c_api][ir_builder][types]") {
	auto* module = nautilus_ir_module_create();
	REQUIRE(module != nullptr);

	auto* function = nautilus_ir_module_create_function(module, "test_function");
	REQUIRE(function != nullptr);

	auto* block = nautilus_ir_function_get_body(function);
	REQUIRE(block != nullptr);

	auto* true_val = nautilus_bool(true);
	REQUIRE(true_val != nullptr);

	auto* false_val = nautilus_bool(false);
	REQUIRE(false_val != nullptr);

	nautilus_ir_operation_destroy(true_val);
	nautilus_ir_operation_destroy(false_val);
	nautilus_ir_block_destroy(block);
	nautilus_ir_module_destroy(module);
}

TEST_CASE("C API IR Builder - Signed Integer Constants", "[c_api][ir_builder][types]") {
	auto* module = nautilus_ir_module_create();
	REQUIRE(module != nullptr);

	auto* function = nautilus_ir_module_create_function(module, "test_function");
	REQUIRE(function != nullptr);

	auto* block = nautilus_ir_function_get_body(function);
	REQUIRE(block != nullptr);

	auto* i8_val = nautilus_int_8(42);
	REQUIRE(i8_val != nullptr);

	auto* i16_val = nautilus_int_16(1000);
	REQUIRE(i16_val != nullptr);

	auto* i32_val = nautilus_int_32(100000);
	REQUIRE(i32_val != nullptr);

	auto* i64_val = nautilus_int_64(1000000000LL);
	REQUIRE(i64_val != nullptr);

	nautilus_ir_operation_destroy(i8_val);
	nautilus_ir_operation_destroy(i16_val);
	nautilus_ir_operation_destroy(i32_val);
	nautilus_ir_operation_destroy(i64_val);
	nautilus_ir_block_destroy(block);
	nautilus_ir_module_destroy(module);
}

TEST_CASE("C API IR Builder - Unsigned Integer Constants", "[c_api][ir_builder][types]") {
	auto* module = nautilus_ir_module_create();
	REQUIRE(module != nullptr);

	auto* function = nautilus_ir_module_create_function(module, "test_function");
	REQUIRE(function != nullptr);

	auto* block = nautilus_ir_function_get_body(function);
	REQUIRE(block != nullptr);

	auto* ui8_val = nautilus_uint_8(255);
	REQUIRE(ui8_val != nullptr);

	auto* ui16_val = nautilus_uint_16(65535);
	REQUIRE(ui16_val != nullptr);

	auto* ui32_val = nautilus_uint_32(4000000000U);
	REQUIRE(ui32_val != nullptr);

	auto* ui64_val = nautilus_uint_64(10000000000ULL);
	REQUIRE(ui64_val != nullptr);

	nautilus_ir_operation_destroy(ui8_val);
	nautilus_ir_operation_destroy(ui16_val);
	nautilus_ir_operation_destroy(ui32_val);
	nautilus_ir_operation_destroy(ui64_val);
	nautilus_ir_block_destroy(block);
	nautilus_ir_module_destroy(module);
}

TEST_CASE("C API IR Builder - Float Constants", "[c_api][ir_builder][types]") {
	auto* module = nautilus_ir_module_create();
	REQUIRE(module != nullptr);

	auto* function = nautilus_ir_module_create_function(module, "test_function");
	REQUIRE(function != nullptr);

	auto* block = nautilus_ir_function_get_body(function);
	REQUIRE(block != nullptr);

	auto* f32_val = nautilus_float_32(3.14159f);
	REQUIRE(f32_val != nullptr);

	auto* f64_val = nautilus_float_64(2.718281828);
	REQUIRE(f64_val != nullptr);

	nautilus_ir_operation_destroy(f32_val);
	nautilus_ir_operation_destroy(f64_val);
	nautilus_ir_block_destroy(block);
	nautilus_ir_module_destroy(module);
}

// ---- Arithmetic Operations Tests ----

TEST_CASE("C API IR Builder - Subtract Operation", "[c_api][ir_builder][arithmetic]") {
	auto* module = nautilus_ir_module_create();
	auto* function = nautilus_ir_module_create_function(module, "test_function");
	auto* block = nautilus_ir_function_get_body(function);

	auto* left = nautilus_int_32(100);
	auto* right = nautilus_int_32(42);
	auto* sub_op = nautilus_sub(left, right);
	REQUIRE(sub_op != nullptr);

	nautilus_ir_operation_destroy(sub_op);
	nautilus_ir_operation_destroy(right);
	nautilus_ir_operation_destroy(left);
	nautilus_ir_block_destroy(block);
	nautilus_ir_module_destroy(module);
}

TEST_CASE("C API IR Builder - Multiply Operation", "[c_api][ir_builder][arithmetic]") {
	auto* module = nautilus_ir_module_create();
	auto* function = nautilus_ir_module_create_function(module, "test_function");
	auto* block = nautilus_ir_function_get_body(function);

	auto* left = nautilus_int_32(6);
	auto* right = nautilus_int_32(7);
	auto* mul_op = nautilus_mul(left, right);
	REQUIRE(mul_op != nullptr);

	nautilus_ir_operation_destroy(mul_op);
	nautilus_ir_operation_destroy(right);
	nautilus_ir_operation_destroy(left);
	nautilus_ir_block_destroy(block);
	nautilus_ir_module_destroy(module);
}

TEST_CASE("C API IR Builder - Divide Operation", "[c_api][ir_builder][arithmetic]") {
	auto* module = nautilus_ir_module_create();
	auto* function = nautilus_ir_module_create_function(module, "test_function");
	auto* block = nautilus_ir_function_get_body(function);

	auto* left = nautilus_int_32(84);
	auto* right = nautilus_int_32(2);
	auto* div_op = nautilus_div(left, right);
	REQUIRE(div_op != nullptr);

	nautilus_ir_operation_destroy(div_op);
	nautilus_ir_operation_destroy(right);
	nautilus_ir_operation_destroy(left);
	nautilus_ir_block_destroy(block);
	nautilus_ir_module_destroy(module);
}

TEST_CASE("C API IR Builder - Modulo Operation", "[c_api][ir_builder][arithmetic]") {
	auto* module = nautilus_ir_module_create();
	auto* function = nautilus_ir_module_create_function(module, "test_function");
	auto* block = nautilus_ir_function_get_body(function);

	auto* left = nautilus_int_32(47);
	auto* right = nautilus_int_32(5);
	auto* mod_op = nautilus_mod(left, right);
	REQUIRE(mod_op != nullptr);

	nautilus_ir_operation_destroy(mod_op);
	nautilus_ir_operation_destroy(right);
	nautilus_ir_operation_destroy(left);
	nautilus_ir_block_destroy(block);
	nautilus_ir_module_destroy(module);
}

TEST_CASE("C API IR Builder - Negate Operation", "[c_api][ir_builder][arithmetic]") {
	auto* module = nautilus_ir_module_create();
	auto* function = nautilus_ir_module_create_function(module, "test_function");
	auto* block = nautilus_ir_function_get_body(function);

	auto* value = nautilus_int_32(42);
	auto* neg_op = nautilus_negate(value);
	REQUIRE(neg_op != nullptr);

	nautilus_ir_operation_destroy(neg_op);
	nautilus_ir_operation_destroy(value);
	nautilus_ir_block_destroy(block);
	nautilus_ir_module_destroy(module);
}

// ---- Logical Operations Tests ----

TEST_CASE("C API IR Builder - AND Operation", "[c_api][ir_builder][logical]") {
	auto* module = nautilus_ir_module_create();
	auto* function = nautilus_ir_module_create_function(module, "test_function");
	auto* block = nautilus_ir_function_get_body(function);

	auto* left = nautilus_bool(true);
	auto* right = nautilus_bool(false);
	auto* and_op = nautilus_and(left, right);
	REQUIRE(and_op != nullptr);

	nautilus_ir_operation_destroy(and_op);
	nautilus_ir_operation_destroy(right);
	nautilus_ir_operation_destroy(left);
	nautilus_ir_block_destroy(block);
	nautilus_ir_module_destroy(module);
}

TEST_CASE("C API IR Builder - OR Operation", "[c_api][ir_builder][logical]") {
	auto* module = nautilus_ir_module_create();
	auto* function = nautilus_ir_module_create_function(module, "test_function");
	auto* block = nautilus_ir_function_get_body(function);

	auto* left = nautilus_bool(true);
	auto* right = nautilus_bool(false);
	auto* or_op = nautilus_or(left, right);
	REQUIRE(or_op != nullptr);

	nautilus_ir_operation_destroy(or_op);
	nautilus_ir_operation_destroy(right);
	nautilus_ir_operation_destroy(left);
	nautilus_ir_block_destroy(block);
	nautilus_ir_module_destroy(module);
}

TEST_CASE("C API IR Builder - NOT Operation", "[c_api][ir_builder][logical]") {
	auto* module = nautilus_ir_module_create();
	auto* function = nautilus_ir_module_create_function(module, "test_function");
	auto* block = nautilus_ir_function_get_body(function);

	auto* value = nautilus_bool(true);
	auto* not_op = nautilus_not(value);
	REQUIRE(not_op != nullptr);

	nautilus_ir_operation_destroy(not_op);
	nautilus_ir_operation_destroy(value);
	nautilus_ir_block_destroy(block);
	nautilus_ir_module_destroy(module);
}

// ---- Comparison Operations Tests ----

TEST_CASE("C API IR Builder - Equals Operation", "[c_api][ir_builder][comparison]") {
	auto* module = nautilus_ir_module_create();
	auto* function = nautilus_ir_module_create_function(module, "test_function");
	auto* block = nautilus_ir_function_get_body(function);

	auto* left = nautilus_int_32(42);
	auto* right = nautilus_int_32(42);
	auto* eq_op = nautilus_equals(left, right);
	REQUIRE(eq_op != nullptr);

	nautilus_ir_operation_destroy(eq_op);
	nautilus_ir_operation_destroy(right);
	nautilus_ir_operation_destroy(left);
	nautilus_ir_block_destroy(block);
	nautilus_ir_module_destroy(module);
}




TEST_CASE("C API IR Builder - Not Equals Operation", "[c_api][ir_builder][comparison]") {
	auto* module = nautilus_ir_module_create();
	auto* function = nautilus_ir_module_create_function(module, "test_function");
	auto* block = nautilus_ir_function_get_body(function);

	auto* left = nautilus_int_32(42);
	auto* right = nautilus_int_32(10);
	auto* ne_op = nautilus_not_equals(left, right);
	REQUIRE(ne_op != nullptr);

	nautilus_ir_operation_destroy(ne_op);
	nautilus_ir_operation_destroy(right);
	nautilus_ir_operation_destroy(left);
	nautilus_ir_block_destroy(block);
	nautilus_ir_module_destroy(module);
}

TEST_CASE("C API IR Builder - Less Than Operation", "[c_api][ir_builder][comparison]") {
	auto* module = nautilus_ir_module_create();
	auto* function = nautilus_ir_module_create_function(module, "test_function");
	auto* block = nautilus_ir_function_get_body(function);

	auto* left = nautilus_int_32(10);
	auto* right = nautilus_int_32(42);
	auto* lt_op = nautilus_less_than(left, right);
	REQUIRE(lt_op != nullptr);

	nautilus_ir_operation_destroy(lt_op);
	nautilus_ir_operation_destroy(right);
	nautilus_ir_operation_destroy(left);
	nautilus_ir_block_destroy(block);
	nautilus_ir_module_destroy(module);
}

TEST_CASE("C API IR Builder - Less Equal Operation", "[c_api][ir_builder][comparison]") {
	auto* module = nautilus_ir_module_create();
	auto* function = nautilus_ir_module_create_function(module, "test_function");
	auto* block = nautilus_ir_function_get_body(function);

	auto* left = nautilus_int_32(42);
	auto* right = nautilus_int_32(42);
	auto* le_op = nautilus_less_equal(left, right);
	REQUIRE(le_op != nullptr);

	nautilus_ir_operation_destroy(le_op);
	nautilus_ir_operation_destroy(right);
	nautilus_ir_operation_destroy(left);
	nautilus_ir_block_destroy(block);
	nautilus_ir_module_destroy(module);
}

TEST_CASE("C API IR Builder - Greater Than Operation", "[c_api][ir_builder][comparison]") {
	auto* module = nautilus_ir_module_create();
	auto* function = nautilus_ir_module_create_function(module, "test_function");
	auto* block = nautilus_ir_function_get_body(function);

	auto* left = nautilus_int_32(100);
	auto* right = nautilus_int_32(42);
	auto* gt_op = nautilus_greater_than(left, right);
	REQUIRE(gt_op != nullptr);

	nautilus_ir_operation_destroy(gt_op);
	nautilus_ir_operation_destroy(right);
	nautilus_ir_operation_destroy(left);
	nautilus_ir_block_destroy(block);
	nautilus_ir_module_destroy(module);
}

TEST_CASE("C API IR Builder - Greater Equal Operation", "[c_api][ir_builder][comparison]") {
	auto* module = nautilus_ir_module_create();
	auto* function = nautilus_ir_module_create_function(module, "test_function");
	auto* block = nautilus_ir_function_get_body(function);

	auto* left = nautilus_int_32(42);
	auto* right = nautilus_int_32(42);
	auto* ge_op = nautilus_greater_equal(left, right);
	REQUIRE(ge_op != nullptr);

	nautilus_ir_operation_destroy(ge_op);
	nautilus_ir_operation_destroy(right);
	nautilus_ir_operation_destroy(left);
	nautilus_ir_block_destroy(block);
	nautilus_ir_module_destroy(module);
}

// ---- Integration Tests ----

TEST_CASE("C API JIT - Compile Subtraction Function", "[c_api][jit][integration][!mayfail]") {
	// Create a function that returns 100 - 58 = 42
	auto* module = nautilus_ir_module_create();
	auto* function = nautilus_ir_module_create_function(module, "execute");
	auto* block = nautilus_ir_function_get_body(function);

	auto* left = nautilus_int_64(100);
	auto* right = nautilus_int_64(58);
	auto* result = nautilus_sub(left, right);
	auto* ret = nautilus_return(result);

	nautilus_ir_operation_destroy(ret);
	nautilus_ir_operation_destroy(result);
	nautilus_ir_operation_destroy(right);
	nautilus_ir_operation_destroy(left);
	nautilus_ir_block_destroy(block);

	auto* engine = nautilus_jit_create();
	auto* executable = nautilus_jit_compile(engine, module);
	REQUIRE(executable != nullptr);

	auto* func_ptr = nautilus_executable_get_function_ptr(executable, "execute");
	REQUIRE(func_ptr != nullptr);

	typedef int64_t (*func_t)();
	auto func = reinterpret_cast<func_t>(func_ptr);
	auto result_val = func();
	REQUIRE(result_val == 42);

	nautilus_executable_destroy(executable);
	nautilus_jit_destroy(engine);
	nautilus_ir_module_destroy(module);
}

TEST_CASE("C API JIT - Compile Multiplication Function", "[c_api][jit][integration][!mayfail]") {
	// Create a function that returns 6 * 7 = 42
	auto* module = nautilus_ir_module_create();
	auto* function = nautilus_ir_module_create_function(module, "execute");
	auto* block = nautilus_ir_function_get_body(function);

	auto* left = nautilus_int_64(6);
	auto* right = nautilus_int_64(7);
	auto* result = nautilus_mul(left, right);
	auto* ret = nautilus_return(result);

	nautilus_ir_operation_destroy(ret);
	nautilus_ir_operation_destroy(result);
	nautilus_ir_operation_destroy(right);
	nautilus_ir_operation_destroy(left);
	nautilus_ir_block_destroy(block);

	auto* engine = nautilus_jit_create();
	auto* executable = nautilus_jit_compile(engine, module);
	REQUIRE(executable != nullptr);

	auto* func_ptr = nautilus_executable_get_function_ptr(executable, "execute");
	REQUIRE(func_ptr != nullptr);

	typedef int64_t (*func_t)();
	auto func = reinterpret_cast<func_t>(func_ptr);
	auto result_val = func();
	REQUIRE(result_val == 42);

	nautilus_executable_destroy(executable);
	nautilus_jit_destroy(engine);
	nautilus_ir_module_destroy(module);
}

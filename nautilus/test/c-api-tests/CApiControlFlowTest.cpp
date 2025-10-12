#include "nautilus/c_api/nautilus_c_api.h"
#include <catch2/catch_all.hpp>

TEST_CASE("C API Control Flow - Create Block", "[c-api][control-flow]") {
    auto* module = nautilus_ir_module_create();
    REQUIRE(module != nullptr);

    auto* function = nautilus_ir_module_create_function(module, "test_function");
    REQUIRE(function != nullptr);

    auto* entry = nautilus_ir_function_get_body(function);
    REQUIRE(entry != nullptr);

    // Create a new block
    auto* block1 = nautilus_ir_module_create_block();
    REQUIRE(block1 != nullptr);

    // Cleanup
    nautilus_ir_block_destroy(block1);
    nautilus_ir_block_destroy(entry);
    nautilus_ir_module_destroy(module);
}

TEST_CASE("C API Control Flow - Simple If-Then", "[c-api][control-flow]") {
    auto* module = nautilus_ir_module_create();
    REQUIRE(module != nullptr);

    auto* function = nautilus_ir_module_create_function(module, "if_then");
    REQUIRE(function != nullptr);

    auto* entry = nautilus_ir_function_get_body(function);
    REQUIRE(entry != nullptr);

    // Create condition: x < 10
    auto* x = nautilus_int_32(5);
    auto* ten = nautilus_int_32(10);
    auto* condition = nautilus_less_than(x, ten);
    REQUIRE(condition != nullptr);

    // Create then block
    auto* then_block = nautilus_ir_module_create_block();
    REQUIRE(then_block != nullptr);

    // Create merge block
    auto* merge_block = nautilus_ir_module_create_block();
    REQUIRE(merge_block != nullptr);

    // Create if operation
    auto* if_op = nautilus_if(condition, then_block, merge_block);
    REQUIRE(if_op != nullptr);

    // Cleanup
    nautilus_ir_operation_destroy(if_op);
    nautilus_ir_block_destroy(merge_block);
    nautilus_ir_block_destroy(then_block);
    nautilus_ir_operation_destroy(condition);
    nautilus_ir_operation_destroy(ten);
    nautilus_ir_operation_destroy(x);
    nautilus_ir_block_destroy(entry);
    nautilus_ir_module_destroy(module);
}

TEST_CASE("C API Control Flow - If-Then-Else", "[c-api][control-flow]") {
    auto* module = nautilus_ir_module_create();
    REQUIRE(module != nullptr);

    auto* function = nautilus_ir_module_create_function(module, "if_then_else");
    REQUIRE(function != nullptr);

    auto* entry = nautilus_ir_function_get_body(function);
    REQUIRE(entry != nullptr);

    // Create condition: x > y
    auto* x = nautilus_int_32(10);
    auto* y = nautilus_int_32(20);
    auto* condition = nautilus_greater_than(x, y);
    REQUIRE(condition != nullptr);

    // Create then block
    auto* then_block = nautilus_ir_module_create_block();
    REQUIRE(then_block != nullptr);

    // Create else block
    auto* else_block = nautilus_ir_module_create_block();
    REQUIRE(else_block != nullptr);

    // Create if operation with both branches
    auto* if_op = nautilus_if(condition, then_block, else_block);
    REQUIRE(if_op != nullptr);

    // Cleanup
    nautilus_ir_operation_destroy(if_op);
    nautilus_ir_block_destroy(else_block);
    nautilus_ir_block_destroy(then_block);
    nautilus_ir_operation_destroy(condition);
    nautilus_ir_operation_destroy(y);
    nautilus_ir_operation_destroy(x);
    nautilus_ir_block_destroy(entry);
    nautilus_ir_module_destroy(module);
}

TEST_CASE("C API Control Flow - Branch Operation", "[c-api][control-flow]") {
    auto* module = nautilus_ir_module_create();
    REQUIRE(module != nullptr);

    auto* function = nautilus_ir_module_create_function(module, "branch_test");
    REQUIRE(function != nullptr);

    auto* entry = nautilus_ir_function_get_body(function);
    REQUIRE(entry != nullptr);

    // Create target block
    auto* target = nautilus_ir_module_create_block();
    REQUIRE(target != nullptr);

    // Create branch operation
    auto* branch_op = nautilus_branch(target);
    REQUIRE(branch_op != nullptr);

    // Cleanup
    nautilus_ir_operation_destroy(branch_op);
    nautilus_ir_block_destroy(target);
    nautilus_ir_block_destroy(entry);
    nautilus_ir_module_destroy(module);
}

TEST_CASE("C API Control Flow - Max Function with If-Else", "[c-api][control-flow]") {
    // This test creates: int max(int a, int b) { return (a > b) ? a : b; }
    // Using explicit blocks and control flow

    auto* module = nautilus_ir_module_create();
    REQUIRE(module != nullptr);

    auto* function = nautilus_ir_module_create_function(module, "max");
    REQUIRE(function != nullptr);

    auto* entry = nautilus_ir_function_get_body(function);
    REQUIRE(entry != nullptr);

    // Create values
    auto* a = nautilus_int_32(10);
    auto* b = nautilus_int_32(20);

    // Create condition: a > b
    auto* condition = nautilus_greater_than(a, b);
    REQUIRE(condition != nullptr);

    // Create then block (return a)
    auto* then_block = nautilus_ir_module_create_block();
    REQUIRE(then_block != nullptr);

    // Create else block (return b)
    auto* else_block = nautilus_ir_module_create_block();
    REQUIRE(else_block != nullptr);

    // Create merge block
    auto* merge_block = nautilus_ir_module_create_block();
    REQUIRE(merge_block != nullptr);

    // Create if operation in entry block
    auto* if_op = nautilus_if(condition, then_block, else_block);
    REQUIRE(if_op != nullptr);

    // Note: In a real implementation, we would need to:
    // 1. Add branch operations from then_block and else_block to merge_block
    // 2. Add phi node in merge_block to select between a and b
    // 3. Add return operation in merge_block
    // This requires a way to set the "current block" for adding operations

    // Cleanup
    nautilus_ir_operation_destroy(if_op);
    nautilus_ir_block_destroy(merge_block);
    nautilus_ir_block_destroy(else_block);
    nautilus_ir_block_destroy(then_block);
    nautilus_ir_operation_destroy(condition);
    nautilus_ir_operation_destroy(b);
    nautilus_ir_operation_destroy(a);
    nautilus_ir_block_destroy(entry);
    nautilus_ir_module_destroy(module);
}

TEST_CASE("C API Control Flow - Simple Loop Structure", "[c-api][control-flow]") {
    // This test creates a simple loop structure:
    // entry -> header -> body -> header (loop back)
    //            |
    //            v
    //          exit

    auto* module = nautilus_ir_module_create();
    REQUIRE(module != nullptr);

    auto* function = nautilus_ir_module_create_function(module, "simple_loop");
    REQUIRE(function != nullptr);

    auto* entry = nautilus_ir_function_get_body(function);
    REQUIRE(entry != nullptr);

    // Create blocks
    auto* header = nautilus_ir_module_create_block();
    REQUIRE(header != nullptr);

    auto* body = nautilus_ir_module_create_block();
    REQUIRE(body != nullptr);

    auto* exit_block = nautilus_ir_module_create_block();
    REQUIRE(exit_block != nullptr);

    // Entry branches to header
    auto* entry_branch = nautilus_branch(header);
    REQUIRE(entry_branch != nullptr);

    // Header has condition: i < 10
    auto* i = nautilus_int_32(0);
    auto* ten = nautilus_int_32(10);
    auto* condition = nautilus_less_than(i, ten);
    REQUIRE(condition != nullptr);

    // If condition true, go to body, else go to exit
    auto* if_op = nautilus_if(condition, body, exit_block);
    REQUIRE(if_op != nullptr);

    // Body branches back to header
    auto* body_branch = nautilus_branch(header);
    REQUIRE(body_branch != nullptr);

    // Exit returns
    auto* zero = nautilus_int_32(0);
    auto* ret = nautilus_return(zero);
    REQUIRE(ret != nullptr);

    // Cleanup
    nautilus_ir_operation_destroy(ret);
    nautilus_ir_operation_destroy(zero);
    nautilus_ir_operation_destroy(body_branch);
    nautilus_ir_operation_destroy(if_op);
    nautilus_ir_operation_destroy(condition);
    nautilus_ir_operation_destroy(ten);
    nautilus_ir_operation_destroy(i);
    nautilus_ir_operation_destroy(entry_branch);
    nautilus_ir_block_destroy(exit_block);
    nautilus_ir_block_destroy(body);
    nautilus_ir_block_destroy(header);
    nautilus_ir_block_destroy(entry);
    nautilus_ir_module_destroy(module);
}

TEST_CASE("C API Control Flow - Nested If", "[c-api][control-flow]") {
    // This test creates nested if statements

    auto* module = nautilus_ir_module_create();
    REQUIRE(module != nullptr);

    auto* function = nautilus_ir_module_create_function(module, "nested_if");
    REQUIRE(function != nullptr);

    auto* entry = nautilus_ir_function_get_body(function);
    REQUIRE(entry != nullptr);

    // Outer if condition
    auto* x = nautilus_int_32(5);
    auto* ten = nautilus_int_32(10);
    auto* outer_cond = nautilus_less_than(x, ten);
    REQUIRE(outer_cond != nullptr);

    // Outer then block
    auto* outer_then = nautilus_ir_module_create_block();
    REQUIRE(outer_then != nullptr);

    // Outer else block
    auto* outer_else = nautilus_ir_module_create_block();
    REQUIRE(outer_else != nullptr);

    // Outer merge block
    auto* outer_merge = nautilus_ir_module_create_block();
    REQUIRE(outer_merge != nullptr);

    // Create outer if
    auto* outer_if = nautilus_if(outer_cond, outer_then, outer_else);
    REQUIRE(outer_if != nullptr);

    // Inner if condition (in outer_then block)
    auto* y = nautilus_int_32(3);
    auto* five = nautilus_int_32(5);
    auto* inner_cond = nautilus_greater_than(y, five);
    REQUIRE(inner_cond != nullptr);

    // Inner then/else blocks
    auto* inner_then = nautilus_ir_module_create_block();
    REQUIRE(inner_then != nullptr);

    auto* inner_else = nautilus_ir_module_create_block();
    REQUIRE(inner_else != nullptr);

    // Create inner if
    auto* inner_if = nautilus_if(inner_cond, inner_then, inner_else);
    REQUIRE(inner_if != nullptr);

    // Cleanup
    nautilus_ir_operation_destroy(inner_if);
    nautilus_ir_block_destroy(inner_else);
    nautilus_ir_block_destroy(inner_then);
    nautilus_ir_operation_destroy(inner_cond);
    nautilus_ir_operation_destroy(five);
    nautilus_ir_operation_destroy(y);
    nautilus_ir_operation_destroy(outer_if);
    nautilus_ir_block_destroy(outer_merge);
    nautilus_ir_block_destroy(outer_else);
    nautilus_ir_block_destroy(outer_then);
    nautilus_ir_operation_destroy(outer_cond);
    nautilus_ir_operation_destroy(ten);
    nautilus_ir_operation_destroy(x);
    nautilus_ir_block_destroy(entry);
    nautilus_ir_module_destroy(module);
}

TEST_CASE("C API Control Flow - Set and Get Current Block", "[c-api][control-flow]") {
    auto* module = nautilus_ir_module_create();
    REQUIRE(module != nullptr);

    auto* function = nautilus_ir_module_create_function(module, "test_current_block");
    REQUIRE(function != nullptr);

    auto* entry = nautilus_ir_function_get_body(function);
    REQUIRE(entry != nullptr);

    // Create a new block
    auto* block1 = nautilus_ir_module_create_block();
    REQUIRE(block1 != nullptr);

    // Set it as current
    nautilus_set_current_block(block1);

    // Get current block (note: this creates a new wrapper)
    auto* current = nautilus_get_current_block();
    REQUIRE(current != nullptr);

    // Cleanup
    nautilus_ir_block_destroy(current);
    nautilus_ir_block_destroy(block1);
    nautilus_ir_block_destroy(entry);
    nautilus_ir_module_destroy(module);
}

TEST_CASE("C API Control Flow JIT - Simple Branch", "[c-api][control-flow][jit]") {
    // This test creates: int test() { goto target; target: return 42; }

    auto* module = nautilus_ir_module_create();
    REQUIRE(module != nullptr);

    auto* function = nautilus_ir_module_create_function(module, "execute");
    REQUIRE(function != nullptr);

    auto* entry = nautilus_ir_function_get_body(function);
    REQUIRE(entry != nullptr);

    // Create target block
    auto* target = nautilus_ir_module_create_block();
    REQUIRE(target != nullptr);

    // In entry: branch to target
    auto* branch_op = nautilus_branch(target);
    REQUIRE(branch_op != nullptr);

    // In target: return 42
    nautilus_set_current_block(target);
    auto* value = nautilus_int_32(42);
    auto* ret = nautilus_return(value);
    REQUIRE(ret != nullptr);

    // Compile
    auto* engine = nautilus_jit_create();
    REQUIRE(engine != nullptr);

    auto* executable = nautilus_jit_compile(engine, module);
    REQUIRE(executable != nullptr);

    // Execute
    void* func_ptr = nautilus_executable_get_function_ptr(executable, "execute");
    REQUIRE(func_ptr != nullptr);

    typedef int32_t (*test_fn)();
    test_fn test_func = (test_fn)func_ptr;
    int32_t result = test_func();

    // Should return 42
    REQUIRE(result == 42);

    // Cleanup
    nautilus_executable_destroy(executable);
    nautilus_jit_destroy(engine);
    nautilus_ir_operation_destroy(ret);
    nautilus_ir_operation_destroy(value);
    nautilus_ir_operation_destroy(branch_op);
    nautilus_ir_block_destroy(target);
    nautilus_ir_block_destroy(entry);
    nautilus_ir_module_destroy(module);
}

TEST_CASE("C API Control Flow JIT - Conditional Return", "[c-api][control-flow][jit]") {
    // This test creates: int test() { if (5 < 10) return 1; else return 2; }

    auto* module = nautilus_ir_module_create();
    REQUIRE(module != nullptr);

    auto* function = nautilus_ir_module_create_function(module, "conditional_return");
    REQUIRE(function != nullptr);

    auto* entry = nautilus_ir_function_get_body(function);
    REQUIRE(entry != nullptr);

    // Create condition: 5 < 10 (always true)
    auto* five = nautilus_int_32(5);
    auto* ten = nautilus_int_32(10);
    auto* condition = nautilus_less_than(five, ten);
    REQUIRE(condition != nullptr);

    // Create blocks
    auto* then_block = nautilus_ir_module_create_block();
    REQUIRE(then_block != nullptr);

    auto* else_block = nautilus_ir_module_create_block();
    REQUIRE(else_block != nullptr);

    // Create if operation
    auto* if_op = nautilus_if(condition, then_block, else_block);
    REQUIRE(if_op != nullptr);

    // Then block: return 1
    nautilus_set_current_block(then_block);
    auto* one = nautilus_int_32(1);
    auto* ret_then = nautilus_return(one);
    REQUIRE(ret_then != nullptr);

    // Else block: return 2
    nautilus_set_current_block(else_block);
    auto* two = nautilus_int_32(2);
    auto* ret_else = nautilus_return(two);
    REQUIRE(ret_else != nullptr);

    // Compile
    auto* engine = nautilus_jit_create();
    REQUIRE(engine != nullptr);

    auto* executable = nautilus_jit_compile(engine, module);
    REQUIRE(executable != nullptr);

    // Execute
    void* func_ptr = nautilus_executable_get_function_ptr(executable, "conditional_return");
    REQUIRE(func_ptr != nullptr);

    typedef int32_t (*test_fn)();
    test_fn test_func = (test_fn)func_ptr;
    int32_t result = test_func();

    // Should return 1 (condition is true)
    REQUIRE(result == 1);

    // Cleanup
    nautilus_executable_destroy(executable);
    nautilus_jit_destroy(engine);
    nautilus_ir_operation_destroy(ret_else);
    nautilus_ir_operation_destroy(two);
    nautilus_ir_operation_destroy(ret_then);
    nautilus_ir_operation_destroy(one);
    nautilus_ir_operation_destroy(if_op);
    nautilus_ir_block_destroy(else_block);
    nautilus_ir_block_destroy(then_block);
    nautilus_ir_operation_destroy(condition);
    nautilus_ir_operation_destroy(ten);
    nautilus_ir_operation_destroy(five);
    nautilus_ir_block_destroy(entry);
    nautilus_ir_module_destroy(module);
}


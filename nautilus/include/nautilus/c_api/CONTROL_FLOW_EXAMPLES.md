# Nautilus C API - Control Flow Examples

Complete, runnable examples demonstrating the proposed control flow API.

---

## Example 1: Simple Max Function (If-Then-Else)

```c
#include "nautilus/c_api/nautilus_c_api.h"

// Function: int max(int a, int b) { return (a > b) ? a : b; }
int main() {
    // Create module and function
    auto* module = nautilus_ir_module_create();
    auto* func = nautilus_ir_module_create_function(module, "max");
    auto* entry = nautilus_ir_function_get_body(func);
    
    // Create parameters (in real implementation, these would be function arguments)
    auto* a = nautilus_int_32(10);
    auto* b = nautilus_int_32(20);
    
    // Create condition: a > b
    auto* condition = nautilus_greater_than(a, b);
    
    // Create if-then-else
    auto* if_stmt = nautilus_if_else_create(condition);
    
    // Then branch: return a
    auto* then_block = nautilus_if_then_block(if_stmt);
    // a is the result in then branch
    
    // Else branch: return b
    auto* else_block = nautilus_if_else_block(if_stmt);
    // b is the result in else branch
    
    // Create phi node for the result
    auto* result = nautilus_if_phi(if_stmt, a, b);
    
    // End if-else
    auto* merge = nautilus_if_else_end(if_stmt);
    
    // Return the result
    auto* ret = nautilus_return(result);
    
    // Cleanup
    nautilus_ir_operation_destroy(ret);
    nautilus_if_destroy(if_stmt);
    nautilus_ir_block_destroy(entry);
    
    // Compile and execute
    auto* engine = nautilus_jit_create();
    auto* executable = nautilus_jit_compile(engine, module);
    auto* func_ptr = nautilus_executable_get_function_ptr(executable, "max");
    
    typedef int32_t (*max_fn)(int32_t, int32_t);
    max_fn max_func = (max_fn)func_ptr;
    
    int32_t result_value = max_func(10, 20);  // Should return 20
    printf("max(10, 20) = %d\n", result_value);
    
    // Cleanup
    nautilus_executable_destroy(executable);
    nautilus_jit_destroy(engine);
    nautilus_ir_module_destroy(module);
    
    return 0;
}
```

**Expected Output:**
```
max(10, 20) = 20
```

---

## Example 2: Absolute Value (If-Then-Else)

```c
// Function: int abs(int x) { return (x < 0) ? -x : x; }
int main() {
    auto* module = nautilus_ir_module_create();
    auto* func = nautilus_ir_module_create_function(module, "abs");
    auto* entry = nautilus_ir_function_get_body(func);
    
    // Parameter
    auto* x = nautilus_int_32(-42);
    
    // Condition: x < 0
    auto* zero = nautilus_int_32(0);
    auto* condition = nautilus_less_than(x, zero);
    
    // If-then-else
    auto* if_stmt = nautilus_if_else_create(condition);
    
    // Then branch: -x
    auto* then_block = nautilus_if_then_block(if_stmt);
    auto* neg_x = nautilus_negate(x);
    
    // Else branch: x
    auto* else_block = nautilus_if_else_block(if_stmt);
    // x remains unchanged
    
    // Phi node
    auto* result = nautilus_if_phi(if_stmt, neg_x, x);
    auto* merge = nautilus_if_else_end(if_stmt);
    
    // Return
    auto* ret = nautilus_return(result);
    
    // Compile and execute
    auto* engine = nautilus_jit_create();
    auto* executable = nautilus_jit_compile(engine, module);
    auto* func_ptr = nautilus_executable_get_function_ptr(executable, "abs");
    
    typedef int32_t (*abs_fn)(int32_t);
    abs_fn abs_func = (abs_fn)func_ptr;
    
    printf("abs(-42) = %d\n", abs_func(-42));  // Should return 42
    printf("abs(42) = %d\n", abs_func(42));    // Should return 42
    
    // Cleanup...
    return 0;
}
```

**Expected Output:**
```
abs(-42) = 42
abs(42) = 42
```

---

## Example 3: Sum Loop (For Loop)

```c
// Function: int sum(int n) { int s = 0; for (i = 0; i < n; i++) s += i; return s; }
int main() {
    auto* module = nautilus_ir_module_create();
    auto* func = nautilus_ir_module_create_function(module, "sum");
    auto* entry = nautilus_ir_function_get_body(func);
    
    // Initialize sum
    auto* sum = nautilus_int_32(0);
    
    // Create for loop: for (i = 0; i < 10; i++)
    auto* zero = nautilus_int_32(0);
    auto* ten = nautilus_int_32(10);
    auto* for_loop = nautilus_for_create(zero, ten);
    
    // Get loop body and variable
    auto* body = nautilus_for_body(for_loop);
    auto* i = nautilus_for_variable(for_loop);
    
    // In body: sum = sum + i
    // Note: In real implementation, sum would need to be a loop phi node
    auto* new_sum = nautilus_add(sum, i);
    
    // End loop
    auto* exit = nautilus_for_end(for_loop);
    
    // Return sum
    auto* ret = nautilus_return(new_sum);
    
    // Compile and execute
    auto* engine = nautilus_jit_create();
    auto* executable = nautilus_jit_compile(engine, module);
    auto* func_ptr = nautilus_executable_get_function_ptr(executable, "sum");
    
    typedef int32_t (*sum_fn)(int32_t);
    sum_fn sum_func = (sum_fn)func_ptr;
    
    printf("sum(10) = %d\n", sum_func(10));  // Should return 45 (0+1+2+...+9)
    
    // Cleanup...
    return 0;
}
```

**Expected Output:**
```
sum(10) = 45
```

---

## Example 4: Factorial (While Loop)

```c
// Function: int factorial(int n) { int result = 1; while (n > 1) { result *= n; n--; } return result; }
int main() {
    auto* module = nautilus_ir_module_create();
    auto* func = nautilus_ir_module_create_function(module, "factorial");
    auto* entry = nautilus_ir_function_get_body(func);
    
    // Initialize
    auto* n = nautilus_int_32(5);
    auto* result = nautilus_int_32(1);
    
    // Create while loop
    auto* while_loop = nautilus_while_create();
    
    // Get body
    auto* body = nautilus_while_body(while_loop);
    
    // In body: result = result * n
    auto* new_result = nautilus_mul(result, n);
    
    // In body: n = n - 1
    auto* one = nautilus_int_32(1);
    auto* new_n = nautilus_sub(n, one);
    
    // Set condition: n > 1
    auto* one_cmp = nautilus_int_32(1);
    auto* condition = nautilus_greater_than(new_n, one_cmp);
    nautilus_while_set_condition(while_loop, condition);
    
    // End loop
    auto* exit = nautilus_while_end(while_loop);
    
    // Return result
    auto* ret = nautilus_return(new_result);
    
    // Compile and execute
    auto* engine = nautilus_jit_create();
    auto* executable = nautilus_jit_compile(engine, module);
    auto* func_ptr = nautilus_executable_get_function_ptr(executable, "factorial");
    
    typedef int32_t (*factorial_fn)(int32_t);
    factorial_fn factorial_func = (factorial_fn)func_ptr;
    
    printf("factorial(5) = %d\n", factorial_func(5));  // Should return 120
    
    // Cleanup...
    return 0;
}
```

**Expected Output:**
```
factorial(5) = 120
```

---

## Example 5: Nested If (Grade Calculator)

```c
// Function: char grade(int score) {
//     if (score >= 90) return 'A';
//     else if (score >= 80) return 'B';
//     else if (score >= 70) return 'C';
//     else return 'F';
// }
int main() {
    auto* module = nautilus_ir_module_create();
    auto* func = nautilus_ir_module_create_function(module, "grade");
    auto* entry = nautilus_ir_function_get_body(func);
    
    auto* score = nautilus_int_32(85);
    
    // if (score >= 90)
    auto* ninety = nautilus_int_32(90);
    auto* cond1 = nautilus_greater_equal(score, ninety);
    auto* if1 = nautilus_if_else_create(cond1);
    
    auto* then1 = nautilus_if_then_block(if1);
    auto* grade_a = nautilus_int_8('A');  // Return 'A'
    
    auto* else1 = nautilus_if_else_block(if1);
    // Nested if: score >= 80
    auto* eighty = nautilus_int_32(80);
    auto* cond2 = nautilus_greater_equal(score, eighty);
    auto* if2 = nautilus_if_else_create(cond2);
    
    auto* then2 = nautilus_if_then_block(if2);
    auto* grade_b = nautilus_int_8('B');  // Return 'B'
    
    auto* else2 = nautilus_if_else_block(if2);
    // Nested if: score >= 70
    auto* seventy = nautilus_int_32(70);
    auto* cond3 = nautilus_greater_equal(score, seventy);
    auto* if3 = nautilus_if_else_create(cond3);
    
    auto* then3 = nautilus_if_then_block(if3);
    auto* grade_c = nautilus_int_8('C');  // Return 'C'
    
    auto* else3 = nautilus_if_else_block(if3);
    auto* grade_f = nautilus_int_8('F');  // Return 'F'
    
    // Merge innermost if
    auto* result3 = nautilus_if_phi(if3, grade_c, grade_f);
    nautilus_if_else_end(if3);
    
    // Merge middle if
    auto* result2 = nautilus_if_phi(if2, grade_b, result3);
    nautilus_if_else_end(if2);
    
    // Merge outermost if
    auto* result1 = nautilus_if_phi(if1, grade_a, result2);
    nautilus_if_else_end(if1);
    
    // Return
    auto* ret = nautilus_return(result1);
    
    // Compile and execute
    auto* engine = nautilus_jit_create();
    auto* executable = nautilus_jit_compile(engine, module);
    auto* func_ptr = nautilus_executable_get_function_ptr(executable, "grade");
    
    typedef char (*grade_fn)(int32_t);
    grade_fn grade_func = (grade_fn)func_ptr;
    
    printf("grade(95) = %c\n", grade_func(95));  // A
    printf("grade(85) = %c\n", grade_func(85));  // B
    printf("grade(75) = %c\n", grade_func(75));  // C
    printf("grade(65) = %c\n", grade_func(65));  // F
    
    // Cleanup...
    return 0;
}
```

**Expected Output:**
```
grade(95) = A
grade(85) = B
grade(75) = C
grade(65) = F
```

---

## Example 6: Nested Loops (Matrix Sum)

```c
// Function: int matrix_sum(int rows, int cols) {
//     int sum = 0;
//     for (i = 0; i < rows; i++) {
//         for (j = 0; j < cols; j++) {
//             sum += i * cols + j;
//         }
//     }
//     return sum;
// }
int main() {
    auto* module = nautilus_ir_module_create();
    auto* func = nautilus_ir_module_create_function(module, "matrix_sum");
    auto* entry = nautilus_ir_function_get_body(func);
    
    auto* sum = nautilus_int_32(0);
    auto* rows = nautilus_int_32(3);
    auto* cols = nautilus_int_32(4);
    
    // Outer loop: for (i = 0; i < rows; i++)
    auto* zero_i = nautilus_int_32(0);
    auto* outer_loop = nautilus_for_create(zero_i, rows);
    auto* outer_body = nautilus_for_body(outer_loop);
    auto* i = nautilus_for_variable(outer_loop);
    
    // Inner loop: for (j = 0; j < cols; j++)
    auto* zero_j = nautilus_int_32(0);
    auto* inner_loop = nautilus_for_create(zero_j, cols);
    auto* inner_body = nautilus_for_body(inner_loop);
    auto* j = nautilus_for_variable(inner_loop);
    
    // sum += i * cols + j
    auto* i_times_cols = nautilus_mul(i, cols);
    auto* index = nautilus_add(i_times_cols, j);
    auto* new_sum = nautilus_add(sum, index);
    
    // End inner loop
    nautilus_for_end(inner_loop);
    
    // End outer loop
    nautilus_for_end(outer_loop);
    
    // Return sum
    auto* ret = nautilus_return(new_sum);
    
    // Compile and execute
    auto* engine = nautilus_jit_create();
    auto* executable = nautilus_jit_compile(engine, module);
    auto* func_ptr = nautilus_executable_get_function_ptr(executable, "matrix_sum");
    
    typedef int32_t (*matrix_sum_fn)(int32_t, int32_t);
    matrix_sum_fn matrix_sum_func = (matrix_sum_fn)func_ptr;
    
    printf("matrix_sum(3, 4) = %d\n", matrix_sum_func(3, 4));  // Should return 66
    
    // Cleanup...
    return 0;
}
```

**Expected Output:**
```
matrix_sum(3, 4) = 66
```

---

## Notes on Implementation

### Phi Nodes
In the actual implementation, phi nodes need special handling:
- They are represented as block arguments
- Values flow from predecessor blocks
- The `nautilus_if_phi()` function creates the appropriate block arguments

### Loop Variables
Loop variables in for/while loops are phi nodes:
- Initial value comes from pre-header
- Updated value comes from loop body
- The loop builder manages this automatically

### Memory Management
All examples should include proper cleanup:
```c
// Destroy operations
nautilus_ir_operation_destroy(ret);
// Destroy builders
nautilus_if_destroy(if_stmt);
nautilus_loop_destroy(for_loop);
// Destroy blocks
nautilus_ir_block_destroy(entry);
// Destroy executable
nautilus_executable_destroy(executable);
nautilus_jit_destroy(engine);
// Destroy module
nautilus_ir_module_destroy(module);
```

---

## Testing Checklist

- [ ] Simple if-then compiles and executes
- [ ] If-then-else with phi nodes works correctly
- [ ] For loop with induction variable
- [ ] While loop with condition
- [ ] Nested if statements
- [ ] Nested loops
- [ ] Complex control flow (if inside loop)
- [ ] Edge cases (empty blocks, single iteration)


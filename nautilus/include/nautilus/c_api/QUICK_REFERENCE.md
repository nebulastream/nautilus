# Nautilus C API - Quick Reference Guide

## Table of Contents
1. [Module & Function Management](#module--function-management)
2. [Constant Operations](#constant-operations)
3. [Arithmetic Operations](#arithmetic-operations)
4. [Logical Operations](#logical-operations)
5. [Comparison Operations](#comparison-operations)
6. [Control Flow](#control-flow)
7. [JIT Compilation](#jit-compilation)
8. [Memory Management](#memory-management)

---

## Module & Function Management

```c
// Create a module
nautilus_ir_module* module = nautilus_ir_module_create();

// Create a function
nautilus_ir_function* func = nautilus_ir_module_create_function(module, "my_function");

// Get function body (entry block)
nautilus_ir_block* block = nautilus_ir_function_get_body(func);

// Cleanup
nautilus_ir_block_destroy(block);
nautilus_ir_module_destroy(module);
```

---

## Constant Operations

### Boolean
```c
nautilus_ir_operation* t = nautilus_bool(true);
nautilus_ir_operation* f = nautilus_bool(false);
```

### Signed Integers
```c
nautilus_ir_operation* i8  = nautilus_int_8(127);
nautilus_ir_operation* i16 = nautilus_int_16(32767);
nautilus_ir_operation* i32 = nautilus_int_32(2147483647);
nautilus_ir_operation* i64 = nautilus_int_64(9223372036854775807LL);
```

### Unsigned Integers
```c
nautilus_ir_operation* ui8  = nautilus_uint_8(255);
nautilus_ir_operation* ui16 = nautilus_uint_16(65535);
nautilus_ir_operation* ui32 = nautilus_uint_32(4294967295U);
nautilus_ir_operation* ui64 = nautilus_uint_64(18446744073709551615ULL);
```

### Floating-Point
```c
nautilus_ir_operation* f32 = nautilus_float_32(3.14159f);
nautilus_ir_operation* f64 = nautilus_float_64(2.718281828);
```

---

## Arithmetic Operations

```c
nautilus_ir_operation* a = nautilus_int_64(10);
nautilus_ir_operation* b = nautilus_int_64(20);

// Binary operations
nautilus_ir_operation* sum  = nautilus_add(a, b);  // a + b
nautilus_ir_operation* diff = nautilus_sub(a, b);  // a - b
nautilus_ir_operation* prod = nautilus_mul(a, b);  // a * b
nautilus_ir_operation* quot = nautilus_div(a, b);  // a / b
nautilus_ir_operation* rem  = nautilus_mod(a, b);  // a % b

// Unary operation
nautilus_ir_operation* neg  = nautilus_negate(a);  // -a
```

---

## Logical Operations

```c
nautilus_ir_operation* x = nautilus_bool(true);
nautilus_ir_operation* y = nautilus_bool(false);

// Binary operations
nautilus_ir_operation* and_result = nautilus_and(x, y);  // x && y
nautilus_ir_operation* or_result  = nautilus_or(x, y);   // x || y

// Unary operation
nautilus_ir_operation* not_result = nautilus_not(x);     // !x
```

---

## Comparison Operations

```c
nautilus_ir_operation* a = nautilus_int_32(10);
nautilus_ir_operation* b = nautilus_int_32(20);

nautilus_ir_operation* eq = nautilus_equals(a, b);         // a == b
nautilus_ir_operation* ne = nautilus_not_equals(a, b);     // a != b
nautilus_ir_operation* lt = nautilus_less_than(a, b);      // a < b
nautilus_ir_operation* le = nautilus_less_equal(a, b);     // a <= b
nautilus_ir_operation* gt = nautilus_greater_than(a, b);   // a > b
nautilus_ir_operation* ge = nautilus_greater_equal(a, b);  // a >= b
```

---

## Control Flow

```c
// Return from function
nautilus_ir_operation* result = nautilus_int_64(42);
nautilus_ir_operation* ret = nautilus_return(result);
```

---

## JIT Compilation

```c
// Create JIT engine
nautilus_engine* engine = nautilus_jit_create();

// Compile module
nautilus_executable* executable = nautilus_jit_compile(engine, module);

// Get function pointer
void* func_ptr = nautilus_executable_get_function_ptr(executable, "my_function");

// Cast and call
typedef int64_t (*my_func_t)();
my_func_t func = (my_func_t)func_ptr;
int64_t result = func();

// Cleanup
nautilus_executable_destroy(executable);
nautilus_jit_destroy(engine);
```

---

## Memory Management

### Operation Lifecycle
```c
// Create operation
nautilus_ir_operation* op = nautilus_int_64(42);

// Use operation
nautilus_ir_operation* result = nautilus_add(op, op);

// Destroy wrapper (actual operation is owned by function)
nautilus_ir_operation_destroy(op);
nautilus_ir_operation_destroy(result);
```

### Cleanup Order
```c
// 1. Destroy operations (wrappers)
nautilus_ir_operation_destroy(ret);
nautilus_ir_operation_destroy(result);

// 2. Destroy block
nautilus_ir_block_destroy(block);

// 3. Destroy executable (if compiled)
nautilus_executable_destroy(executable);

// 4. Destroy JIT engine
nautilus_jit_destroy(engine);

// 5. Destroy module (this frees actual operations)
nautilus_ir_module_destroy(module);
```

---

## Complete Example

```c
#include "nautilus/c_api/nautilus_c_api.h"

int main() {
    // Create module and function
    nautilus_ir_module* module = nautilus_ir_module_create();
    nautilus_ir_function* func = nautilus_ir_module_create_function(module, "compute");
    nautilus_ir_block* block = nautilus_ir_function_get_body(func);
    
    // Build expression: (10 + 5) * 3 - 3 = 42
    nautilus_ir_operation* a = nautilus_int_64(10);
    nautilus_ir_operation* b = nautilus_int_64(5);
    nautilus_ir_operation* sum = nautilus_add(a, b);
    
    nautilus_ir_operation* c = nautilus_int_64(3);
    nautilus_ir_operation* product = nautilus_mul(sum, c);
    
    nautilus_ir_operation* d = nautilus_int_64(3);
    nautilus_ir_operation* result = nautilus_sub(product, d);
    
    nautilus_ir_operation* ret = nautilus_return(result);
    
    // Cleanup operations
    nautilus_ir_operation_destroy(ret);
    nautilus_ir_operation_destroy(result);
    nautilus_ir_operation_destroy(d);
    nautilus_ir_operation_destroy(product);
    nautilus_ir_operation_destroy(c);
    nautilus_ir_operation_destroy(sum);
    nautilus_ir_operation_destroy(b);
    nautilus_ir_operation_destroy(a);
    nautilus_ir_block_destroy(block);
    
    // Compile and execute
    nautilus_engine* engine = nautilus_jit_create();
    nautilus_executable* executable = nautilus_jit_compile(engine, module);
    
    void* func_ptr = nautilus_executable_get_function_ptr(executable, "compute");
    typedef int64_t (*compute_t)();
    compute_t compute = (compute_t)func_ptr;
    
    int64_t value = compute();  // Returns 42
    
    // Final cleanup
    nautilus_executable_destroy(executable);
    nautilus_jit_destroy(engine);
    nautilus_ir_module_destroy(module);
    
    return 0;
}
```

---

## Type Reference

| Nautilus Type | C Type | Function | Range |
|---------------|--------|----------|-------|
| `b` | `bool` | `nautilus_bool()` | true/false |
| `i8` | `int8_t` | `nautilus_int_8()` | -128 to 127 |
| `i16` | `int16_t` | `nautilus_int_16()` | -32,768 to 32,767 |
| `i32` | `int32_t` | `nautilus_int_32()` | -2^31 to 2^31-1 |
| `i64` | `int64_t` | `nautilus_int_64()` | -2^63 to 2^63-1 |
| `ui8` | `uint8_t` | `nautilus_uint_8()` | 0 to 255 |
| `ui16` | `uint16_t` | `nautilus_uint_16()` | 0 to 65,535 |
| `ui32` | `uint32_t` | `nautilus_uint_32()` | 0 to 2^32-1 |
| `ui64` | `uint64_t` | `nautilus_uint_64()` | 0 to 2^64-1 |
| `f32` | `float` | `nautilus_float_32()` | IEEE 754 single |
| `f64` | `double` | `nautilus_float_64()` | IEEE 754 double |

---

## Common Patterns

### Building a Simple Function
```c
// Function that returns a constant
auto* module = nautilus_ir_module_create();
auto* func = nautilus_ir_module_create_function(module, "get_answer");
auto* block = nautilus_ir_function_get_body(func);

auto* value = nautilus_int_64(42);
auto* ret = nautilus_return(value);

nautilus_ir_operation_destroy(ret);
nautilus_ir_operation_destroy(value);
nautilus_ir_block_destroy(block);
```

### Chaining Operations
```c
// a + b + c
auto* a = nautilus_int_32(10);
auto* b = nautilus_int_32(20);
auto* c = nautilus_int_32(30);

auto* ab = nautilus_add(a, b);
auto* abc = nautilus_add(ab, c);
```

### Conditional Logic (using comparisons)
```c
// Compare two values
auto* x = nautilus_int_32(10);
auto* y = nautilus_int_32(20);
auto* is_less = nautilus_less_than(x, y);  // Will be true
```

---

## Tips & Best Practices

1. **Always destroy operation wrappers** after use to prevent memory leaks
2. **Destroy in reverse order** of creation for clarity
3. **Check for NULL** returns from API functions
4. **Use appropriate types** - match your data to the right integer/float size
5. **Clean up blocks** before destroying the module
6. **Compile once, execute many** - reuse executables when possible
7. **Name functions clearly** for easier debugging

---

## Error Handling

Most functions return `nullptr` on error. Always check:

```c
auto* module = nautilus_ir_module_create();
if (!module) {
    // Handle error
    return -1;
}

auto* func = nautilus_ir_module_create_function(module, "my_func");
if (!func) {
    nautilus_ir_module_destroy(module);
    return -1;
}
```


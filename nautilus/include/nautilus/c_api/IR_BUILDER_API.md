# Nautilus C API - IR Builder Implementation

This document describes the implementation of the C API for building Nautilus IR programmatically.

## Overview

The IR Builder API allows users to construct Nautilus IR modules, functions, and operations using a C interface. This is useful for:
- Language bindings (Python, Rust, etc.)
- External tools that need to generate Nautilus IR
- Integration with other JIT frameworks

## API Components

### Module Management

```c
nautilus_ir_module* nautilus_ir_module_create();
void nautilus_ir_module_destroy(nautilus_ir_module* module);
```

Creates and destroys IR modules. A module is a container for functions.

### Function Management

```c
nautilus_ir_function* nautilus_ir_module_create_function(nautilus_ir_module* module, const char* name);
void nautilus_ir_module_destroy_function(nautilus_ir_function* function);
nautilus_ir_block* nautilus_ir_function_get_body(nautilus_ir_function* function);
void nautilus_ir_block_destroy(nautilus_ir_block* block);
```

Functions are created within modules. Each function has an entry block (body) where operations are added.

### Operation Building

#### Constant Operations

```c
// Boolean
nautilus_ir_operation* nautilus_bool(bool value);

// Signed integers
nautilus_ir_operation* nautilus_int_8(int8_t value);
nautilus_ir_operation* nautilus_int_16(int16_t value);
nautilus_ir_operation* nautilus_int_32(int32_t value);
nautilus_ir_operation* nautilus_int_64(int64_t value);

// Unsigned integers
nautilus_ir_operation* nautilus_uint_8(uint8_t value);
nautilus_ir_operation* nautilus_uint_16(uint16_t value);
nautilus_ir_operation* nautilus_uint_32(uint32_t value);
nautilus_ir_operation* nautilus_uint_64(uint64_t value);

// Floating-point
nautilus_ir_operation* nautilus_float_32(float value);
nautilus_ir_operation* nautilus_float_64(double value);

void nautilus_ir_operation_destroy(nautilus_ir_operation* op);
```

#### Arithmetic Operations

```c
nautilus_ir_operation* nautilus_add(nautilus_ir_operation* left, nautilus_ir_operation* right);
nautilus_ir_operation* nautilus_sub(nautilus_ir_operation* left, nautilus_ir_operation* right);
nautilus_ir_operation* nautilus_mul(nautilus_ir_operation* left, nautilus_ir_operation* right);
nautilus_ir_operation* nautilus_div(nautilus_ir_operation* left, nautilus_ir_operation* right);
nautilus_ir_operation* nautilus_mod(nautilus_ir_operation* left, nautilus_ir_operation* right);
nautilus_ir_operation* nautilus_negate(nautilus_ir_operation* operand);
```

#### Logical Operations

```c
nautilus_ir_operation* nautilus_and(nautilus_ir_operation* left, nautilus_ir_operation* right);
nautilus_ir_operation* nautilus_or(nautilus_ir_operation* left, nautilus_ir_operation* right);
nautilus_ir_operation* nautilus_not(nautilus_ir_operation* operand);
```

#### Comparison Operations

```c
nautilus_ir_operation* nautilus_equals(nautilus_ir_operation* left, nautilus_ir_operation* right);
nautilus_ir_operation* nautilus_not_equals(nautilus_ir_operation* left, nautilus_ir_operation* right);
nautilus_ir_operation* nautilus_less_than(nautilus_ir_operation* left, nautilus_ir_operation* right);
nautilus_ir_operation* nautilus_less_equal(nautilus_ir_operation* left, nautilus_ir_operation* right);
nautilus_ir_operation* nautilus_greater_than(nautilus_ir_operation* left, nautilus_ir_operation* right);
nautilus_ir_operation* nautilus_greater_equal(nautilus_ir_operation* left, nautilus_ir_operation* right);
```

#### Control Flow Operations

```c
nautilus_ir_operation* nautilus_return(nautilus_ir_operation* op);
```

### JIT Compilation

```c
nautilus_engine* nautilus_jit_create();
void nautilus_jit_destroy(nautilus_engine* jit);
nautilus_executable* nautilus_jit_compile(nautilus_engine* jit, nautilus_ir_module* module);
void* nautilus_executable_get_function_ptr(nautilus_executable* executable, const char* name);
void nautilus_executable_destroy(nautilus_executable* executable);
```

The JIT engine compiles IR modules into executable machine code.

## Example Usage

Here's a complete example that creates a function returning `10 + 32 = 42`:

```c
// Create a module
nautilus_ir_module* module = nautilus_ir_module_create();

// Create a function named "execute"
nautilus_ir_function* function = nautilus_ir_module_create_function(module, "execute");

// Get the function body (entry block)
nautilus_ir_block* block = nautilus_ir_function_get_body(function);

// Create constants
nautilus_ir_operation* left = nautilus_int_64(10);
nautilus_ir_operation* right = nautilus_int_64(32);

// Add them
nautilus_ir_operation* sum = nautilus_add(left, right);

// Return the result
nautilus_ir_operation* ret = nautilus_return(sum);

// Clean up operation wrappers
nautilus_ir_operation_destroy(ret);
nautilus_ir_operation_destroy(sum);
nautilus_ir_operation_destroy(right);
nautilus_ir_operation_destroy(left);
nautilus_ir_block_destroy(block);

// Create JIT engine
nautilus_engine* engine = nautilus_jit_create();

// Compile the module
nautilus_executable* executable = nautilus_jit_compile(engine, module);

// Get function pointer
void* func_ptr = nautilus_executable_get_function_ptr(executable, "execute");

// Call the function
typedef int64_t (*func_t)();
func_t func = (func_t)func_ptr;
int64_t result = func();  // Returns 42

// Clean up
nautilus_executable_destroy(executable);
nautilus_jit_destroy(engine);
nautilus_ir_module_destroy(module);
```

## Implementation Details

### Memory Management

- **Modules** own their functions
- **Functions** own their operations
- **Operation wrappers** (`nautilus_ir_operation*`) are lightweight handles that should be destroyed after use
- The actual IR operations are owned by the function and are automatically cleaned up when the module is destroyed

### Global Context

The implementation uses a global context (`g_current_function`) to track which function operations should be added to. This is set when:
- A function is created via `nautilus_ir_module_create_function`
- A function body is retrieved via `nautilus_ir_function_get_body`

### Operation IDs

Each operation is assigned a unique ID within its function. The ID counter is managed by the function and incremented for each new operation.

### Finalization

When a module is compiled, the function is finalized:
1. All operations are added to the entry block
2. A `FunctionOperation` is created with the blocks
3. The function is added to the IR graph as a root operation

## Testing

The implementation includes comprehensive tests in:
- `test/c-api-tests/CApiIRBuilderTest.cpp` - Basic IR builder tests
- `test/c-api-tests/CApiExtendedTypesTest.cpp` - Extended type and operation tests

Test coverage includes:
- Module creation/destruction
- Function creation
- All data types (bool, i8-i64, ui8-ui64, f32, f64)
- Arithmetic operations (add, sub, mul, div, mod, negate)
- Logical operations (and, or, not)
- Comparison operations (eq, ne, lt, le, gt, ge)
- Complete function building
- JIT compilation and execution
- Complex expression evaluation

**All 29 tests pass successfully with 80 assertions.**

## Supported Data Types

The C API supports all Nautilus IR data types:

| Type | C Type | Description |
|------|--------|-------------|
| `b` | `bool` | Boolean |
| `i8` | `int8_t` | 8-bit signed integer |
| `i16` | `int16_t` | 16-bit signed integer |
| `i32` | `int32_t` | 32-bit signed integer |
| `i64` | `int64_t` | 64-bit signed integer |
| `ui8` | `uint8_t` | 8-bit unsigned integer |
| `ui16` | `uint16_t` | 16-bit unsigned integer |
| `ui32` | `uint32_t` | 32-bit unsigned integer |
| `ui64` | `uint64_t` | 64-bit unsigned integer |
| `f32` | `float` | 32-bit floating-point |
| `f64` | `double` | 64-bit floating-point |

## Future Enhancements

Potential additions to the API:
- Function parameters
- Multiple basic blocks
- Control flow operations (if, while, loops)
- Memory operations (load, store)
- Pointer operations
- Cast operations
- Binary operations (shift, bitwise)
- Error reporting with detailed messages
- IR introspection and debugging utilities


# Nautilus C API - Control Flow Interface Proposal

## Overview

This document proposes a C API interface for control flow statements in Nautilus IR, including:
- **If-Then** statements
- **If-Then-Else** statements  
- **For loops**
- **While loops**

## Design Philosophy

The API follows these principles:

1. **Builder Pattern**: Use a fluent, builder-style API for constructing control flow
2. **Explicit Block Management**: Users explicitly create and manage basic blocks
3. **Type Safety**: Strong typing through opaque pointers
4. **Memory Safety**: Clear ownership semantics
5. **Flexibility**: Support for complex nested control flow

---

## API Design

### Core Types

```c
// Opaque types (already defined)
typedef struct nautilus_ir_module nautilus_ir_module;
typedef struct nautilus_ir_function nautilus_ir_function;
typedef struct nautilus_ir_block nautilus_ir_block;
typedef struct nautilus_ir_operation nautilus_ir_operation;

// New opaque types for control flow
typedef struct nautilus_if_builder nautilus_if_builder;
typedef struct nautilus_loop_builder nautilus_loop_builder;
```

---

## If-Then Statement

### API Functions

```c
/// @brief Creates an if-then statement builder
/// @param condition Boolean operation to test
/// @return If builder or NULL on error
nautilus_if_builder* nautilus_if_create(nautilus_ir_operation* condition);

/// @brief Gets the "then" block for adding operations
/// @param if_builder The if builder
/// @return Block for the then branch
nautilus_ir_block* nautilus_if_then_block(nautilus_if_builder* if_builder);

/// @brief Finalizes the if-then statement and returns to parent block
/// @param if_builder The if builder
/// @return The merge block where execution continues
nautilus_ir_block* nautilus_if_end(nautilus_if_builder* if_builder);

/// @brief Destroys the if builder
void nautilus_if_destroy(nautilus_if_builder* if_builder);
```

### Example Usage

```c
// Create: if (x < 10) { y = y + 1; }
auto* x = nautilus_int_32(5);
auto* ten = nautilus_int_32(10);
auto* condition = nautilus_less_than(x, ten);

// Create if statement
auto* if_stmt = nautilus_if_create(condition);

// Get then block and add operations
auto* then_block = nautilus_if_then_block(if_stmt);
auto* y = nautilus_int_32(0);
auto* one = nautilus_int_32(1);
auto* y_plus_1 = nautilus_add(y, one);

// End if statement - returns merge block
auto* merge_block = nautilus_if_end(if_stmt);

// Continue with merge block...
auto* result = nautilus_return(y_plus_1);

// Cleanup
nautilus_if_destroy(if_stmt);
```

---

## If-Then-Else Statement

### API Functions

```c
/// @brief Creates an if-then-else statement builder
/// @param condition Boolean operation to test
/// @return If builder or NULL on error
nautilus_if_builder* nautilus_if_else_create(nautilus_ir_operation* condition);

/// @brief Gets the "then" block
nautilus_ir_block* nautilus_if_then_block(nautilus_if_builder* if_builder);

/// @brief Gets the "else" block
nautilus_ir_block* nautilus_if_else_block(nautilus_if_builder* if_builder);

/// @brief Finalizes the if-then-else and returns merge block
nautilus_ir_block* nautilus_if_else_end(nautilus_if_builder* if_builder);

/// @brief Destroys the if builder
void nautilus_if_destroy(nautilus_if_builder* if_builder);
```

### Example Usage

```c
// Create: if (x < 10) { result = 1; } else { result = 2; }
auto* x = nautilus_int_32(5);
auto* ten = nautilus_int_32(10);
auto* condition = nautilus_less_than(x, ten);

// Create if-else statement
auto* if_stmt = nautilus_if_else_create(condition);

// Then branch
auto* then_block = nautilus_if_then_block(if_stmt);
auto* result_then = nautilus_int_32(1);

// Else branch
auto* else_block = nautilus_if_else_block(if_stmt);
auto* result_else = nautilus_int_32(2);

// End if-else - returns merge block
auto* merge_block = nautilus_if_else_end(if_stmt);

// The merge block receives phi values from both branches
// Continue execution...

nautilus_if_destroy(if_stmt);
```

---

## For Loop

### API Functions

```c
/// @brief Creates a for loop builder
/// @param initial_value Initial value of loop variable
/// @param condition Loop continuation condition (receives loop variable)
/// @param increment Increment operation (receives loop variable)
/// @return Loop builder or NULL on error
nautilus_loop_builder* nautilus_for_create(
    nautilus_ir_operation* initial_value,
    nautilus_ir_operation* upper_bound
);

/// @brief Gets the loop body block
/// @param loop_builder The loop builder
/// @return Block for loop body
nautilus_ir_block* nautilus_for_body(nautilus_loop_builder* loop_builder);

/// @brief Gets the loop variable (induction variable)
/// @param loop_builder The loop builder
/// @return Operation representing current loop variable value
nautilus_ir_operation* nautilus_for_variable(nautilus_loop_builder* loop_builder);

/// @brief Sets the increment for the loop variable (default is +1)
/// @param loop_builder The loop builder
/// @param increment Value to add each iteration
void nautilus_for_set_increment(nautilus_loop_builder* loop_builder, 
                                 nautilus_ir_operation* increment);

/// @brief Finalizes the for loop and returns exit block
/// @param loop_builder The loop builder
/// @return Block where execution continues after loop
nautilus_ir_block* nautilus_for_end(nautilus_loop_builder* loop_builder);

/// @brief Destroys the loop builder
void nautilus_loop_destroy(nautilus_loop_builder* loop_builder);
```

### Example Usage

```c
// Create: for (i = 0; i < 10; i++) { sum = sum + i; }
auto* zero = nautilus_int_32(0);
auto* ten = nautilus_int_32(10);

// Create for loop
auto* for_loop = nautilus_for_create(zero, ten);

// Get loop body and loop variable
auto* body = nautilus_for_body(for_loop);
auto* i = nautilus_for_variable(for_loop);

// Add operations in loop body
auto* sum = nautilus_int_32(0);  // This should be a phi node in real implementation
auto* new_sum = nautilus_add(sum, i);

// End loop
auto* exit_block = nautilus_for_end(for_loop);

// Continue after loop
auto* result = nautilus_return(new_sum);

nautilus_loop_destroy(for_loop);
```

---

## While Loop

### API Functions

```c
/// @brief Creates a while loop builder
/// @return Loop builder or NULL on error
nautilus_loop_builder* nautilus_while_create();

/// @brief Sets the loop condition
/// @param loop_builder The loop builder
/// @param condition Boolean operation to test each iteration
void nautilus_while_set_condition(nautilus_loop_builder* loop_builder,
                                   nautilus_ir_operation* condition);

/// @brief Gets the loop body block
nautilus_ir_block* nautilus_while_body(nautilus_loop_builder* loop_builder);

/// @brief Finalizes the while loop
nautilus_ir_block* nautilus_while_end(nautilus_loop_builder* loop_builder);

/// @brief Destroys the loop builder
void nautilus_loop_destroy(nautilus_loop_builder* loop_builder);
```

### Example Usage

```c
// Create: while (x < 10) { x = x + 1; }
auto* x = nautilus_int_32(0);

// Create while loop
auto* while_loop = nautilus_while_create();

// Get body block
auto* body = nautilus_while_body(while_loop);

// Add operations in body
auto* one = nautilus_int_32(1);
auto* new_x = nautilus_add(x, one);

// Set condition (evaluated each iteration)
auto* ten = nautilus_int_32(10);
auto* condition = nautilus_less_than(new_x, ten);
nautilus_while_set_condition(while_loop, condition);

// End loop
auto* exit_block = nautilus_while_end(while_loop);

nautilus_loop_destroy(while_loop);
```

---

## Alternative Design: Callback-Based API

For a more C-friendly approach, we could use callbacks:

```c
typedef void (*nautilus_block_builder_fn)(nautilus_ir_block* block, void* user_data);

/// @brief Creates if-then with callback
void nautilus_if_then(
    nautilus_ir_operation* condition,
    nautilus_block_builder_fn then_fn,
    void* user_data
);

/// @brief Creates if-then-else with callbacks
void nautilus_if_then_else(
    nautilus_ir_operation* condition,
    nautilus_block_builder_fn then_fn,
    nautilus_block_builder_fn else_fn,
    void* user_data
);

/// @brief Creates for loop with callback
void nautilus_for_loop(
    nautilus_ir_operation* start,
    nautilus_ir_operation* end,
    nautilus_block_builder_fn body_fn,
    void* user_data
);
```

### Callback Example

```c
void build_then_block(nautilus_ir_block* block, void* data) {
    int* counter = (int*)data;
    auto* one = nautilus_int_32(1);
    auto* incremented = nautilus_add(nautilus_int_32(*counter), one);
    *counter = 1;
}

void build_else_block(nautilus_ir_block* block, void* data) {
    int* counter = (int*)data;
    *counter = 2;
}

// Usage
int result = 0;
auto* condition = nautilus_less_than(x, y);
nautilus_if_then_else(condition, build_then_block, build_else_block, &result);
```

---

## Recommended Approach: Hybrid Design

Combine both approaches for maximum flexibility:

1. **Builder API** for complex, nested control flow
2. **Callback API** for simple, single-level control flow
3. **Helper functions** for common patterns

This gives users choice based on their use case complexity.

---

## Implementation Considerations

### 1. Block Management

- Builders internally manage block creation and linking
- Users get block references for adding operations
- Automatic merge block creation and phi node insertion

### 2. Variable Scoping

For loops need special handling for loop variables:
- Loop variable is a phi node in the loop header
- Updated value flows back to loop header
- Exit value available after loop

### 3. Nested Control Flow

Support arbitrary nesting:
```c
auto* outer_if = nautilus_if_create(cond1);
auto* then1 = nautilus_if_then_block(outer_if);

    auto* inner_if = nautilus_if_create(cond2);
    auto* then2 = nautilus_if_then_block(inner_if);
    // ... operations ...
    nautilus_if_end(inner_if);

nautilus_if_end(outer_if);
```

### 4. Break and Continue

Additional functions for loop control:

```c
/// @brief Creates a break statement (exit loop)
nautilus_ir_operation* nautilus_break(nautilus_loop_builder* loop);

/// @brief Creates a continue statement (next iteration)
nautilus_ir_operation* nautilus_continue(nautilus_loop_builder* loop);
```

---

## Next Steps

1. Implement basic if-then/if-then-else support
2. Add for loop support with induction variables
3. Add while loop support
4. Implement break/continue
5. Add comprehensive tests
6. Document patterns and best practices



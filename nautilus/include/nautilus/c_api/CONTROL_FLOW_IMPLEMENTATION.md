# Nautilus C API - Control Flow Implementation

## Overview

This document describes the **implemented** control flow interface for the Nautilus C API. This is a low-level, explicit API that gives users direct control over basic blocks and control flow operations.

## Implemented API

### Block Creation

```c
/// @brief Creates a new basic block in the current function
/// @return A new basic block or NULL if no current function
nautilus_ir_block* nautilus_ir_module_create_block();
```

Creates a new basic block that can be used as a target for branches and if operations.

### If Operation (Conditional Branch)

```c
/// @brief Creates an if operation (conditional branch)
/// @param condition Boolean operation to test
/// @param true_block Block to execute if condition is true
/// @param false_block Block to execute if condition is false (can be NULL for if-then)
/// @return If operation or NULL on error
nautilus_ir_operation* nautilus_if(nautilus_ir_operation* condition,
                                    nautilus_ir_block* true_block,
                                    nautilus_ir_block* false_block);
```

Creates a conditional branch that tests a boolean condition and jumps to either the true or false block.

### Branch Operation (Unconditional Jump)

```c
/// @brief Creates a branch operation (unconditional jump)
/// @param target_block Block to branch to
/// @return Branch operation or NULL on error
nautilus_ir_operation* nautilus_branch(nautilus_ir_block* target_block);
```

Creates an unconditional branch to a target block.

---

## Usage Examples

### Example 1: Simple If-Then

```c
// Create: if (x < 10) { /* then block */ }

auto* module = nautilus_ir_module_create();
auto* function = nautilus_ir_module_create_function(module, "if_then");
auto* entry = nautilus_ir_function_get_body(function);

// Create condition
auto* x = nautilus_int_32(5);
auto* ten = nautilus_int_32(10);
auto* condition = nautilus_less_than(x, ten);

// Create blocks
auto* then_block = nautilus_ir_module_create_block();
auto* merge_block = nautilus_ir_module_create_block();

// Create if operation (false_block is merge_block for if-then)
auto* if_op = nautilus_if(condition, then_block, merge_block);

// Add operations to then_block...
// Add branch from then_block to merge_block...

// Cleanup
nautilus_ir_operation_destroy(if_op);
nautilus_ir_block_destroy(merge_block);
nautilus_ir_block_destroy(then_block);
nautilus_ir_operation_destroy(condition);
nautilus_ir_operation_destroy(ten);
nautilus_ir_operation_destroy(x);
nautilus_ir_block_destroy(entry);
nautilus_ir_module_destroy(module);
```

### Example 2: If-Then-Else

```c
// Create: if (x > y) { /* then */ } else { /* else */ }

auto* module = nautilus_ir_module_create();
auto* function = nautilus_ir_module_create_function(module, "if_then_else");
auto* entry = nautilus_ir_function_get_body(function);

// Create condition
auto* x = nautilus_int_32(10);
auto* y = nautilus_int_32(20);
auto* condition = nautilus_greater_than(x, y);

// Create blocks
auto* then_block = nautilus_ir_module_create_block();
auto* else_block = nautilus_ir_module_create_block();
auto* merge_block = nautilus_ir_module_create_block();

// Create if operation
auto* if_op = nautilus_if(condition, then_block, else_block);

// Add operations to then_block...
// Add branch from then_block to merge_block...

// Add operations to else_block...
// Add branch from else_block to merge_block...

// Add operations to merge_block...

// Cleanup...
```

### Example 3: Simple Loop

```c
// Create: while (i < 10) { /* body */ }
// Structure: entry -> header -> body -> header (loop back)
//                       |
//                       v
//                     exit

auto* module = nautilus_ir_module_create();
auto* function = nautilus_ir_module_create_function(module, "simple_loop");
auto* entry = nautilus_ir_function_get_body(function);

// Create blocks
auto* header = nautilus_ir_module_create_block();
auto* body = nautilus_ir_module_create_block();
auto* exit_block = nautilus_ir_module_create_block();

// Entry branches to header
auto* entry_branch = nautilus_branch(header);

// Header has condition: i < 10
auto* i = nautilus_int_32(0);
auto* ten = nautilus_int_32(10);
auto* condition = nautilus_less_than(i, ten);

// If condition true, go to body, else go to exit
auto* if_op = nautilus_if(condition, body, exit_block);

// Body branches back to header
auto* body_branch = nautilus_branch(header);

// Exit returns
auto* zero = nautilus_int_32(0);
auto* ret = nautilus_return(zero);

// Cleanup...
```

### Example 4: Nested If Statements

```c
// Create: if (x < 10) { if (y > 5) { /* inner then */ } }

auto* module = nautilus_ir_module_create();
auto* function = nautilus_ir_module_create_function(module, "nested_if");
auto* entry = nautilus_ir_function_get_body(function);

// Outer if condition
auto* x = nautilus_int_32(5);
auto* ten = nautilus_int_32(10);
auto* outer_cond = nautilus_less_than(x, ten);

// Outer blocks
auto* outer_then = nautilus_ir_module_create_block();
auto* outer_else = nautilus_ir_module_create_block();
auto* outer_merge = nautilus_ir_module_create_block();

// Create outer if
auto* outer_if = nautilus_if(outer_cond, outer_then, outer_else);

// Inner if condition (in outer_then block)
auto* y = nautilus_int_32(3);
auto* five = nautilus_int_32(5);
auto* inner_cond = nautilus_greater_than(y, five);

// Inner blocks
auto* inner_then = nautilus_ir_module_create_block();
auto* inner_else = nautilus_ir_module_create_block();

// Create inner if
auto* inner_if = nautilus_if(inner_cond, inner_then, inner_else);

// Cleanup...
```

---

## Design Characteristics

### ✅ Advantages

1. **Simple and Direct** - Minimal abstraction over Nautilus IR
2. **Explicit Control** - User has full control over block creation and connections
3. **Flexible** - Can construct any control flow pattern
4. **Low Overhead** - Thin wrapper around IR operations

### ⚠️ Limitations

1. **Manual Block Management** - User must create and connect all blocks
2. **No Phi Node Helpers** - User must manually manage block arguments for value merging
3. **No Loop Variable Management** - For loops require manual induction variable setup
4. **Easy to Make Mistakes** - Missing branches or incorrect connections can cause errors

### 🔄 Current Limitations

The current implementation has some limitations:

1. **Single Block Context** - All operations are added to the entry block during finalization
2. **No Block-Scoped Operations** - Cannot add operations to specific blocks after creation
3. **No Phi Nodes** - Block arguments (phi nodes) are not yet exposed in the C API

These limitations mean that while the API can create the control flow structure, it cannot yet:
- Add different operations to different blocks
- Create phi nodes for value merging
- Compile and execute functions with control flow

---

## Future Enhancements

### Phase 1: Block-Scoped Operations

Add ability to set the current block for operation creation:

```c
/// @brief Sets the current block for adding operations
void nautilus_set_current_block(nautilus_ir_block* block);

/// @brief Gets the current block
nautilus_ir_block* nautilus_get_current_block();
```

### Phase 2: Phi Nodes (Block Arguments)

Add support for block arguments (phi nodes):

```c
/// @brief Adds an argument to a block
nautilus_ir_operation* nautilus_block_add_argument(nautilus_ir_block* block, nautilus_type type);

/// @brief Adds an argument value when branching to a block
void nautilus_branch_with_args(nautilus_ir_block* target, nautilus_ir_operation** args, size_t num_args);
```

### Phase 3: Higher-Level Builders

Build convenience APIs on top of the low-level primitives:

```c
// If-then-else builder
nautilus_if_builder* nautilus_if_builder_create(nautilus_ir_operation* condition);
nautilus_ir_block* nautilus_if_builder_then_block(nautilus_if_builder* builder);
nautilus_ir_block* nautilus_if_builder_else_block(nautilus_if_builder* builder);
nautilus_ir_operation* nautilus_if_builder_phi(nautilus_if_builder* builder,
                                                nautilus_ir_operation* then_val,
                                                nautilus_ir_operation* else_val);
void nautilus_if_builder_end(nautilus_if_builder* builder);
```

---

## Testing

The control flow API is tested with **7 test cases**:

1. **Create Block** - Basic block creation
2. **Simple If-Then** - Single-branch conditional
3. **If-Then-Else** - Two-way branching
4. **Branch Operation** - Unconditional jump
5. **Max Function with If-Else** - Practical example
6. **Simple Loop Structure** - Loop with header, body, and exit
7. **Nested If** - Nested conditional statements

All tests pass successfully, verifying that the API correctly creates the control flow structure.

---

## Implementation Details

### Internal Structures

```cpp
struct nautilus_ir_block {
    nautilus::compiler::ir::BasicBlock* block;
    nautilus_ir_function* function;
};
```

### Block Creation

Blocks are created with a unique ID and added to the function's block list:

```cpp
nautilus_ir_block* nautilus_ir_module_create_block() {
    if (!g_current_function) {
        return nullptr;
    }

    std::vector<std::unique_ptr<BasicBlockArgument>> args;
    uint16_t blockId = g_current_function->blocks.size();
    auto block = std::make_unique<BasicBlock>(blockId, args);
    auto* blockPtr = block.get();

    g_current_function->blocks.push_back(std::move(block));

    return new nautilus_ir_block(blockPtr, g_current_function);
}
```

### If Operation

Creates an `IfOperation` and sets the true and false block invocations:

```cpp
nautilus_ir_operation* nautilus_if(nautilus_ir_operation* condition,
                                    nautilus_ir_block* true_block,
                                    nautilus_ir_block* false_block) {
    if (!g_current_function || !condition || !true_block) {
        return nullptr;
    }

    auto ifOp = std::make_unique<IfOperation>(condition->op);
    ifOp->setTrueBlockInvocation(true_block->block);

    if (false_block) {
        ifOp->setFalseBlockInvocation(false_block->block);
    }

    auto* opPtr = g_current_function->addOperation(std::move(ifOp));
    return new nautilus_ir_operation(opPtr, g_current_function);
}
```

### Branch Operation

Creates a `BranchOperation` and sets the target block:

```cpp
nautilus_ir_operation* nautilus_branch(nautilus_ir_block* target_block) {
    if (!g_current_function || !target_block) {
        return nullptr;
    }

    auto branchOp = std::make_unique<BranchOperation>();
    branchOp->getNextBlockInvocation().setBlock(target_block->block);

    auto* opPtr = g_current_function->addOperation(std::move(branchOp));
    return new nautilus_ir_operation(opPtr, g_current_function);
}
```

---

## Summary

The implemented control flow API provides:

✅ **Block Creation** - Create new basic blocks
✅ **If Operations** - Conditional branching
✅ **Branch Operations** - Unconditional jumps
✅ **Nested Structures** - Support for complex control flow
✅ **Comprehensive Tests** - 7 test cases covering all features

This is a solid foundation for control flow in the Nautilus C API. The next steps are to add block-scoped operations and phi node support to enable full compilation and execution of functions with control flow.


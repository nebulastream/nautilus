# Nautilus C API - Control Flow Detailed Design

## Table of Contents
1. [Architecture Overview](#architecture-overview)
2. [If-Then-Else Design](#if-then-else-design)
3. [Loop Design](#loop-design)
4. [Implementation Details](#implementation-details)
5. [Usage Examples](#usage-examples)

---

## Architecture Overview

### Basic Block Structure in Nautilus IR

Nautilus IR uses a **basic block** representation where:
- Each block contains a sequence of operations
- Blocks are connected via **branch** and **if** operations
- **Phi nodes** (block arguments) handle values from multiple predecessors
- Control flow is explicit through block invocations

### Key Concepts

```
Block_0:                    // Entry block
    $1 = condition
    if $1 ? Block_1 : Block_2

Block_1:                    // Then branch
    $2 = operation1
    br Block_3($2)          // Branch to merge

Block_2:                    // Else branch  
    $3 = operation2
    br Block_3($3)          // Branch to merge

Block_3($4):                // Merge block (receives phi)
    return $4
```

---

## If-Then-Else Design

### Internal Structure

```c
struct nautilus_if_builder {
    nautilus_ir_function* function;      // Parent function
    nautilus_ir_block* parent_block;     // Block before if
    nautilus_ir_block* then_block;       // Then branch
    nautilus_ir_block* else_block;       // Else branch (NULL for if-then)
    nautilus_ir_block* merge_block;      // Merge point
    nautilus_ir_operation* condition;    // Boolean condition
    
    // Track values that need phi nodes
    std::vector<phi_value_pair> phi_values;
    
    bool finalized;
};

struct phi_value_pair {
    nautilus_ir_operation* then_value;
    nautilus_ir_operation* else_value;
    nautilus::Type type;
};
```

### API Implementation

```c
// Create if-then-else
nautilus_if_builder* nautilus_if_else_create(nautilus_ir_operation* condition) {
    auto* builder = new nautilus_if_builder();
    builder->condition = condition;
    builder->function = g_current_function;
    builder->parent_block = g_current_block;
    
    // Create blocks
    builder->then_block = create_new_block(function, "then");
    builder->else_block = create_new_block(function, "else");
    builder->merge_block = create_new_block(function, "merge");
    
    // Create if operation in parent block
    auto* if_op = new IfOperation(condition->op);
    if_op->setTrueBlockInvocation(builder->then_block);
    if_op->setFalseBlockInvocation(builder->else_block);
    if_op->setMergeBlock(builder->merge_block);
    
    return builder;
}

// Get then block
nautilus_ir_block* nautilus_if_then_block(nautilus_if_builder* builder) {
    g_current_block = builder->then_block;
    return builder->then_block;
}

// Get else block
nautilus_ir_block* nautilus_if_else_block(nautilus_if_builder* builder) {
    g_current_block = builder->else_block;
    return builder->else_block;
}

// Finalize and get merge block
nautilus_ir_block* nautilus_if_else_end(nautilus_if_builder* builder) {
    // Add branch operations to merge block from both branches
    auto* then_branch = new BranchOperation();
    then_branch->getNextBlockInvocation().setBlock(builder->merge_block);
    builder->then_block->addOperation(then_branch);
    
    auto* else_branch = new BranchOperation();
    else_branch->getNextBlockInvocation().setBlock(builder->merge_block);
    builder->else_block->addOperation(else_branch);
    
    // Set current block to merge
    g_current_block = builder->merge_block;
    builder->finalized = true;
    
    return builder->merge_block;
}
```

### Phi Node Handling

For values that differ between branches:

```c
/// @brief Registers a value that needs a phi node
/// @param builder The if builder
/// @param then_value Value from then branch
/// @param else_value Value from else branch
/// @return Phi operation representing merged value
nautilus_ir_operation* nautilus_if_phi(
    nautilus_if_builder* builder,
    nautilus_ir_operation* then_value,
    nautilus_ir_operation* else_value
);
```

Usage:
```c
auto* if_stmt = nautilus_if_else_create(condition);

auto* then_block = nautilus_if_then_block(if_stmt);
auto* result_then = nautilus_int_32(1);

auto* else_block = nautilus_if_else_block(if_stmt);
auto* result_else = nautilus_int_32(2);

// Create phi for the result
auto* result = nautilus_if_phi(if_stmt, result_then, result_else);

auto* merge = nautilus_if_else_end(if_stmt);
// Now 'result' can be used in merge block
```

---

## Loop Design

### For Loop Structure

```c
struct nautilus_loop_builder {
    nautilus_ir_function* function;
    nautilus_ir_block* pre_header;       // Block before loop
    nautilus_ir_block* header;           // Loop header (condition check)
    nautilus_ir_block* body;             // Loop body
    nautilus_ir_block* exit;             // Exit block
    
    nautilus_ir_operation* loop_var;     // Induction variable (phi)
    nautilus_ir_operation* initial;      // Initial value
    nautilus_ir_operation* bound;        // Upper bound
    nautilus_ir_operation* increment;    // Increment (default: 1)
    
    bool finalized;
};
```

### For Loop Implementation

```c
nautilus_loop_builder* nautilus_for_create(
    nautilus_ir_operation* initial,
    nautilus_ir_operation* bound
) {
    auto* builder = new nautilus_loop_builder();
    builder->function = g_current_function;
    builder->pre_header = g_current_block;
    builder->initial = initial;
    builder->bound = bound;
    
    // Create blocks
    builder->header = create_new_block(function, "loop_header");
    builder->body = create_new_block(function, "loop_body");
    builder->exit = create_new_block(function, "loop_exit");
    
    // Create loop variable as block argument (phi node)
    auto* loop_var_arg = new BasicBlockArgument(initial->op->getStamp());
    builder->header->addArgument(loop_var_arg);
    builder->loop_var = wrap_operation(loop_var_arg);
    
    // Branch from pre-header to header with initial value
    auto* entry_branch = new BranchOperation();
    entry_branch->getNextBlockInvocation().setBlock(builder->header);
    entry_branch->getNextBlockInvocation().addArgument(initial->op);
    builder->pre_header->addOperation(entry_branch);
    
    // Create condition in header
    auto* condition = new CompareOperation(
        loop_var_arg, bound->op, CompareOperation::LT
    );
    builder->header->addOperation(condition);
    
    // Create if operation in header
    auto* if_op = new IfOperation(condition);
    if_op->setTrueBlockInvocation(builder->body);
    if_op->setFalseBlockInvocation(builder->exit);
    builder->header->addOperation(if_op);
    
    // Default increment is 1
    builder->increment = nautilus_int_32(1);
    
    return builder;
}

nautilus_ir_block* nautilus_for_body(nautilus_loop_builder* builder) {
    g_current_block = builder->body;
    return builder->body;
}

nautilus_ir_operation* nautilus_for_variable(nautilus_loop_builder* builder) {
    return builder->loop_var;
}

void nautilus_for_set_increment(
    nautilus_loop_builder* builder,
    nautilus_ir_operation* increment
) {
    builder->increment = increment;
}

nautilus_ir_block* nautilus_for_end(nautilus_loop_builder* builder) {
    // Add increment operation
    auto* incremented = nautilus_add(builder->loop_var, builder->increment);
    
    // Branch back to header with incremented value
    auto* back_branch = new BranchOperation();
    back_branch->getNextBlockInvocation().setBlock(builder->header);
    back_branch->getNextBlockInvocation().addArgument(incremented->op);
    builder->body->addOperation(back_branch);
    
    g_current_block = builder->exit;
    builder->finalized = true;
    
    return builder->exit;
}
```

### While Loop Implementation

```c
nautilus_loop_builder* nautilus_while_create() {
    auto* builder = new nautilus_loop_builder();
    builder->function = g_current_function;
    builder->pre_header = g_current_block;
    
    // Create blocks
    builder->header = create_new_block(function, "while_header");
    builder->body = create_new_block(function, "while_body");
    builder->exit = create_new_block(function, "while_exit");
    
    // Branch to header
    auto* entry_branch = new BranchOperation();
    entry_branch->getNextBlockInvocation().setBlock(builder->header);
    builder->pre_header->addOperation(entry_branch);
    
    return builder;
}

void nautilus_while_set_condition(
    nautilus_loop_builder* builder,
    nautilus_ir_operation* condition
) {
    // Create if operation in header
    auto* if_op = new IfOperation(condition->op);
    if_op->setTrueBlockInvocation(builder->body);
    if_op->setFalseBlockInvocation(builder->exit);
    builder->header->addOperation(if_op);
}

nautilus_ir_block* nautilus_while_body(nautilus_loop_builder* builder) {
    g_current_block = builder->body;
    return builder->body;
}

nautilus_ir_block* nautilus_while_end(nautilus_loop_builder* builder) {
    // Branch back to header
    auto* back_branch = new BranchOperation();
    back_branch->getNextBlockInvocation().setBlock(builder->header);
    builder->body->addOperation(back_branch);
    
    g_current_block = builder->exit;
    builder->finalized = true;
    
    return builder->exit;
}
```

---

## Implementation Details

### Global State Management

```c
// Current block context (similar to g_current_function)
static nautilus_ir_block* g_current_block = nullptr;

// Stack for nested control flow
static std::vector<nautilus_ir_block*> g_block_stack;

void push_block(nautilus_ir_block* block) {
    g_block_stack.push_back(g_current_block);
    g_current_block = block;
}

void pop_block() {
    g_current_block = g_block_stack.back();
    g_block_stack.pop_back();
}
```

### Block Creation Helper

```c
nautilus_ir_block* create_new_block(
    nautilus_ir_function* function,
    const char* name
) {
    auto block_id = function->nextBlockId++;
    auto* block = new BasicBlock(block_id, /* arguments */);
    function->blocks.push_back(block);
    return wrap_block(block);
}
```

### Memory Management

- Builders own temporary state
- Blocks are owned by the function
- Operations are owned by blocks
- Builders must be destroyed after use

---

## Usage Examples

### Example 1: Simple If-Then

```c
// if (x < 10) { y = y + 1; }
auto* x = nautilus_int_32(5);
auto* y = nautilus_int_32(0);
auto* ten = nautilus_int_32(10);

auto* cond = nautilus_less_than(x, ten);
auto* if_stmt = nautilus_if_create(cond);

auto* then_block = nautilus_if_then_block(if_stmt);
auto* one = nautilus_int_32(1);
auto* new_y = nautilus_add(y, one);

auto* merge = nautilus_if_end(if_stmt);
nautilus_if_destroy(if_stmt);
```

### Example 2: If-Then-Else with Phi

```c
// result = (x < 10) ? 1 : 2;
auto* x = nautilus_int_32(5);
auto* ten = nautilus_int_32(10);
auto* cond = nautilus_less_than(x, ten);

auto* if_stmt = nautilus_if_else_create(cond);

auto* then_block = nautilus_if_then_block(if_stmt);
auto* val_then = nautilus_int_32(1);

auto* else_block = nautilus_if_else_block(if_stmt);
auto* val_else = nautilus_int_32(2);

auto* result = nautilus_if_phi(if_stmt, val_then, val_else);
auto* merge = nautilus_if_else_end(if_stmt);

auto* ret = nautilus_return(result);
nautilus_if_destroy(if_stmt);
```

### Example 3: For Loop

```c
// sum = 0; for (i = 0; i < 10; i++) { sum += i; }
auto* sum = nautilus_int_32(0);
auto* zero = nautilus_int_32(0);
auto* ten = nautilus_int_32(10);

auto* for_loop = nautilus_for_create(zero, ten);
auto* body = nautilus_for_body(for_loop);
auto* i = nautilus_for_variable(for_loop);

// In body: sum = sum + i (needs phi handling)
auto* new_sum = nautilus_add(sum, i);

auto* exit = nautilus_for_end(for_loop);
auto* ret = nautilus_return(new_sum);

nautilus_loop_destroy(for_loop);
```

### Example 4: Nested If

```c
// if (x > 0) { if (x < 10) { result = 1; } else { result = 2; } } else { result = 3; }
auto* outer_if = nautilus_if_else_create(cond1);

auto* then1 = nautilus_if_then_block(outer_if);
    auto* inner_if = nautilus_if_else_create(cond2);
    auto* then2 = nautilus_if_then_block(inner_if);
    auto* val1 = nautilus_int_32(1);
    auto* else2 = nautilus_if_else_block(inner_if);
    auto* val2 = nautilus_int_32(2);
    auto* inner_result = nautilus_if_phi(inner_if, val1, val2);
    nautilus_if_else_end(inner_if);

auto* else1 = nautilus_if_else_block(outer_if);
auto* val3 = nautilus_int_32(3);

auto* result = nautilus_if_phi(outer_if, inner_result, val3);
nautilus_if_else_end(outer_if);

nautilus_if_destroy(inner_if);
nautilus_if_destroy(outer_if);
```



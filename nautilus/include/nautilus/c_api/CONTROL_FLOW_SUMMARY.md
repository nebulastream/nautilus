# Nautilus C API - Control Flow Interface Summary

## Executive Summary

This document summarizes the proposed control flow interface for the Nautilus C API, enabling users to build if-then-else statements and loops programmatically.

---

## Proposed API Overview

### Core Control Flow Constructs

1. **If-Then Statement**
   ```c
   nautilus_if_builder* nautilus_if_create(nautilus_ir_operation* condition);
   nautilus_ir_block* nautilus_if_then_block(nautilus_if_builder* builder);
   nautilus_ir_block* nautilus_if_end(nautilus_if_builder* builder);
   void nautilus_if_destroy(nautilus_if_builder* builder);
   ```

2. **If-Then-Else Statement**
   ```c
   nautilus_if_builder* nautilus_if_else_create(nautilus_ir_operation* condition);
   nautilus_ir_block* nautilus_if_then_block(nautilus_if_builder* builder);
   nautilus_ir_block* nautilus_if_else_block(nautilus_if_builder* builder);
   nautilus_ir_operation* nautilus_if_phi(nautilus_if_builder* builder,
                                          nautilus_ir_operation* then_val,
                                          nautilus_ir_operation* else_val);
   nautilus_ir_block* nautilus_if_else_end(nautilus_if_builder* builder);
   void nautilus_if_destroy(nautilus_if_builder* builder);
   ```

3. **For Loop**
   ```c
   nautilus_loop_builder* nautilus_for_create(nautilus_ir_operation* start,
                                              nautilus_ir_operation* end);
   nautilus_ir_block* nautilus_for_body(nautilus_loop_builder* builder);
   nautilus_ir_operation* nautilus_for_variable(nautilus_loop_builder* builder);
   void nautilus_for_set_increment(nautilus_loop_builder* builder,
                                    nautilus_ir_operation* increment);
   nautilus_ir_block* nautilus_for_end(nautilus_loop_builder* builder);
   void nautilus_loop_destroy(nautilus_loop_builder* builder);
   ```

4. **While Loop**
   ```c
   nautilus_loop_builder* nautilus_while_create(void);
   void nautilus_while_set_condition(nautilus_loop_builder* builder,
                                      nautilus_ir_operation* condition);
   nautilus_ir_block* nautilus_while_body(nautilus_loop_builder* builder);
   nautilus_ir_block* nautilus_while_end(nautilus_loop_builder* builder);
   void nautilus_loop_destroy(nautilus_loop_builder* builder);
   ```

---

## Quick Examples

### If-Then-Else

```c
// if (x < 10) { result = 1; } else { result = 2; }
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

### For Loop

```c
// for (i = 0; i < 10; i++) { sum = sum + i; }
auto* sum = nautilus_int_32(0);
auto* zero = nautilus_int_32(0);
auto* ten = nautilus_int_32(10);

auto* for_loop = nautilus_for_create(zero, ten);
auto* body = nautilus_for_body(for_loop);
auto* i = nautilus_for_variable(for_loop);

auto* new_sum = nautilus_add(sum, i);

auto* exit = nautilus_for_end(for_loop);
auto* ret = nautilus_return(new_sum);

nautilus_loop_destroy(for_loop);
```

---

## Design Principles

### 1. Builder Pattern
- Explicit state management
- Clear ownership semantics
- Supports complex nesting
- Easy to debug

### 2. Block-Based Control Flow
- Follows Nautilus IR structure
- Explicit basic blocks
- Manual block transitions
- Phi nodes for value merging

### 3. Memory Management
- Builders are temporary
- Blocks owned by function
- Operations owned by blocks
- Explicit cleanup required

### 4. Type Safety
- Opaque pointer types
- Compile-time type checking
- NULL checks for errors

---

## Implementation Architecture

### Internal Structures

```c
struct nautilus_if_builder {
    nautilus_ir_function* function;
    nautilus_ir_block* parent_block;
    nautilus_ir_block* then_block;
    nautilus_ir_block* else_block;
    nautilus_ir_block* merge_block;
    nautilus_ir_operation* condition;
    bool finalized;
};

struct nautilus_loop_builder {
    nautilus_ir_function* function;
    nautilus_ir_block* pre_header;
    nautilus_ir_block* header;
    nautilus_ir_block* body;
    nautilus_ir_block* exit;
    nautilus_ir_operation* loop_var;
    nautilus_ir_operation* initial;
    nautilus_ir_operation* bound;
    nautilus_ir_operation* increment;
    bool finalized;
};
```

### Global State

```c
// Current block context
static nautilus_ir_block* g_current_block = nullptr;

// Block stack for nesting
static std::vector<nautilus_ir_block*> g_block_stack;
```

---

## Key Features

### ✅ Supported

1. **If-Then Statements**
   - Single branch conditionals
   - Automatic merge block creation

2. **If-Then-Else Statements**
   - Two-way branching
   - Phi node support for value merging

3. **For Loops**
   - Counted loops with induction variable
   - Configurable increment
   - Automatic loop variable management

4. **While Loops**
   - Condition-based loops
   - Flexible condition placement

5. **Nested Control Flow**
   - Arbitrary nesting depth
   - Proper block management

### 🔄 Future Enhancements

1. **Break/Continue**
   ```c
   nautilus_ir_operation* nautilus_break(nautilus_loop_builder* loop);
   nautilus_ir_operation* nautilus_continue(nautilus_loop_builder* loop);
   ```

2. **Switch Statements**
   ```c
   nautilus_switch_builder* nautilus_switch_create(nautilus_ir_operation* value);
   nautilus_ir_block* nautilus_switch_case(nautilus_switch_builder* builder, int64_t value);
   nautilus_ir_block* nautilus_switch_default(nautilus_switch_builder* builder);
   ```

3. **Do-While Loops**
   ```c
   nautilus_loop_builder* nautilus_do_while_create(void);
   ```

4. **Loop Phi Nodes**
   ```c
   nautilus_ir_operation* nautilus_loop_phi(nautilus_loop_builder* builder,
                                            nautilus_ir_operation* initial,
                                            nautilus_ir_operation* update);
   ```

---

## Usage Patterns

### Pattern 1: Simple Conditional

```c
auto* cond = nautilus_less_than(x, y);
auto* if_stmt = nautilus_if_create(cond);
auto* then_block = nautilus_if_then_block(if_stmt);
// Add operations...
nautilus_if_end(if_stmt);
nautilus_if_destroy(if_stmt);
```

### Pattern 2: Value Selection

```c
auto* if_stmt = nautilus_if_else_create(cond);
auto* then_block = nautilus_if_then_block(if_stmt);
auto* val1 = nautilus_int_32(1);
auto* else_block = nautilus_if_else_block(if_stmt);
auto* val2 = nautilus_int_32(2);
auto* result = nautilus_if_phi(if_stmt, val1, val2);
nautilus_if_else_end(if_stmt);
```

### Pattern 3: Counted Loop

```c
auto* for_loop = nautilus_for_create(start, end);
auto* body = nautilus_for_body(for_loop);
auto* i = nautilus_for_variable(for_loop);
// Use i in body...
nautilus_for_end(for_loop);
nautilus_loop_destroy(for_loop);
```

### Pattern 4: Nested Control Flow

```c
auto* outer_if = nautilus_if_else_create(cond1);
auto* then1 = nautilus_if_then_block(outer_if);
    auto* inner_if = nautilus_if_create(cond2);
    auto* then2 = nautilus_if_then_block(inner_if);
    // ...
    nautilus_if_end(inner_if);
    nautilus_if_destroy(inner_if);
auto* else1 = nautilus_if_else_block(outer_if);
// ...
nautilus_if_else_end(outer_if);
nautilus_if_destroy(outer_if);
```

---

## Testing Strategy

### Unit Tests

1. **If-Then Tests**
   - Simple if statement
   - If with multiple operations
   - If with no operations

2. **If-Then-Else Tests**
   - Basic if-else
   - If-else with phi nodes
   - Nested if-else

3. **For Loop Tests**
   - Simple counting loop
   - Loop with custom increment
   - Empty loop
   - Nested loops

4. **While Loop Tests**
   - Basic while loop
   - While with complex condition
   - Infinite loop (with break)

### Integration Tests

1. **JIT Compilation**
   - Compile and execute if-then-else
   - Compile and execute for loop
   - Compile and execute nested structures

2. **Complex Scenarios**
   - Fibonacci sequence
   - Factorial calculation
   - Nested loop matrix operations

---

## Documentation Deliverables

1. ✅ **CONTROL_FLOW_PROPOSAL.md** - High-level API proposal
2. ✅ **CONTROL_FLOW_DETAILED_DESIGN.md** - Implementation details
3. ✅ **CONTROL_FLOW_API_STYLES.md** - API style comparison
4. ✅ **CONTROL_FLOW_SUMMARY.md** - This document

---

## Next Steps

### Phase 1: Foundation (Week 1-2)
- [ ] Implement if-then builder
- [ ] Implement if-then-else builder
- [ ] Add phi node support
- [ ] Write basic tests

### Phase 2: Loops (Week 3-4)
- [ ] Implement for loop builder
- [ ] Implement while loop builder
- [ ] Add loop variable management
- [ ] Write loop tests

### Phase 3: Integration (Week 5)
- [ ] JIT compilation tests
- [ ] Complex scenario tests
- [ ] Performance benchmarks
- [ ] Documentation updates

### Phase 4: Polish (Week 6)
- [ ] Add break/continue support
- [ ] Error handling improvements
- [ ] API refinements based on feedback
- [ ] Final documentation

---

## Conclusion

The proposed control flow API provides:

✅ **Complete control flow support** - if-then, if-then-else, for, while
✅ **Builder pattern** - Explicit, flexible, debuggable
✅ **Phi node support** - Proper value merging
✅ **Nested structures** - Arbitrary nesting depth
✅ **Type safety** - Strong typing through opaque pointers
✅ **Clear semantics** - Follows Nautilus IR structure

The API is ready for implementation and will significantly enhance the Nautilus C API's capabilities for programmatic IR construction.


# Nautilus C API - Control Flow API Style Comparison

This document compares different API design styles for control flow in the Nautilus C API.

---

## Style 1: Builder Pattern (Recommended)

### Characteristics
- Explicit block management
- Maximum flexibility
- Clear ownership semantics
- Supports complex nesting

### API

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

### Example

```c
auto* cond = nautilus_less_than(x, y);
auto* if_stmt = nautilus_if_else_create(cond);

auto* then_block = nautilus_if_then_block(if_stmt);
auto* result_then = nautilus_int_32(1);

auto* else_block = nautilus_if_else_block(if_stmt);
auto* result_else = nautilus_int_32(2);

auto* result = nautilus_if_phi(if_stmt, result_then, result_else);
auto* merge = nautilus_if_else_end(if_stmt);

nautilus_if_destroy(if_stmt);
```

### Pros
✅ Explicit and clear
✅ Easy to debug
✅ Supports arbitrary nesting
✅ User controls block transitions
✅ Familiar to C++ developers

### Cons
❌ Verbose
❌ Requires manual cleanup
❌ More boilerplate code

---

## Style 2: Callback Pattern

### Characteristics
- Automatic block management
- Scoped execution
- Less boilerplate
- Common in C APIs

### API

```c
typedef void (*nautilus_block_fn)(void* user_data);

void nautilus_if_then_else(
    nautilus_ir_operation* condition,
    nautilus_block_fn then_fn,
    nautilus_block_fn else_fn,
    void* user_data
);

void nautilus_for_loop(
    nautilus_ir_operation* start,
    nautilus_ir_operation* end,
    nautilus_block_fn body_fn,
    void* user_data
);
```

### Example

```c
typedef struct {
    nautilus_ir_operation* x;
    nautilus_ir_operation* result;
} if_context;

void then_branch(void* data) {
    if_context* ctx = (if_context*)data;
    ctx->result = nautilus_int_32(1);
}

void else_branch(void* data) {
    if_context* ctx = (if_context*)data;
    ctx->result = nautilus_int_32(2);
}

if_context ctx = {x, NULL};
auto* cond = nautilus_less_than(x, y);
nautilus_if_then_else(cond, then_branch, else_branch, &ctx);
// ctx.result now contains the phi value
```

### Pros
✅ Less boilerplate
✅ Automatic cleanup
✅ Scoped execution
✅ Familiar to C developers

### Cons
❌ Less flexible
❌ Harder to debug
❌ Callback hell for nested structures
❌ Requires context structs

---

## Style 3: Macro-Based DSL

### Characteristics
- Most concise
- Looks like native C
- Hides complexity
- Compile-time generation

### API

```c
#define NAUTILUS_IF(cond) \
    nautilus_if_begin(cond); \
    if (1)

#define NAUTILUS_ELSE \
    nautilus_else_begin(); \
    if (1)

#define NAUTILUS_END_IF \
    nautilus_if_end();

#define NAUTILUS_FOR(var, start, end) \
    for (auto* var = nautilus_for_begin(start, end); \
         nautilus_for_check(); \
         nautilus_for_next())
```

### Example

```c
auto* cond = nautilus_less_than(x, y);

NAUTILUS_IF(cond) {
    auto* result = nautilus_int_32(1);
} NAUTILUS_ELSE {
    auto* result = nautilus_int_32(2);
} NAUTILUS_END_IF

NAUTILUS_FOR(i, zero, ten) {
    sum = nautilus_add(sum, i);
}
```

### Pros
✅ Very concise
✅ Looks like native code
✅ Easy to read

### Cons
❌ Macro magic (hard to debug)
❌ Limited flexibility
❌ Compiler-specific behavior
❌ Not pure C

---

## Style 4: Fluent/Chaining API

### Characteristics
- Method chaining
- Compact syntax
- Self-documenting
- Modern C++ style

### API

```c
nautilus_if_builder* nautilus_if_else_create(nautilus_ir_operation* condition);
nautilus_if_builder* nautilus_if_then(nautilus_if_builder* builder, 
                                      nautilus_block_fn fn, void* data);
nautilus_if_builder* nautilus_if_else(nautilus_if_builder* builder,
                                      nautilus_block_fn fn, void* data);
nautilus_ir_block* nautilus_if_build(nautilus_if_builder* builder);
```

### Example

```c
auto* merge = nautilus_if_build(
    nautilus_if_else(
        nautilus_if_then(
            nautilus_if_else_create(cond),
            then_fn, &then_data
        ),
        else_fn, &else_data
    )
);
```

### Pros
✅ Compact
✅ Self-documenting
✅ Automatic cleanup

### Cons
❌ Hard to read when nested
❌ Difficult to debug
❌ Not idiomatic C

---

## Style 5: Hybrid Approach (Recommended)

Combine the best of multiple styles:

### API

```c
// Builder API for complex cases
nautilus_if_builder* nautilus_if_else_create(nautilus_ir_operation* condition);
nautilus_ir_block* nautilus_if_then_block(nautilus_if_builder* builder);
nautilus_ir_block* nautilus_if_else_block(nautilus_if_builder* builder);
nautilus_ir_block* nautilus_if_else_end(nautilus_if_builder* builder);

// Callback API for simple cases
void nautilus_if_then_else_simple(
    nautilus_ir_operation* condition,
    nautilus_block_fn then_fn,
    nautilus_block_fn else_fn,
    void* user_data
);

// Helper macros (optional)
#define NAUTILUS_IF_BUILDER(name, cond) \
    nautilus_if_builder* name = nautilus_if_else_create(cond)

#define NAUTILUS_IF_CLEANUP(name) \
    nautilus_if_destroy(name)
```

### Example - Complex Case (Builder)

```c
auto* outer_if = nautilus_if_else_create(cond1);
auto* then1 = nautilus_if_then_block(outer_if);

    auto* inner_if = nautilus_if_else_create(cond2);
    auto* then2 = nautilus_if_then_block(inner_if);
    // ...
    nautilus_if_else_end(inner_if);

auto* else1 = nautilus_if_else_block(outer_if);
// ...
nautilus_if_else_end(outer_if);
```

### Example - Simple Case (Callback)

```c
void then_fn(void* data) {
    auto* val = nautilus_int_32(1);
}

void else_fn(void* data) {
    auto* val = nautilus_int_32(2);
}

nautilus_if_then_else_simple(cond, then_fn, else_fn, NULL);
```

### Pros
✅ Flexibility for all use cases
✅ Simple cases are simple
✅ Complex cases are explicit
✅ Users choose their style

### Cons
❌ Larger API surface
❌ More documentation needed

---

## Recommendation

**Use the Hybrid Approach (Style 5)** with:

1. **Builder API as primary interface**
   - For complex, nested control flow
   - When fine-grained control is needed
   - For debugging and introspection

2. **Callback API for convenience**
   - For simple, single-level control flow
   - When brevity is preferred
   - For common patterns

3. **Optional helper macros**
   - For reducing boilerplate
   - For RAII-style cleanup
   - User can opt-in

### Implementation Priority

1. **Phase 1**: Implement builder API
   - If-then
   - If-then-else
   - Basic tests

2. **Phase 2**: Add loop support
   - For loops
   - While loops
   - Break/continue

3. **Phase 3**: Add callback API
   - Simple if-then-else
   - Simple loops
   - Convenience functions

4. **Phase 4**: Add helper macros
   - Cleanup helpers
   - Common patterns
   - Documentation

---

## API Comparison Table

| Feature | Builder | Callback | Macro | Fluent | Hybrid |
|---------|---------|----------|-------|--------|--------|
| Simplicity | ⭐⭐ | ⭐⭐⭐⭐ | ⭐⭐⭐⭐⭐ | ⭐⭐⭐ | ⭐⭐⭐ |
| Flexibility | ⭐⭐⭐⭐⭐ | ⭐⭐ | ⭐⭐ | ⭐⭐⭐ | ⭐⭐⭐⭐⭐ |
| Debuggability | ⭐⭐⭐⭐⭐ | ⭐⭐⭐ | ⭐ | ⭐⭐ | ⭐⭐⭐⭐ |
| C Idioms | ⭐⭐⭐ | ⭐⭐⭐⭐⭐ | ⭐⭐⭐ | ⭐⭐ | ⭐⭐⭐⭐ |
| Nesting | ⭐⭐⭐⭐⭐ | ⭐⭐ | ⭐⭐⭐ | ⭐⭐ | ⭐⭐⭐⭐⭐ |
| Safety | ⭐⭐⭐⭐ | ⭐⭐⭐⭐ | ⭐⭐ | ⭐⭐⭐ | ⭐⭐⭐⭐ |

**Legend**: ⭐ = Poor, ⭐⭐⭐ = Good, ⭐⭐⭐⭐⭐ = Excellent

---

## Conclusion

The **Hybrid Approach** provides the best balance of:
- Simplicity for common cases
- Power for complex scenarios
- Familiarity for C developers
- Flexibility for different use cases

Start with the builder API as the foundation, then add convenience functions based on user feedback.


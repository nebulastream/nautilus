# If-Then-Else Lambda Abstraction Design

## Overview

This document describes the design for an `ifThenElse` abstraction that allows expressing conditional logic using lambdas, similar to the `select` interface but with proper control flow branching.

## Motivation

### Current Approaches

1. **Native if-else**: Works with the tracing system but doesn't return a value directly:
   ```cpp
   val<int> result;
   if (condition) {
       result = compute1();
   } else {
       result = compute2();
   }
   ```

2. **select operator**: Returns a value but evaluates BOTH branches before selecting:
   ```cpp
   val<int> result = select(condition, compute1(), compute2());
   // Both compute1() and compute2() are evaluated!
   ```

### The `ifThenElse` Abstraction

Provides expression-based conditional with short-circuit evaluation:
```cpp
val<int> result = ifThenElse(condition,
    []{ return compute1(); },  // Only evaluated if true
    []{ return compute2(); }   // Only evaluated if false
);
```

## Design Goals

1. **Expression-based**: Can be used in expressions and returns a value
2. **Short-circuit evaluation**: Only the appropriate branch is evaluated per execution path
3. **Proper control flow**: Creates CMP, true block, false block, and merge block in the trace
4. **Type safety**: Both branches must return the same type
5. **Composable**: Can be nested and combined with other operations

## Semantic Differences

| Aspect | `select(cond, a, b)` | `ifThenElse(cond, λ₁, λ₂)` |
|--------|---------------------|---------------------------|
| Evaluation | Both `a` and `b` evaluated | Only one lambda executed |
| IR Operation | Single `SELECT` op | `CMP` + blocks + `JMP` |
| Side effects | Both sides have side effects | Only one side has side effects |
| Compilation | Conditional move (cmov) | Branch instructions |
| Use case | Simple value selection | Complex branch logic |

## Trace Output

For this code:
```cpp
val<int32_t> result = ifThenElse(condition,
    []{ return val<int32_t>(10); },
    []{ return val<int32_t>(20); }
);
```

Expected trace structure:
```
B0(...)
    ...condition evaluation...
    CMP $N $cond B1() B2()
B1()
    CONST $X 10 :i32
    JMP B3()
B2()
    CONST $Y 20 :i32
    JMP B3()
B3() ControlFlowMerge
    ...merged result used here...
```

## Implementation

### Function Signature

```cpp
template <typename ThenFn, typename ElseFn>
    requires std::invocable<ThenFn> && std::invocable<ElseFn>
auto ifThenElse(val<bool> condition, ThenFn&& thenBranch, ElseFn&& elseBranch)
    -> std::invoke_result_t<ThenFn>;
```

### Type Constraints

- Both `ThenFn` and `ElseFn` must be callable with no arguments
- Return types of both lambdas must be identical
- Return type must be a valid `val<T>` type or void

### Implementation Strategy

The implementation leverages the existing tracing infrastructure:

1. When `condition` is converted to `bool` (via `operator bool()`), it calls `traceBool()` which:
   - Records a `CMP` operation in the trace
   - Creates true and false blocks
   - Returns the boolean result for the current execution path

2. Based on the result, only one lambda is executed

3. The symbolic execution context ensures all paths are eventually explored

### Code

```cpp
template <typename ThenFn, typename ElseFn>
    requires std::invocable<ThenFn> && std::invocable<ElseFn>
auto ifThenElse(val<bool> condition, ThenFn&& thenBranch, ElseFn&& elseBranch)
    -> std::invoke_result_t<ThenFn> {

    using ThenResult = std::invoke_result_t<ThenFn>;
    using ElseResult = std::invoke_result_t<ElseFn>;

    static_assert(std::is_same_v<ThenResult, ElseResult>,
        "ifThenElse: both branches must return the same type");

    if (condition) {
        return std::forward<ThenFn>(thenBranch)();
    } else {
        return std::forward<ElseFn>(elseBranch)();
    }
}
```

### Void Return Type Support

For cases where branches don't return values:

```cpp
template <typename ThenFn, typename ElseFn>
    requires std::invocable<ThenFn> && std::invocable<ElseFn> &&
             std::is_void_v<std::invoke_result_t<ThenFn>>
void ifThenElse(val<bool> condition, ThenFn&& thenBranch, ElseFn&& elseBranch) {

    static_assert(std::is_void_v<std::invoke_result_t<ElseFn>>,
        "ifThenElse: both branches must have the same return type (void)");

    if (condition) {
        std::forward<ThenFn>(thenBranch)();
    } else {
        std::forward<ElseFn>(elseBranch)();
    }
}
```

## Usage Examples

### Basic Usage

```cpp
val<int32_t> max_value = ifThenElse(x > y,
    [&]{ return x; },
    [&]{ return y; }
);
```

### With Side Effects

```cpp
val<int32_t> result = ifThenElse(condition,
    [&]{
        counter = counter + 1;  // Side effect only when true
        return expensive_compute();
    },
    [&]{
        return cached_value;
    }
);
```

### Nested ifThenElse

```cpp
val<int32_t> classify = ifThenElse(x < 0,
    []{ return val<int32_t>(-1); },
    [&]{
        return ifThenElse(x > 0,
            []{ return val<int32_t>(1); },
            []{ return val<int32_t>(0); }
        );
    }
);
```

### Probability Hints

```cpp
auto likely_condition = (value > threshold);
likely_condition.setIsTrueProbability(0.9);

val<int32_t> result = ifThenElse(likely_condition,
    [&]{ return fast_path(); },
    [&]{ return slow_path(); }
);
```

## Short-Circuit Evaluation Details

The tracing system handles short-circuit evaluation through symbolic execution:

1. **First Pass**: Symbolic execution chooses one branch (e.g., true)
   - Only the then-lambda is executed
   - Trace records: CMP → true block → JMP to merge

2. **Subsequent Pass**: Symbolic execution explores the other branch
   - Only the else-lambda is executed
   - Trace records: (follows existing) → false block → JMP to merge

3. **Final Trace**: Contains both paths, connected via ControlFlowMerge block

This ensures:
- Side effects only occur in the executed branch
- Expensive computations are avoided when not needed
- The trace accurately represents all possible execution paths

## Testing Strategy

1. **Basic tests**: Verify correct value selection for true/false conditions
2. **Side effect tests**: Verify only one branch has side effects
3. **Type tests**: Test with various val<T> types (int, float, bool, pointer)
4. **Nested tests**: Test nested ifThenElse expressions
5. **Trace tests**: Verify correct block structure in generated trace
6. **Compilation tests**: Verify correct code generation across backends

## File Organization

- Implementation: `nautilus/include/nautilus/ifThenElse.hpp`
- Tests: `nautilus/test/execution-tests/IfThenElseTest.cpp`
- Test functions: `nautilus/test/common/IfThenElseOperations.hpp`
- Test data: `nautilus/test/data/if-then-else-tests/`

---
name: nautilus-review
description: Review user-written Nautilus code (functions using `val<T>`, `static_val`, `invoke`, traced control flow, etc.) and propose strategies that improve tracing quality and the quality of generated code. Use when the user asks to review traced kernels, audit a Nautilus PR, evaluate a JIT-compiled function for tracing pitfalls, or asks about why a Nautilus function generates bad IR / slow code.
---

# Nautilus Code Review: Tracing & Codegen Quality

This skill reviews Nautilus user code with one specific goal: **improve the quality of the trace and the code that the JIT eventually emits.** It is not a generic C++ review. It assumes Nautilus's tracing semantics: every operation on a `val<T>` is recorded into an execution trace, the trace is lifted to an SSA control-flow graph, optimized (constant folding, DCE, loop analysis, control-flow analysis), and lowered through MLIR / C++ / bytecode / AsmJit.

The review's goal is to flag patterns that:

1. **Escape the traced domain** (operations that are not recorded into the IR).
2. **Bloat the trace** (excessive symbolic paths, redundant operations, unwanted unrolling).
3. **Defeat optimization passes** (block constant folding, DCE, alias analysis, loop transforms).
4. **Pessimize the backend** (force opaque calls, defeat cmov/select lowering, lose probability information).

## When to invoke

Run this review when the user asks for any of:

- "Review this nautilus function" / "audit this traced kernel"
- "Why is the generated code for X slow / large / inefficient?"
- "What can I do to make this trace smaller / faster to compile?"
- A PR-level review of code under `nautilus/include/`, `nautilus/src/`, `example/`, or `plugins/` whose diff includes `val<…>`, `static_val`, `invoke`, `select`, or `NAUTILUS_INLINE`.

Do **not** invoke this skill for:

- Pure compiler-internal changes (changes to phases / IR / backends without touching traced code) — those are normal C++ reviews.
- Build, CMake, or formatting feedback.

## Workflow

Follow these steps in order. Stop early if the user only asked for a narrow review.

### 1. Identify the review target

Decide what to read:

- **Branch / PR**: `git diff origin/main...HEAD -- '*.hpp' '*.cpp'` to scope to changed traced code.
- **A specific file or function**: read it fully, plus its callers and any `invoke()`d helpers.
- **A snippet pasted by the user**: review only that snippet but ask about the surrounding loop / driver if it isn't visible.

### 2. Read in this order

For each function under review, mentally classify each value as one of:

| Category                         | Examples                                                                                                |
| -------------------------------- | ------------------------------------------------------------------------------------------------------- |
| **Traced runtime value**         | `val<int32_t>`, `val<int32_t*>`, `val<bool>`, `val<MyStruct>` — captured in the IR.                     |
| **Static / compile-time value**  | `static_val<int>`, plain `int` literal used as a constant, `constexpr`. Specializes the trace.          |
| **Engine / build-time argument** | Native C++ values used during *trace construction* but not part of the JIT'd code (e.g., loop bounds known to the host at trace time). |
| **Escape hatch**                 | Casts from `val<T*>` to `T*`, `(uintptr_t)ptr`, raw pointer arithmetic on the unwrapped pointer. **Suspect.** |

This classification drives most of the findings below.

### 3. Walk the checklist

Apply the **Anti-Pattern Catalog** below. For each match, record:

- File:line of the offending pattern.
- One-sentence description of the smell.
- The IR / codegen consequence (be specific — e.g. "prevents `ConstantFoldingAndCopyPropagationPass` from collapsing the constant", "forces an opaque CALL in the loop body").
- A concrete rewrite suggestion.

### 4. (Optional) Cross-check against the IR

If the user has access to a build, suggest enabling IR dumps to confirm a finding before committing to a rewrite:

```cpp
nautilus::engine::Options options;
options.setOption("dump.ir", true);
options.setOption("dump.after_ssa", true);
options.setOption("dump.after_constant_folding", true);
// Also useful: "mlir.inline_invoke_calls", "engine.backend"
```

Compare the generated IR / `after_ssa` / `after_constant_folding` files (the same layout used under `nautilus/test/data/<category>/`). If a finding is about a CALL that should have been inlined, look for the `call` op in the dumped IR. If it's about constant folding, look at `after_constant_folding/`.

### 5. Report findings

Produce the output described in **Reporting Format** at the bottom.

## Anti-Pattern Catalog

Each entry has: **smell**, **why it hurts the IR/codegen**, and a **rewrite**. Apply them in order — earlier categories are usually higher-impact.

### A. Traced-domain hygiene (highest impact)

#### A1. Casting `val<T*>` to a raw `T*` (or `uintptr_t`) and dereferencing it

The unwrapped load is **not recorded** in the trace, so the IR doesn't see the load at all. Alias analysis then has to assume the worst about every other memory op, and redundant-load elimination cannot fire. Reference: `include/nautilus/val_ptr.hpp` (the `val<T*>::operator*` and `operator[]` overloads are the only path that produces a `Load` op).

Bad:
```cpp
val<int32_t> sum(val<int32_t*> p, val<int32_t> n) {
    auto raw = (int32_t*)(uintptr_t) p;   // escapes the traced domain
    int32_t acc = 0;
    for (val<int32_t> i = 0; i < n; i = i + 1)
        acc += raw[(int32_t) i];          // not traced!
    return val<int32_t>(acc);
}
```
Rewrite: keep the pointer and the accumulator in `val<T>`; index with `p[i]` so the `Load` is emitted.

#### A2. Mixing a native `T` accumulator with `val<T>` operands

The host-side variable is captured by the traced expressions during execution, so the *value* is correct, but the IR sees a sequence of disconnected ConstInt operands instead of an induction variable. Loop analysis cannot recognize the reduction.

Bad:
```cpp
int32_t acc = 0;                          // host variable
for (val<int32_t> i = 0; i < n; i = i + 1)
    acc = acc + (int32_t) array[i];       // each iteration: ConstInt(acc) + Load
```
Rewrite: `val<int32_t> acc = 0;` so the accumulator becomes a live SSA value carried through the loop's phi.

#### A3. Manual offset arithmetic with `(uintptr_t)` to fake a struct field access

Per `val_ptr.hpp:264` the API is `ptr->get(&Struct::field)` / `ptr->set(&Struct::field, value)`. These produce a typed `Load` / `Store` against a known offset, which the MLIR backend can lower with field-aware alias info. Manual `((uintptr_t)ptr + offsetof(...))` casting bypasses this entirely.

Rewrite: always go through `get()` / `set()` for fields; use `p[i]` for array indexing.

### B. Loop strategy

#### B1. Using `val<T>` for a loop bound that is known at trace-construction time

Dynamic loops are explored by the symbolic execution engine (`src/nautilus/tracing/symbolic_execution/`), which iterates until paths converge — small fixed counts (e.g., a 4-lane unroll, a tile size) cause no real speedup as `val<T>` and only inflate the trace and the resulting CFG.

Bad:
```cpp
val<int32_t> tile_size = 4;
for (val<int32_t> j = 0; j < tile_size; j = j + 1) { … }
```
Rewrite: `for (static_val<int> j = 0; j < 4; j = j + 1) { … }` — `static_val` (defined in `include/nautilus/static.hpp`) unrolls at trace time, so the loop disappears from the IR and the body is fully visible to constant folding.

#### B2. Nested dynamic loops where one bound is compile-time known

If the inner loop's count is known, make it `static_val`. The outer dynamic loop remains; the inner becomes straight-line code that the constant-folding and DCE phases can simplify. Dynamic × dynamic nests are the largest cause of long compile times.

#### B3. `while (val<bool> continuing) { …; continuing = (i < n); }` style loops

Re-binding a `val<bool>` condition at the bottom of the loop creates an extra comparison op per iteration and obscures the induction variable. The control-flow analysis phase then often fails to fold the loop into the canonical for-shape.

Rewrite: `for (val<int32_t> i = 0; i < n; i = i + 1) { … }`. Place the comparison in the loop header.

#### B4. Loop counter typed as plain `int` with per-iteration `val<int32_t>(i)` casts

Each cast is a separate IR op. The IR cannot recognize `i` as an induction variable, so strength reduction (e.g., turning `&array[i]` into a running pointer) doesn't fire. Use `val<int32_t> i` from the start.

### C. Branching

#### C1. `if/else` that only chooses between two values

`select(cond, a, b)` from `include/nautilus/select.hpp` lowers to LLVM's `select` intrinsic and typically becomes a `cmov`. An `if/else` produces two basic blocks plus a phi at the merge — visible in `after_ssa/`.

Bad: `if (x < 0) y = 0; else y = x;`
Rewrite: `val<int32_t> y = select(x < 0, val<int32_t>(0), x);`

Use this only for **simple value selection**. Don't use it when both arms have side effects — `select` evaluates both arms unconditionally.

#### C2. No probability hint on a heavily skewed `val<bool>` condition

`val<bool>::setIsTrueProbability(p)` (see `include/nautilus/val_bool.hpp:386`) propagates a hint to the LLVM lowering. With a strong skew (e.g., a hot path validity check that is true >95% of the time), this lets the backend emit better branch layout and cmov decisions.

Add it where the asymmetry is real and large. Don't add `setIsTrueProbability(0.5)` — that's the default.

```cpp
val<bool> is_valid = (record.get(&Record::flag) != 0);
is_valid.setIsTrueProbability(0.98);   // documented expected skew
if (is_valid) { /* hot path */ } else { /* rare error */ }
```

#### C3. Deeply nested `if … else { if … else { … } }` chains

Each level adds a basic block and a phi at merge. A flat `if/else if/else if/…` is lower-cost in the IR and lets the backend more easily lower the chain into a switch or a cmov chain.

#### C4. Repeated identical comparison on the same operands

```cpp
if (x == 42 && (x == 42 || y == 0))   // x == 42 traced twice
```

The IR records each comparison separately. Copy propagation eventually cleans these up, but it is wasted work and lengthens use-def chains. Materialize once: `val<bool> is42 = (x == 42);`.

#### C5. Storing a `val<bool>` result into a plain `bool` for the conditional

```cpp
bool valid = (x > 0);   // strips traced state
if (valid) { … }
```

The conversion to `bool` invokes `operator bool()` which traces a bool (good) but the host `bool` is now disconnected from any data dependency. Reuses become untracked. Keep it as `val<bool>`.

#### C6. Relying on overloaded `&&` / `||` for short-circuit semantics

Overloaded operators in C++ **always evaluate both operands** — this is a language rule, not a Nautilus bug. If both sides have side effects or expensive trace paths, write the short-circuit explicitly:

```cpp
val<bool> result = select(a, b, val<bool>(false));   // a && b, with short-circuit on b's trace
```

Or build the structure with two `if`s. See `include/nautilus/val_bool.hpp` lines around 538–577 for the overload set.

### D. Constants, types, and folding

#### D1. Wrapping a literal in `val<T>` only to pass it as an operand

`val<int32_t>(0) + x` causes the `0` to be emitted as a ConstInt op. That's fine — the `ConstantFoldingAndCopyPropagationPass` (see `src/nautilus/compiler/ir/passes/`) will collapse it later if the other operand is also constant. But avoid wrapping `static_val` constants where the framework already handles the propagation natively — the unrolled trace already has the constant inline.

#### D2. Implicit cross-width conversions in arithmetic

`val<int32_t>(a) + val<int64_t>(b)` introduces a hidden `Cast` op. A few are fine; chains of mixed-width arithmetic generate trace bloat. Pick the wider type up-front and cast once.

#### D3. Storing a wider type through a narrower pointer without an explicit cast

This is the regression captured in `nautilus/test/data/regressions/tracing/store_mising_downcast-gh_#90.*`. Implicit narrowing on `*ptr = wider_val` historically produced an IR `Store` with mismatched operand and pointer types. Always write the cast explicitly:

```cpp
*ptr32 = static_cast<val<uint32_t>>(value64);
```

### E. Memory and aliasing

#### E1. Pointer arithmetic done outside the `val<T*>` API

```cpp
val<int32_t*> p2 = (int32_t*)((uintptr_t)p + 16);   // BAD
val<int32_t*> p2 = p + 4;                            // GOOD: traced offset
```

The latter records a pointer-add op that alias analysis can reason about (the pointers share a base). The former is two opaque values to the IR.

#### E2. Two loads from a pointer that the optimizer should have shared

If you read the same field twice in a function, name it once and reuse — `Load` ops are not always coalesced (it depends on whether memory analysis can prove no intervening `Store`). When the memory layout makes that obvious to a human, hoist the load yourself.

#### E3. Aliasing introduced by `val<ClassType>` copies

`val<ClassType>` parameters are heap-managed. Passing the same struct by value and then writing back to the original pointer creates aliasing edges that defeat load forwarding. Pass `val<MyStruct*>` and operate via `get`/`set` when in doubt.

### F. Function calls

#### F1. `invoke()` on an external helper inside a hot loop without `NAUTILUS_INLINE`

A bare `invoke(fn, args…)` produces an opaque `Call` op in the IR. Inside a loop, that becomes a per-iteration call boundary that blocks loop fusion, hoisting, and strength reduction.

If the helper is small and the inlining plugin is enabled (see `plugins/inlining/include/nautilus/inline.hpp:24`), annotate the helper:

```cpp
NAUTILUS_INLINE int32_t clamp(int32_t x, int32_t lo, int32_t hi) {
    return x < lo ? lo : (x > hi ? hi : x);
}
```

The plugin extracts the helper's LLVM bitcode at compile time and inlines it at JIT time. Without the annotation (Clang only — it's a no-op on GCC), the call stays opaque.

When invoking a helper that won't be inlined, prefer to call it once outside the hot path or to specialize with `static_val` arguments.

#### F2. `invoke()` arguments that are constants but aren't `static_val`

If a helper has a parameter that is fixed across all calls in a function, a `static_val<T>` argument lets the trace specialize the helper-call site to a known constant — or, with the inlining plugin, lets LLVM constant-fold inside the inlined body.

#### F3. Forgetting `FunctionAttributes` for pure helpers

`include/nautilus/common/FunctionAttributes.hpp` lets you mark a `Call` as pure / readonly. Pure marks let DCE delete the call when the result is unused, and let CSE deduplicate identical calls. Use them when truthful.

### G. Composite types (`val<ClassType>` and `val_std`)

#### G1. Hand-rolled struct field access instead of `get`/`set`

See A3. The Member-pointer overload (`val_ptr.hpp:264`) produces a typed `Load` / `Store`. Hand-rolled offset math doesn't.

#### G2. Reaching for `val_std` types only when needed

`include/nautilus/val_std.hpp` and the `std/` plugin provide traced wrappers for std containers / strings. Use them when the operation set you need is supported (random access, length, iteration). For raw buffers with a known stride, keeping plain `val<T*>` + size is often simpler and traces just as well — pick based on what your kernel actually does, don't reach for the wrapper just for style.

### H. Diagnostic workflow

#### H1. When a finding is non-obvious, ask the user to dump the IR

`Engine` accepts options (`include/nautilus/options.hpp`) — `dump.ir`, `dump.after_ssa`, `dump.after_constant_folding`, `dump.after_empty_block_elim`. The same directory layout is mirrored under `nautilus/test/data/<category>/` so you can diff a problematic kernel against a known-good test for the same shape.

#### H2. Cross-check against `nautilus/test/data/regressions/`

Many subtle bugs are catalogued there with the exact problematic trace. If a reviewed kernel matches the shape of an existing regression (e.g., the GH#90 implicit-store-narrowing case), reference the regression file in the finding.

## Reporting Format

Output a structured report. Be terse — one block per finding. Group by **severity**:

```
## Critical (escapes traced domain — IR is incorrect or missing operations)
- <file:line> — <smell, one sentence>
  Effect: <which IR pass / backend feature is defeated>
  Rewrite: <minimal patch>

## Major (traces but blocks optimization)
- …

## Minor (style / micro-opt)
- …

## Diagnostic suggestions
- <e.g., "Enable dump.after_ssa to confirm the loop didn't fold">
```

If the reviewed code has **no findings**, say so explicitly and call out two or three patterns the code uses *well* (e.g., correct use of `static_val`, clean `select`, explicit casts on narrowing stores). Do not invent issues.

## What this skill is NOT

- Not a build/CMake/formatting reviewer — defer to `./format.sh` and `cmake --build . --target format`.
- Not a generic C++ style reviewer — naming, comments, file layout are out of scope unless they obscure traced semantics.
- Not a tool for changing IR phases or backend code — that's compiler-internal work and gets a normal C++ review, not this skill.
- Not authoritative about whether a transformation will *actually* fire on a given kernel without checking the dumped IR. Always frame findings as "this *should* enable X — confirm with `dump.after_ssa`" when uncertain.

## Quick reference: signal words to grep for during review

When skimming a diff, these tokens often indicate a finding:

| Search                                         | What it might be hiding                                       |
| ---------------------------------------------- | ------------------------------------------------------------- |
| `(int*)`, `(uintptr_t)`, `reinterpret_cast`    | Escape from traced domain (A1, A3, E1).                       |
| `static_cast<int*>` from a `val<T*>`           | Same.                                                         |
| `int <name> = 0;` near `val<int*>` array reads | Untraced accumulator (A2).                                    |
| `for (val<int…> i = 0; i < <literal>;`         | Should it be `static_val`? (B1, B2).                          |
| `while (`                                      | Often a B3 candidate.                                         |
| `if (…) { ret = a; } else { ret = b; }`        | C1 — replace with `select`.                                   |
| `&&`, `\|\|` between two `val<bool>`           | C6 — confirm short-circuit is intentional.                    |
| `invoke(` inside a `for`                       | F1 — does the helper carry `NAUTILUS_INLINE`?                 |
| `*ptr = …` where the RHS is a wider `val<T>`   | D3 — explicit cast missing.                                   |
| `setIsTrueProbability` absent on a hot guard   | C2 — opportunity, not a bug.                                  |

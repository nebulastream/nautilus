# Nautilus memory buffers — design

> **Status: proposal.** No code exists yet. This document fixes the shape of the
> abstraction, the semantics it must guarantee, the code it must generate, and a staged
> plan that can be executed independently. Section [§9](#9-is-this-just-stdspan) answers the
> question this design gets asked most: *how is this different from `std::span`?*

Related reading: [`docs/pointers.md`](../pointers.md), [`docs/static-val.md`](../static-val.md),
[`docs/region.md`](../region.md), [`docs/simd.md`](../simd.md),
[`nautilus/include/nautilus/val_std.hpp`](../../nautilus/include/nautilus/val_std.hpp),
[`plugins/std/include/nautilus/std/vector.h`](../../plugins/std/include/nautilus/std/vector.h).

---

## 1. Why

Everything Nautilus can say about a block of memory today is said with a bare `val<T*>`.
That is enough to be Turing-complete and not enough to be an abstraction: a pointer carries
no length, no alignment, no element layout, no aliasing promise, and no statement about who
owns the bytes or when they die. Every user re-derives those four facts, and each of them
does it slightly differently:

* **Length travels beside the pointer, by convention.** `f(val<int32_t*> data, val<uint64_t> n)`.
  Nothing checks that they belong together, and nothing lets the compiler know that `n` is a
  loop bound over `data`.
* **Layout is re-derived per call site.** `val<std::vector<T>>` probes libstdc++'s three
  pointer fields at static-init time and hand-rolls three `load_*_ptr()` helpers
  ([`vector.h:24-88`](../../plugins/std/include/nautilus/std/vector.h)). That trick is good
  and it is copy-pasted; the next runtime type that wants it copies it again.
* **Stack storage is a different mechanism from heap storage.** `val<Struct>` allocates via
  `traceAlloca` ([`val_std.hpp:140`](../../nautilus/include/nautilus/val_std.hpp)); a
  runtime-owned block arrives as a pointer argument. The two have nothing in common at the
  API level even when the traced code that reads them is identical.
* **Address math is written by hand and is easy to get wrong.** `val<T*>::operator[]` casts
  the index to `val<int32_t>` ([`val_ptr.hpp:326`](../../nautilus/include/nautilus/val_ptr.hpp)),
  which silently truncates any index past 2³¹ elements and forces a sign-extend into the
  generated address computation. Anyone who cares writes `ptr + i` with a `val<uint64_t>`
  themselves.

The abstraction this document proposes is one type that carries those four facts, is free at
run time, and is the *same* type whether the bytes came from a host allocator, a foreign
runtime object, or the generated function's own stack frame.

### 1.1 Requirements

1. A buffer's **bytes** may live in host-runtime memory or in the generated function's stack frame.
2. A buffer's **descriptor** may live on the Nautilus side (SSA values) or on the runtime side
   (fields of a C++ object that must be loaded).
3. Elements may be a **native Nautilus type** (`i8`…`f64`, `bool`, pointers) or a **C++ struct**,
   including structs whose layout is only known at trace time.
4. There is a **common API on the Nautilus side** and a **companion API on the runtime side**.
5. It must be **efficient**: the descriptor costs nothing at run time, and the generated code
   is what a competent C programmer would have written by hand.

---

## 2. The two axes people conflate

Requirements 1 and 2 read like one axis and are two. Separating them is most of the design.

**Axis A — where the bytes live** (*provenance*): host runtime, or the JIT function's stack.
This decides who allocates, who frees, and what the compiler may assume about the address.

**Axis B — where the descriptor lives** (*staging*): the data pointer, length and stride are
either SSA values the JIT already holds, or fields inside a runtime object that have to be
loaded from memory before every use.

|                          | **descriptor staged** (Nautilus side)                | **descriptor resident** (runtime side)                        |
| ------------------------ | ---------------------------------------------------- | ------------------------------------------------------------- |
| **bytes: host runtime**  | the common case — `(ptr, len)` arrive as arguments    | a `TupleBuffer` / `std::vector` / `std::span` object is passed |
| **bytes: Nautilus stack**| `stack_buffer<T, N>` — one `alloca` in the prologue   | only when a descriptor must be handed to an `invoke` callee    |

All four quadrants are real, and three of the four want *identical* traced code. That is the
lever: make one staged descriptor type the currency, and make every quadrant a way of
*producing* one.

---

## 3. Core idea: the descriptor is staged, not stored

> **A Nautilus buffer is a span whose descriptor lives in the compiler instead of in memory.**

`buffer<T>` is a host-side C++ object, held by the tracer's own stack during tracing, whose
members are traced values:

```cpp
template <typename T, typename Extent = dyn_extent, typename Stride = static_stride<sizeof(T)>,
          typename Hints = no_hints>
class buffer {
    val<T*> data_;
    [[no_unique_address]] Extent size_;    // val<uint64_t>, static_size<N>, or a static_val binding
    [[no_unique_address]] Stride stride_;  // empty type when the stride is a trace-time constant
};
```

Three consequences fall out, and they are the whole value proposition:

1. **The descriptor never exists at run time.** There is no `{ptr, len}` struct in the frame,
   no load of a length before a loop, no register pressure from carrying a size that was
   constant all along. `sizeof(buffer<T>)` is a *host* fact; the generated function sees only
   the fields it actually uses.
2. **Every field is independently static or dynamic.** A `static_size<1024>` extent is an empty
   member and the loop bound is a literal. A `val<uint64_t>` extent is one SSA value. Mixing is
   free, and `if (buf.size() > 0)` is a host-side `if` in the static case and a traced branch in
   the dynamic case — partial evaluation, without the user writing `if constexpr`.
3. **The natural spelling is the fast one.** `buf[i]` can do its address math in `uint64_t`,
   fold `i * sizeof(T)` when both are trace-time constants, and reuse an induction pointer in a
   loop, because `buffer` owns the arithmetic instead of forwarding to `val<T*>::operator[]`.

The cost of that staging is the thing to keep honest about: **the descriptor is a compile-time
object, so its shape is part of the compiled artifact's identity.** Two different static extents
are two different traces and two different compiled functions. §7.6 and §8.5 make that explicit.

---

## 4. Type surface

### 4.1 `buffer<T>` — the view everything converges to

Non-owning, like `std::span`. Copyable at trace time. This is what user code takes as a
parameter and what every storage flavour converts to.

```cpp
using nautilus::buffer;
using nautilus::static_size;

val<int64_t> sum(buffer<int32_t> b) {                  // dynamic extent
    val<int64_t> acc = 0;
    b.for_each([&](val<int32_t> x) { acc += x; });
    return acc;
}

val<int64_t> sum16(buffer<int32_t, static_size<16>> b); // extent is a literal; loop unrolls
```

Construction is explicit, never implicit from a pointer — an unattached pointer has no length
and adopting one is a claim the user must make:

```cpp
auto b = buffer<int32_t>::unsafe_from(ptr, n);   // "I promise ptr covers n elements"
```

### 4.2 Storage flavours

| Type | Bytes | Descriptor | Frees | Notes |
| --- | --- | --- | --- | --- |
| `buffer<T, …>` | anywhere | staged | never | the view; non-owning |
| `stack_buffer<T, N>` | JIT stack frame | staged | frame exit | one `traceAlloca(sizeof(T)*N, alignof(T))` |
| `runtime_buffer<T>` | host allocator | staged | scope exit (traced release) | RAII against the runtime hook table (§6.1) |
| `adopted<H>` | host runtime | resident → staged | never | a foreign handle `H` plus `buffer_traits<H>` (§6.2) |

Each yields a `buffer<T>` by `.view()` (and implicitly, for the owners, wherever a `buffer<T>`
parameter is expected). Only the owner types have destructors; the view has none, so passing a
view never emits a release.

`stack_buffer` needs **no new tracing operation**: `traceAlloca` already takes a host-side
`size_t`, so an array allocation is `traceAlloca(sizeof(T) * N, alignof(T))`. The extent must be
a trace-time constant, which is exactly the constraint that makes the alloca legal in the
prologue. (A dynamically sized stack buffer would need a variable-length `ALLOCA` in the IR —
out of scope; §12.)

### 4.3 Extents

| Extent | Trace ops | Generated code | Specializes per value? |
| --- | --- | --- | --- |
| `static_size<N>` | 0 | literal bound; loops unrollable | no — `N` is in the type |
| `static_val<uint64_t>` binding | 0 (snapshot only) | literal bound | **yes** — one artifact per size |
| `val<uint64_t>` (default) | 1 | SSA value; runtime trip count | no |

The `static_val` form is the JIT-specialization lever: a size that is a *run-time* value of the
host program becomes a *compile-time* constant of the generated program. It participates in the
trace snapshot (`pushStaticVal`), so each distinct size is a distinct trace — which is the
point, and also the hazard (§8.5).

### 4.4 Element typing

**Native types** (`is_arithmetic`, `bool`, pointers): `b[i]` yields `val<T&>`, so reads and
writes both work — the same proxy `val<T*>::operator*` already returns.

**C++ structs** (trivially copyable, standard layout): `b[i]` yields a `val<T*>`-backed element
proxy, matching what `val<std::vector<Struct>>::operator[]` already does. Field access reuses
`field_offset()` ([`val_ptr.hpp:252`](../../nautilus/include/nautilus/val_ptr.hpp)), so every
offset is a host-side constant folded into the address:

```cpp
struct Row { int64_t key; double value; uint8_t flags; };

buffer<Row> rows = …;
val<int64_t> k = rows[i].get(&Row::key);
rows[i].set(&Row::flags, uint8_t{1});
```

Non-trivially-copyable element types are rejected by a `static_assert`. A buffer's bulk
operations move bytes; running N constructors or destructors through `invoke` in a traced loop
is a different feature with different costs, and it should be spelled differently
(`construct_at` / `destroy_at` helpers) rather than hidden behind `copy_from`.

**Field views (AoS → column).** The single most useful thing a buffer can do that a pointer
cannot: project one field of a struct buffer into a strided buffer of that field's type.

```cpp
auto keys = rows.field(&Row::key);   // buffer<int64_t, dyn_extent, static_stride<sizeof(Row)>>
keys.for_each([&](val<int64_t> k) { … });   // load i64 at base+0, step 24 — LLVM sees a stride
```

The result is an ordinary `buffer` with a non-unit stride, so every algorithm written against
`buffer` works on a column without knowing it. This is the case `std::span` structurally cannot
express (§9).

**Trace-time layouts.** When the row layout is data (a query schema, a serialization format),
the offsets are ordinary host values at trace time and therefore constants in the generated
code:

```cpp
struct field_desc { size_t offset; size_t stride; };

auto col = rows_bytes.column<int64_t>(schema.field(3));  // offset/stride from a host object
```

This is where the buffer earns its keep in a query engine: one abstraction covers
`struct`-typed and schema-typed rows, and neither pays for the flexibility at run time. The
operational consequence — the compiled artifact is specific to that schema, so the JIT cache
key must include it — is in §8.5.

### 4.5 Access hints

Hints are part of the type, so they are free and so they can reach the backend:

```cpp
buffer<float>::aligned<64>::noalias        // → buffer<float, dyn_extent, …, hints<64, noalias>>
auto v = b.with_align<64>();               // builder form; returns a differently-typed view
```

| Hint | Meaning | Backend use |
| --- | --- | --- |
| `align<A>` | base address is `A`-byte aligned | aligned loads/stores, SIMD without a scalar peel |
| `noalias` | no other buffer in the function writes these bytes | LLVM `noalias` on the parameter |
| `readonly` | traced code never writes it | LLVM `readonly`; enables hoisting across calls |
| `nonnull` / `dereferenceable(n)` | derived from a static extent | LLVM parameter attributes |

Hints are **promises, not checks**. A `noalias` violation is UB in the generated code exactly as
`restrict` is in C. Debug builds can turn `align` into a traced assertion; §12 asks whether they
should by default.

---

## 5. The Nautilus-side API

One table, because the point of a general abstraction is that the list is short and total.

| Group | Operations | Lowers to |
| --- | --- | --- |
| Observers | `size()`, `size_bytes()`, `empty()`, `data()`, `stride()`, `alignment()` | nothing, or one existing SSA value |
| Element access | `operator[](i)`, `at(i)` (checked), `front()`, `back()`, `ptr(i)`, `ref(i)` | one `ADD` (+ one `MUL` if dynamic), one `LOAD`/`STORE` |
| Sub-views | `subspan(off, n)`, `first(n)`, `last(n)`, `chunk(n)`, `split_at(i)` | pointer `ADD`; extent arithmetic, often at trace time |
| Reinterpretation | `as_bytes()`, `cast<U>()`, `column<U>(desc)`, `field(&S::m)` | pointer cast; offsets/strides fold to constants |
| Iteration | `for_each(f)`, `for_each_index(f)`, `iterate<Unroll>(f)`, `begin()/end()` cursors | induction pointer + compare (§7.2) |
| Bulk | `copy_from(other)`, `fill(v)`, `zero()`, `equal(other)` | `memcpy`/`memset`/`memcmp`, or unrolled stores when small and static |
| SIMD | `load_vec<W>(i)`, `store_vec(i, v)`, `vectorize(body, tail)` | `val<vec<T>>` load/store from the SIMD plugin |
| Ownership (owners only) | `release()`, `grow(n)`, `pin()/unpin()`, `view()` | traced calls into the runtime hook table |
| Materialization | `to_runtime()` | writes `{ptr,len}` into an `alloca`, yields `val<layout*>` |

Two API decisions worth stating rather than assuming:

**Range-`for` works, and is not the recommended form.** `begin()/end()` return pointer cursors,
and `!=` on `val<T*>` yields a `val<bool>` that converts to a native `bool` through
`traceBool` — so `for (auto x : buf)` traces a loop correctly. It is still worse than
`for_each`, because the induction variable is a pointer the user can also mutate, which
defeats the stride-folding in §7.2. Provide both; document `for_each` as the fast path. Note
that cursor comparison must use `!=`/`==`, never a pointer-to-`bool` conversion — `val_ptr.hpp`
deliberately omits `operator bool()` because the null test can be split from the dereference it
guards and miscompile at `-O3`.

**Checked access is opt-in and vanishes when it can.** `at(i)` on a static extent with a
`static_val` index is a host-side `assert` and emits nothing. On a dynamic extent it emits a
compare and a branch to a trap or a throw. `operator[]` is never checked.

---

## 6. The runtime side

### 6.1 The host API and the hook table

The host program allocates, fills and hands over buffers with a small RAII type that is a plain
C++ object — no tracing involved:

```cpp
namespace nautilus::rt {
    template <typename T> class buffer {          // owns bytes; aligned allocation
        T* data(); size_t size();
        static buffer alloc(size_t n, size_t align = alignof(T));
    };
}

auto in  = rt::buffer<int32_t>::alloc(1024, 64);
auto out = rt::buffer<int32_t>::alloc(1024, 64);
auto f = engine.registerFunction(kernel);
f(in, out);                                        // expands to (ptr, len, ptr, len)
```

Everything the traced side can ask a runtime to *do* goes through one hook table, so a host
runtime (NebulaStream's buffer manager, an arena, plain `malloc`) is plugged in once:

```cpp
struct buffer_hooks {
    void* (*allocate)(void* ctx, size_t bytes, size_t align);
    void  (*release)(void* ctx, void* p, size_t bytes);
    void* (*grow)(void* ctx, void* p, size_t old_bytes, size_t new_bytes, size_t align);
    void* (*pin)(void* ctx, void* handle);      // optional: spillable buffers
    void  (*unpin)(void* ctx, void* handle);
};
```

Each hook is called through `invoke` with explicit `FunctionAttributes`
([`common/FunctionAttributes.hpp`](../../nautilus/include/nautilus/common/FunctionAttributes.hpp)),
which is not decoration: `release` is `ModRef`, but `pin` on an already-pinned buffer is
`Ref + willReturn + noUnwind` and can therefore be hoisted out of a loop or deleted when its
result is unused. Getting these right is a large part of the "efficient" requirement, because a
single unannotated call in a loop body is a memory barrier for every buffer in scope.

### 6.2 Adopting a foreign handle: `buffer_traits<H>`

For quadrant 2 — a runtime object that *is* a buffer — the trait says how to reach the two
fields, and the buffer machinery does the rest:

```cpp
template <> struct nautilus::buffer_traits<NES::TupleBuffer> {
    static val<int8_t*>   data(val<NES::TupleBuffer*> h);
    static val<uint64_t>  size(val<NES::TupleBuffer*> h);
    static constexpr auto hints = hints_of<align<64>>;
};

buffer<Row> rows = adopt<NES::TupleBuffer>(handle).as<Row>();
```

A trait has two ways to implement `data()`, and both belong in the design:

* **Probed load** — read the field at a byte offset established once at static-init time, as
  `val<std::vector<T>>` already does. One `LOAD`, no call, fully transparent to the optimizer.
  Costs a layout assumption that must be validated at start-up.
* **`invoke` accessor** — call a real accessor function. Opaque, correct by construction,
  and a barrier unless annotated `Ref/willReturn/noUnwind`.

Provide helpers for both (`probe_field<&H::member>()` generalizing `probe_vector_layout`, and
`accessor<&H::data>()`), and let each trait pick. The std plugin then gets
`val<std::span<T>>` and `val<std::vector<T>>::view()` implemented as traits, which is how a
`std::span` sitting in runtime memory enters this world (§9.3).

### 6.3 The invalidation rule

This is the correctness rule of the whole design, and it must be stated where users will read it:

> **A staged descriptor is a cached copy of runtime state. Any traced call that could
> reallocate or release the buffer invalidates every view derived from it.**

Nautilus cannot see through an `invoke`, so it cannot invalidate views for you. The design
handles this in three layers:

1. `runtime_buffer::grow()` returns a *new* view and marks the old one dead; using the dead one
   is a trace-time error, not a silent stale pointer. (`val`-typed members make the check a
   host-side flag, so it costs nothing.)
2. Adopted handles are re-loaded per view construction, never cached across an unannotated
   call — `adopt(h).as<T>()` is cheap precisely because the loads fold when nothing intervenes.
3. `pin()` returns a `pinned_buffer` whose existence is the user's assertion that the address is
   stable for its scope; that is the only form for which caching across calls is legal.

---

## 7. What the generated code must look like

"Efficient" is not a property of an API; it is a property of the IR it produces. These are the
acceptance criteria, and each is testable with an LLVM-IR golden test.

### 7.1 Address math is `uint64_t` and folds

`buf[i]` with `T = int32_t` and a dynamic extent must produce exactly:

```llvm
%off = mul i64 %i, 4
%p   = getelementptr i8, ptr %base, i64 %off
%v   = load i32, ptr %p
```

No 32-bit truncation, no sign-extend — which is why `buffer` must not route through
`val<T*>::operator[]` ([`val_ptr.hpp:326`](../../nautilus/include/nautilus/val_ptr.hpp)). With a
`static_val` index and a static stride, the `mul` and the `getelementptr` are one literal offset
before the IR is even built, because both operands are host values at trace time.

### 7.2 Loops carry a pointer, not an index

`for_each` emits an induction pointer, so the body has one `add` and no multiply:

```llvm
loop:
  %p   = phi ptr [ %base, %entry ], [ %next, %loop ]
  %v   = load i32, ptr %p
  …
  %next = getelementptr i8, ptr %p, i64 24      ; stride: a literal, even for a field view
```

The stride is a constant for contiguous buffers *and* for field views, which is what lets LLVM
recognize the access pattern and vectorize it. `LoopInvariantCodeMotionPass` will hoist a loaded
base pointer, but it cannot invent an induction variable — so this has to be generated, not
optimized for.

### 7.3 Bulk operations are bulk operations

`copy_from`/`fill` route to `nautilus::memcpy`/`memset`, which today are `invoke`s of
`std::memcpy` with `noUnwind` attributes
([`api/val_memcpy.cpp:22`](../../nautilus/src/nautilus/api/val_memcpy.cpp)). For a small static
size, emit unrolled loads and stores instead — an 8-byte copy should not be a call. Whether the
emitted callee is named such that LLVM recognizes it as the `memcpy` libcall and lowers it to
`llvm.memcpy` needs to be verified rather than assumed; it is an explicit item in Stage 1.

### 7.4 Hints reach the parameter

A `buffer` parameter of the traced function **expands into ABI parameters** — `(ptr, len)`, or
just `ptr` for a static extent — and the expansion is where `align`, `noalias`,
`dereferenceable` and `readonly` are attached. This extends the entry-parameter attribute
mechanism added in #444, which currently derives attributes from the type stamp alone; buffers
need per-parameter metadata carried from the signature into `FunctionOperation`.

For buffers formed *inside* the body (adopted handles, subviews), there is no parameter to
annotate. Those need an assumption operation (`ASSUME_ALIGNED`, or an aliasing-scope marker) or
they get no hints. Parameter expansion first; assumption ops are Stage 4's stretch goal.

### 7.5 Checks that can be resolved are resolved

Static extent + `static_val` index → host-side `assert`, zero emitted code. Dynamic extent +
`at()` → one compare, one cold branch. `operator[]` → nothing, ever.

### 7.6 Tracing cost is part of the cost

A `buffer` is copied whenever it is passed by value, and copying a `val<T*>` emits a `COPY`
operation (`traceCopy`). These are cleaned up downstream, but they cost trace time and trace
memory, and tracing time is on the critical path of a JIT. Helpers take `const buffer&`; the
extent contributes zero ops when static; `for_each` takes its callable by reference.

---

## 8. Semantics and hazards

**8.1 Aliasing across calls.** Without `noalias`, LLVM must assume any traced call may write any
buffer. Two buffers of the same element type passed as separate parameters are assumed to
overlap. `copy_from` therefore uses `memmove` semantics unless the user opts into
`copy_from_disjoint`.

**8.2 One `alloca` per trace site.** `traceAlloca` is tag-checked, so a `stack_buffer` created
inside a traced loop yields **one** entry in the alloca table and **one** real stack slot,
reused by every iteration ([`Operations.hpp:50-55`](../../nautilus/include/nautilus/tracing/Operations.hpp)).
Nothing carries between iterations by accident — but two `stack_buffer`s that the user believes
are distinct because they were "created in different iterations" are the same memory. Document
it; it is the same rule C gives a function-scope array.

**8.3 Regions.** Under `lazyTracing`, no value created inside a region may outlive it
([`docs/region.md`](../region.md)). A `buffer` is a value, so a buffer produced inside a region
body and used after it is a trace-time error — declare the `buffer` outside and assign inside,
as with any `val`. Allocas inside a region body are traced against the region and memoized with
it, so `stack_buffer` inside a region is fine.

**8.4 Lifetime across the trace boundary.** `runtime_buffer`'s destructor runs during *tracing*,
at the point the host C++ object dies, and emits the release call there. That is the intended
semantics — one release per traced path — and it composes with symbolic exploration because each
pass re-runs the destructor. For exception safety, reuse the `registerDestructor` mechanism
`val<Struct>` already uses, so an unwinding path still releases.

**8.5 Specialization is a cache key.** A `static_val` extent, a trace-time schema, or a static
alignment all move information out of the run-time program and into the compiled artifact. The
engine's compilation cache must key on them, or two different schemas will share one compiled
function. This is not new — `static_val` has always had it — but a buffer makes it easy to do at
scale, and therefore easy to get wrong at scale.

**8.6 Interpreted mode.** With tracing off, `buffer` is a plain pointer and length and every
operation is a direct memory access — no engine involvement. `stack_buffer` must mirror what
`nautilus_alloca` does today: it falls back to an aligned `operator new` paired with an aligned
`operator delete` in the destructor. (The class comment in `val_std.hpp` describing a
thread-local `AllocationContext` arena no longer matches that code; the buffer implementation
should follow the code and the comment should be corrected.)

---

## 9. Is this just `std::span`?

Short answer: **the API is span-shaped and the semantics are not.** `std::span` is a run-time
object that describes memory. `buffer` is a compile-time object that describes memory *and
disappears*. Everything below follows from that one difference.

### 9.1 What we deliberately keep

`subspan`/`first`/`last`, the static-vs-dynamic extent distinction, non-owning by default,
implicit conversion from owning containers, `as_bytes`. Users know these names and the semantics
transfer exactly. Any place this design invents a different name for the same idea is a bug in
this design.

### 9.2 What is different

| | `std::span<T, N>` | `nautilus::buffer<T, Extent, Stride, Hints>` |
| --- | --- | --- |
| Descriptor at run time | a real `{ptr, len}` object; may be spilled, passed in registers, loaded | none — erased; only the fields the code uses survive, as SSA values |
| "Static extent" means | known when the *host* was compiled | known when the *kernel* was traced — a host run-time value can be a generated-code constant |
| Element access | `T&` | `val<T&>` proxy (native) or an element proxy with `get`/`set` (struct) |
| Stride | contiguous, always | contiguous *or* a trace-time constant stride — field views over AoS data |
| Layout knowledge | the C++ type | the C++ type **or** a trace-time layout descriptor (schema) |
| Codegen metadata | none possible | alignment / `noalias` / `readonly` / `dereferenceable` are in the type and reach LLVM |
| Ownership | never owns | the view never owns; sibling types own stack or runtime storage |
| Iteration | real iterators, ranges, algorithms | traced `for_each` / cursors; `std::ranges` cannot apply, because the loop is traced |
| Bounds checks | `at()` throws in-process | `at()` emits a branch into the generated program |
| Cost of a copy | register moves | trace-time work (a `COPY` op per pointer) — free at run time |

The two rows that matter most are the first and the third-from-last. A `std::span` in a hot loop
is *usually* free because the optimizer scalar-replaces it — but "usually" is a property of the
optimizer, and it stops being true the moment the span crosses a call boundary the optimizer
cannot see through. A staged descriptor is free *by construction*, and its constants are
constants before the IR exists, so constant-folding and unrolling happen on the smaller problem.

### 9.3 Why not simply use `std::span` in traced code

Three attempts, and why each fails:

* **`std::span<val<T>>`** — nonsense. `val<T>` is a tracing handle carrying SSA state, not a
  storage type; an array of them is an array of compiler objects, not the memory being described.
* **`val<std::span<T>>`** — coherent, and it is quadrant 2, not the abstraction. It describes a
  `std::span` object *living in runtime memory*, whose `ptr` and `len` must be loaded before use.
  This is worth having, as a `buffer_traits` specialization in the std plugin, so a host that
  already passes `std::span` around can hand one over. `.view()` converts it into a staged
  `buffer<T>`, and everything downstream is the fast path again.
* **A host-side `std::span` closed over by the traced lambda** — works for the *address* (it is a
  trace-time constant) and silently bakes the length in. Fine when intended, a bug when not;
  `buffer` makes the choice explicit by requiring `static_size` or a `static_val` binding.

### 9.4 Neighbours

`std::mdspan` is the closer relative for the strided case: layout-as-policy is exactly the
`Stride`/`Layout` parameter here, and if multi-dimensional buffers are ever needed, mdspan's
`layout_stride` is the model to copy rather than reinvent. This design deliberately stops at 1-D
plus stride, because that covers rows, columns and tiles, and mdspan's full layout/accessor
machinery would double the surface for the remaining cases. Rust's `&[T]` is the ownership model
being borrowed (a view is a borrow; owners are separate types); C's `restrict` is the semantics
of the `noalias` hint, UB and all.

---

## 10. Staged implementation plan

Each stage compiles, tests, and ships on its own.

**Stage 0 — the view.** `buffer<T, Extent, Stride, Hints>` for native `T`: observers, `operator[]`,
`ptr/ref`, `subspan/first/last`, `as_bytes`, `cast<U>`. Own address math in `uint64_t`.
*Done when*: execution tests cover static and dynamic extents against a raw-pointer baseline, and
an LLVM-IR golden shows the §7.1 shape with no 32-bit index cast.

**Stage 1 — iteration and bulk.** `for_each`, `for_each_index`, `iterate<Unroll>`, cursors,
`copy_from`, `fill`, `zero`, `equal`. Small static copies unrolled; large ones through
`memcpy`/`memset`. *Done when*: an IR golden shows the induction-pointer loop of §7.2, and the
question in §7.3 (does the emitted call reach `llvm.memcpy`?) is answered in the doc.

**Stage 2 — struct and schema elements.** Element proxies, `field(&S::m)` strided views,
`column<U>(desc)` for trace-time layouts, `static_assert` on non-trivially-copyable `T`.
*Done when*: an AoS sum over `field()` generates the same loop as a hand-written strided load,
and two different schemas produce two different traces.

**Stage 3 — stack storage.** `stack_buffer<T, N>` over `traceAlloca(sizeof(T)*N, alignof(T))`,
with the interpreted-mode `operator new`/`delete` fallback. *Done when*: a stack buffer in a loop
is shown to be one slot (§8.2), and alignment requests reach the alloca.

**Stage 4 — parameter expansion and attributes.** `buffer` as a traced-function parameter,
expanded to `(ptr, len)`; per-parameter `align`/`noalias`/`readonly`/`dereferenceable` carried
into `FunctionOperation` and emitted by the MLIR backend, extending #444. *Done when*: an IR
golden shows the attributes on the entry function and a `noalias` pair vectorizes where the
un-hinted pair does not.

**Stage 5 — the runtime side.** `rt::buffer<T>`, the hook table, `runtime_buffer` RAII with
exception-safe release, `buffer_traits<H>` with `probe_field` and `accessor` helpers, the
invalidation rule, `val<std::span<T>>` and `val<std::vector<T>>::view()` in the std plugin.
*Done when*: a foreign handle type is adopted end-to-end in a test with no changes to `buffer`.

**Stage 6 — SIMD.** `load_vec/store_vec/vectorize` over the SIMD plugin, with alignment from
hints and a scalar tail derived from the extent. *Done when*: a vectorized sum matches the
hand-written `val<vec<T>>` version in the benchmark suite.

---

## 11. Testing

* **Execution tests** per stage in `nautilus/test/execution-tests/`, run across all backends —
  the bytecode and C++ backends are the check that nothing depends on LLVM-specific folding.
* **Trace/IR goldens** under `nautilus/test/data/buffer-tests/{tracing,ir,after_ssa}`, matching
  the existing convention; these lock in *trace-time* cost (op counts), which is where a staged
  descriptor can silently regress.
* **LLVM-IR goldens** under `nautilus/test/llvm-ir-test/` for the four shapes that define
  "efficient": the §7.1 address, the §7.2 loop, the §7.4 attributes, and a static-extent loop
  that must fully unroll.
* **Benchmarks** comparing, for the same kernel: hand-written `val<T*>`, `buffer`, and native
  C++. The efficiency claim is only real once it is measured; a `buffer` loop that is slower
  than the pointer loop it replaces is a Stage-0 bug, not a tuning opportunity.

---

## 12. Open questions

1. **Naming.** `buffer` for a non-owning view invites the `std::span` misreading in reverse
   (people expect it to own). `buffer_view` + `buffer` (owning) is the honest split; it is also
   two names for what users will type constantly. *Recommendation*: keep `buffer` as the view,
   name the owners `stack_buffer`/`runtime_buffer`, and say "non-owning" in the first line of
   the docs — but this is worth one round of bikeshedding before Stage 0 fixes it.
2. **Bounds-check policy.** Trap, throw, or clamp? Throwing integrates with the existing
   exception machinery and is expensive; trapping is free and undebuggable in a JIT.
3. **Hint validation.** Should debug builds emit traced assertions for `align`/`noalias`
   violations, or is that too much emitted code for a promise-based API?
4. **Dynamically sized stack buffers.** Needs a variable-length `ALLOCA` in the IR and stack
   discipline in every backend. Deferred — is there a real use case, or does `runtime_buffer`
   cover it?
5. **Device memory.** The GPU plugin's buffers live in another address space. The `Hints`
   parameter is the natural place for a `memory_space` policy that changes how loads and stores
   lower. Worth reserving the slot now, not implementing it.
6. **Multi-dimensional buffers.** If tiles and matrices show up, adopt `mdspan`'s
   `layout_stride` rather than growing `Stride` organically (§9.4).

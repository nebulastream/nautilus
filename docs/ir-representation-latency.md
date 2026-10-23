# Design: Trace and IR Representation for Low Compile Latency

Status: proposal · Scope: `ExecutionTrace`, `TraceToIRConversionPhase`, `ir::*`,
IR passes, backend lowering front-ends · Companion to
[low-latency-compilation.md](low-latency-compilation.md). Tiering is out of
scope here.

## 1. Question

How much of the time from first call to machine code comes from *how* the
trace and the IR are represented: their layout, allocation, indirection and
side tables? Which changes to the representation would let a single-pass
backend (TPDE, Flying Start style) run at its potential?

Fast back ends need four things from the IR (see TPDE's IR adapter):

1. **dense value numbers**, so every side table is a flat array;
2. **cheap linear iteration** over blocks and operations;
3. **use counts or liveness** without hash maps;
4. **no per-operation heap allocation** on the path.

The sections below check the current representation against these four
needs.

## 2. What the representation looks like today

Sizes come from compiling a `sizeof` probe against the current headers with
clang 18 on x86-64.

### 2.1 Trace (`tracing/`)

| Object | Size | Storage |
|---|---|---|
| `TraceOperation` | 56 B | arena |
| one input (`InputVariant`, a `std::variant` of 8 alternatives) | 24 B | arena, next to the op |
| `TypedValueRef` | 8 B | inline |
| `Snapshot` (tag pointer + static-value hash) | 16 B | inline in each op |
| `BlockRef` (branch target + `std::vector` of arguments) | 32 B + vector | arena + **heap** |
| `FunctionCall` (two `std::string`s, argument and destructor vectors) | — | arena + **heap** |
| `Block::operations` / `arguments` / `predecessors` | `std::vector` | **heap** |
| `ExecutionTrace::globalTagMap` | `std::unordered_map<Snapshot, operation_identifier>` | **heap node per recorded op** |

So a traced binary `add` costs 56 + 2 × 24 = **104 B of trace**. On top of
that come a `push_back` into a heap vector, one hash-map node allocation, and
the tag:

- **Tag creation:** `TagRecorder` walks the frame-pointer chain and appends
  each return address into a trie (`tag/Trie.hpp`).
- **Trie lookup is a linear scan.** `TrieNode::append` scans the child array
  linearly. A node that has many distinct call sites below it, such as one
  `val` operator used at hundreds of call sites in a large function, makes
  each lookup O(children). Recording stays linear only while fan-out is
  small. This is a hypothesis to measure (§5).

### 2.2 IR (`compiler/ir/`)

| Object | Size | Notes |
|---|---|---|
| `Operation` header | 32 B | type, stamp, region, id, `std::span` of inputs, source tag; packed with no padding |
| `AddOperation` | 32 B (+16 B inputs) | Inputs are a separate bump allocation. They are adjacent in practice because they are allocated from the constructor |
| `ConstIntOperation` | 40 B | |
| `BranchOperation` | 80 B | embeds one `BasicBlockInvocation` (48 B) |
| `IfOperation` | 144 B | embeds two `BasicBlockInvocation`s |
| `CallOperation` | 152 B | holds **two `std::string`s** (symbol, name) plus a destructor vector, although the `FunctionTable` already interns the callee by `FunctionId` |
| `BasicBlock` | 88 B | three heap `std::vector`s: operations, arguments, predecessors |
| `FunctionOperation` | — | `std::string` name, vectors, a `std::unordered_map` of attributes |

Identifiers: an IR value's `OperationIdentifier` is the trace's `ValueRef`.
Those are numbered densely per trace. `FunctionRewriter::freshId()` continues
from `max + 1`. Void operations (store, branch, if, return) all share id `0`.
**The ids are therefore almost dense already, but nothing guarantees it, and
no consumer relies on it.**

### 2.3 How consumers access the IR

| Consumer | Side structures | Cost |
|---|---|---|
| Trace → IR (`TraceToIRConversionPhase`) | `valueStates` is a `std::vector` indexed by `ValueRef`; its own open-addressing table | **Already dense and linear.** This is the model to follow |
| Every IR pass | builds a `FunctionRewriter`, whose external use table is a `std::pmr::unordered_map<const Operation*, std::pmr::vector<Use>>`, built in O(function) | Rebuilt **per pass, per round**: up to 7 passes × `ir.maxPipelineIterations` (4) = **28 full hash-table builds** per function |
| `IRPassManager` | clears and rebuilds predecessor lists each run | Another full walk per run |
| `asmjit` / `bc` / `tbc` / `cpp` / `mlir` lowering | `Frame<OperationIdentifier, V>` = `std::unordered_map`. Usage counts (`ir::countUsages`), cross-block sets and block-label maps are also `unordered_map`/`unordered_set` | A **hash lookup per operand and per definition**, plus at least one extra full walk per backend to count uses |

Pointer chasing per operand: block → `std::vector<Operation*>` (heap) →
`Operation` (arena) → `inputs` span → input `Operation` → its id → hash
lookup in the backend frame. That is about five dependent loads and a hash
probe for every operand read.

## 3. Where this costs latency

These are hypotheses, ordered by expected impact. Each gets a measurement in
§5 before any change is made.

1. **Hash maps in the per-operand hot path of every backend.** A
   single-pass backend's whole budget is a handful of nanoseconds per
   instruction. An `unordered_map` probe per operand (with a node allocation
   per definition) can be the dominant cost of lowering.
2. **Rebuilding pass-local use tables.** The fixed-point pipeline pays for
   up to 28 O(n) hash-table builds before a backend runs.
3. **Per-operation heap allocations during tracing.** The tag-map node,
   vector growth and `std::string` copies for calls go through `malloc`.
   Tracing replays the function once per path, so this cost scales with
   paths × length.
4. **Two full materializations.** Every operation is first written as a
   104 B trace record, then rebuilt as a 32–152 B IR object. The trace stays
   alive until conversion finishes.
5. **Redundant per-backend analysis.** Use counts, cross-block liveness and
   block numbering are recomputed by each backend from scratch.

## 4. Design

The design keeps the pointer-based, mutable, arena-allocated IR that the
passes are built on. It adds **dense numbering as an invariant** and a
**frozen, array-based lowering view** for the backends. Data-oriented
storage is used wherever a structure is rebuilt per compile.

### 4.1 Dense value and block numbering as an IR invariant

- Each `FunctionOperation` carries `valueCount` and `blockCount`. Every
  value-producing operation and every `BasicBlockArgument` has a unique id in
  `[0, valueCount)`. Void operations keep a sentinel and never index a value
  table.
- Trace → IR keeps the trace `ValueRef` as the id; it already does. The
  conversion records the final count.
- `FunctionRewriter::freshId()` bumps `valueCount` instead of a private
  counter, so passes preserve the invariant.
- Block ids become dense per function in the same way.
- `IRVerifier` checks uniqueness and range.

With this in place every side table becomes a flat array.

### 4.2 Replace `Frame<OperationIdentifier, V>` with a dense value table

```cpp
template <class V>
class ValueTable {          // drop-in for Frame<OperationIdentifier, V>
	std::vector<V> values_;  // indexed by OperationIdentifier::getId()
	std::vector<bool> defined_; // or a sentinel V
	...
};
```

`getValue`, `setValue` and `contains` keep their signatures, so all backends
(`bc`, `tbc`, `asmjit`, `cpp`, `mlir`) switch over mechanically. Do the same
for:

- `usageCounts`;
- `crossBlockIds` / `functionArgs` (`bc`);
- `blockLabels` / `processedBlocks` (`asmjit`), which become arrays indexed
  by block id;
- `inlinedConstParams_` and `definedValues`, both keyed by `Operation*`,
  which switch to the id.

This is the cheapest change with the broadest effect. It touches no IR
semantics.

### 4.3 A frozen lowering view (the backend IR adapter)

After the pass pipeline, build one compact, read-only, per-function view in
a single linear pass. Backends consume this view instead of walking the
mutable graph.

```
struct LoweringView {
	std::span<const BlockInfo> blocks;    // in layout order (RPO, loops contiguous)
	std::span<const Operation*> ops;      // all ops, block-contiguous
	std::span<const uint32_t> useCount;   // by value id
	std::span<const uint32_t> lastUseBlock; // by value id; liveness (Kohn et al., linear)
	std::span<const uint32_t> loopDepth;  // by block
	// BlockInfo: first/last op index, successors, argument range
};
```

- It is computed **once** and shared by every consumer. It replaces
  `ir::countUsages`, `TBCLoweringProvider::countAllUsages` and the
  cross-block analysis in `bc`.
- It matches the TPDE `IRAdaptor` contract exactly: values numbered densely,
  blocks in a given order, successors, phis (block arguments) and operands.
  It is therefore also the integration point for a TPDE-style backend.
- It lives in the IR arena next to the graph and is invalidated by any
  later mutation. A debug-mode generation counter on `FunctionOperation`
  catches stale use.

### 4.4 Use lists that survive across passes

There are two options:

- **(a) A use table per pipeline run.** Lift the `FunctionRewriter`
  use-table session from "per pass" to "per `IRPassManager::run`". With
  dense ids the table is a `std::vector<UseRange>` indexed by value id,
  backed by one bump buffer. Passes already route every mutation through the
  rewriter, so the contract is unchanged. Only the session's lifetime grows.
- **(b) Intrusive use lists in `Operation`.** These would push `Operation`
  past 32 B. The class comment shows the 32 B layout is deliberate, so (b) is
  rejected.

**Recommendation: (a).** It removes up to 27 of 28 table builds per function
and costs no IR memory.

### 4.5 Arena-backed block and function containers

- Replace the three `std::vector`s in `BasicBlock` (operations, arguments,
  predecessors) with an arena vector, a small growable array that allocates
  from `IRGraph::getArena()`. Growth abandons the old buffer to the arena,
  which is the policy `BasicBlockInvocation` already uses.
- Do the same for `FunctionOperation`'s vectors. Keep the attributes as a
  sorted small array, which suits their 0–5 entries better than a hash map.
- `CallOperation`: drop the two `std::string`s. The callee is already
  interned in the `FunctionTable` (`calleeId`), so names resolve through it
  only when dumping. This shrinks the op and removes two string copies per
  call site.

### 4.6 A slimmer trace record

The trace is written on every replayed path, so per-record cost is
multiplied. Proposed changes:

- **Operands as 4 B `ValueRef`s.** Today each operand is a 24 B
  `InputVariant`. Constants, call descriptors, block refs, branch
  probabilities and alloca indices move to typed side arrays on the trace,
  and the operand slot holds an index into them. This takes a binary op from
  104 B to roughly 32–40 B.
- **`Snapshot` out of the record.** It is needed only for the tag map and
  for source attribution. Keep the 8 B tag pointer, and put the
  static-value hash only into the map key.
- **`globalTagMap` as an open-addressing table in the trace arena**, like
  the one in `TraceToIRConversionPhase.cpp`. It needs no per-insert
  allocation.
- **Trie fan-out.** Above a threshold (about 8 children), switch a node's
  children to a small open-addressing table. This keeps lookup O(1) for
  functions with many call sites.
- **`Block` containers in the arena** (as in §4.5).
- **Call names.** `FunctionCall` stores the callee identity (pointer +
  `CalleeKind`). Names are resolved lazily, which `engine.resolveFunctionNames`
  already does by default.

Recording IR directly during tracing, which would skip the trace, is
**not** proposed. The trace CFG is finished only after tracing: loop
headers are detected by snapshot merges, and blocks are split and region
seams collapsed afterwards. Braun-style SSA construction relies on a
complete CFG for sealing. A slim trace plus the already-linear conversion
gets most of the benefit at far lower risk.

### 4.7 Compact control-flow operations

`IfOperation` is 144 B because it embeds two 48 B `BasicBlockInvocation`s,
each a full `Operation`. An invocation needs a target block and an argument
span (24 B), not an `Operation` header with an id, tag and region. Making
the invocation a plain struct roughly halves both `If` and `Branch`. There
is a catch: generic passes find block arguments through
`Operation::getInputs()` on the invocation (see the comment in
`BasicBlockInvocation.hpp`), so this change must come with an equivalent
generic operand iterator. It is therefore lower priority than §4.1–4.6.

## 5. Measurement plan

The first deliverable is numbers, not code. Use the existing benchmarks
(`TracingBenchmark`, `IRPassBenchmark`, `ExecutionBenchmark`) plus a corpus
of large generated functions: 1k / 10k / 100k operations, straight-line,
branchy and with many call sites.

For each stage, report:

- **ns per operation:** trace record, trace → IR, each pass, each backend's
  lowering;
- **heap allocations per operation,** counted with a counting
  `operator new` in the benchmark binary;
- **peak bytes** of the trace arena and of the IR arena;
- **perf profile share** of hash-map functions (`std::_Hashtable`,
  `std::hash`) and of `TrieNode::append`.

Each hypothesis in §3 is confirmed or rejected before the change that
addresses it is merged. Changes land in this order: §4.2 and §4.1 first
(they are mechanical and isolated), then §4.4, §4.3, §4.6, §4.5, §4.7.
Expected outcomes to verify:

- backend lowering without hash probes;
- passes with one use-table build per pipeline run;
- tracing with no per-operation `malloc`;
- trace memory roughly 2–3x smaller.

## 6. Risks

- **Dense-id invariant drift.** A pass that mints ids outside the rewriter
  breaks every table. The verifier check and the rewriter-only rule mitigate
  this.
- **Stale lowering view.** The view must be built after the last mutation.
  Enforce this with the generation counter (§4.3).
- **Trace format churn.** Many trace consumers (tests' `tracing/` reference
  dumps, `CopyAttribution`, region handling) read `InputVariant` directly.
  §4.6 needs an accessor layer first so the storage can change underneath.
- **Gains may be small for small functions.** For short queries, fixed
  costs such as backend setup can dominate. §5 decides whether §4.5–4.7 are
  worth their churn.

---

# Part II: What to change if semantics may change

Part I keeps every observable behavior. This part asks the opposite: which
*semantic* commitments cause the latency in the first place, and what should
replace them? Two commitments stand out: tracing control flow implicitly,
and an IR that is pessimistic by default. Everything in Part I treats their
symptoms.

## 7. Root causes in today's semantics

| Commitment | What it forces |
|---|---|
| **Implicit control flow.** `if (val<bool>)` and `for (val<…>)` are plain C++, and the tracer discovers branches and merges by itself | The function must be re-executed **once per path** (`runScope` replay). Every operation must carry a call-stack **tag** (a frame-pointer walk plus a trie insert) and a **snapshot hash**, and must be looked up in `globalTagMap`, because *any* operation could be a merge point. `static_val` exists only to perturb that hash. The CFG comes out full of seams, which is why block merging, empty-block elimination, argument pruning and branch threading in the conversion exist |
| **Mutable CFG with per-op identity.** Every op has its own id, tag and region, and passes rewrite in place | Optimizations happen in separate passes run to a fixed point, with use tables rebuilt each time. Nothing can be deduplicated when it is created |
| **Pessimistic call semantics.** `FunctionAttributes` defaults to `ModRef`, `willReturn=false` and `noUnwind=false` unless the callee is `noexcept` | Exception regions, landing pads and `ExceptionRegionPreparationPass` are needed, plus `NoThrowInferencePass` and `FunctionAttributeInferencePass` just to recover facts. Every call is a barrier for CSE and hoisting |
| **Signedness in value types.** `Type` has `i32` and `ui32` and so on | No-op cast operations, and folding, verifier and opcode tables over 10 integer types instead of 4 widths (tbc expands every numeric family over all 10) |

## 8. Proposed semantic changes

### S1. Explicit join points: a structured tracing mode (deferred follow-up)

> **Decision:** implicit, trace-based control-flow discovery stays. S1 is a
> possible follow-up API on top of the representation in Part III, not part
> of this design. Part III is built so that S1 can be added later without
> changing the formats.

Add a tracing mode in which control flow on traced values is written with
combinators. JAX made the same choice (`lax.cond`, `lax.while_loop`,
`fori_loop`): branching in Python on a traced value is an error there,
which lets JAX trace once, in linear time. Nautilus already has one such
construct in `select`.

```cpp
auto r = nautilus::if_(x > 0, [&] { return x * 2; }, [&] { return x - 1; });

nautilus::while_([&] { return i < n; },
                 [&] { sum = sum + data[i]; i = i + 1; });

nautilus::for_(0, n, [&](val<int64_t> i) { sum = sum + data[i]; });
```

Semantics:

- **Each lambda body is traced exactly once.** The tracer opens the
  then/else/header/body blocks *before* running the lambda, so the CFG is
  known while it is being built.
- **Mutated variables become block arguments.** A `val` assigned inside a
  body is a variable (an `ASSIGN` target). The existing Braun-style SSA
  construction turns it into block arguments, so the current variable
  model works unchanged.
- **Branching on a `val<bool>` directly is an error** in this mode
  (`explicit operator bool` with a diagnostic naming the combinator to
  use). Alternatively it falls back to replay for that scope (see
  "Migration" below).
- **Plain C++ control flow unrolls.** A C++ `for` over `int` simply runs and
  emits its iterations. **`static_val` is unnecessary** in this mode, and so
  is the static-value hash.
- **Early exits are values, not statements.** `break_()` and `return_()`
  are combinators that act on the enclosing construct, because a `return`
  inside a lambda cannot leave the traced function.

What this removes from the compile path:

- **Replay:** tracing takes one pass, and its cost is linear in program
  size instead of paths × length.
- **Tags, snapshot hashes and `globalTagMap` lookups per operation.** Merges
  are now explicit. A tag is captured only when source locations are
  requested (`debug`, `perf`), and only then.
- **Seam cleanup.** The trace has no seams for block merging, empty-block
  elimination or branch threading to remove.
- **Side effects at trace time run once.** Plain C++ code inside a traced
  function runs once per program point instead of once per explored path.
  That is a semantic simplification for users as well.

**Migration.** Make the mode selectable per `nautilus::function` and per
`region()`, and keep replay tracing as the compatibility mode. Then migrate
hot NebulaStream operators first and measure before changing the default.
The `nautilus-review` skill can flag direct branches on `val<bool>`.

### S2. A construction-time optimizing, structured IR

> **Without S1:** the trace CFG is complete only once tracing ends, so the
> optimizing builder runs during trace→IR conversion instead of during
> tracing, and the IR stays in CFG form (§13). Folding, scoped CSE and use
> counts all still apply there.

When the CFG is known during construction (S1), the IR can do what the
passes do now while it is being built. LuaJIT's IR emitter works this way:
it folds, applies CSE and simplifies at emission time, in one pass.

- **Pure operations have no identity.** Two structurally equal pure
  operations *are* the same value. The builder hash-conses them per
  dominating scope, which costs one open-addressing probe per operation.
  Source attribution becomes lossy (one tag per value), which is acceptable
  because tags are optional under S1.
- **Folding and simplification at creation.** The rules of
  `ConstantFoldingAndCopyPropagation`, `AlgebraicSimplification` and
  `ConstantBranchFolding` run as builder rules. `if_` on a constant
  condition traces only the taken arm, with no dead blocks to sweep.
- **DCE via use counts.** The builder maintains use counts. A backend skips
  pure values with zero uses, and no DCE pass is needed.
- **Structured control-flow operations in the IR** (`if`/`loop` with nested
  blocks and explicit loop-carried values, like MLIR `scf` or Wasm) instead
  of a flat CFG of block arguments:
  - **Loops are known by construction,** so `LoopInfo` and dominators are
    not needed on the fast path.
  - **Liveness is structural:** a value defined outside a loop and used
    inside is live for the whole loop.
  - **A single-pass backend is simpler.** Wasm baseline compilers such as
    Liftoff and Winch are single-pass for exactly this reason.
  - **MLIR still works.** It can lower to `scf` and let MLIR produce `cf`.
- **Memory operations stay ordered** by an explicit effect chain (a token
  operand, or "last store" state in the builder). That makes the CSE above
  safe without alias analysis. Loads may be CSE'd against the most recent
  effect only.

The fixed-point pass pipeline disappears from the critical path. Expensive
optimizations (LICM, strength reduction) remain optional passes.

### S3. Optimistic call semantics with declared effects

- **`invoke` is `noUnwind` by default.** Throwing callees must opt in
  (`invoke<may_throw>(…)` or an attribute). Most query runtime functions do
  not throw. The opt-in keeps exception regions and landing pads, and the
  capture wrapper, to the call sites that need them.
- **Declared effects.** A call declares its memory behavior: `pure`,
  `readonly`, `argmemonly` or `ModRef`. Today's pessimistic default stays
  available, but marking hot runtime calls `pure`/`readonly` turns them from
  barriers into values that S2 can CSE.
- **Trust declarations instead of inferring them.** `NoThrowInferencePass`
  and `FunctionAttributeInferencePass` leave the critical path. A debug mode
  checks the declarations (for example, that a `noUnwind` callee does not
  throw, enforced with a terminate handler).

### S4. Signless integer values

- **Values have only a width.** `i8`, `i16`, `i32`, `i64`, `f32`, `f64`,
  `ptr`, `bool`.
- **Signedness moves onto the operations that need it:** `div`, `rem`,
  shift-right, compare, int↔float casts and extends. LLVM and MLIR's `arith`
  do the same.
- **Effects:**
  - casts between `i32` and `ui32` disappear;
  - tables over 10 integer types become tables over 4 widths;
  - backend lowering has fewer cases (the 10-type families in tbc halve).
- **The user-facing `val<uint32_t>` keeps its C++ semantics.** The operator
  overloads pick the signed or unsigned operation.
- **Integer semantics are defined in the IR.** For example, wrap-around on
  overflow, a trap on division by zero, and masked shift amounts. Then
  construction-time folding and every backend agree without
  backend-specific guards.

## 9. Resulting pipeline (implicit control flow kept)

```
replay tracing (implicit control flow, tags, merge detection)
        │   records into the flat trace (§12)
        ▼
optimizing conversion (one RPO pass: SSA + fold + scoped CSE + use counts)
        │   writes the flat IR (§13)
        ▼
optional passes (LICM, …) ─► single-pass backend / bc / tbc / MLIR
```

## 10. Priority and risks

| Change | Latency effect | Cost to users | Order |
|---|---|---|---|
| Part III flat trace + flat IR | high (no per-op malloc, no hash maps, ~5–10x fewer bytes) | none (internal; dumps change) | 1 |
| S2 in the conversion | high (fixed-point pipeline off the critical path) | none | 2 |
| S3 optimistic calls | medium | low: annotations on throwing callees | 3 |
| S4 signless integers | low–medium | none (internal) | 4 |
| S1 structured tracing | high | high: API change | deferred follow-up |

S2 and S3 carry the risks listed under them above. S1's risks only matter
once it is picked up.

---

# Part III: Memory representation (trace kept)

## 11. Two flat representations, one shared vocabulary

Today a value exists three times:

- as a 104 B `TraceOperation` with variant inputs;
- as a 32–152 B arena `Operation`, plus its input array and a slot in a
  heap `std::vector`;
- as hash-map entries in each backend.

The trace stays, because replay-based exploration and merge detection need
a record of what was executed and where. But the trace and the IR become
**two flat arrays with the same instruction format**:

- **The trace** is an append-only stream in recording order, grouped into
  blocks by index lists. It carries the snapshot information that merge
  detection needs, in a side array.
- **The IR** is the same instruction format in SSA, laid out per function
  in layout order, without snapshots.
- **Conversion** reads one flat array and writes another: a linear rewrite
  instead of building a pointer graph.

The model follows LuaJIT's IR (fixed-size instructions, 16-bit refs,
constants below a bias), Cranelift's entity tables (`u32` ids, secondary
maps, pooled value lists) and Zig's AIR (struct-of-arrays, tag + data, an
"extra" array for variable-length payloads).

### Shared instruction format

```cpp
struct Inst {                 // 16 B
	Opcode op;                // u8
	Type   type;              // u8: signless width once S4 lands
	uint16_t flags;           // branch hint, effect class, ...
	uint32_t a, b;            // operands: value refs or tagged constant refs
	uint32_t c;               // third operand, immediate, or index into `extra`
};
```

Shared by trace and IR:

- **`extra`** (`std::vector<uint32_t>`) holds variable-length payloads: call
  arguments, block-argument lists, `{argCount, args…}`.
- **`ConstPool`** holds interned constants, referenced with the top bit set
  (the counterpart of LuaJIT's refs below `REF_BIAS`). An add of a constant
  needs no constant instruction.
- **Call descriptors** go into a per-module table (the existing
  `FunctionTable`), referenced by `FunctionId` in `a`. Names are resolved
  only when dumping.
- **Storage** is recycled per thread, so a steady-state compile does no
  per-operation `malloc`.

## 12. The flat trace

```cpp
struct FlatTrace {
	std::vector<Inst>      ops;        // trace op id == index; recording order
	std::vector<uint32_t>  extra;
	ConstPool              consts;
	std::vector<Snapshot>  snapshot;   // by trace op id (16 B); only the merge map reads it
	std::vector<uint16_t>  region;     // by trace op id
	std::vector<TraceBlock> blocks;    // per block: arena index list of op ids, predecessors
	FlatMap<Snapshot, uint32_t> tagMap; // open addressing in the trace arena: snapshot -> op id
};
```

What changes compared with today:

- **Operations are identified by a global op id, not by `(block, index)`.**
  `globalTagMap` stores `(blockIndex, operationIndex)` today. When
  `processControlFlowMerge` splits a block, it moves the tail into the
  merge block and **rewrites the map entry of every moved operation**. With
  a global op id the map entries stay valid. A split moves only the tail of
  one `u32` index list, plus one `opBlock[]` update per moved op if a
  reverse map is kept.
- **Records are 16 B instead of 56 + 24·n B.** The snapshot (16 B) moves
  to a parallel array that is written in record mode and read only by the
  merge map and source attribution. Operands are 4 B refs. Constants, call
  descriptors, block refs, probabilities and alloca indices go into
  `consts` / `extra` / the side tables.
- **The tag map is an open-addressing table in the trace arena,** so an
  insert does not allocate. Today every recorded operation allocates a
  `std::unordered_map` node.
- **Trie fan-out is hashed.** `TrieNode::append` switches to a small
  open-addressing child table above about 8 children, so tag creation stays
  O(depth) for functions with many call sites.
- **Replay keeps its cursor.** Follow mode already advances a cursor without
  taking a snapshot (`TraceContext::follow`). With the flat trace the cursor
  is a position in the block's index list, and reading the recorded op is an
  index into `ops`. The reconciliation-`ASSIGN` skip in `follow()` compares
  `snapshot[id]` instead of the record's embedded tag.
- **Value refs stay what they are.** `TypedValueRef` (a `u32` ref plus the
  type) is what `val<T>` holds. Operand slots in `Inst` are those refs, so
  recording an operation writes one 16 B record and the ref of its result.

Explicitly unchanged:

- the replay algorithm (`SymbolicExecutionContext`, follow and record
  modes);
- tag creation by frame-pointer walk;
- snapshot hashing with the static-value stack;
- `static_val` and `region()` semantics;
- merge detection at any operation.

S1 can later be added as a mode that skips snapshots and the tag map; it
would write the same format.

**Open question to measure.** Copies (`ASSIGN` from `val` copy
construction) are about half of all traced operations, according to the
comment in `TraceContext::traceCopy`. In the flat trace they still cost a
16 B record, a snapshot and a map insert each. A lazier copy model could
remove most of them, but copy sites take part in merge detection, so this
needs its own investigation and is not part of this design.

## 13. The flat IR

The trace's CFG is complete only once tracing ends, so the IR keeps
**CFG form** (blocks + block arguments) instead of structured markers. It
is laid out for single-pass consumers:

```cpp
struct FlatFunction {
	std::vector<Inst>     insts;      // value id == index; blocks contiguous, in layout order
	std::vector<uint32_t> extra;      // block-argument lists, call args
	ConstPool             consts;
	std::vector<BlockInfo> blocks;    // per block: first inst, arg range in extra, loop depth, loop end
	// Cold side tables by value id, filled only on request:
	std::vector<const Tag*> sourceTag;
	std::vector<uint16_t>   region;
	// Built by the conversion:
	std::vector<uint32_t>   useCount;
	std::vector<uint32_t>   forward;  // replaced-by, resolved on read
};
```

- **Block arguments are the first instructions of a block** (`op=BlockArg`),
  so they have value ids like everything else. Terminators are
  `br target, extraIdx` and `condbr cond, t, f, extraIdx`; the argument
  lists live in `extra`. There are no `BasicBlock`, `BasicBlockInvocation`
  or predecessor objects; predecessor counts are a side array computed by
  the conversion.
- **Layout order is fixed by the conversion:** RPO with loop bodies
  contiguous. Each `BlockInfo` carries loop depth and loop end, so a
  TPDE-style backend needs no analysis pass of its own. The trace gives
  this nearly for free, because the tracer already marks `ControlFlowMerge`
  blocks (loop-header candidates) and knows which edges go back.
- **Dense by construction.** Value id = index, block id = index. Every
  backend side table is a `std::vector`, and `Frame<OperationIdentifier, V>`
  becomes an array (§4.2 falls out directly).

### The optimizing conversion (S2 without S1)

`TraceToIRConversionPhase` already converts in one linear pass with dense
tables and Braun-style SSA. It becomes the optimizing builder:

1. **Visit blocks in RPO** with predecessors before successors, except for
   back edges. A block is *live* once a live edge enters it, and dead
   blocks are never emitted. A `condbr` on a value that folded to a
   constant adds only the taken edge, so constant branch folding and
   unreachable-block removal happen during conversion. Back edges come from
   latches converted after the header, which is when Braun sealing closes
   the header.
2. **Fold on emission.** The rules of `ConstantFoldingAndCopyPropagation`
   and `AlgebraicSimplification` are builder rules. A trivial block
   argument (every edge passes the same value, or only itself) is folded
   into `forward[]`, as Braun's algorithm already does.
3. **Scoped CSE.** An open-addressing table over `(op, type, a, b, c)` with
   an undo log, scoped along the dominator tree. A dominator tree over the
   trace CFG is cheap (Cooper–Harvey–Kennedy). A block-local scope is a
   valid first step. Loads and calls key their entries on the current
   effect id, so memory order is kept without alias analysis.
4. **Use counts while emitting.** A pure value with zero uses at the end is
   skipped by backends, so no DCE pass is needed on the fast path.
5. **Block merging and empty-block elimination become threading.** A trace
   block with a single predecessor and an unconditional jump is appended to
   its predecessor's IR block instead of becoming its own block. Jump-only
   trace blocks are already threaded today.

After this, the fixed-point pass group is no longer on the critical path.
LICM and strength reduction remain optional passes.

### Mutation model: build once, rewrite by copying

- **Linear rewrites copy.** A pass that removes or replaces operations
  streams `insts` into a new array using an old→new id map (one
  `std::vector<uint32_t>`). This is O(n), stays cache-friendly and keeps
  ids dense.
- **Local replacements use `forward[]`.** Compaction applies them later.
- **No persistent use lists.** A pass that needs real uses builds a CSR
  table once (offsets + flat use array). `FunctionRewriter`'s per-pass
  hash table and the predecessor upkeep in `IRPassManager` go away.

## 14. Cost comparison for one binary `add` (x86-64)

| | Today | Proposed |
|---|---|---|
| Trace record | 56 B + 2 × 24 B inputs | 16 B + 16 B snapshot (record mode only) |
| Tag map insert | `unordered_map` node (`malloc`); rewritten when its block is split | open-addressing slot; never rewritten |
| IR object | 32 B op + 16 B inputs + 8 B vector slot | 16 B `Inst` |
| Constant operand | separate 40 B `ConstIntOperation` | interned entry in `ConstPool`, shared |
| Branch / if | 80 B / 144 B + block objects | 16 B terminator + args in `extra` |
| Backend value lookup | `unordered_map` probe | array index |
| Heap allocations per op (steady state) | ≥ 1 | 0 (recycled arrays) |
| IR passes before codegen | fixed point, up to 28 use-table builds | none required (done in conversion) |

That is roughly 5x fewer bytes per traced operation, about 3x fewer per IR
operation, and no per-operation allocation. Tag creation (the frame-pointer
walk) remains the main per-operation tracing cost in record mode. That cost
belongs to the kept trace semantics and is what a later S1 would remove.

## 15. Consequences for consumers

- **Backends:**
  - `bc`/`tbc` become a near 1:1 translation, because a register bytecode
    is almost this format;
  - `asmjit` and a TPDE-style backend iterate `insts` directly, using
    `BlockInfo` for loops;
  - MLIR builds `cf`/`arith` blocks from `BlockInfo` and the terminators.
- **Trace consumers.** `CopyAttribution`, region seam collapsing and the
  trace dump read `InputVariant` today. They switch to the flat accessors;
  add the accessor layer first (§6).
- **Dumps and tests.** The `tracing/`, `ir/` and `after_*` reference dumps
  under `test/data` change format. That means a one-time regeneration, with
  the differential fuzzer guarding semantics across it.
- **Migration.** Land it in four steps:
  1. flat trace behind the existing `TraceModule` interface;
  2. flat IR alongside `IRGraph`, with an adapter from flat IR to `IRGraph`
     so the existing passes and backends keep working;
  3. port backends one at a time: `bc`/`tbc` first, then `asmjit`, then
     MLIR;
  4. delete `IRGraph` and the pointer-based passes.

## 16. Risks

- **The 16 B instruction format is a hard budget.** Wide immediates and
  `__int128` constants go through `ConstPool`; SIMD shuffles and other
  variable-length payloads go through `extra`. Each new opcode must fit,
  which constrains the plugin APIs (intrinsics, SIMD, GPU).
- **Scoped CSE must follow dominance exactly.** Start with block-local
  scopes. Widening the scope to the dominator tree needs dedicated fuzzing.
- **Lossy provenance.** A value shared through CSE keeps one source tag.
  Disable CSE in debug builds if debuggers need per-call-site identity.
- **Conversion order.** Folding during conversion changes which blocks
  exist, so verify on every merge-heavy test (regions, nested loops,
  `break`) that Braun sealing does not wait on predecessors that were
  folded away.

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

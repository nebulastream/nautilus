# Component: Optimizing Conversion and Passes

Part of the [Nautilus 0.4 design](README.md) · Milestone M3

## 1. Purpose

The conversion turns trace-level NIR into SSA-level NIR. In 0.4 it also does
the cleanup that today's fixed-point pass pipeline does, so the pipeline
leaves the critical path. Legalization prepares SSA-level NIR for a specific
backend. Expensive optimizations remain available as optional passes.

## 2. Why

P0 ([p0-baseline.md](p0-baseline.md)):

| Code shape | IR passes' share of the compile |
|---|---|
| Linear code on `main` | **62–68%** |
| Linear code with #561 | **44–53%** (the largest stage) |
| Branchy code (`main` and #561) | 10–40% |

On `main`, building `FunctionRewriter` tables is 44% of all instructions,
and most pass runs change nothing (#561's pass statistics). The pipeline
runs up to `ir.maxPipelineIterations` (4) rounds of:
- constant folding and copy propagation;
- algebraic simplification;
- constant branch folding;
- empty-block elimination;
- block merging;
- DCE;
- argument pruning.

Most of this cleans up artifacts of tracing that a builder can avoid
creating.

## 3. The optimizing conversion

Today's `TraceToIRConversionPhase` already converts in one linear pass with
dense tables and Braun-style SSA construction on the complete trace CFG.
0.4 extends it into an optimizing builder:

1. **Visit blocks in RPO,** predecessors before successors except along
   back edges:
   - a block is *live* once a live edge enters it, and dead blocks are
     never emitted;
   - a `condbr` on a value that folded to a constant adds only the taken
     edge, so constant branch folding and unreachable-block removal happen
     during conversion;
   - back edges come from latches converted after the header, which is when
     Braun sealing closes the header.
2. **Fold and simplify on emission.** The rules of today's
   `ConstantFoldingAndCopyPropagation` and `AlgebraicSimplification` become
   builder rules, along with the rules of extension ops
   ([04](04-extensions.md)). A block argument that every edge passes the same
   value for (or only itself) is folded into `forward[]`, as Braun's
   algorithm already does.
3. **CSE:**
   - an open-addressing table over `(op, type, a, b, c)` with an undo log;
   - **block-local first**, then scoped along the dominator tree (a dominator
     tree over the trace CFG is cheap: Cooper–Harvey–Kennedy);
   - loads and calls key their entries on the current effect id, so memory
     order is kept without alias analysis.
4. **Use counts while emitting.** A pure value with zero uses at the end is
   skipped by backends, so the fast path needs no DCE pass.
5. **Threading.** A trace block with a single predecessor and an
   unconditional jump is appended to its predecessor's NIR block (block
   merging and empty-block elimination). Jump-only trace blocks are already
   threaded today.
6. **Layout.** The conversion emits blocks in final layout order (RPO, loop
   bodies contiguous) and fills `BlockInfo` with loop depth and loop end.
   The tracer already marks control-flow merge blocks (loop-header
   candidates) and knows the back edges.
7. **Exceptions.** The work of today's `ExceptionRegionPreparationPass`
   happens during emission: landing pads are keyed by their ordered
   destructor set and shared, and unwinding calls point at them
   ([02 §3.4](02-nir.md)).
8. **Attributes.** The analyses of `NoThrowInferencePass` and
   `FunctionAttributeInferencePass` run once inside the conversion, on the
   function table, instead of as separate passes. From M6 on, declared
   effects and `nothrow` annotations ([06](06-semantics.md)) take precedence
   where present.

## 4. Legalization (NIR → NIR)

Before a backend runs, one copy pass prepares SSA-level NIR for it:

- extension ops without a handler on that backend expand to their
  fallback (a native call or an expansion into core ops);
- extension types without support become their memory fallback (stack
  slots);
- calls are lowered to the target ABI where the backend asks for it.

There is no separate low-level IR in core. Each backend keeps its own
internal form below this point ([05](05-backends.md)).

## 5. Optional and plugin passes

- **Optional passes:** LICM and strength reduction, rewritten as copy
  passes ([02 §4](02-nir.md)). They are off on latency-critical compiles and
  enabled per module.
- **Plugin passes** register at named points (`afterConversion`,
  `beforeLowering`), each marked *required* (correctness, for example GPU
  kernel outlining) or *optional*. Only required passes run on
  latency-critical compiles, so a plugin cannot silently put a fixed-point
  pass back on the critical path.
- **Analyses** come from `BlockInfo` and `useCount`; plugins do not
  recompute them.
- The options `ir.runOptimizationPasses` and `ir.disable*` keep their meaning
  for the optional passes. Options for passes that no longer exist are
  accepted with a deprecation warning.

## 6. Exit criteria (M3)

- `irPasses.totalMs` ≈ 0 on default compiles, and the conversion itself at
  most 0.25 µs/op on linear corpus code (0.3: IR generation ~0.15 µs/op plus
  passes 0.6–1.6 µs/op).
- IR op counts at most those of 0.3 after its full pipeline, on every
  corpus function and on TPC-H (nautilus-duck) kernels.
- Differential fuzzer clean, including merge-heavy corpora: regions, nested
  loops, `break`.

## 7. Risks

- **CSE correctness.** Block-local CSE ships first. Dominator-scoped CSE
  ships only after dedicated fuzzing. CSE is off in debug builds that need
  per-call-site provenance.
- **Folding changes which blocks exist.** Braun sealing must not wait on
  predecessors that were folded away; cover this with merge-heavy tests.
- **Code quality on backends that relied on the pipeline** (`bc`/`tbc`
  dispatch counts). Compare executed instruction counts in
  `ExecutionBenchmark` and keep an optional per-backend pass list where it
  pays.

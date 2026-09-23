# Nautilus Lint: a layered linter for traced user code

Status: design proposal. Companion rule catalog: [`nautilus-lint-rules.yaml`](nautilus-lint-rules.yaml).

## 1. Why

nebulastream/nebulastream#2038 (the LLVM 22 / clang 22 toolchain upgrade) surfaced a class of
defects that only exist because user code is *traced*: the code is valid C++, compiles, runs
correctly when executed directly, and produces wrong or uncompilable JIT output. Two of them were
found by debugging wrong query results, not by any tool:

| Symptom in #2038 | Root cause | Upstream issue | What would have caught it |
| --- | --- | --- | --- |
| NLJ probe joined records with the wrong tuple layout | `performNLJ(left, right, …)` and `performNLJ(right, left, …)` in the two arms of a traced `if`. Clang 22 at `-O1` hoists the two calls into one call site. Nautilus identifies trace points by return address, so both arms produced the same tags and the tracer bound one arm's arguments to the other arm's roles. | nautilus#487 | A syntactic check: same callee in both arms of a traced branch. |
| `invoke()` on a proxy returning `OriginId` / `Timestamp` failed to compile | `invoke()` on a non-`noexcept` function pointer takes the exception-handling path, which needs `R{}`. Strong types are deliberately not default-constructible. The fix was to mark the proxies `noexcept` (nautilus#474/#475 made this a compile-time decision). | nautilus#474 | A check that proxies with a non-default-constructible return type are `noexcept`, with a message that names the fix. |
| `no SSA value recorded for operation $N` in ordinary scan/emit pipelines | Any loop with a potentially-throwing `invoke()` and a live destructor-bearing `val<Struct>` local; a compiler pass dropped a block argument still referenced by a destructor list. | nautilus#478 (library bug) | Nothing in user code is wrong. But the *shape* (throwing invoke + destructor-bearing local in a loop) is worth a hint: mark the invoke `noexcept` when it cannot throw and the exception region disappears. |
| `llvm.landingpad needs to be in a function with a personality` | Same shape, nested `NautilusFunction`s with mixed `noexcept`. | nautilus#481 (library bug) | Same hint. |
| `engine.compilationStrategy` silently ignored | Stringly typed option that upstream removed. | – | A check of `setOption` keys against the option manifest. |

The existing LLM review skill (`.claude/skills/nautilus-review/SKILL.md`) covers codegen-quality
advice well, but it is neither deterministic nor gateable in CI, and none of its rules would have
flagged the first two rows. This design adds the deterministic tier, keeps the LLM tier for
judgment calls, and names a third tier (the tracer itself) for properties only the runtime can
verify.

## 2. Decision: linter, skill, or both?

**Both, in layers, with a clear owner per rule.** A rule lives in exactly one layer.

| Layer | Vehicle | Owns | Why here |
| --- | --- | --- | --- |
| L0 — Tracer / library self-checks | C++ in `nautilus/` (static_asserts, trace-time diagnostics, option validation) | Properties that need runtime facts: tag collisions with divergent bindings (#487), unknown option keys, `val` escaping the trace, recursion depth. | The tracer already sees the ground truth; a lint can only approximate it. Fixes here protect every downstream, including code the lint never sees. |
| L1 — Deterministic linter | Out-of-tree **clang-tidy module** `nautilus-*` (LibTooling, AST matchers), prototyped with `clang-query` | Syntactic and type-level rules with near-zero false positives: branch-merged call sites, `noexcept` on proxies, host loops over traced bodies, `static_val` bounds depending on `val`, narrowing stores, addresses of host locals passed to `invoke`, multiple returns, escapes via raw casts, deprecated option keys. | Gateable in CI with nebulastream's existing ratchet (`.clang-tidy` `WarningsAsErrors`), `NOLINT` suppression, fix-its, `clang-tidy-diff` on PRs. |
| L2 — LLM skill | `.claude/skills/nautilus-review` (extended) | Judgment rules: `static_val` vs `val` for a given bound, `select` vs `if/else`, probability hints, load hoisting, region placement, "is this host call trace-time metadata or a runtime side effect". | Needs intent and cost/benefit reasoning; false positives are tolerable in a review comment, not in a CI gate. |

Why not a skill only: the #487 and #474 defects are mechanical, and a reviewer that misses them
one time in ten is not a gate. Why not a linter only: half of the catalog is advice whose validity
depends on what the kernel is for; encoding that as errors would train people to sprinkle
`NOLINT`. Why clang-tidy rather than a standalone tool: nebulastream already runs
`clang-tidy-diff` on every PR and `run-clang-tidy` on `main`, both driver scripts forward
`-load`, and the `.clang-tidy` ratchet gives per-rule promotion from warning to error for free.
The cost is the plugin ABI: the module must be built against the exact clang-tidy major that
loads it. nebulastream pins that version in its toolchain image, so this is a build-matrix entry,
not a design problem.

## 3. Narrowing to Nautilus code

Generic C++ rules must not fire on ordinary host code. The linter therefore applies every rule
only inside a **traced context**, computed per translation unit:

1. **Seed functions.** A `FunctionDecl` (including lambdas and member functions) is traced if any
   of the following holds:
   - a parameter or the return type, after desugaring, is `nautilus::val<T>`, `nautilus::static_val<T>`,
     or a type whose definition is a class template specialization of `nautilus::val`;
   - its body contains a call to `nautilus::invoke`, `nautilus::function`, `nautilus::select`,
     `nautilus::region`, `nautilus::static_iterable`, `nautilus::static_enumerable`, or a
     construction of `nautilus::NautilusFunction`;
   - it is the callable passed to `NautilusEngine::registerFunction`, `Module::registerFunction`,
     `NautilusFunction{…}`, or `nautilus::region(…)`.
2. **Closure.** A function called from a traced function with at least one `val`-typed argument
   is traced. Iterate to a fixed point within the TU. Cross-TU callees are not analysed; the
   rule that needs them (`nautilus-branch-merged-call`) only needs the callee's declaration.
3. **Framework exclusion.** Files that *implement* traced types are excluded by a
   `FrameworkFiles` regex option (default: `nautilus/include/nautilus/.*`, `plugins/.*/include/.*`,
   and for nebulastream `nes-nautilus/include/(DataTypes|Interface)/(VarVal|NESStrongTypeRef|TimestampRef)\.hpp`).
   Inside those files the rules about raw `.value` access and casts are meaningless.
4. **Path scoping is secondary.** nebulastream can additionally restrict with
   `HeaderFilterRegex` and by running the module only on the `nes-nautilus`, `nes-physical-operators`,
   `nes-plugins`, `nes-input-formatters`, `nes-output-formatters` and `nes-runtime` targets, but the
   type-based predicate is what keeps the checks quiet in `nes-sql-parser`.

The predicate is implemented once as a shared `TracedContext` analysis that every check queries
(`isTraced(const FunctionDecl*)`, `enclosingTraced(const Stmt*)`), so a new rule is one matcher
plus one diagnostic.

Nautilus-typedness of an expression is decided by a single helper `isValType(QualType)` that
matches `nautilus::val`, `nautilus::static_val`, and any class in the configurable
`ValLikeTypes` list (for nebulastream: `NES::VarVal`, `NES::Record`, `NES::Nautilus::Interface::*Ref`).

## 4. Rule catalog

Rule IDs are stable. Severity uses the review vocabulary already in the skill: **Critical**
(trace is wrong or missing operations), **Major** (traces but blocks optimization or is a
known-miscompile shape), **Minor** (style / micro-opt). Layer is the owner from §2. The YAML
catalog carries the same data plus matcher sketches and test cases; this table is the summary.

### 4.1 Correctness of the trace (Critical)

| ID | Name | Rule | Origin |
| --- | --- | --- | --- |
| NL-CALL-001 | `nautilus-branch-merged-call` | In a traced function, the same non-inlined callee is called in two arms of the same `if`/`else`/`?:`/`switch` on a `val<bool>` with non-identical argument lists. The host compiler may hoist the two calls into one call site, so both arms get the same tag and the tracer merges them with the wrong argument binding. Fix-it: route each arm through a distinct `[[gnu::noinline]]` wrapper, or restructure so one call follows the branch. | #2038, nautilus#487 |
| NL-CALL-002 | `nautilus-invoke-host-local-address` | `invoke(f, &x)` or `val<T*>{&x}` where `x` has automatic storage in the host function and is not a `val`. The address is baked into the compiled code as a constant pointing at trace-time stack. | new |
| NL-CF-001 | `nautilus-host-loop-in-traced-body` | A loop whose induction variable or range is a host type (`int`, `size_t`, range-for over a std container without `static_iterable`) and whose body records traced operations. The tracer keys loop detection on the tag and the live-value snapshot; a host loop provides neither a per-iteration tag nor a per-iteration snapshot, which is exactly what `static_val` / `static_iterable` add. Fix-it: `static_val<…>` / `static_iterable(…)`. | docs/tracing.md, AggregationBuild in NES |
| NL-CF-002 | `nautilus-static-loop-bound-depends-on-val` | A `static_val` loop whose condition references a `val<T>`. Cannot be unrolled at trace time. | docs/static-val.md |
| NL-CF-003 | `nautilus-recursive-traced-call` | A traced function reaches itself in the TU call graph. The tag recorder throws "Stack is too deep". | TagRecorder.cpp |
| NL-ESC-001 | `nautilus-val-static-storage` | `val<T>` / `static_val<T>` / val-like object with static or thread-local storage, or as a member of a type with static storage. Trace state does not outlive the trace. | docs/tracing.md |
| NL-ESC-002 | `nautilus-raw-value-control-flow` | Host control flow (`if`, loop condition, `?:`, `switch`) on a value obtained by unwrapping a `val` (`.value`, `getRawValue()`, `RawValueResolver`, `static_cast<T>` of a `val<T>`). The branch is decided at trace time and only one path is traced. | skill C5 |
| NL-ESC-003 | `nautilus-untraced-side-effect` | Direct call in a traced function to a denylisted host side effect: stream output, `printf` family, `fmt::print`, logging macros, `new`/`delete`, file I/O. It executes once at trace time. Use `invoke`. | docs/tracing.md, NES `logProxy` |
| NL-ESC-004 | `nautilus-raw-pointer-escape` | C-style cast, `reinterpret_cast`, or `(uintptr_t)` applied to a `val<T*>`; subsequent loads are untraced. | skill A1, E1 |
| NL-TYPE-001 | `nautilus-narrowing-store` | `*p = v` / `p[i] = v` where `v` is a `val` wider than the pointee. Reproduces regression GH#90. Fix-it: explicit `static_cast<val<Pointee>>`. | skill D3 |

### 4.2 Known-miscompile shapes and blocked optimization (Major)

| ID | Name | Rule | Origin |
| --- | --- | --- | --- |
| NL-CALL-003 | `nautilus-invoke-prefer-noexcept` | `invoke(f, …)` where `f` (function pointer or `+[]` lambda) is not `noexcept`, and either (a) `R` is not default-constructible, or (b) a destructor-bearing `val<Struct>` is live at the call. (a) is a compile error with an unreadable template trace; (b) creates an exception region (the #478/#481 shape). Message names the fix: add `noexcept` when the callee cannot throw. Plain (c) "any non-noexcept invoke" is Minor. | #2038 (`abb138a`, `877a53b`), nautilus#474/#475/#478/#481 |
| NL-RET-001 | `nautilus-single-return` | More than one `return` in a traced function. Configurable; nebulastream may start it as a warning. Exempt fixtures via `NOLINT`. | skill H1 |
| NL-CF-004 | `nautilus-loop-exit-in-dynamic-loop` | `return`, `break`, or `goto` inside a `for (val<…> …)` loop body. Adds a second exit edge; loop analysis loses the canonical shape. | skill H6 |
| NL-CF-005 | `nautilus-static-unroll-cap` | `static_val` loop with a literal bound above `MaxUnroll` (default 100), or `static_iterable` over a `std::array` / C array with more elements than that. | skill B1 |
| NL-CALL-004 | `nautilus-invoke-in-loop-opaque` | `invoke` inside a dynamic loop whose callee is neither `NAUTILUS_INLINE` nor carries `FunctionAttributes` (`ModRefInfo::Ref`, `noUnwind`). Hint only unless `Strict`. | skill F1, F3 |
| NL-OPT-001 | `nautilus-unknown-option-key` | String literal passed to `Options::setOption` not present in the option manifest shipped by nautilus (generated from `docs/options.md` into `share/nautilus/options.json`). Catches removed keys such as `engine.compilationStrategy`. | #2038 |
| NL-FN-001 | `nautilus-function-object-lifetime` | `NautilusFunction` with automatic storage that is copied, moved, or returned. It is non-copyable and must outlive all callers. | docs/functions.md |

### 4.3 Codegen quality (Minor, L2 unless marked)

| ID | Name | Rule | Layer |
| --- | --- | --- | --- |
| NL-SEL-001 | `nautilus-prefer-select` | `if/else` (or `?:`) whose two arms only assign pure expressions to the same `val`. Fix-it to `select(cond, a, b)`. | L1 (mechanical) |
| NL-BOOL-001 | `nautilus-bool-short-circuit` | `&&` / `||` on `val<bool>` operands when `ENABLE_SHORT_CIRCUIT_BOOL` is off and the right operand contains a call or load. | L1 |
| NL-TYPE-002 | `nautilus-mixed-signedness` | Arithmetic or comparison between signed and unsigned `val`s. | L1 |
| NL-PROB-001 | probability hint missing on a skewed guard | L2 |
| NL-LOOP-001 | `val` loop with trace-time-known small bound → `static_val` | L2 |
| NL-MEM-001 | repeated loads of the same field with no intervening store | L2 |
| NL-REG-001 | large straight-line body without `nautilus::region` boundaries | L2 |

### 4.4 Build configuration (L1, CMake-level)

| ID | Rule |
| --- | --- |
| NL-BUILD-001 | Any target that links `nautilus::nautilus` and compiles traced code must carry `-fno-omit-frame-pointer` (the tag recorder walks the frame chain). Nautilus already exports this as an `INTERFACE` compile option; the check is that no later `target_compile_options` re-enables omission. |
| NL-BUILD-002 | Optional mitigation for NL-CALL-001 at the host-compiler level: `-fno-optimize-sibling-calls` and `-mllvm -enable-tail-merge=false` on traced targets. Marked *needs verification*: SimplifyCFG hoisting (the mechanism in #487) is not covered by these flags. The durable fix is L0. |

### 4.5 Library-level fixes (L0)

These are not lint rules; they are the changes in `nautilus/` that remove whole rule classes.

| ID | Change | Replaces |
| --- | --- | --- |
| L0-001 | Tracer: when a path reaches an existing tag with a different argument binding, either emit block arguments per path or fail with a diagnostic naming the enclosing function and both call sites. Tracks nautilus#487. | Makes NL-CALL-001 a hint instead of a gate. |
| L0-002 | `function.hpp`: `static_assert(NoUnwind || std::is_default_constructible_v<R>, "invoke(): a potentially-throwing callee must return a default-constructible type; mark the callee noexcept if it cannot throw")`. | The unreadable template error behind NL-CALL-003(a). |
| L0-003 | `Options::setOption`: validate keys against the manifest; throw on unknown keys in debug builds, log once in release. Ship the manifest as a generated JSON alongside `docs/options.md`. | NL-OPT-001 at runtime. |
| L0-004 | Debug-build `val` escape detection: stamp each `TypedValueRefHolder` with the trace generation and assert on use from another generation. | NL-ESC-001 for cases the lint cannot see (containers, members). |

## 5. Architecture of the L1 module

```
tools/nautilus-tidy/
├── CMakeLists.txt              # add_llvm_library(NautilusTidyModule MODULE …), find_package(Clang)
├── NautilusTidyModule.cpp      # registers all checks under the "nautilus-" prefix
├── TracedContext.{hpp,cpp}     # §3 predicate, cached per TU
├── ValTypes.{hpp,cpp}          # isValType(), isValLike(), width/signedness helpers
├── checks/
│   ├── BranchMergedCallCheck.cpp        # NL-CALL-001
│   ├── InvokeHostLocalAddressCheck.cpp  # NL-CALL-002
│   ├── InvokePreferNoexceptCheck.cpp    # NL-CALL-003
│   ├── HostLoopInTracedBodyCheck.cpp    # NL-CF-001
│   ├── …                                # one file per rule ID
├── test/
│   ├── lit.cfg.py                        # uses check_clang_tidy.py from the LLVM install
│   ├── nautilus-branch-merged-call.cpp   # // CHECK-MESSAGES: annotations, bad + good cases
│   └── …
└── query/
    └── *.clang-query                     # matcher prototypes for each rule, runnable without a build
```

Build: `-DENABLE_NAUTILUS_TIDY=ON` requires `find_package(Clang CONFIG)` for the same major as
the `clang-tidy` binary that will load it. Nautilus CI builds it in the Clang 21 job and runs the
lit tests; nebulastream builds it inside its toolchain image and adds
`--load=$<TARGET_FILE:NautilusTidyModule>` to `scripts/tidy.sh` (both driver scripts accept
`-load`). Until the module exists, the `query/` directory is usable with `clang-query` against any
`compile_commands.json` and is the recommended way to iterate on a matcher.

Configuration (all via `.clang-tidy` `CheckOptions`):

| Option | Default | Used by |
| --- | --- | --- |
| `nautilus-*.FrameworkFiles` | `nautilus/include/.*;plugins/.*/include/.*` | all |
| `nautilus-*.ValLikeTypes` | `` | `isValType` |
| `nautilus-static-unroll-cap.MaxUnroll` | `100` | NL-CF-005 |
| `nautilus-untraced-side-effect.Denylist` | streams, printf family, fmt, `NES_*LOG*` macros | NL-ESC-003 |
| `nautilus-unknown-option-key.Manifest` | `share/nautilus/options.json` | NL-OPT-001 |
| `nautilus-single-return.Enabled` | `true` | NL-RET-001 |

Roll-out in nebulastream follows the ratchet documented in its `.clang-tidy`: every
`nautilus-*` check starts in `Checks` but excluded from `WarningsAsErrors`; a check is promoted once
`main` is clean. The Critical rows in §4.1 are the first promotion targets.

## 6. Matcher sketches for the first three checks

Untested sketches, written to be pasted into `clang-query` for validation.

**NL-CALL-001**

```
match ifStmt(
  hasCondition(hasDescendant(cxxMemberCallExpr(callee(cxxConversionDecl()),
                             on(hasType(cxxRecordDecl(hasName("::nautilus::val"))))))),
  hasThen(forEachDescendant(callExpr(callee(functionDecl().bind("f"))).bind("thenCall"))),
  hasElse(forEachDescendant(callExpr(callee(functionDecl(equalsBoundNode("f")))).bind("elseCall"))),
  unless(hasAncestor(functionDecl(unless(isTraced()))))
)
```

Post-filter in C++: drop pairs whose argument expressions are structurally identical
(`Stmt::Profile`), drop callees marked `always_inline` or `constexpr` whose body is visible and
trivially inlinable, and drop callees that take no `val`-typed parameters. Message:
"calls to '%0' in both arms of a traced branch may be merged into one call site by the host
compiler; the tracer identifies call sites by return address and will bind the arguments of one
arm on the other (nautilus#487). Give each arm a distinct noinline wrapper."

**NL-CALL-003**

```
match callExpr(callee(functionDecl(hasName("::nautilus::invoke"))),
  hasArgument(0, expr(hasType(pointsTo(functionProtoType(unless(isNoThrow())))))).bind("fn"))
```

Then in C++: read `R` from the prototype; (a) if `!R.isDefaultConstructible()` emit Major;
(b) else walk the enclosing compound statements for a `VarDecl` of `val<Struct>` with a
non-trivial `~Struct()` declared before the call and still in scope, emit Major; else Minor.
Fix-it: insert ` noexcept` after the parameter list of the lambda or the named function's
declaration when its definition is in the TU.

**NL-CF-001**

```
match forStmt(
  hasLoopInit(declStmt(hasSingleDecl(varDecl(unless(hasType(isValType()))).bind("iv")))),
  hasBody(hasDescendant(expr(hasType(isValType())))),
  hasAncestor(functionDecl(isTraced()))
)
```

plus the `cxxForRangeStmt` variant whose range expression is not a `static_iterable` /
`static_enumerable` / val-like range. Suppress when the body contains no val-typed operation at all
(host-only setup loops are fine).

## 7. What agents implementing this need

- Pick a rule ID from the YAML catalog; each entry has `layer`, `severity`, `matcher_sketch`,
  `message`, `fixit`, and `tests` with `bad` and `good` snippets. The snippet is the lit test.
- Add the check under `tools/nautilus-tidy/checks/`, register it in `NautilusTidyModule.cpp`,
  add the lit file, run `ninja check-nautilus-tidy`.
- A rule is "done" when the lit test passes, the `query/` prototype exists, and the rule has a row
  in `docs/design/nautilus-lint-rules.yaml` with `status: implemented`.
- L2 rules are edited in `.claude/skills/nautilus-review/SKILL.md`; when the skill runs on a repo
  that has the module built, it should run `clang-tidy --checks='nautilus-*' --load=…` on the
  files under review first and reason about the L1 output instead of re-deriving it.
- L0 changes are ordinary nautilus PRs; each links the rule it retires.

## 8. Out of scope

- Rewriting the tracer's tag scheme (nautilus#487 is tracked separately; L0-001 is the interface).
- A GCC-based implementation. Tag creation and `NAUTILUS_INLINE` are Clang-only already.
- Linting generated C++ from the CPP backend.

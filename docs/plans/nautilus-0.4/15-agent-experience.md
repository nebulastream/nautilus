# Component: Agent Experience

Part of the [Nautilus 0.4 design](README.md) · Milestones M1–M7 · Builds on
the agent-tooling PR against `main` (`AGENTS.md`, presets, session hook,
skills) and the playground (`tools/playground`)

## 1. Purpose

Make Nautilus easy to work with for coding agents, in two roles:

1. **Contributors:** agents that change Nautilus itself.
2. **Users:** agents that write Nautilus code in applications such as
   NebulaStream.

Both need the same things people need, only more strictly: accurate written
ground truth, fast and reproducible builds, machine-readable feedback, and
error messages that say how to fix the problem. Anything vague or stale costs
an agent far more than a person, because it acts on it.

## 2. What lands on `main` first

One PR against `main`, carried into `nautilus-0.4` by the regular merges:

| Item | Content |
|---|---|
| `AGENTS.md` | the contributor guide, checked against the tree; `CLAUDE.md` imports it |
| Subsystem notes | `AGENTS.md` next to the tracer, the compiler, the backends and the plugins |
| `CMakePresets.json` | `dev` (no MLIR), `full`, `debug`, `asan`, `tsan`, `nocompiler`, `bench` |
| Session hook | `.claude/hooks/session-start.sh`: GCC 14, Clang 21, ninja, ccache, valgrind, clang-format 22 |
| `format.sh` | warns when it is not using clang-format 22, whose output CI checks |
| Skills | `build-and-test`, `ir-change`, `perf-investigation`, `regenerate-dumps` (plus the existing `nautilus-review`) |

## 3. Contributors in 0.4

### 3.1 Ground truth that stays true

- **`AGENTS.md` is part of the change.** A change that moves, renames or adds
  a component updates the `AGENTS.md` that describes it, in the same PR.
- **A doc check in CI:** a script verifies that every path, target, preset,
  option and skill named in `AGENTS.md` files and skills exists. Stale
  guidance then fails CI instead of misleading the next agent.
- **The NIR specification is the source** for every IR checklist
  ([13](13-nir-spec.md)): skills link to its sections instead of restating
  them.

### 3.2 Skills for 0.4

| Skill | Content |
|---|---|
| `ir-change` (rewritten for NIR) | the per-opcode checklist from the spec: record layout, verifier rule, printer and parser, serializer, conversion folding, every backend's lowering, C API enum, golden images, fuzzer |
| `extension-plugin` | an extension operation, type or intrinsic with its fallback, traits and per-backend handlers ([04](04-extensions.md)) |
| `fuzz-triage` | run the differential fuzzer, minimize a finding, add it to the corpus, write the regression test |
| `memory-check` | the long-running probe and ASan/LSan, with arena-pool counters ([02 §4.1](02-nir.md)) |
| `asm-diff` | `asmjit` assembly before and after a change |
| `steward` | PR conventions: the CI matrix, the benchmark bot, format, dumps in separate commits |

### 3.3 Feedback agents can parse

- **NIR text parser.** The printer's text form ([13 §9](13-nir-spec.md))
  becomes parseable in 0.4. Tests can then be short `.nir` files with the
  expected output after a pass or a backend (FileCheck style), instead of C++
  that must trace something first. This is the largest single gain for agents
  writing tests. Round trip: print → parse → print is byte-identical.
- **Verifier and loader errors carry stable codes** (`NIR-V012: block ^b3 has
  no terminator`), listed in the spec.
- **The latency corpus and the probes write JSON** ([10](10-observability.md)),
  so before/after comparisons are a diff of two files.
- **`tools/agent/affected-tests`:** maps changed files to the test targets and
  backends to run.

## 4. Users in 0.4

### 4.1 Diagnostics written for fixing

- **Every user-facing error has a stable code, the user's source location and
  a hint.** For example:
  `NAUT-T001 constant loop at Query.cpp:42: the loop condition does not
  depend on a val; use a val loop variable, or static_val to unroll`.
- **A catalog** of codes in `docs/diagnostics.md`, one entry per code with a
  minimal example and the fix. Each code has a test that triggers it.
- **`engine.diagnostics=json`** writes diagnostics as JSON lines for tools.
- **Source locations** come from the debug section ([13 §4.9](13-nir-spec.md))
  and the tag's frame walk; tracing diagnostics (path explosion, replay
  share, R5 in [01 §4.2](01-tracing-frontend.md)) use the same format.

### 4.2 Readable compile errors

`val<T>` misuse today produces template errors that are hard for people and
worse for agents. The public headers constrain their templates with named
concepts and `static_assert`s that say what is wrong (`val<T> requires an
arithmetic, bool, enum or pointer type; got std::string`), tested by a
compile-fail test suite.

### 4.3 Introspection

`CompiledModule::explain()` returns one structured report (also as JSON):
- per function: tier, backend, op counts, IR at each stage on request;
- compile statistics;
- why the module is not persistable (`artifact.ineligibleReason`), why a
  call was not inlined, which promotion status applies
  ([11 §4.3](11-tiered-compilation.md)).

Most of the data exists in 0.4's statistics; `explain()` is the single entry
point.

### 4.4 The playground as an MCP server

`tools/playground` already compiles Nautilus modules in a sandboxed runner and
shows every stage. An MCP server in front of the same runner gives agents the
tools:

| Tool | Does |
|---|---|
| `compile` | compile a module (C++ with `val<T>`, or NIR text) with chosen options; return diagnostics |
| `stages` | return the trace, NIR after conversion and after each pass, and backend output |
| `run` | execute a function with arguments; return the result and any exception |
| `benchmark` | compile-time statistics and execution time per backend |
| `explain` | the `explain()` report |

It runs locally by default (no network exposure) and reuses the playground's
sandbox for untrusted code.

### 4.5 A user plugin with skills

A Claude Code plugin shipped from the repository (`plugins/agent/` or a
separate marketplace entry) with skills for users:

| Skill | Content |
|---|---|
| `write-kernel` | idioms: `val` vs `static_val`, `region()`, explicit control flow, `invoke`, `constData`, effects and `nothrow` |
| `nautilus-review` | the existing review skill, updated for 0.4 |
| `debug-trace` | constant loops, path explosion, replay share, values escaping regions, by diagnostic code |
| `choose-backend` | latency vs. code quality, tiers, caches, artifacts |
| `migrate-0.3-to-0.4` | intrinsics → extensions and intrinsics, signless integers, renamed options, `bc` removal |

### 4.6 Documentation agents can use

- **`llms.txt`** at the repository root and a one-page API reference
  (`docs/api.md`), generated from the headers where possible.
- **Every code example in `docs/` compiles and runs in CI,** extracted from
  the Markdown, so documentation cannot drift from the API.

## 5. Milestones

| Milestone | Agent-experience work |
|---|---|
| M1 | doc check in CI; NIR text parser and FileCheck-style tests; verifier error codes; `ir-change` rewritten for NIR |
| M2 | tracing diagnostics with codes, locations and hints; `debug-trace` skill |
| M3 | compile-fail suite for the public headers; `extension-plugin` skill |
| M4 | `explain()`; `affected-tests`; `asm-diff` and `memory-check` skills |
| M5 | playground MCP server |
| M6 | diagnostics catalog complete; `docs/` examples compiled in CI |
| M7 | user plugin and its skills; `llms.txt` and `docs/api.md`; `migrate-0.3-to-0.4` |

## 6. Exit criteria

- The doc check passes, and every skill names only existing paths, targets
  and options.
- Every diagnostic code is documented and triggered by a test.
- Every `docs/` example compiles and runs in CI.
- The MCP server's tools are covered by end-to-end tests against the runner.
- A fresh cloud session goes from start to a green `dev` test run with only
  the commands in `AGENTS.md`.

## 7. Risks

| Risk | Mitigation |
|---|---|
| Skills and `AGENTS.md` drift from the code | the doc check in CI; updates in the same PR as the change |
| Diagnostics add cost to the hot path | codes and hints are built only when an error is raised; locations come from data recorded anyway |
| The MCP server executes arbitrary code | local by default; the playground's sandboxed runner |
| Too many skills dilute each other | each skill has a narrow trigger in its description; review the set each milestone |

# Component: C API

Part of the [Nautilus 0.4 design](README.md) · Milestone M5 · Builds on #531

## 1. Purpose

A C interface for building, inspecting, verifying, serializing and compiling
NIR without the C++ tracing frontend. It is designed to be used from C and
bound mechanically from other languages (Rust via bindgen, Python, Zig).

## 2. Carried over from #531

- **Headers:**

  | Header | Content |
  |---|---|
  | `nautilus/c/common.h` | versioning, status codes, per-thread last error, strings, options, executables |
  | `nautilus/c/ir.h` | modules, function builders, every operation kind, inspection, verification, optimization, compilation by backend name |
  | `nautilus/c/engine.h` | engines configured once, many compiles, per-compile option overrides |

- **FFI rules:**
  - frozen fixed-width enums; unknown values are rejected, never undefined
    behavior;
  - one error convention: constructors return a handle or `NULL`; other
    fallible calls return a `NautilusStatus` and write through
    out-parameters; no C++ exception crosses the API;
  - `(pointer, length)` strings in and out;
  - copy-out list accessors; accessors never allocate;
  - abandonable builders; verification before passes and backends;
  - thread-safety and lifetime contracts per handle;
  - `nautilus_c_api_version()` plus header macros.
- **`ENABLE_C_API_SHARED_LIBRARY`:** one `libnautilus-c` that exports only
  `nautilus_*` symbols, so no C++ or LLVM symbols leak into the binding's
  process.

## 3. What changes on NIR

- **Handles.** Functions, blocks and values are ids into the flat
  sections; builders append instructions, inspection reads records.
  - Ids are valid for the builder or module **revision** that produced
    them. Every mutating call (`nautilus_ir_optimize`,
    `nautilus_ir_canonicalize`, `nautilus_ir_save`, which canonicalizes)
    bumps the module revision, because compaction and canonicalization
    renumber ids ([13 §7](13-nir-spec.md)).
  - An id handle is `{u32 id, u32 revision}`. A stale handle is rejected
    with `NAUTILUS_STATUS_STALE_HANDLE`, never silently reinterpreted.
  - Module, builder, engine and executable handles are opaque pointers with
    an explicit `nautilus_*_destroy`. An executable keeps its module's code
    alive ([13 §4.4.1](13-nir-spec.md)); destroying the module handle first
    is allowed.
- **Opcodes and record layouts are versioned NIR wire values,** as defined in
  [13-nir-spec.md](13-nir-spec.md). The C enums
  map one-to-one, without #531's `switch` mapping layer between C and C++
  enums. Opcodes added later still surface as `NAUTILUS_IR_OP_UNKNOWN` to
  older bindings.
- **Extension ops and types** are addressed by name (`name@version`)
  through the registry ([04](04-extensions.md)). Their numeric ids are
  queried at run time.
- **Serialization:** `nautilus_ir_save`, `nautilus_ir_load` and
  `nautilus_ir_inspect` expose [07-serialization.md](07-serialization.md).
- **Load environment.** `nautilus_ir_load` and the compile calls take a
  `NautilusBindings` (binding name → address, [07 §4](07-serialization.md))
  and an optional resolver callback `(name, length, user) → address`
  playing the role of the C++ `SymbolResolver`; `nautilus_ir_save` takes an
  optional namer callback `(address, user) → (name, length)`, the
  `SymbolNamer`. Without a namer, external callees stay `ProcessLocal`
  pointers and the image is process-local ([07 §3](07-serialization.md));
  without the bindings an image that declares bindings is rejected with a
  status naming the first missing one.
- **Calls.** Compiled functions can throw: `invoke` keeps 0.3's exception
  semantics (D4) and checked division throws `ArithmeticException` (D10).
  A C++ exception unwinding through C frames is undefined behavior, so:
  - `nautilus_executable_call(exe, fn, args, nargs, ret)` calls a function
    through a C++ trampoline in the library that checks `args` (tagged
    `NautilusValue`s) against the function's signature, catches every
    exception, and returns `NAUTILUS_STATUS_EXCEPTION` with the exception's
    type and message in the per-thread last error;
  - `nautilus_executable_function_pointer` returns the raw entry point only
    for functions whose attributes include `noUnwind`
    ([13 §4.6.2](13-nir-spec.md), [06 §3](06-semantics.md)); for any other
    function it returns `NULL` and `NAUTILUS_STATUS_MAY_THROW`.
- **Levels:** C builders produce SSA-level NIR directly. The verifier
  rejects pre-SSA constructs.
- **Modules and call targets** follow [13 §4.4–§4.6](13-nir-spec.md): a
  builder names its module, marks functions exported or internal, declares
  imports by module name, function name, signature and assumed attributes,
  and adds external, imported and intrinsic callees, and read-only data
  objects. `nautilus_ir_save`
  writes a self-contained image that bundles every import
  ([13 §8.5](13-nir-spec.md)).

## 4. Exit criteria (M5)

- #531's test programs (strict C11) ported, on every backend.
- The shared-library export test is in the CI matrix.
- A save/load round trip through the C API, byte-identical to the C++ path,
  with a namer and a resolver: a C-built module with external callees saves
  a `Portable` image and reloads in a fresh process.
- A C program catches a division by zero and a throwing `invoke` callee as
  a status from `nautilus_executable_call`.
- A value handle kept across `nautilus_ir_optimize` is rejected with
  `NAUTILUS_STATUS_STALE_HANDLE`.

## 5. Risks

- **The frozen C enums tie down NIR opcode numbering.** New opcodes are
  appended only; renumbering requires a major NIR version.

## 6. Stability

- **Within 0.4.x the C API is source- and ABI-compatible:** enums are
  append-only, struct layouts do not change, functions are only added, and
  `nautilus_c_api_version()` is checked against the header macros when a
  binding initializes.
- **A NIR major version is a C API major version**
  ([13 §8.2](13-nir-spec.md)): it may renumber opcodes, so it changes the
  enums and therefore both source and ABI.

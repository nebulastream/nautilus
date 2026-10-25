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
    them. Every call that renumbers ids (`nautilus_ir_optimize`,
    `nautilus_ir_canonicalize`) bumps the module revision, because
    compaction and canonicalization renumber ids
    ([13 §7](13-nir-spec.md)). `nautilus_ir_save` canonicalizes a copy and
    leaves the module and its handles unchanged, so a finished module stays
    immutable and can be saved from several threads.
  - An id handle is `{u32 id, u32 revision}`. Revisions are drawn from one
    process-wide counter, so a handle used with another module is detected
    too. A stale or foreign handle is rejected with
    `NAUTILUS_STATUS_STALE_HANDLE`, never silently reinterpreted.
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
- **Naming and load environment** follow [07 §3](07-serialization.md)
  ("Where naming happens"):
  - `nautilus_engine_set_namer` installs a namer callback
    `(address, user) → (name, length)`, the C++ `SymbolNamer`, and
    `nautilus_engine_set_resolver` a resolver callback
    `(name, length, user) → address`, the `SymbolResolver`; both before the
    engine's first compile. A builder created for an engine applies its
    namer when it adds a `pointers` entry by address; an entry added by
    name is `Portable` (mode 1) directly.
  - `nautilus_ir_load` and the compile calls take a `NautilusBindings`
    (binding name → address, [07 §4](07-serialization.md)) and an optional
    resolver that replaces the engine's for that call; without the bindings
    an image that declares bindings is rejected with a status naming the
    first missing one.
  - `nautilus_ir_save` takes an optional namer that names only the entries
    still in mode 0; the image is re-checked for eligibility and written
    without native payloads if the namer changed it. Entries no namer names
    stay `ProcessLocal`, and the image is process-local.
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
    function it returns `NULL` and `NAUTILUS_STATUS_MAY_THROW`;
  - external callees written in C or another language are called from
    compiled code like any `invoke` target and must not unwind: a Rust
    panic or a `longjmp` across Nautilus frames is undefined. Bindings
    declare such callees `nothrow` ([13 §4.6.2](13-nir-spec.md)), which also
    lets their callers stay `noUnwind`. A may-throw external callee or
    `call.indirect` built through the C API names a capture thunk, a
    `pointers` entry of kind thunk that the builder adds by name and the
    resolver resolves (13 §5.7); may-throw intrinsic calls need none,
    because their thunk is registered with the intrinsic
    ([13 §4.6.1](13-nir-spec.md)).
- **Levels:** C builders produce SSA-level NIR directly. The verifier
  rejects pre-SSA constructs.
- **Diagnostics:** `nautilus_ir_verify`, `nautilus_ir_load` and the text
  parser report the stable code of [13 §6.6](13-nir-spec.md) (`NIR-V`,
  `NIR-L`, `NIR-P`) in the last error, as a string and as its numeric part,
  with the function and instruction or block id.
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
  with a namer and a resolver set on the engine: a C-built module with
  external callees saves a `Portable` image and reloads in a fresh process;
  the same module saved with only a `nautilus_ir_save` namer gives a
  `Portable` image without payloads.
- A C program catches a division by zero and a throwing `invoke` callee as
  a status from `nautilus_executable_call`.
- A value handle kept across `nautilus_ir_optimize`, or used with another
  module, is rejected with `NAUTILUS_STATUS_STALE_HANDLE`; one kept across
  `nautilus_ir_save` stays valid.

## 5. Risks

- **The frozen C enums tie down NIR opcode numbering.** New opcodes are
  appended only; renumbering requires a major NIR version.

## 6. Stability

- **Within 0.4.x the C API is source- and ABI-compatible:** enums are
  append-only, struct layouts do not change, functions are only added, and
  `nautilus_c_api_version()` is checked against the header macros when a
  binding initializes: the majors must be equal and the library's minor at
  least the header's. A binding built against an older minor sees later
  enum values as `*_UNKNOWN` (§3).
- **New status codes** (`NAUTILUS_STATUS_STALE_HANDLE`,
  `NAUTILUS_STATUS_MAY_THROW`, `NAUTILUS_STATUS_EXCEPTION` and later ones)
  are appended; a binding treats an unknown status as a failure.
- **A NIR major version is a C API major version**
  ([13 §8.2](13-nir-spec.md)): it may renumber opcodes, so it changes the
  enums and therefore both source and ABI.

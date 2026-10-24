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

- **Handles are `u32` ids into the flat sections:** functions, blocks and
  values. Builders append instructions; inspection reads records.
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
- **Levels:** C builders produce SSA-level NIR directly. The verifier
  rejects trace-level constructs.
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
- A save/load round trip through the C API, byte-identical to the C++ path.

## 5. Risks

- **The frozen C enums tie down NIR opcode numbering.** New opcodes are
  appended only; renumbering requires a major NIR version.

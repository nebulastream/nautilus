# C API

Nautilus has a C API for building, inspecting and compiling Nautilus IR without
the C++ tracing frontend. It is meant to be used from C directly, and to be
bound from other languages (Rust, Python, Zig, ...).

| Header | Contents |
|---|---|
| `nautilus/c/common.h` | Versioning, status codes and the last error, strings, options, executables |
| `nautilus/c/ir.h` | Graphs, function builders, instruction building, inspection, optimization, compilation by backend name |
| `nautilus/c/engine.h` | Engines: configure once, compile many graphs with the engine's backend and options |

The API is available whenever the library is built with `ENABLE_TRACING` (the
default). With `-DENABLE_C_API_SHARED_LIBRARY=ON`, the build also produces
`libnautilus-c`, a shared library that exports the C API and nothing else (see
[Bindings](#bindings)).

## Example

```c
#include <nautilus/c/engine.h>

/* int64_t sum_to(int64_t n): 0 + 1 + ... + (n - 1) */
NautilusIRGraphRef g = nautilus_ir_graph_create(nautilus_string_ref("example"));
NautilusIRFunctionBuilderRef fb =
    nautilus_ir_function_builder_create(g, nautilus_string_ref("sum_to"), NAUTILUS_IR_TYPE_I64);

NautilusIRType n[] = {NAUTILUS_IR_TYPE_I64};
NautilusIRType loop[] = {NAUTILUS_IR_TYPE_I64, NAUTILUS_IR_TYPE_I64, NAUTILUS_IR_TYPE_I64}; /* i, acc, n */
NautilusIRBlockRef entry = nautilus_ir_function_builder_add_block(fb, n, 1); /* params = entry args */
NautilusIRBlockRef header = nautilus_ir_function_builder_add_block(fb, loop, 3);
NautilusIRBlockRef body = nautilus_ir_function_builder_add_block(fb, loop, 3);
NautilusIRBlockRef done = nautilus_ir_function_builder_add_block(fb, n, 1);

NautilusIRValueRef zero = nautilus_ir_build_const_int(fb, entry, 0, NAUTILUS_IR_TYPE_I64);
NautilusIRValueRef init[] = {zero, zero, nautilus_ir_block_get_argument(entry, 0)};
nautilus_ir_build_branch(fb, entry, header, init, 3);

NautilusIRValueRef i = nautilus_ir_block_get_argument(header, 0);
NautilusIRValueRef acc = nautilus_ir_block_get_argument(header, 1);
NautilusIRValueRef lim = nautilus_ir_block_get_argument(header, 2);
NautilusIRValueRef cond = nautilus_ir_build_compare(fb, header, NAUTILUS_IR_CMP_LT, i, lim);
NautilusIRValueRef toBody[] = {i, acc, lim}, toDone[] = {acc};
nautilus_ir_build_if(fb, header, cond, body, toBody, 3, done, toDone, 1, 0.9);

/* body: acc += i; i += 1; loop */
NautilusIRValueRef bi = nautilus_ir_block_get_argument(body, 0);
NautilusIRValueRef one = nautilus_ir_build_const_int(fb, body, 1, NAUTILUS_IR_TYPE_I64);
NautilusIRValueRef next[] = {
    nautilus_ir_build_binary(fb, body, NAUTILUS_IR_BINARY_ADD, bi, one),
    nautilus_ir_build_binary(fb, body, NAUTILUS_IR_BINARY_ADD, nautilus_ir_block_get_argument(body, 1), bi),
    nautilus_ir_block_get_argument(body, 2)};
nautilus_ir_build_branch(fb, body, header, next, 3);

nautilus_ir_build_return(fb, done, nautilus_ir_block_get_argument(done, 0));
if (!nautilus_ir_function_builder_finish(fb)) {
    NautilusStringRef msg = nautilus_last_error_message();
    fprintf(stderr, "%.*s\n", (int) msg.length, msg.data);
}

NautilusEngineRef engine = nautilus_engine_create(NULL);
NautilusExecutableRef exe = nautilus_engine_compile(engine, g, NULL);
void* fn = NULL;
nautilus_executable_get_function(exe, nautilus_string_ref("sum_to"), &fn);
((int64_t (*)(int64_t)) fn)(10); /* 45 */

nautilus_executable_dispose(exe);
nautilus_engine_dispose(engine);
nautilus_ir_graph_dispose(g);
```

## Conventions

The API follows one set of rules everywhere, so it can be bound mechanically
and wrapped safely. `common.h` states them in full.

- **ABI-stable types.** Every enumeration is a fixed-width integer typedef with
  explicit constants whose values never change meaning. They are independent
  of the C++ enumerations, which stay free to change. An unknown value is
  rejected with `NAUTILUS_ERROR_INVALID_ARGUMENT`, never undefined behavior,
  and `NAUTILUS_IR_OP_UNKNOWN` covers operation kinds added later. Booleans are
  C99 `bool`. Bit sets such as `NautilusIRFunctionFlags` reject unknown bits.
- **Errors.** A function that creates an object returns its handle, or NULL on
  failure. Any other function that can fail returns a `NautilusStatus`
  (`NAUTILUS_OK` or a specific `NAUTILUS_ERROR_*`) and writes its results
  through out-parameters, which are left untouched on failure. Plain accessors
  cannot fail. Every failure also records its status and message for the
  calling thread, readable with `nautilus_last_error_code()` and
  `nautilus_last_error_message()`. No C++ exception crosses the API.
- **Strings.** Inputs are `NautilusStringRef {data, length}` and need not be
  NUL-terminated; `nautilus_string_ref()` wraps a C string. Borrowed outputs
  are `NautilusStringRef` too, and are also NUL-terminated. Owned outputs are
  `NautilusString`, released with `nautilus_string_dispose()`.
- **Lists.** These are read with copy-out accessors:
  `size_t f(handle, T* out, size_t capacity)` copies what fits and returns the
  total, so `f(h, NULL, 0)` sizes the buffer. A whole block, function or
  operand list costs two calls, not one call per element.
- **Handles.** Every object is an opaque pointer typedef. Owning handles have a
  `_dispose` function that accepts NULL; everything else is borrowed, and its
  owner is documented. Function, block and value refs are the IR's own
  pointers, so handing them out costs nothing.

## Building IR

- **SSA with block arguments.** There are no phi nodes. A block declares typed
  arguments, and every `branch` or `if` passes values for them. The entry
  block's arguments are the function's parameters, and the entry block cannot
  be branched to.
- **Terminators.** Every block ends in exactly one `branch`, `if` or `return`.
  Nothing can be appended after it, and `finish` rejects a block without one.
- **Calls.** `nautilus_ir_build_call` targets a `NautilusIRCalleeId`. A
  function's id is available before the function is finished
  (`nautilus_ir_function_builder_get_callee`), so functions can call each other
  and themselves. Native functions are declared with
  `nautilus_ir_graph_declare_external_function` and must not throw C++
  exceptions. `nautilus_ir_build_indirect_call` calls through a pointer value.
- **Stack memory.** `nautilus_ir_function_builder_add_stack_slot` reserves a
  frame slot, and `nautilus_ir_build_alloca` yields a pointer to it.
- **Abandoning.** `nautilus_ir_function_builder_dispose` drops an unfinished
  function. A failed `finish` consumes the builder too.
- **Checking.** Builders check what they can locally: operand types, branch
  and call arity, block ownership, terminators. `nautilus_ir_graph_verify` runs
  the full IR verifier, and optimizing or compiling runs it automatically, so a
  malformed graph is reported (`NAUTILUS_ERROR_VERIFICATION_FAILED`) rather
  than reaching a backend.

## Optimization and compilation

There are two ways to compile a graph:

- `nautilus_ir_graph_compile(graph, backend, options)` compiles with a named
  backend (an empty name picks the build's default).
- `nautilus_engine_compile(engine, graph, overrides)` compiles with an engine,
  the C counterpart of `NautilusEngine`. The engine takes the engine-wide
  options once (`engine.backend` and every other option in
  [options.md](options.md)), so the configuration lives in one place.
  `overrides` are layered on top for one compile, like per-module options in
  C++. A graph compiles synchronously with the engine's primary backend: the
  one `engine.backend` pins, or the tier-1 backend of a tiered engine. A
  prebuilt graph has nothing to promote between tiers. Compilation statistics
  are collected as for traced modules, and `engine.logStatistics` logs them.
  The same entry point is available from C++ as `NautilusEngine::compileIR`.

Both run the IR pass pipeline first, at the level the backend asks for.
`nautilus_ir_graph_optimize` runs it on its own, e.g. to inspect the optimized
IR or choose the level. The pipeline runs at most once per graph and rewrites
it in place, so refs taken before may dangle afterwards, and the graph accepts
no new functions.

Executables are independent of the graph and engine that produced them.

## Bindings

**Linking.** Build with `-DENABLE_C_API_SHARED_LIBRARY=ON` and link
`libnautilus-c`. It contains all of Nautilus and its dependencies and exports
only the `nautilus_*` functions. A binding then links one library, without
C++ standard-library or LLVM symbols clashing with its own. The option builds
everything position-independent; when the MLIR backend is enabled, the MLIR
libraries must be built position-independent too (LLVM's default).

**Version check.** Compare `nautilus_c_api_version()` with the
`NAUTILUS_C_API_VERSION_*` macros the bindings were generated from. The major
versions must match, and the library's minor version must be at least the
bindings'.

**Mapping to Rust.** The API is shaped so `bindgen` output can be wrapped
without guesswork:

| C | Rust |
|---|---|
| `NautilusStatus` + last error | `Result<T, Error { code, message }>`; read the message right after the failing call, on the same thread |
| Handle-returning constructor | `Option<NonNull<_>>` → `Result` |
| Owning handles (`Graph`, `Engine`, `Executable`, `Options`, `FunctionBuilder`) | Newtypes with `Drop` calling `_dispose` |
| Function, block and value refs | `Copy` newtypes with a lifetime tied to `&Graph`; `optimize`/`compile` take `&mut Graph`, so stale refs cannot outlive the rewrite |
| `NautilusStringRef` | `&str` in (no allocation), `&str` out (borrowed from its owner) |
| `NautilusString` | `String`, copied and then disposed |
| Copy-out accessors | `Vec<T>`, from one sizing call and one fill call |
| Enumerations (`uint32_t` + constants) | `#[non_exhaustive]` Rust enums, converting unknown values to an `Unknown` variant |

Thread safety maps directly: a graph (with its builders and refs) is `Send`
but not `Sync`. An engine and an executable are `Send + Sync`. Options are
`Send`. The last error is thread-local.

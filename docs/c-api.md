# C API for the Nautilus IR

`#include <nautilus/c/ir.h>` gives C (and anything with a C FFI: Rust, Python
`ctypes`, Zig, ...) direct access to the Nautilus IR, without going through the
C++ tracing frontend. A program can:

- **build** IR functions block by block,
- **inspect** any graph: functions, blocks, operations, operands, successors,
  constants and the function table,
- **verify** it with the IR verifier,
- **optimize** it with the same pass pipeline traced code goes through, and
- **compile** it with any backend in the build (`mlir`, `cpp`, `bc`, `tbc`,
  `asmjit`) into native function pointers.

The API is available whenever the library is built with `ENABLE_TRACING`
(the default).

## Example

```c
#include <nautilus/c/ir.h>

int64_t sum_to(int64_t n); /* 0 + 1 + ... + (n - 1) */

NautilusIRGraphRef g = nautilus_ir_graph_create("example");
NautilusIRFunctionBuilderRef fb = nautilus_ir_function_builder_create(g, "sum_to", NAUTILUS_IR_TYPE_I64);

NautilusIRType n[] = {NAUTILUS_IR_TYPE_I64};
NautilusIRType loop[] = {NAUTILUS_IR_TYPE_I64, NAUTILUS_IR_TYPE_I64, NAUTILUS_IR_TYPE_I64}; /* i, acc, n */
NautilusIRBlockRef entry = nautilus_ir_function_builder_add_block(fb, n, 1);   /* params = entry args */
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
NautilusIRValueRef toBody[] = {i, acc, lim}, toExit[] = {acc};
nautilus_ir_build_if(fb, header, cond, body, toBody, 3, done, toExit, 1, 0.9);

/* body: acc += i; i += 1; loop */
NautilusIRValueRef bi = nautilus_ir_block_get_argument(body, 0);
NautilusIRValueRef one = nautilus_ir_build_const_int(fb, body, 1, NAUTILUS_IR_TYPE_I64);
NautilusIRValueRef next[] = {
    nautilus_ir_build_binary(fb, body, NAUTILUS_IR_BINARY_ADD, bi, one),
    nautilus_ir_build_binary(fb, body, NAUTILUS_IR_BINARY_ADD, nautilus_ir_block_get_argument(body, 1), bi),
    nautilus_ir_block_get_argument(body, 2)};
nautilus_ir_build_branch(fb, body, header, next, 3);

nautilus_ir_build_return(fb, done, nautilus_ir_block_get_argument(done, 0));
nautilus_ir_function_builder_finish(fb);

NautilusIRExecutableRef exe = nautilus_ir_graph_compile(g, "mlir", NULL);
int64_t (*fn)(int64_t) = (int64_t (*)(int64_t)) nautilus_ir_executable_get_function(exe, "sum_to");
fn(10); /* 45 */

nautilus_ir_executable_dispose(exe);
nautilus_ir_graph_dispose(g);
```

## Concepts

| C handle | Nautilus IR | Lifetime |
|---|---|---|
| `NautilusIRGraphRef` | `IRGraph` (a module) | owned by the caller; `nautilus_ir_graph_dispose` |
| `NautilusIRFunctionBuilderRef` | a function under construction | consumed by `nautilus_ir_function_builder_finish` |
| `NautilusIRFunctionRef` | `FunctionOperation` | borrowed from the graph |
| `NautilusIRBlockRef` | `BasicBlock` | borrowed from the graph |
| `NautilusIRValueRef` | any `Operation`, including block arguments | borrowed from the graph |
| `NautilusIRCalleeId` | `FunctionId` in the graph's function table | plain integer |
| `NautilusIRExecutableRef` | `Executable` | owned by the caller; outlives the graph |

- **SSA with block arguments.** There are no phi nodes: a block declares typed
  arguments, and every `branch`/`if` passes values for them. The entry block's
  arguments are the function's parameters, and the entry block cannot be
  branched to.
- **Terminators.** Every block ends in exactly one `branch`, `if` or `return`;
  nothing can be appended after it, and `finish` rejects a block without one.
- **Calls.** `nautilus_ir_build_call` targets a `NautilusIRCalleeId`. A
  function's id exists as soon as its builder is created
  (`nautilus_ir_function_builder_get_callee`), so functions can call each other
  and themselves before they are finished. Native functions are declared with
  `nautilus_ir_graph_declare_external_function`; they must not throw C++
  exceptions. `nautilus_ir_build_indirect_call` calls through a pointer value.
- **Stack memory.** `nautilus_ir_function_builder_add_stack_slot` reserves a
  frame slot; `nautilus_ir_build_alloca` yields a pointer to it for `load` and
  `store`.
- **Errors.** Builders check what they can locally (types of operands, arity of
  branch and call arguments, block ownership, terminators) and return `NULL`
  with a message in `nautilus_ir_get_last_error()`; no C++ exception crosses
  the API. `nautilus_ir_graph_verify` runs the full IR verifier, which also
  catches cross-function operands and uses that do not follow their definition.

## Optimization and compilation

`nautilus_ir_graph_compile` runs the IR pass pipeline at the level the chosen
backend asks for, then the backend. `nautilus_ir_graph_optimize` runs the
pipeline on its own, e.g. to inspect the optimized IR. Both rewrite the graph
in place: block and value refs taken before may be stale afterwards, and the
graph accepts no new functions. The pipeline runs once per graph.

Options from [options.md](options.md) are passed through a
`NautilusIROptionsRef` (`nautilus_ir_options_set_bool/int/double/string`), for
example `dump.all` to write the IR and backend dumps, or `ir.enableLICM`.

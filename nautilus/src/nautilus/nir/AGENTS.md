# AGENTS.md: NIR (`nautilus/src/nautilus/nir/`)

NIR is the Nautilus IR of 0.4. The design is in `docs/plans/nautilus-0.4/` (start at `02-nir.md`); **the
normative definition is `13-nir-spec.md`**, and where code and spec differ the spec wins. A change to the format
follows the spec's change control (§11): change the spec first, in the same PR.

Since the cut-over (plan PR 6) the engine's default pipeline traces to NIR and converts it (`tracing/nir/`,
`conversion/`, driven by `compiler/CompilationPipeline.cpp`); `tracing.nirTracer=false` selects the 0.3 pipeline. `tbc`
compiles NIR natively; `asmjit`, `mlir` and `cpp` get the 0.3 IR built from it by `compiler/NirToIR.cpp` until their
ports. It is the static library `nautilus-nir`; its tests are `nautilus-nir-tests`; the tool is `nautilus-nir-opt`.

## Files

| File | Content | Spec |
|---|---|---|
| `Opcodes.def`, `Diagnostics.def` | the opcode table and the diagnostic codes, as X-macros; `Diagnostics.def` is generated from the table of §6.6 | §5, §6.6 |
| `Types.hpp`, `Inst.hpp`, `Records.hpp` | types, the 16-byte `Inst`, value references, every section record with its `static_assert`ed size | §2-§4 |
| `Module.*`, `ConstPool.*`, `Builder.*`, `Rewriter.*`, `Uses.*`, `Operands.hpp` | the module as flat arrays, the interning tables, building, copy-with-forward-map, the CSR use table, the walker over an instruction's value operands | §3, §4 |
| `Verifier.*`, `VerifierFunction.cpp` | `verify`: module tables, then each function (structure, types, levels, dominance, layout) | §6 |
| `Printer.*`, `Parser.*` | the text form | §9 |
| `Canonicalize.*`, `Hash.*` | canonical form, structural hash (XXH3-128, vendored in `third_party/xxhash`), key of inlined imports | §7 |
| `Image.*`, `ImageLoader.*`, `Inspect.*` | the binary image: writer, loader (`NIR-L` codes), description without materializing | §8 |
| `../common/OptionRegistry.*` | the typed option registry; compiled into `nautilus-nir` for now | M1.4a |

## Rules that are easy to break

- **One opcode table.** A new opcode is a line in `Opcodes.def` (and the spec); the verifier, printer, parser,
  canonicalizer and rewriter each have a `switch` over `Op` that the compiler will point at.
- **Codes are never renumbered or reused.** Every verifier check has a `NIR-V` code and a positive and a negative test
  (`nautilus/test/nir-tests/Verifier*Test.cpp`); a new check adds a code to the spec's table and to `Diagnostics.def`.
- **The verifier must survive any module.** It runs on bytes from the loader and the parser. Check a table index
  before you read through it, and gate analyses that follow lists (dominance, definite assignment) on a function that
  has no structural diagnostics (`clean()`). The garbage-record test in `ImageTest.cpp` and the fuzzer
  (`nautilus/test/fuzz/NirFuzzTarget.hpp`) have each already found a crash of this kind.
- **Printing is a fixed function of the module**; `print(parse(t)) == t` for text the printer wrote. Change the printer
  and the corpus files in `nautilus/test/data/nir-conformance/text` change with it: regenerate them with
  `nautilus-nir-opt file.nir > file.nir`, in a commit of their own.
- **Canonical form is the hash input.** Anything that changes `canonicalize` or the image layout changes every cache key:
  that is a format change (spec §8.2), including the golden images (`NIR_UPDATE_GOLDEN=1 nautilus-nir-tests` rewrites
  them; review the diff).
- **Only SSA-level modules are canonicalized and hashed.** Pre-SSA modules are written as they are, for reproducers.

## Testing

```bash
cmake --build build/dev --target nautilus-nir-tests nautilus-nir-opt
./build/dev/nautilus/test/nir-tests/nautilus-nir-tests            # unit tests, corpus, golden images
ctest --test-dir build/dev -R '^nir-'                              # the .nir files, one ctest each
./build/dev/nautilus/test/nir-tests/nautilus-nir-tests "[verifier]"
```

`nautilus-nir-opt [--hash] [--image=FILE] [--inspect[=hex]] [file | -]` parses text or reads an image, verifies it
and prints it back; use it to write and canonicalize `.nir` tests.

The format readers are fuzzed: `nautilus-nir-fuzz` (libFuzzer, `-DENABLE_FUZZING=ON`) and `nautilus-nir-fuzz-replay`
(any compiler, `-DENABLE_FUZZER_REPLAY=ON`, `--mutate N` for a deterministic mutation run; build it with
`-DENABLE_ADDRESS_SANITIZER=ON` to make it useful). Seeds are in `nautilus/test/fuzz/corpus-nir`.

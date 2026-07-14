# Copy-and-Patch Stencil Toolchain

`build_stencils.py` regenerates the checked-in stencil tables used by the tbc
backend's copy-and-patch JIT (`tbc.mode=jit`):

```
nautilus/src/nautilus/compiler/backends/tbc/jit/generated/
  stencils_x86_64_elf.inc      x86-64 Linux (ELF)
  stencils_aarch64_elf.inc     aarch64 Linux (ELF)
  stencils_arm64_macho.inc     arm64 macOS (Mach-O)
```

Each table holds, per opcode, the machine code of its handler (a *stencil*)
plus a list of *holes* — relocation sites the runtime stitcher
(`jit/TBCStitcher.cpp`) patches per instruction with operand offsets, branch
targets, call-site record addresses, and helper addresses.

## Regenerating

Normal builds never run this tool: the `.inc` files are committed. Regenerate
after changing `jit/TBCStencilSource.cpp`, `TBCOpcodes.hpp`'s opcode lists, or
the pinned compile flags:

```bash
python3 tools/stencils/build_stencils.py          # all targets
python3 tools/stencils/build_stencils.py --only x86_64_elf
cmake --build build --target tbc-regen-stencils   # same, via CMake
```

Requirements: **clang++ ≥ 19** (`preserve_none`) and a matching
`llvm-readobj` on PATH (auto-detected, override with `--clang`/`--readobj`).
Cross-compilation needs no sysroots — the stencil TU is freestanding.

## How it works

1. `jit/TBCStencilSource.cpp` is compiled per target triple with a pinned
   flag set (`-O2 -ffreestanding -fno-pic -mcmodel=large -fno-jump-tables
   -fno-exceptions -fno-asynchronous-unwind-tables -ffunction-sections`, …).
   `-mcmodel=large` forces every symbol reference into a patchable absolute
   materialization (x86-64: one `movabs` per hole, relocation `R_X86_64_64`;
   aarch64 ELF: `movz/movk` chains + direct `b`/`bl`). Mach-O arm64 ignores
   the code model and routes every reference through the GOT
   (`ARM64_RELOC_GOT_LOAD_PAGE21/PAGEOFF12`); the stitcher emulates the GOT
   with per-instruction value slots inside the stitched span.
2. The object file is parsed (llvm-readobj JSON for ELF, a built-in Mach-O
   parser otherwise); every `stencil_*` function's bytes and relocations are
   harvested. The extractor is **strict**: an unexpected relocation kind,
   undefined symbol, or data/bss section is a hard error at generation time —
   compiler-output surprises can never reach the runtime.
3. The `.inc` tables are emitted keyed by opcode *name*. The runtime resolves
   names against the current `Op` enum, so a stale table degrades safely: a
   program touching an opcode with no stencil falls back to the interpreter
   (see `TBCStitcher.cpp`'s bail-out), it never mis-executes.

The extractor also locates each stencil's trailing "jump to next instruction"
sequence so the stitcher can drop it when instructions are stitched
adjacently (fall-through elision).

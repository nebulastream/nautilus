# Nautilus IR — VS Code Extension

A first-class editor experience for the intermediate representation (IR)
emitted by [Nautilus](https://github.com/nebulastream/nautilus), the tracing
JIT compiler used by NebulaStream.

This extension turns plain `.nesir` / `.trace` IR dumps from the Nautilus
test suite, the engine's `dump.ir = true` option, or any custom pipeline
into a navigable, debuggable artifact.

## Features

### Editing

- **Syntax highlighting** for the full Nautilus IR grammar — types
  (`i8…i64`, `ui8…ui64`, `f32`, `f64`, `bool`, `ptr`, `void`), arithmetic,
  bitwise, logical, and comparison operators, control flow (`if`, `br`,
  `return`, `cmp`, `jmp`), memory (`load`, `store`, `alloca`), casts
  (`cast_to`, `CAST`), function pointer calls (`func_*`), block headers
  (`Block_N(...)`, `B0(...)`), the `NautilusIr` / `execute()` wrapper, and
  the `//NESIR` end marker.
- **Bracket matching, auto-close, smart indent** for `{}`, `()`, `[]`.
- **Code folding** for the module wrapper, the `execute()` body, and every
  basic block.
- **Snippets** (`nesir`, `block`, `br`, `if`, `const`, `cast`, `load`,
  `store`, `ret`, `call`) for quickly authoring IR by hand.

### Navigation

- **Document outline / breadcrumbs** — every basic block becomes a top-level
  symbol; every SSA definition (`$N = ...`) is a child symbol with its
  type. Use the outline view, breadcrumb dropdowns, and **Go to Symbol in
  File** (`Ctrl+Shift+O`).
- **Go to Definition** (`F12`) on `$N` jumps to its assignment; on
  `Block_N` it jumps to the block header.
- **Find All References** (`Shift+F12`) for SSA values and block names.
- **Document highlight** — placing the caret on `$N` or `Block_N` paints
  every occurrence (definition vs. use distinguished).
- **Rename Symbol** (`F2`) renames an SSA value consistently across the
  file, including its block-argument occurrences.
- **Hover** shows the SSA value's defining expression, type, and use count;
  for blocks it lists arguments, predecessors, successors, and instruction
  count; for types and operations it shows inline docs.
- **CodeLens** above every block: `N preds · M succs · K insn`.
- **Status bar** indicator for the block currently containing the cursor,
  with a quick tooltip of its CFG context.
- **Quick block jumper** — `Ctrl+Shift+B` (`Cmd+Shift+B` on macOS) opens a
  Quick Pick listing every block with its successor summary.

### Diagnostics

Light static checks are surfaced in the Problems panel:

- Undefined SSA references (`$N` used without an assignment or block-arg
  binding).
- Branches that target a block that does not exist in the file.
- Blocks with no `return` / `br` / `if` terminator.

### Debugging (gdb)

The extension contributes a `nautilus-ir-gdb` debug adapter that drives
`gdb --interpreter=mi2` underneath, exposing the full VS Code debugging
UI for Nautilus IR files:

- Set/clear breakpoints in the IR gutter (with conditions and hit counts).
- Step over / into / out, continue, pause, restart.
- Live **Variables** view (locals, arguments, registers).
- **Watch** and **Hover** evaluation through `data-evaluate-expression`.
- Stack trace, call hierarchy navigation across frames.
- Console output forwarded from gdb (including target stdout/stderr).
- Conditional & log-point breakpoints.

#### Quick start

1. Open a `.nesir` / `.trace` file.
2. Click **Run and Debug** in the side bar, then **Nautilus IR (gdb)**.
3. The pre-supplied launch configuration runs

   ```jsonc
   {
       "type": "nautilus-ir-gdb",
       "request": "launch",
       "name": "Debug Nautilus IR (gdb)",
       "program": "${file}",
       "stopOnEntry": true,
       "cwd": "${workspaceFolder}"
   }
   ```

   You can also right-click the file and pick **Nautilus IR: Debug Current
   File with gdb**.

#### Configuration options

| Setting                              | Default | Purpose                                                                          |
|--------------------------------------|---------|----------------------------------------------------------------------------------|
| `nautilusIr.gdbPath`                 | `gdb`   | Path to the gdb executable.                                                      |
| `nautilusIr.gdbExtraArgs`            | `[]`    | Extra args appended to every gdb invocation.                                     |
| `nautilusIr.highlightDefinitions`    | `true`  | Highlight all uses of an SSA value when the cursor is on its definition.         |
| `nautilusIr.codeLens`                | `true`  | Show CodeLens above every block.                                                 |
| `nautilusIr.showInlayTypes`          | `true`  | Reserved for future inlay-hint support.                                          |

You can also override `gdbPath` and `gdbArgs` per launch configuration.

### Commands

| Command                                          | Default keybinding         |
|--------------------------------------------------|----------------------------|
| `Nautilus IR: Go to Block...`                    | `Ctrl/Cmd+Shift+B`         |
| `Nautilus IR: Show File Statistics`              | —                          |
| `Nautilus IR: Toggle SSA Definition Highlight`   | —                          |
| `Nautilus IR: Debug Current File with gdb`       | —                          |

## Building from source

```bash
cd tools/vscode-nautilus-ir
npm install
npm run compile           # tsc → ./out
# Open this folder in VS Code and press F5 to launch an Extension Host.
```

To package a `.vsix`:

```bash
npm install -g @vscode/vsce
vsce package
```

## File associations

The extension registers itself for `.nesir`, `.trace`, and `.nir`
extensions. If your project keeps `.trace` files used by another tool, set

```jsonc
"files.associations": {
    "**/test/data/**/ir/*.trace": "nautilus-ir",
    "**/test/data/**/after_ssa/*.trace": "nautilus-ir",
    "**/test/data/**/after_constant_folding/*.trace": "nautilus-ir"
}
```

so only Nautilus traces opt in.

## Example

Open `nautilus/test/data/loop-tests/ir/collatz.trace` and you should see
syntax-highlighted blocks, a populated outline, and CodeLens annotations
above each `Block_N(...)` showing predecessor and successor counts.

## License

MIT — same as the Nautilus project itself.

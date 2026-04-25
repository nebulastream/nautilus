# Nautilus IR — VS Code Extension

A first-class editor experience for the intermediate representation (IR)
emitted by [Nautilus](https://github.com/nebulastream/nautilus), the tracing
JIT compiler used by NebulaStream.

This extension turns plain `.nautilus` IR dumps from the Nautilus test
suite, the engine's `dump.ir = true` option, or any custom pipeline into
a navigable, debuggable artifact.

## Features

### Editing

- **Syntax highlighting** for the full Nautilus IR grammar as emitted into
  `nautilus/test/data/*/ir/*.nautilus` — primitive types
  (`i8…i64`, `ui8…ui64`, `f32`, `f64`, `bool`, `ptr`, `void`), arithmetic,
  bitwise, logical, comparison and unary (`~`, `!`) operators, control
  flow (`if … ? … : …`, `br`, `return`), memory (`load`, `store`,
  `alloca Nb`), casts (`cast_to TYPE`), runtime function pointer calls
  (`func_*(…)`), block headers (`Block_N(...):`), bare-typed
  declarations (`$N :type`), the `nautilus { … }` module wrapper,
  arbitrary function definitions (`execute()`, `add()`, `inner()`,
  `loopHelper()`, …), and the `//nautilus` end marker. Validated
  against all 439 IR fixtures in the test corpus with 100% token
  coverage.

  > Note: the alternate "mnemonic" dialect that appears in
  > `*/after_ssa/*.trace` and `*/tracing/*.trace` (`EXECUTE: B0(…)
  > CONST $3 5 :i32 …`) is not handled by this extension. Open those
  > files as plain text.
- **Bracket matching, auto-close, smart indent** for `{}`, `()`, `[]`.
- **Code folding** for the module wrapper, the `execute()` body, and every
  basic block.
- **Snippets** (`nesir`, `block`, `br`, `if`, `const`, `cast`, `load`,
  `store`, `ret`, `call`) for quickly authoring IR by hand.

### Navigation

- **Document outline / breadcrumbs** — top level: every IR function in
  the module, rendered with its typed signature
  (e.g. `add($1:i32, $2:i32) :i32`). A single file may contain
  `execute()` plus helpers like `add()`, `inner()`, `loopHelper()`, etc.
  Children: basic blocks. Grandchildren: SSA definitions with types.
  Use the outline view, breadcrumb dropdowns, and **Go to Symbol in
  File** (`Ctrl+Shift+O`).
- **Go to Definition** (`F12`) on `$N` jumps to its assignment; on
  `Block_N` it jumps to the block header.
- **Find All References** (`Shift+F12`) for SSA values and block names.
  References are scoped to the cursor's enclosing function — `$3` in
  `add()` is correctly distinguished from `$3` in `execute()`.
- **Document highlight** — placing the caret on `$N` or `Block_N` paints
  every occurrence within the enclosing function (definition vs. use
  distinguished).
- **Rename Symbol** (`F2`) renames an SSA value consistently across its
  enclosing function, including its block-argument occurrences. Other
  functions in the same module are untouched.
- **Hover** shows the SSA value's defining expression, type, and use count;
  for blocks it lists arguments, predecessors, successors, and instruction
  count; for types and operations it shows inline docs.
- **CodeLens** above every block: `N preds · M succs · K insn`.
- **Status bar** indicator for the block currently containing the cursor,
  with a quick tooltip of its CFG context.
- **Quick block jumper** — `Ctrl+Shift+B` (`Cmd+Shift+B` on macOS) opens a
  Quick Pick listing every block with its successor summary.

### Graph view

Run **Nautilus IR: Show Graph** (palette, editor title bar, or right-click
menu) to open a side-by-side control-flow graph of the active `.nautilus`
file. The graph is synthesized in the extension from the parsed IR text, so
it works on any IR dump — there is no dependency on Nautilus's
`dump.graph=true` option.

- **Click a block node** to jump to its `Block_N(...)` header in the editor.
- **Move the cursor** in the editor to highlight the enclosing block in the
  graph; switching between functions in the file flips the rendered graph
  automatically.
- **Function picker** in the toolbar selects which function to render when
  the file contains multiple (`execute`, helpers, etc.).
- **Pan & zoom** with the mouse; **Fit** / **Reset** buttons restore the
  default view.

The graph shape conveys terminator type at a glance:

| Shape       | Meaning                                |
|-------------|----------------------------------------|
| Rectangle   | Block ends in `br` (unconditional jump) |
| Diamond     | Block ends in `if` (conditional)        |
| Stadium     | Block ends in `return`                  |

Conditional edges are labeled `true` / `false`. Entry blocks (`Block_0`)
are tinted teal; return blocks are tinted pink, mirroring the palette of
Nautilus's own C++ Mermaid dumper.

### Diagnostics

Light static checks are surfaced in the Problems panel:

- Undefined SSA references (`$N` used without an assignment or block-arg
  binding).
- Branches that target a block that does not exist in the file.
- Blocks with no `return` / `br` / `if` terminator.

### Debugging (gdb, via the C++ host program)

> **Important model.** A Nautilus IR file is **not** a standalone executable.
> It is loaded by a C++ host program (an `ExecutionTest` binary, the demo
> JIT, a NebulaStream worker, your own embedding, …) which JIT-compiles
> the IR via the Nautilus engine. To debug an IR file you launch *that
> host program* under gdb. Once execution enters the JIT'd code, gdb
> uses the source-line debug info that Nautilus emits to map back to
> file/line positions in the IR file — at which point all the usual
> gdb facilities (gutter breakpoints, step, variables, watch) light up.

The extension contributes a `nautilus-ir-gdb` debug adapter that drives
`gdb --interpreter=mi2` underneath:

- Set/clear breakpoints in the IR gutter (with conditions, hit counts,
  and log-points). Breakpoints are inserted with `-break-insert -f` so
  they remain pending until the JIT registers symbols for the IR file.
- Step over / into / out, continue, pause, restart.
- Live **Variables** view (locals, arguments, registers).
- **Inline values** — while stopped, the live value of every SSA `$N`,
  function parameter, and block argument is displayed next to its
  definition in the editor (only for definitions at or above the current
  stop line, since later defs have not yet been computed). Nautilus emits
  each IR value `$N` into DWARF as a local named `vN`; the extension
  rewrites the lookup automatically. Toggle via `nautilusIr.inlineValues`.
- **Watch** and **Hover** evaluation via `data-evaluate-expression`.
- Stack trace and frame navigation across host C++ ↔ JIT'd IR frames.
- Console / stdout / stderr forwarded from gdb and the inferior.
- Optional one-shot break at the IR file's first instruction
  (`stopAtIrEntry`) so execution halts exactly when control reaches
  your IR.

#### Quick start

1. Build a host program with debug info (`cmake -DCMAKE_BUILD_TYPE=Debug ..`).
2. Open a `.nautilus` file.
3. Click **Run and Debug** in the side bar, then **Nautilus IR via host
   program (gdb)**. Edit `program` in `launch.json` to point at the host
   binary that loads this IR (e.g. `${workspaceFolder}/build/nautilus/test/execution-tests/ExecutionTest`).
4. Use the gutter to set breakpoints in the IR file. The first time you
   hit Run, breakpoints will show as pending until the JIT compiles the
   IR; after that they bind and execution stops normally.

Alternatively, right-click an IR file and pick **Nautilus IR: Debug
Current File with gdb** — it prompts you to select the host executable
and remembers the choice in workspace settings.

Example `launch.json`:

```jsonc
{
    "type": "nautilus-ir-gdb",
    "request": "launch",
    "name": "Debug Nautilus IR via host program (gdb)",
    "program": "${workspaceFolder}/build/nautilus/test/execution-tests/ExecutionTest",
    "args": ["--gtest_filter=ExecutionTest.MyTest"],
    "irFile": "${file}",
    "stopAtIrEntry": true,
    "sourceDirectories": [
        "${workspaceFolder}/nautilus/test/data"
    ],
    "cwd": "${workspaceFolder}"
}
```

#### Launch configuration attributes

| Field                | Purpose                                                                                                                                                       |
|----------------------|---------------------------------------------------------------------------------------------------------------------------------------------------------------|
| `program`            | C++ host executable to launch under gdb. **Required.**                                                                                                        |
| `args`               | Command-line arguments forwarded to the host process.                                                                                                         |
| `irFile`             | Optional Nautilus IR file of interest. Adds its directory to gdb's source search path and, if `stopAtIrEntry` is true, sets a one-shot entry breakpoint.      |
| `stopAtIrEntry`      | When true (default), break on the first instruction of the entry block of `irFile`.                                                                           |
| `stopOnEntry`        | When true, break at host `main()` before running anything else.                                                                                               |
| `sourceDirectories`  | Extra directories appended to gdb's source search path (`directory` command).                                                                                 |
| `cwd`                | Working directory for the gdb process.                                                                                                                        |
| `env`                | Extra environment variables for the inferior.                                                                                                                 |
| `gdbPath`/`gdbArgs`  | Override the gdb executable / pass extra args to gdb itself.                                                                                                  |
| `trace`              | Log MI/DAP traffic to the debug console for diagnostics.                                                                                                      |

For attach mode, additionally provide `pid`. The `irFile` field is still
honored so gdb can locate the IR source.

#### Workspace settings

| Setting                              | Default | Purpose                                                                          |
|--------------------------------------|---------|----------------------------------------------------------------------------------|
| `nautilusIr.gdbPath`                 | `gdb`   | Default gdb executable.                                                          |
| `nautilusIr.gdbExtraArgs`            | `[]`    | Extra args appended to every gdb invocation.                                     |
| `nautilusIr.highlightDefinitions`    | `true`  | Highlight all uses of an SSA value when the cursor is on its definition.         |
| `nautilusIr.codeLens`                | `true`  | Show CodeLens above every block.                                                 |
| `nautilusIr.graph.autoOpen`          | `false` | Open the graph view automatically when a `.nautilus` file is opened.             |
| `nautilusIr.inlineValues`            | `true`  | Show SSA values inline next to their definitions while debugging.                |
| `nautilusIr.lastHostProgram`         | `""`    | Remembered host program for the ad-hoc "Debug Current File" command.             |

### Commands

| Command                                          | Default keybinding         |
|--------------------------------------------------|----------------------------|
| `Nautilus IR: Go to Block...`                    | `Ctrl/Cmd+Shift+B`         |
| `Nautilus IR: Show Graph`                        | —                          |
| `Nautilus IR: Show File Statistics`              | —                          |
| `Nautilus IR: Toggle SSA Definition Highlight`   | —                          |
| `Nautilus IR: Debug Current File with gdb`       | —                          |

## Building from source

```bash
cd tools/vscode-nautilus-ir
npm install
npm run compile           # tsc → ./out, plus copies Mermaid + svg-pan-zoom
                          # bundles into media/vendor/ for the graph webview
# Open this folder in VS Code and press F5 to launch an Extension Host.
```

To package a `.vsix`:

```bash
npm install -g @vscode/vsce
vsce package
```

## File associations

The extension registers itself for the `.nautilus` extension. If you also
want it to handle older IR dumps that still carry a different extension
(e.g. legacy `.trace` files from before the Nautilus rename), add an
explicit association in your settings:

```jsonc
"files.associations": {
    "**/test/data/**/ir/*.trace": "nautilus-ir"
}
```

## Example

Open `nautilus/test/data/loop-tests/ir/collatz.nautilus` and you should see
syntax-highlighted blocks, a populated outline, and CodeLens annotations
above each `Block_N(...)` showing predecessor and successor counts.

## License

MIT — same as the Nautilus project itself.

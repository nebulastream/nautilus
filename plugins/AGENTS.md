# Plugins: notes for agents

Each plugin is a separate library with `include/`, `src/` and `test/`, enabled
by an `ENABLE_<NAME>_PLUGIN` option in the top-level `CMakeLists.txt`.

| Plugin | What it adds |
|---|---|
| `std` | `val`-aware wrappers of standard-library functions (`docs/std-plugin.md`) |
| `simd` | `val<vec<T>>` vector types and operations (`docs/simd.md`) |
| `builtin` | prefetch, overflow-checked arithmetic, 128-bit integers (`docs/builtin.md`) |
| `specialization` | profiling and `assume` hints |
| `inlining` | LLVM-level inlining for the MLIR backend (needs MLIR) |
| `gpu` | CUDA and Metal backends (`ENABLE_GPU_PLUGIN`, off by default) |
| `profiling` | in-process perf counters and sampling (Linux) |

- Plugins register intrinsics and backend lowerings at static-initialization
  time (see `builtin/src/BuiltinPluginInit.cpp`). A lowering must behave
  exactly like the native fallback function.
- Each plugin's tests run as their own ctest directory:
  `ctest --test-dir build/<preset>/plugins/<name>/test --output-on-failure`.
- A plugin change must pass its tests on every backend it has a lowering for.

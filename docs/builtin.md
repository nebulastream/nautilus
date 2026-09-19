# Builtins

## Overview

The builtin plugin (`nautilus-builtin`) exposes compiler builtins that have no natural `val<T>` operator. Each builtin is a plain function that works on every backend: the MLIR and AsmJit backends lower it to a native instruction, all other backends (C++, bytecode, interpreter) call a portable fallback.

Enable it with `-DENABLE_BUILTIN_PLUGIN=ON` (default), link `nautilus-builtin`, and include `<nautilus/builtin/plugin.hpp>` in one translation unit of your executable so the native lowerings are registered when `nautilus-builtin` is linked as a static library.

## Prefetch

```cpp
#include <nautilus/builtin/plugin.hpp>
#include <nautilus/builtin/prefetch/prefetch.hpp>

val<int64_t> sum(val<int64_t*> data, val<int64_t> count) {
    val<int64_t> sum = 0;
    for (val<int64_t> i = 0; i < count; i = i + 1) {
        prefetch(data + (i + 16), PrefetchRW::Read, PrefetchLocality::High);
        sum = sum + data[i];
    }
    return sum;
}
```

`prefetch(address, rw, locality)` asks the CPU to pull the cache line containing `address` closer to the core. It is only a hint: it never changes program results and never faults, so it is safe to prefetch past the end of an array or at an unmapped address.

| Argument | Values | Meaning |
|----------|--------|---------|
| `rw` | `Read` (default), `Write` | Whether the line is read or written next |
| `locality` | `None`, `Low`, `Moderate`, `High` (default) | How soon the line is reused; mirrors `__builtin_prefetch` levels 0-3 |

### Lowering

| Backend | Lowering |
|---------|----------|
| MLIR | `llvm.prefetch` |
| AsmJit x86-64 | `PREFETCHT0/T1/T2/NTA` for reads; `PREFETCHW` for writes (read hint of the same locality on CPUs without `PREFETCHW`) |
| AsmJit AArch64 | `PRFM PLDL1KEEP/PLDL2KEEP/PLDL3KEEP/PLDL1STRM` for reads, `PST*` for writes |
| C++, bytecode, interpreter | call to a `__builtin_prefetch` wrapper |

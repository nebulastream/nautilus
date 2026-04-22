#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// ---- Default C-ABI (reads the cycle counter internally) ---------------------
//
// These are the entry points Nautilus-traced code reaches through invoke().
// They take the region/counter name and capture a timestamp themselves via
// the session timebase (invariant TSC where available, steady_clock fallback).
// Region names must have static storage duration — the pointer is kept as-is
// in the event record.

void __nautilus_profile_begin(const char* name);
void __nautilus_profile_end(const char* name);
void __nautilus_profile_counter_i64(const char* name, int64_t value);

// ---- Pre-timed variants (caller supplies the timestamp) ---------------------
//
// The MLIR intrinsic lowering emits `llvm.readcyclecounter()` inline in the
// JIT'd code and then calls these, saving one function call + one vDSO entry
// per event. `ticks` is a raw cycle-counter value on the same timebase the
// recorder uses. Don't mix these with the default variants above in the same
// session — the raw values must come from the same clock source.

void __nautilus_profile_begin_ticks(const char* name, uint64_t ticks);
void __nautilus_profile_end_ticks(const char* name, uint64_t ticks);
void __nautilus_profile_counter_i64_ticks(const char* name, int64_t value, uint64_t ticks);

#ifdef __cplusplus
} // extern "C"
#endif

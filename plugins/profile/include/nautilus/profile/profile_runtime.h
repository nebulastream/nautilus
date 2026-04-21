#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// C-ABI runtime entry points that JIT-compiled Nautilus code calls into.
// Region names are expected to be string literals with static storage;
// passing non-static strings is undefined behavior because the Perfetto
// backend (step 3) will rely on pointer stability.

void __nautilus_profile_begin(const char* name);
void __nautilus_profile_end(const char* name);
void __nautilus_profile_counter_i64(const char* name, int64_t value);

#ifdef __cplusplus
} // extern "C"
#endif

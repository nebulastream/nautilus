#pragma once

#include "nautilus/common/FunctionAttributes.hpp"
#include "nautilus/function.hpp"
#include "nautilus/profile/profile_runtime.h"
#include "nautilus/val.hpp"
#include "nautilus/val_ptr.hpp"
#include <cstdint>

namespace nautilus::profile {

namespace detail {

// modRefInfo=ModRef  keeps the call from being CSE'd or DCE'd.
// willReturn=true    the runtime never aborts the traced function.
// noUnwind=true      the runtime never throws; lets LLVM skip unwind tables.
inline constexpr FunctionAttributes profilerCallAttrs {
    /*modRefInfo=*/ModRefInfo::ModRef,
    /*willReturn=*/true,
    /*noUnwind=*/true,
};

} // namespace detail

inline void startRegion(val<const char*> name) {
	invoke<void, const char*>(detail::profilerCallAttrs, &__nautilus_profile_begin, name);
}

inline void endRegion(val<const char*> name) {
	invoke<void, const char*>(detail::profilerCallAttrs, &__nautilus_profile_end, name);
}

inline void traceCounter(val<const char*> name, val<int64_t> value) {
	invoke<void, const char*, int64_t>(detail::profilerCallAttrs, &__nautilus_profile_counter_i64, name, value);
}

// RAII helper. Prefer this over paired startRegion / endRegion calls so the
// end is emitted even on early return. The name is kept in the traced call,
// not on the host stack; the val<> copy is cheap.
class ScopedRegion {
public:
	explicit ScopedRegion(val<const char*> name) : name_(name) {
		startRegion(name_);
	}

	~ScopedRegion() {
		endRegion(name_);
	}

	ScopedRegion(const ScopedRegion&) = delete;
	ScopedRegion& operator=(const ScopedRegion&) = delete;
	ScopedRegion(ScopedRegion&&) = delete;
	ScopedRegion& operator=(ScopedRegion&&) = delete;

private:
	val<const char*> name_;
};

} // namespace nautilus::profile

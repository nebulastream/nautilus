// The intrinsics the core owns (13-nir-spec.md §4.6.1, 04 §3.5). They are backend-neutral: a backend attaches its
// lowering by name, and every other path calls the fallback. They are registered by the registry's constructor because
// a static initializer in a static library is dropped by the linker when nothing else references its object file.
#include "nautilus/extensions/IntrinsicRegistration.hpp"
#include <cstring>

namespace nautilus::ext {

ExtensionRegistry::ExtensionRegistry() {
	constexpr uint32_t ARG_MEMORY_ONLY = AttrNoOtherRead | AttrNoOtherWrite | AttrWillReturn | AttrNoUnwind;
	auto add = [this](const char* name, auto fn, uint32_t attrs) {
		registerIntrinsic(detail::makeIntrinsic<true>(name, fn, attrs), Owner::Core);
	};
	add("core.memcpy", static_cast<void* (*) (void*, const void*, size_t)>(&std::memcpy), ARG_MEMORY_ONLY);
	add("core.memmove", static_cast<void* (*) (void*, const void*, size_t)>(&std::memmove), ARG_MEMORY_ONLY);
	add("core.memset", static_cast<void* (*) (void*, int, size_t)>(&std::memset), ARG_MEMORY_ONLY);
	add("core.memcmp", static_cast<int (*)(const void*, const void*, size_t)>(&std::memcmp),
	    ARG_MEMORY_ONLY | AttrNoArgWrite);
}

} // namespace nautilus::ext

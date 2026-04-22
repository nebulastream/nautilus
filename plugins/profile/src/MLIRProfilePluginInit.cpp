// Only compiled when ENABLE_MLIR_BACKEND is ON (see plugins/profile/CMakeLists.txt).
// Registers a jitCompiledFunctionObserver hook that forwards every JIT lookup
// into the profile plugin's JIT symbol table, so sample-based profiling can
// symbolicate frames that land inside JIT'd code.
//
// Size resolution strategy, in preference order:
//   1) When two or more functions from the same compilation have been
//      registered, compute each function's size as the delta to the next
//      function's start. This gives the tightest bounds.
//   2) For the last (or only) function, query the OS virtual-memory map for
//      the executable region that contains the address. On Linux that's
//      /proc/self/maps; on macOS it's mach_vm_region. The region's remaining
//      bytes after the function's start are an upper bound on its code size.
//   3) If the OS query fails (unusual), fall back to a conservative default.

#include "nautilus/compiler/backends/mlir/LLVMBackendHooks.hpp"
#include "nautilus/profile/JitSymbols.hpp"
#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

#if defined(__linux__)
#include <cstdlib>
#include <fstream>
#elif defined(__APPLE__)
#include <mach/mach.h>
#include <mach/vm_map.h>
#endif

namespace {

// Fallback if no OS query is available.
constexpr size_t kFallbackJitFunctionSize = 4 * 1024 * 1024;

// Result of a /proc/self/maps (or equivalent) lookup.
struct ExecRegion {
	uintptr_t start = 0;
	uintptr_t end = 0;
	bool valid = false;
};

#if defined(__linux__)
// Walks /proc/self/maps and returns the first executable region that contains
// `ip`. Parsing is deliberately minimal — just enough to read "lo-hi perms".
static ExecRegion queryExecRegion(uintptr_t ip) {
	ExecRegion out;
	std::ifstream maps("/proc/self/maps");
	if (!maps) {
		return out;
	}
	std::string line;
	while (std::getline(maps, line)) {
		// Format: lo-hi perms offset dev inode  pathname
		// We only need lo, hi, and the first perms char triple.
		char* endp = nullptr;
		unsigned long long lo = std::strtoull(line.c_str(), &endp, 16);
		if (endp == nullptr || *endp != '-') {
			continue;
		}
		unsigned long long hi = std::strtoull(endp + 1, &endp, 16);
		if (endp == nullptr) {
			continue;
		}
		// Advance to perms field.
		while (*endp == ' ' || *endp == '\t') {
			++endp;
		}
		if (std::strlen(endp) < 4) {
			continue;
		}
		const bool executable = endp[2] == 'x';
		if (!executable) {
			continue;
		}
		if (ip >= lo && ip < hi) {
			out.start = static_cast<uintptr_t>(lo);
			out.end = static_cast<uintptr_t>(hi);
			out.valid = true;
			return out;
		}
	}
	return out;
}
#elif defined(__APPLE__)
static ExecRegion queryExecRegion(uintptr_t ip) {
	ExecRegion out;
	mach_vm_address_t addr = ip;
	mach_vm_size_t size = 0;
	vm_region_basic_info_data_64_t info {};
	mach_msg_type_number_t cnt = VM_REGION_BASIC_INFO_COUNT_64;
	mach_port_t object_name = MACH_PORT_NULL;
	kern_return_t kr = mach_vm_region(mach_task_self(), &addr, &size, VM_REGION_BASIC_INFO_64,
	                                  reinterpret_cast<vm_region_info_t>(&info), &cnt, &object_name);
	if (kr != KERN_SUCCESS) {
		return out;
	}
	if ((info.protection & VM_PROT_EXECUTE) == 0) {
		return out;
	}
	if (ip < static_cast<uintptr_t>(addr) || ip >= static_cast<uintptr_t>(addr + size)) {
		return out;
	}
	out.start = static_cast<uintptr_t>(addr);
	out.end = static_cast<uintptr_t>(addr + size);
	out.valid = true;
	return out;
}
#else
static ExecRegion queryExecRegion(uintptr_t) {
	return {};
}
#endif

// Per-process state so multiple lookups from the same compilation can see
// each other and recompute their bounds from address deltas.
struct State {
	std::mutex mu;
	// name -> addr for every function we've observed this process run.
	std::unordered_map<std::string, uintptr_t> byName;
	// addr -> name for fast delta recompute.
	std::vector<std::pair<uintptr_t, std::string>> sorted;
};

static State& state() {
	static State s;
	return s;
}

// Compute size for `addr` as "gap to next registered address that lives in the
// same OS-executable region", capped by the region end. Falls back to the
// region bound and finally to the fixed default.
static size_t inferSize(uintptr_t addr, const ExecRegion& region) {
	auto& st = state();
	// Find the smallest registered address strictly greater than `addr` that
	// lives in the same executable region. sorted is kept address-ordered.
	uintptr_t cap = region.valid ? region.end : addr + kFallbackJitFunctionSize;
	for (const auto& [other_addr, _name] : st.sorted) {
		if (other_addr <= addr) {
			continue;
		}
		if (region.valid && (other_addr < region.start || other_addr >= region.end)) {
			continue;
		}
		if (other_addr < cap) {
			cap = other_addr;
		}
		break; // sorted, so first hit is the tightest bound
	}
	return cap > addr ? static_cast<size_t>(cap - addr) : kFallbackJitFunctionSize;
}

// Re-register every known function so their sizes reflect any new neighbour
// that was just added.
static void recomputeAll() {
	auto& st = state();
	// Sort by addr before the sweep.
	std::sort(st.sorted.begin(), st.sorted.end(), [](const auto& a, const auto& b) { return a.first < b.first; });
	for (const auto& [addr, name] : st.sorted) {
		ExecRegion region = queryExecRegion(addr);
		size_t size = inferSize(addr, region);
		nautilus::profile::JitSymbol sym {};
		sym.addr = addr;
		sym.size = size;
		sym.name = name;
		sym.module = "mlir";
		nautilus::profile::registerJitCode(std::move(sym));
	}
}

struct ProfileMLIRHookRegistrar {
	ProfileMLIRHookRegistrar() {
		nautilus::compiler::mlir::getLLVMBackendHooks().jitCompiledFunctionObserver = [](const std::string& name,
		                                                                                 void* addr) {
			if (addr == nullptr) {
				return;
			}
			const auto ip = reinterpret_cast<uintptr_t>(addr);
			auto& st = state();
			std::lock_guard<std::mutex> lock(st.mu);
			auto inserted = st.byName.emplace(name, ip);
			if (!inserted.second && inserted.first->second == ip) {
				return; // identical (name, addr) pair already known
			}
			if (!inserted.second) {
				// Same name, different address — module was recompiled. Drop the
				// old entry and re-register.
				nautilus::profile::unregisterJitCode(inserted.first->second);
				for (auto it = st.sorted.begin(); it != st.sorted.end(); ++it) {
					if (it->second == name) {
						st.sorted.erase(it);
						break;
					}
				}
				inserted.first->second = ip;
			}
			st.sorted.emplace_back(ip, name);
			recomputeAll();
		};
	}
};

static ProfileMLIRHookRegistrar registrar_;

} // namespace

namespace nautilus::profile {
// Forces this TU to be linked when the profile plugin is a static library.
// Tests and users that want JIT-symbol registration on MLIR can call this
// once; it's a no-op after the static initializer above has run.
void ensureMLIRBackendHookInstalled() {
	(void) &registrar_;
}
} // namespace nautilus::profile

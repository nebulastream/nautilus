#include "nautilus/profile/JitSymbols.hpp"
#include "nautilus/config.hpp"
#include "nautilus/logging.hpp"
#include <algorithm>
#include <atomic>
#include <cstdio>
#include <fstream>
#include <memory>
#include <mutex>
#include <unistd.h>
#include <vector>

#ifndef ENABLE_MLIR_BACKEND
// When MLIR is disabled the MLIRProfilePluginInit TU is not compiled in, so
// we provide a no-op fallback here to keep the public API definition complete.
namespace nautilus::profile {
void ensureMLIRBackendHookInstalled() {
}
} // namespace nautilus::profile
#endif

namespace nautilus::profile {

namespace {

// The live snapshot is an immutable sorted-by-addr vector behind a shared_ptr;
// lookups acquire via atomic_load and binary-search without locking.
// Registrations serialize on `mu` and publish a fresh snapshot.
std::mutex& writerMutex() {
	static std::mutex m;
	return m;
}

using Snapshot = std::vector<JitSymbol>;

std::atomic<std::shared_ptr<const Snapshot>>& snapshot() {
	// Using atomic<shared_ptr<...>> requires C++20 — available per
	// CMakeLists.txt. Default-construct to an empty snapshot.
	static std::atomic<std::shared_ptr<const Snapshot>> s {std::make_shared<Snapshot>()};
	return s;
}

std::shared_ptr<Snapshot> cloneCurrent() {
	auto cur = snapshot().load(std::memory_order_acquire);
	return std::make_shared<Snapshot>(*cur);
}

void publish(std::shared_ptr<Snapshot> next) {
	std::sort(next->begin(), next->end(), [](const JitSymbol& a, const JitSymbol& b) { return a.addr < b.addr; });
	snapshot().store(std::shared_ptr<const Snapshot>(std::move(next)), std::memory_order_release);
}

} // namespace

void registerJitCode(JitSymbol sym) {
	std::lock_guard<std::mutex> lock(writerMutex());
	auto next = cloneCurrent();
	// Drop any existing entry with the same start address so re-registrations
	// (same JIT slot reused across compiles) don't accumulate duplicates.
	next->erase(std::remove_if(next->begin(), next->end(), [&](const JitSymbol& s) { return s.addr == sym.addr; }),
	            next->end());
	next->push_back(std::move(sym));
	publish(std::move(next));
}

void unregisterJitCode(uintptr_t addr) {
	std::lock_guard<std::mutex> lock(writerMutex());
	auto next = cloneCurrent();
	next->erase(std::remove_if(next->begin(), next->end(), [&](const JitSymbol& s) { return s.addr == addr; }),
	            next->end());
	publish(std::move(next));
}

std::optional<JitSymbol> lookupJitSymbol(uintptr_t ip) {
	auto s = snapshot().load(std::memory_order_acquire);
	// Binary search for the greatest addr <= ip, then check ip falls in range.
	auto it =
	    std::upper_bound(s->begin(), s->end(), ip, [](uintptr_t v, const JitSymbol& sym) { return v < sym.addr; });
	if (it == s->begin()) {
		return std::nullopt;
	}
	--it;
	if (ip >= it->addr && ip < it->addr + it->size) {
		return *it;
	}
	return std::nullopt;
}

void clearJitSymbols() {
	std::lock_guard<std::mutex> lock(writerMutex());
	publish(std::make_shared<Snapshot>());
}

std::string writePerfMap() {
	char path[64];
	std::snprintf(path, sizeof(path), "/tmp/perf-%d.map", static_cast<int>(::getpid()));
	std::ofstream out(path);
	if (!out) {
		nautilus::log::error("profile.writePerfMap: cannot open {}", path);
		return {};
	}
	auto s = snapshot().load(std::memory_order_acquire);
	for (const auto& sym : *s) {
		// `perf` expects hex addr, hex size, and a name without spaces. Replace
		// spaces in the name defensively; the module is appended as a suffix
		// so users can tell which Nautilus module produced each frame.
		std::string name = sym.name;
		for (auto& c : name) {
			if (c == ' ' || c == '\t') {
				c = '_';
			}
		}
		if (!sym.module.empty()) {
			name.append("@");
			name.append(sym.module);
		}
		out << std::hex << sym.addr << ' ' << sym.size << ' ' << std::dec << name << '\n';
	}
	if (!out) {
		nautilus::log::error("profile.writePerfMap: write failed for {}", path);
		return {};
	}
	return path;
}

} // namespace nautilus::profile

#include "FunctionNameCache.hpp"
#include <cstdlib>
#include <cxxabi.h>
#include <dlfcn.h>
#include <mutex>
#include <shared_mutex>
#include <sstream>
#include <unordered_map>

namespace nautilus::tracing {

namespace {

ResolvedFunctionName lookup(void* fnptr) {
	ResolvedFunctionName result;
	Dl_info info;
	if (dladdr(fnptr, &info) != 0 && info.dli_sname != nullptr) {
		result.mangled = info.dli_sname;
	} else {
		std::stringstream ss;
		ss << fnptr;
		result.mangled = ss.str();
	}

	int status;
	char* demangled = __cxxabiv1::__cxa_demangle(result.mangled.c_str(), nullptr, nullptr, &status);
	if (status == 0 && demangled != nullptr) {
		result.demangled = demangled;
	} else {
		result.demangled = result.mangled;
	}
	std::free(demangled);
	return result;
}

} // namespace

const ResolvedFunctionName& resolveFunctionName(void* fnptr) {
	// Leaked on purpose: a trace may be formatted from a static destructor, and
	// the cache must outlive every one of them.
	static auto* cache = new std::unordered_map<void*, ResolvedFunctionName>();
	static auto* mutex = new std::shared_mutex();

	{
		std::shared_lock lock(*mutex);
		if (const auto it = cache->find(fnptr); it != cache->end()) {
			return it->second;
		}
	}

	// Resolve outside the lock so concurrent tracers are not serialized behind
	// dladdr. Two threads may race to resolve the same pointer; both compute the
	// same names and the first insert wins.
	auto resolved = lookup(fnptr);
	std::unique_lock lock(*mutex);
	// unordered_map never moves its nodes, so the reference outlives later inserts.
	return cache->try_emplace(fnptr, std::move(resolved)).first->second;
}

} // namespace nautilus::tracing

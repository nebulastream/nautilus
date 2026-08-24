#include "TraceContextBase.hpp"
#include "ExecutionTrace.hpp"
#include "nautilus/logging.hpp"
#include "symbolic_execution/SymbolicExecutionContext.hpp"
#include <cstdlib>
#include <cstring>
#include <cxxabi.h>
#include <dlfcn.h>
#include <sstream>

namespace nautilus::tracing {

TraceState::TraceState(TagRecorder& tr, ExecutionTrace& et, SymbolicExecutionContext& sec, const engine::Options& opts)
    : tagRecorder(tr), executionTrace(et), symbolicExecutionContext(sec), options(opts) {
}

std::string TraceContextBase::getMangledName(void* fnptr) {
	if (const auto it = mangledNameCache.find(fnptr); it != mangledNameCache.end()) {
		return it->second;
	}

	Dl_info info;
	if (dladdr(fnptr, &info) != 0 && info.dli_sname != nullptr) {
		mangledNameCache[fnptr] = info.dli_sname;
		return info.dli_sname;
	}
	std::stringstream ss;
	ss << fnptr;
	std::string ptrStr = ss.str();
	mangledNameCache[fnptr] = ptrStr;
	return ptrStr;
}

std::string TraceContextBase::getFunctionName(void* fnptr, const std::string& mangledName) {
	bool normalizeFunctionNames = state->options.getOptionOrDefault("engine.normalizeFunctionNames", false);

	if (normalizeFunctionNames) {
		auto it = state->normalizedFunctionNameCache.find(fnptr);
		if (it != state->normalizedFunctionNameCache.end()) {
			return "runtimeFunc" + std::to_string(it->second);
		}
		uint32_t index = state->nextNormalizedFunctionIndex++;
		state->normalizedFunctionNameCache[fnptr] = index;
		return "runtimeFunc" + std::to_string(index);
	}

	bool demangleFunctionNames = state->options.getOptionOrDefault("engine.demangleFunctionNames", true);

	if (!demangleFunctionNames) {
		return mangledName;
	}

	int status;
	char* demangled = __cxxabiv1::__cxa_demangle(mangledName.c_str(), nullptr, nullptr, &status);
	if (status == 0 && demangled != nullptr) {
		std::string result(demangled);
		std::free(demangled);
		return result;
	}

	return mangledName;
}

constexpr size_t fnv_prime = 0x100000001b3;
constexpr size_t offset_basis = 0xcbf29ce484222325;

uint64_t hashStaticVector(const std::vector<StaticVarHolder>& data) {
	size_t hash = offset_basis;
	for (auto& entry : data) {
		uint64_t val = 0;
		std::memcpy(&val, entry.ptr, entry.size);
		hash ^= val;
		hash *= fnv_prime;
	}
	return hash;
}

uint64_t TraceContextBase::currentStateHash() const {
	return hashStaticVector(staticVars) ^ aliveVars.hash();
}

TraceEnv& TraceContextBase::getEnv() {
	return env;
}

ExecutionTrace& TraceContextBase::getExecutionTrace() {
	return state->executionTrace;
}

std::string TraceContextBase::formatStaticVars() const {
	std::string result;
	for (size_t i = 0; i < staticVars.size(); i++) {
		if (i > 0) {
			result += ", ";
		}
		result += std::to_string(getStaticVarValue(staticVars[i]));
	}
	return result;
}

void TraceContextBase::pushStaticVal(void* valPtr, size_t size) {
	staticVars.emplace_back(valPtr, size);
	if (log::options::getLogStaticVars()) {
		log::info("pushStaticVal: [{}]", formatStaticVars());
	}
}

void TraceContextBase::popStaticVal() {
	if (log::options::getLogStaticVars()) {
		log::info("popStaticVal: [{}] (popping last)", formatStaticVars());
	}
	staticVars.pop_back();
}

} // namespace nautilus::tracing

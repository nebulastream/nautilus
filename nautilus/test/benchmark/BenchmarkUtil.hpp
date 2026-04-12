#pragma once

#include "nautilus/config.hpp"
#include <string>
#include <vector>

namespace nautilus::benchmark {

/// Returns the names of all backends that were enabled at CMake configure time.
inline std::vector<std::string> getEnabledBackends() {
	std::vector<std::string> backends;
#ifdef ENABLE_MLIR_BACKEND
	backends.emplace_back("mlir");
#endif
#ifdef ENABLE_C_BACKEND
	backends.emplace_back("cpp");
#endif
#ifdef ENABLE_BC_BACKEND
	backends.emplace_back("bc");
#endif
#ifdef ENABLE_ASMJIT_BACKEND
	backends.emplace_back("asmjit");
#endif
	return backends;
}

} // namespace nautilus::benchmark

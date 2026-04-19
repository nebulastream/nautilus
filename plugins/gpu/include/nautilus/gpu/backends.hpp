#pragma once

#include "nautilus/gpu/config.hpp"
#include "nautilus/options.hpp"
#include <memory>
#include <string>

namespace nautilus::compiler::ir {
class IRGraph;
}

namespace nautilus::gpu {

#ifdef ENABLE_CUDA_BACKEND
/// Lower an IR graph to CUDA source code (.cu).
std::string lowerToCUDA(std::shared_ptr<compiler::ir::IRGraph> ir, const engine::Options& options);
#endif

#ifdef ENABLE_METAL_BACKEND
/// Result of Metal lowering: separate device (MSL) and host (C++) code.
struct MetalOutput {
	std::string deviceCode; // .metal kernel source
	std::string hostCode;   // .cpp host source with Metal API dispatch
};

/// Lower an IR graph to Metal output (device MSL + host C++).
MetalOutput lowerToMetal(std::shared_ptr<compiler::ir::IRGraph> ir, const engine::Options& options);
#endif

} // namespace nautilus::gpu

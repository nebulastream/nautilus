#pragma once

#include "nautilus/compiler/backends/cpp/SharedLibrary.hpp"
#include "nautilus/options.hpp"
#include <string>

namespace nautilus::compiler::cuda {

/// Compiles CUDA source code (.cu) to a shared library using nvcc.
///
/// All knobs are read from `engine::Options`:
///   - `gpu.cuda.nvccPath`     (string): explicit path to nvcc (overrides discovery)
///   - `gpu.cuda.arch`         (string): `-arch=` value (e.g. "sm_75"); empty = let nvcc decide
///   - `gpu.cuda.optLevel`     (int):    `-O` level for host & device (default 3)
///   - `gpu.cuda.fastMath`     (bool):   add `--use_fast_math` (default false)
///   - `gpu.cuda.debug`        (bool):   add `-g -G` device debug + line info (default false)
///   - `gpu.cuda.extraFlags`   (string): extra space-separated nvcc flags (advanced)
///   - `gpu.cuda.keepArtifacts`(bool):   on compile failure, keep the .cu next to the
///                                      shared library and include the path in the
///                                      thrown error (default true).
class CUDACompiler {
public:
	/// Compiles `.cu` source to a shared library. Returns a loaded SharedLibrary.
	static cpp::SharedLibraryPtr compile(const std::string& identifier, const std::string& code,
	                                     const engine::Options& options);

private:
	static std::string findNvcc(const engine::Options& options);
	static std::string buildNvccCommand(const std::string& nvcc, const std::string& sourcePath,
	                                    const std::string& outputPath, const engine::Options& options);
	static void runCommand(const std::string& cmd);
	/// Returns the supplied path quoted for safe inclusion in a /bin/sh command line.
	static std::string shellQuote(const std::string& s);
	/// Generates a unique temp-file stem (`identifier`-PID-random) to avoid clashes
	/// between concurrent compilations of the same IR graph.
	static std::string makeUniqueStem(const std::string& identifier);
};

} // namespace nautilus::compiler::cuda

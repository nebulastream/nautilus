
#include "CUDACompiler.hpp"
#include "nautilus/common/File.hpp"
#include "nautilus/exceptions/RuntimeException.hpp"
#include <atomic>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <random>
#include <sstream>
#include <string>
#include <unistd.h>

namespace nautilus::compiler::cuda {

std::string CUDACompiler::shellQuote(const std::string& s) {
	// Wrap in single quotes for /bin/sh and escape any embedded single quotes
	// using the standard 'foo'\''bar' trick. This is the safe form that works
	// even with paths containing spaces, $, ;, &, etc.
	std::string out;
	out.reserve(s.size() + 2);
	out.push_back('\'');
	for (char c : s) {
		if (c == '\'') {
			out += "'\\''";
		} else {
			out.push_back(c);
		}
	}
	out.push_back('\'');
	return out;
}

std::string CUDACompiler::makeUniqueStem(const std::string& identifier) {
	// Collision-resistant suffix: PID + monotonic counter + a single random
	// 32-bit value. Avoids the race where two threads (or processes) compile
	// the same IR identifier simultaneously and clobber each other's source.
	static std::atomic<uint64_t> counter {0};
	static thread_local std::mt19937_64 rng {
	    static_cast<uint64_t>(std::chrono::steady_clock::now().time_since_epoch().count()) ^
	    static_cast<uint64_t>(::getpid())};

	std::ostringstream stem;
	stem << identifier << "-" << ::getpid() << "-" << counter.fetch_add(1, std::memory_order_relaxed) << "-" << std::hex
	     << rng();
	return stem.str();
}

std::string CUDACompiler::findNvcc(const engine::Options& options) {
	// 1. Explicit override always wins.
	auto explicitPath = options.getOptionOrDefault<std::string>("gpu.cuda.nvccPath", "");
	if (!explicitPath.empty()) {
		if (!std::filesystem::exists(explicitPath)) {
			throw RuntimeException("CUDACompiler: gpu.cuda.nvccPath points to non-existent file: " + explicitPath);
		}
		return explicitPath;
	}

	// 2. Honour CUDA_HOME / CUDA_PATH before falling back to PATH lookup.
	for (const char* var : {"CUDA_HOME", "CUDA_PATH"}) {
		if (auto* home = std::getenv(var)) {
			auto candidate = std::filesystem::path(home) / "bin" / "nvcc";
			if (std::filesystem::exists(candidate)) {
				return candidate.string();
			}
		}
	}

	// 3. PATH lookup — pure C++ to avoid spawning a shell.
	if (auto* pathEnv = std::getenv("PATH")) {
		std::string path = pathEnv;
		size_t start = 0;
		while (start <= path.size()) {
			auto end = path.find(':', start);
			auto dir = path.substr(start, end == std::string::npos ? std::string::npos : end - start);
			if (!dir.empty()) {
				auto candidate = std::filesystem::path(dir) / "nvcc";
				if (std::filesystem::exists(candidate)) {
					return candidate.string();
				}
			}
			if (end == std::string::npos) {
				break;
			}
			start = end + 1;
		}
	}

	// 4. Common install locations.
	for (const auto* candidate : {"/usr/local/cuda/bin/nvcc", "/opt/cuda/bin/nvcc"}) {
		if (std::filesystem::exists(candidate)) {
			return candidate;
		}
	}

	throw RuntimeException("CUDACompiler: could not locate 'nvcc'. Install the CUDA toolkit, set CUDA_HOME, "
	                       "or set the 'gpu.cuda.nvccPath' option.");
}

std::string CUDACompiler::buildNvccCommand(const std::string& nvcc, const std::string& sourcePath,
                                           const std::string& outputPath, const engine::Options& options) {
	auto arch = options.getOptionOrDefault<std::string>("gpu.cuda.arch", "");
	auto optLevel = options.getOptionOrDefault<int>("gpu.cuda.optLevel", 3);
	auto fastMath = options.getOptionOrDefault<bool>("gpu.cuda.fastMath", false);
	auto debug = options.getOptionOrDefault<bool>("gpu.cuda.debug", false);
	auto extraFlags = options.getOptionOrDefault<std::string>("gpu.cuda.extraFlags", "");

	std::ostringstream cmd;
	cmd << shellQuote(nvcc);
	cmd << " -shared";
	cmd << " -Xcompiler -fPIC";
	cmd << " -std=c++20";
	cmd << " -O" << optLevel;
	if (!arch.empty()) {
		cmd << " -arch=" << shellQuote(arch);
	}
	if (fastMath) {
		cmd << " --use_fast_math";
	}
	if (debug) {
		// `-g` for host, `-G` for device, `-lineinfo` for sane stack traces.
		cmd << " -g -G -lineinfo";
	}
	if (!extraFlags.empty()) {
		// Trusted, advanced-user knob — passed verbatim, intentionally not quoted.
		cmd << " " << extraFlags;
	}
	cmd << " -o " << shellQuote(outputPath);
	cmd << " " << shellQuote(sourcePath);
	return cmd.str();
}

cpp::SharedLibraryPtr CUDACompiler::compile(const std::string& identifier, const std::string& code,
                                            const engine::Options& options) {
	auto tmpDir = std::filesystem::temp_directory_path();
	auto stem = makeUniqueStem(identifier);

	auto sourcePath = (tmpDir / (stem + ".cu")).string();
	auto libraryPath = (tmpDir / (stem +
#ifdef __linux__
	                              ".so"
#elif defined(__APPLE__)
	                              ".dylib"
#else
#error "Unknown platform"
#endif
	                              ))
	                       .string();

	common::File::createFile(sourcePath, code);

	auto nvcc = findNvcc(options);
	auto cmd = buildNvccCommand(nvcc, sourcePath, libraryPath, options);

	const bool keepArtifacts = options.getOptionOrDefault<bool>("gpu.cuda.keepArtifacts", true);

	try {
		runCommand(cmd);
	} catch (const std::exception& e) {
		// Compilation failed: surface the path to the offending source so the
		// user can reproduce the failure outside the JIT pipeline.
		if (!keepArtifacts) {
			std::error_code ec;
			std::filesystem::remove(sourcePath, ec);
			std::filesystem::remove(libraryPath, ec);
			throw;
		}
		throw RuntimeException(std::string(e.what()) + "\n[nautilus-cuda] failing source kept at: " + sourcePath);
	}

	auto sharedLibrary = cpp::SharedLibrary::load(libraryPath);

	// Source is no longer needed; the loaded library is mmaped, so it survives
	// the unlink. SharedLibrary's destructor cleans up the .so on shutdown.
	std::error_code ec;
	std::filesystem::remove(sourcePath, ec);

	return sharedLibrary;
}

void CUDACompiler::runCommand(const std::string& cmd) {
	std::string fullCmd = cmd + " 2>&1";
	FILE* fp = popen(fullCmd.c_str(), "r");
	if (fp == nullptr) {
		throw RuntimeException("CUDACompiler: failed to spawn shell for command: " + cmd);
	}

	std::ostringstream output;
	char buffer[8192];
	while (fgets(buffer, sizeof(buffer), fp) != nullptr) {
		output << buffer;
	}

	auto ret = pclose(fp);
	if (ret != 0) {
		throw RuntimeException("CUDACompiler: nvcc invocation failed (exit=" + std::to_string(ret) +
		                       "):\ncommand: " + cmd + "\noutput:\n" + output.str());
	}
}

} // namespace nautilus::compiler::cuda

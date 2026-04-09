#pragma once

#include "nautilus/Engine.hpp"
#include <catch2/catch_all.hpp>
#include <cstdint>
#include <filesystem>
#include <functional>
#include <string>

namespace nautilus::engine {

/**
 * @brief Helper function to compare generated LLVM IR against reference IR
 *
 * This function:
 * 1. Compiles a Nautilus function using the MLIR backend
 * 2. Retrieves the generated LLVM IR
 * 3. Normalizes both generated and reference IR (removes machine-specific attributes)
 * 4. Compares using llvm-diff
 *
 * @tparam Func Function type to test
 * @param functionName Name of the function (used for reference file lookup)
 * @param func The function to compile and test
 * @param enableIntrinsics Whether to enable MLIR intrinsics (default: true)
 * @param referenceIRDir Directory containing reference IR files
 * @param extraOptions Optional callback for setting additional engine options
 *                     (e.g. to toggle plugin-specific knobs like
 *                     `mlir.inline_invoke_calls`). Invoked after the default
 *                     options have been set so callers can override them.
 */
template <typename Func>
void testLLVMIR(const std::string& functionName, Func func, bool enableIntrinsics,
                const std::filesystem::path& referenceIRDir,
                const std::function<void(engine::Options&)>& extraOptions = {}) {
	std::string dumpPath = (std::filesystem::temp_directory_path() / ("nautilus-ir-test-" + functionName)).string();
	std::filesystem::remove_all(dumpPath);
	std::filesystem::create_directories(dumpPath);

	engine::Options options;
	options.setOption("engine.backend", "mlir");
	options.setOption("dump.after_llvm_generation", true);
	options.setOption("dump.all", true);
	options.setOption("dump.console", true);
	options.setOption("dump.path", dumpPath);
	options.setOption("engine.normalizeFunctionNames", true);
	options.setOption("mlir.enableIntrinsics", enableIntrinsics);

	if (extraOptions) {
		extraOptions(options);
	}

	auto engine = engine::NautilusEngine(options);
	auto function = engine.registerFunction(func);

	// Get the generated LLVM IR file from the executable
	auto generatedLLVMFile = function.getExecutable()->getGeneratedFile("after_llvm_generation");
	REQUIRE(!generatedLLVMFile.empty());

	// Get reference IR path
	std::string referenceIRPath = (referenceIRDir / (functionName + ".ll")).string();

	// If reference IR doesn't exist, store the generated IR
	if (!std::filesystem::exists(referenceIRPath)) {
		// Create reference-ir directory if it doesn't exist
		std::filesystem::create_directories(referenceIRDir);

		// Normalize and store the generated IR as reference
		std::string normCmd = "sed -E '"
		                      "s/ \"target-cpu\"=\"[^\"]+\"//g; "
		                      "s/ \"target-features\"=\"[^\"]+\"//g; "
		                      "s/ \"tune-cpu\"=\"[^\"]+\"//g; "
		                      "s/ captures\\([^)]*\\)//g; "
		                      "s/, errnomem: [^,)]+//g; "
		                      "s/ range\\([^)]+\\)//g; "
		                      "s/ initializes\\(\\([^()]*\\)\\)//g; "
		                      "/^attributes #[0-9]+ = \\{ \\}$/d"
		                      "' " +
		                      std::string(generatedLLVMFile) + " > " + referenceIRPath;
		std::system(normCmd.c_str());

		// Skip comparison since we just created the reference
		std::filesystem::remove_all(dumpPath);
		return;
	}

	// Normalize IR files to remove machine-specific attributes
	// This ensures tests pass across different CPU architectures and LLVM versions
	std::string tempGenFile = dumpPath + "/" + functionName + "_gen_normalized.ll";
	std::string tempRefFile = dumpPath + "/" + functionName + "_ref_normalized.ll";

	// Strip machine-specific attributes:
	// - target-cpu, target-features, tune-cpu (CPU-specific)
	// - captures(...) attribute (LLVM 19+)
	// - errnomem (LLVM 19+ memory attribute)
	// - range(...) (LLVM 19+ range attribute)
	// - Empty attribute groups left after stripping (invalid IR for llvm-diff)
	std::string normCmd = "sed -E '"
	                      "s/ \"target-cpu\"=\"[^\"]+\"//g; "
	                      "s/ \"target-features\"=\"[^\"]+\"//g; "
	                      "s/ \"tune-cpu\"=\"[^\"]+\"//g; "
	                      "s/ captures\\([^)]*\\)//g; "
	                      "s/, errnomem: [^,)]+//g; "
	                      "s/ range\\([^)]+\\)//g; "
	                      "s/ initializes\\(\\([^()]*\\)\\)//g; "
	                      "/^attributes #[0-9]+ = \\{ \\}$/d"
	                      "' ";

	std::system((normCmd + std::string(generatedLLVMFile) + " > " + tempGenFile).c_str());
	std::system((normCmd + referenceIRPath + " > " + tempRefFile).c_str());

	// Run llvm-diff to compare normalized IR files.
	// Try versioned llvm-diff in descending order, fall back to unversioned.
	std::string llvmDiff = "llvm-diff";
	for (const char* candidate : {"llvm-diff-21", "llvm-diff-20", "llvm-diff-19"}) {
		std::string check = std::string("which ") + candidate + " > /dev/null 2>&1";
		if (std::system(check.c_str()) == 0) {
			llvmDiff = candidate;
			break;
		}
	}
	std::string command = llvmDiff + " " + tempGenFile + " " + tempRefFile;
	int result = std::system(command.c_str());

	std::filesystem::remove_all(dumpPath);

	// llvm-diff returns 0 if files are equivalent, 1 if different
	REQUIRE(result == 0);
}

} // namespace nautilus::engine

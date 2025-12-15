#pragma once

#include "nautilus/Engine.hpp"
#include <array>
#include <catch2/catch_all.hpp>
#include <cstdint>
#include <filesystem>
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
 */
template <typename Func>
void testLLVMIR(const std::string& functionName, Func func, bool enableIntrinsics,
                const std::filesystem::path& referenceIRDir) {
	std::string dumpPath = "/tmp/nautilus-ir-test";
	std::filesystem::remove_all(dumpPath);
	std::filesystem::create_directories(dumpPath);

	engine::Options options;
	options.setOption("engine.backend", "mlir");
	options.setOption("dump.after_llvm_generation", true);
	options.setOption("dump.path", dumpPath);
	options.setOption("engine.normalizeFunctionNames", true);
	options.setOption("mlir.enableIntrinsics", enableIntrinsics);

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
		std::string normCmd = "sed -E 's/ \"target-cpu\"=\"[^\"]+\"//g; "
		                      "s/ \"target-features\"=\"[^\"]+\"//g; "
		                      "s/ \"tune-cpu\"=\"[^\"]+\"//g; "
		                      "s/ captures\\([^)]*\\)//g; "
		                      "s/, errnomem: [^,)]+//g; "
		                      "s/ range\\([^)]+\\)//g' " +
		                      std::string(generatedLLVMFile) + " > " + referenceIRPath;
		std::system(normCmd.c_str());

		// Skip comparison since we just created the reference
		return;
	}

	// Normalize IR files to remove machine-specific attributes
	// This ensures tests pass across different CPU architectures and LLVM versions
	std::string tempGenFile = "/tmp/" + functionName + "_gen_normalized.ll";
	std::string tempRefFile = "/tmp/" + functionName + "_ref_normalized.ll";

	// Strip machine-specific attributes:
	// - target-cpu and target-features (CPU-specific)
	// - tune-cpu (optimization hints)
	// - captures(...) attribute (LLVM 19+ feature not in llvm-diff 18)
	// - errnomem (LLVM 19+ memory attribute, with leading comma)
	// - range(...) (LLVM 19+ range attribute)
	// - External function names (replace hex addresses with constant name)
	std::string normCmd = "sed -E 's/ \"target-cpu\"=\"[^\"]+\"//g; "
	                      "s/ \"target-features\"=\"[^\"]+\"//g; "
	                      "s/ \"tune-cpu\"=\"[^\"]+\"//g; "
	                      "s/ captures\\([^)]*\\)//g; "
	                      "s/, errnomem: [^,)]+//g; "
	                      "s/ range\\([^)]+\\)//g' " +
	                      std::string(generatedLLVMFile) + " > " + tempGenFile;
	std::system(normCmd.c_str());

	normCmd = "sed -E 's/ \"target-cpu\"=\"[^\"]+\"//g; "
	          "s/ \"target-features\"=\"[^\"]+\"//g; "
	          "s/ \"tune-cpu\"=\"[^\"]+\"//g; "
	          "s/ captures\\([^)]*\\)//g; "
	          "s/, errnomem: [^,)]+//g; "
	          "s/ range\\([^)]+\\)//g' " +
	          referenceIRPath + " > " + tempRefFile;
	std::system(normCmd.c_str());

        // Run llvm-diff to compare normalized IR files
        // Prefer a versioned binary if available, falling back to the generic name.
        const std::array<std::string, 3> llvmDiffCandidates = {"llvm-diff-20", "llvm-diff-19", "llvm-diff"};
        const auto llvmDiff = [&llvmDiffCandidates]() {
                for (const auto& candidate : llvmDiffCandidates) {
                        if (std::system(("which " + candidate + " > /dev/null 2>&1").c_str()) == 0) {
                                return candidate;
                        }
                }
                return std::string("llvm-diff");
        }();
	std::string command = llvmDiff + " " + tempGenFile + " " + tempRefFile;
	int result = std::system(command.c_str());

	// llvm-diff returns 0 if files are equivalent, 1 if different
	REQUIRE(result == 0);
}

} // namespace nautilus::engine

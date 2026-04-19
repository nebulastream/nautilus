#pragma once

#include "nautilus/config.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <unistd.h>

namespace nautilus::testing {

/**
 * @brief Reference-dump helper lifted out of the tracing tests so both the
 * tracing test binary and the pass-test binary share one primitive.
 *
 * Compares @p actual against the checked-in reference file at
 * `<TEST_DATA_FOLDER>/<category>/<group>/<name>.trace`. If the file does
 * not exist it is created from @p actual and the function returns false,
 * so CI fails on the first run and the developer can commit the generated
 * reference. This matches the existing tracing-test workflow exactly.
 *
 * @p usePartialEvaluationOverride selects a per-mode override directory
 * at `<TEST_DATA_FOLDER>/constant-tracer/<category>/<group>/<name>.trace`.
 * TracingTest passes `true` only when iterating the "partialEvaluation"
 * trace mode — the PE-adjusted traces live under the override dir,
 * everything else uses the baseline. Writing a new file (on first run)
 * goes to the override dir iff @p usePartialEvaluationOverride is true,
 * so the main reference set is never touched by a PE-mode run.
 */
inline bool checkReferenceDump(const std::string& actual, const std::string& category, const std::string& group,
                               const std::string& name, bool usePartialEvaluationOverride = false) {
	const auto mainDir = std::string(TEST_DATA_FOLDER) + category + "/" + group + "/";
	const std::string mainFile = mainDir + name + ".trace";
	const auto overrideDir = std::string(TEST_DATA_FOLDER) + "constant-tracer/" + category + "/" + group + "/";
	const std::string overrideFile = overrideDir + name + ".trace";

	// When the caller asked for the PE override *and* one is committed,
	// prefer it. If no override is committed yet, fall back to the
	// baseline — the fallback case is handled below (a diverging trace
	// writes a new override file, same as the original workflow).
	const std::string filePath =
	    (usePartialEvaluationOverride && std::filesystem::exists(overrideFile)) ? overrideFile : mainFile;

	if (!std::filesystem::exists(filePath)) {
		std::cerr << "File does not exist: " << filePath << " Initializing with current dump. Please Rerun.\n";
		if (!std::filesystem::exists(std::filesystem::path(filePath).parent_path())) {
			std::filesystem::create_directories(std::filesystem::path(filePath).parent_path());
		}
		std::ofstream file {filePath};
		file << actual;
		return false;
	}

	std::ifstream file(filePath);
	if (!file.is_open()) {
		std::cerr << "Unable to open file " << filePath << std::endl;
		return false;
	}

	std::stringstream expect;
	expect << file.rdbuf();

	if (expect.str() == actual) {
		return true;
	}

	if (usePartialEvaluationOverride && filePath == mainFile && !std::filesystem::exists(overrideFile)) {
		// Baseline didn't match and the caller explicitly wants a PE
		// override. Write one so the next run picks it up. Same
		// first-run-ergonomics contract the original helper offered.
		std::cerr << "Partial-evaluation trace diverges from baseline at " << mainFile
		          << ". Writing override: " << overrideFile << ". Please Rerun.\n";
		std::filesystem::create_directories(overrideDir);
		std::ofstream ofs {overrideFile};
		ofs << actual;
		return false;
	}

	char tmpName[] = "/tmp/actual_trace_XXXXXX";
	int tmpFd = mkstemp(tmpName);
	close(tmpFd);
	std::ofstream tmpfile {tmpName};
	tmpfile << actual;
	std::cerr << "Trace mismatch: (exp vs act) " << filePath << " " << tmpName << std::endl;
	std::cerr << "=== Expected (" << filePath << ") ===\n"
	          << expect.str() << "\n=== Actual ===\n"
	          << actual << "\n=== End ===\n";
	return false;
}

} // namespace nautilus::testing

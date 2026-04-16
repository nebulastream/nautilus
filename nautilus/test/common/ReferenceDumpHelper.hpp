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
 */
inline bool checkReferenceDump(const std::string& actual, const std::string& category, const std::string& group,
                               const std::string& name) {
	auto groupDir = std::string(TEST_DATA_FOLDER) + category + "/" + group + "/";
	if (!std::filesystem::exists(groupDir)) {
		std::filesystem::create_directories(groupDir);
	}

	std::string filePath = groupDir + name + ".trace";
	if (!std::filesystem::exists(filePath)) {
		std::cerr << "File does not exist: " << filePath << " Initializing with current dump. Please Rerun.\n";
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

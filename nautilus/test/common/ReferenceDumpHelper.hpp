#pragma once

#include "nautilus/config.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <regex>
#include <sstream>
#include <string>
#include <unistd.h>

namespace nautilus::testing {

/**
 * @brief Rewrites the source locations a dump may carry into a portable form:
 * `at /abs/path/File.hpp:41:9` becomes `at File.hpp:41`.
 *
 * Only region attributes (docs/region.md) put a source location in a dump today. Both
 * halves of the rewrite are needed for a checked-in dump to be comparable at all:
 *
 *  - the path is wherever the build happened to compile from, which differs between a
 *    developer checkout and a CI runner;
 *  - the column is the compiler's own choice. For one and the same `region(...)` call
 *    GCC reports the callee's closing position and Clang the start of the expression
 *    (column 8 vs 2 on the same line), so a dump with columns could only ever match the
 *    compiler that produced it.
 *
 * The file name and line survive, so a dump still pins which call site each region came
 * from.
 */
inline std::string normalizeSourceLocations(const std::string& dump) {
	// " at <path>:<line>:<column>" -- the path is everything up to the line number that is
	// neither a colon nor a newline, so a leading directory is optional.
	static const std::regex location(R"( at ([^\s:]*[/\\])?([^\s:/\\]+):(\d+):\d+)");
	return std::regex_replace(dump, location, " at $2:$3");
}

/**
 * @brief Reference-dump helper lifted out of the tracing tests so both the
 * tracing test binary and the pass-test binary share one primitive.
 *
 * Compares @p actual against the checked-in reference file at
 * `<TEST_DATA_FOLDER>/<category>/<group>/<name><extension>`. If the file does
 * not exist it is created from @p actual and the function returns false,
 * so CI fails on the first run and the developer can commit the generated
 * reference. This matches the existing tracing-test workflow exactly.
 *
 * The default extension is `.trace` (raw execution trace dumps); Nautilus IR
 * dumps should pass `.nautilus` so the on-disk artefacts carry the right name.
 *
 * When the build defines `ENABLE_SHORT_CIRCUIT_BOOL`, the IR shape produced by
 * `&&` / `||` is no longer a flat AND/OR op but a short-circuit branch tree,
 * so a subset of the default fixtures stop matching. To keep both
 * configurations in tree, the helper first looks for an optional sibling file
 * named `<name>.shortcircuit<extension>`. If that override exists it takes
 * priority; otherwise the default fixture is consulted. When the default
 * fixture exists but does not match, the helper materialises the override
 * with the actual dump so the next run picks it up — same auto-init flow as
 * the default-fixture case.
 */
inline bool checkReferenceDump(const std::string& rawActual, const std::string& category, const std::string& group,
                               const std::string& name, const std::string& extension = ".trace") {
	// Compare (and store) the portable form; see normalizeSourceLocations. A dump without
	// a source location in it passes through untouched.
	const std::string actual = normalizeSourceLocations(rawActual);
	auto groupDir = std::string(TEST_DATA_FOLDER) + category + "/" + group + "/";
	if (!std::filesystem::exists(groupDir)) {
		std::filesystem::create_directories(groupDir);
	}

	std::string defaultPath = groupDir + name + extension;
#ifdef ENABLE_SHORT_CIRCUIT_BOOL
	std::string overridePath = groupDir + name + ".shortcircuit" + extension;
	std::string filePath = std::filesystem::exists(overridePath) ? overridePath : defaultPath;
#else
	std::string filePath = defaultPath;
#endif

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

#ifdef ENABLE_SHORT_CIRCUIT_BOOL
	// Default fixture exists and did not match. Materialise a short-circuit
	// override so the developer can rerun and commit it.
	if (filePath == defaultPath) {
		std::cerr << "Trace mismatch under ENABLE_SHORT_CIRCUIT_BOOL; writing override " << overridePath
		          << ". Please rerun and commit the new fixture.\n";
		std::ofstream out {overridePath};
		out << actual;
		return false;
	}
#endif

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

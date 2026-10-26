#pragma once

#include "nautilus/nir/Diagnostics.hpp"
#include "nautilus/nir/Image.hpp"
#include "nautilus/nir/Verifier.hpp"
#include <optional>
#include <span>
#include <string>

namespace nautilus::nir {

/// Limits checked before anything is allocated (13-nir-spec.md §8.1).
struct LoadLimits {
	uint64_t totalSize = 1ull << 30;
	uint64_t functions = 1ull << 20;
	uint64_t instsPerFunction = 1ull << 28;
	uint64_t blocksPerFunction = 1ull << 24;
	uint64_t tableEntries = 1ull << 28;
	uint64_t extraWords = 1ull << 28;
	uint64_t extensionEntries = 1ull << 16;
	uint64_t dataBytes = 256ull << 20;
};

struct LoadOptions {
	LoadLimits limits;
	/// The token of this process; an image that is process-local must carry it (NIR-L014).
	uint64_t processToken = 0; // 0: currentProcessToken()
	/// Skip the process token check (inspection tools).
	bool ignoreProcessToken = false;
	/// Accept pre-SSA functions (the debugging API; `loadModule` rejects them with NIR-L016).
	bool allowPreSSA = false;
	/// Run the verifier after loading (NIR-L017 carries its first code).
	bool verify = true;
	VerifyOptions verifyOptions;
};

struct LoadError {
	DiagCode code;
	std::string message;
	/// For NIR-L017: what the verifier reported.
	std::vector<Diagnostic> verifierDiagnostics;

	std::string str() const;
};

struct LoadResult {
	std::optional<Module> module;
	std::optional<LoadError> error;
	/// The header, valid once the magic and size checks passed.
	std::optional<ImageHeader> header;

	bool ok() const {
		return module && !error;
	}
};

/// Validates in the order of §8.1 and rejects at the first failure with its NIR-L code: magic, version and feature
/// bits, totalSize, checksum, limits, section table, token, then the verifier. A malformed image never causes
/// undefined behavior. The module is copied out of the buffer.
LoadResult loadImage(std::span<const uint8_t> image, const LoadOptions& options = {});

} // namespace nautilus::nir

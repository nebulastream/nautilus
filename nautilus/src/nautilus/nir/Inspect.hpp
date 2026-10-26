#pragma once

#include <cstdint>
#include <span>
#include <string>

namespace nautilus::nir {

struct InspectOptions {
	/// Also dump every section as annotated hex: one line per record, the byte offset of the record first.
	bool hexDump = false;
};

/// Describes an image without materializing a module: the header, the feature bits, the checksum status and the section
/// table (13-nir-spec.md §8.1). Malformed input is described as far as it can be read, never trusted.
std::string inspectImage(std::span<const uint8_t> image, const InspectOptions& options = {});

} // namespace nautilus::nir

#pragma once

#include "nautilus/serialization/IRSerialization.hpp"
#include <cstddef>
#include <cstdint>
#include <span>
#include <vector>

/// Lets a layer above the IR (e.g. a module manifest) add its own sections to
/// a serialized graph. Internal to the plugin.
namespace nautilus::serialization::detail {

/// One section written verbatim after the IR's own. Never flagged required:
/// a plain IR reader skips it.
struct ExtraSection {
	uint32_t tag;
	uint32_t recordStride;
	uint32_t recordCount;
	std::vector<std::byte> bytes; ///< recordStride * recordCount bytes
};

std::vector<std::byte> serializeWithSections(const IRGraph& graph, const SerializeOptions& options,
                                             std::span<const ExtraSection> extras);

} // namespace nautilus::serialization::detail

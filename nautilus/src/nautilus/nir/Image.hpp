#pragma once

#include "nautilus/nir/Module.hpp"
#include <bit>
#include <cstdint>
#include <span>
#include <vector>

namespace nautilus::nir {

static_assert(std::endian::native == std::endian::little, "NIR 1.0 images are little-endian");

// 13-nir-spec.md §8.1. The header and the section table are also the framing of the artifact container
// (§8.6): they live in their own component so that format can reuse them.

constexpr uint16_t IMAGE_MAJOR = 1;
constexpr uint16_t IMAGE_MINOR = 0;

/// Feature bits (§8.3).
constexpr uint32_t FEATURE_DEBUG_V1 = 1u << 0;      // compat
constexpr uint32_t FEATURE_EXTENSIONS = 1u << 1;    // incompat
constexpr uint32_t FEATURE_PROCESS_LOCAL = 1u << 2; // incompat
constexpr uint32_t KNOWN_INCOMPAT_FEATURES = FEATURE_EXTENSIONS | FEATURE_PROCESS_LOCAL;

struct ImageHeader {
	char magic[4];
	uint16_t major, minor;
	uint32_t compatFeatures;
	uint32_t incompatFeatures;
	uint64_t totalSize;
	uint32_t sectionCount;
	uint32_t crc32c;
	uint8_t reserved[16];
};
static_assert(sizeof(ImageHeader) == 48);

struct SectionEntry {
	uint32_t kind;
	uint32_t stride;
	uint64_t offset;
	uint64_t count;
};
static_assert(sizeof(SectionEntry) == 24);

/// Section kinds (§4.1).
enum SectionKind : uint32_t {
	SecInsts = 1,
	SecExtra = 2,
	SecConsts = 3,
	SecConstBytes = 4,
	SecBlocks = 5,
	SecFunctions = 6,
	SecSignatures = 7,
	SecCallees = 8,
	SecAllocas = 9,
	SecPointers = 10,
	SecBindings = 11,
	SecNames = 12,
	SecNameBytes = 13,
	SecExtensions = 14,
	SecDebugLocations = 15,
	SecDebugInstLocations = 16,
	SecDebugRegions = 17,
	SecDebugInstRegions = 18,
	SecModule = 19,
	SecImports = 20,
	SecDebugNames = 21,
	SecDebugNameBytes = 22,
	SecData = 23,
	SecDataBytes = 24,
	SecFunctionAttrs = 25,
	SecDebugFunctions = 26,
	SecDebugParamNames = 27,
	SecFirstVendor = 1024,
};

/// What the writer and the loader need to know about a section kind.
struct SectionInfo {
	uint32_t kind;
	const char* name;
	uint32_t recordSize; ///< 1 for byte sections, 4 for u32 sections
	bool debug;          ///< an optional debug section (signalled by DEBUG_V1)
	/// The records of the section in `m`, as bytes.
	std::span<const std::byte> (*view)(const Module& m);
	/// Replaces the section of `m` with `count` records of exactly `recordSize` bytes.
	void (*assign)(Module& m, std::span<const std::byte> bytes, uint64_t count);
};

/// All known section kinds, in kind order.
std::span<const SectionInfo> sectionInfos();
const SectionInfo* sectionInfo(uint32_t kind);

/// CRC-32C (Castagnoli) of `data`.
uint32_t crc32c(std::span<const uint8_t> data);

struct WriteOptions {
	/// Write the debug sections and set DEBUG_V1.
	bool debug = false;
};

/// Serializes the module as it is: a canonical module (`canonicalize`) gives the canonical image of §7/§8.1, any
/// other module (a pre-SSA one, for reproducers) a valid image of its tables. Padding, unused fields and reserved bits
/// are written as 0.
std::vector<uint8_t> writeImage(const Module& m, const WriteOptions& options = {});

/// The feature bits the module requires (§8.3).
void imageFeatures(const Module& m, bool debug, uint32_t& compat, uint32_t& incompat);

} // namespace nautilus::nir

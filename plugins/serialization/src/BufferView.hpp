#pragma once

#include "Crc32c.hpp"
#include "nautilus/serialization/IRBinaryFormat.hpp"
#include "nautilus/serialization/IRSerialization.hpp"
#include <cstring>
#include <fmt/format.h>
#include <span>
#include <string_view>
#include <unordered_map>

/// Validation and in-place reading of a serialized buffer, shared by the IR
/// materializer and the module-image reader. Internal to the plugin.
namespace nautilus::serialization::detail {

/// Throws unless @p condition holds. Every check on untrusted input goes
/// through here, so a malformed buffer always surfaces as one exception type.
inline void require(bool condition, std::string_view message) {
	if (!condition) {
		throw SerializationException(fmt::format("malformed buffer: {}", message));
	}
}

/// [begin, begin + count) lies within [0, size). Overflow-safe.
inline bool inRange(uint32_t begin, uint32_t count, uint64_t size) {
	return static_cast<uint64_t>(begin) + count <= size;
}

struct SectionView {
	const std::byte* data = nullptr;
	uint32_t stride = 0;
	uint32_t count = 0;
	bool present = false;
};

/**
 * A validated, zero-copy view over a serialized buffer: the header and the
 * section directory are checked up front, records are read in place on
 * demand. Records are copied out with memcpy, so the buffer needs no
 * particular alignment (a network receive buffer rarely has any).
 */
class BufferView {
public:
	BufferView(std::span<const std::byte> buffer, const DeserializeOptions& options) : buffer(buffer) {
		require(buffer.size() >= sizeof(FileHeader), "shorter than the file header");
		std::memcpy(&header, buffer.data(), sizeof(FileHeader));
		require(std::memcmp(header.magic, MAGIC, sizeof(MAGIC)) == 0, "bad magic");
		if (header.versionMajor != VERSION_MAJOR) {
			throw SerializationException(fmt::format("unsupported format version {}.{} (this reader reads {}.x)",
			                                         header.versionMajor, header.versionMinor, VERSION_MAJOR));
		}
		if ((header.incompatFeatures & ~KNOWN_INCOMPAT_FEATURES) != 0) {
			throw SerializationException(fmt::format("buffer requires unsupported features {:#x}",
			                                         header.incompatFeatures & ~KNOWN_INCOMPAT_FEATURES));
		}
		require(header.headerSize >= sizeof(FileHeader) && header.headerSize <= buffer.size(), "bad header size");
		require(header.totalSize == buffer.size(), "size does not match the header");
		if (options.verifyChecksum) {
			require(crc32c(buffer.subspan(header.headerSize)) == header.checksum, "checksum mismatch");
		}
		require(header.sectionDirOffset <= buffer.size() &&
		            header.sectionCount <= (buffer.size() - header.sectionDirOffset) / sizeof(SectionEntry),
		        "section directory out of bounds");

		for (uint32_t i = 0; i < header.sectionCount; ++i) {
			// Bounds were checked for the whole directory above.
			const uint64_t entryOffset = header.sectionDirOffset + static_cast<uint64_t>(i) * sizeof(SectionEntry);
			SectionEntry entry {};
			std::memcpy(&entry, buffer.subspan(entryOffset, sizeof(SectionEntry)).data(), sizeof(SectionEntry));
			const uint64_t bytes = static_cast<uint64_t>(entry.recordStride) * entry.recordCount;
			require(entry.offset <= buffer.size() && bytes <= buffer.size() - entry.offset, "section out of bounds");
			auto* view = sectionFor(entry.tag);
			if (view == nullptr) {
				if ((entry.flags & SECTION_REQUIRED) != 0) {
					throw SerializationException(fmt::format(
					    "buffer requires unknown section {:#010x}; it was written by a newer writer", entry.tag));
				}
				// Not part of the IR: kept for a layer above (e.g. a module
				// manifest) that knows the tag, and otherwise ignored.
				require(!extras.contains(entry.tag), "duplicate section");
				extras.emplace(entry.tag,
				               SectionView {buffer.data() + entry.offset, entry.recordStride, entry.recordCount, true});
				continue;
			}
			require(!view->present, "duplicate section");
			*view = SectionView {buffer.data() + entry.offset, entry.recordStride, entry.recordCount, true};
		}

		requireSection(strings, 1, true);
		requireSection(module, sizeof(ModuleRecord));
		requireSection(pointers, sizeof(PointerRecord));
		requireSection(targets, sizeof(TargetRecord));
		requireSection(types, 1, true);
		requireSection(functions, sizeof(FunctionRecord));
		requireSection(blocks, sizeof(BlockRecord));
		requireSection(ops, sizeof(OpRecord));
		requireSection(operands, sizeof(uint32_t), true);
		requireSection(edges, sizeof(EdgeRecord));
		requireSection(constants, sizeof(uint64_t), true);
		requireSection(calls, sizeof(CallRecord));
		requireSection(destructors, sizeof(DestructorRecord));
		requireSection(allocas, sizeof(AllocaRecord));
		requireSection(attributes, sizeof(AttributeRecord));
		requireSection(regions, sizeof(RegionRecord));
		requireSection(stringRefs, sizeof(StringRef));
		requireSection(indices, sizeof(uint32_t), true);
		requireSection(callSites, sizeof(CallSiteRecord));
		require(module.count == 1, "expected exactly one module record");
	}

	/// Record @p index of @p section. A record written with a larger stride
	/// by a newer minor version is read through its known prefix.
	template <typename T>
	T get(const SectionView& section, uint32_t index) const {
		require(index < section.count, "record index out of range");
		T value {};
		std::memcpy(&value, section.data + static_cast<uint64_t>(index) * section.stride, sizeof(T));
		return value;
	}

	std::string_view string(StringRef ref) const {
		require(ref.offset != NONE, "unexpected null string");
		require(inRange(ref.offset, ref.length, strings.count), "string out of range");
		return {reinterpret_cast<const char*>(strings.data) + ref.offset, ref.length};
	}

	FileHeader header {};
	/// Sections the IR does not define, by tag.
	std::unordered_map<uint32_t, SectionView> extras;
	SectionView strings, module, pointers, targets, types, functions, blocks, ops, operands, edges, constants, calls,
	    destructors, allocas, attributes, regions, stringRefs, indices, callSites;

private:
	SectionView* sectionFor(uint32_t tag) {
		switch (tag) {
		case TAG_STRINGS:
			return &strings;
		case TAG_MODULE:
			return &module;
		case TAG_POINTERS:
			return &pointers;
		case TAG_TARGETS:
			return &targets;
		case TAG_TYPES:
			return &types;
		case TAG_FUNCTIONS:
			return &functions;
		case TAG_BLOCKS:
			return &blocks;
		case TAG_OPERATIONS:
			return &ops;
		case TAG_OPERANDS:
			return &operands;
		case TAG_EDGES:
			return &edges;
		case TAG_CONSTANTS:
			return &constants;
		case TAG_CALLS:
			return &calls;
		case TAG_DESTRUCTORS:
			return &destructors;
		case TAG_ALLOCAS:
			return &allocas;
		case TAG_ATTRIBUTES:
			return &attributes;
		case TAG_REGIONS:
			return &regions;
		case TAG_STRING_REFS:
			return &stringRefs;
		case TAG_INDICES:
			return &indices;
		case TAG_CALL_SITES:
			return &callSites;
		default:
			return nullptr;
		}
	}

	/// A pool of scalars must use exactly its element size; a record section
	/// may use a larger stride (fields appended by a later minor version).
	static void requireSection(const SectionView& section, uint32_t size, bool exact = false) {
		require(section.present, "missing section");
		require(exact ? section.stride == size : section.stride >= size, "section has an unexpected record size");
	}

	std::span<const std::byte> buffer;
};

} // namespace nautilus::serialization::detail

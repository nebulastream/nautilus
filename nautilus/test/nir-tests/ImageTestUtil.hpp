#pragma once

#include "nautilus/nir/Image.hpp"
#include "nautilus/nir/ImageLoader.hpp"
#include <cstring>
#include <stdexcept>
#include <string>
#include <vector>

namespace nautilus::nir::test {

/// An image taken apart into its sections, so tests can damage one part and write it back with a valid checksum.
struct RawImage {
	ImageHeader header;
	struct Section {
		uint32_t kind;
		uint32_t stride;
		uint64_t count;
		std::vector<uint8_t> bytes;
	};
	std::vector<Section> sections;

	static RawImage decode(const std::vector<uint8_t>& image) {
		RawImage r;
		std::memcpy(&r.header, image.data(), sizeof(ImageHeader));
		for (uint32_t i = 0; i < r.header.sectionCount; ++i) {
			SectionEntry e {};
			std::memcpy(&e, image.data() + sizeof(ImageHeader) + i * sizeof(SectionEntry), sizeof e);
			Section s {e.kind, e.stride, e.count, {}};
			s.bytes.assign(image.begin() + e.offset, image.begin() + e.offset + e.count * e.stride);
			r.sections.push_back(std::move(s));
		}
		return r;
	}

	Section& section(uint32_t kind) {
		for (auto& s : sections) {
			if (s.kind == kind) {
				return s;
			}
		}
		throw std::logic_error("no such section");
	}

	/// Lays the sections out as a canonical image would and fills in totalSize and the checksum.
	std::vector<uint8_t> encode(bool fixCrc = true) const {
		uint64_t offset = sizeof(ImageHeader) + sections.size() * sizeof(SectionEntry);
		std::vector<SectionEntry> entries;
		for (auto& s : sections) {
			entries.push_back({s.kind, s.stride, offset, s.count});
			offset += (s.bytes.size() + 7) / 8 * 8;
		}
		std::vector<uint8_t> image(offset, 0);
		auto h = header;
		h.totalSize = image.size();
		h.sectionCount = static_cast<uint32_t>(sections.size());
		std::memcpy(image.data() + sizeof(ImageHeader), entries.data(), entries.size() * sizeof(SectionEntry));
		for (size_t i = 0; i < sections.size(); ++i) {
			if (!sections[i].bytes.empty()) {
				std::memcpy(image.data() + entries[i].offset, sections[i].bytes.data(), sections[i].bytes.size());
			}
		}
		if (fixCrc) {
			h.crc32c = crc32c(std::span<const uint8_t>(image).subspan(sizeof(ImageHeader)));
		}
		std::memcpy(image.data(), &h, sizeof h);
		return image;
	}
};

/// Recomputes the checksum of an image whose bytes were edited in place.
inline void fixChecksum(std::vector<uint8_t>& image) {
	ImageHeader h {};
	std::memcpy(&h, image.data(), sizeof h);
	h.crc32c = crc32c(std::span<const uint8_t>(image).subspan(sizeof(ImageHeader)));
	std::memcpy(image.data(), &h, sizeof h);
}

inline std::string why(const LoadResult& r) {
	return r.error ? r.error->str() : std::string();
}

} // namespace nautilus::nir::test

#include "nautilus/nir/Inspect.hpp"
#include "nautilus/nir/Image.hpp"
#include <cstdio>
#include <cstring>

namespace nautilus::nir {

namespace {

std::string hexBytes(const uint8_t* p, size_t n) {
	std::string s;
	char buf[4];
	for (size_t i = 0; i < n; ++i) {
		std::snprintf(buf, sizeof buf, "%02x", p[i]);
		s += buf;
		if (i + 1 < n) {
			s += ' ';
		}
	}
	return s;
}

std::string line(uint64_t offset, const uint8_t* p, size_t n, const std::string& note) {
	char buf[16];
	std::snprintf(buf, sizeof buf, "%08llx", static_cast<unsigned long long>(offset));
	std::string s = buf;
	s += "  " + hexBytes(p, n);
	if (!note.empty()) {
		s += std::string(n < 16 ? (16 - n) * 3 : 0, ' ') + "  ; " + note;
	}
	return s + "\n";
}

std::string features(uint32_t compat, uint32_t incompat) {
	std::string s;
	auto add = [&](const char* n) {
		s += s.empty() ? n : std::string(" | ") + n;
	};
	if (compat & FEATURE_DEBUG_V1) {
		add("DEBUG_V1");
	}
	if (incompat & FEATURE_EXTENSIONS) {
		add("EXTENSIONS");
	}
	if (incompat & FEATURE_PROCESS_LOCAL) {
		add("PROCESS_LOCAL");
	}
	return s.empty() ? "none" : s;
}

} // namespace

std::string inspectImage(std::span<const uint8_t> image, const InspectOptions& options) {
	std::string out;
	if (image.size() < sizeof(ImageHeader)) {
		return "not an image: " + std::to_string(image.size()) + " bytes, a header has 48\n";
	}
	ImageHeader h {};
	std::memcpy(&h, image.data(), sizeof h);
	bool magic = std::memcmp(h.magic, "NIR\0", 4) == 0;
	out += std::string("NIR image, magic ") + (magic ? "ok" : "BAD") + "\n";
	out += "version " + std::to_string(h.major) + "." + std::to_string(h.minor) + "\n";
	out += "compat features: " + features(h.compatFeatures, 0) + "\n";
	out += "incompat features: " + features(0, h.incompatFeatures) + "\n";
	out += "totalSize " + std::to_string(h.totalSize) +
	       (h.totalSize == image.size() ? "" : " (buffer is " + std::to_string(image.size()) + ")") + "\n";
	bool crcOk = crc32c(image.subspan(sizeof(ImageHeader))) == h.crc32c;
	char crc[16];
	std::snprintf(crc, sizeof crc, "%08x", h.crc32c);
	out += std::string("crc32c ") + crc + (crcOk ? " (ok)" : " (MISMATCH)") + "\n";
	out += "sections " + std::to_string(h.sectionCount) + "\n";

	uint64_t tableBytes = uint64_t(h.sectionCount) * sizeof(SectionEntry);
	if (tableBytes > image.size() - sizeof(ImageHeader)) {
		return out + "the section table is out of bounds\n";
	}
	std::vector<SectionEntry> entries(h.sectionCount);
	if (!entries.empty()) {
		std::memcpy(entries.data(), image.data() + sizeof(ImageHeader), tableBytes);
	}
	if (options.hexDump) {
		out += "\nheader\n";
		const uint8_t* p = image.data();
		out += line(0, p, 4, "magic");
		out += line(4, p + 4, 4, "major, minor");
		out += line(8, p + 8, 4, "compatFeatures");
		out += line(12, p + 12, 4, "incompatFeatures");
		out += line(16, p + 16, 8, "totalSize");
		out += line(24, p + 24, 4, "sectionCount");
		out += line(28, p + 28, 4, "crc32c");
		out += line(32, p + 32, 16, "reserved");
		out += "\nsection table\n";
	}
	for (size_t i = 0; i < entries.size(); ++i) {
		auto& e = entries[i];
		auto* info = sectionInfo(e.kind);
		out += "  kind " + std::to_string(e.kind) + " " + (info ? info->name : "(unknown)") + ": stride " +
		       std::to_string(e.stride) + ", offset " + std::to_string(e.offset) + ", count " +
		       std::to_string(e.count) + "\n";
		if (options.hexDump) {
			uint64_t at = sizeof(ImageHeader) + i * sizeof(SectionEntry);
			out += line(at, image.data() + at, 8, "kind, stride");
			out += line(at + 8, image.data() + at + 8, 16,
			            std::string("offset, count") + (info ? std::string(" -> ") + info->name : ""));
		}
	}
	if (options.hexDump) {
		for (auto& e : entries) {
			auto* info = sectionInfo(e.kind);
			if (e.stride == 0 || (e.count != 0 && e.stride > UINT64_MAX / e.count)) {
				continue;
			}
			uint64_t size = e.count * e.stride;
			if (e.offset > image.size() || size > image.size() - e.offset) {
				continue;
			}
			out += std::string("\n") + (info ? info->name : "section " + std::to_string(e.kind)) + " (" +
			       std::to_string(e.count) + " x " + std::to_string(e.stride) + " bytes)\n";
			for (uint64_t i = 0; i < e.count; ++i) {
				uint64_t at = e.offset + i * e.stride;
				if (e.stride <= 16) {
					out += line(at, image.data() + at, e.stride, "[" + std::to_string(i) + "]");
				} else {
					for (uint64_t k = 0; k < e.stride; k += 16) {
						out += line(at + k, image.data() + at + k, std::min<uint64_t>(16, e.stride - k),
						            k == 0 ? "[" + std::to_string(i) + "]" : "");
					}
				}
			}
		}
	}
	return out;
}

} // namespace nautilus::nir

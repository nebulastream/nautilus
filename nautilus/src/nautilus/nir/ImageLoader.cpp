#include "nautilus/nir/ImageLoader.hpp"
#include <algorithm>
#include <cstring>

namespace nautilus::nir {

std::string LoadError::str() const {
	return std::string(codeName(code)) + ": " + message;
}

namespace {

struct Reject {
	LoadError error;
};

[[noreturn]] void reject(DiagCode code, std::string message) {
	throw Reject {LoadError {code, std::move(message), {}}};
}

template <class T>
T readAt(std::span<const uint8_t> image, uint64_t offset) {
	T v;
	std::memcpy(&v, image.data() + offset, sizeof v);
	return v;
}

} // namespace

LoadResult loadImage(std::span<const uint8_t> image, const LoadOptions& options) {
	LoadResult result;
	try {
		auto& limits = options.limits;
		if (image.size() < sizeof(ImageHeader) || std::memcmp(image.data(), "NIR\0", 4) != 0) {
			reject(DiagCode::L001, "bad magic");
		}
		auto header = readAt<ImageHeader>(image, 0);
		result.header = header;
		if (header.major > IMAGE_MAJOR) {
			reject(DiagCode::L002, "format version " + std::to_string(header.major) + "." +
			                           std::to_string(header.minor) + " is newer than this reader's " +
			                           std::to_string(IMAGE_MAJOR) + "." + std::to_string(IMAGE_MINOR));
		}
		if ((header.incompatFeatures & ~KNOWN_INCOMPAT_FEATURES) != 0) {
			reject(DiagCode::L003, "unknown incompatible feature bits " +
			                           std::to_string(header.incompatFeatures & ~KNOWN_INCOMPAT_FEATURES));
		}
		if (header.totalSize != image.size()) {
			reject(DiagCode::L004, "totalSize " + std::to_string(header.totalSize) +
			                           " differs from the buffer length " + std::to_string(image.size()));
		}
		if (crc32c(image.subspan(sizeof(ImageHeader))) != header.crc32c) {
			reject(DiagCode::L005, "checksum mismatch");
		}
		if (header.totalSize > limits.totalSize) {
			reject(DiagCode::L008, "totalSize exceeds the limit");
		}

		// section table: read the entries only as far as the buffer allows, apply the count limits of the known
		// kinds, then validate the structure
		uint64_t tableBytes = uint64_t(header.sectionCount) * sizeof(SectionEntry);
		if (tableBytes > image.size() - sizeof(ImageHeader)) {
			reject(DiagCode::L006, "the section table is out of bounds");
		}
		std::vector<SectionEntry> entries(header.sectionCount);
		if (!entries.empty()) {
			std::memcpy(entries.data(), image.data() + sizeof(ImageHeader), tableBytes);
		}
		auto countLimit = [&](uint32_t kind) -> uint64_t {
			switch (kind) {
			case SecFunctions:
				return limits.functions;
			case SecExtra:
				return limits.extraWords;
			case SecExtensions:
				return limits.extensionEntries;
			case SecDataBytes:
				return limits.dataBytes;
			default:
				return limits.tableEntries;
			}
		};
		for (auto& e : entries) {
			if (sectionInfo(e.kind) && e.count > countLimit(e.kind)) {
				reject(DiagCode::L008, std::string("section ") + sectionInfo(e.kind)->name + " exceeds the limit");
			}
		}
		uint64_t previousKind = 0;
		uint64_t dataStart = sizeof(ImageHeader) + tableBytes;
		struct Range {
			uint64_t begin, end;
		};
		std::vector<Range> ranges;
		for (auto& e : entries) {
			if (e.kind <= previousKind) {
				reject(DiagCode::L006,
				       "section entries are not in increasing kind order (kind " + std::to_string(e.kind) + ")");
			}
			previousKind = e.kind;
			if (e.offset % 8 != 0) {
				reject(DiagCode::L006, "section " + std::to_string(e.kind) + " is not 8-byte aligned");
			}
			auto* info = sectionInfo(e.kind);
			if (info != nullptr) {
				if (e.stride < info->recordSize ||
				    ((info->recordSize == 1 || info->recordSize == 4) && e.stride != info->recordSize)) {
					reject(DiagCode::L007, std::string("section ") + info->name + " has an invalid stride");
				}
			} else if (e.stride == 0) {
				reject(DiagCode::L007, "section " + std::to_string(e.kind) + " has stride 0");
			}
			// bounds, without letting the product overflow
			if (e.count != 0 && e.stride > UINT64_MAX / e.count) {
				reject(DiagCode::L006, "section " + std::to_string(e.kind) + " is out of bounds");
			}
			uint64_t size = e.count * e.stride;
			if (e.offset > header.totalSize || size > header.totalSize - e.offset) {
				reject(DiagCode::L006, "section " + std::to_string(e.kind) + " is out of bounds");
			}
			if (size != 0) {
				if (e.offset < dataStart) {
					reject(DiagCode::L006, "section " + std::to_string(e.kind) + " overlaps the section table");
				}
				ranges.push_back({e.offset, e.offset + size});
			}
		}
		std::sort(ranges.begin(), ranges.end(), [](auto& a, auto& b) { return a.begin < b.begin; });
		for (size_t i = 1; i < ranges.size(); ++i) {
			if (ranges[i].begin < ranges[i - 1].end) {
				reject(DiagCode::L006, "sections overlap");
			}
		}
		bool hasModule = false;
		for (auto& e : entries) {
			hasModule |= e.kind == SecModule && e.count == 1;
			if (e.kind == SecModule && e.count > 1) {
				reject(DiagCode::L006, "more than one module record");
			}
		}
		if (!hasModule) {
			reject(DiagCode::L009, "the module section is missing");
		}

		// per-function limits are known once the functions are read; do that before the large sections are copied
		for (auto& e : entries) {
			if (e.kind != SecFunctions) {
				continue;
			}
			for (uint64_t i = 0; i < e.count; ++i) {
				FunctionRecord f {};
				std::memcpy(&f, image.data() + e.offset + i * e.stride, sizeof f);
				if (f.instCount > limits.instsPerFunction || f.blockCount > limits.blocksPerFunction) {
					reject(DiagCode::L008, "function " + std::to_string(i) + " exceeds the instruction or block limit");
				}
			}
		}

		// materialize
		Module m;
		for (auto& e : entries) {
			auto* info = sectionInfo(e.kind);
			if (info == nullptr) {
				continue; // unknown kinds and vendor sections are skipped
			}
			if (e.stride == info->recordSize) {
				info->assign(m, std::as_bytes(image.subspan(e.offset, e.count * e.stride)), e.count);
			} else {
				// a wider stride (a later minor version): copy the part this reader knows
				std::vector<std::byte> compact(e.count * info->recordSize);
				for (uint64_t i = 0; i < e.count; ++i) {
					std::memcpy(compact.data() + i * info->recordSize, image.data() + e.offset + i * e.stride,
					            info->recordSize);
				}
				info->assign(m, compact, e.count);
			}
		}
		m.rebuildIndexes();

		if ((header.incompatFeatures & FEATURE_PROCESS_LOCAL) && !options.ignoreProcessToken) {
			auto token = options.processToken != 0 ? options.processToken : currentProcessToken();
			if (m.module.processToken != token) {
				reject(DiagCode::L014, "the image is process-local and was produced by another process");
			}
		}
		if (!options.allowPreSSA) {
			for (auto& f : m.functions) {
				if (f.level == static_cast<uint8_t>(Level::PreSSA)) {
					reject(DiagCode::L016, "the image has a pre-SSA function");
				}
			}
		}
		if (options.verify) {
			VerifyOptions vo = options.verifyOptions;
			vo.deserialized = true;
			auto diagnostics = verify(m, vo);
			if (!diagnostics.empty()) {
				LoadError e {DiagCode::L017, "the verifier rejects the module: " + diagnostics[0].str(), diagnostics};
				throw Reject {std::move(e)};
			}
		}
		result.module = std::move(m);
	} catch (Reject& r) {
		result.error = std::move(r.error);
	}
	return result;
}

} // namespace nautilus::nir

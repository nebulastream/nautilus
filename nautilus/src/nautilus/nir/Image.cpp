#include "nautilus/nir/Image.hpp"
#include <array>
#include <cstring>

namespace nautilus::nir {

namespace {

template <class T>
std::span<const std::byte> asBytes(const std::vector<T>& v) {
	return std::as_bytes(std::span<const T>(v));
}

template <class T>
void copyInto(std::vector<T>& v, std::span<const std::byte> bytes, uint64_t count) {
	v.resize(count);
	if (count != 0) {
		std::memcpy(v.data(), bytes.data(), count * sizeof(T));
	}
}

// the sections are exactly the arrays of Module; each entry maps a kind to its array
#define SECTION_FIELD(KIND, NAME, FIELD, TYPE, DEBUG)                                                                  \
	{KIND,                                                                                                             \
	 NAME,                                                                                                             \
	 sizeof(TYPE),                                                                                                     \
	 DEBUG,                                                                                                            \
	 [](const Module& m) { return asBytes(m.FIELD); },                                                                 \
	 [](Module& m, std::span<const std::byte> b, uint64_t n) { copyInto(m.FIELD, b, n); }}

const SectionInfo infos[] = {
    SECTION_FIELD(SecInsts, "insts", insts, Inst, false),
    SECTION_FIELD(SecExtra, "extra", extra, uint32_t, false),
    {SecConsts, "consts", sizeof(ConstRecord), false, [](const Module& m) { return std::as_bytes(m.consts.records()); },
     [](Module& m, std::span<const std::byte> b, uint64_t n) {
	     std::vector<ConstRecord> records(n);
	     if (n != 0) {
		     std::memcpy(records.data(), b.data(), n * sizeof(ConstRecord));
	     }
	     // the bytes of wide constants are assigned by SecConstBytes, which follows in kind order
	     m.consts.reset(std::move(records), {m.consts.bytes().begin(), m.consts.bytes().end()});
     }},
    {SecConstBytes, "constBytes", 1, false, [](const Module& m) { return m.consts.bytes(); },
     [](Module& m, std::span<const std::byte> b, uint64_t) {
	     std::vector<ConstRecord> records(m.consts.records().begin(), m.consts.records().end());
	     m.consts.reset(std::move(records), {b.begin(), b.end()});
     }},
    SECTION_FIELD(SecBlocks, "blocks", blocks, BlockRecord, false),
    SECTION_FIELD(SecFunctions, "functions", functions, FunctionRecord, false),
    SECTION_FIELD(SecSignatures, "signatures", signatures, SignatureRecord, false),
    SECTION_FIELD(SecCallees, "callees", callees, CalleeRecord, false),
    SECTION_FIELD(SecAllocas, "allocas", allocas, AllocaRecord, false),
    SECTION_FIELD(SecPointers, "pointers", pointers, PointerRecord, false),
    SECTION_FIELD(SecBindings, "bindings", bindings, BindingRecord, false),
    SECTION_FIELD(SecNames, "names", names, NameRecord, false),
    SECTION_FIELD(SecNameBytes, "nameBytes", nameBytes, char, false),
    SECTION_FIELD(SecExtensions, "extensions", extensions, ExtensionRecord, false),
    SECTION_FIELD(SecDebugLocations, "debugLocations", debugLocations, DebugLocationRecord, true),
    SECTION_FIELD(SecDebugInstLocations, "debugInstLocations", debugInstLocations, uint32_t, true),
    SECTION_FIELD(SecDebugRegions, "debugRegions", debugRegions, RegionRecord, true),
    SECTION_FIELD(SecDebugInstRegions, "debugInstRegions", debugInstRegions, uint32_t, true),
    {SecModule, "module", sizeof(ModuleRecord), false,
     [](const Module& m) { return std::as_bytes(std::span<const ModuleRecord>(&m.module, 1)); },
     [](Module& m, std::span<const std::byte> b, uint64_t n) {
	     if (n != 0) {
		     std::memcpy(&m.module, b.data(), sizeof(ModuleRecord));
	     }
     }},
    SECTION_FIELD(SecImports, "imports", imports, ImportRecord, false),
    SECTION_FIELD(SecDebugNames, "debugNames", debugNames, NameRecord, true),
    SECTION_FIELD(SecDebugNameBytes, "debugNameBytes", debugNameBytes, char, true),
    SECTION_FIELD(SecData, "data", data, DataRecord, false),
    SECTION_FIELD(SecDataBytes, "dataBytes", dataBytes, std::byte, false),
    SECTION_FIELD(SecFunctionAttrs, "functionAttrs", functionAttrs, FunctionAttrRecord, false),
    SECTION_FIELD(SecDebugFunctions, "debugFunctions", debugFunctions, DebugFunctionRecord, true),
    SECTION_FIELD(SecDebugParamNames, "debugParamNames", debugParamNames, uint32_t, true),
};
#undef SECTION_FIELD

constexpr std::array<uint32_t, 256> makeCrcTable() {
	std::array<uint32_t, 256> t {};
	for (uint32_t i = 0; i < 256; ++i) {
		uint32_t c = i;
		for (int k = 0; k < 8; ++k) {
			c = (c & 1) ? (c >> 1) ^ 0x82F63B78u : c >> 1;
		}
		t[i] = c;
	}
	return t;
}

} // namespace

std::span<const SectionInfo> sectionInfos() {
	return infos;
}

const SectionInfo* sectionInfo(uint32_t kind) {
	// the table is in kind order but not dense (kinds 1-27 are all present), so index directly
	if (kind >= 1 && kind <= std::size(infos) && infos[kind - 1].kind == kind) {
		return &infos[kind - 1];
	}
	return nullptr;
}

uint32_t crc32c(std::span<const uint8_t> data) {
	static constexpr auto table = makeCrcTable();
	uint32_t c = 0xFFFFFFFFu;
	for (auto b : data) {
		c = table[(c ^ b) & 0xFF] ^ (c >> 8);
	}
	return ~c;
}

void imageFeatures(const Module& m, bool debug, uint32_t& compat, uint32_t& incompat) {
	compat = debug ? FEATURE_DEBUG_V1 : 0;
	incompat = 0;
	if (!m.extensions.empty()) {
		incompat |= FEATURE_EXTENSIONS;
	}
	if (m.module.processToken != 0) {
		incompat |= FEATURE_PROCESS_LOCAL;
	}
}

std::vector<uint8_t> writeImage(const Module& source, const WriteOptions& options) {
	// padding and reserved fields are written as 0 whatever the in-memory records hold
	Module m = source;
	for (auto& f : m.functions) {
		f.pad = 0;
	}
	for (auto& b : m.blocks) {
		b.pad = 0;
		b.flags &= BLOCK_LANDING_PAD;
	}
	for (auto& c : m.callees) {
		c.pad[0] = c.pad[1] = c.pad[2] = 0;
	}
	for (auto& p : m.pointers) {
		p.pad = 0;
		p.reserved = 0;
	}
	for (auto& r : m.functionAttrs) {
		r.pad = 0;
	}
	for (auto& e : m.extensions) {
		e.pad[0] = e.pad[1] = e.pad[2] = 0;
	}
	m.module.flags = 0;
	{
		std::vector<ConstRecord> cr(m.consts.records().begin(), m.consts.records().end());
		for (auto& c : cr) {
			c.pad = 0;
		}
		m.consts.reset(std::move(cr), {m.consts.bytes().begin(), m.consts.bytes().end()});
	}

	std::vector<const SectionInfo*> sections;
	for (auto& info : infos) {
		if (!info.debug || options.debug) {
			sections.push_back(&info);
		}
	}
	auto tableEnd = sizeof(ImageHeader) + sizeof(SectionEntry) * sections.size();
	std::vector<SectionEntry> entries;
	std::vector<std::span<const std::byte>> data;
	uint64_t offset = tableEnd;
	for (auto* info : sections) {
		auto bytes = info->view(m);
		SectionEntry e {};
		e.kind = info->kind;
		e.stride = info->recordSize;
		e.offset = offset;
		e.count = bytes.size() / info->recordSize;
		entries.push_back(e);
		data.push_back(bytes);
		offset += (bytes.size() + 7) / 8 * 8;
	}
	std::vector<uint8_t> image(offset, 0);
	std::memcpy(image.data() + sizeof(ImageHeader), entries.data(), entries.size() * sizeof(SectionEntry));
	for (size_t i = 0; i < data.size(); ++i) {
		if (!data[i].empty()) {
			std::memcpy(image.data() + entries[i].offset, data[i].data(), data[i].size());
		}
	}
	ImageHeader h {};
	std::memcpy(h.magic, "NIR\0", 4);
	h.major = IMAGE_MAJOR;
	h.minor = IMAGE_MINOR;
	imageFeatures(m, options.debug, h.compatFeatures, h.incompatFeatures);
	h.totalSize = image.size();
	h.sectionCount = static_cast<uint32_t>(sections.size());
	h.crc32c = crc32c(std::span<const uint8_t>(image).subspan(sizeof(ImageHeader)));
	std::memcpy(image.data(), &h, sizeof h);
	return image;
}

} // namespace nautilus::nir

#include "ImageTestUtil.hpp"
#include "TestModules.hpp"
#include "nautilus/nir/Canonicalize.hpp"
#include "nautilus/nir/ImageLoader.hpp"
#include "nautilus/nir/Inspect.hpp"
#include "nautilus/nir/Parser.hpp"
#include "nautilus/nir/Printer.hpp"
#include <catch2/catch_all.hpp>

using namespace nautilus::nir;
using test::RawImage;

namespace {

const char* const small = R"(nir 1.0
module "m"
pointer #p0 = callee symbol "puts"
callee #c0 = external #p0 : (i64) -> i64 attrs(noUnwind)

func export @f : (i64) -> i64 {
^b0(%0: i64):
  %1 = call #c0(%0)
  %2 = add %1, 42:i64
  ret %2
}
)";

Module parseOk(const std::string& text) {
	auto r = parse(text);
	REQUIRE(r.ok());
	return std::move(*r.module);
}

std::vector<uint8_t> imageOf(const std::string& text) {
	return writeImage(canonicalize(parseOk(text)));
}

LoadError rejection(const std::vector<uint8_t>& image, const LoadOptions& options = {}) {
	auto r = loadImage(image, options);
	REQUIRE_FALSE(r.ok());
	REQUIRE(r.error.has_value());
	return *r.error;
}

} // namespace

TEST_CASE("CRC-32C check value", "[nir][image]") {
	std::string s = "123456789";
	REQUIRE(crc32c(std::span<const uint8_t>(reinterpret_cast<const uint8_t*>(s.data()), s.size())) == 0xE3069283u);
	REQUIRE(crc32c({}) == 0);
}

TEST_CASE("a canonical image has the layout of 13 §8.1", "[nir][image]") {
	auto image = imageOf(small);
	ImageHeader h;
	std::memcpy(&h, image.data(), sizeof h);
	REQUIRE(std::memcmp(h.magic, "NIR\0", 4) == 0);
	REQUIRE(h.major == 1);
	REQUIRE(h.minor == 0);
	REQUIRE(h.compatFeatures == 0);
	REQUIRE(h.incompatFeatures == 0); // the pointer is portable (mode 1)
	REQUIRE(h.totalSize == image.size());
	REQUIRE(image.size() % 8 == 0);
	// canonical images list the sections 1-14, 19, 20, 23, 24, 25, including empty ones
	REQUIRE(h.sectionCount == 19);
	REQUIRE(h.crc32c == crc32c(std::span<const uint8_t>(image).subspan(48)));
	for (int i : {0, 1, 2}) {
		REQUIRE(h.reserved[i] == 0);
	}
	uint64_t expectedOffset = 48 + 24 * 19;
	uint32_t previous = 0;
	for (uint32_t i = 0; i < h.sectionCount; ++i) {
		SectionEntry e;
		std::memcpy(&e, image.data() + 48 + 24 * i, sizeof e);
		REQUIRE(e.kind > previous);
		previous = e.kind;
		REQUIRE(e.offset == expectedOffset);
		REQUIRE(e.offset % 8 == 0);
		REQUIRE(e.stride == sectionInfo(e.kind)->recordSize);
		expectedOffset += (e.count * e.stride + 7) / 8 * 8;
	}
	REQUIRE(expectedOffset == h.totalSize);
}

TEST_CASE("an empty section has the offset where the next one starts", "[nir][image]") {
	auto image = imageOf(small);
	RawImage raw = RawImage::decode(image);
	REQUIRE(raw.section(SecImports).count == 0);
	SectionEntry imports, data;
	for (uint32_t i = 0; i < raw.header.sectionCount; ++i) {
		SectionEntry e;
		std::memcpy(&e, image.data() + 48 + 24 * i, sizeof e);
		if (e.kind == SecImports) {
			imports = e;
		}
		if (e.kind == SecData) {
			data = e;
		}
	}
	REQUIRE(imports.offset == data.offset);
}

TEST_CASE("debug sections are written exactly when asked", "[nir][image]") {
	auto m = canonicalize(parseOk(small));
	WriteOptions o;
	o.debug = true;
	auto image = writeImage(m, o);
	ImageHeader h;
	std::memcpy(&h, image.data(), sizeof h);
	REQUIRE(h.sectionCount == 27);
	REQUIRE((h.compatFeatures & FEATURE_DEBUG_V1) != 0);
	REQUIRE(loadImage(image).ok());
}

TEST_CASE("load(write(m)) gives the module back and re-serialization is byte-identical", "[nir][image]") {
	for (auto* text : {small}) {
		auto canonical = canonicalize(parseOk(text));
		auto image = writeImage(canonical);
		auto loaded = loadImage(image, [] {
			LoadOptions o;
			o.ignoreProcessToken = true;
			return o;
		}());
		INFO(test::why(loaded));
		REQUIRE(loaded.ok());
		REQUIRE(sectionsEqual(canonical, *loaded.module));
		REQUIRE(writeImage(*loaded.module) == image);
		REQUIRE(print(*loaded.module) == print(canonical));
	}
}

TEST_CASE("the conformance-style module survives a save and a load", "[nir][image]") {
	Module m;
	test::buildAllSSA(m);
	m.module.processToken = currentProcessToken();
	auto canonical = canonicalize(m);
	auto image = writeImage(canonical);
	auto loaded = loadImage(image);
	INFO(test::why(loaded));
	REQUIRE(loaded.ok());
	REQUIRE(sectionsEqual(canonical, *loaded.module));
	REQUIRE(writeImage(*loaded.module) == image);
}

TEST_CASE("pre-SSA modules are written as they are and load only for debugging", "[nir][image]") {
	Module m;
	test::buildPreSSA(m);
	auto image = writeImage(m);
	REQUIRE(rejection(image).code == DiagCode::L016);
	LoadOptions o;
	o.allowPreSSA = true;
	auto loaded = loadImage(image, o);
	INFO(test::why(loaded));
	REQUIRE(loaded.ok());
	REQUIRE(print(*loaded.module) == print(m));
}

TEST_CASE("NIR-L001 magic", "[nir][image]") {
	auto image = imageOf(small);
	image[0] = 'X';
	REQUIRE(rejection(image).code == DiagCode::L001);
	REQUIRE(rejection({}).code == DiagCode::L001);
	REQUIRE(rejection(std::vector<uint8_t>(10, 0)).code == DiagCode::L001);
}

TEST_CASE("NIR-L002 and NIR-L003 version and features", "[nir][image]") {
	auto raw = RawImage::decode(imageOf(small));
	raw.header.major = 2;
	REQUIRE(rejection(raw.encode()).code == DiagCode::L002);
	raw.header.major = 1;
	raw.header.minor = 7; // a later minor version is accepted
	REQUIRE(loadImage(raw.encode(),
	                  [] {
		                  LoadOptions o;
		                  o.ignoreProcessToken = true;
		                  return o;
	                  }())
	            .ok());
	raw.header.incompatFeatures |= 0x80;
	REQUIRE(rejection(raw.encode()).code == DiagCode::L003);
	raw.header.incompatFeatures &= ~0x80u;
	raw.header.compatFeatures |= 0x80; // unknown compat bits may be ignored
	LoadOptions o;
	o.ignoreProcessToken = true;
	REQUIRE(loadImage(raw.encode(), o).ok());
}

TEST_CASE("NIR-L004 total size", "[nir][image]") {
	auto image = imageOf(small);
	auto shorter = image;
	shorter.resize(image.size() - 8);
	REQUIRE(rejection(shorter).code == DiagCode::L004);
	auto longer = image;
	longer.resize(image.size() + 8);
	REQUIRE(rejection(longer).code == DiagCode::L004);
}

TEST_CASE("NIR-L005 checksum", "[nir][image]") {
	auto image = imageOf(small);
	image[image.size() - 9] ^= 1;
	REQUIRE(rejection(image).code == DiagCode::L005);
	auto crc = imageOf(small);
	crc[28] ^= 1;
	REQUIRE(rejection(crc).code == DiagCode::L005);
}

TEST_CASE("NIR-L006 section table", "[nir][image]") {
	auto base = imageOf(small);
	auto edit = [&](auto&& f) {
		auto image = base;
		f(image);
		test::fixChecksum(image);
		return rejection(image).code;
	};
	auto entryAt = [](std::vector<uint8_t>& image, uint32_t i) {
		SectionEntry e;
		std::memcpy(&e, image.data() + 48 + 24 * i, sizeof e);
		return e;
	};
	auto setEntry = [](std::vector<uint8_t>& image, uint32_t i, const SectionEntry& e) {
		std::memcpy(image.data() + 48 + 24 * i, &e, sizeof e);
	};
	// order
	REQUIRE(edit([&](auto& im) {
		        auto a = entryAt(im, 3), b = entryAt(im, 4);
		        setEntry(im, 3, b);
		        setEntry(im, 4, a);
	        }) == DiagCode::L006);
	// duplicate kind
	REQUIRE(edit([&](auto& im) {
		        auto a = entryAt(im, 3);
		        auto b = entryAt(im, 4);
		        b.kind = a.kind;
		        setEntry(im, 4, b);
	        }) == DiagCode::L006);
	// alignment
	REQUIRE(edit([&](auto& im) {
		        auto a = entryAt(im, 0);
		        a.offset += 4;
		        setEntry(im, 0, a);
	        }) == DiagCode::L006);
	// bounds
	REQUIRE(edit([&](auto& im) {
		        auto a = entryAt(im, 0);
		        a.count = 1u << 20;
		        setEntry(im, 0, a);
	        }) == DiagCode::L006);
	REQUIRE(edit([&](auto& im) {
		        auto a = entryAt(im, 0);
		        a.offset = 1ull << 40;
		        setEntry(im, 0, a);
	        }) == DiagCode::L006);
	// overlap: the second section points into the first
	REQUIRE(edit([&](auto& im) {
		        auto a = entryAt(im, 0);
		        auto b = entryAt(im, 1);
		        b.offset = a.offset;
		        setEntry(im, 1, b);
	        }) == DiagCode::L006);
	// into the table itself
	REQUIRE(edit([&](auto& im) {
		        auto a = entryAt(im, 0);
		        a.offset = 48;
		        setEntry(im, 0, a);
	        }) == DiagCode::L006);
	// a table that does not fit
	REQUIRE(edit([&](auto& im) {
		        ImageHeader h;
		        std::memcpy(&h, im.data(), sizeof h);
		        h.sectionCount = 1u << 24;
		        std::memcpy(im.data(), &h, sizeof h);
	        }) == DiagCode::L006);
}

TEST_CASE("NIR-L007 strides", "[nir][image]") {
	auto raw = RawImage::decode(imageOf(small));
	auto narrow = raw;
	narrow.section(SecInsts).stride = 8; // below the record size
	narrow.section(SecInsts).bytes.resize(narrow.section(SecInsts).count * 8);
	REQUIRE(rejection(narrow.encode()).code == DiagCode::L007);
	auto bytes = raw;
	bytes.section(SecNameBytes).stride = 2; // a byte section has stride 1
	bytes.section(SecNameBytes).bytes.resize(bytes.section(SecNameBytes).count * 2);
	REQUIRE(rejection(bytes.encode()).code == DiagCode::L007);
	auto u32 = raw;
	u32.section(SecExtra).stride = 8;
	u32.section(SecExtra).bytes.resize(u32.section(SecExtra).count * 8);
	REQUIRE(rejection(u32.encode()).code == DiagCode::L007);
}

TEST_CASE("NIR-L008 limits", "[nir][image]") {
	auto image = imageOf(small);
	LoadOptions o;
	o.limits.totalSize = 64;
	REQUIRE(rejection(image, o).code == DiagCode::L008);
	o = {};
	o.limits.functions = 0;
	REQUIRE(rejection(image, o).code == DiagCode::L008);
	o = {};
	o.limits.instsPerFunction = 2;
	REQUIRE(rejection(image, o).code == DiagCode::L008);
	o = {};
	o.limits.blocksPerFunction = 0;
	REQUIRE(rejection(image, o).code == DiagCode::L008);
	o = {};
	o.limits.extraWords = 1;
	REQUIRE(rejection(image, o).code == DiagCode::L008);
	o = {};
	o.limits.tableEntries = 0;
	REQUIRE(rejection(image, o).code == DiagCode::L008);
	o = {};
	o.limits.dataBytes = 0;
	o.ignoreProcessToken = true;
	REQUIRE(loadImage(image, o).ok()); // the image has no data
}

TEST_CASE("NIR-L009 module section", "[nir][image]") {
	auto raw = RawImage::decode(imageOf(small));
	raw.section(SecModule).count = 0;
	raw.section(SecModule).bytes.clear();
	REQUIRE(rejection(raw.encode()).code == DiagCode::L009);
	auto none = RawImage::decode(imageOf(small));
	none.sections.erase(
	    std::remove_if(none.sections.begin(), none.sections.end(), [](auto& s) { return s.kind == SecModule; }),
	    none.sections.end());
	REQUIRE(rejection(none.encode()).code == DiagCode::L009);
}

TEST_CASE("NIR-L014 process token", "[nir][image]") {
	auto image = imageOf(small);
	// the module is process-local (its pointer is mode 1 here, but the text parser has made it portable, so
	// build a process-local one)
	auto m = parseOk(R"(nir 1.0
pointer #p0 = address local 0x1000 raw
func export @f : () -> ptr {
^b0:
  %0 = ptr.const #p0
  ret %0
}
)");
	auto local = writeImage(canonicalize(m));
	REQUIRE(loadImage(local).ok()); // this process
	LoadOptions o;
	o.processToken = m.module.processToken + 1; // another process
	REQUIRE(rejection(local, o).code == DiagCode::L014);
	o = {};
	o.ignoreProcessToken = true;
	REQUIRE(loadImage(local, o).ok());
	(void) image;
}

TEST_CASE("NIR-L016 and NIR-L017", "[nir][image]") {
	Module m;
	test::buildPreSSA(m);
	REQUIRE(rejection(writeImage(m)).code == DiagCode::L016);
	// a module the verifier rejects
	auto bad = canonicalize(parseOk(small));
	bad.insts[2].a = 77; // the add uses a value out of range
	auto err = rejection(writeImage(bad), [] {
		LoadOptions o;
		o.ignoreProcessToken = true;
		return o;
	}());
	REQUIRE(err.code == DiagCode::L017);
	REQUIRE(err.verifierDiagnostics[0].code == DiagCode::V005);
	LoadOptions skip;
	skip.verify = false;
	skip.ignoreProcessToken = true;
	REQUIRE(loadImage(writeImage(bad), skip).ok());
}

TEST_CASE("unknown and vendor sections are skipped, wider strides are compacted", "[nir][image]") {
	auto base = imageOf(small);
	LoadOptions lo;
	lo.ignoreProcessToken = true;
	auto raw = RawImage::decode(base);
	raw.sections.push_back({1024, 4, 3, std::vector<uint8_t>(12, 0xAB)});
	raw.sections.push_back({1u << 20, 1, 5, std::vector<uint8_t>(5, 0xCD)});
	REQUIRE(loadImage(raw.encode(), lo).ok());

	// widen `names` from 8 to 16 bytes per record, as a later minor version might
	auto wide = RawImage::decode(base);
	auto& names = wide.section(SecNames);
	std::vector<uint8_t> widened;
	for (uint64_t i = 0; i < names.count; ++i) {
		widened.insert(widened.end(), names.bytes.begin() + i * 8, names.bytes.begin() + i * 8 + 8);
		widened.insert(widened.end(), 8, 0x7F);
	}
	names.stride = 16;
	names.bytes = widened;
	auto loaded = loadImage(wide.encode(), lo);
	INFO(test::why(loaded));
	REQUIRE(loaded.ok());
	REQUIRE(sectionsEqual(canonicalize(parseOk(small)), *loaded.module));
}

TEST_CASE("an image with garbage in a record never crashes the loader", "[nir][image]") {
	auto base = imageOf(small);
	LoadOptions lo;
	lo.ignoreProcessToken = true;
	// flip every byte of the section data in turn (fixing the checksum): the loader must answer, not crash
	size_t start = 48 + 24 * 19;
	for (size_t i = start; i < base.size(); ++i) {
		for (uint8_t mask : {0x01, 0x80, 0xFF}) {
			auto image = base;
			image[i] ^= mask;
			test::fixChecksum(image);
			auto r = loadImage(image, lo);
			REQUIRE((r.ok() != r.error.has_value()));
		}
	}
}

TEST_CASE("inspect describes an image without materializing it", "[nir][image]") {
	auto image = imageOf(small);
	auto text = inspectImage(image);
	REQUIRE(text.find("NIR image, magic ok") != std::string::npos);
	REQUIRE(text.find("version 1.0") != std::string::npos);
	REQUIRE(text.find("incompat features: none") != std::string::npos);
	REQUIRE(text.find("(ok)") != std::string::npos);
	REQUIRE(text.find("kind 6 functions: stride 40, offset") != std::string::npos);
	REQUIRE(text.find("count 1") != std::string::npos);
	auto broken = image;
	broken[image.size() - 9] ^= 1;
	REQUIRE(inspectImage(broken).find("MISMATCH") != std::string::npos);
	REQUIRE(inspectImage({}).find("not an image") != std::string::npos);
	auto dump = inspectImage(image, [] {
		InspectOptions o;
		o.hexDump = true;
		return o;
	}());
	REQUIRE(dump.find("00000000  4e 49 52 00") != std::string::npos);
	REQUIRE(dump.find("; magic") != std::string::npos);
	REQUIRE(dump.find("functions (1 x 40 bytes)") != std::string::npos);
}

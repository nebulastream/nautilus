// Golden images: conformance material for the binary format (13-nir-spec.md §11, plan M1.5). The files under
// nautilus/test/data/nir-conformance/images are checked in. To regenerate them after an intentional change of the
// format (a new minor version), run this binary with NIR_UPDATE_GOLDEN=1 and review the diff.
#include "ImageTestUtil.hpp"
#include "TestModules.hpp"
#include "nautilus/nir/Canonicalize.hpp"
#include "nautilus/nir/Hash.hpp"
#include "nautilus/nir/Inspect.hpp"
#include "nautilus/nir/Parser.hpp"
#include "nautilus/nir/Printer.hpp"
#include <algorithm>
#include <catch2/catch_all.hpp>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <sstream>

using namespace nautilus::nir;
using test::RawImage;
namespace fs = std::filesystem;

namespace {

fs::path imagesDir() {
	return fs::path(NIR_CONFORMANCE_DIR) / "images";
}

std::string slurp(const fs::path& p) {
	std::ifstream in(p, std::ios::binary);
	std::stringstream ss;
	ss << in.rdbuf();
	return ss.str();
}

std::vector<uint8_t> slurpBytes(const fs::path& p) {
	auto s = slurp(p);
	return {s.begin(), s.end()};
}

void spit(const fs::path& p, const std::string& s) {
	fs::create_directories(p.parent_path());
	std::ofstream out(p, std::ios::binary);
	out.write(s.data(), static_cast<std::streamsize>(s.size()));
}

void spitBytes(const fs::path& p, const std::vector<uint8_t>& b) {
	spit(p, std::string(b.begin(), b.end()));
}

bool update() {
	return std::getenv("NIR_UPDATE_GOLDEN") != nullptr;
}

std::vector<fs::path> corpusTexts() {
	std::vector<fs::path> files;
	for (auto& e : fs::directory_iterator(fs::path(NIR_CONFORMANCE_DIR) / "text")) {
		if (e.path().extension() == ".nir") {
			files.push_back(e.path());
		}
	}
	std::sort(files.begin(), files.end());
	return files;
}

Module parseFile(const fs::path& p) {
	auto r = parse(slurp(p));
	REQUIRE(r.ok());
	return std::move(*r.module);
}

bool preSSA(const Module& m) {
	return std::any_of(m.functions.begin(), m.functions.end(), [](auto& f) { return f.level == 0; });
}

/// The valid golden images: every corpus text that is not process-local (a process-local image carries the token of the
/// process that wrote it, so it cannot be a fixed file).
std::vector<std::pair<std::string, std::vector<uint8_t>>> validImages(std::vector<std::string>* hashes = nullptr) {
	std::vector<std::pair<std::string, std::vector<uint8_t>>> out;
	for (auto& f : corpusTexts()) {
		auto m = parseFile(f);
		if (m.module.processToken != 0) {
			continue;
		}
		std::string hash = "-";
		std::vector<uint8_t> image;
		if (preSSA(m)) {
			image = writeImage(m); // pre-SSA modules are serialized as they are, never hashed
		} else {
			image = writeImage(canonicalize(m));
			hash = structuralHash(m, nautilus::effectiveOptions({})).hex();
		}
		out.push_back({f.stem().string() + ".bin", std::move(image)});
		if (hashes) {
			hashes->push_back(hash);
		}
	}
	return out;
}

constexpr uint64_t foreignToken = 0x1122334455667788ull;

/// The invalid golden images with the NIR-L code each must be rejected with.
std::vector<std::tuple<std::string, std::string, std::vector<uint8_t>>> invalidImages() {
	auto base = [] {
		auto m = parse(R"(nir 1.0
func export @f : (i64) -> i64 {
^b0(%0: i64):
  %1 = add %0, 1:i64
  ret %1
}
)");
		return writeImage(canonicalize(*m.module));
	};
	auto raw = [&] {
		return RawImage::decode(base());
	};
	std::vector<std::tuple<std::string, std::string, std::vector<uint8_t>>> out;
	auto add = [&](const char* name, const char* code, std::vector<uint8_t> image) {
		out.push_back({name, code, std::move(image)});
	};
	{
		auto b = base();
		b[0] = 'X';
		add("l001_magic", "NIR-L001", b);
	}
	{
		auto r = raw();
		r.header.major = 2;
		add("l002_major", "NIR-L002", r.encode());
	}
	{
		auto r = raw();
		r.header.incompatFeatures |= 0x10;
		add("l003_incompat_bit", "NIR-L003", r.encode());
	}
	{
		auto b = base();
		b.resize(b.size() - 8);
		add("l004_total_size", "NIR-L004", b);
	}
	{
		auto b = base();
		b[b.size() - 9] ^= 0x40;
		add("l005_checksum", "NIR-L005", b);
	}
	{
		auto r = raw();
		std::swap(r.sections[3], r.sections[4]);
		add("l006_order", "NIR-L006", r.encode());
	}
	{
		auto b = base();
		SectionEntry e;
		std::memcpy(&e, b.data() + 48, sizeof e);
		e.offset += 4;
		std::memcpy(b.data() + 48, &e, sizeof e);
		test::fixChecksum(b);
		add("l006_alignment", "NIR-L006", b);
	}
	{
		auto b = base();
		SectionEntry e;
		std::memcpy(&e, b.data() + 48, sizeof e);
		e.count = 1u << 20;
		std::memcpy(b.data() + 48, &e, sizeof e);
		test::fixChecksum(b);
		add("l006_bounds", "NIR-L006", b);
	}
	{
		auto b = base();
		SectionEntry a, c;
		std::memcpy(&a, b.data() + 48, sizeof a);
		std::memcpy(&c, b.data() + 48 + 24, sizeof c);
		c.offset = a.offset;
		std::memcpy(b.data() + 48 + 24, &c, sizeof c);
		test::fixChecksum(b);
		add("l006_overlap", "NIR-L006", b);
	}
	{
		auto r = raw();
		r.section(SecInsts).stride = 8;
		r.section(SecInsts).bytes.resize(r.section(SecInsts).count * 8);
		add("l007_stride_below_record_size", "NIR-L007", r.encode());
	}
	{
		auto r = raw();
		r.section(SecNameBytes).stride = 2;
		r.section(SecNameBytes).bytes.resize(r.section(SecNameBytes).count * 2);
		add("l007_byte_section_stride", "NIR-L007", r.encode());
	}
	{
		auto r = raw();
		r.section(SecModule).count = 0;
		r.section(SecModule).bytes.clear();
		add("l009_no_module", "NIR-L009", r.encode());
	}
	{
		auto r = raw();
		r.header.incompatFeatures |= FEATURE_PROCESS_LOCAL;
		ModuleRecord rec {NONE, 0, foreignToken};
		std::memcpy(r.section(SecModule).bytes.data(), &rec, sizeof rec);
		add("l014_foreign_process_token", "NIR-L014", r.encode());
	}
	{
		Module m;
		test::buildPreSSA(m);
		add("l016_pre_ssa", "NIR-L016", writeImage(m));
	}
	{
		auto m = canonicalize(*parse(R"(nir 1.0
func export @f : (i64) -> i64 {
^b0(%0: i64):
  %1 = add %0, 1:i64
  ret %1
}
)")
		                           .module);
		m.insts[1].a = 77; // a value reference out of range
		add("l017_verifier_rejects", "NIR-L017", writeImage(m));
	}
	return out;
}

/// The annotated hex dump of the second example of §9, canonicalized, with its structural hash.
std::string exampleDump() {
	auto r = parse(R"(nir 1.0
module "nes.q1"
pointer #p0 = callee symbol "process"
pointer #p1 = thunk symbol "process$thunk"
pointer #p2 = destructor symbol "Buffer::~Buffer"
callee #c0 = external #p0 : (ptr) -> i64
callee #c1 = external #p2 : (ptr) -> void attrs(noUnwind)

func export @run : (ptr) -> i64 {
  alloca 16 align 8
^b0(%0: ptr):
  %1 = alloca 0
  %2 = call #c0(%1) unwind ^b1 thunk #p1
  call #c1(%1)
  ret %2
^b1 pad:
  call #c1(%1)
  resume
}
)");
	REQUIRE(r.ok());
	auto image = writeImage(canonicalize(*r.module));
	InspectOptions o;
	o.hexDump = true;
	return "# structural hash " + structuralHash(*r.module, nautilus::effectiveOptions({})).hex() + "\n" +
	       inspectImage(image, o);
}

LoadOptions forGolden() {
	LoadOptions o;
	o.allowPreSSA = true;
	o.ignoreProcessToken = true;
	return o;
}

} // namespace

TEST_CASE("golden images are up to date (NIR_UPDATE_GOLDEN=1 regenerates them)", "[nir][image][conformance]") {
	std::vector<std::string> hashes;
	auto valid = validImages(&hashes);
	auto invalid = invalidImages();
	if (update()) {
		fs::remove_all(imagesDir());
		std::string list;
		for (size_t i = 0; i < valid.size(); ++i) {
			spitBytes(imagesDir() / "valid" / valid[i].first, valid[i].second);
			list += valid[i].first + " " + hashes[i] + "\n";
		}
		spit(imagesDir() / "valid.txt", list);
		std::string bad;
		for (auto& [name, code, image] : invalid) {
			spitBytes(imagesDir() / "invalid" / (name + ".bin"), image);
			bad += name + ".bin " + code + "\n";
		}
		spit(imagesDir() / "invalid.txt", bad);
		spit(imagesDir() / "example2.hexdump.txt", exampleDump());
	}
	std::string list = slurp(imagesDir() / "valid.txt");
	std::string expected;
	for (size_t i = 0; i < valid.size(); ++i) {
		expected += valid[i].first + " " + hashes[i] + "\n";
		INFO(valid[i].first);
		REQUIRE(slurpBytes(imagesDir() / "valid" / valid[i].first) == valid[i].second);
	}
	REQUIRE(list == expected);
	std::string bad;
	for (auto& [name, code, image] : invalid) {
		bad += name + ".bin " + code + "\n";
		INFO(name);
		REQUIRE(slurpBytes(imagesDir() / "invalid" / (name + ".bin")) == image);
	}
	REQUIRE(slurp(imagesDir() / "invalid.txt") == bad);
	// no stale files
	size_t files = 0;
	for (auto& e : fs::directory_iterator(imagesDir() / "valid")) {
		(void) e;
		files++;
	}
	REQUIRE(files == valid.size());
	files = 0;
	for (auto& e : fs::directory_iterator(imagesDir() / "invalid")) {
		(void) e;
		files++;
	}
	REQUIRE(files == invalid.size());
}

TEST_CASE("every valid golden image loads, re-serializes identically and matches its text",
          "[nir][image][conformance]") {
	std::istringstream list(slurp(imagesDir() / "valid.txt"));
	std::string file, hash;
	int n = 0;
	while (list >> file >> hash) {
		INFO(file);
		auto image = slurpBytes(imagesDir() / "valid" / file);
		auto loaded = loadImage(image, forGolden());
		INFO(test::why(loaded));
		REQUIRE(loaded.ok());
		REQUIRE(writeImage(*loaded.module) == image);
		auto text = parseFile(fs::path(NIR_CONFORMANCE_DIR) / "text" / (fs::path(file).stem().string() + ".nir"));
		if (hash == "-") {
			REQUIRE(print(*loaded.module) == print(text));
		} else {
			// the image holds the canonical module, and the text's structural hash is the pinned one
			REQUIRE(print(*loaded.module) == print(canonicalize(text)));
			REQUIRE(structuralHash(*loaded.module, nautilus::effectiveOptions({})).hex() == hash);
			REQUIRE(structuralHash(text, nautilus::effectiveOptions({})).hex() == hash);
		}
		n++;
	}
	REQUIRE(n >= 8);
}

TEST_CASE("every invalid golden image is rejected with its code", "[nir][image][conformance]") {
	std::istringstream list(slurp(imagesDir() / "invalid.txt"));
	std::string file, code;
	std::set<std::string> codes;
	while (list >> file >> code) {
		INFO(file);
		auto r = loadImage(slurpBytes(imagesDir() / "invalid" / file));
		REQUIRE_FALSE(r.ok());
		REQUIRE(std::string(codeName(r.error->code)) == code);
		codes.insert(code);
	}
	// the loader codes of 13 §6.6 that need no registry or resolver each have an image; L008 (limits) is a
	// configuration and is tested with options, L010-L012 and L015 need the registry and the pointer resolver
	for (auto* c : {"NIR-L001", "NIR-L002", "NIR-L003", "NIR-L004", "NIR-L005", "NIR-L006", "NIR-L007", "NIR-L009",
	                "NIR-L014", "NIR-L016", "NIR-L017"}) {
		INFO(c);
		REQUIRE(codes.count(c) == 1);
	}
}

TEST_CASE("the annotated hex dump of the second example matches byte for byte", "[nir][image][conformance]") {
	if (update()) {
		spit(imagesDir() / "example2.hexdump.txt", exampleDump());
	}
	REQUIRE(slurp(imagesDir() / "example2.hexdump.txt") == exampleDump());
}

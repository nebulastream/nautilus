// The target of the NIR image and text fuzzers (M1.6, docs/plans/nautilus-0.4/16-implementation-plan.md).
//
// The first byte of an input selects the reader, the rest is its input:
//   bit 0 clear: a binary image -> loadImage, then whatever a loaded module is used for
//   bit 0 set:   NIR text       -> parse, then print / parse round trips
//   bit 1 set (image only): totalSize and the checksum are repaired before loading, so that mutations get past the
//   two checks that reject almost every damaged image and reach the section table, the records and the verifier
// A reader must answer every input, accepting or rejecting it, without undefined behavior (sanitizers watch for it)
// and without breaking an invariant of the format; an invariant violation aborts.
#pragma once

#include "nautilus/nir/Hash.hpp"
#include "nautilus/nir/ImageLoader.hpp"
#include "nautilus/nir/Parser.hpp"
#include "nautilus/nir/Printer.hpp"
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string_view>
#include <utility>
#include <vector>

namespace nautilus::nir::fuzz {

/// The input being run, so that a violation can save it (libFuzzer saves its own crashing inputs).
inline std::pair<const uint8_t*, size_t>& currentInput() {
	static std::pair<const uint8_t*, size_t> input {nullptr, 0};
	return input;
}

[[noreturn]] inline void violated(const char* what) {
	std::fprintf(stderr, "NIR fuzz invariant violated: %s\n", what);
	if (const char* path = std::getenv("NIR_FUZZ_ARTIFACT")) {
		if (FILE* f = std::fopen(path, "wb")) {
			std::fwrite(currentInput().first, 1, currentInput().second, f);
			std::fclose(f);
			std::fprintf(stderr, "the input is saved in %s\n", path);
		}
	}
	std::abort();
}

inline bool hasPreSSA(const Module& m) {
	for (auto& f : m.functions) {
		if (f.level == static_cast<uint8_t>(Level::PreSSA)) {
			return true;
		}
	}
	return false;
}

/// What a verified module must satisfy.
inline void checkModule(const Module& m) {
	auto text = print(m);
	// print -> parse -> print is the identity on the printer's own output. Extension types and operations are the
	// business of M1.3a (they print with placeholders that the parser does not read yet), so a module that has any is
	// not checked this way.
	if (text.find('!') == std::string::npos) {
		auto reparsed = parse(text);
		if (reparsed.error) {
			std::fprintf(stderr, "%s\n%s\n", reparsed.error->str().c_str(), text.c_str());
			violated("the printer's output does not parse");
		}
		if (!reparsed.verifierDiagnostics.empty()) {
			violated("the printer's output does not verify");
		}
		if (print(*reparsed.module) != text) {
			violated("print(parse(print(m))) differs from print(m)");
		}
	}
	// write -> load gives the module back. A module with imports is not a loadable image until bundling exists
	// (13 §8.5): an image has no imports (NIR-V038), though the hash input of such a module is written.
	LoadOptions lo;
	lo.ignoreProcessToken = true;
	lo.allowPreSSA = true;
	if (m.imports.empty()) {
		auto image = writeImage(m);
		auto loaded = loadImage(image, lo);
		if (!loaded.ok()) {
			violated("a verified module does not load from its own image");
		}
		if (print(*loaded.module) != text) {
			violated("the image does not load to the module it was written from");
		}
	}
	if (hasPreSSA(m)) {
		return;
	}
	auto canonical = canonicalize(m);
	if (!verify(canonical).empty()) {
		violated("a canonical module does not verify");
	}
	if (!sectionsEqual(canonical, canonicalize(canonical))) {
		violated("canonicalization is not idempotent");
	}
	if (canonical.imports.empty()) {
		auto canonicalImage = writeImage(canonical);
		auto again = loadImage(canonicalImage, lo);
		if (!again.ok() || writeImage(*again.module) != canonicalImage) {
			violated("a canonical image is not stable under load and write");
		}
	}
	(void) structuralHash(m, effectiveOptions({}));
}

inline void runInput(const uint8_t* data, size_t size) {
	if (size == 0) {
		return;
	}
	currentInput() = {data, size};
	uint8_t flags = data[0];
	bool text = (flags & 1) != 0;
	data++;
	size--;
	if (text) {
		auto result = parse(std::string_view(reinterpret_cast<const char*>(data), size));
		if (result.module && !result.error && result.verifierDiagnostics.empty()) {
			checkModule(*result.module);
		}
	} else {
		LoadOptions lo;
		lo.ignoreProcessToken = true;
		lo.allowPreSSA = true;
		std::vector<uint8_t> repaired;
		std::span<const uint8_t> image(data, size);
		if ((flags & 2) != 0 && size >= sizeof(ImageHeader)) {
			repaired.assign(data, data + size);
			ImageHeader h;
			std::memcpy(&h, repaired.data(), sizeof h);
			h.totalSize = size;
			h.crc32c = crc32c(image.subspan(sizeof(ImageHeader)));
			std::memcpy(repaired.data(), &h, sizeof h);
			image = repaired;
		}
		auto result = loadImage(image, lo);
		if (result.ok()) {
			checkModule(*result.module);
		} else if (!result.error) {
			violated("a load neither succeeded nor failed");
		}
	}
}

} // namespace nautilus::nir::fuzz

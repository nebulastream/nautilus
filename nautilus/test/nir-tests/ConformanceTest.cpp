#include "nautilus/nir/Parser.hpp"
#include "nautilus/nir/Printer.hpp"
#include <algorithm>
#include <catch2/catch_all.hpp>
#include <filesystem>
#include <fstream>
#include <set>
#include <sstream>

using namespace nautilus::nir;
namespace fs = std::filesystem;

namespace {
std::string slurp(const fs::path& p) {
	std::ifstream in(p, std::ios::binary);
	std::stringstream ss;
	ss << in.rdbuf();
	return ss.str();
}

std::vector<fs::path> corpus(const char* sub) {
	std::vector<fs::path> files;
	for (auto& e : fs::directory_iterator(fs::path(NIR_CONFORMANCE_DIR) / sub)) {
		if (e.path().extension() == ".nir") {
			files.push_back(e.path());
		}
	}
	std::sort(files.begin(), files.end());
	return files;
}
} // namespace

TEST_CASE("every positive corpus text is canonical and round-trips", "[nir][conformance]") {
	auto files = corpus("text");
	REQUIRE(files.size() >= 10);
	for (auto& f : files) {
		DYNAMIC_SECTION(f.filename().string()) {
			auto text = slurp(f);
			auto r = parse(text);
			std::string all;
			if (r.error) {
				all += r.error->str() + "\n";
			}
			for (auto& d : r.verifierDiagnostics) {
				all += d.str() + "\n";
			}
			INFO(all);
			REQUIRE(r.ok());
			REQUIRE(print(*r.module) == text);
			// the printed text of the parsed module parses to the same text again
			auto again = parse(print(*r.module));
			REQUIRE(again.ok());
			REQUIRE(print(*again.module) == text);
		}
	}
}

TEST_CASE("every invalid corpus text is rejected with its code", "[nir][conformance]") {
	// the parser's NIR-P codes (13 §6.6) each have a negative text (sections rerun the test case, so scan up front)
	std::set<std::string> seen;
	for (auto& f : corpus("text/invalid")) {
		auto text = slurp(f);
		seen.insert(text.substr(text.find("NIR-"), 8));
	}
	for (auto* code : {"NIR-P001", "NIR-P002", "NIR-P003", "NIR-P004", "NIR-P006"}) {
		INFO(code);
		REQUIRE(seen.count(code) == 1);
	}
	for (auto& f : corpus("text/invalid")) {
		DYNAMIC_SECTION(f.filename().string()) {
			auto text = slurp(f);
			auto pos = text.find("NIR-");
			REQUIRE(pos != std::string::npos);
			std::string code = text.substr(pos, 8);
			auto r = parse(text);
			std::string got;
			if (r.error) {
				got = std::string(codeName(r.error->code));
			} else if (!r.verifierDiagnostics.empty()) {
				for (auto& d : r.verifierDiagnostics) {
					if (std::string(codeName(d.code)) == code) {
						got = code;
					}
				}
				if (got.empty()) {
					got = std::string(codeName(r.verifierDiagnostics[0].code));
				}
			}
			INFO(f.string());
			REQUIRE(got == code);
		}
	}
}

TEST_CASE("the positive corpus uses every core opcode", "[nir][conformance]") {
	std::string all;
	for (auto& f : corpus("text")) {
		all += slurp(f);
	}
#define NIR_OP(OPNAME, CODE, TEXT, KA, KB, KC, FL)                                                                     \
	{                                                                                                                  \
		INFO(TEXT);                                                                                                    \
		/* blockarg is spelled in the block header, not as an instruction */                                           \
		REQUIRE((std::string(TEXT) == "blockarg" || all.find(std::string(TEXT)) != std::string::npos));                \
	}
#include "nautilus/nir/Opcodes.def"
#undef NIR_OP
}

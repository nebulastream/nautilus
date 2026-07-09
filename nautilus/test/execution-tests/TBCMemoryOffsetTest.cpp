#include "ExecutionTest.hpp"
#include "nautilus/Engine.hpp"
#include "nautilus/val.hpp"
#include "nautilus/val_ptr.hpp"
#include <algorithm>
#include <catch2/catch_all.hpp>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <set>
#include <sstream>

// Covers two tbc backend follow-ups (see
// src/nautilus/compiler/backends/tbc/README.md, "Follow-ups"): dead-MOV
// elimination after immediate folding, and LOAD_off/STORE_off memory-offset
// superinstructions that fuse a single-use `ptr + const` into the
// load/store. Both are exercised for correctness across dispatch/lowering
// variants, and their effect on the emitted instruction stream is checked
// directly via the after_tbc_generation dump.
#ifdef ENABLE_TBC_BACKEND

namespace nautilus::engine {

namespace {

struct Record {
	int64_t a;
	int64_t b;
	int64_t c;
};

// Reads three fields at offsets 0, 8, 16. The address for `b` and `c` is a
// single-use `ptr + const` immediately feeding a LOAD — fusable into
// LOAD_off. The constant offsets are also small enough to exercise dead-MOV
// elimination once folded into the preceding ADD.
val<int64_t> tbcOffsetLoadSum(val<Record*> p) {
	val<int64_t> a = p.get(&Record::a);
	val<int64_t> b = p.get(&Record::b);
	val<int64_t> c = p.get(&Record::c);
	return a + b + c;
}

// Writes three fields at offsets 0, 8, 16 — exercises STORE_off.
void tbcOffsetStoreFields(val<Record*> p, val<int64_t> a, val<int64_t> b, val<int64_t> c) {
	p.set(&Record::a, a);
	p.set(&Record::b, b);
	p.set(&Record::c, c);
}

// The offset address is used by two separate loads: the add must survive as
// a plain ADD/ADD_imm feeding both, rather than being incorrectly fused into
// just one of them (which would leave the other reading a stale/reused
// register once the add is dropped).
val<int64_t> tbcOffsetAddressReused(val<int64_t*> p) {
	auto advanced = p + 1;
	val<int64_t> first = *advanced;
	val<int64_t> second = *advanced;
	return first + second * 10;
}

engine::NautilusEngine tbcEngine(const std::string& dispatch, bool superinstructions, bool immediates) {
	engine::Options options;
	options.setOption("engine.backend", std::string("tbc"));
	options.setOption("tbc.dispatch", dispatch);
	options.setOption("tbc.superinstructions", superinstructions);
	options.setOption("tbc.immediates", immediates);
	return engine::NautilusEngine(options);
}

std::string readFile(const std::filesystem::path& path) {
	std::ifstream in(path);
	std::stringstream ss;
	ss << in.rdbuf();
	return ss.str();
}

// DumpHandler writes under $TMPDIR/dump/<compilation-unit-id>/ with no way
// to override the location, so snapshot the existing subdirs, run the
// compilation, then read back the one freshly-created after_tbc_generation
// dump (mirrors DebugInfoExecutionTest.cpp's approach for MLIR IR dumps).
std::string dumpTbcCode(bool superinstructions, bool immediates) {
	const auto dumpRoot = std::filesystem::temp_directory_path() / "dump";
	std::set<std::filesystem::path> existing;
	if (std::filesystem::exists(dumpRoot)) {
		for (const auto& e : std::filesystem::directory_iterator(dumpRoot)) {
			existing.insert(e.path());
		}
	}

	engine::Options options;
	options.setOption("engine.backend", std::string("tbc"));
	options.setOption("tbc.superinstructions", superinstructions);
	options.setOption("tbc.immediates", immediates);
	options.setOption("dump.after_tbc_generation", true);
	NautilusEngine engine(options);
	auto fn = engine.registerFunction(tbcOffsetLoadSum);
	Record record {1, 2, 3};
	REQUIRE(fn(&record) == 6);

	std::string content;
	if (std::filesystem::exists(dumpRoot)) {
		for (const auto& dir : std::filesystem::directory_iterator(dumpRoot)) {
			if (existing.count(dir.path()) > 0) {
				continue;
			}
			for (const auto& entry : std::filesystem::recursive_directory_iterator(dir)) {
				if (entry.is_regular_file() && entry.path().extension() == ".tbc") {
					content += readFile(entry.path());
				}
			}
		}
	}
	return content;
}

} // namespace

TEST_CASE("TBC memory-offset superinstructions produce correct results") {
	auto reference = tbcEngine("switch", false, false);
	const std::vector<std::tuple<std::string, bool, bool>> variants = {
	    {"switch", true, true}, {"goto", true, true}, {"goto", false, true},
	    {"tailcall", true, true}, {"tailcall", true, false}, {"auto", true, true},
	};
	for (const auto& [dispatch, super, imm] : variants) {
		DYNAMIC_SECTION(dispatch << "_super" << super << "_imm" << imm) {
			auto alt = tbcEngine(dispatch, super, imm);

			auto cLoad = reference.registerFunction(tbcOffsetLoadSum);
			auto aLoad = alt.registerFunction(tbcOffsetLoadSum);
			auto cStore = reference.registerFunction(tbcOffsetStoreFields);
			auto aStore = alt.registerFunction(tbcOffsetStoreFields);
			auto cReused = reference.registerFunction(tbcOffsetAddressReused);
			auto aReused = alt.registerFunction(tbcOffsetAddressReused);

			for (int64_t base = -5; base <= 5; ++base) {
				Record refRecord {base, base * 2, base * 3};
				Record altRecord {base, base * 2, base * 3};
				REQUIRE(aLoad(&altRecord) == cLoad(&refRecord));

				cStore(&refRecord, base + 10, base + 20, base + 30);
				aStore(&altRecord, base + 10, base + 20, base + 30);
				REQUIRE(altRecord.a == refRecord.a);
				REQUIRE(altRecord.b == refRecord.b);
				REQUIRE(altRecord.c == refRecord.c);

				int64_t data[] = {base, base + 1, base + 2};
				int64_t altData[] = {base, base + 1, base + 2};
				REQUIRE(aReused(altData) == cReused(data));
			}
		}
	}
}

TEST_CASE("TBC memory-offset fusion and dead-MOV elimination fire when enabled") {
	const auto optimized = dumpTbcCode(/*superinstructions=*/true, /*immediates=*/true);
	const auto unoptimized = dumpTbcCode(/*superinstructions=*/false, /*immediates=*/false);

	REQUIRE_FALSE(optimized.empty());
	REQUIRE_FALSE(unoptimized.empty());

	// The fused address computation for the two non-zero-offset fields
	// (b, c) must show up as LOAD_off, and must not appear at all when the
	// optimizations are disabled.
	CHECK(optimized.find("LOAD_off_i64") != std::string::npos);
	CHECK(unoptimized.find("LOAD_off_i64") == std::string::npos);

	// Fusing away the two pointer adds (and immediate folding + dead-MOV
	// elimination collapsing their constant materialization) must strictly
	// shrink the instruction count relative to the unoptimized baseline.
	auto countLines = [](const std::string& text) {
		return static_cast<std::size_t>(std::count(text.begin(), text.end(), '\n'));
	};
	CHECK(countLines(optimized) < countLines(unoptimized));
}

} // namespace nautilus::engine

#endif // ENABLE_TBC_BACKEND

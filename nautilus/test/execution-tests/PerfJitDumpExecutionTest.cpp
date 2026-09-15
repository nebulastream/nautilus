#include "nautilus/config.hpp"

#if defined(ENABLE_TRACING) && defined(ENABLE_MLIR_BACKEND) && defined(__linux__)

#include "ExecutionTest.hpp"
#include "PerfFixtureFunctions.hpp"
#include "nautilus/Engine.hpp"
#include <algorithm>
#include <catch2/catch_all.hpp>
#include <cstdint>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <optional>
#include <set>
#include <string>
#include <string_view>
#include <unistd.h>
#include <vector>

namespace nautilus::engine {

namespace {

// On-disk perf jitdump structures (the Linux perf "jitdump" format read by
// `perf inject --jit`). LLVM ships the wire format for the RPC that
// *produces* these records (llvm/ExecutionEngine/Orc/Shared/PerfSharedStructs.h)
// but not the on-disk POD layout itself, so this mirrors LLVM's writer
// (llvm/lib/ExecutionEngine/Orc/TargetProcess/JITLoaderPerf.cpp) directly.
// All fields are naturally aligned (no reordering needed to avoid padding),
// so this layout is stable across compilers.
constexpr uint32_t kJitDumpMagic = (uint32_t('J') << 24) | (uint32_t('i') << 16) | (uint32_t('T') << 8) | uint32_t('D');

enum class RecordType : uint32_t {
	CodeLoad = 0,
	CodeMove = 1,
	DebugInfo = 2,
	Close = 3,
	UnwindingInfo = 4,
};

struct JitDumpHeader {
	uint32_t magic;
	uint32_t version;
	uint32_t totalSize;
	uint32_t elfMach;
	uint32_t pad1;
	uint32_t pid;
	uint64_t timestamp;
	uint64_t flags;
};
static_assert(sizeof(JitDumpHeader) == 40);

struct RecordPrefix {
	uint32_t id;
	uint32_t totalSize;
	uint64_t timestamp;
};
static_assert(sizeof(RecordPrefix) == 16);

struct CodeLoadRecordFixed {
	uint32_t pid;
	uint32_t tid;
	uint64_t vma;
	uint64_t codeAddr;
	uint64_t codeSize;
	uint64_t codeIndex;
};
static_assert(sizeof(CodeLoadRecordFixed) == 40);

struct DebugInfoRecordFixed {
	uint64_t codeAddr;
	uint64_t nrEntry;
};
static_assert(sizeof(DebugInfoRecordFixed) == 16);

struct DebugEntryFixed {
	uint64_t addr;
	uint32_t line;
	uint32_t discrim;
};
static_assert(sizeof(DebugEntryFixed) == 16);

struct CodeLoadEntry {
	std::string name;
	uint64_t codeSize = 0;
	uint64_t codeAddr = 0;
};

struct DebugLine {
	uint32_t line = 0;
	std::string file;
};

struct DebugInfoEntry {
	uint64_t codeAddr = 0;
	std::vector<DebugLine> lines;
};

// One record in file order, reduced to what the pairing rule below needs.
// `perf inject --jit` consumes a JIT_CODE_DEBUG_INFO record positionally --
// it stashes one pending line table and hands it to the *next* JIT_CODE_LOAD
// -- so the order records appear in is load-bearing, not cosmetic, and a
// parser that only collects the two kinds into separate buckets cannot see a
// mis-ordering at all.
struct RecordRef {
	RecordType type;
	// CodeLoad: the record's own code address. DebugInfo: the address of the
	// code it describes. The two must match for a pair to be correct.
	uint64_t codeAddr = 0;
	std::string name; // CodeLoad only
};

struct JitDump {
	JitDumpHeader header {};
	std::vector<CodeLoadEntry> codeLoads;
	std::vector<DebugInfoEntry> debugInfos;
	std::vector<RecordRef> records;
};

std::string readCString(std::ifstream& in) {
	std::string s;
	char c = 0;
	while (in.get(c) && c != '\0') {
		s.push_back(c);
	}
	return s;
}

std::optional<JitDump> parseJitDump(const std::filesystem::path& path) {
	std::ifstream in(path, std::ios::binary);
	if (!in) {
		return std::nullopt;
	}
	JitDump dump;
	if (!in.read(reinterpret_cast<char*>(&dump.header), sizeof(dump.header))) {
		return std::nullopt;
	}
	if (dump.header.magic != kJitDumpMagic) {
		return std::nullopt;
	}
	// The header names its own size; skip to its end rather than assume
	// sizeof(JitDumpHeader) matches a future format revision.
	in.seekg(dump.header.totalSize, std::ios::beg);

	while (in) {
		RecordPrefix prefix {};
		if (!in.read(reinterpret_cast<char*>(&prefix), sizeof(prefix))) {
			break;
		}
		const auto bodyStart = in.tellg();
		const auto recordType = static_cast<RecordType>(prefix.id);

		if (recordType == RecordType::CodeLoad) {
			CodeLoadRecordFixed fixed {};
			if (in.read(reinterpret_cast<char*>(&fixed), sizeof(fixed))) {
				CodeLoadEntry entry;
				entry.name = readCString(in);
				entry.codeSize = fixed.codeSize;
				entry.codeAddr = fixed.codeAddr;
				dump.records.push_back({RecordType::CodeLoad, fixed.codeAddr, entry.name});
				dump.codeLoads.push_back(std::move(entry));
			}
		} else if (recordType == RecordType::DebugInfo) {
			DebugInfoRecordFixed fixed {};
			if (in.read(reinterpret_cast<char*>(&fixed), sizeof(fixed))) {
				DebugInfoEntry entry;
				entry.codeAddr = fixed.codeAddr;
				dump.records.push_back({RecordType::DebugInfo, fixed.codeAddr, {}});
				for (uint64_t i = 0; i < fixed.nrEntry; ++i) {
					DebugEntryFixed die {};
					if (!in.read(reinterpret_cast<char*>(&die), sizeof(die))) {
						break;
					}
					entry.lines.push_back({die.line, readCString(in)});
				}
				dump.debugInfos.push_back(std::move(entry));
			}
		} else if (recordType == RecordType::Close) {
			break;
		}

		// TotalSize is authoritative regardless of what the branch above
		// consumed (or a record type we don't parse at all), so a mismatch
		// in one branch cannot desync the rest of the file.
		if (prefix.totalSize < sizeof(prefix)) {
			break;
		}
		in.seekg(bodyStart + std::streamoff(prefix.totalSize - sizeof(prefix)), std::ios::beg);
	}
	return dump;
}

// LLVM writes the dump to `<jitDumpDir>/.debug/jit/<unique-dir>/jit-<pid>.dump`,
// where `<unique-dir>` (`llvm-IR-jit-<date>-<random>`) cannot be predicted.
// Pointing JITDUMPDIR at a directory of the test's own and being the only
// thing that writes under it makes a plain recursive search unambiguous.
std::optional<std::filesystem::path> findJitDumpFile(const std::filesystem::path& jitDumpDir) {
	const auto jitRoot = jitDumpDir / ".debug" / "jit";
	if (!std::filesystem::exists(jitRoot)) {
		return std::nullopt;
	}
	for (const auto& uniqueDir : std::filesystem::directory_iterator(jitRoot)) {
		if (!uniqueDir.is_directory()) {
			continue;
		}
		for (const auto& entry : std::filesystem::directory_iterator(uniqueDir)) {
			const auto filename = entry.path().filename().string();
			if (filename.starts_with("jit-") && entry.path().extension() == ".dump") {
				return entry.path();
			}
		}
	}
	return std::nullopt;
}

/// Compiles `perfCompositeKernel` once (rounds=1 -- this checks structure, not
/// sampling volume) with the given option tweak, pointing JITDUMPDIR at a
/// scratch directory of this call's own.
std::optional<std::filesystem::path> compileWithJitDumpDir(const std::function<void(Options&)>& tweak) {
	const auto jitDumpDir =
	    std::filesystem::temp_directory_path() / ("nautilus_perf_jitdump_" + std::to_string(::getpid()) + "_" +
	                                              std::to_string(reinterpret_cast<uintptr_t>(&tweak)));
	std::filesystem::remove_all(jitDumpDir);
	std::filesystem::create_directories(jitDumpDir);
	setenv("JITDUMPDIR", jitDumpDir.c_str(), 1);

	Options options;
	options.setOption("engine.backend", std::string("mlir"));
	tweak(options);

	{
		NautilusEngine engine(options);
		auto fn = engine.registerFunction(perfCompositeKernel);
		std::vector<int64_t> data {1, -2, 3, -4, 5};
		REQUIRE(fn(data.data(), static_cast<int32_t>(data.size()), 1) != 0);
	}
	// PerfSupportPlugin writes (and flushes) each record as the module links,
	// so the file is complete before the engine above is even destroyed;
	// destroying it first just mirrors a real perf run, where the process
	// exits (running PerfSupportPlugin's destructor, which appends the
	// JIT_CODE_CLOSE record `perf inject` uses to treat the dump as done)
	// before `perf inject` reads the result.

	unsetenv("JITDUMPDIR");
	auto dumpPath = findJitDumpFile(jitDumpDir);
	if (dumpPath) {
		// Copy out of the directory we're about to delete.
		auto kept = std::filesystem::temp_directory_path() /
		            ("nautilus_perf_jitdump_kept_" + std::to_string(::getpid()) + ".dump");
		std::filesystem::copy_file(*dumpPath, kept, std::filesystem::copy_options::overwrite_existing);
		std::filesystem::remove_all(jitDumpDir);
		return kept;
	}
	std::filesystem::remove_all(jitDumpDir);
	return std::nullopt;
}

JitDump compilePerfJitDump(const std::function<void(Options&)>& tweak = [](Options& o) {
	o.setOption("mlir.perf.enable", true);
	o.setOption("mlir.debug.source_mode", std::string("nautilus-ir"));
}) {
	auto path = compileWithJitDumpDir(tweak);
	REQUIRE(path.has_value());
	auto parsed = parseJitDump(*path);
	std::filesystem::remove(*path);
	REQUIRE(parsed.has_value());
	return std::move(*parsed);
}

} // namespace

TEST_CASE("Perf jitdump: disabled by default writes no dump") {
	auto path = compileWithJitDumpDir([](Options&) {});
	REQUIRE_FALSE(path.has_value());
}

TEST_CASE("Perf jitdump: magic and pid identify the file") {
	auto dump = compilePerfJitDump();
	REQUIRE(dump.header.magic == kJitDumpMagic);
	REQUIRE(dump.header.pid == static_cast<uint32_t>(::getpid()));
}

TEST_CASE("Perf jitdump: JIT_CODE_LOAD records name every compiled function") {
	auto dump = compilePerfJitDump();
	REQUIRE_FALSE(dump.codeLoads.empty());

	auto hasNameContaining = [&](std::string_view needle) {
		return std::any_of(dump.codeLoads.begin(), dump.codeLoads.end(),
		                   [&](const CodeLoadEntry& e) { return e.name.find(needle) != std::string::npos; });
	};
	// The root kernel and the nested Nautilus function each lower to their
	// own func.func, and so to their own JIT_CODE_LOAD record.
	REQUIRE(hasNameContaining("execute"));
	REQUIRE(hasNameContaining("perfHelper"));
	for (const auto& entry : dump.codeLoads) {
		INFO("JIT_CODE_LOAD " << entry.name);
		REQUIRE(entry.codeSize > 0);
	}
}

TEST_CASE("Perf jitdump: JIT_CODE_DEBUG_INFO carries a multi-line line table") {
	// Guards against the trap in Finding 1a of the perf jitdump issue:
	// skipping debug-info emission under `mlir.perf.enable` would still
	// produce JIT_CODE_LOAD records, just with no JIT_CODE_DEBUG_INFO
	// alongside -- symbols with no source attribution, silently.
	auto dump = compilePerfJitDump();
	REQUIRE_FALSE(dump.debugInfos.empty());

	// DWARFContext::getLineInfoForAddressRange() includes each sequence's
	// closing end-of-range row, which conventionally carries line 0 (no
	// source position, just marking where the address range ends) even
	// though every real instruction in the range has a proper line -- so
	// line 0 is expected on some entries and only a lack of real ones would
	// indicate a problem.
	std::set<uint32_t> lines;
	bool sawAnyEntry = false;
	for (const auto& info : dump.debugInfos) {
		for (const auto& entry : info.lines) {
			sawAnyEntry = true;
			REQUIRE_FALSE(entry.file.empty());
			if (entry.line != 0) {
				lines.insert(entry.line);
			}
		}
	}
	REQUIRE(sawAnyEntry);
	// More than one distinct line: collapsing the whole module onto a single
	// synthetic location would pass a "debug info exists" check but
	// attribute every sample to the same line.
	REQUIRE(lines.size() > 1);
}

// Both tests below are tagged [!shouldfail]: they assert what a correct
// jitdump must look like, and both currently fail. Catch2 inverts the result,
// so the suite stays green while the defect stands *and* turns red the moment
// either is fixed -- at which point the tag comes off and the test becomes an
// ordinary regression guard. They are written this way because both defects
// are silent: symbols still appear in `perf report`, so nothing looks broken.

TEST_CASE("Perf jitdump: each JIT_CODE_DEBUG_INFO immediately precedes its own JIT_CODE_LOAD", "[!shouldfail]") {
	// `perf inject --jit` pairs the two record kinds *positionally*, not by
	// code_addr: it keeps a single pending line table, overwrites it with each
	// new JIT_CODE_DEBUG_INFO, and hands whatever is pending to the next
	// JIT_CODE_LOAD, then drops it. So a debug record only reaches its
	// function if it is the record immediately before that function's load.
	//
	// LLVM's PerfSupportPlugin batches a whole object into one
	// PerfJITRecordBatch of two separate vectors (DebugInfoRecords,
	// CodeLoadRecords) and JITLoaderPerf writes every debug record first, then
	// every code record. With more than one debug record in a batch -- which
	// is every Nautilus compile, since even a trivial kernel emits `execute`
	// plus its `_mlir_ciface_*` trampolines -- the pairing is wrong for all of
	// them.
	//
	// Measured on this fixture: of 8 JIT_CODE_LOADs, exactly one generated ELF
	// came out with a .debug_line section, and it was the *wrong* function's
	// line table; `execute`, the kernel that owns 42 of the line entries, got
	// none. Re-ordering the records by hand and re-running `perf inject`
	// gives every function its correct table, which is what pins the cause to
	// ordering rather than to the line tables themselves.
	auto dump = compilePerfJitDump();
	REQUIRE_FALSE(dump.debugInfos.empty());

	for (size_t i = 0; i < dump.records.size(); ++i) {
		if (dump.records[i].type != RecordType::DebugInfo) {
			continue;
		}
		INFO("JIT_CODE_DEBUG_INFO at record index " << i << " for code_addr " << dump.records[i].codeAddr);
		REQUIRE(i + 1 < dump.records.size());
		const auto& next = dump.records[i + 1];
		REQUIRE(next.type == RecordType::CodeLoad);
		// Same code range: the pending table perf hands to this load is the
		// one that describes it.
		REQUIRE(next.codeAddr == dump.records[i].codeAddr);
	}
}

TEST_CASE("Perf jitdump: region() scopes are visible to perf", "[!shouldfail]") {
	// region() lowers to a DWARF DW_TAG_inlined_subroutine (docs/region.md,
	// and "Debug info: region() scopes lower to a DWARF inlined subroutine"
	// in DebugInfoExecutionTest.cpp), which is what makes each region its own
	// frame in a GDB backtrace. None of that reaches perf.
	//
	// perf never reads the JIT-registered object that carries the DWARF.
	// `perf inject --jit` synthesizes its own ELF per code range from the
	// jitdump alone, and the jitdump format has nowhere to put a scope tree:
	// JIT_CODE_DEBUG_INFO is a flat line table ({code_addr, nr_entry} then
	// {addr, line, discrim, file} entries) with no subprogram records and no
	// inline nesting. Verified end-to-end: `readelf --debug-dump=info` on the
	// generated jitted-*.so shows a bare DW_TAG_compile_unit carrying only
	// DW_AT_stmt_list -- zero DW_TAG_subprogram, zero
	// DW_TAG_inlined_subroutine -- and the strings "outer", "hot" and "mix"
	// appear nowhere in the file. `perf report --inline` correspondingly
	// showed only `execute` (50.7%) and `perfHelper` (27.8%): every region
	// collapses into the enclosing function's box in a flame graph.
	//
	// What survives is line numbers, so the attribution is *recoverable* --
	// mapping the hot addresses back through the line table onto the Nautilus
	// IR dump's region legend put 63.9% of `execute`'s samples in "hot" --
	// but only by hand, and only while that IR dump still exists (it lands in
	// a temp directory by default). No profiler does this on its own.
	//
	// The fix has to put the region into something perf does read. The
	// cheapest is a JIT_CODE_LOAD per contiguous region range under a
	// qualified name (execute::outer::hot), which makes regions sibling
	// symbols rather than nested frames; ranges must be disjoint, so the
	// function's own symbol has to be split around them. This test asserts
	// only the outcome -- the region is nameable in a profile -- not the
	// mechanism, so it holds for whichever fix lands.
	auto dump = compilePerfJitDump();
	REQUIRE_FALSE(dump.codeLoads.empty());

	auto namesAnyRegion = [&](std::string_view region) {
		return std::any_of(dump.codeLoads.begin(), dump.codeLoads.end(),
		                   [&](const CodeLoadEntry& e) { return e.name.find(region) != std::string::npos; });
	};
	// The fixture's three regions: outer, and the two nested inside it.
	REQUIRE(namesAnyRegion("outer"));
	REQUIRE(namesAnyRegion("hot"));
	REQUIRE(namesAnyRegion("mix"));
}

TEST_CASE("Perf jitdump: emit_debug_info=false omits JIT_CODE_DEBUG_INFO") {
	auto dump = compilePerfJitDump([](Options& o) {
		o.setOption("mlir.perf.enable", true);
		o.setOption("mlir.perf.emit_debug_info", false);
	});
	REQUIRE_FALSE(dump.codeLoads.empty());
	REQUIRE(dump.debugInfos.empty());
}

} // namespace nautilus::engine

#endif // ENABLE_TRACING && ENABLE_MLIR_BACKEND && __linux__

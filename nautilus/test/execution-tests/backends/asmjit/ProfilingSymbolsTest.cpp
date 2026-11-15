#include "nautilus/config.hpp"

#if defined(ENABLE_TRACING) && defined(ENABLE_ASMJIT_BACKEND) && defined(__linux__)

#include "nautilus/Engine.hpp"
#include "nautilus/compiler/JitSymbolRegistry.hpp"
#include "nautilus/region.hpp"
#include <algorithm>
#include <catch2/catch_all.hpp>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <set>
#include <sstream>
#include <string>
#include <unistd.h>
#include <vector>

#ifdef ENABLE_TBC_JIT
namespace nautilus::compiler::tbc::jit {
bool jitRuntimeAvailable();
} // namespace nautilus::compiler::tbc::jit
#endif

// D14 (docs/plans/nautilus-0.4/05-backends.md §7): the backends that generate code themselves name it for a profiler.
// `asmjit` (and the copy-and-patch mode of `tbc`) publish one symbol per function and per `region()` into the
// JitSymbolRegistry and a line table (code offset -> line of the NIR text form), which `resolveLine` serves; with
// `perf` they also write the perf jitdump. The tests need no performance counters: what they check is the compile.

namespace nautilus::engine {

namespace {

constexpr uint64_t mixCold(uint64_t acc, uint64_t i) {
	return acc * 3 + i;
}
constexpr uint64_t mixHot(uint64_t acc, uint64_t i) {
	return (acc ^ i) * 7;
}

/// Two regions with a multiply each and a different second operation, so that the text lines of the regions can be
/// told apart by what they do.
val<uint64_t> regionKernel(val<uint64_t> n) {
	val<uint64_t> acc = 1;
	region("cold", [&]() {
		for (val<uint64_t> i = 0; i < n; i = i + 1) {
			acc = acc * 3 + i;
		}
	});
	region("hot", [&]() {
		for (val<uint64_t> i = 0; i < n * 4; i = i + 1) {
			acc = (acc ^ i) * 7;
		}
	});
	return acc;
}

uint64_t regionKernelReference(uint64_t n) {
	uint64_t acc = 1;
	for (uint64_t i = 0; i < n; ++i) {
		acc = mixCold(acc, i);
	}
	for (uint64_t i = 0; i < n * 4; ++i) {
		acc = mixHot(acc, i);
	}
	return acc;
}

/// A profiling compile writes the text form of the module to the working directory, where it outlives the process on
/// purpose (`perf annotate` reads it later). A test works in a directory of its own instead and removes it.
class ScratchDirectory {
public:
	ScratchDirectory() : previous_(std::filesystem::current_path()) {
		path_ = std::filesystem::temp_directory_path() /
		        ("nautilus_profiling_symbols_" + std::to_string(::getpid()) + "_" + std::to_string(counter_++));
		std::filesystem::create_directories(path_);
		std::filesystem::current_path(path_);
	}
	~ScratchDirectory() {
		std::filesystem::current_path(previous_);
		std::error_code ignored;
		std::filesystem::remove_all(path_, ignored);
	}
	ScratchDirectory(const ScratchDirectory&) = delete;
	ScratchDirectory& operator=(const ScratchDirectory&) = delete;

private:
	static inline int counter_ = 0;
	std::filesystem::path previous_, path_;
};

Options profileOptions(const std::string& backend, bool sample = true) {
	Options options;
	options.setOption("engine.backend", backend);
	if (sample) {
		options.setOption("perf.sample", true);
	}
	return options;
}

std::vector<std::string> sourceLines(const std::string& file) {
	std::ifstream in(file);
	std::vector<std::string> lines;
	for (std::string line; std::getline(in, line);) {
		lines.push_back(line);
	}
	return lines;
}

std::vector<compiler::JitSymbol> symbolsContaining(const std::string& needle) {
	std::vector<compiler::JitSymbol> found;
	for (const auto& symbol : compiler::JitSymbolRegistry::instance().snapshot()) {
		if (symbol.name.find(needle) != std::string::npos) {
			found.push_back(symbol);
		}
	}
	return found;
}

/// The distinct source lines the addresses of @p symbols resolve to.
std::set<uint32_t> linesOf(const std::vector<compiler::JitSymbol>& symbols) {
	std::set<uint32_t> lines;
	for (const auto& symbol : symbols) {
		for (auto address = symbol.start; address < symbol.end; ++address) {
			compiler::JitLine line;
			if (compiler::JitSymbolRegistry::instance().resolveLine(address, line)) {
				lines.insert(line.line);
			}
		}
	}
	return lines;
}

bool anyLineContains(const std::vector<std::string>& text, const std::set<uint32_t>& lines, const std::string& needle) {
	return std::any_of(lines.begin(), lines.end(), [&](uint32_t line) {
		return line >= 1 && line <= text.size() && text[line - 1].find(needle) != std::string::npos;
	});
}

} // namespace

TEST_CASE("asmjit publishes a symbol per function and per region", "[profiling][asmjit]") {
	ScratchDirectory scratch;
	compiler::JitSymbolRegistry::instance().clear();
	NautilusEngine engine(profileOptions("asmjit"));
	auto compiled = engine.registerFunction(regionKernel);
	REQUIRE(compiled(10) == regionKernelReference(10));

	const auto symbols = compiler::JitSymbolRegistry::instance().snapshot();
	REQUIRE_FALSE(symbols.empty());
	std::string names;
	for (const auto& symbol : symbols) {
		names += "  " + symbol.name + " [" + std::to_string(symbol.size()) + " bytes]\n";
	}
	INFO("published symbols:\n" << names);

	// the regions qualify the name of the function they were traced in, and the function keeps the rest of its code
	CHECK_FALSE(symbolsContaining("execute::cold").empty());
	CHECK_FALSE(symbolsContaining("execute::hot").empty());
	CHECK(std::any_of(symbols.begin(), symbols.end(), [](const auto& s) { return s.name == "execute"; }));

	// the ranges are disjoint, which both the registry and the jitdump rely on
	auto sorted = symbols;
	std::sort(sorted.begin(), sorted.end(), [](const auto& a, const auto& b) { return a.start < b.start; });
	for (size_t i = 1; i < sorted.size(); ++i) {
		CHECK(sorted[i - 1].end <= sorted[i].start);
	}
	// every address of the function's code resolves to the registry handle of this compile
	for (const auto& symbol : symbols) {
		CHECK(symbol.moduleIndex != compiler::NO_MODULE);
		CHECK_FALSE(compiler::JitSymbolRegistry::instance().moduleId(symbol.moduleIndex).empty());
	}
}

TEST_CASE("asmjit attributes code to the NIR operation it came from", "[profiling][asmjit]") {
	ScratchDirectory scratch;
	compiler::JitSymbolRegistry::instance().clear();
	NautilusEngine engine(profileOptions("asmjit"));
	auto compiled = engine.registerFunction(regionKernel);
	REQUIRE(compiled(10) == regionKernelReference(10));

	const auto cold = symbolsContaining("execute::cold");
	const auto hot = symbolsContaining("execute::hot");
	REQUIRE_FALSE(cold.empty());
	REQUIRE_FALSE(hot.empty());

	// the line table points into the text form of the module, which is on disk
	compiler::JitLine line;
	REQUIRE(compiler::JitSymbolRegistry::instance().resolveLine(hot.front().start + hot.front().size() / 2, line));
	const auto file = compiler::JitSymbolRegistry::instance().sourceFile(line.moduleIndex);
	REQUIRE_FALSE(file.empty());
	REQUIRE(std::filesystem::exists(file));
	const auto text = sourceLines(file);
	REQUIRE_FALSE(text.empty());

	const auto hotLines = linesOf(hot);
	const auto coldLines = linesOf(cold);
	INFO("hot lines: " << hotLines.size() << ", cold lines: " << coldLines.size());
	CHECK(hotLines.size() >= 3);
	CHECK(coldLines.size() >= 3);
	for (const auto l : hotLines) {
		CHECK(l >= 1);
		CHECK(l <= text.size());
	}
	// the regions are told apart by their operations: `bxor` is only in the hot one, the add of `acc * 3 + i` only in
	// the cold one
	CHECK(anyLineContains(text, hotLines, "bxor"));
	CHECK(anyLineContains(text, hotLines, "mul"));
	CHECK_FALSE(anyLineContains(text, coldLines, "bxor"));
	CHECK(anyLineContains(text, coldLines, "mul"));
	// and no line is in both
	std::vector<uint32_t> shared;
	std::set_intersection(hotLines.begin(), hotLines.end(), coldLines.begin(), coldLines.end(),
	                      std::back_inserter(shared));
	CHECK(shared.empty());

	// an address outside every range does not pick up a neighbouring module's line
	const auto symbols = compiler::JitSymbolRegistry::instance().snapshot();
	CHECK_FALSE(compiler::JitSymbolRegistry::instance().resolveLine(symbols.back().end + 0x100000, line));

	std::filesystem::remove(file);
}

TEST_CASE("asmjit takes its symbols and lines back with the code", "[profiling][asmjit]") {
	ScratchDirectory scratch;
	compiler::JitSymbolRegistry::instance().clear();
	std::string file;
	{
		NautilusEngine engine(profileOptions("asmjit"));
		auto compiled = engine.registerFunction(regionKernel);
		REQUIRE(compiled(3) == regionKernelReference(3));
		REQUIRE(compiler::JitSymbolRegistry::instance().size() > 0);
		compiler::JitLine line;
		const auto symbol = symbolsContaining("execute::hot").front();
		REQUIRE(compiler::JitSymbolRegistry::instance().resolveLine(symbol.start + symbol.size() / 2, line));
		file = compiler::JitSymbolRegistry::instance().sourceFile(line.moduleIndex);
	}
	CHECK(compiler::JitSymbolRegistry::instance().size() == 0);
	compiler::JitLine line;
	CHECK_FALSE(compiler::JitSymbolRegistry::instance().resolveLine(0x1000, line));
	if (!file.empty()) {
		std::filesystem::remove(file);
	}
}

TEST_CASE("asmjit publishes nothing unless asked to", "[profiling][asmjit]") {
	ScratchDirectory scratch;
	compiler::JitSymbolRegistry::instance().clear();
	NautilusEngine engine(profileOptions("asmjit", /*sample=*/false));
	auto compiled = engine.registerFunction(regionKernel);
	REQUIRE(compiled(10) == regionKernelReference(10));
	CHECK(compiler::JitSymbolRegistry::instance().size() == 0);
}

TEST_CASE("the default tiers publish the code of tier 0", "[profiling][asmjit]") {
	ScratchDirectory scratch;
	// no backend pinned: tier 0 is asmjit, so profiling works with the default engine
	compiler::JitSymbolRegistry::instance().clear();
	Options options;
	options.setOption("perf.sample", true);
	NautilusEngine engine(options);
	auto compiled = engine.registerFunction(regionKernel);
	REQUIRE(compiled(10) == regionKernelReference(10));
	CHECK_FALSE(symbolsContaining("execute").empty());
}

TEST_CASE("asmjit writes the perf jitdump", "[profiling][asmjit]") {
	ScratchDirectory scratch;
	const auto dumpDir =
	    std::filesystem::temp_directory_path() / ("nautilus_asmjit_jitdump_" + std::to_string(::getpid()));
	std::filesystem::remove_all(dumpDir);
	std::filesystem::create_directories(dumpDir);
	setenv("JITDUMPDIR", dumpDir.c_str(), 1);

	{
		auto options = profileOptions("asmjit", /*sample=*/false);
		options.setOption("perf", true);
		NautilusEngine engine(options);
		auto compiled = engine.registerFunction(regionKernel);
		REQUIRE(compiled(10) == regionKernelReference(10));
	}
	unsetenv("JITDUMPDIR");

	std::filesystem::path dump;
	if (std::filesystem::exists(dumpDir / ".debug" / "jit")) {
		for (const auto& entry : std::filesystem::recursive_directory_iterator(dumpDir / ".debug" / "jit")) {
			if (entry.path().extension() == ".dump") {
				dump = entry.path();
			}
		}
	}
	REQUIRE_FALSE(dump.empty());

	std::ifstream in(dump, std::ios::binary);
	std::stringstream buffer;
	buffer << in.rdbuf();
	const auto bytes = buffer.str();
	REQUIRE(bytes.size() > 40);
	uint32_t magic = 0;
	std::memcpy(&magic, bytes.data(), sizeof(magic));
	CHECK(magic == ((uint32_t('J') << 24) | (uint32_t('i') << 16) | (uint32_t('T') << 8) | uint32_t('D')));

	// walk the records: the code-load records name the function and its regions, and each debug-info record is
	// immediately followed by the load record it describes (perf inject pairs them by position)
	std::vector<std::string> loaded;
	uint64_t pendingDebug = 0;
	size_t pairs = 0;
	bool mispaired = false;
	size_t at = 40;
	while (at + 16 <= bytes.size()) {
		uint32_t id = 0, total = 0;
		std::memcpy(&id, bytes.data() + at, 4);
		std::memcpy(&total, bytes.data() + at + 4, 4);
		if (total < 16 || at + total > bytes.size()) {
			break;
		}
		if (id == 0) {
			// pid, tid, vma, code address, code size, code index, then the NUL-terminated name
			uint64_t codeAddress = 0;
			std::memcpy(&codeAddress, bytes.data() + at + 16 + 16, 8);
			loaded.emplace_back(bytes.data() + at + 16 + 40);
			if (pendingDebug != 0) {
				mispaired = mispaired || pendingDebug != codeAddress;
				++pairs;
				pendingDebug = 0;
			}
		} else if (id == 2) {
			std::memcpy(&pendingDebug, bytes.data() + at + 16, 8);
		}
		at += total;
	}
	CHECK_FALSE(mispaired);
	CHECK(pairs >= 2);
	CHECK(std::find(loaded.begin(), loaded.end(), "execute::hot") != loaded.end());
	CHECK(std::find(loaded.begin(), loaded.end(), "execute::cold") != loaded.end());
	CHECK(std::any_of(loaded.begin(), loaded.end(), [](const auto& n) { return n == "execute"; }));

	std::filesystem::remove_all(dumpDir);
}

#ifdef ENABLE_TBC_JIT
TEST_CASE("tbc publishes its stitched code", "[profiling][tbc]") {
	ScratchDirectory scratch;
	if (!compiler::tbc::jit::jitRuntimeAvailable()) {
		SKIP("the copy-and-patch mode cannot run in this build");
	}
	compiler::JitSymbolRegistry::instance().clear();
	auto options = profileOptions("tbc");
	options.setOption("tbc.mode", std::string("jit"));
	NautilusEngine engine(options);
	auto compiled = engine.registerFunction(regionKernel);
	REQUIRE(compiled(10) == regionKernelReference(10));

	CHECK_FALSE(symbolsContaining("execute::cold").empty());
	CHECK_FALSE(symbolsContaining("execute::hot").empty());
	const auto hot = symbolsContaining("execute::hot");
	REQUIRE_FALSE(hot.empty());
	const auto lines = linesOf(hot);
	CHECK(lines.size() >= 2);
	compiler::JitLine line;
	if (compiler::JitSymbolRegistry::instance().resolveLine(hot.front().start, line)) {
		const auto file = compiler::JitSymbolRegistry::instance().sourceFile(line.moduleIndex);
		if (!file.empty()) {
			std::filesystem::remove(file);
		}
	}
}
#endif

} // namespace nautilus::engine

#endif

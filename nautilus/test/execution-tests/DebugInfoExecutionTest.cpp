#include "nautilus/config.hpp"

#if defined(ENABLE_TRACING) && defined(ENABLE_MLIR_BACKEND)

#include "ExecutionTest.hpp"
#include "nautilus/Engine.hpp"
#include "nautilus/nautilus_function.hpp"
#include "nautilus/region.hpp"
#include <algorithm>
#include <atomic>
#include <catch2/catch_all.hpp>
#include <cctype>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <functional>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <string_view>
#include <unistd.h>
#include <vector>

namespace nautilus::engine {

namespace {

val<int32_t> debugAddOne(val<int32_t> x) {
	return x + 1;
}

val<int32_t> debugSumThree(val<int32_t> a, val<int32_t> b, val<int32_t> c) {
	auto t = a + b;
	return t + c;
}

val<int32_t> debugSumLoop(val<int32_t> upperLimit) {
	val<int32_t> agg = val<int32_t>(0);
	for (val<int32_t> i = 0; i < upperLimit; i = i + 1) {
		agg = agg + i;
	}
	return agg;
}

// A small bundle of functions registered together in a CompiledModule,
// used to exercise debug info for compilation units that contain more
// than one Nautilus function.
val<int32_t> debugModAdd(val<int32_t> x) {
	return x + 1;
}

val<int32_t> debugModMul(val<int32_t> x, val<int32_t> y) {
	auto t = x * y;
	return t + 1;
}

val<int32_t> debugModLoop(val<int32_t> n) {
	val<int32_t> agg = 0;
	for (val<int32_t> i = 0; i < n; i = i + 1) {
		agg = agg + i;
	}
	return agg;
}

// A helper traced as a separate Nautilus function.  Wrapping it in a
// NautilusFunction makes the call from `debugCaller` emit a
// CallOperation — the caller and callee lower to two distinct
// `func.func`s in the same MLIR module and exercise debug info across
// function boundaries.
val<int32_t> debugHelperImpl(val<int32_t> x, val<int32_t> y) {
	auto prod = x * y;
	return prod + 1;
}

static auto debugHelper = NautilusFunction {"debug_helper", debugHelperImpl};

val<int32_t> debugCaller(val<int32_t> a, val<int32_t> b) {
	val<int32_t> acc = 0;
	for (val<int32_t> i = 0; i < b; i = i + 1) {
		acc = acc + debugHelper(a, i);
	}
	return acc;
}

// Wraps the loop body in a named region() so the MLIR backend has region
// metadata (docs/region.md) to lower to a DWARF lexical scope (#455).
val<int32_t> debugRegionSum(val<int32_t> upperLimit) {
	val<int32_t> agg = val<int32_t>(0);
	region("accumulate", [&]() {
		for (val<int32_t> i = 0; i < upperLimit; i = i + 1) {
			region("agg if", [&]() {
				if (i < 100) {
					agg = agg + i;
				}
			});
		}
	});
	region("accumulate2", [&]() {
		for (val<int32_t> i = 0; i < upperLimit; i = i + 1) {
			agg = agg + i;
		}
	});
	return agg;
}

// Four levels of region() nesting, so the DWARF inlined-subroutine chain a
// region lowers to is exercised past the two levels debugRegionSum reaches.
val<int32_t> debugDeeplyNestedRegions(val<int32_t> upperLimit) {
	val<int32_t> agg = val<int32_t>(0);
	region("lvl1", [&]() {
		region("lvl2", [&]() {
			region("lvl3", [&]() {
				region("lvl4", [&]() {
					for (val<int32_t> i = 0; i < upperLimit; i = i + 1) {
						agg = agg + i;
					}
				});
			});
		});
	});
	return agg;
}

// How deep debugVeryDeeplyNestedRegions nests. Region names are `d1` (innermost) .. `d50`.
constexpr int kDebugNestDepth = 50;

// Stable, distinct names: region() stores the const char* it is handed.
const char* debugNestedRegionName(int level) {
	static const std::vector<std::string>* names = [] {
		auto* built = new std::vector<std::string>(kDebugNestDepth + 1);
		for (int i = 0; i <= kDebugNestDepth; ++i) {
			(*built)[i] = "d" + std::to_string(i);
		}
		return built;
	}();
	return (*names)[level].c_str();
}

// Opens N nested region()s around `body`, outermost first. Recursive so the depth is one
// constant; each level is a distinct instantiation, so region()'s frame-pointer walk still sees
// a distinct call site per level.
template <int N>
struct DebugNestRegions {
	template <typename F>
	static void apply(F&& body) {
		region(debugNestedRegionName(N), [&]() { DebugNestRegions<N - 1>::apply(body); });
	}
};

template <>
struct DebugNestRegions<0> {
	template <typename F>
	static void apply(F&& body) {
		body();
	}
};

val<int32_t> debugVeryDeeplyNestedRegions(val<int32_t> upperLimit) {
	val<int32_t> agg = val<int32_t>(0);
	DebugNestRegions<kDebugNestDepth>::apply([&]() {
		for (val<int32_t> i = 0; i < upperLimit; i = i + 1) {
			agg = agg + i;
		}
	});
	return agg;
}

val<int32_t> debugNestedControlFlow(val<int32_t> limit) {
	val<int32_t> even = 0;
	val<int32_t> odd = 0;
	for (val<int32_t> i = 0; i < limit; i = i + 1) {
		if (i % 2 == 0) {
			if (i > 4) {
				even = even + i;
			}
		} else {
			odd = odd + 1;
		}
	}
	return even + odd;
}

std::string readFile(const std::string& path) {
	std::ifstream in(path);
	std::stringstream ss;
	ss << in.rdbuf();
	return ss.str();
}

/// The debug metadata of one emitted module, parsed once.
///
/// Every debug-info assertion in this file works from one of these rather than
/// scanning the IR text for itself: the metadata graph (which scope owns what,
/// which location a given instruction carries) is the thing under test, and
/// re-deriving it per test is how assertions drift apart.
struct DebugIr {
	struct Subprogram {
		std::string name;
		int line = 0;
		std::string fileId;
	};
	struct LexicalBlock {
		std::string scopeId;
		int line = 0;
	};
	struct Location {
		int line = 0;
		std::string scopeId;
		std::string inlinedAtId; // empty when not inlined
	};
	struct LocalVariable {
		std::string name;
		std::string scopeId;
		int line = 0;
	};

	std::string text;                                  // the emitted LLVM IR
	std::string sourceText;                            // the Nautilus-IR dump the DWARF points at
	std::map<std::string, Subprogram> subprograms;     // metadata id -> subprogram
	std::map<std::string, LexicalBlock> lexicalBlocks; // metadata id -> lexical block
	std::map<std::string, Location> locations;         // metadata id -> location
	std::vector<LocalVariable> variables;
	bool sawLexicalBlockFile = false;

	[[nodiscard]] bool contains(std::string_view needle) const {
		return text.find(needle) != std::string::npos;
	}

	/// The metadata id of the DISubprogram named @p name, or empty.
	[[nodiscard]] std::string subprogramId(std::string_view name) const {
		for (const auto& [id, sub] : subprograms) {
			if (sub.name == name) {
				return id;
			}
		}
		return {};
	}

	/// The body lines of `define ... @name(`, without the define/closing brace.
	[[nodiscard]] std::vector<std::string> bodyOf(std::string_view function) const {
		std::vector<std::string> body;
		std::istringstream iss(text);
		std::string line;
		bool inside = false;
		const std::string marker = "@" + std::string(function) + "(";
		while (std::getline(iss, line)) {
			if (!inside) {
				inside = line.rfind("define ", 0) == 0 && line.find(marker) != std::string::npos;
				continue;
			}
			if (line == "}") {
				break;
			}
			body.push_back(line);
		}
		return body;
	}

	/// The `!dbg` id on the first instruction of @p function that contains every
	/// one of @p needles and none of @p without. Empty when there is no match.
	[[nodiscard]] std::string dbgIdOf(std::string_view function, const std::vector<std::string>& needles,
	                                  const std::vector<std::string>& without = {}) const {
		for (const auto& line : bodyOf(function)) {
			const bool wanted = std::all_of(needles.begin(), needles.end(),
			                                [&](const std::string& n) { return line.find(n) != std::string::npos; });
			const bool unwanted = std::any_of(without.begin(), without.end(),
			                                  [&](const std::string& n) { return line.find(n) != std::string::npos; });
			if (!wanted || unwanted) {
				continue;
			}
			auto pos = line.find("!dbg !");
			if (pos == std::string::npos) {
				continue;
			}
			auto start = pos + std::string("!dbg !").size();
			auto end = start;
			while (end < line.size() && std::isdigit(static_cast<unsigned char>(line[end]))) {
				++end;
			}
			return line.substr(start, end - start);
		}
		return {};
	}

	/// The `!dbg` ids of every instruction in @p function whose text, once
	/// indentation is stripped, starts with @p prefix.
	[[nodiscard]] std::vector<std::string> dbgIdsStartingWith(std::string_view function,
	                                                          std::string_view prefix) const {
		std::vector<std::string> ids;
		for (const auto& line : bodyOf(function)) {
			auto first = line.find_first_not_of(" \t");
			if (first == std::string::npos || line.compare(first, prefix.size(), prefix) != 0) {
				continue;
			}
			auto pos = line.find("!dbg !");
			if (pos == std::string::npos) {
				continue;
			}
			auto start = pos + std::string("!dbg !").size();
			auto end = start;
			while (end < line.size() && std::isdigit(static_cast<unsigned char>(line[end]))) {
				++end;
			}
			ids.push_back(line.substr(start, end - start));
		}
		return ids;
	}

	/// The scope ids, innermost first, that @p dbgId resolves through: its own
	/// scope, then the scope of each `inlinedAt` location above it.
	[[nodiscard]] std::vector<std::string> frameScopes(std::string dbgId) const {
		std::vector<std::string> scopes;
		while (!dbgId.empty()) {
			auto it = locations.find(dbgId);
			if (it == locations.end()) {
				break;
			}
			scopes.push_back(it->second.scopeId);
			dbgId = it->second.inlinedAtId;
		}
		return scopes;
	}

	/// The subprogram a scope id belongs to, following a lexical block to its
	/// parent. Empty when the scope is neither.
	[[nodiscard]] std::string owningSubprogram(const std::string& scopeId) const {
		if (subprograms.count(scopeId)) {
			return scopeId;
		}
		if (auto it = lexicalBlocks.find(scopeId); it != lexicalBlocks.end()) {
			return owningSubprogram(it->second.scopeId);
		}
		return {};
	}

	/// Lines of the lexical blocks parented directly on @p subprogramId.
	[[nodiscard]] std::vector<int> lexicalBlockLinesOf(const std::string& subprogramId) const {
		std::vector<int> lines;
		for (const auto& [id, block] : lexicalBlocks) {
			if (block.scopeId == subprogramId) {
				lines.push_back(block.line);
			}
		}
		return lines;
	}

	/// Names of the variables whose scope resolves to @p subprogramId.
	[[nodiscard]] std::vector<std::string> variableNamesIn(const std::string& subprogramId) const {
		std::vector<std::string> names;
		for (const auto& variable : variables) {
			if (owningSubprogram(variable.scopeId) == subprogramId) {
				names.push_back(variable.name);
			}
		}
		return names;
	}
};

/// Reads `key` out of @p line as a run of digits, e.g. valueAfter(l, "line: ").
int intAfter(const std::string& line, const std::string& key) {
	auto pos = line.find(key);
	if (pos == std::string::npos) {
		return 0;
	}
	auto start = pos + key.size();
	auto end = start;
	while (end < line.size() && std::isdigit(static_cast<unsigned char>(line[end]))) {
		++end;
	}
	return end == start ? 0 : std::stoi(line.substr(start, end - start));
}

/// Reads a metadata reference, e.g. refAfter(l, "scope: !") -> "12".
std::string refAfter(const std::string& line, const std::string& key) {
	auto pos = line.find(key);
	if (pos == std::string::npos) {
		return {};
	}
	auto start = pos + key.size();
	auto end = start;
	while (end < line.size() && std::isdigit(static_cast<unsigned char>(line[end]))) {
		++end;
	}
	return line.substr(start, end - start);
}

DebugIr parseDebugIr(std::string text) {
	DebugIr ir;
	ir.text = std::move(text);
	std::istringstream iss(ir.text);
	std::string line;
	while (std::getline(iss, line)) {
		if (line.find("!DILexicalBlockFile(") != std::string::npos) {
			ir.sawLexicalBlockFile = true;
		}
		if (line.empty() || line[0] != '!') {
			continue;
		}
		auto idEnd = line.find(' ');
		if (idEnd == std::string::npos) {
			continue;
		}
		auto id = line.substr(1, idEnd - 1);
		if (line.find("!DISubprogram(") != std::string::npos) {
			DebugIr::Subprogram sub;
			auto nameKey = line.find("name: \"");
			if (nameKey != std::string::npos) {
				auto start = nameKey + std::string("name: \"").size();
				sub.name = line.substr(start, line.find('"', start) - start);
			}
			sub.line = intAfter(line, "line: ");
			sub.fileId = refAfter(line, "file: !");
			ir.subprograms[id] = sub;
		} else if (line.find("!DILexicalBlock(") != std::string::npos) {
			ir.lexicalBlocks[id] = {refAfter(line, "scope: !"), intAfter(line, "line: ")};
		} else if (line.find("!DILocation(") != std::string::npos) {
			ir.locations[id] = {intAfter(line, "line: "), refAfter(line, "scope: !"),
			                    line.find("inlinedAt: !") != std::string::npos ? refAfter(line, "inlinedAt: !")
			                                                                   : std::string {}};
		} else if (line.find("!DILocalVariable(") != std::string::npos) {
			DebugIr::LocalVariable variable;
			auto nameKey = line.find("name: \"");
			if (nameKey != std::string::npos) {
				auto start = nameKey + std::string("name: \"").size();
				variable.name = line.substr(start, line.find('"', start) - start);
			}
			variable.scopeId = refAfter(line, "scope: !");
			variable.line = intAfter(line, "line: ");
			ir.variables.push_back(variable);
		}
	}
	return ir;
}

using OptionTweak = std::function<void(Options&)>;

/// Options with debug info on and the Nautilus-IR dump as the DWARF source --
/// the configuration every metadata test in this file needs.
Options debugOptions(const OptionTweak& tweak = {}) {
	Options options;
	options.setOption("engine.backend", std::string("mlir"));
	options.setOption("mlir.debug.enable", true);
	options.setOption("mlir.debug.source_mode", std::string("nautilus-ir"));
	if (tweak) {
		tweak(options);
	}
	return options;
}

/// Compiles through @p body with the DWARF "source" file written to a path of
/// this test's own, and returns what landed there.
template <typename Body>
std::string compileDebugSource(const std::string& extension, Body&& body, const OptionTweak& tweak = {}) {
	const auto sourcePath = (std::filesystem::temp_directory_path() /
	                         ("nautilus_debug_source_" + std::to_string(::getpid()) + "." + extension))
	                            .string();
	std::filesystem::remove(sourcePath);

	auto options = debugOptions([&](Options& o) {
		o.setOption("mlir.debug.source_file", sourcePath);
		if (tweak) {
			tweak(o);
		}
	});
	NautilusEngine engine(options);
	body(engine);

	REQUIRE(std::filesystem::exists(sourcePath));
	auto contents = readFile(sourcePath);
	std::filesystem::remove(sourcePath);
	return contents;
}

/// Compiles through @p body and returns the parsed debug metadata of the module
/// that defines @p functionName.
///
/// The emitted module is recovered from the `before_llvm_optimization` dump.
/// The dump root is shared by every test in the process, so candidates are
/// filtered by the function they define rather than by "whichever directory
/// appeared" -- otherwise a concurrently running test's dump can be picked up.
template <typename Body>
DebugIr compileDebugIr(const std::string& functionName, Body&& body, const OptionTweak& tweak = {},
                       const std::string& dumpStage = "before_llvm_optimization") {
	const auto dumpRoot = std::filesystem::temp_directory_path() / "dump";
	std::set<std::filesystem::path> existing;
	if (std::filesystem::exists(dumpRoot)) {
		for (const auto& e : std::filesystem::directory_iterator(dumpRoot)) {
			existing.insert(e.path());
		}
	}

	// A path of our own, so the Nautilus-IR dump the DWARF points at can be
	// read back as part of the result instead of hunting for it.
	static std::atomic<unsigned> sourceCounter {0};
	const auto sourcePath =
	    (std::filesystem::temp_directory_path() / ("nautilus_debug_test_" + std::to_string(::getpid()) + "_" +
	                                               std::to_string(sourceCounter.fetch_add(1)) + ".ir"))
	        .string();
	std::filesystem::remove(sourcePath);

	auto options = debugOptions([&](Options& o) {
		o.setOption("dump." + dumpStage, true);
		o.setOption("mlir.debug.source_file", sourcePath);
		if (tweak) {
			tweak(o);
		}
	});
	NautilusEngine engine(options);
	body(engine);

	std::string sourceText;
	if (std::filesystem::exists(sourcePath)) {
		sourceText = readFile(sourcePath);
		std::filesystem::remove(sourcePath);
	}

	// Candidates are matched on this compilation's own source-file path, which
	// the module records in its DIFile. The dump root is shared by every test
	// in the process and nearly every traced function lowers to `@execute`, so
	// neither "whichever directory appeared" nor the function name alone can
	// tell two concurrent tests apart.
	const std::string marker = "@" + functionName + "(";
	const std::string sourceMarker = std::filesystem::path(sourcePath).filename().string();
	if (std::filesystem::exists(dumpRoot)) {
		for (const auto& dir : std::filesystem::directory_iterator(dumpRoot)) {
			if (existing.count(dir.path())) {
				continue;
			}
			for (const auto& entry : std::filesystem::recursive_directory_iterator(dir)) {
				if (!entry.is_regular_file() || entry.path().filename() != dumpStage + ".ll") {
					continue;
				}
				auto contents = readFile(entry.path().string());
				if (contents.find(sourceMarker) != std::string::npos && contents.find(marker) != std::string::npos) {
					auto ir = parseDebugIr(std::move(contents));
					ir.sourceText = std::move(sourceText);
					return ir;
				}
			}
		}
	}
	FAIL("no " << dumpStage << " dump defining " << functionName);
	return {};
}

} // namespace

TEST_CASE("Debug info: disabled by default produces identical results") {
	Options options;
	options.setOption("engine.backend", std::string("mlir"));

	NautilusEngine engine(options);
	auto fn = engine.registerFunction(debugAddOne);
	REQUIRE(fn(41) == 42);
}

TEST_CASE("Debug info: MLIR source mode writes a snapshot file and compiles") {
	// LocationSnapshot writes the post-inline MLIR to the configured path. It
	// must be non-empty and contain the func symbol, so gdb/lldb can resolve
	// breakpoints against its lines.
	const auto contents = compileDebugSource(
	    "mlir",
	    [](NautilusEngine& engine) {
		    auto fn = engine.registerFunction(debugAddOne);
		    REQUIRE(fn(41) == 42);
	    },
	    [](Options& options) { options.setOption("mlir.debug.source_mode", std::string("mlir")); });

	REQUIRE_FALSE(contents.empty());
	REQUIRE(contents.find("func.func") != std::string::npos);
}

TEST_CASE("Debug info: Nautilus IR source mode emits the IR dump as the source file") {
	const auto contents = compileDebugSource("ir", [](NautilusEngine& engine) {
		auto fn = engine.registerFunction(debugSumThree);
		REQUIRE(fn(1, 2, 3) == 6);
	});

	// The source file is the Nautilus IR dump, recognizable by the outer
	// `nautilus { ... } //nautilus` bracket IRGraph::toString() produces.
	REQUIRE(contents.find("nautilus {") != std::string::npos);
	REQUIRE(contents.find("//nautilus") != std::string::npos);
}

TEST_CASE("Debug info: default source path is synthesized when none provided") {
	auto options = debugOptions();

	NautilusEngine engine(options);
	auto fn = engine.registerFunction(debugAddOne);
	REQUIRE(fn(10) == 11);
}

TEST_CASE("Debug info: the synthesized source file lands in the temp directory") {
	// The default keeps generated files out of the user's tree. An IDE that
	// cannot open a $TMPDIR path -- on macOS a /var/folders/... one, outside
	// its source roots -- points `mlir.debug.source_dir` somewhere it can.
	const auto tempDir = std::filesystem::temp_directory_path();
	std::set<std::filesystem::path> before;
	for (const auto& e : std::filesystem::directory_iterator(tempDir)) {
		before.insert(e.path());
	}
	const auto cwd = std::filesystem::current_path();
	std::set<std::filesystem::path> cwdBefore;
	for (const auto& e : std::filesystem::directory_iterator(cwd)) {
		cwdBefore.insert(e.path());
	}

	auto options = debugOptions();

	NautilusEngine engine(options);
	auto fn = engine.registerFunction(debugAddOne);
	REQUIRE(fn(10) == 11);

	auto newDumps = [](const std::filesystem::path& dir, const std::set<std::filesystem::path>& seen) {
		std::vector<std::filesystem::path> created;
		for (const auto& e : std::filesystem::directory_iterator(dir)) {
			if (!seen.count(e.path()) && e.path().filename().string().starts_with("nautilus_debug_") &&
			    e.path().extension() == ".ir") {
				created.push_back(e.path());
			}
		}
		return created;
	};

	const auto inTemp = newDumps(tempDir, before);
	REQUIRE_FALSE(inTemp.empty());
	// And nothing dropped into the working directory.
	REQUIRE(newDumps(cwd, cwdBefore).empty());

	for (const auto& path : inTemp) {
		std::filesystem::remove(path);
	}
}

TEST_CASE("Debug info: source_dir redirects the synthesized source file") {
	const auto dir = std::filesystem::temp_directory_path() / ("nautilus_src_dir_" + std::to_string(::getpid()));
	std::filesystem::remove_all(dir);
	std::filesystem::create_directories(dir);

	auto options = debugOptions();
	options.setOption("mlir.debug.source_dir", dir.string());

	NautilusEngine engine(options);
	auto fn = engine.registerFunction(debugAddOne);
	REQUIRE(fn(10) == 11);

	REQUIRE(std::distance(std::filesystem::directory_iterator(dir), std::filesystem::directory_iterator {}) > 0);
	std::filesystem::remove_all(dir);
}

TEST_CASE("Debug info: a relative source_file is recorded as an absolute path") {
	// A relative name in the DWARF would be resolved against DW_AT_comp_dir,
	// which for a JIT module is not a directory the user controls — the
	// debugger then reports the source as missing.
	const auto relative = "nautilus_relative_" + std::to_string(::getpid()) + ".ir";
	const auto expected = std::filesystem::current_path() / relative;
	std::filesystem::remove(expected);

	auto options = debugOptions();
	options.setOption("mlir.debug.source_file", relative);

	NautilusEngine engine(options);
	auto fn = engine.registerFunction(debugAddOne);
	REQUIRE(fn(10) == 11);

	REQUIRE(std::filesystem::exists(expected));
	// The IR dump names itself by the absolute path the DWARF points at.
	std::filesystem::remove(expected);
}

// The GDB JIT interface: the debugger sets a breakpoint on
// `__jit_debug_register_code` and walks `__jit_debug_descriptor`'s linked list
// to find the in-memory objects it should read DWARF from.  LLVM's
// GDBJITDebugInfoRegistrationPlugin (installed by
// `llvm::orc::enableDebuggerSupport`) owns both symbols in this process, so the
// descriptor's entry list is what a debugger would actually see.
extern "C" {
struct jit_code_entry {
	jit_code_entry* next_entry;
	jit_code_entry* prev_entry;
	const char* symfile_addr;
	uint64_t symfile_size;
};
struct jit_descriptor {
	uint32_t version;
	uint32_t action_flag;
	jit_code_entry* relevant_entry;
	jit_code_entry* first_entry;
};
extern jit_descriptor __jit_debug_descriptor;
}

namespace {

size_t jitDebugEntryCount() {
	size_t count = 0;
	for (auto* e = __jit_debug_descriptor.first_entry; e != nullptr; e = e->next_entry) {
		++count;
	}
	return count;
}

} // namespace

TEST_CASE("Debug info: JIT-linked objects are registered with the debugger") {
	const auto before = jitDebugEntryCount();

	auto options = debugOptions();
	options.setOption("mlir.eager_compilation", true);

	NautilusEngine engine(options);
	auto fn = engine.registerFunction(debugAddOne);
	REQUIRE(fn(10) == 11);

	// Without the registration plugin the DWARF is emitted but never handed
	// to the debugger, and this list stays empty no matter how much debug
	// info the object carries.
	REQUIRE(jitDebugEntryCount() > before);
}

TEST_CASE("Debug info: debugger registration can be disabled") {
	const auto before = jitDebugEntryCount();

	auto options = debugOptions();
	options.setOption("mlir.debug.register_with_debugger", false);
	options.setOption("mlir.eager_compilation", true);

	NautilusEngine engine(options);
	auto fn = engine.registerFunction(debugAddOne);
	REQUIRE(fn(10) == 11);

	REQUIRE(jitDebugEntryCount() == before);
}

TEST_CASE("Debug info: nautilus-ir mode emits alloca + dbg.declare for each $N DILocalVariable") {
	// Each `$N` is a shadow alloca with a dbg.declare pointing at it (see
	// EmitDbgValuePass). Both the DILocalVariable metadata and the debug
	// record have to survive MLIR->LLVM translation.
	auto ir = compileDebugIr("execute", [](NautilusEngine& engine) {
		auto fn = engine.registerFunction(debugSumThree);
		REQUIRE(fn(1, 2, 3) == 6);
	});

	REQUIRE_FALSE(ir.variables.empty());
	// DILocalVariable names use a `v<N>` prefix rather than `$<N>`, which GDB's
	// value-history syntax reserves.
	for (const auto& variable : ir.variables) {
		INFO("variable " << variable.name);
		REQUIRE(variable.name.rfind('v', 0) == 0);
	}
	// LLVM 21 emits `#dbg_declare` records; older syntax is the intrinsic call.
	REQUIRE((ir.contains("#dbg_declare") || ir.contains("@llvm.dbg.declare")));
	REQUIRE(ir.contains("= alloca i32"));
}

TEST_CASE("Debug info: perf-only mode gets a line table but no $N shadow allocas") {
	// mlir.perf.enable alone (mlir.debug.enable off) is Axis B off / Axis A
	// on: MLIRLoweringProvider only ever creates a $N shadow alloca (and the
	// dbg.declare/dbg.value machinery this test's sibling above checks for)
	// when `enableDebug` clamps for stepping, so a perf-only compile should
	// keep the codegen-distorting side of debug info off entirely while
	// still getting a real DISubprogram line table for jitdump to read.
	auto ir = compileDebugIr(
	    "execute",
	    [](NautilusEngine& engine) {
		    auto fn = engine.registerFunction(debugSumThree);
		    REQUIRE(fn(1, 2, 3) == 6);
	    },
	    [](Options& options) {
		    options.setOption("mlir.debug.enable", false);
		    options.setOption("mlir.perf.enable", true);
	    });

	REQUIRE_FALSE(ir.subprograms.empty());
	for (const auto& [id, subprogram] : ir.subprograms) {
		INFO("DISubprogram !" << id << " (" << subprogram.name << ")");
		REQUIRE(subprogram.line != 0);
	}
	REQUIRE(ir.variables.empty());
	REQUIRE_FALSE(ir.contains("#dbg_declare"));
	REQUIRE_FALSE(ir.contains("@llvm.dbg.declare"));
	REQUIRE_FALSE(ir.contains("= alloca i32"));
}

TEST_CASE("Debug info: generated LLVM IR contains DICompileUnit") {
	auto ir = compileDebugIr(
	    "execute",
	    [](NautilusEngine& engine) {
		    auto fn = engine.registerFunction(debugAddOne);
		    REQUIRE(fn(5) == 6);
	    },
	    [](Options& options) { options.setOption("mlir.debug.source_mode", std::string("mlir")); },
	    "after_llvm_generation");

	REQUIRE(ir.contains("DICompileUnit"));
}

TEST_CASE("Debug info: nautilus-ir mode emits DISubprogram with non-zero line") {
	// `line: 0` on a subprogram means "no source position", which costs the
	// function its entry in a debugger's line table.
	auto ir = compileDebugIr(
	    "execute",
	    [](NautilusEngine& engine) {
		    auto fn = engine.registerFunction(debugAddOne);
		    REQUIRE(fn(5) == 6);
	    },
	    {}, "after_llvm_generation");

	REQUIRE_FALSE(ir.subprograms.empty());
	for (const auto& [id, subprogram] : ir.subprograms) {
		INFO("DISubprogram !" << id << " (" << subprogram.name << ")");
		REQUIRE(subprogram.line != 0);
	}
}

TEST_CASE("Debug info: loop body produces a multi-block IR with N lines for every op") {
	// A looping function lowers to several MLIR blocks and carries SSA block
	// arguments across iterations. It must still produce a valid IR source
	// dump, and every emitted DISubprogram must keep a real `line:`.
	auto ir = compileDebugIr(
	    "execute",
	    [](NautilusEngine& engine) {
		    auto fn = engine.registerFunction(debugSumLoop);
		    REQUIRE(fn(5) == 10); // sum of 0..4
	    },
	    {}, "after_llvm_generation");

	// The dump is in fact multi-block.
	REQUIRE(ir.sourceText.find("Block_") != std::string::npos);
	REQUIRE(ir.sourceText.find("return") != std::string::npos);
	REQUIRE(std::count(ir.sourceText.begin(), ir.sourceText.end(), '\n') > 5);

	REQUIRE_FALSE(ir.subprograms.empty());
	for (const auto& [id, subprogram] : ir.subprograms) {
		INFO("DISubprogram !" << id << " (" << subprogram.name << ")");
		REQUIRE(subprogram.line != 0);
	}
}

TEST_CASE("Debug info: nested control flow preserves $N DILocalVariables across blocks") {
	// Every `$N` in the IR dump -- block-argument declarations in a block
	// header as well as `$N = ...` definitions -- must surface as a
	// DILocalVariable, otherwise GDB cannot resolve it across a branch.
	auto ir = compileDebugIr("execute", [](NautilusEngine& engine) {
		auto fn = engine.registerFunction(debugNestedControlFlow);
		REQUIRE(fn(10) == 19);
	});

	std::set<int> irIds;
	for (std::string::size_type p = 0; (p = ir.sourceText.find('$', p)) != std::string::npos; ++p) {
		std::string::size_type q = p + 1;
		int id = 0;
		bool any = false;
		while (q < ir.sourceText.size() && std::isdigit(static_cast<unsigned char>(ir.sourceText[q]))) {
			id = id * 10 + (ir.sourceText[q] - '0');
			any = true;
			++q;
		}
		if (any) {
			irIds.insert(id);
		}
	}
	REQUIRE(irIds.size() > 5);

	// DILocalVariable uses a `v` prefix instead of `$`, which GDB would read as
	// value-history syntax.
	std::set<int> variableIds;
	for (const auto& variable : ir.variables) {
		if (variable.name.size() > 1 && variable.name.front() == 'v') {
			variableIds.insert(std::stoi(variable.name.substr(1)));
		}
	}
	REQUIRE_FALSE(variableIds.empty());
	for (int id : irIds) {
		INFO("IR defines $" << id << " but no matching DILocalVariable was emitted");
		REQUIRE(variableIds.count(id) == 1);
	}
}

TEST_CASE("Debug info: per-block DILexicalBlock scoping narrows variable visibility") {
	// EmitDbgValuePass builds one DILexicalBlock per Nautilus basic block and
	// scopes each DILocalVariable to the block that stores into its shadow
	// alloca, so a variable is only in scope where it is live.
	auto ir = compileDebugIr("execute", [](NautilusEngine& engine) {
		auto fn = engine.registerFunction(debugNestedControlFlow);
		REQUIRE(fn(10) == 19);
	});

	const auto executeId = ir.subprogramId("execute");
	REQUIRE_FALSE(executeId.empty());

	// More than one block scope, all parented on the function: a single scope
	// would mean every variable is visible everywhere.
	const auto blockLines = ir.lexicalBlockLinesOf(executeId);
	REQUIRE(blockLines.size() >= 2);

	// And variables actually sit in those block scopes rather than directly on
	// the subprogram -- that is what narrows visibility.
	int varsInBlockScope = 0;
	for (const auto& variable : ir.variables) {
		if (ir.lexicalBlocks.count(variable.scopeId) && ir.owningSubprogram(variable.scopeId) == executeId) {
			++varsInBlockScope;
		}
	}
	REQUIRE(varsInBlockScope >= 1);
}

TEST_CASE("Debug info: region() scopes lower to a DWARF inlined subroutine, shared across blocks") {
	// A region() lowers to a DWARF *inlined subroutine*, not a plain lexical
	// block: gdb prints one line per stack frame, and a lexical block is never
	// a frame boundary, so a region built from one would never appear in `bt`.
	// Modeling it as "as if inlined" reuses the mechanism gdb already has for
	// an -O2-inlined function, giving region("accumulate", ...) a real
	// `#0 accumulate () / #1 execute ()` frame pair. This was verified against
	// a real binary under gdb; the test checks the static structure that makes
	// it possible.
	auto ir = compileDebugIr("execute", [](NautilusEngine& engine) {
		auto fn = engine.registerFunction(debugRegionSum);
		// Two regions each accumulate 0..4.
		REQUIRE(fn(5) == 20);
	});

	const auto executeId = ir.subprogramId("execute");
	const auto accumulateId = ir.subprogramId("accumulate");
	const auto aggIfId = ir.subprogramId("agg if");
	REQUIRE_FALSE(executeId.empty());
	REQUIRE_FALSE(accumulateId.empty());
	REQUIRE_FALSE(aggIfId.empty());

	// A DILexicalBlockFile can never surface as its own DWARF scope, region or
	// not (see RegionScopeInfo.hpp).
	REQUIRE_FALSE(ir.sawLexicalBlockFile);

	// `line: 0` means "compiler-generated, no source position". Anything the
	// region lowers -- a block argument's phi included -- that keeps a
	// still-marked region location translates to exactly that, and a debugger
	// then attributes the instruction to the inlinedAt call site instead.
	for (const auto& [id, location] : ir.locations) {
		INFO("DILocation !" << id);
		REQUIRE(location.line != 0);
	}

	// Each region's synthetic subprogram stays on execute's own file: a
	// DILocation has no file of its own, so a mismatch here would silently
	// reinterpret unrelated source lines.
	REQUIRE(ir.subprograms.at(accumulateId).fileId == ir.subprograms.at(executeId).fileId);
	REQUIRE(ir.subprograms.at(aggIfId).fileId == ir.subprograms.at(executeId).fileId);

	// A shared scope, not one re-created per block: the loop condition
	// (loop-header block) and the induction increment (latch block) sit in two
	// different Nautilus blocks and resolve to the same "accumulate"
	// subprogram. The accumulation is one region deeper -- inside
	// region("agg if") -- so it belongs to that region's subprogram; a nested
	// region that collapsed into its parent would show "accumulate" here and
	// make the inner frame invisible in a backtrace.
	const auto condition = ir.dbgIdOf("execute", {"icmp slt"});
	const auto increment = ir.dbgIdOf("execute", {"add i32 %", ", 1,"});
	const auto accumulation = ir.dbgIdOf("execute", {"add i32 %", ", %"});
	REQUIRE_FALSE(condition.empty());
	REQUIRE_FALSE(increment.empty());
	REQUIRE_FALSE(accumulation.empty());

	REQUIRE(ir.frameScopes(condition) == std::vector<std::string> {accumulateId, executeId});
	REQUIRE(ir.frameScopes(increment) == std::vector<std::string> {accumulateId, executeId});
	// Two deep, and in order: that is what makes `bt` read agg if / accumulate
	// / execute rather than a dangling or flattened scope.
	REQUIRE(ir.frameScopes(accumulation) == std::vector<std::string> {aggIfId, accumulateId, executeId});

	// The ops inside a region keep their own line. Sharing one location for a
	// whole region would collapse it onto a single steppable line.
	REQUIRE(ir.locations.at(condition).line != ir.locations.at(accumulation).line);

	// Each enclosing frame is located where the region below it opens, not at
	// the function header: stopped in the accumulation, gdb shows "agg if" at
	// the add's own line, "accumulate" at the line "agg if" opens on, and
	// "execute" at the line "accumulate" opens on.
	const int functionHeaderLine = ir.subprograms.at(executeId).line;
	const int innerLine = ir.locations.at(ir.locations.at(accumulation).inlinedAtId).line;
	const auto outerId = ir.locations.at(ir.locations.at(accumulation).inlinedAtId).inlinedAtId;
	const int outerLine = ir.locations.at(outerId).line;
	INFO("agg if at " << ir.locations.at(accumulation).line << ", accumulate at " << innerLine << ", execute at "
	                  << outerLine);
	REQUIRE(outerLine != functionHeaderLine);
	REQUIRE(innerLine != functionHeaderLine);
	// Outermost opens first, then the nested region, then the op itself.
	REQUIRE(outerLine < innerLine);
	REQUIRE(innerLine < ir.locations.at(accumulation).line);

	// A region is an inlined frame, and a debugger stopped in one looks for
	// variables in *its* scope tree. Scope them all to the enclosing function
	// and `info locals` comes back empty inside every region.
	REQUIRE_FALSE(ir.variableNamesIn(accumulateId).empty());
	REQUIRE_FALSE(ir.variableNamesIn(aggIfId).empty());
}

TEST_CASE("Debug info: region() nesting survives to arbitrary depth") {
	// Regression test for #467. A region chain used to be encoded by fusing each
	// level's NameLoc with its parent's chain, which cannot express more than two
	// levels at all: FusedLoc::get() decomposes a nested FusedLoc whose metadata
	// equals the metadata being built, so fuse(lvl4, fuse(lvl3, ...)) collapsed
	// into one flat FusedLoc that no longer matched the shape the reader
	// recognised. The reader then reported "no region here" -- and every level's
	// scope, not just the ones past the boundary, was silently dropped from the
	// emitted DWARF.
	//
	// The encoding is a CallSiteLoc chain now (RegionScopeInfo.hpp), which MLIR
	// does not canonicalize, so depth is bounded only by the source.
	auto ir = compileDebugIr("execute", [](NautilusEngine& engine) {
		auto fn = engine.registerFunction(debugDeeplyNestedRegions);
		REQUIRE(fn(5) == 10);
	});

	std::vector<std::string> expected;
	for (const auto* name : {"lvl4", "lvl3", "lvl2", "lvl1"}) {
		const auto id = ir.subprogramId(name);
		INFO("DISubprogram for region " << name);
		REQUIRE_FALSE(id.empty());
		expected.push_back(id);
	}
	const auto executeId = ir.subprogramId("execute");
	REQUIRE_FALSE(executeId.empty());
	expected.push_back(executeId);

	// Innermost first, every level present and in order: that is what makes `bt`
	// read lvl4 / lvl3 / lvl2 / lvl1 / execute. A chain that lost a level would
	// still produce a plausible-looking backtrace, just a shorter one.
	const auto accumulation = ir.dbgIdOf("execute", {"add i32 %", ", %"});
	REQUIRE_FALSE(accumulation.empty());
	REQUIRE(ir.frameScopes(accumulation) == expected);

	// Each frame is located where the frame below it opens, so lines do not
	// decrease from the outermost region inwards. Not *strictly*: these four
	// regions open back to back with nothing traced between them, so every
	// enclosing frame points at the same first line inside -- what matters is
	// that the frames exist and are ordered, not that each contributes a
	// distinct line.
	std::vector<int> lines;
	for (std::string id = accumulation; !id.empty(); id = ir.locations.at(id).inlinedAtId) {
		lines.push_back(ir.locations.at(id).line);
	}
	REQUIRE(lines.size() == expected.size());
	INFO("frame lines, innermost first: " << [&] {
		std::string all;
		for (int line : lines) {
			all += std::to_string(line) + " ";
		}
		return all;
	}());
	REQUIRE(std::is_sorted(lines.rbegin(), lines.rend()));
	// `line: 0` means "compiler-generated, no source position"; a frame that
	// came out 0 would send a debugger to the call site instead.
	REQUIRE(std::all_of(lines.begin(), lines.end(), [](int line) { return line != 0; }));
	// The accumulation is really inside the regions, not level with where the
	// outermost one opens.
	REQUIRE(lines.front() > lines.back());
}

TEST_CASE("Debug info: 50 levels of region() nesting produce 50 DWARF frames") {
	// The test above pins that the two-level boundary #467 reported is gone. This one pins that no
	// new boundary exists anywhere a program could reach: the encoding is recursive, so 50 levels
	// have to behave exactly like 2 -- 50 synthetic subprograms, and an inlinedAt chain 50 frames
	// deep, innermost first, with nothing dropped in between.
	auto ir = compileDebugIr("execute", [](NautilusEngine& engine) {
		auto fn = engine.registerFunction(debugVeryDeeplyNestedRegions);
		REQUIRE(fn(5) == 10);
	});

	std::vector<std::string> expected;
	for (int level = 1; level <= kDebugNestDepth; ++level) {
		const auto id = ir.subprogramId(debugNestedRegionName(level));
		INFO("DISubprogram for region " << debugNestedRegionName(level));
		REQUIRE_FALSE(id.empty());
		expected.push_back(id);
	}
	const auto executeId = ir.subprogramId("execute");
	REQUIRE_FALSE(executeId.empty());
	expected.push_back(executeId);

	const auto accumulation = ir.dbgIdOf("execute", {"add i32 %", ", %"});
	REQUIRE_FALSE(accumulation.empty());
	const auto scopes = ir.frameScopes(accumulation);
	// Report where a truncated chain stopped rather than just that it did not match.
	INFO("chain is " << scopes.size() << " frames, expected " << expected.size());
	REQUIRE(scopes == expected);

	// `line: 0` means "compiler-generated, no source position". One such frame anywhere in a
	// 50-deep chain would send a debugger to the wrong place for that level.
	for (const auto& id : ir.locations) {
		INFO("DILocation !" << id.first);
		REQUIRE(id.second.line != 0);
	}
}

TEST_CASE("Debug info: the entry block's scope is the function's own, not a variable's decl line") {
	// The prologue's shadow allocas each carry the line of the variable they
	// stand for, so the entry block's DILexicalBlock must not be derived from
	// its first op. DILexicalBlock is uniqued by (scope, file, line, column):
	// give the entry block some variable's decl line and it merges with the
	// block that really starts there, moving every entry-block variable into a
	// scope whose PC range lies elsewhere -- which is what "no variable data
	// available" looks like in a debugger.
	auto ir = compileDebugIr("execute", [](NautilusEngine& engine) {
		auto fn = engine.registerFunction(debugNestedControlFlow);
		// odd counts 1, 3 and 5; no even value is greater than 4.
		REQUIRE(fn(6) == 3);
	});

	const auto executeId = ir.subprogramId("execute");
	REQUIRE_FALSE(executeId.empty());
	// Only execute's own blocks matter: the `_mlir_ciface_` wrapper in the same
	// module has an entry block too, and counting it would let the bug through.
	auto blockLines = ir.lexicalBlockLinesOf(executeId);
	REQUIRE(blockLines.size() > 1);

	// Exactly one of them -- the entry block -- sits on the function's line.
	INFO("execute's lexical block lines must include the function line " << ir.subprograms.at(executeId).line);
	REQUIRE(std::count(blockLines.begin(), blockLines.end(), ir.subprograms.at(executeId).line) == 1);

	// And no two share a line, i.e. the attribute uniquing merged no scopes.
	std::sort(blockLines.begin(), blockLines.end());
	REQUIRE(std::adjacent_find(blockLines.begin(), blockLines.end()) == blockLines.end());
}

TEST_CASE("Debug info: multi-function module emits a DISubprogram + scopes per function") {
	// A CompiledModule built from several functions lowers to one MLIR module
	// with a `func.func` each. Every one must surface as its own DISubprogram
	// with its own lexical blocks, so scopes and variables cannot leak between
	// functions.
	auto ir = compileDebugIr("mod_add", [](NautilusEngine& engine) {
		auto module = engine.createModule();
		module.registerFunction("mod_add", debugModAdd);
		module.registerFunction("mod_mul", debugModMul);
		module.registerFunction("mod_loop", debugModLoop);
		auto compiled = module.compile();
		REQUIRE(compiled.getFunction<int32_t(int32_t)>("mod_add")(41) == 42);
		REQUIRE(compiled.getFunction<int32_t(int32_t, int32_t)>("mod_mul")(3, 4) == 13);
		REQUIRE(compiled.getFunction<int32_t(int32_t)>("mod_loop")(5) == 10);
	});

	// One subprogram per user function, ignoring the `_mlir_ciface_*` wrappers
	// convert-func-to-llvm synthesizes alongside each.
	for (const auto& name : {"mod_add", "mod_mul", "mod_loop"}) {
		INFO("expected a DISubprogram for " << name);
		REQUIRE_FALSE(ir.subprogramId(name).empty());
	}

	// Every subprogram, wrappers included, needs a real line or GDB cannot land
	// inside the function on `step`.
	for (const auto& [id, subprogram] : ir.subprograms) {
		INFO("DISubprogram !" << id << " (" << subprogram.name << ")");
		REQUIRE(subprogram.line != 0);
	}

	// Each lexical block belongs to exactly one of those subprograms...
	REQUIRE_FALSE(ir.lexicalBlocks.empty());
	for (const auto& [id, block] : ir.lexicalBlocks) {
		INFO("DILexicalBlock !" << id);
		REQUIRE_FALSE(ir.owningSubprogram(block.scopeId).empty());
	}

	// ...and the multi-block `mod_loop` contributes more than one, which is
	// per-block scoping working independently across module functions.
	REQUIRE(ir.lexicalBlockLinesOf(ir.subprogramId("mod_loop")).size() >= 2);
}

TEST_CASE("Debug info: one Nautilus function calling another gets per-function debug info") {
	// A NautilusFunction callee lowers to its own `func.func` in the same
	// module, so caller and callee each get a DISubprogram of their own.
	auto ir = compileDebugIr("execute", [](NautilusEngine& engine) {
		auto fn = engine.registerFunction(debugCaller);
		REQUIRE(fn(3, 4) == 22);
	});

	const auto callerId = ir.subprogramId("execute");
	const auto calleeId = ir.subprogramId("debug_helper");
	REQUIRE_FALSE(callerId.empty());
	REQUIRE_FALSE(calleeId.empty());

	for (const auto& [id, subprogram] : ir.subprograms) {
		INFO("DISubprogram !" << id << " (" << subprogram.name << ")");
		REQUIRE(subprogram.line != 0);
	}

	// The call instruction belongs to the CALLER's scope, not the callee's:
	// the callee's frame only begins inside debug_helper itself.
	const auto callDbgId = ir.dbgIdOf("execute", {"call ", "@debug_helper"});
	REQUIRE_FALSE(callDbgId.empty());
	const auto scopes = ir.frameScopes(callDbgId);
	REQUIRE_FALSE(scopes.empty());
	REQUIRE(ir.owningSubprogram(scopes.front()) == callerId);
}

TEST_CASE("Debug info: block terminators carry non-zero !dbg lines") {
	// A terminator (`br`, `cond_br`, `return`) has no `$N = ...` line of its
	// own in the IR dump, so it is the op most likely to end up with line 0 --
	// which DWARF reads as "no location" and GDB's `step` silently skips.
	auto ir = compileDebugIr("execute", [](NautilusEngine& engine) {
		auto fn = engine.registerFunction(debugNestedControlFlow);
		REQUIRE(fn(10) == 19);
	});

	auto terminators = ir.dbgIdsStartingWith("execute", "br ");
	const auto returns = ir.dbgIdsStartingWith("execute", "ret ");
	terminators.insert(terminators.end(), returns.begin(), returns.end());
	// The fixture branches both ways and returns, so there is plenty to check.
	REQUIRE(terminators.size() >= 3);
	for (const auto& dbgId : terminators) {
		INFO("terminator !dbg !" << dbgId);
		REQUIRE(ir.locations.count(dbgId) == 1);
		REQUIRE(ir.locations.at(dbgId).line != 0);
	}
}
} // namespace nautilus::engine

#endif // ENABLE_TRACING && ENABLE_MLIR_BACKEND

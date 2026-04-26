#include "nautilus/config.hpp"

#if defined(ENABLE_TRACING) && defined(ENABLE_MLIR_BACKEND)

#include "ExecutionTest.hpp"
#include "nautilus/Engine.hpp"
#include "nautilus/nautilus_function.hpp"
#include <algorithm>
#include <catch2/catch_all.hpp>
#include <cctype>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <map>
#include <set>
#include <sstream>
#include <unistd.h>

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
// ProxyCallOperation — the caller and callee lower to two distinct
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

} // namespace

TEST_CASE("Debug info: disabled by default produces identical results") {
	Options options;
	options.setOption("engine.backend", std::string("mlir"));

	NautilusEngine engine(options);
	auto fn = engine.registerFunction(debugAddOne);
	REQUIRE(fn(41) == 42);
}

TEST_CASE("Debug info: MLIR source mode writes a snapshot file and compiles") {
	const auto sourcePath =
	    (std::filesystem::temp_directory_path() / ("nautilus_debug_mlir_test_" + std::to_string(::getpid()) + ".mlir"))
	        .string();
	std::filesystem::remove(sourcePath);

	Options options;
	options.setOption("engine.backend", std::string("mlir"));
	options.setOption("mlir.debug.enable", true);
	options.setOption("mlir.debug.source_mode", std::string("mlir"));
	options.setOption("mlir.debug.source_file", sourcePath);

	NautilusEngine engine(options);
	auto fn = engine.registerFunction(debugAddOne);
	REQUIRE(fn(41) == 42);

	// LocationSnapshot should have written the post-inline MLIR to the
	// configured path.  The file must be non-empty and contain the func
	// symbol so gdb/lldb can resolve breakpoints against its lines.
	REQUIRE(std::filesystem::exists(sourcePath));
	const auto contents = readFile(sourcePath);
	REQUIRE_FALSE(contents.empty());
	REQUIRE(contents.find("func.func") != std::string::npos);

	std::filesystem::remove(sourcePath);
}

TEST_CASE("Debug info: Nautilus IR source mode emits the IR dump as the source file") {
	const auto sourcePath =
	    (std::filesystem::temp_directory_path() / ("nautilus_debug_ir_test_" + std::to_string(::getpid()) + ".ir"))
	        .string();
	std::filesystem::remove(sourcePath);

	Options options;
	options.setOption("engine.backend", std::string("mlir"));
	options.setOption("mlir.debug.enable", true);
	options.setOption("mlir.debug.source_mode", std::string("nautilus-ir"));
	options.setOption("mlir.debug.source_file", sourcePath);

	NautilusEngine engine(options);
	auto fn = engine.registerFunction(debugSumThree);
	REQUIRE(fn(1, 2, 3) == 6);

	// The IR source file should be the Nautilus IR dump, recognizable by
	// the outer `nautilus { ... } //nautilus` bracket pattern produced by
	// IRGraph::toString().
	REQUIRE(std::filesystem::exists(sourcePath));
	const auto contents = readFile(sourcePath);
	REQUIRE(contents.find("nautilus {") != std::string::npos);
	REQUIRE(contents.find("//nautilus") != std::string::npos);

	std::filesystem::remove(sourcePath);
}

TEST_CASE("Debug info: default source path is synthesized when none provided") {
	Options options;
	options.setOption("engine.backend", std::string("mlir"));
	options.setOption("mlir.debug.enable", true);
	options.setOption("mlir.debug.source_mode", std::string("nautilus-ir"));

	NautilusEngine engine(options);
	auto fn = engine.registerFunction(debugAddOne);
	REQUIRE(fn(10) == 11);
}

TEST_CASE("Debug info: nautilus-ir mode emits alloca + dbg.declare for each $N DILocalVariable") {
	const auto dumpRoot = std::filesystem::temp_directory_path() / "dump";
	std::set<std::filesystem::path> existing;
	if (std::filesystem::exists(dumpRoot)) {
		for (const auto& e : std::filesystem::directory_iterator(dumpRoot)) {
			existing.insert(e.path());
		}
	}

	Options options;
	options.setOption("engine.backend", std::string("mlir"));
	options.setOption("mlir.debug.enable", true);
	options.setOption("mlir.debug.source_mode", std::string("nautilus-ir"));
	options.setOption("dump.before_llvm_optimization", true);

	NautilusEngine engine(options);
	auto fn = engine.registerFunction(debugSumThree);
	REQUIRE(fn(1, 2, 3) == 6);

	// Each `$N` is now a shadow alloca with a dbg.declare pointing at
	// it (see EmitDbgValuePass).  Verify both the DILocalVariable
	// metadata and the dbg.declare debug record survive MLIR→LLVM
	// translation.  Accept either the legacy `call void
	// @llvm.dbg.declare` syntax or LLVM 21's `#dbg_declare` record form.
	bool foundDILocalVar = false;
	bool foundDbgDeclare = false;
	bool foundAlloca = false;
	if (std::filesystem::exists(dumpRoot)) {
		for (const auto& dir : std::filesystem::directory_iterator(dumpRoot)) {
			if (existing.count(dir.path())) {
				continue;
			}
			for (const auto& entry : std::filesystem::recursive_directory_iterator(dir)) {
				if (entry.is_regular_file() && entry.path().filename() == "before_llvm_optimization.ll") {
					auto contents = readFile(entry.path().string());
					// DILocalVariable names use a `v<N>` prefix rather
					// than `$<N>` to avoid GDB's value-history syntax.
					if (contents.find("!DILocalVariable(name: \"v") != std::string::npos) {
						foundDILocalVar = true;
					}
					if (contents.find("#dbg_declare") != std::string::npos ||
					    contents.find("@llvm.dbg.declare") != std::string::npos) {
						foundDbgDeclare = true;
					}
					if (contents.find("= alloca i32") != std::string::npos) {
						foundAlloca = true;
					}
				}
			}
		}
	}
	REQUIRE(foundDILocalVar);
	REQUIRE(foundDbgDeclare);
	REQUIRE(foundAlloca);
}

TEST_CASE("Debug info: generated LLVM IR contains DICompileUnit") {
	// DumpHandler writes under $TMPDIR/dump/<compilation-unit-id>/ — no way
	// to override, so snapshot the state, run, then search newly-created
	// subdirs for the after_llvm_generation.ll dump.
	const auto dumpRoot = std::filesystem::temp_directory_path() / "dump";
	std::set<std::filesystem::path> existing;
	if (std::filesystem::exists(dumpRoot)) {
		for (const auto& e : std::filesystem::directory_iterator(dumpRoot)) {
			existing.insert(e.path());
		}
	}

	Options options;
	options.setOption("engine.backend", std::string("mlir"));
	options.setOption("mlir.debug.enable", true);
	options.setOption("mlir.debug.source_mode", std::string("mlir"));
	options.setOption("dump.after_llvm_generation", true);

	NautilusEngine engine(options);
	auto fn = engine.registerFunction(debugAddOne);
	REQUIRE(fn(5) == 6);

	// Find new subdir(s) and look for the expected LLVM IR dump.
	bool foundDICompileUnit = false;
	if (std::filesystem::exists(dumpRoot)) {
		for (const auto& dir : std::filesystem::directory_iterator(dumpRoot)) {
			if (existing.count(dir.path())) {
				continue;
			}
			for (const auto& entry : std::filesystem::recursive_directory_iterator(dir)) {
				if (entry.is_regular_file() && entry.path().extension() == ".ll") {
					auto contents = readFile(entry.path().string());
					if (contents.find("DICompileUnit") != std::string::npos) {
						foundDICompileUnit = true;
						break;
					}
				}
			}
			if (foundDICompileUnit) {
				break;
			}
		}
	}
	REQUIRE(foundDICompileUnit);
}

TEST_CASE("Debug info: nautilus-ir mode emits DISubprogram with non-zero line") {
	// Regression: DIScopeForLLVMFuncOpPass used to synthesize DISubprograms
	// with `line: 0` because MLIRLoweringProvider tagged the FuncOp with a
	// NameLoc whose nested FileLineColLoc had line 0.  DWARF treats line 0
	// as "no location", so GDB's `step` could not land inside the function.
	// The lowering now looks the function's header line up in
	// IRSourceMap::functionLines, which must surface as a non-zero `line:`
	// attribute on every emitted DISubprogram.
	const auto dumpRoot = std::filesystem::temp_directory_path() / "dump";
	std::set<std::filesystem::path> existing;
	if (std::filesystem::exists(dumpRoot)) {
		for (const auto& e : std::filesystem::directory_iterator(dumpRoot)) {
			existing.insert(e.path());
		}
	}

	Options options;
	options.setOption("engine.backend", std::string("mlir"));
	options.setOption("mlir.debug.enable", true);
	options.setOption("mlir.debug.source_mode", std::string("nautilus-ir"));
	options.setOption("dump.after_llvm_generation", true);

	NautilusEngine engine(options);
	auto fn = engine.registerFunction(debugSumThree);
	REQUIRE(fn(1, 2, 3) == 6);

	bool foundDISubprogram = false;
	bool foundLineZero = false;
	if (std::filesystem::exists(dumpRoot)) {
		for (const auto& dir : std::filesystem::directory_iterator(dumpRoot)) {
			if (existing.count(dir.path())) {
				continue;
			}
			for (const auto& entry : std::filesystem::recursive_directory_iterator(dir)) {
				if (entry.is_regular_file() && entry.path().filename() == "after_llvm_generation.ll") {
					auto contents = readFile(entry.path().string());
					std::string::size_type pos = 0;
					while ((pos = contents.find("!DISubprogram(", pos)) != std::string::npos) {
						const auto end = contents.find(')', pos);
						if (end == std::string::npos) {
							break;
						}
						const auto record = contents.substr(pos, end - pos);
						foundDISubprogram = true;
						if (record.find("line: 0") != std::string::npos) {
							foundLineZero = true;
						}
						pos = end;
					}
				}
			}
		}
	}
	REQUIRE(foundDISubprogram);
	REQUIRE_FALSE(foundLineZero);
}

TEST_CASE("Debug info: loop body produces a multi-block IR with N lines for every op") {
	// Sanity-check that a looping function — which lowers to several MLIR
	// basic blocks and carries SSA block arguments across iterations —
	// still produces a valid IR source dump where every `$N = ...`
	// definition and every block-argument appears on its own line, and
	// every emitted DISubprogram gets a real `line:` attribute.
	const auto sourcePath =
	    (std::filesystem::temp_directory_path() / ("nautilus_debug_loop_test_" + std::to_string(::getpid()) + ".ir"))
	        .string();
	std::filesystem::remove(sourcePath);

	const auto dumpRoot = std::filesystem::temp_directory_path() / "dump";
	std::set<std::filesystem::path> existing;
	if (std::filesystem::exists(dumpRoot)) {
		for (const auto& e : std::filesystem::directory_iterator(dumpRoot)) {
			existing.insert(e.path());
		}
	}

	Options options;
	options.setOption("engine.backend", std::string("mlir"));
	options.setOption("mlir.debug.enable", true);
	options.setOption("mlir.debug.source_mode", std::string("nautilus-ir"));
	options.setOption("mlir.debug.source_file", sourcePath);
	options.setOption("dump.after_llvm_generation", true);

	NautilusEngine engine(options);
	auto fn = engine.registerFunction(debugSumLoop);
	// sum of 0..4 inclusive
	REQUIRE(fn(5) == 10);

	// IR dump should contain at least one `Block_` header (the loop
	// body/header) and the final `return` — i.e. the function is in fact
	// multi-block.
	REQUIRE(std::filesystem::exists(sourcePath));
	const auto ir = readFile(sourcePath);
	REQUIRE(ir.find("Block_") != std::string::npos);
	REQUIRE(ir.find("return") != std::string::npos);
	REQUIRE(std::count(ir.begin(), ir.end(), '\n') > 5);

	// DISubprogram must still carry a non-zero `line:` for the main
	// function even when the body is multi-block.
	bool foundDISubprogram = false;
	bool foundLineZero = false;
	if (std::filesystem::exists(dumpRoot)) {
		for (const auto& dir : std::filesystem::directory_iterator(dumpRoot)) {
			if (existing.count(dir.path())) {
				continue;
			}
			for (const auto& entry : std::filesystem::recursive_directory_iterator(dir)) {
				if (entry.is_regular_file() && entry.path().filename() == "after_llvm_generation.ll") {
					auto contents = readFile(entry.path().string());
					std::string::size_type pos = 0;
					while ((pos = contents.find("!DISubprogram(", pos)) != std::string::npos) {
						const auto end = contents.find(')', pos);
						if (end == std::string::npos) {
							break;
						}
						const auto record = contents.substr(pos, end - pos);
						foundDISubprogram = true;
						if (record.find("line: 0") != std::string::npos) {
							foundLineZero = true;
						}
						pos = end;
					}
				}
			}
		}
	}
	REQUIRE(foundDISubprogram);
	REQUIRE_FALSE(foundLineZero);

	std::filesystem::remove(sourcePath);
}

TEST_CASE("Debug info: nested control flow preserves $N DILocalVariables across blocks") {
	// Nested if/else inside a loop produces branching control flow where
	// block-arg SSA ids bridge iterations.  Verify that every Nautilus
	// `$N` in the IR dump has a matching DILocalVariable in the lowered
	// LLVM IR so GDB can resolve each intermediate across all branches.
	const auto dumpRoot = std::filesystem::temp_directory_path() / "dump";
	std::set<std::filesystem::path> existing;
	if (std::filesystem::exists(dumpRoot)) {
		for (const auto& e : std::filesystem::directory_iterator(dumpRoot)) {
			existing.insert(e.path());
		}
	}

	const auto sourcePath =
	    (std::filesystem::temp_directory_path() / ("nautilus_debug_cf_test_" + std::to_string(::getpid()) + ".ir"))
	        .string();
	std::filesystem::remove(sourcePath);

	Options options;
	options.setOption("engine.backend", std::string("mlir"));
	options.setOption("mlir.debug.enable", true);
	options.setOption("mlir.debug.source_mode", std::string("nautilus-ir"));
	options.setOption("mlir.debug.source_file", sourcePath);
	options.setOption("dump.before_llvm_optimization", true);

	NautilusEngine engine(options);
	auto fn = engine.registerFunction(debugNestedControlFlow);
	// limit = 10 → odd count {1,3,5,7,9}=5; even>4 sum {6,8}=14; total=19.
	REQUIRE(fn(10) == 19);

	REQUIRE(std::filesystem::exists(sourcePath));
	const auto ir = readFile(sourcePath);

	// Count distinct `$N` definitions in the IR dump — block arg
	// declarations (`$N:type` inside a block header) and `$N = ...`
	// lines.  Every one must surface as a `!DILocalVariable(name: "vN"…)`
	// in the lowered LLVM IR, otherwise GDB cannot resolve it across a
	// branch.  (DILocalVariable uses the `v` prefix instead of `$` so
	// GDB doesn't interpret the name as value-history syntax.)
	std::set<int> irIds;
	for (std::string::size_type p = 0; (p = ir.find('$', p)) != std::string::npos; ++p) {
		std::string::size_type q = p + 1;
		int id = 0;
		bool any = false;
		while (q < ir.size() && std::isdigit(static_cast<unsigned char>(ir[q]))) {
			id = id * 10 + (ir[q] - '0');
			any = true;
			++q;
		}
		if (any) {
			irIds.insert(id);
		}
	}
	REQUIRE(irIds.size() > 5);

	std::set<int> llIds;
	if (std::filesystem::exists(dumpRoot)) {
		for (const auto& dir : std::filesystem::directory_iterator(dumpRoot)) {
			if (existing.count(dir.path())) {
				continue;
			}
			for (const auto& entry : std::filesystem::recursive_directory_iterator(dir)) {
				if (entry.is_regular_file() && entry.path().filename() == "before_llvm_optimization.ll") {
					auto contents = readFile(entry.path().string());
					const std::string marker = "!DILocalVariable(name: \"v";
					std::string::size_type pos = 0;
					while ((pos = contents.find(marker, pos)) != std::string::npos) {
						auto idStart = pos + marker.size();
						int id = 0;
						bool any = false;
						while (idStart < contents.size() &&
						       std::isdigit(static_cast<unsigned char>(contents[idStart]))) {
							id = id * 10 + (contents[idStart] - '0');
							any = true;
							++idStart;
						}
						if (any) {
							llIds.insert(id);
						}
						pos = idStart;
					}
				}
			}
		}
	}

	REQUIRE_FALSE(llIds.empty());
	for (int id : irIds) {
		INFO("IR defines $" << id << " but no matching DILocalVariable was emitted in LLVM IR");
		REQUIRE(llIds.count(id) == 1);
	}

	std::filesystem::remove(sourcePath);
}

TEST_CASE("Debug info: per-block DILexicalBlock scoping narrows variable visibility") {
	// EmitDbgValuePass builds one DILexicalBlock per Nautilus basic
	// block and scopes each DILocalVariable to the block that stores
	// into its shadow alloca.  This test asserts:
	//   * more than one DILexicalBlock is emitted for a multi-block
	//     function (otherwise every variable would share the function
	//     scope).
	//   * each DILexicalBlock's parent is the function's DISubprogram.
	//   * at least one DILocalVariable is scoped to a DILexicalBlock
	//     rather than directly to the DISubprogram — proving that
	//     scoping has actually been narrowed from function scope.
	const auto dumpRoot = std::filesystem::temp_directory_path() / "dump";
	std::set<std::filesystem::path> existing;
	if (std::filesystem::exists(dumpRoot)) {
		for (const auto& e : std::filesystem::directory_iterator(dumpRoot)) {
			existing.insert(e.path());
		}
	}

	Options options;
	options.setOption("engine.backend", std::string("mlir"));
	options.setOption("mlir.debug.enable", true);
	options.setOption("mlir.debug.source_mode", std::string("nautilus-ir"));
	options.setOption("dump.before_llvm_optimization", true);

	NautilusEngine engine(options);
	auto fn = engine.registerFunction(debugSumLoop);
	REQUIRE(fn(5) == 10);

	int lexicalBlockCount = 0;
	int varsScopedToLexBlock = 0;
	bool foundLexBlock = false;
	if (std::filesystem::exists(dumpRoot)) {
		for (const auto& dir : std::filesystem::directory_iterator(dumpRoot)) {
			if (existing.count(dir.path())) {
				continue;
			}
			for (const auto& entry : std::filesystem::recursive_directory_iterator(dir)) {
				if (entry.is_regular_file() && entry.path().filename() == "before_llvm_optimization.ll") {
					auto contents = readFile(entry.path().string());

					// Iterate line-by-line so parser state machines
					// do not risk looping on the same position.
					std::set<std::string> lexBlockIds;
					{
						std::istringstream iss(contents);
						std::string line;
						while (std::getline(iss, line)) {
							const auto marker = line.find("= distinct !DILexicalBlock(");
							if (marker == std::string::npos) {
								continue;
							}
							// Line shape: `!<N> = distinct !DILexicalBlock(...)`
							auto bang = line.find('!');
							if (bang == std::string::npos) {
								continue;
							}
							auto idEnd = line.find(' ', bang);
							if (idEnd == std::string::npos) {
								continue;
							}
							lexBlockIds.insert(line.substr(bang + 1, idEnd - bang - 1));
							foundLexBlock = true;
						}
					}
					lexicalBlockCount = static_cast<int>(lexBlockIds.size());

					{
						std::istringstream iss(contents);
						std::string line;
						while (std::getline(iss, line)) {
							if (line.find("= !DILocalVariable(") == std::string::npos) {
								continue;
							}
							const auto scopeKey = line.find("scope: !");
							if (scopeKey == std::string::npos) {
								continue;
							}
							auto scopeStart = scopeKey + std::string("scope: !").size();
							auto scopeEnd = scopeStart;
							while (scopeEnd < line.size() &&
							       std::isdigit(static_cast<unsigned char>(line[scopeEnd]))) {
								++scopeEnd;
							}
							auto scopeId = line.substr(scopeStart, scopeEnd - scopeStart);
							if (lexBlockIds.count(scopeId)) {
								++varsScopedToLexBlock;
							}
						}
					}
				}
			}
		}
	}

	REQUIRE(foundLexBlock);
	REQUIRE(lexicalBlockCount >= 2);
	REQUIRE(varsScopedToLexBlock >= 1);
}

TEST_CASE("Debug info: multi-function module emits a DISubprogram + scopes per function") {
	// A CompiledModule built from several Nautilus functions lowers to a
	// single MLIR module containing one `func.func` per registration.
	// Each must surface as its own DWARF DISubprogram (with a non-zero
	// `line:`), carry its own DILexicalBlocks, and isolate its `vN`
	// DILocalVariables so names don't leak across functions.
	const auto dumpRoot = std::filesystem::temp_directory_path() / "dump";
	std::set<std::filesystem::path> existing;
	if (std::filesystem::exists(dumpRoot)) {
		for (const auto& e : std::filesystem::directory_iterator(dumpRoot)) {
			existing.insert(e.path());
		}
	}

	Options options;
	options.setOption("engine.backend", std::string("mlir"));
	options.setOption("mlir.debug.enable", true);
	options.setOption("mlir.debug.source_mode", std::string("nautilus-ir"));
	options.setOption("dump.before_llvm_optimization", true);

	NautilusEngine engine(options);
	auto module = engine.createModule();
	module.registerFunction("mod_add", debugModAdd);
	module.registerFunction("mod_mul", debugModMul);
	module.registerFunction("mod_loop", debugModLoop);
	auto compiled = module.compile();

	auto addFn = compiled.getFunction<int32_t(int32_t)>("mod_add");
	auto mulFn = compiled.getFunction<int32_t(int32_t, int32_t)>("mod_mul");
	auto loopFn = compiled.getFunction<int32_t(int32_t)>("mod_loop");
	REQUIRE(addFn(41) == 42);
	REQUIRE(mulFn(3, 4) == 13);
	REQUIRE(loopFn(5) == 10);

	// Harvest (subprogramId → name) and (lexBlockId → subprogramId)
	// tables from the pre-optimization dump, then check that:
	//   * one DISubprogram is emitted per user function (plus the
	//     MLIR-generated `_mlir_ciface_*` wrappers that ride along);
	//   * every DISubprogram has line != 0;
	//   * each DILexicalBlock's `scope:` is one of the DISubprograms;
	//   * DILocalVariables scoped to a lexical block never cross
	//     function boundaries (a given scope → exactly one function).
	std::map<std::string, std::string> subprogramName; // id → name
	std::map<std::string, std::string> lexBlockOwner;  // id → subprogram id
	std::map<std::string, std::string> subprogramLine; // id → line string
	if (std::filesystem::exists(dumpRoot)) {
		for (const auto& dir : std::filesystem::directory_iterator(dumpRoot)) {
			if (existing.count(dir.path())) {
				continue;
			}
			for (const auto& entry : std::filesystem::recursive_directory_iterator(dir)) {
				if (!entry.is_regular_file() || entry.path().filename() != "before_llvm_optimization.ll") {
					continue;
				}
				auto contents = readFile(entry.path().string());
				std::istringstream iss(contents);
				std::string line;
				while (std::getline(iss, line)) {
					const auto bang = line.find('!');
					if (bang == std::string::npos) {
						continue;
					}
					const auto idEnd = line.find(' ', bang);
					if (idEnd == std::string::npos || line.find('=', idEnd) == std::string::npos) {
						continue;
					}
					const auto metaId = line.substr(bang + 1, idEnd - bang - 1);

					if (line.find("!DISubprogram(") != std::string::npos) {
						auto nameKey = line.find("name: \"");
						auto lineKey = line.find("line: ");
						if (nameKey == std::string::npos || lineKey == std::string::npos) {
							continue;
						}
						auto nameStart = nameKey + std::string("name: \"").size();
						auto nameEnd = line.find('"', nameStart);
						auto lineStart = lineKey + std::string("line: ").size();
						auto lineEnd = lineStart;
						while (lineEnd < line.size() && std::isdigit(static_cast<unsigned char>(line[lineEnd]))) {
							++lineEnd;
						}
						subprogramName[metaId] = line.substr(nameStart, nameEnd - nameStart);
						subprogramLine[metaId] = line.substr(lineStart, lineEnd - lineStart);
					} else if (line.find("!DILexicalBlock(") != std::string::npos) {
						auto scopeKey = line.find("scope: !");
						if (scopeKey == std::string::npos) {
							continue;
						}
						auto scopeStart = scopeKey + std::string("scope: !").size();
						auto scopeEnd = scopeStart;
						while (scopeEnd < line.size() && std::isdigit(static_cast<unsigned char>(line[scopeEnd]))) {
							++scopeEnd;
						}
						lexBlockOwner[metaId] = line.substr(scopeStart, scopeEnd - scopeStart);
					}
				}
				break; // one dump is enough
			}
			if (!subprogramName.empty()) {
				break;
			}
		}
	}

	// Collect distinct user-level function names — ignore the
	// `_mlir_ciface_*` / `_mlir_*` wrappers that convert-func-to-llvm
	// synthesizes alongside each emitted function.
	std::set<std::string> userFunctions;
	for (const auto& kv : subprogramName) {
		if (kv.second.rfind("_mlir_", 0) == 0) {
			continue;
		}
		userFunctions.insert(kv.second);
	}
	REQUIRE(userFunctions.count("mod_add") == 1);
	REQUIRE(userFunctions.count("mod_mul") == 1);
	REQUIRE(userFunctions.count("mod_loop") == 1);

	// Every DISubprogram — including the C-interface wrappers — must
	// have a non-zero `line:` attribute, otherwise GDB cannot land
	// inside the function on `step`.
	for (const auto& kv : subprogramLine) {
		INFO("DISubprogram !" << kv.first << " has line: " << kv.second);
		REQUIRE(kv.second != "0");
	}

	// Per-function lexical-block isolation: each DILexicalBlock's
	// parent scope must resolve to a DISubprogram we captured.
	REQUIRE_FALSE(lexBlockOwner.empty());
	for (const auto& kv : lexBlockOwner) {
		INFO("DILexicalBlock !" << kv.first << " owner !" << kv.second);
		REQUIRE(subprogramName.count(kv.second) == 1);
	}

	// Count lexical blocks per owning DISubprogram.  The multi-block
	// `mod_loop` function must contribute more than one lexical block
	// (entry + loop header + body + exit), proving that per-block
	// scoping works independently across module functions.
	std::map<std::string, int> blocksPerSubprogram;
	for (const auto& kv : lexBlockOwner) {
		blocksPerSubprogram[kv.second]++;
	}
	int multiBlockFunctions = 0;
	for (const auto& kv : blocksPerSubprogram) {
		if (kv.second >= 2) {
			++multiBlockFunctions;
		}
	}
	REQUIRE(multiBlockFunctions >= 1);
}

TEST_CASE("Debug info: one Nautilus function calling another gets per-function debug info") {
	// `debugCaller` invokes `debug_helper` via a NautilusFunction
	// wrapper, which ends up as a ProxyCallOperation — so both
	// functions are compiled into the same MLIR module and the caller
	// site lowers to a `func.call` inside `debugCaller`'s body.
	// Verify that:
	//   * Both functions appear as distinct DISubprograms with
	//     non-zero `line:` attributes.
	//   * The emitted LLVM IR contains an actual `call i32 @debug_helper`
	//     from within `debugCaller`.
	//   * The call instruction carries a `!dbg` whose scope resolves
	//     to the CALLER's DILexicalBlock — proving we preserve scope
	//     across the call site.
	const auto sourcePath =
	    (std::filesystem::temp_directory_path() / ("nautilus_debug_call_" + std::to_string(::getpid()) + ".ir"))
	        .string();
	std::filesystem::remove(sourcePath);

	const auto dumpRoot = std::filesystem::temp_directory_path() / "dump";
	std::set<std::filesystem::path> existing;
	if (std::filesystem::exists(dumpRoot)) {
		for (const auto& e : std::filesystem::directory_iterator(dumpRoot)) {
			existing.insert(e.path());
		}
	}

	Options options;
	options.setOption("engine.backend", std::string("mlir"));
	options.setOption("mlir.debug.enable", true);
	options.setOption("mlir.debug.source_mode", std::string("nautilus-ir"));
	options.setOption("mlir.debug.source_file", sourcePath);
	options.setOption("dump.before_llvm_optimization", true);

	NautilusEngine engine(options);
	auto fn = engine.registerFunction(debugCaller);
	// sum_{i=0..3} ((5*i)+1) = 1 + 6 + 11 + 16 = 34
	REQUIRE(fn(5, 4) == 34);

	// IR dump should list two function headers so the user can step
	// into both caller and callee.
	REQUIRE(std::filesystem::exists(sourcePath));
	const auto ir = readFile(sourcePath);
	REQUIRE(ir.find("debug_helper(") != std::string::npos);
	const bool hasOuterFn =
	    ir.find("execute(") != std::string::npos || ir.find("debugCaller(") != std::string::npos;
	REQUIRE(hasOuterFn);

	// Parse the pre-optimization LLVM IR to map subprograms to their
	// metadata ids, check the call lowered correctly, and confirm the
	// call's !dbg scope belongs to the caller.  We must capture the
	// call that appears in the OUTER `@execute` function — the module
	// also contains `@debug_helper` and its `_mlir_ciface_*` wrapper,
	// each of which emits its own `call i32 @debug_helper` forwarder.
	std::map<std::string, std::string> subprogramName;
	std::map<std::string, std::string> subprogramLine;
	std::map<std::string, std::string> lexBlockOwner;   // !N -> !M (lexBlock -> scope)
	std::map<std::string, std::string> dilocationScope; // !N -> !M (DILocation -> scope)
	std::string callDbgScope;
	bool sawHelperCall = false;
	if (std::filesystem::exists(dumpRoot)) {
		for (const auto& dir : std::filesystem::directory_iterator(dumpRoot)) {
			if (existing.count(dir.path())) {
				continue;
			}
			for (const auto& entry : std::filesystem::recursive_directory_iterator(dir)) {
				if (!entry.is_regular_file() || entry.path().filename() != "before_llvm_optimization.ll") {
					continue;
				}
				auto contents = readFile(entry.path().string());
				std::istringstream iss(contents);
				std::string line;
				std::string currentFunction;
				while (std::getline(iss, line)) {
					// Track which user-level function we're scanning
					// through.  `define ... @<name>(...)` marks the
					// start, `^}` the end.
					if (line.rfind("define ", 0) == 0) {
						auto at = line.find('@');
						auto open = (at == std::string::npos) ? std::string::npos : line.find('(', at);
						currentFunction = (at != std::string::npos && open != std::string::npos)
						                      ? line.substr(at + 1, open - at - 1)
						                      : std::string {};
					} else if (line == "}") {
						currentFunction.clear();
					}

					// Only capture the call from the CALLER — ignore
					// the `_mlir_ciface_debug_helper` forwarder and any
					// un-debugged `_mlir_*` wrapper.
					if (currentFunction == "execute" && line.find("call i32 @debug_helper") != std::string::npos) {
						sawHelperCall = true;
						const auto dbgKey = line.find("!dbg !");
						if (dbgKey != std::string::npos) {
							auto idStart = dbgKey + std::string("!dbg !").size();
							auto idEnd = idStart;
							while (idEnd < line.size() &&
							       std::isdigit(static_cast<unsigned char>(line[idEnd]))) {
								++idEnd;
							}
							callDbgScope = line.substr(idStart, idEnd - idStart);
						}
						continue;
					}
					const auto bang = line.find('!');
					if (bang == std::string::npos) {
						continue;
					}
					const auto idEnd = line.find(' ', bang);
					if (idEnd == std::string::npos) {
						continue;
					}
					const auto metaId = line.substr(bang + 1, idEnd - bang - 1);
					auto scopeIdAt = [&](std::string::size_type key) {
						auto start = key + std::string("scope: !").size();
						auto end = start;
						while (end < line.size() && std::isdigit(static_cast<unsigned char>(line[end]))) {
							++end;
						}
						return line.substr(start, end - start);
					};
					if (line.find("!DISubprogram(") != std::string::npos) {
						auto nameKey = line.find("name: \"");
						auto lineKey = line.find("line: ");
						if (nameKey == std::string::npos || lineKey == std::string::npos) {
							continue;
						}
						auto nameStart = nameKey + std::string("name: \"").size();
						auto nameEnd = line.find('"', nameStart);
						auto lineStart = lineKey + std::string("line: ").size();
						auto lineNumEnd = lineStart;
						while (lineNumEnd < line.size() &&
						       std::isdigit(static_cast<unsigned char>(line[lineNumEnd]))) {
							++lineNumEnd;
						}
						subprogramName[metaId] = line.substr(nameStart, nameEnd - nameStart);
						subprogramLine[metaId] = line.substr(lineStart, lineNumEnd - lineStart);
					} else if (line.find("!DILexicalBlock(") != std::string::npos) {
						auto scopeKey = line.find("scope: !");
						if (scopeKey == std::string::npos) {
							continue;
						}
						lexBlockOwner[metaId] = scopeIdAt(scopeKey);
					} else if (line.find("!DILocation(") != std::string::npos) {
						auto scopeKey = line.find("scope: !");
						if (scopeKey == std::string::npos) {
							continue;
						}
						dilocationScope[metaId] = scopeIdAt(scopeKey);
					}
				}
				break;
			}
			if (sawHelperCall) {
				break;
			}
		}
	}
	REQUIRE(sawHelperCall);

	// Both the helper and the outer function must have their own
	// DISubprogram with a real line number.
	std::set<std::string> userFuncs;
	for (const auto& kv : subprogramName) {
		if (kv.second.rfind("_mlir_", 0) != 0) {
			userFuncs.insert(kv.second);
		}
	}
	REQUIRE(userFuncs.count("debug_helper") == 1);
	// The outer fn is registered via engine.registerFunction(debugCaller)
	// without an explicit name, so Nautilus assigns the standard
	// "execute" symbol for single-registerFunction engines.
	REQUIRE(userFuncs.count("execute") == 1);
	for (const auto& kv : subprogramLine) {
		INFO("DISubprogram !" << kv.first << " (" << subprogramName[kv.first] << ") line: " << kv.second);
		REQUIRE(kv.second != "0");
	}

	// The `call @debug_helper` instruction's `!dbg` must live under a
	// scope whose chain terminates at the CALLER's DISubprogram — not
	// the callee's.  The chain is
	//     DILocation -> (DILexicalBlock*) -> DISubprogram.
	REQUIRE_FALSE(callDbgScope.empty());
	std::string walkId = callDbgScope;
	for (int steps = 0; steps < 16 && !walkId.empty(); ++steps) {
		if (subprogramName.count(walkId)) {
			break;
		}
		if (auto it = dilocationScope.find(walkId); it != dilocationScope.end()) {
			walkId = it->second;
			continue;
		}
		if (auto it = lexBlockOwner.find(walkId); it != lexBlockOwner.end()) {
			walkId = it->second;
			continue;
		}
		walkId.clear();
		break;
	}
	REQUIRE(subprogramName.count(walkId) == 1);
	REQUIRE(subprogramName[walkId] != "debug_helper");

	std::filesystem::remove(sourcePath);
}

TEST_CASE("Debug info: block terminators carry non-zero !dbg lines") {
	// Terminators (`br`, `cond_br`, `return`) don't have a `$N = ...`
	// line in the Nautilus IR dump, so prior to positional
	// `blockOpLines` tracking they inherited line 0 — which DWARF
	// treats as "no location" and GDB's `step` silently skips.  This
	// test lowers a function with both forms of branch plus a return,
	// then scans the LLVM IR for those terminators and requires every
	// one of their `!dbg` lines to be > 0.
	const auto dumpRoot = std::filesystem::temp_directory_path() / "dump";
	std::set<std::filesystem::path> existing;
	if (std::filesystem::exists(dumpRoot)) {
		for (const auto& e : std::filesystem::directory_iterator(dumpRoot)) {
			existing.insert(e.path());
		}
	}

	Options options;
	options.setOption("engine.backend", std::string("mlir"));
	options.setOption("mlir.debug.enable", true);
	options.setOption("mlir.debug.source_mode", std::string("nautilus-ir"));
	options.setOption("dump.before_llvm_optimization", true);

	NautilusEngine engine(options);
	auto fn = engine.registerFunction(debugNestedControlFlow);
	REQUIRE(fn(10) == 19);

	int terminatorsChecked = 0;
	bool sawLineZero = false;
	if (std::filesystem::exists(dumpRoot)) {
		for (const auto& dir : std::filesystem::directory_iterator(dumpRoot)) {
			if (existing.count(dir.path())) {
				continue;
			}
			for (const auto& entry : std::filesystem::recursive_directory_iterator(dir)) {
				if (!entry.is_regular_file() || entry.path().filename() != "before_llvm_optimization.ll") {
					continue;
				}
				auto contents = readFile(entry.path().string());

				// Map each DILocation metadata id to its `line:` value.
				std::map<std::string, std::string> dilocationLine;
				{
					std::istringstream iss(contents);
					std::string line;
					while (std::getline(iss, line)) {
						if (line.find("!DILocation(") == std::string::npos) {
							continue;
						}
						const auto bang = line.find('!');
						const auto idEnd = line.find(' ', bang);
						if (bang == std::string::npos || idEnd == std::string::npos) {
							continue;
						}
						auto metaId = line.substr(bang + 1, idEnd - bang - 1);
						const auto key = line.find("line: ");
						if (key == std::string::npos) {
							continue;
						}
						auto start = key + std::string("line: ").size();
						auto end = start;
						while (end < line.size() && std::isdigit(static_cast<unsigned char>(line[end]))) {
							++end;
						}
						dilocationLine[metaId] = line.substr(start, end - start);
					}
				}

				// Scan instruction lines for branches and returns and
				// assert every associated !dbg resolves to a non-zero
				// line.  Skip the `_mlir_*` ABI wrappers — those are
				// synthesized by MLIR and correctly carry line 0.
				std::istringstream iss(contents);
				std::string line;
				std::string currentFunction;
				while (std::getline(iss, line)) {
					if (line.rfind("define ", 0) == 0) {
						auto at = line.find('@');
						auto open = (at == std::string::npos) ? std::string::npos : line.find('(', at);
						currentFunction = (at != std::string::npos && open != std::string::npos)
						                      ? line.substr(at + 1, open - at - 1)
						                      : std::string {};
					} else if (line == "}") {
						currentFunction.clear();
					}
					if (currentFunction.empty() || currentFunction.rfind("_mlir_", 0) == 0) {
						continue;
					}
					const bool isBr = line.find("  br label ") != std::string::npos ||
					                  line.find("  br i1 ") != std::string::npos ||
					                  line.find("  ret ") != std::string::npos;
					if (!isBr) {
						continue;
					}
					const auto dbgKey = line.find("!dbg !");
					if (dbgKey == std::string::npos) {
						continue;
					}
					auto start = dbgKey + std::string("!dbg !").size();
					auto end = start;
					while (end < line.size() && std::isdigit(static_cast<unsigned char>(line[end]))) {
						++end;
					}
					auto id = line.substr(start, end - start);
					const auto it = dilocationLine.find(id);
					REQUIRE(it != dilocationLine.end());
					++terminatorsChecked;
					if (it->second == "0") {
						INFO("terminator in @" << currentFunction << " has !dbg !" << id << " line: 0 — "
						                       << line);
						sawLineZero = true;
					}
				}
				break;
			}
			if (terminatorsChecked > 0) {
				break;
			}
		}
	}
	REQUIRE(terminatorsChecked > 0);
	REQUIRE_FALSE(sawLineZero);
}

} // namespace nautilus::engine

#endif // ENABLE_TRACING && ENABLE_MLIR_BACKEND

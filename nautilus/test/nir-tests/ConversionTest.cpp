// The conversion from pre-SSA to SSA NIR (docs/plans/nautilus-0.4/03-conversion-and-passes.md §3).
#include "nautilus/conversion/Conversion.hpp"
#include "nautilus/nir/Parser.hpp"
#include "nautilus/nir/Printer.hpp"
#include "nautilus/nir/Verifier.hpp"
#include <catch2/catch_all.hpp>

using namespace nautilus::nir;
using nautilus::conversion::ConversionStatistics;
using nautilus::conversion::convert;

namespace {

Module parseOk(const std::string& text) {
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
	return std::move(*r.module);
}

/// Converts and requires the result to verify; returns its text.
std::string converted(const std::string& text, ConversionStatistics* stats = nullptr) {
	auto pre = parseOk(text);
	auto ssa = convert(pre, {}, stats);
	auto diagnostics = verify(ssa);
	std::string all;
	for (auto& d : diagnostics) {
		all += d.str() + "\n";
	}
	INFO(all + print(ssa));
	REQUIRE(diagnostics.empty());
	return print(ssa);
}

} // namespace

TEST_CASE("a straight-line function converts without block parameters", "[nir][conversion]") {
	auto text = converted(R"(nir 1.0

func export pressa @f : (i64) -> i64 {
^b0(%0: i64):
  %1 = var 1:i64
  %2 = add %0, %1
  [3] ret %2
}
)");
	CHECK(text.find("func export @f : (i64) -> i64") != std::string::npos);
	CHECK(text.find("add %0, 1:i64") != std::string::npos);
	CHECK(text.find("ret") != std::string::npos);
}

TEST_CASE("a variable assigned in both arms becomes a block parameter of the merge", "[nir][conversion]") {
	ConversionStatistics stats;
	auto text = converted(R"(nir 1.0

func export pressa @f : (i64) -> i64 {
^b0(%0: i64):
  %1 = var 0:i64
  %2 = cmp.gt.s %0, 0:i64
  [3] condbr %2, ^b1, ^b2
^b1:
  [4] assign %1, %0
  [5] br ^b3
^b2:
  %6 = var 5:i64
  [7] assign %1, %6
  [8] br ^b3
^b3:
  [9] ret %1
}
)",
	                      &stats);
	CHECK(stats.phis == 1);
	CHECK(stats.functions == 1);
	CHECK(text.find("^b3(") != std::string::npos);
}

TEST_CASE("a loop-carried variable gets a header parameter and the layout keeps the loop contiguous",
          "[nir][conversion]") {
	ConversionStatistics stats;
	auto text = converted(R"(nir 1.0

func export pressa @sum : (i64) -> i64 {
^b0(%0: i64):
  %1 = var 0:i64
  %2 = var 0:i64
  [3] br ^b1
^b1:
  %4 = cmp.lt.s %2, %0
  [5] condbr %4, ^b2, ^b3
^b2:
  %6 = add %1, %2
  [7] assign %1, %6
  %8 = add %2, 1:i64
  [9] assign %2, %8
  [10] br ^b1
^b3:
  [11] ret %1
}
)",
	                      &stats);
	CHECK(stats.loops == 1);
	CHECK(stats.phis == 2);
	CHECK(stats.irreducibleRegions == 0);
}

TEST_CASE("a copy is a copy: assigning the source later does not change it", "[nir][conversion]") {
	auto text = converted(R"(nir 1.0

func export pressa @f : (i64) -> i64 {
^b0(%0: i64):
  %1 = var %0
  %2 = var %1
  %3 = add %1, 1:i64
  [4] assign %1, %3
  %5 = add %2, %1
  [6] ret %5
}
)");
	// %2 keeps the parameter, %1 became %0 + 1
	CHECK(text.find("add %0, 1:i64") != std::string::npos);
	CHECK(text.find("add %0, %1") != std::string::npos);
}

TEST_CASE("a trivial header parameter is folded away", "[nir][conversion]") {
	ConversionStatistics stats;
	converted(R"(nir 1.0

func export pressa @f : (i64) -> i64 {
^b0(%0: i64):
  %1 = var %0
  [2] br ^b1
^b1:
  %3 = cmp.lt.s %1, 10:i64
  [4] condbr %3, ^b2, ^b3
^b2:
  [5] assign %1, %0
  [6] br ^b1
^b3:
  [7] ret %1
}
)",
	          &stats);
	CHECK(stats.phis == 0);
	CHECK(stats.removedPhis >= 1);
}

TEST_CASE("cleanup lists become landing pads that equal lists share", "[nir][conversion]") {
	ConversionStatistics stats;
	auto text = converted(R"(nir 1.0
pointer #p0 = destructor symbol "~T"
pointer #p1 = callee symbol "g"
pointer #p2 = thunk symbol "g$thunk"
callee #c0 = external #p0 : (ptr) -> void attrs(noUnwind)
callee #c1 = external #p1 : (ptr) -> void

func export pressa @f : (ptr) -> void {
^b0(%0: ptr):
  [1] call #c1(%0) cleanup [(#c0, %0)] thunk #p2
  [2] call #c1(%0) cleanup [(#c0, %0)] thunk #p2
  [3] call #c1(%0) cleanup [] thunk #p2
  [4] ret
}
)",
	                      &stats);
	CHECK(stats.landingPads == 1);
	CHECK(text.find("resume") != std::string::npos);
}

TEST_CASE("an irreducible region keeps its blocks in reverse postorder", "[nir][conversion]") {
	ConversionStatistics stats;
	converted(R"(nir 1.0

func export pressa @f : (i64) -> i64 {
^b0(%0: i64):
  %1 = var 0:i64
  %2 = cmp.gt.s %0, 0:i64
  [3] condbr %2, ^b1, ^b2
^b1:
  %4 = add %1, 1:i64
  [5] assign %1, %4
  %6 = cmp.lt.s %1, 100:i64
  [7] condbr %6, ^b2, ^b3
^b2:
  %8 = add %1, 2:i64
  [9] assign %1, %8
  %10 = cmp.lt.s %1, 100:i64
  [11] condbr %10, ^b1, ^b3
^b3:
  [12] ret %1
}
)",
	          &stats);
	CHECK(stats.irreducibleRegions >= 1);
}

TEST_CASE("a long chain of blocks converts without deep recursion", "[nir][conversion]") {
	// 40000 single-predecessor blocks, the variable defined in the first: reading it in the last walks the chain
	std::string text =
	    "nir 1.0\n\nfunc export pressa @chain : (i64) -> i64 {\n^b0(%0: i64):\n  %1 = var %0\n  [2] br ^b1\n";
	const int blocks = 40000;
	int id = 3;
	for (int b = 1; b <= blocks; ++b) {
		text += "^b" + std::to_string(b) + ":\n";
		if (b < blocks) {
			text += "  [" + std::to_string(id) + "] br ^b" + std::to_string(b + 1) + "\n";
		} else {
			text += "  [" + std::to_string(id) + "] ret %1\n";
		}
		++id;
	}
	text += "}\n";
	converted(text);
}

// ---- optimization during emission (03 §3 steps 2 and 4, §3.3) -------------------------------------------------------

TEST_CASE("constants fold and identities simplify on emission", "[nir][conversion][fold]") {
	ConversionStatistics stats;
	auto text = converted(R"(nir 1.0

func export pressa @f : (i64) -> i64 {
^b0(%0: i64):
  %1 = add 2:i64, 3:i64
  %2 = add %0, 0:i64
  %3 = mul %2, 1:i64
  %4 = band %3, -1:i64
  %5 = add %4, %1
  [6] ret %5
}
)",
	                      &stats);
	CHECK(stats.folded == 4);
	// only the addition of the parameter and 5 is left
	CHECK(text.find("add %0, 5:i64") != std::string::npos);
	CHECK(text.find("mul") == std::string::npos);
	CHECK(stats.instructions == 2);
}

TEST_CASE("a division by a safe constant needs no check and no landing pad", "[nir][conversion][fold]") {
	ConversionStatistics stats;
	auto text = converted(R"(nir 1.0
pointer #p0 = destructor symbol "~T"
callee #c0 = external #p0 : (ptr) -> void attrs(noUnwind)

func export pressa @f : (i64, ptr) -> i64 {
^b0(%0: i64, %1: ptr):
  %2 = div.s.checked %0, 4:i64 cleanup [(#c0, %1)]
  %3 = div.s.checked %0, -1:i64 cleanup [(#c0, %1)]
  %4 = add %2, %3
  [5] ret %4
}
)",
	                      &stats);
	CHECK(stats.uncheckedDivisions == 1);
	// the division by -1 can still throw (the minimum divided by -1), so its pad stays
	CHECK(stats.landingPads == 1);
	CHECK(text.find("div.s %0, 4:i64") != std::string::npos);
}

TEST_CASE("a division by zero is never folded", "[nir][conversion][fold]") {
	auto text = converted(R"(nir 1.0

func export pressa @f : (i64) -> i64 {
^b0(%0: i64):
  %1 = div.s.checked 7:i64, 0:i64 cleanup []
  [2] ret %1
}
)");
	CHECK(text.find("div.s.checked") != std::string::npos);
}

TEST_CASE("a constant branch keeps only the taken edge and the dead block disappears", "[nir][conversion][fold]") {
	ConversionStatistics stats;
	auto text = converted(R"(nir 1.0

func export pressa @f : (i64) -> i64 {
^b0(%0: i64):
  %1 = var 1:i64
  %2 = cmp.lt.s 1:i64, 2:i64
  [3] condbr %2, ^b1, ^b2
^b1:
  %4 = add %1, %0
  [5] assign %1, %4
  [6] br ^b3
^b2:
  [7] assign %1, 99:i64
  [8] br ^b3
^b3:
  [9] ret %1
}
)",
	                      &stats);
	CHECK(stats.foldedBranches == 1);
	CHECK(stats.deadBlocks == 1);
	// no merge parameter is left: only one edge enters the merge block
	CHECK(stats.phis == 0);
	CHECK(text.find("99") == std::string::npos);
}

TEST_CASE("a loop behind a dead branch is dropped with its header", "[nir][conversion][fold]") {
	ConversionStatistics stats;
	converted(R"(nir 1.0

func export pressa @f : (i64) -> i64 {
^b0(%0: i64):
  %1 = var 0:i64
  [2] condbr false:bool, ^b1, ^b4
^b1:
  [3] br ^b2
^b2:
  %4 = cmp.lt.s %1, %0
  [5] condbr %4, ^b3, ^b4
^b3:
  %6 = add %1, 1:i64
  [7] assign %1, %6
  [8] br ^b2
^b4:
  [9] ret %1
}
)",
	          &stats);
	CHECK(stats.deadBlocks == 3);
	CHECK(stats.loops == 1);
}

TEST_CASE("the dead-value sweep removes a loop-carried variable nobody reads", "[nir][conversion][fold]") {
	ConversionStatistics stats;
	auto text = converted(R"(nir 1.0

func export pressa @f : (i64) -> i64 {
^b0(%0: i64):
  %1 = var 0:i64
  %2 = var 0:i64
  [3] br ^b1
^b1:
  %4 = cmp.lt.s %1, %0
  [5] condbr %4, ^b2, ^b3
^b2:
  %6 = add %1, 1:i64
  [7] assign %1, %6
  %8 = add %2, 2:i64
  [9] assign %2, %8
  [10] br ^b1
^b3:
  [11] ret %1
}
)",
	                      &stats);
	// %2 is assigned in the loop and never read after it: its phi and its additions go
	CHECK(stats.phis == 1);
	CHECK(text.find("2:i64") == std::string::npos);
	CHECK(stats.deadValues >= 2);
}

TEST_CASE("an unoptimized conversion keeps everything the trace recorded", "[nir][conversion][fold]") {
	auto pre = parseOk(R"(nir 1.0

func export pressa @f : (i64) -> i64 {
^b0(%0: i64):
  %1 = add 2:i64, 3:i64
  %2 = add %0, %1
  [3] ret %2
}
)");
	ConversionStatistics folded, plain;
	convert(pre, {}, &folded);
	convert(pre, {.optimize = false}, &plain);
	CHECK(folded.instructions == 2);
	CHECK(plain.instructions == 3);
	CHECK(plain.folded == 0);
}

// ---- the attribute pre-pass (03 §3.5) ----------------------------------------------------------------------------

namespace {

constexpr uint32_t kNoRead = AttrNoArgRead | AttrNoOtherRead;
constexpr uint32_t kNoWrite = AttrNoArgWrite | AttrNoOtherWrite;
constexpr uint32_t kPure = kNoRead | kNoWrite | AttrNoUnwind | AttrWillReturn;

/// The guarantee bits of the function named `name` after conversion, which every callee record naming it must repeat.
uint32_t inferred(const Module& ssa, std::string_view name) {
	for (uint32_t f = 0; f < ssa.functions.size(); ++f) {
		if (ssa.name(ssa.functions[f].nameId) == name) {
			for (auto& c : ssa.callees) {
				if (c.kind == static_cast<uint8_t>(CalleeKind::Internal) && c.target == f) {
					CHECK((c.attrs & GUARANTEE_MASK) == (ssa.functions[f].attrs & GUARANTEE_MASK));
				}
			}
			return ssa.functions[f].attrs & GUARANTEE_MASK;
		}
	}
	FAIL("no function " << name);
	return 0;
}

Module convertedModule(const std::string& text, ConversionStatistics* stats = nullptr) {
	auto pre = parseOk(text);
	auto ssa = convert(pre, {}, stats);
	auto diagnostics = verify(ssa);
	std::string all;
	for (auto& d : diagnostics) {
		all += d.str() + "\n";
	}
	INFO(all + print(ssa));
	REQUIRE(diagnostics.empty());
	return ssa;
}

} // namespace

TEST_CASE("a function without memory access, loops or throwing calls gets every guarantee",
          "[nir][conversion][attrs]") {
	ConversionStatistics stats;
	auto ssa = convertedModule(R"(nir 1.0
callee #c0 = internal @pure : (i64) -> i64

func pressa @pure : (i64) -> i64 {
^b0(%0: i64):
  %1 = add %0, 1:i64
  [2] ret %1
}

func export pressa @f : (i64) -> i64 {
^b0(%0: i64):
  %1 = call #c0(%0)
  [2] ret %1
}
)",
	                           &stats);
	CHECK(inferred(ssa, "pure") == kPure);
	CHECK(stats.inferredFunctions == 1);
	CHECK(stats.inferredNoUnwind == 1);
	CHECK(stats.inferredWillReturn == 1);
}

TEST_CASE("loads and stores take away the no-read and no-write guarantees and callers inherit that",
          "[nir][conversion][attrs]") {
	auto ssa = convertedModule(R"(nir 1.0
callee #c0 = internal @reader : (ptr) -> i64
callee #c1 = internal @writer : (ptr) -> void
callee #c2 = internal @viaReader : (ptr) -> i64

func pressa @reader : (ptr) -> i64 {
^b0(%0: ptr):
  %1 = load %0 : i64
  [2] ret %1
}

func pressa @writer : (ptr) -> void {
^b0(%0: ptr):
  [1] store %0, 1:i64
  [2] ret
}

func pressa @viaReader : (ptr) -> i64 {
^b0(%0: ptr):
  %1 = call #c0(%0)
  [2] ret %1
}

func export pressa @f : (ptr) -> i64 {
^b0(%0: ptr):
  [1] call #c1(%0)
  %2 = call #c2(%0)
  [3] ret %2
}
)");
	CHECK(inferred(ssa, "reader") == (kNoWrite | AttrNoUnwind | AttrWillReturn));
	CHECK(inferred(ssa, "writer") == (kNoRead | AttrNoUnwind | AttrWillReturn));
	CHECK(inferred(ssa, "viaReader") == (kNoWrite | AttrNoUnwind | AttrWillReturn));
}

TEST_CASE("a loop is not willReturn, and a cycle in the call graph gets nothing", "[nir][conversion][attrs]") {
	ConversionStatistics stats;
	auto ssa = convertedModule(R"(nir 1.0
callee #c0 = internal @spin : (i64) -> i64
callee #c1 = internal @ping : (i64) -> i64
callee #c2 = internal @pong : (i64) -> i64

func pressa @spin : (i64) -> i64 {
^b0(%0: i64):
  %1 = var 0:i64
  [2] br ^b1
^b1:
  %3 = cmp.lt.s %1, %0
  [4] condbr %3, ^b2, ^b3
^b2:
  %5 = add %1, 1:i64
  [6] assign %1, %5
  [7] br ^b1
^b3:
  [8] ret %1
}

func pressa @ping : (i64) -> i64 {
^b0(%0: i64):
  %1 = call #c2(%0)
  [2] ret %1
}

func pressa @pong : (i64) -> i64 {
^b0(%0: i64):
  %1 = call #c1(%0)
  [2] ret %1
}

func export pressa @f : (i64) -> i64 {
^b0(%0: i64):
  %1 = call #c0(%0)
  %2 = call #c1(%1)
  [3] ret %2
}
)",
	                           &stats);
	CHECK(inferred(ssa, "spin") == (kNoRead | kNoWrite | AttrNoUnwind));
	CHECK(inferred(ssa, "ping") == 0);
	CHECK(inferred(ssa, "pong") == 0);
	CHECK(stats.cyclicFunctions == 2);
}

TEST_CASE("a checked division makes a function may-throw unless the divisor is a safe constant",
          "[nir][conversion][attrs]") {
	auto ssa = convertedModule(R"(nir 1.0
callee #c0 = internal @byVariable : (i64, i64) -> i64
callee #c1 = internal @byConstant : (i64) -> i64
callee #c2 = internal @byConstantVariable : (i64) -> i64
callee #c3 = internal @byMinusOne : (i64) -> i64

func pressa @byVariable : (i64, i64) -> i64 {
^b0(%0: i64, %1: i64):
  %2 = div.s.checked %0, %1 cleanup []
  [3] ret %2
}

func pressa @byConstant : (i64) -> i64 {
^b0(%0: i64):
  %1 = div.s.checked %0, 4:i64 cleanup []
  [2] ret %1
}

func pressa @byConstantVariable : (i64) -> i64 {
^b0(%0: i64):
  %1 = var 8:i64
  %2 = div.u.checked %0, %1 cleanup []
  [3] ret %2
}

func pressa @byMinusOne : (i64) -> i64 {
^b0(%0: i64):
  %1 = div.s.checked %0, -1:i64 cleanup []
  [2] ret %1
}

func export pressa @f : (i64, i64) -> i64 {
^b0(%0: i64, %1: i64):
  %2 = call #c0(%0, %1)
  %3 = call #c1(%2)
  %4 = call #c2(%3)
  %5 = call #c3(%4)
  [6] ret %5
}
)");
	CHECK(inferred(ssa, "byVariable") == (kNoRead | kNoWrite | AttrWillReturn));
	CHECK(inferred(ssa, "byConstant") == kPure);
	CHECK(inferred(ssa, "byConstantVariable") == kPure);
	// the minimum divided by -1 still throws
	CHECK(inferred(ssa, "byMinusOne") == (kNoRead | kNoWrite | AttrWillReturn));
}

TEST_CASE("an exported function keeps what the trace recorded", "[nir][conversion][attrs]") {
	auto pre = parseOk(R"(nir 1.0

func export pressa @f : (i64) -> i64 {
^b0(%0: i64):
  %1 = add %0, 1:i64
  [2] ret %1
}
)");
	auto before = pre.functions[0].attrs;
	auto ssa = convert(pre);
	CHECK(ssa.functions[0].attrs == before);
}

TEST_CASE("a call to a callee that cannot throw loses its unwind entry and its landing pad",
          "[nir][conversion][attrs]") {
	ConversionStatistics stats;
	auto ssa = convertedModule(R"(nir 1.0
pointer #p0 = destructor symbol "~T"
callee #c0 = external #p0 : (ptr) -> void attrs(noUnwind)
callee #c1 = internal @quiet : (ptr) -> void

func pressa @quiet : (ptr) -> void {
^b0(%0: ptr):
  [1] ret
}

func export pressa @f : (ptr) -> void {
^b0(%0: ptr):
  [1] call #c1(%0) cleanup [(#c0, %0)]
  [2] ret
}
)",
	                           &stats);
	CHECK(stats.droppedUnwinds == 1);
	CHECK(stats.landingPads == 0);
}

// ---- common subexpressions (03 §3.4) -----------------------------------------------------------------------------

namespace {

uint32_t countOps(const Module& ssa, Op op) {
	uint32_t n = 0;
	for (auto& inst : ssa.insts) {
		n += inst.op == static_cast<uint16_t>(op);
	}
	return n;
}

} // namespace

TEST_CASE("equal pure values and loads in a block, and in its single-predecessor successors, are shared",
          "[nir][conversion][cse]") {
	ConversionStatistics stats;
	auto ssa = convertedModule(R"(nir 1.0
func export pressa @f : (ptr, i64, i64) -> i64 {
^b0(%0: ptr, %1: i64, %2: i64):
  %3 = add %1, %2
  %4 = add %2, %1
  %5 = load %0 : i64
  %6 = load %0 : i64
  %7 = cmp.lt.s %3, %1
  [8] condbr %7, ^b1, ^b2
^b1:
  %9 = add %1, %2
  %10 = load %0 : i64
  %11 = mul %9, %10
  %12 = add %11, %5
  [13] ret %12
^b2:
  %14 = add %3, %6
  [15] ret %14
}
)",
	                           &stats);
	CHECK(countOps(ssa, Op::Load) == 1);
	CHECK(stats.cse >= 4);
}

TEST_CASE("a store between two loads keeps both, and a join does not reuse a load across an arm that stored",
          "[nir][conversion][cse]") {
	auto ssa = convertedModule(R"(nir 1.0
func export pressa @f : (ptr, i64) -> i64 {
^b0(%0: ptr, %1: i64):
  %2 = load %0 : i64
  %3 = load %0 : i64
  [4] store %0, 7:i64
  %5 = load %0 : i64
  %6 = cmp.lt.s %1, 5:i64
  [7] condbr %6, ^b1, ^b2
^b1:
  [8] store %0, 9:i64
  [9] br ^b3
^b2:
  [10] br ^b3
^b3:
  %11 = load %0 : i64
  %12 = add %2, %3
  %13 = add %12, %5
  %14 = add %13, %11
  [15] ret %14
}
)");
	// before the store (shared), after it, and at the join
	CHECK(countOps(ssa, Op::Load) == 3);
}

TEST_CASE("a load in a loop header is not replaced by one before the loop that the body overwrites",
          "[nir][conversion][cse]") {
	auto ssa = convertedModule(R"(nir 1.0
func export pressa @f : (ptr, i64) -> i64 {
^b0(%0: ptr, %1: i64):
  %2 = load %0 : i64
  %3 = var 0:i64
  [4] br ^b1
^b1:
  %5 = load %0 : i64
  %6 = cmp.lt.s %3, %1
  [7] condbr %6, ^b2, ^b3
^b2:
  [8] store %0, %3
  %9 = add %3, 1:i64
  [10] assign %3, %9
  [11] br ^b1
^b3:
  %12 = add %2, %5
  [13] ret %12
}
)");
	CHECK(countOps(ssa, Op::Load) == 2);
}

TEST_CASE("a read-only callee is called once until memory may change", "[nir][conversion][cse]") {
	auto ssa = convertedModule(R"(nir 1.0
callee #c0 = internal @peek : (ptr) -> i64
callee #c1 = internal @poke : (ptr) -> void

func pressa @peek : (ptr) -> i64 {
^b0(%0: ptr):
  %1 = load %0 : i64
  [2] ret %1
}

func pressa @poke : (ptr) -> void {
^b0(%0: ptr):
  [1] store %0, 1:i64
  [2] ret
}

func export pressa @f : (ptr) -> i64 {
^b0(%0: ptr):
  %1 = call #c0(%0)
  %2 = call #c0(%0)
  [3] call #c1(%0)
  %4 = call #c0(%0)
  %5 = add %1, %2
  %6 = add %5, %4
  [7] ret %6
}
)");
	uint32_t peeks = 0;
	for (auto& inst : ssa.insts) {
		if (inst.op == static_cast<uint16_t>(Op::Call) &&
		    ssa.callees[inst.a].kind == static_cast<uint8_t>(CalleeKind::Internal) &&
		    ssa.name(ssa.functions[ssa.callees[inst.a].target].nameId) == "peek") {
			++peeks;
		}
	}
	CHECK(peeks == 2);
}

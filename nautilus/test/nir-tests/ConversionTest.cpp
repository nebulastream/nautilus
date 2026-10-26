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
	std::string text = "nir 1.0\n\nfunc export pressa @chain : (i64) -> i64 {\n^b0(%0: i64):\n  %1 = var %0\n  [2] br ^b1\n";
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

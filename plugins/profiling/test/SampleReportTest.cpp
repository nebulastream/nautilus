#include "nautilus/profiling/sample_report.hpp"
#include <catch2/catch_test_macros.hpp>
#include <filesystem>
#include <fstream>
#include <string>

using namespace nautilus;

// Pure aggregation and formatting over already-resolved sites: no hardware, no
// skips.

namespace {
profiling::SampleReport makeReport() {
	return profiling::SampleReport({{"execute::query::mix", profiling::SymbolOrigin::Jit, 700, 70.0, ""},
	                                {"mergeAccumulators(long, long)", profiling::SymbolOrigin::Host, 200, 20.0, ""},
	                                {"0x7ffff7a1b2c3", profiling::SymbolOrigin::Unresolved, 100, 10.0, ""}},
	                               1000);
}
} // namespace

TEST_CASE("an empty report renders as nothing", "[profiling][report]") {
	const profiling::SampleReport report;
	CHECK(report.empty());
	CHECK(report.total() == 0);
	CHECK(report.jitSamples() == 0);
	CHECK(report.table().empty());
	CHECK(report.foldedStacks().empty());
}

TEST_CASE("jit samples are counted apart from host and unresolved ones", "[profiling][report]") {
	const auto report = makeReport();
	CHECK(report.total() == 1000);
	CHECK(report.jitSamples() == 700);
}

TEST_CASE("the table lists sites and the JIT share", "[profiling][report]") {
	const auto table = makeReport().table();
	CHECK(table.find("execute::query::mix") != std::string::npos);
	CHECK(table.find("jit") != std::string::npos);
	CHECK(table.find("host") != std::string::npos);
	CHECK(table.find("1000 samples") != std::string::npos);
}

TEST_CASE("the table can be truncated to the hottest sites", "[profiling][report]") {
	const auto table = makeReport().table(1);
	CHECK(table.find("execute::query::mix") != std::string::npos);
	CHECK(table.find("mergeAccumulators") == std::string::npos);
	CHECK(table.find("2 more") != std::string::npos);
}

TEST_CASE("a region-qualified name folds into nested frames", "[profiling][report]") {
	const auto folded = makeReport().foldedStacks();
	// Each level keeps its fully qualified name, so the rendered graph labels a
	// region the way the report names it while nesting it under its parent.
	CHECK(folded.find("execute;execute::query;execute::query::mix 700\n") != std::string::npos);
}

TEST_CASE("a demangled C++ symbol is left as one frame", "[profiling][report]") {
	// `mergeAccumulators(long, long)` contains `::`-free text but a real C++
	// signature with namespaces would too; the argument list is what marks it
	// as a host symbol whose `::` were never frames.
	const profiling::SampleReport report({{"ns::Klass::method(int)", profiling::SymbolOrigin::Host, 5, 100.0, ""}}, 5);
	CHECK(report.foldedStacks() == "ns::Klass::method(int) 5\n");
}

TEST_CASE("a templated symbol is left as one frame", "[profiling][report]") {
	const profiling::SampleReport report({{"std::vector<int>::push_back", profiling::SymbolOrigin::Host, 5, 100.0, ""}},
	                                     5);
	CHECK(report.foldedStacks() == "std::vector<int>::push_back 5\n");
}

TEST_CASE("an unqualified name folds to itself", "[profiling][report]") {
	const profiling::SampleReport report({{"execute", profiling::SymbolOrigin::Jit, 5, 100.0, ""}}, 5);
	CHECK(report.foldedStacks() == "execute 5\n");
}

TEST_CASE("csv carries one row per site", "[profiling][report]") {
	const auto csv = makeReport().csv();
	CHECK(csv.find("share,samples,origin,module,symbol\n") == 0);
	CHECK(csv.find(",700,jit,,execute::query::mix\n") != std::string::npos);
	CHECK(csv.find(",100,unresolved,,0x7ffff7a1b2c3\n") != std::string::npos);
}

// Module scoping: two compiles in one process both produce a symbol called
// `execute`, and the profile has to keep them apart without making the
// single-module case unreadable.

TEST_CASE("a name from two modules is qualified by its compile", "[profiling][report]") {
	const profiling::SampleReport report(
	    {{"execute::query", profiling::SymbolOrigin::Jit, 60, 60.0, "2026-09-20_11-09-32.0_#AAAAAAA"},
	     {"execute::query", profiling::SymbolOrigin::Jit, 40, 40.0, "2026-09-20_11-09-33.0_#BBBBBBB"}},
	    100);

	REQUIRE(report.sites().size() == 2);
	CHECK(report.sites()[0].symbol == "execute::query @AAAAAAA");
	CHECK(report.sites()[1].symbol == "execute::query @BBBBBBB");
	// The full id stays available whether or not the name was qualified.
	CHECK(report.sites()[0].module == "2026-09-20_11-09-32.0_#AAAAAAA");
	CHECK(report.modules().size() == 2);
}

TEST_CASE("a name only one module contributed is left bare", "[profiling][report]") {
	// The common case, and what every single-module profile looks like: adding
	// module scoping must not change how those read.
	const profiling::SampleReport report(
	    {{"execute::query", profiling::SymbolOrigin::Jit, 60, 60.0, "2026-09-20_11-09-32.0_#AAAAAAA"},
	     {"execute::scan", profiling::SymbolOrigin::Jit, 40, 40.0, "2026-09-20_11-09-32.0_#AAAAAAA"}},
	    100);

	CHECK(report.sites()[0].symbol == "execute::query");
	CHECK(report.sites()[1].symbol == "execute::scan");
	CHECK(report.modules().size() == 1);
}

TEST_CASE("host frames are never qualified", "[profiling][report]") {
	// A host symbol has no module, so it cannot collide with one -- and two
	// modules calling the same host function is not ambiguity, it is one
	// function.
	const profiling::SampleReport report(
	    {{"execute", profiling::SymbolOrigin::Jit, 50, 50.0, "2026-09-20_11-09-32.0_#AAAAAAA"},
	     {"execute", profiling::SymbolOrigin::Jit, 30, 30.0, "2026-09-20_11-09-33.0_#BBBBBBB"},
	     {"memcpy", profiling::SymbolOrigin::Host, 20, 20.0, ""}},
	    100);

	CHECK(report.sites()[0].symbol == "execute @AAAAAAA");
	CHECK(report.sites()[2].symbol == "memcpy");
}

TEST_CASE("csv carries the full module id", "[profiling][report]") {
	const profiling::SampleReport report(
	    {{"execute", profiling::SymbolOrigin::Jit, 5, 100.0, "2026-09-20_11-09-32.0_#AAAAAAA"}}, 5);
	CHECK(report.csv().find(",5,jit,2026-09-20_11-09-32.0_#AAAAAAA,execute\n") != std::string::npos);
}

// Flame graphs. The SVG is generated here rather than handed to flamegraph.pl,
// so its structure is ours to get right and worth pinning.

namespace {

/// A report with real call stacks, as a callchain-enabled sampler produces.
profiling::SampleReport makeStackedReport() {
	const profiling::Frame host {"main", profiling::SymbolOrigin::Host};
	const profiling::Frame jitOuter {"execute", profiling::SymbolOrigin::Jit};
	const profiling::Frame jitInner {"execute::mix", profiling::SymbolOrigin::Jit};
	const profiling::Frame jitOther {"execute::scan", profiling::SymbolOrigin::Jit};
	return profiling::SampleReport({{"execute::mix", profiling::SymbolOrigin::Jit, 75, 75.0, ""},
	                                {"execute::scan", profiling::SymbolOrigin::Jit, 25, 25.0, ""}},
	                               {{{host, jitOuter, jitInner}, 75}, {{host, jitOuter, jitOther}, 25}}, 100);
}

} // namespace

TEST_CASE("an empty report has no flame graph", "[profiling][flamegraph]") {
	CHECK(profiling::SampleReport().flameGraph().empty());
}

TEST_CASE("recorded stacks drive the folded output", "[profiling][flamegraph]") {
	// With call chains there is no need to synthesize nesting from a name --
	// the frames are already frames, host ones included.
	const auto folded = makeStackedReport().foldedStacks();
	CHECK(folded.find("main;execute;execute::mix 75\n") != std::string::npos);
	CHECK(folded.find("main;execute;execute::scan 25\n") != std::string::npos);
}

TEST_CASE("the flame graph is well-formed svg", "[profiling][flamegraph]") {
	const auto svg = makeStackedReport().flameGraph("a title");
	CHECK(svg.find("<?xml") == 0);
	CHECK(svg.find("<svg") != std::string::npos);
	CHECK(svg.rfind("</svg>") != std::string::npos);
	CHECK(svg.find("a title") != std::string::npos);
	// Every frame carries a tooltip with its share.
	CHECK(svg.find("<title>execute::mix -- 75 samples (75.00%)</title>") != std::string::npos);
}

TEST_CASE("frames are coloured by origin", "[profiling][flamegraph]") {
	// The one thing a general-purpose flame graph cannot know, so it is the one
	// thing worth asserting about the rendering.
	const auto svg = makeStackedReport().flameGraph();
	CHECK(svg.find("#e8663d") != std::string::npos); // jit
	CHECK(svg.find("#6a9fd4") != std::string::npos); // host
	CHECK(svg.find("JIT-compiled") != std::string::npos);
}

TEST_CASE("a report without stacks still renders region nesting", "[profiling][flamegraph]") {
	// No call chains recorded: the nesting a region-qualified name encodes is
	// all there is, and it still has to produce a tree rather than a flat row.
	const auto svg = makeReport().flameGraph();
	CHECK(svg.find("<title>execute -- 700 samples") != std::string::npos);
	CHECK(svg.find("<title>execute::query::mix -- 700 samples") != std::string::npos);
}

TEST_CASE("a templated symbol cannot break the svg", "[profiling][flamegraph]") {
	// `<` and `>` in a demangled name would close the enclosing element.
	const profiling::SampleReport report({{"std::vector<int>::push_back", profiling::SymbolOrigin::Host, 5, 100.0, ""}},
	                                     5);
	const auto svg = report.flameGraph();
	CHECK(svg.find("std::vector&lt;int&gt;::push_back") != std::string::npos);
	CHECK(svg.find("<int>") == std::string::npos);
}

TEST_CASE("writeFlameGraph reports an unopenable path", "[profiling][flamegraph]") {
	CHECK_FALSE(makeStackedReport().writeFlameGraph("/nonexistent-directory/flame.svg"));
}

// IR annotation. The rendering is ours, and so is the behaviour when the dump
// the samples point at is not where it was left.

namespace {

profiling::SampleReport makeAnnotatedReport(const std::string& file) {
	return profiling::SampleReport({{"execute::mix", profiling::SymbolOrigin::Jit, 80, 80.0, ""}}, {},
	                               {{2, 60, 60.0}, {4, 20, 20.0}}, file, 100);
}

} // namespace

TEST_CASE("annotation without a line table explains itself", "[profiling][annotate]") {
	const auto text = profiling::SampleReport().annotateSource();
	CHECK(text.find("no Nautilus-IR dump") != std::string::npos);
	CHECK(text.find("perf.sample") != std::string::npos);
}

TEST_CASE("a missing dump is reported, not fatal", "[profiling][annotate]") {
	// The dump lives in the working directory and nothing cleans it up or stops
	// it being moved, so this is a case that happens rather than a hypothetical.
	const auto text = makeAnnotatedReport("/nonexistent/nautilus_debug.ir").annotateSource();
	CHECK(text.find("could not read") != std::string::npos);
	CHECK(text.find("working directory") != std::string::npos);
}

TEST_CASE("annotation puts a share against the lines samples landed on", "[profiling][annotate]") {
	const auto path = std::filesystem::temp_directory_path() / "nautilus-annotate-test.ir";
	{
		std::ofstream out(path);
		out << "line one\nline two\nline three\nline four\nline five\n";
	}

	const auto text = makeAnnotatedReport(path.string()).annotateSource();
	std::filesystem::remove(path);

	// Shares are of the whole profile, the same scale the symbol table uses, so
	// they do not sum to 100 over one file.
	CHECK(text.find("80.00% of all samples land in this file") != std::string::npos);
	CHECK(text.find(" 60.00 |    2 | line two") != std::string::npos);
	CHECK(text.find(" 20.00 |    4 | line four") != std::string::npos);
	// A cold line is still printed, just without a percentage.
	CHECK(text.find("      |    1 | line one") != std::string::npos);
}

TEST_CASE("a context window elides cold stretches", "[profiling][annotate]") {
	const auto path = std::filesystem::temp_directory_path() / "nautilus-annotate-context.ir";
	{
		std::ofstream out(path);
		for (int line = 1; line <= 40; ++line) {
			out << "operation " << line << "\n";
		}
	}

	// Samples on lines 2 and 4 only; with one line of context the far end of a
	// forty-line dump must not be printed.
	const auto text = makeAnnotatedReport(path.string()).annotateSource({}, /*context=*/1);
	std::filesystem::remove(path);

	CHECK(text.find("operation 3") != std::string::npos);
	CHECK(text.find("operation 30") == std::string::npos);
	CHECK(text.find("| ... |") != std::string::npos);
}

#pragma once

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

namespace nautilus::profiling {

/// Where a sampled instruction pointer was resolved.
enum class SymbolOrigin : uint8_t {
	/// A nautilus JIT code range, named from the compile's own DWARF -- so
	/// possibly a `region()` scope, qualified by its nesting.
	Jit,
	/// An ordinary ELF symbol in the host binary or a shared library.
	Host,
	/// Nothing claimed the address: host code with no symbol table, a kernel
	/// address, or JIT code compiled without `perf.sample` set.
	Unresolved,
};

/// One symbol and how many samples landed in it.
struct SampleSite {
	/// What to print. Bare where the name is unambiguous across the profile;
	/// suffixed with `@<module>` where two compiles contributed a symbol of the
	/// same name, which is the common case once several modules are live (every
	/// single-function registration lowers to a function called `execute`).
	std::string symbol;
	SymbolOrigin origin = SymbolOrigin::Unresolved;
	uint64_t samples = 0;
	/// `samples` as a fraction of the profile, in percent.
	double share = 0.0;
	/// The `CompilationUnitID` this symbol's code was compiled by, or empty for
	/// host and unresolved frames. Always the full id, whether or not `symbol`
	/// was qualified -- so a report can be correlated with the compile's dump
	/// directory and its `compilation.unitId` statistic.
	std::string module;
};

/// One frame of a sampled call stack.
struct Frame {
	std::string name;
	SymbolOrigin origin = SymbolOrigin::Unresolved;
};

/// One distinct call stack and how many samples ended in it, root frame first.
///
/// Populated only when the sampler was asked for call chains
/// (`Sampler::Options::callchain`). Without them a stack is just its leaf, plus
/// whatever nesting a region-qualified name encodes -- enough for a readable
/// flame graph of JIT code, but with no host frames beneath it.
struct SampleStack {
	std::vector<Frame> frames;
	uint64_t samples = 0;
};

/// One line of the Nautilus-IR dump and how many samples landed on it.
struct SourceLine {
	uint32_t line = 0;
	uint64_t samples = 0;
	/// `samples` as a fraction of the whole profile, in percent.
	double share = 0.0;
};

/// An aggregated sampling profile.
///
/// The counterpart of what `perf report` prints, built without perf's tooling:
/// no `perf record`, no `-k mono`, no `perf inject --jit` pass, and no
/// requirement that the Nautilus-IR dump still exist when the profile is read.
class SampleReport {
public:
	SampleReport() = default;
	SampleReport(std::vector<SampleSite> sites, uint64_t total);
	SampleReport(std::vector<SampleSite> sites, std::vector<SampleStack> stacks, uint64_t total);
	SampleReport(std::vector<SampleSite> sites, std::vector<SampleStack> stacks, std::vector<SourceLine> sourceLines,
	             std::string sourceFile, uint64_t total);

	/// Sites in descending sample order.
	[[nodiscard]] const std::vector<SampleSite>& sites() const noexcept;

	/// Every sample taken, including the ones nothing could be resolved for.
	[[nodiscard]] uint64_t total() const noexcept;

	/// Samples that landed in nautilus JIT-compiled code.
	[[nodiscard]] uint64_t jitSamples() const noexcept;

	/// Samples per line of the Nautilus-IR dump, ascending by line number.
	/// Empty unless the compile published a line table (`perf.sample`).
	[[nodiscard]] const std::vector<SourceLine>& sourceLines() const noexcept;

	/// Path of the Nautilus-IR dump `sourceLines()` refer to, or empty.
	[[nodiscard]] const std::string& sourceFile() const noexcept;

	/// Distinct call stacks, descending by sample count.
	[[nodiscard]] const std::vector<SampleStack>& stacks() const noexcept;

	/// The distinct compiles that contributed samples, in first-seen order.
	/// More than one means the profile spans several live modules, and the
	/// symbols of any name they share were qualified.
	[[nodiscard]] std::vector<std::string> modules() const;

	[[nodiscard]] bool empty() const noexcept;

	/// A `perf report`-shaped table of the hottest @p top sites.
	[[nodiscard]] std::string table(size_t top = 20) const;

	/// `share,samples,origin,module,symbol`, one row per site.
	[[nodiscard]] std::string csv() const;

	/// Folded stacks in the format FlameGraph's `flamegraph.pl` and
	/// `inferno-flamegraph` read on stdin: `frame;frame;frame <count>`.
	///
	/// A region-qualified name is split back into the nesting it encodes, so
	/// `execute::query::mix` becomes `execute;execute::query;execute::query::mix`
	/// and the rendered graph shows a region inside its parent rather than
	/// beside it. Only nautilus names are split: a demangled C++ symbol also
	/// contains `::`, and exploding it per namespace component would invent
	/// frames that never existed. What tells them apart is that a nautilus
	/// qualified name is nothing but `::`-joined identifiers, never carrying a
	/// `(` for an argument list or a `<` for a template -- the same rule
	/// `tools/nautilus-perf.sh flamegraph` applies downstream of `perf script`.
	[[nodiscard]] std::string foldedStacks() const;

	/// A self-contained flame graph, as an SVG document.
	///
	/// Rendered here rather than by handing `foldedStacks()` to
	/// [FlameGraph](https://github.com/brendangregg/FlameGraph)'s
	/// `flamegraph.pl`, so that reading a profile needs nothing but the program
	/// that produced it -- no Perl, no clone, nothing to install in a container
	/// or a CI job. The folded output is still there for anyone who prefers
	/// their own renderer, or wants to merge several runs.
	///
	/// Frames are coloured by `SymbolOrigin`, which is the one thing a
	/// general-purpose flame graph cannot know: JIT-compiled nautilus code,
	/// host code and unresolved addresses are told apart at a glance rather
	/// than by the usual arbitrary hues. Hovering a frame shows its full name
	/// and share.
	///
	/// @param title shown in the header, above the graph.
	[[nodiscard]] std::string flameGraph(std::string_view title = "nautilus profile") const;

	/// Writes `flameGraph()` to @p path. Returns false if the file could not be
	/// opened, which is the only way this fails.
	[[nodiscard]] bool writeFlameGraph(const std::string& path, std::string_view title = "nautilus profile") const;

	/// The Nautilus-IR dump with a per-line percentage gutter, as
	/// `tools/nautilus-perf.sh annotate-ir` prints it after an out-of-process
	/// run -- but read straight from the sampler, with no `perf report` in
	/// between.
	///
	/// This is the level below the symbol table: `table()` says which region is
	/// hot, this says which IR operation inside it is. Percentages are shares of
	/// the *whole* profile, the same scale `table()` uses, so lines do not sum
	/// to 100 over one file -- the remainder is host code, the kernel, and the
	/// in-process compile.
	///
	/// @param path the IR dump to read; defaults to `sourceFile()`, which is
	///        where the samples themselves point. A perf-enabled compile writes
	///        it to the working directory precisely so it outlives the compile,
	///        but nothing cleans it up and nothing stops it being moved -- if it
	///        is gone, this returns a one-line explanation rather than failing.
	/// @param context lines of unannotated IR to keep around each annotated one,
	///        or 0 for the whole file.
	[[nodiscard]] std::string annotateSource(const std::string& path = {}, size_t context = 0) const;

private:
	std::vector<SampleSite> sites_;
	std::vector<SampleStack> stacks_;
	std::vector<SourceLine> sourceLines_;
	std::string sourceFile_;
	uint64_t total_ = 0;
};

} // namespace nautilus::profiling

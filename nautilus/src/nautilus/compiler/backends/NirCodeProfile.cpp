#include "nautilus/compiler/backends/NirCodeProfile.hpp"
#include "nautilus/compiler/backends/DebugSource.hpp"
#include "nautilus/compiler/backends/PerfJitDump.hpp"
#include "nautilus/nir/Printer.hpp"
#include <algorithm>
#include <atomic>
#include <numeric>
#include <unistd.h>

namespace nautilus::compiler {

namespace {

/// A span of the code of one function that shares a name: the function's own, or the function's qualified by the
/// `region()`s it was traced in.
struct Range {
	uint32_t start;
	uint32_t end;
	std::string name;
	/// The marks (indices into `marks`) whose code is in the range.
	uint32_t firstMark;
	uint32_t lastMark;
};

/// `outer::inner` for region `region`, outermost first; empty for no region.
std::string regionPath(const nir::Module& m, uint32_t region) {
	std::vector<uint32_t> chain;
	for (auto r = region; r != nir::NONE && r < m.debugRegions.size() && chain.size() < 4096;
	     r = m.debugRegions[r].parent) {
		chain.push_back(r);
		if (m.debugRegions[r].parent >= r) {
			break; // parents come first; anything else is not a chain
		}
	}
	std::string path;
	for (auto it = chain.rbegin(); it != chain.rend(); ++it) {
		const auto nameId = m.debugRegions[*it].nameId;
		path += "::";
		path += nameId != nir::NONE ? std::string(m.debugName(nameId)) : std::string("<region>");
	}
	return path;
}

} // namespace

bool profilesCode(const engine::Options& options) {
	return options.getOptionOrDefault("perf", false) || options.getOptionOrDefault("perf.sample", false);
}

std::string newCodeUnit(const std::string& prefix) {
	static std::atomic<uint64_t> counter {0};
	return prefix + "_" + std::to_string(::getpid()) + "_" + std::to_string(counter.fetch_add(1));
}

ModuleIndex publishNirCode(const nir::Module& m, const NirCodeMap& code, const engine::Options& options,
                           const std::string& unit) {
	const bool sample = options.getOptionOrDefault("perf.sample", false);
	const bool jitdump = options.getOptionOrDefault("perf", false);
	if ((!sample && !jitdump) || code.base == 0 || code.functionOffsets.size() != m.functions.size()) {
		return NO_MODULE;
	}
	const auto base = code.base;
	const auto codeSize = code.size;
	const auto& marks = code.marks;
	const auto& functionOffsets = code.functionOffsets;

	// The text the lines refer to, written where a profiler finds it after the process is gone (the working directory).
	nir::PrintLines printed;
	const auto text = nir::print(m, printed);
	const auto sourceFile = writeDebugSource(newDebugSourcePath(true, "ir"), text);

	// the function ranges: a function runs to the start of the next one
	std::vector<uint32_t> order(m.functions.size());
	std::iota(order.begin(), order.end(), 0u);
	std::sort(order.begin(), order.end(),
	          [&](uint32_t a, uint32_t b) { return functionOffsets[a] < functionOffsets[b]; });

	std::vector<Range> ranges;
	size_t nextMark = 0;
	for (size_t i = 0; i < order.size(); ++i) {
		const auto f = order[i];
		const auto start = functionOffsets[f];
		const auto end = i + 1 < order.size() ? functionOffsets[order[i + 1]] : codeSize;
		if (end <= start) {
			continue;
		}
		const std::string functionName(m.name(m.functions[f].nameId));
		// the marks of the code in [start, end), in code order
		while (nextMark < marks.size() && marks[nextMark].offset < start) {
			++nextMark;
		}
		uint32_t region = nir::NONE;
		std::string name = functionName;
		const auto firstOfFunction = static_cast<uint32_t>(nextMark);
		ranges.push_back({start, start, name, firstOfFunction, firstOfFunction});
		for (; nextMark < marks.size() && marks[nextMark].offset < end; ++nextMark) {
			const auto& mark = marks[nextMark];
			// instructions of an inlined callee stay in the region of the call; code that belongs to no instruction
			// (the cold paths) stays in the region it follows
			if (mark.inst != nir::NONE && mark.function == f) {
				region = m.regionOf(f, mark.inst);
			}
			auto qualified = functionName + regionPath(m, region);
			if (qualified != name) {
				ranges.back().end = std::max(ranges.back().start, mark.offset);
				if (ranges.back().end == ranges.back().start) {
					ranges.pop_back(); // a range of no code
				}
				name = std::move(qualified);
				ranges.push_back(
				    {mark.offset, mark.offset, name, static_cast<uint32_t>(nextMark), static_cast<uint32_t>(nextMark)});
			}
			ranges.back().lastMark = static_cast<uint32_t>(nextMark) + 1;
		}
		ranges.back().end = end;
	}
	// ranges that ended up empty
	ranges.erase(std::remove_if(ranges.begin(), ranges.end(), [](const Range& r) { return r.end <= r.start; }),
	             ranges.end());

	// the line of each mark that stands for an instruction
	const auto lineOf = [&](const NirCodeMark& mark) -> uint32_t {
		if (mark.inst == nir::NONE || mark.function >= m.functions.size()) {
			return 0;
		}
		return printed.instruction[m.functions[mark.function].firstInst + mark.inst];
	};

	ModuleIndex moduleIndex = NO_MODULE;
	if (sample) {
		auto& registry = JitSymbolRegistry::instance();
		moduleIndex = registry.intern(unit);
		std::vector<JitSymbol> symbols;
		symbols.reserve(ranges.size());
		for (const auto& range : ranges) {
			symbols.push_back(JitSymbol {base + range.start, base + range.end, range.name, moduleIndex});
		}
		registry.addAll(std::move(symbols));
		// after the ranges: resolveLine bounds a lookup by the range that covers the address
		std::vector<JitLine> lines;
		lines.reserve(marks.size());
		for (const auto& mark : marks) {
			if (const auto line = lineOf(mark); line != 0) {
				if (!lines.empty() && lines.back().address == base + mark.offset) {
					lines.back().line = line; // an instruction without code of its own: the next one has it
				} else {
					lines.push_back(JitLine {base + mark.offset, line, moduleIndex});
				}
			}
		}
		registry.addLines(std::move(lines), sourceFile, moduleIndex);
	}
	if (jitdump) {
		auto& writer = PerfJitDump::instance();
		for (const auto& range : ranges) {
			std::vector<JitDumpLine> lines;
			for (auto k = range.firstMark; k < range.lastMark && k < marks.size(); ++k) {
				if (const auto line = lineOf(marks[k]);
				    line != 0 && marks[k].offset >= range.start && marks[k].offset < range.end) {
					lines.push_back(JitDumpLine {base + marks[k].offset, line, sourceFile});
				}
			}
			writer.writeCode(range.name, base + range.start, range.end - range.start, lines);
		}
	}
	return moduleIndex;
}

} // namespace nautilus::compiler

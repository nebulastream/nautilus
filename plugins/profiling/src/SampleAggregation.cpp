#include "SampleAggregation.hpp"
#include "JitSymbolResolver.hpp"
#include "RegionNames.hpp"
#include "nautilus/compiler/JitSymbolRegistry.hpp"
#include <algorithm>
#include <cstdint>
#include <linux/perf_event.h>
#include <map>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

namespace pc = ::perf;

namespace nautilus::profiling::detail {

namespace {

/// Appends one resolved address to @p frames, expanding a region-qualified JIT
/// name into the nesting it encodes so a flame graph stacks a region inside its
/// parent. Consecutive duplicates are collapsed: perf's callchain and the
/// sampled instruction pointer name the same leaf, and a region's outer levels
/// repeat between adjacent frames of the same chain.
void appendFrames(std::vector<Frame>& frames, const SymbolResolver::Resolution& resolution) {
	for (auto& name : expandRegionFrames(resolution.name)) {
		if (!frames.empty() && frames.back().name == name) {
			continue;
		}
		frames.push_back({std::move(name), resolution.origin});
	}
}

} // namespace

/// Aggregates the raw samples into the report, resolving each instruction
/// pointer exactly once per distinct address.
SampleReport aggregate(const pc::SampleResult& samples) {
	detail::SymbolResolver resolver;

	// Resolution is the expensive half (an ELF symbol table lookup on the
	// first host address), and a profile is by nature many samples over few
	// addresses, so each distinct address is resolved once and the counts
	// are accumulated against it.
	std::unordered_map<uintptr_t, detail::SymbolResolver::Resolution> byAddress;
	// Keyed by module *and* name, not name alone: every single-function
	// registration lowers to a function called literally `execute`, so two
	// modules compiled in one process would otherwise have their samples
	// silently added together. The addresses resolve correctly either way;
	// it is the aggregation that needs the module kept separate.
	std::map<std::pair<compiler::ModuleIndex, std::string>, SampleSite> bySymbol;
	// Distinct call stacks, keyed by their joined frame names. Populated only
	// when the sampler recorded call chains; without them a flame graph has
	// nothing but the leaf and whatever nesting its name encodes.
	std::map<std::string, SampleStack> byStack;
	// Samples per line of the Nautilus-IR dump, which is the level below the
	// symbol table: which IR operation inside a hot region is the hot one.
	std::map<uint32_t, uint64_t> byLine;
	std::string sourceFile;
	uint64_t total = 0;

	// Resolves an address through the per-address cache above.
	const auto resolveCached = [&](uintptr_t address) -> const detail::SymbolResolver::Resolution& {
		auto cached = byAddress.find(address);
		if (cached == byAddress.end()) {
			cached = byAddress.emplace(address, resolver.resolve(address)).first;
		}
		return cached->second;
	};

	for (const auto& sample : samples) {
		const auto pointer = sample.instruction_execution().logical_instruction_pointer();
		if (!pointer.has_value()) {
			continue;
		}
		++total;

		const auto& resolution = resolveCached(*pointer);

		auto& site = bySymbol[{resolution.moduleIndex, resolution.name}];
		if (site.samples == 0) {
			site.symbol = resolution.name;
			site.origin = resolution.origin;
			// Resolved to the full CompilationUnitID once per site rather
			// than per sample; SampleReport decides how much of it to show.
			site.module = compiler::JitSymbolRegistry::instance().moduleId(resolution.moduleIndex);
		}
		++site.samples;

		if (compiler::JitLine irLine; compiler::JitSymbolRegistry::instance().resolveLine(*pointer, irLine)) {
			++byLine[irLine.line];
			if (sourceFile.empty()) {
				sourceFile = compiler::JitSymbolRegistry::instance().sourceFile(irLine.moduleIndex);
			}
		}

		// Build this sample's stack, root frame first. perf records a callchain
		// leaf-to-root, so it is reversed; the sampled instruction pointer is
		// appended only when the chain does not already end there, since perf
		// usually includes the leaf and appending it again would duplicate every
		// frame its region name expands to.
		std::vector<uintptr_t> chain;
		const auto& callchain = sample.instruction_execution().callchain();
		if (callchain.has_value()) {
			chain.reserve(callchain->size() + 1);
			for (auto address = callchain->rbegin(); address != callchain->rend(); ++address) {
				// perf splices context markers (PERF_CONTEXT_KERNEL,
				// PERF_CONTEXT_USER, ...) into a callchain to say which address
				// space the frames after them belong to. They are sentinels, not
				// addresses, and resolving one yields a bogus
				// `0xfffffffffffffe00` frame in the middle of the stack.
				if (*address >= PERF_CONTEXT_MAX) {
					continue;
				}
				chain.push_back(static_cast<uintptr_t>(*address));
			}
		}
		if (chain.empty() || chain.back() != *pointer) {
			chain.push_back(*pointer);
		}

		std::vector<Frame> frames;
		frames.reserve(chain.size());
		for (const auto address : chain) {
			appendFrames(frames, resolveCached(address));
		}
		if (frames.empty()) {
			continue;
		}

		std::string key;
		for (const auto& frame : frames) {
			key += frame.name;
			key += '\x1f';
		}
		auto& stack = byStack[key];
		if (stack.samples == 0) {
			stack.frames = std::move(frames);
		}
		++stack.samples;
	}

	std::vector<SampleSite> sites;
	sites.reserve(bySymbol.size());
	for (auto& [key, site] : bySymbol) {
		site.share = total == 0 ? 0.0 : 100.0 * static_cast<double>(site.samples) / static_cast<double>(total);
		sites.push_back(std::move(site));
	}
	// Descending by samples, then by name and module so a tie is reported
	// the same way on every run rather than in container order.
	std::sort(sites.begin(), sites.end(), [](const SampleSite& left, const SampleSite& right) {
		if (left.samples != right.samples) {
			return left.samples > right.samples;
		}
		if (left.symbol != right.symbol) {
			return left.symbol < right.symbol;
		}
		return left.module < right.module;
	});

	std::vector<SampleStack> stacks;
	stacks.reserve(byStack.size());
	for (auto& [key, stack] : byStack) {
		stacks.push_back(std::move(stack));
	}
	std::sort(stacks.begin(), stacks.end(),
	          [](const SampleStack& left, const SampleStack& right) { return left.samples > right.samples; });

	std::vector<SourceLine> sourceLines;
	sourceLines.reserve(byLine.size());
	for (const auto& [line, samples] : byLine) {
		const double share = total == 0 ? 0.0 : 100.0 * static_cast<double>(samples) / static_cast<double>(total);
		sourceLines.push_back({line, samples, share});
	}

	return SampleReport(std::move(sites), std::move(stacks), std::move(sourceLines), std::move(sourceFile), total);
}

} // namespace nautilus::profiling::detail

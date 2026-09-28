#include "nautilus/tracing/CopyAttribution.hpp"
#include "nautilus/tracing/ExecutionTrace.hpp"
#include "nautilus/tracing/tag/SourceLocationResolver.hpp"
#include <algorithm>
#include <cstdint>
#include <fmt/format.h>
#include <map>
#include <string>
#include <string_view>
#include <vector>

namespace nautilus::tracing {

namespace {

enum class FrameKind { Tracer, Nautilus, User };

bool startsWith(std::string_view text, std::string_view prefix) {
	return text.substr(0, prefix.size()) == prefix;
}

/// Classifies a frame by its source file when it has one, and by its symbol otherwise (a binary without debug
/// information still names the function a return address lies in).
FrameKind classify(const SourceFrame& frame) {
	if (!frame.file.empty() && frame.line != 0) {
		if (frame.file.find("/src/nautilus/") != std::string::npos) {
			return FrameKind::Tracer;
		}
		if (frame.file.find("/include/nautilus/") != std::string::npos) {
			return FrameKind::Nautilus;
		}
		return FrameKind::User;
	}
	if (startsWith(frame.function, "nautilus::tracing::")) {
		return FrameKind::Tracer;
	}
	return FrameKind::User;
}

std::string describe(const SourceFrame& frame, TagAddress address) {
	if (frame.file.empty() && frame.function.empty()) {
		return fmt::format("{:#x}", address);
	}
	auto function = frame.function.empty() ? std::string("?") : frame.function;
	if (frame.file.empty() || frame.line == 0) {
		return function;
	}
	auto slash = frame.file.find_last_of('/');
	auto file = slash == std::string::npos ? frame.file : frame.file.substr(slash + 1);
	return fmt::format("{} ({}:{})", function, file, frame.line);
}

struct Attribution {
	/// The frames from the innermost one outside the tracer up to the first one outside nautilus, innermost first.
	std::vector<std::string> chain;
	/// The first frame outside nautilus: the embedder's code that made the copy.
	std::string origin;
	/// The outermost nautilus frame, i.e. the nautilus function the embedder called, or the copy itself when the
	/// embedder copied a val directly.
	std::string api;
};

Attribution attribute(const std::vector<TagAddress>& stack, SourceLocationResolver& resolver) {
	Attribution attribution;
	for (auto address : stack) {
		for (const auto& frame : resolver.resolveInlined(address)) {
			auto kind = classify(frame);
			if (kind == FrameKind::Tracer && attribution.chain.empty()) {
				continue;
			}
			if (kind == FrameKind::User) {
				attribution.api = attribution.chain.empty() ? std::string("<none>") : attribution.chain.back();
				attribution.chain.push_back(describe(frame, address));
				attribution.origin = attribution.chain.back();
				return attribution;
			}
			attribution.chain.push_back(describe(frame, address));
		}
	}
	return attribution;
}

void printTable(std::string& out, std::string_view title, const std::map<std::string, size_t>& counts, size_t total,
                size_t limit) {
	std::vector<std::pair<std::string_view, size_t>> rows(counts.begin(), counts.end());
	std::stable_sort(rows.begin(), rows.end(), [](const auto& a, const auto& b) { return a.second > b.second; });
	out += fmt::format("{} ({} distinct)\n", title, rows.size());
	out += fmt::format("{:>8}  {:>6}  {}\n", "copies", "share", "site");
	for (size_t i = 0; i < rows.size() && i < limit; i++) {
		out += fmt::format("{:>8}  {:>5.1f}%  {}\n", rows[i].second, 100.0 * rows[i].second / total, rows[i].first);
	}
	if (rows.size() > limit) {
		out += fmt::format("{:>8}  {:>6}  ... {} more\n", "", "", rows.size() - limit);
	}
	out += "\n";
}

} // namespace

std::string formatCopySites(const TraceModule& module, SourceLocationResolver& resolver, size_t limit) {
	std::map<std::string, size_t> byChain;
	std::map<std::string, size_t> byOrigin;
	std::map<std::string, size_t> byApi;
	size_t total = 0;
	for (const auto& name : module.getFunctionNames()) {
		for (const auto& stack : module.getFunction(name)->copySites) {
			auto attribution = attribute(stack, resolver);
			std::string chain;
			for (const auto& frame : attribution.chain) {
				chain += chain.empty() ? frame : "\n" + std::string(20, ' ') + "<- " + frame;
			}
			byChain[chain.empty() ? std::string("<unresolved>") : chain]++;
			byOrigin[attribution.origin.empty() ? std::string("<no frame outside nautilus>") : attribution.origin]++;
			byApi[attribution.api.empty() ? std::string("<no frame outside nautilus>") : attribution.api]++;
			total++;
		}
	}
	std::string out = fmt::format("{} copies traced (traceCopy: ASSIGN to a fresh value)\n\n", total);
	if (total == 0) {
		return out;
	}
	printTable(out, "by the nautilus function called from outside nautilus", byApi, total, limit);
	printTable(out, "by call chain, from the copy up to the first frame outside nautilus", byChain, total, limit);
	printTable(out, "by the first frame outside nautilus", byOrigin, total, limit);
	return out;
}

} // namespace nautilus::tracing

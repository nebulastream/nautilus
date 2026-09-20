#include "RegionNames.hpp"
#include "nautilus/profiling/sample_report.hpp"
#include <algorithm>
#include <cstdio>
#include <fstream>
#include <iomanip>
#include <map>
#include <sstream>
#include <unordered_map>
#include <unordered_set>
#include <utility>

namespace nautilus::profiling {

using detail::expandRegionFrames;
using detail::isQualifiedRegionName;

namespace {

const char* originName(SymbolOrigin origin) {
	switch (origin) {
	case SymbolOrigin::Jit:
		return "jit";
	case SymbolOrigin::Host:
		return "host";
	case SymbolOrigin::Unresolved:
		break;
	}
	return "unresolved";
}

std::string formatShare(double share) {
	char buffer[32];
	std::snprintf(buffer, sizeof(buffer), "%6.2f%%", share);
	return buffer;
}

std::string shortModuleId(const std::string& moduleId) {
	// A CompilationUnitID reads `2026-09-20_11-09-32.063927204_#831093F`: a
	// timestamp for ordering plus a random suffix for uniqueness. Only the
	// suffix distinguishes two compiles that a profile holds at once, and it is
	// short enough to sit in a symbol column, so that is what gets printed. The
	// full id stays on the site for anyone correlating with a dump directory.
	const auto hash = moduleId.rfind('#');
	if (hash != std::string::npos && hash + 1 < moduleId.size()) {
		return moduleId.substr(hash + 1);
	}
	return moduleId;
}

/// Appends `@<module>` to every symbol whose name more than one compile
/// contributed.
///
/// Only on collision, for two reasons. A single-module profile -- the common
/// case, and every profile before several modules were live at once -- reads
/// exactly as it always did. And a CompilationUnitID embeds a timestamp and a
/// random suffix, so it differs on every run: a test asserting on a symbol name
/// would be flaky if the qualification were unconditional, and stays stable
/// while its profile holds one module.
void qualifyAmbiguousNames(std::vector<SampleSite>& sites) {
	std::unordered_map<std::string, std::string> firstModuleFor;
	std::unordered_set<std::string> ambiguous;
	for (const auto& site : sites) {
		if (site.module.empty()) {
			continue;
		}
		const auto [entry, inserted] = firstModuleFor.emplace(site.symbol, site.module);
		if (!inserted && entry->second != site.module) {
			ambiguous.insert(site.symbol);
		}
	}
	if (ambiguous.empty()) {
		return;
	}
	for (auto& site : sites) {
		if (!site.module.empty() && ambiguous.count(site.symbol) != 0) {
			site.symbol += " @" + shortModuleId(site.module);
		}
	}
}

} // namespace

SampleReport::SampleReport(std::vector<SampleSite> sites, uint64_t total) : SampleReport(std::move(sites), {}, total) {
}

SampleReport::SampleReport(std::vector<SampleSite> sites, std::vector<SampleStack> stacks, uint64_t total)
    : SampleReport(std::move(sites), std::move(stacks), {}, {}, total) {
}

SampleReport::SampleReport(std::vector<SampleSite> sites, std::vector<SampleStack> stacks,
                           std::vector<SourceLine> sourceLines, std::string sourceFile, uint64_t total)
    : sites_(std::move(sites)), stacks_(std::move(stacks)), sourceLines_(std::move(sourceLines)),
      sourceFile_(std::move(sourceFile)), total_(total) {
	qualifyAmbiguousNames(sites_);
}

const std::vector<SourceLine>& SampleReport::sourceLines() const noexcept {
	return sourceLines_;
}

const std::string& SampleReport::sourceFile() const noexcept {
	return sourceFile_;
}

const std::vector<SampleStack>& SampleReport::stacks() const noexcept {
	return stacks_;
}

const std::vector<SampleSite>& SampleReport::sites() const noexcept {
	return sites_;
}

uint64_t SampleReport::total() const noexcept {
	return total_;
}

std::vector<std::string> SampleReport::modules() const {
	std::vector<std::string> seen;
	for (const auto& site : sites_) {
		if (site.module.empty()) {
			continue;
		}
		if (std::find(seen.begin(), seen.end(), site.module) == seen.end()) {
			seen.push_back(site.module);
		}
	}
	return seen;
}

uint64_t SampleReport::jitSamples() const noexcept {
	uint64_t count = 0;
	for (const auto& site : sites_) {
		if (site.origin == SymbolOrigin::Jit) {
			count += site.samples;
		}
	}
	return count;
}

bool SampleReport::empty() const noexcept {
	return sites_.empty();
}

std::string SampleReport::table(size_t top) const {
	if (sites_.empty()) {
		return {};
	}

	std::ostringstream out;
	out << "# " << total_ << " samples, "
	    << formatShare(total_ == 0 ? 0.0 : 100.0 * static_cast<double>(jitSamples()) / static_cast<double>(total_))
	    << " in JIT-compiled code\n";
	out << "# overhead  origin      symbol\n";

	const size_t rows = std::min(top, sites_.size());
	for (size_t index = 0; index < rows; ++index) {
		const auto& site = sites_[index];
		out << "  " << formatShare(site.share) << "  ";
		std::string origin = originName(site.origin);
		origin.resize(10, ' ');
		out << origin << "  " << site.symbol << '\n';
	}
	if (rows < sites_.size()) {
		out << "  ... " << (sites_.size() - rows) << " more\n";
	}
	return out.str();
}

std::string SampleReport::csv() const {
	std::ostringstream out;
	out << "share,samples,origin,module,symbol\n";
	for (const auto& site : sites_) {
		char share[32];
		std::snprintf(share, sizeof(share), "%.4f", site.share);
		out << share << ',' << site.samples << ',' << originName(site.origin) << ',' << site.module << ','
		    << site.symbol << '\n';
	}
	return out.str();
}

std::string SampleReport::foldedStacks() const {
	std::ostringstream out;
	if (!stacks_.empty()) {
		// Real call chains: every frame is already a frame, and a JIT frame's
		// region nesting was expanded when the stack was built.
		for (const auto& stack : stacks_) {
			for (size_t index = 0; index < stack.frames.size(); ++index) {
				if (index > 0) {
					out << ';';
				}
				out << stack.frames[index].name;
			}
			out << ' ' << stack.samples << '\n';
		}
		return out.str();
	}

	// Leaf-only: the nesting a region-qualified name encodes is all there is.
	for (const auto& site : sites_) {
		if (!isQualifiedRegionName(site.symbol)) {
			out << site.symbol << ' ' << site.samples << '\n';
			continue;
		}
		const auto frames = expandRegionFrames(site.symbol);
		for (size_t index = 0; index < frames.size(); ++index) {
			if (index > 0) {
				out << ';';
			}
			out << frames[index];
		}
		out << ' ' << site.samples << '\n';
	}
	return out.str();
}

namespace {

/// One node of the flame graph tree.
struct FlameNode {
	std::string name;
	SymbolOrigin origin = SymbolOrigin::Unresolved;
	uint64_t samples = 0;
	std::map<std::string, FlameNode> children;
};

void insertStack(FlameNode& root, const std::vector<Frame>& frames, uint64_t samples) {
	root.samples += samples;
	FlameNode* node = &root;
	for (const auto& frame : frames) {
		auto& child = node->children[frame.name];
		if (child.name.empty()) {
			child.name = frame.name;
			child.origin = frame.origin;
		}
		child.samples += samples;
		node = &child;
	}
}

/// Fill for a frame, chosen by where its code came from rather than by the
/// usual hash-of-the-name hue: which frames are JIT-compiled is the question a
/// nautilus profile is being read to answer.
const char* frameFill(SymbolOrigin origin) {
	switch (origin) {
	case SymbolOrigin::Jit:
		return "#e8663d";
	case SymbolOrigin::Host:
		return "#6a9fd4";
	case SymbolOrigin::Unresolved:
		break;
	}
	return "#b0b0b0";
}

/// XML-escapes a symbol name. Template arguments in a demangled C++ name make
/// this mandatory, not defensive: `vector<int>` would otherwise close the
/// enclosing element.
std::string escapeXml(std::string_view text) {
	std::string escaped;
	escaped.reserve(text.size());
	for (const char character : text) {
		switch (character) {
		case '&':
			escaped += "&amp;";
			break;
		case '<':
			escaped += "&lt;";
			break;
		case '>':
			escaped += "&gt;";
			break;
		case '"':
			escaped += "&quot;";
			break;
		case '\'':
			escaped += "&apos;";
			break;
		default:
			escaped += character;
		}
	}
	return escaped;
}

constexpr double FRAME_HEIGHT = 17.0;
constexpr double WIDTH = 1200.0;
constexpr double MARGIN = 10.0;
constexpr double HEADER = 34.0;

/// Emits one frame and its children, left to right, growing upward.
void renderNode(std::ostringstream& out, const FlameNode& node, double x, double depth, double perSample,
                uint64_t total, double maxDepth) {
	const double width = node.samples * perSample;
	// Sub-pixel frames cannot be read or hovered, and a deep profile has a long
	// tail of them; dropping them keeps the document a sane size.
	if (width < 0.4) {
		return;
	}
	const double y = MARGIN + HEADER + (maxDepth - depth - 1) * FRAME_HEIGHT;
	const double share = total == 0 ? 0.0 : 100.0 * static_cast<double>(node.samples) / static_cast<double>(total);

	char buffer[128];
	std::snprintf(buffer, sizeof(buffer), "%.2f", share);

	out << "<g><title>" << escapeXml(node.name) << " -- " << node.samples << " samples (" << buffer << "%)</title>"
	    << "<rect x=\"" << x << "\" y=\"" << y << "\" width=\"" << width << "\" height=\"" << (FRAME_HEIGHT - 1)
	    << "\" fill=\"" << frameFill(node.origin) << "\" rx=\"2\"/>";
	// Only label a frame wide enough to hold readable text, truncated to fit.
	const auto characters = static_cast<size_t>((width - 6.0) / 6.2);
	if (characters >= 3) {
		std::string label = node.name;
		if (label.size() > characters) {
			label = label.substr(0, characters - 2) + "..";
		}
		out << "<text x=\"" << (x + 3.0) << "\" y=\"" << (y + 12.0) << "\">" << escapeXml(label) << "</text>";
	}
	out << "</g>\n";

	double childX = x;
	for (const auto& [name, child] : node.children) {
		renderNode(out, child, childX, depth + 1, perSample, total, maxDepth);
		childX += child.samples * perSample;
	}
}

double treeDepth(const FlameNode& node) {
	double deepest = 0.0;
	for (const auto& [name, child] : node.children) {
		deepest = std::max(deepest, treeDepth(child));
	}
	return deepest + 1.0;
}

} // namespace

std::string SampleReport::flameGraph(std::string_view title) const {
	FlameNode root;
	root.name = "all";
	if (!stacks_.empty()) {
		for (const auto& stack : stacks_) {
			insertStack(root, stack.frames, stack.samples);
		}
	} else {
		// No call chains recorded: synthesize the only nesting there is, the
		// one a region-qualified name carries.
		for (const auto& site : sites_) {
			std::vector<Frame> frames;
			for (auto& name : expandRegionFrames(site.symbol)) {
				frames.push_back({std::move(name), site.origin});
			}
			insertStack(root, frames, site.samples);
		}
	}

	if (root.samples == 0) {
		return {};
	}

	const double depth = treeDepth(root);
	const double height = MARGIN * 2.0 + HEADER + depth * FRAME_HEIGHT;
	const double perSample = (WIDTH - MARGIN * 2.0) / static_cast<double>(root.samples);

	std::ostringstream out;
	out << "<?xml version=\"1.0\" standalone=\"no\"?>\n"
	    << "<svg version=\"1.1\" width=\"" << WIDTH << "\" height=\"" << height
	    << "\" xmlns=\"http://www.w3.org/2000/svg\">\n"
	    << "<style>text{font-family:monospace;font-size:11px;fill:#111;pointer-events:none}"
	    << "rect{stroke:#fff;stroke-width:0.5}"
	    << ".hdr{font-size:13px;font-weight:bold}.key{font-size:10px}</style>\n"
	    << "<rect x=\"0\" y=\"0\" width=\"" << WIDTH << "\" height=\"" << height
	    << "\" fill=\"#f8f8f8\" stroke=\"none\"/>\n"
	    << "<text class=\"hdr\" x=\"" << MARGIN << "\" y=\"18\">" << escapeXml(title) << "</text>\n";

	// A legend, because the colours carry meaning here rather than being
	// decorative.
	double keyX = MARGIN;
	for (const auto& [origin, label] :
	     {std::pair {SymbolOrigin::Jit, "JIT-compiled"}, std::pair {SymbolOrigin::Host, "host"},
	      std::pair {SymbolOrigin::Unresolved, "unresolved"}}) {
		out << "<rect x=\"" << keyX << "\" y=\"24\" width=\"9\" height=\"9\" fill=\"" << frameFill(origin) << "\"/>"
		    << "<text class=\"key\" x=\"" << (keyX + 13.0) << "\" y=\"32\">" << label << "</text>\n";
		keyX += 13.0 + 6.2 * static_cast<double>(std::string_view(label).size()) + 14.0;
	}

	double childX = MARGIN;
	for (const auto& [name, child] : root.children) {
		renderNode(out, child, childX, 0.0, perSample, root.samples, depth - 1.0);
		childX += child.samples * perSample;
	}

	out << "</svg>\n";
	return out.str();
}

std::string SampleReport::annotateSource(const std::string& path, size_t context) const {
	const std::string& file = path.empty() ? sourceFile_ : path;
	if (file.empty()) {
		return "no Nautilus-IR dump to annotate: the compile published no line table "
		       "(is `perf.sample` set, with the MLIR backend pinned?)\n";
	}

	std::ifstream input(file);
	if (!input) {
		return "could not read '" + file +
		       "': a perf-enabled compile writes the IR dump to its working directory, "
		       "and nothing cleans it up or stops it being moved -- annotation needs it "
		       "where the samples point.\n";
	}

	// Line numbers are 1-based and sparse; a vector indexed by line is simpler
	// than a map and costs a few hundred bytes for a dump of any realistic size.
	uint32_t highest = 0;
	for (const auto& entry : sourceLines_) {
		highest = std::max(highest, entry.line);
	}
	std::vector<double> shareByLine(highest + 1, 0.0);
	std::vector<uint64_t> samplesByLine(highest + 1, 0);
	uint64_t attributed = 0;
	for (const auto& entry : sourceLines_) {
		shareByLine[entry.line] += entry.share;
		samplesByLine[entry.line] += entry.samples;
		attributed += entry.samples;
	}

	std::vector<std::string> lines;
	for (std::string text; std::getline(input, text);) {
		lines.push_back(std::move(text));
	}

	// With a context window, only lines near an annotated one are printed --
	// a dump of a real query is thousands of lines, most of them cold.
	std::vector<char> keep(lines.size(), context == 0 ? 1 : 0);
	if (context > 0) {
		for (uint32_t line = 1; line <= highest; ++line) {
			if (samplesByLine[line] == 0) {
				continue;
			}
			const size_t index = line - 1;
			const size_t from = index < context ? 0 : index - context;
			const size_t to = std::min(lines.size(), index + context + 1);
			for (size_t keepIndex = from; keepIndex < to; ++keepIndex) {
				keep[keepIndex] = 1;
			}
		}
	}

	std::ostringstream out;
	const double attributedShare =
	    total_ == 0 ? 0.0 : 100.0 * static_cast<double>(attributed) / static_cast<double>(total_);
	char header[64];
	std::snprintf(header, sizeof(header), "%.2f", attributedShare);
	out << "# annotation of " << file << '\n'
	    << "# " << header << "% of all samples land in this file; the rest is host code,\n"
	    << "# the kernel, and the in-process MLIR/LLVM compile.\n"
	    << "# columns: percent of all samples | IR line | IR\n#\n";

	bool skipping = false;
	for (size_t index = 0; index < lines.size(); ++index) {
		if (!keep[index]) {
			skipping = true;
			continue;
		}
		if (skipping) {
			out << "       | ... |\n";
			skipping = false;
		}
		const uint32_t line = static_cast<uint32_t>(index + 1);
		const double share = line <= highest ? shareByLine[line] : 0.0;
		if (share > 0.0) {
			char percent[32];
			std::snprintf(percent, sizeof(percent), "%6.2f", share);
			out << percent;
		} else {
			out << "      ";
		}
		out << " | " << std::setw(4) << line << " | " << lines[index] << '\n';
	}
	if (skipping) {
		// A trailing elided stretch needs the marker too, or the output simply
		// stops and the reader cannot tell a truncated dump from a finished one.
		out << "       | ... |\n";
	}
	return out.str();
}

bool SampleReport::writeFlameGraph(const std::string& path, std::string_view title) const {
	std::ofstream file(path, std::ios::trunc);
	if (!file) {
		return false;
	}
	file << flameGraph(title);
	return static_cast<bool>(file);
}

} // namespace nautilus::profiling

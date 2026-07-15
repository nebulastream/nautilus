// Scans the Nautilus dump directories produced by one compile and writes a
// single JSON manifest with every pipeline-stage artifact.
//
// One module compile creates TWO dump directories under
// `$TMPDIR/dump/<compilationId>/`: CompilationPipeline::compileToIR and
// ::compileIR each construct their own DumpHandler with a fresh compilation
// unit ID. The entrypoint gives every run a fresh TMPDIR, so scanning
// `$TMPDIR/dump/*/*` deterministically yields exactly this run's files.
//
// The manifest is written to a file (not stdout) because `dump.file=true`
// makes DumpHandler print a `stage -- id -- file://...` line to stdout for
// every dumped stage.
#pragma once

#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <string>
#include <string_view>
#include <vector>

namespace playground {

// A single artifact is capped so adversarial inputs (huge unrolled traces)
// cannot produce an unbounded manifest.
constexpr std::uintmax_t MAX_ARTIFACT_BYTES = 2 * 1024 * 1024;

struct StageMeta {
	std::string_view key;
	std::string_view phase; // frontend | pass | graph | backend
	std::string_view title;
	int order;
};

// Canonical stage ordering. Frontend stage names come from
// CompilationPipeline::compileToIR, per-pass names are "after_" + getName()
// of each pass in the order IRPassManager's fixed-point group runs them,
// graph names come from GraphVizUtil, and backend names from the backends'
// dump calls. Per-pass files are only written when the pass changed the IR
// and are overwritten each fixed-point iteration, so they show the state
// after the LAST iteration in which the pass fired.
inline constexpr StageMeta STAGE_TABLE[] = {
    {"after_tracing", "frontend", "Execution trace", 0},
    {"after_ssa", "frontend", "Trace (SSA form)", 1},
    {"after_ir_creation", "frontend", "IR (initial)", 2},
    {"mermaid", "graph", "CFG (Mermaid, initial IR)", 3},
    {"graphviz", "graph", "CFG (Graphviz, initial IR)", 4},
    {"after_ConstantFoldingAndCopyPropagation", "pass", "After constant folding & copy propagation", 10},
    {"after_AlgebraicSimplification", "pass", "After algebraic simplification", 11},
    {"after_ConstantBranchFolding", "pass", "After constant branch folding", 12},
    {"after_EmptyBlockElimination", "pass", "After empty block elimination", 13},
    {"after_BlockMerging", "pass", "After block merging", 14},
    {"after_LocalCSE", "pass", "After local CSE", 15},
    {"after_StrengthReduction", "pass", "After strength reduction", 16},
    {"after_DeadCodeElimination", "pass", "After dead code elimination", 17},
    {"after_BlockArgumentPruning", "pass", "After block argument pruning", 18},
    {"after_LoopInvariantCodeMotion", "pass", "After loop-invariant code motion", 19},
    {"after_ir_passes", "frontend", "IR (optimized)", 30},
    {"after_mlir_generation", "backend", "MLIR", 40},
    {"before_llvm_optimization", "backend", "LLVM IR (before optimization)", 41},
    {"after_llvm_generation", "backend", "LLVM IR (optimized)", 42},
    {"after_c_generation", "backend", "Generated C", 43},
    {"after_cpp_generation", "backend", "Generated C++", 43},
    {"after_bc_generation", "backend", "Bytecode", 44},
    {"after_tbc_generation", "backend", "Tiered bytecode", 45},
    {"after_asmjit_generation", "backend", "AsmJit IR", 46},
    {"after_asmjit_assembly", "backend", "Assembly", 47},
    {"after_cuda_generation", "backend", "Generated CUDA", 48},
    {"after_metal_generation", "backend", "Metal shader", 49},
    {"after_metal_host_generation", "backend", "Metal host (Obj-C++)", 50},
};

struct StageEntry {
	std::string key;
	std::string phase;
	std::string title;
	std::string ext;
	std::string text;
	int order = 0;
	bool truncated = false;
};

// One CompilationStatistics entry, pre-encoded by the driver so this header
// stays independent of nautilus types. `valueJson` is a ready-to-emit JSON
// value (raw number or quoted string).
struct StatEntry {
	std::string key;
	std::string type; // counter | duration | text
	std::string valueJson;
};

inline std::string jsonEscape(std::string_view input) {
	std::string out;
	out.reserve(input.size() + 16);
	for (const char c : input) {
		switch (c) {
		case '"':
			out += "\\\"";
			break;
		case '\\':
			out += "\\\\";
			break;
		case '\n':
			out += "\\n";
			break;
		case '\r':
			out += "\\r";
			break;
		case '\t':
			out += "\\t";
			break;
		default:
			if (static_cast<unsigned char>(c) < 0x20) {
				char buf[8];
				std::snprintf(buf, sizeof(buf), "\\u%04x", static_cast<unsigned char>(c));
				out += buf;
			} else {
				out += c;
			}
		}
	}
	return out;
}

inline const StageMeta* findStageMeta(std::string_view key) {
	for (const auto& meta : STAGE_TABLE) {
		if (meta.key == key) {
			return &meta;
		}
	}
	return nullptr;
}

// Backend code generators emit C-family sources without any layout (one
// declaration per line, no indentation). Pretty-print them with the
// clang-format shipped in the runner image so the playground shows readable
// code. Formatting failures (missing binary, syntax the formatter rejects)
// fall back to the raw text.
inline bool isCFamilyExt(std::string_view ext) {
	return ext == "c" || ext == "cpp" || ext == "cu" || ext == "metal" || ext == "mm";
}

inline std::string clangFormatFile(const std::filesystem::path& file, std::string_view ext) {
	// --assume-filename picks the language grammar; Metal Shading Language is
	// C++-based, Objective-C++ needs the objc grammar. Dump paths are
	// generated by DumpHandler and contain no shell metacharacters.
	const std::string assumeName = ext == "mm" ? "code.mm" : "code.cpp";
	const std::string cmd = "clang-format-21 --style='{BasedOnStyle: LLVM, IndentWidth: 4, ColumnLimit: 120}'"
	                        " --assume-filename=" +
	                        assumeName + " < '" + file.string() + "' 2>/dev/null";
	FILE* pipe = popen(cmd.c_str(), "r");
	if (pipe == nullptr) {
		return {};
	}
	std::string out;
	char buf[4096];
	size_t n = 0;
	while ((n = fread(buf, 1, sizeof(buf), pipe)) > 0) {
		out.append(buf, n);
	}
	if (pclose(pipe) != 0) {
		return {};
	}
	return out;
}

inline StageEntry readStageFile(const std::filesystem::path& file) {
	StageEntry entry;
	const auto filename = file.filename().string();
	// Filenames are `<stage>.<ext>`; the cpp backend passes ".c" as the
	// extension, producing `after_c_generation..c`, so split on the FIRST
	// dot and strip any leading dots from the extension.
	const auto dot = filename.find('.');
	entry.key = filename.substr(0, dot);
	if (dot != std::string::npos) {
		auto ext = filename.substr(dot + 1);
		while (!ext.empty() && ext.front() == '.') {
			ext.erase(ext.begin());
		}
		entry.ext = ext;
	}

	if (const auto* meta = findStageMeta(entry.key)) {
		entry.phase = meta->phase;
		entry.title = meta->title;
		entry.order = meta->order;
	} else {
		// Unknown stage (e.g. a pass added after this table was written):
		// keep it visible instead of dropping it.
		entry.phase = (entry.key.starts_with("after_") && entry.ext == "ir") ? "pass" : "other";
		entry.title = entry.key;
		entry.order = 100;
	}

	std::ifstream in(file, std::ios::binary);
	if (in) {
		const auto size = std::filesystem::file_size(file);
		const auto toRead = std::min<std::uintmax_t>(size, MAX_ARTIFACT_BYTES);
		entry.text.resize(static_cast<size_t>(toRead));
		in.read(entry.text.data(), static_cast<std::streamsize>(toRead));
		entry.truncated = size > MAX_ARTIFACT_BYTES;
	}

	// Only complete artifacts are formatted: a truncated source would make
	// the formatter fail (or lie about the tail), and huge inputs would eat
	// into the driver's run timeout.
	if (!entry.truncated && isCFamilyExt(entry.ext)) {
		auto formatted = clangFormatFile(file, entry.ext);
		if (!formatted.empty()) {
			if (formatted.size() > MAX_ARTIFACT_BYTES) {
				formatted.resize(MAX_ARTIFACT_BYTES);
				entry.truncated = true;
			}
			entry.text = std::move(formatted);
		}
	}
	return entry;
}

inline std::vector<StageEntry> collectStages() {
	std::vector<StageEntry> stages;
	const auto dumpRoot = std::filesystem::temp_directory_path() / "dump";
	if (!std::filesystem::exists(dumpRoot)) {
		return stages;
	}
	for (const auto& unitDir : std::filesystem::directory_iterator(dumpRoot)) {
		if (!unitDir.is_directory()) {
			continue;
		}
		for (const auto& file : std::filesystem::directory_iterator(unitDir.path())) {
			if (file.is_regular_file()) {
				stages.push_back(readStageFile(file.path()));
			}
		}
	}
	std::stable_sort(stages.begin(), stages.end(), [](const StageEntry& a, const StageEntry& b) {
		return a.order != b.order ? a.order < b.order : a.key < b.key;
	});
	return stages;
}

// Writes `<outDir>/manifest.json`. On pipeline failure the caller passes the
// exception text via `error`; whatever stages were dumped before the failure
// are still included so partial results remain inspectable.
inline void writeManifest(const std::filesystem::path& outDir, const std::string& backend,
                          const std::string& error = "", const std::vector<StatEntry>& statistics = {}) {
	const auto stages = collectStages();
	std::ofstream out(outDir / "manifest.json", std::ios::binary | std::ios::trunc);
	out << "{\"schema\":1,\"backend\":\"" << jsonEscape(backend) << "\"";
	if (!error.empty()) {
		out << ",\"error\":\"" << jsonEscape(error) << "\"";
	}
	if (!statistics.empty()) {
		out << ",\"statistics\":[";
		bool firstStat = true;
		for (const auto& stat : statistics) {
			if (!firstStat) {
				out << ",";
			}
			firstStat = false;
			out << "{\"key\":\"" << jsonEscape(stat.key) << "\",\"type\":\"" << jsonEscape(stat.type)
			    << "\",\"value\":" << stat.valueJson << "}";
		}
		out << "]";
	}
	out << ",\"stages\":[";
	bool first = true;
	for (const auto& stage : stages) {
		if (!first) {
			out << ",";
		}
		first = false;
		out << "{\"key\":\"" << jsonEscape(stage.key) << "\",\"title\":\"" << jsonEscape(stage.title)
		    << "\",\"phase\":\"" << jsonEscape(stage.phase) << "\",\"ext\":\"" << jsonEscape(stage.ext)
		    << "\",\"order\":" << stage.order << ",\"truncated\":" << (stage.truncated ? "true" : "false")
		    << ",\"text\":\"" << jsonEscape(stage.text) << "\"}";
	}
	out << "]}\n";
}

} // namespace playground

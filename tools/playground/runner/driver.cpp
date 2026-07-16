// Playground driver: compiles the user-provided module with every pipeline
// dump enabled and writes a JSON manifest of all stage artifacts.
//
// The user's translation unit (user_module.cpp, compiled per request by
// entrypoint.sh) defines `playground_register`, which registers one or more
// traced functions on the module — exactly the NautilusModule API from
// example/src/DemoModules.cpp. This driver TU is prebuilt into the runner
// image; only the user TU is compiled per request.

#include "manifest.hpp"
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <nautilus/Engine.hpp>
#include <string>
#include <string_view>
#include <variant>
#include <vector>

// Implemented by the user's translation unit.
extern void playground_register(nautilus::engine::NautilusModule& m);

namespace {

struct DriverArgs {
	std::string backend = "mlir";
	std::string outDir = "/out";
	bool enableLICM = false;
	bool enableLocalCSE = false;
	bool enableStrengthReduction = false;
	bool enableDwarf = false;
	int maxPipelineIterations = 4;
};

DriverArgs parseArgs(int argc, char** argv) {
	DriverArgs args;
	for (int i = 1; i < argc; ++i) {
		const std::string_view arg = argv[i];
		if (arg.starts_with("--backend=")) {
			args.backend = arg.substr(10);
		} else if (arg.starts_with("--out=")) {
			args.outDir = arg.substr(6);
		} else if (arg == "--enable-licm") {
			args.enableLICM = true;
		} else if (arg == "--enable-local-cse") {
			args.enableLocalCSE = true;
		} else if (arg == "--enable-strength-reduction") {
			args.enableStrengthReduction = true;
		} else if (arg == "--enable-dwarf") {
			args.enableDwarf = true;
		} else if (arg.starts_with("--max-iterations=")) {
			args.maxPipelineIterations = std::atoi(std::string(arg.substr(17)).c_str());
			if (args.maxPipelineIterations < 1 || args.maxPipelineIterations > 8) {
				args.maxPipelineIterations = 4;
			}
		} else {
			std::cerr << "unknown argument: " << arg << "\n";
		}
	}
	return args;
}

// Encode the compilation statistics (always collected by the engine) as
// pre-serialized JSON entries for the manifest. Counters map to raw numbers,
// timings (double, milliseconds) keep sub-ms precision, text stays a string.
std::vector<playground::StatEntry> encodeStatistics(const nautilus::compiler::CompilationStatistics* statistics) {
	std::vector<playground::StatEntry> entries;
	if (statistics == nullptr) {
		return entries;
	}
	for (const auto& [key, value] : *statistics) {
		playground::StatEntry entry;
		entry.key = key;
		if (const auto* counter = std::get_if<int64_t>(&value)) {
			entry.type = "counter";
			entry.valueJson = std::to_string(*counter);
		} else if (const auto* duration = std::get_if<double>(&value)) {
			entry.type = "duration";
			char buf[32];
			std::snprintf(buf, sizeof(buf), "%.6g", *duration);
			entry.valueJson = buf;
		} else {
			entry.type = "text";
			entry.valueJson = "\"" + playground::jsonEscape(std::get<std::string>(value)) + "\"";
		}
		entries.push_back(std::move(entry));
	}
	return entries;
}

} // namespace

int main(int argc, char** argv) {
	const auto args = parseArgs(argc, argv);

	nautilus::engine::Options options;
	options.setOption("engine.backend", args.backend);
	// Single-tier, synchronous compile: no background promotion threads and
	// no second dump directory from a tier-1 recompile.
	options.setOption("engine.tiered.backgroundPromotion", false);
	options.setOption("dump.all", true);
	options.setOption("ir.dumpAfterEachPass", true);
	options.setOption("dump.graph", true);
	options.setOption("dump.graph.type", std::string("mermaid"));
	options.setOption("ir.maxPipelineIterations", args.maxPipelineIterations);
	// The runner image ships no GPU toolchain (nvcc / xcrun): the CUDA and
	// Metal backends stop after emitting device/host sources, which is all
	// the playground needs to display.
	options.setOption("gpu.codegenOnly", true);
	if (args.enableLICM) {
		options.setOption("ir.enableLICM", true);
	}
	if (args.enableLocalCSE) {
		options.setOption("ir.enableLocalCSE", true);
	}
	if (args.enableStrengthReduction) {
		options.setOption("ir.enableStrengthReduction", true);
	}
	if (args.enableDwarf) {
		// Only consumed by the MLIR backend; harmless (ignored) on the others.
		options.setOption("mlir.debug.enable", true);
	}

	std::vector<playground::StatEntry> statistics;
	try {
		const auto engine = nautilus::engine::NautilusEngine(options);
		auto module = engine.createModule();
		playground_register(module);
		auto compiled = module.compile();
		statistics = encodeStatistics(compiled.getStatistics().get());
	} catch (const std::exception& e) {
		// Still write the manifest: stages dumped before the failure remain
		// inspectable, and the error text tells the user what broke.
		std::cerr << "compilation failed: " << e.what() << "\n";
		playground::writeManifest(args.outDir, args.backend, e.what());
		return 13;
	}

	playground::writeManifest(args.outDir, args.backend, "", statistics);
	return 0;
}

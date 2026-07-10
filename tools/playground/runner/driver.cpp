// Playground driver: compiles the user-provided module with every pipeline
// dump enabled and writes a JSON manifest of all stage artifacts.
//
// The user's translation unit (user_module.cpp, compiled per request by
// entrypoint.sh) defines `playground_register`, which registers one or more
// traced functions on the module — exactly the NautilusModule API from
// example/src/DemoModules.cpp. This driver TU is prebuilt into the runner
// image; only the user TU is compiled per request.

#include "manifest.hpp"
#include <cstdlib>
#include <iostream>
#include <nautilus/Engine.hpp>
#include <string>
#include <string_view>

// Implemented by the user's translation unit.
extern void playground_register(nautilus::engine::NautilusModule& m);

namespace {

struct DriverArgs {
	std::string backend = "mlir";
	std::string outDir = "/out";
	bool enableLICM = false;
	bool enableLocalCSE = false;
	bool enableStrengthReduction = false;
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
	if (args.enableLICM) {
		options.setOption("ir.enableLICM", true);
	}
	if (args.enableLocalCSE) {
		options.setOption("ir.enableLocalCSE", true);
	}
	if (args.enableStrengthReduction) {
		options.setOption("ir.enableStrengthReduction", true);
	}

	try {
		const auto engine = nautilus::engine::NautilusEngine(options);
		auto module = engine.createModule();
		playground_register(module);
		auto compiled = module.compile();
		(void) compiled;
	} catch (const std::exception& e) {
		// Still write the manifest: stages dumped before the failure remain
		// inspectable, and the error text tells the user what broke.
		std::cerr << "compilation failed: " << e.what() << "\n";
		playground::writeManifest(args.outDir, args.backend, e.what());
		return 13;
	}

	playground::writeManifest(args.outDir, args.backend);
	return 0;
}

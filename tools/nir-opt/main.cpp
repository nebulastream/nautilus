// nautilus-nir-opt: read NIR (text or binary image), verify it and print it back in canonical form.
//
//   nautilus-nir-opt [options] [file | -]
//
//   --no-verify      do not verify after parsing
//   --verify-only    print nothing on success
//   --hash           print the structural hash (13-nir-spec.md §7.2) of the module with the default options
//   --image=FILE     write the canonical binary image of the module to FILE
//   --inspect[=hex]  describe a binary image (header and section table, or an annotated hex dump)
//
// The input is a binary image if it starts with the magic "NIR\0", else text. Passes (M3) will be selected with
// --passes once they exist. Exit status: 0 success, 1 parse, load or verifier error, 2 usage or I/O error.
#include "nautilus/nir/Hash.hpp"
#include "nautilus/nir/ImageLoader.hpp"
#include "nautilus/nir/Inspect.hpp"
#include "nautilus/nir/Parser.hpp"
#include "nautilus/nir/Printer.hpp"
#include <fstream>
#include <iostream>
#include <iterator>
#include <sstream>
#include <string>

int main(int argc, char** argv) {
	using namespace nautilus::nir;
	ParseOptions options;
	bool verifyOnly = false, hash = false, inspect = false, hexDump = false;
	std::string path = "-", imagePath;
	for (int i = 1; i < argc; ++i) {
		std::string arg = argv[i];
		if (arg == "--no-verify") {
			options.verify = false;
		} else if (arg == "--verify-only") {
			verifyOnly = true;
		} else if (arg == "--hash") {
			hash = true;
		} else if (arg.rfind("--image=", 0) == 0) {
			imagePath = arg.substr(8);
		} else if (arg == "--inspect" || arg == "--inspect=hex") {
			inspect = true;
			hexDump = arg == "--inspect=hex";
		} else if (arg == "-h" || arg == "--help") {
			std::cout << "usage: nautilus-nir-opt [--no-verify] [--verify-only] [--hash] [--image=FILE] "
			             "[--inspect[=hex]] [file | -]\n";
			return 0;
		} else if (arg.size() > 1 && arg[0] == '-') {
			std::cerr << "unknown option " << arg << "\n";
			return 2;
		} else {
			path = arg;
		}
	}
	std::string input;
	{
		std::stringstream text;
		if (path == "-") {
			text << std::cin.rdbuf();
		} else {
			std::ifstream in(path, std::ios::binary);
			if (!in) {
				std::cerr << "cannot read " << path << "\n";
				return 2;
			}
			text << in.rdbuf();
		}
		input = text.str();
	}
	auto bytes = std::span<const uint8_t>(reinterpret_cast<const uint8_t*>(input.data()), input.size());
	std::optional<Module> module;
	std::vector<Diagnostic> diagnostics;
	if (input.size() >= 4 && input.compare(0, 4, std::string("NIR\0", 4)) == 0) {
		if (inspect) {
			InspectOptions io;
			io.hexDump = hexDump;
			std::cout << inspectImage(bytes, io);
			return 0;
		}
		LoadOptions lo;
		lo.ignoreProcessToken = true;
		lo.allowPreSSA = true;
		lo.verify = options.verify;
		auto loaded = loadImage(bytes, lo);
		if (loaded.error) {
			std::cerr << path << ": " << loaded.error->str() << "\n";
			return 1;
		}
		module = std::move(loaded.module);
	} else {
		if (inspect) {
			std::cerr << "--inspect needs a binary image\n";
			return 2;
		}
		auto result = parse(input, options);
		if (result.error) {
			std::cerr << path << ": " << result.error->str() << "\n";
			return 1;
		}
		module = std::move(result.module);
		diagnostics = std::move(result.verifierDiagnostics);
	}
	for (auto& d : diagnostics) {
		std::string name = d.function == NONE ? "" : std::string(module->name(module->functions[d.function].nameId));
		std::cerr << path << ": " << d.str(name) << "\n";
	}
	if (!diagnostics.empty()) {
		return 1;
	}
	if (hash || !imagePath.empty()) {
		try {
			if (hash) {
				std::cout << structuralHash(*module, nautilus::effectiveOptions({})).hex() << "\n";
			}
			if (!imagePath.empty()) {
				auto image = writeImage(canonicalize(*module));
				std::ofstream out(imagePath, std::ios::binary);
				out.write(reinterpret_cast<const char*>(image.data()), static_cast<std::streamsize>(image.size()));
				if (!out) {
					std::cerr << "cannot write " << imagePath << "\n";
					return 2;
				}
			}
		} catch (std::exception& e) {
			std::cerr << path << ": " << e.what() << "\n";
			return 1;
		}
		return 0;
	}
	if (!verifyOnly) {
		print(*module, std::cout);
	}
	return 0;
}

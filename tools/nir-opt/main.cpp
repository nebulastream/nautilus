// nautilus-nir-opt: parse a .nir text, verify it, and print it back in canonical form.
//
//   nautilus-nir-opt [--no-verify] [--verify-only] [file | -]
//
// Passes (M3) will be selected with --passes once they exist. Exit status: 0 success, 1 parse or verifier error,
// 2 usage or I/O error.
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
	bool verifyOnly = false;
	std::string path = "-";
	for (int i = 1; i < argc; ++i) {
		std::string arg = argv[i];
		if (arg == "--no-verify") {
			options.verify = false;
		} else if (arg == "--verify-only") {
			verifyOnly = true;
		} else if (arg == "-h" || arg == "--help") {
			std::cout << "usage: nautilus-nir-opt [--no-verify] [--verify-only] [file | -]\n";
			return 0;
		} else if (arg.size() > 1 && arg[0] == '-') {
			std::cerr << "unknown option " << arg << "\n";
			return 2;
		} else {
			path = arg;
		}
	}
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
	auto result = parse(text.str(), options);
	if (result.error) {
		std::cerr << path << ": " << result.error->str() << "\n";
		return 1;
	}
	for (auto& d : result.verifierDiagnostics) {
		std::string name = d.function == NONE ? "" : std::string(result.module->name(result.module->functions[d.function].nameId));
		std::cerr << path << ": " << d.str(name) << "\n";
	}
	if (!result.verifierDiagnostics.empty()) {
		return 1;
	}
	if (!verifyOnly) {
		print(*result.module, std::cout);
	}
	return 0;
}

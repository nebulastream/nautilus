// Plain-main driver of the NIR fuzz target: no libFuzzer runtime, any compiler.
//
//   nautilus-nir-fuzz-replay [--mutate N] [--seed S] <file | directory> ...
//
// Runs every input once. With --mutate N it then runs N mutants of the inputs (byte flips, insertions, deletions,
// splices), a deterministic stand-in for coverage-guided mutation on compilers without libFuzzer. A finding aborts
// (an invariant of NirFuzzTarget.hpp, or a sanitizer report).
#include "NirFuzzTarget.hpp"
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <random>
#include <sstream>
#include <vector>

namespace fs = std::filesystem;

int main(int argc, char** argv) {
	std::vector<std::vector<uint8_t>> inputs;
	uint64_t mutants = 0, seed = 1;
	for (int i = 1; i < argc; ++i) {
		std::string arg = argv[i];
		if (arg == "--mutate" && i + 1 < argc) {
			mutants = std::stoull(argv[++i]);
			continue;
		}
		if (arg == "--seed" && i + 1 < argc) {
			seed = std::stoull(argv[++i]);
			continue;
		}
		std::vector<fs::path> files;
		if (!fs::exists(arg)) {
			std::cerr << "no such file or directory: " << arg << "\n";
			return 2;
		}
		if (fs::is_directory(arg)) {
			for (auto& e : fs::recursive_directory_iterator(arg)) {
				if (e.is_regular_file()) {
					files.push_back(e.path());
				}
			}
		} else {
			files.push_back(arg);
		}
		std::sort(files.begin(), files.end());
		for (auto& f : files) {
			std::ifstream in(f, std::ios::binary);
			std::stringstream ss;
			ss << in.rdbuf();
			auto s = ss.str();
			inputs.emplace_back(s.begin(), s.end());
		}
	}
	if (inputs.empty()) {
		std::cerr << "usage: nautilus-nir-fuzz-replay [--mutate N] [--seed S] <file | directory> ...\n";
		return 2;
	}
	for (auto& in : inputs) {
		nautilus::nir::fuzz::runInput(in.data(), in.size());
	}
	std::cout << "replayed " << inputs.size() << " inputs\n";
	std::mt19937_64 rng(seed);
	for (uint64_t n = 0; n < mutants; ++n) {
		auto input = inputs[rng() % inputs.size()];
		int edits = 1 + rng() % 8;
		for (int e = 0; e < edits && !input.empty(); ++e) {
			size_t at = rng() % input.size();
			switch (rng() % 5) {
			case 0:
				input[at] ^= static_cast<uint8_t>(1u << (rng() % 8));
				break;
			case 1:
				input[at] = static_cast<uint8_t>(rng());
				break;
			case 2:
				input.insert(input.begin() + at, static_cast<uint8_t>(rng()));
				break;
			case 3:
				input.erase(input.begin() + at);
				break;
			default: {
				auto& other = inputs[rng() % inputs.size()];
				size_t from = rng() % other.size();
				size_t len = std::min<size_t>(1 + rng() % 16, other.size() - from);
				input.insert(input.begin() + at, other.begin() + from, other.begin() + from + len);
			}
			}
		}
		nautilus::nir::fuzz::runInput(input.data(), input.size());
	}
	if (mutants != 0) {
		std::cout << "ran " << mutants << " mutants\n";
	}
	return 0;
}

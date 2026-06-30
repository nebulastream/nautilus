#include "Harness.hpp"
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <map>
#include <string>
#include <vector>

// A libFuzzer-free driver for the same differential check. Useful where the
// libFuzzer runtime is unavailable (non-Clang toolchains, CI smoke jobs) and
// for replaying a saved crash input.
//
// Usage:
//   nautilus-fuzz-replay <file> [<file> ...]   replay specific input(s)
//   nautilus-fuzz-replay                       run a built-in deterministic
//                                              corpus of N pseudo-random inputs
//
// Any mismatch aborts with a full report (see Harness.hpp), so this also works
// as a self-checking smoke test: exit 0 means every generated program agreed
// across all backends and the interpreter.

namespace {

// Deterministic xorshift so the built-in corpus is fully reproducible.
uint64_t rngState = 0x9e3779b97f4a7c15ull;
uint64_t nextRand() {
	rngState ^= rngState << 13;
	rngState ^= rngState >> 7;
	rngState ^= rngState << 17;
	return rngState;
}

std::vector<uint8_t> readFile(const char* path) {
	std::ifstream in(path, std::ios::binary);
	return std::vector<uint8_t>((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
}

std::vector<uint8_t> randomBuffer() {
	const size_t len = nextRand() % 96;
	std::vector<uint8_t> buf(len);
	for (size_t i = 0; i < len; ++i) {
		buf[i] = static_cast<uint8_t>(nextRand());
	}
	return buf;
}

int builtinCorpus() {
	constexpr int ITERATIONS = 2000;
	for (int it = 0; it < ITERATIONS; ++it) {
		const std::vector<uint8_t> buf = randomBuffer();
		nautilus::fuzz::runOne(buf.data(), buf.size());
		if ((it + 1) % 200 == 0) {
			std::printf("  ... %d/%d inputs agreed across all backends\n", it + 1, ITERATIONS);
		}
	}
	std::printf("OK: %d generated programs agreed across all backends + interpreter\n", ITERATIONS);
	return 0;
}

// Survey mode: run many inputs WITHOUT aborting, bucketing findings by
// (backend, has-runtime-parameter) and keeping one example per bucket. Used to
// see whether distinct bug classes exist (e.g. a mismatch involving a runtime
// parameter would point to a codegen bug rather than constant folding).
int survey(int iterations) {
	std::map<std::string, nautilus::fuzz::Finding> buckets;
	std::map<std::string, int> counts;
	int totalFindings = 0;

	for (int it = 0; it < iterations; ++it) {
		const std::vector<uint8_t> buf = randomBuffer();
		for (const auto& f : nautilus::fuzz::checkOne(buf.data(), buf.size())) {
			++totalFindings;
			const std::string key =
			    f.backend + (f.exception ? "|exception" : (f.hasParam ? "|has-param" : "|all-const"));
			counts[key]++;
			if (!buckets.count(key)) {
				buckets.emplace(key, f);
			}
		}
		if ((it + 1) % 500 == 0) {
			std::printf("  ... surveyed %d/%d inputs, %d findings so far\n", it + 1, iterations, totalFindings);
		}
	}

	std::printf("\n===== SURVEY SUMMARY (%d inputs, %d total findings) =====\n", iterations, totalFindings);
	std::printf("distinct buckets: %zu\n", buckets.size());
	for (const auto& [key, example] : buckets) {
		std::printf("\n[%s]  x%d\n  example: %s\n", key.c_str(), counts[key], example.program.c_str());
		if (!example.exception) {
			std::printf("  args p0=%llu p1=%llu p2=%llu  expected=%llu got=%llu\n",
			            (unsigned long long) example.args[0], (unsigned long long) example.args[1],
			            (unsigned long long) example.args[2], (unsigned long long) example.expected,
			            (unsigned long long) example.got);
		} else {
			std::printf("  exception: %s\n", example.what.c_str());
		}
	}
	return 0;
}

} // namespace

int main(int argc, char** argv) {
	// Survey mode: nautilus-fuzz-replay --survey [N]
	if (argc > 1 && std::strcmp(argv[1], "--survey") == 0) {
		const int n = (argc > 2) ? std::atoi(argv[2]) : 5000;
		return survey(n);
	}
	if (argc > 1) {
		for (int i = 1; i < argc; ++i) {
			std::printf("replaying %s\n", argv[i]);
			const std::vector<uint8_t> buf = readFile(argv[i]);
			nautilus::fuzz::runOne(buf.data(), buf.size());
			std::printf("  OK: all backends agreed\n");
		}
		return 0;
	}
	return builtinCorpus();
}

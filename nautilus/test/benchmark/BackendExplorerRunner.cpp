// nautilus-backend-explorer: compiles and runs the benchmark kernels (BenchmarkKernels.hpp) under one backend
// configuration and prints one JSON object per kernel to stdout. Driven by tools/backend-explorer/explore.py, which
// sweeps configurations, runs each in its own process (so a crashing configuration cannot take the sweep down) and
// renders the results as an HTML report.
//
//   nautilus-backend-explorer --list
//   nautilus-backend-explorer --print-pipeline [--opt optimizationLevel=2]
//   nautilus-backend-explorer --backend mlir --opt optimizationLevel=2 [--opt-str key=value] [--kernel tpchQ6]...
//                             [--compile-reps 5] [--samples 10] [--sample-ms 5] [--kernel-budget-ms 2000]

#include "BenchmarkKernels.hpp"
#include "nautilus/Engine.hpp"
#include "nautilus/config.hpp"
#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <variant>
#include <vector>
#ifdef ENABLE_TBC_JIT
namespace nautilus::compiler::tbc::jit {
bool jitRuntimeAvailable();
} // namespace nautilus::compiler::tbc::jit
#endif

namespace nautilus::engine::benchmark { namespace {

using Clock = std::chrono::steady_clock;

std::string jsonString(const std::string& text) {
	std::string out = "\"";
	for (const char c : text) {
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
				char buffer[8];
				std::snprintf(buffer, sizeof(buffer), "\\u%04x", c);
				out += buffer;
			} else {
				out += c;
			}
		}
	}
	return out + "\"";
}

std::string jsonNumber(double value) {
	if (!std::isfinite(value)) {
		return "null";
	}
	std::ostringstream stream;
	stream.precision(9);
	stream << value;
	return stream.str();
}

std::string jsonArray(const std::vector<double>& values) {
	std::string out = "[";
	for (size_t i = 0; i < values.size(); ++i) {
		out += (i == 0 ? "" : ",") + jsonNumber(values[i]);
	}
	return out + "]";
}

double median(std::vector<double> values) {
	if (values.empty()) {
		return NAN;
	}
	std::sort(values.begin(), values.end());
	const auto mid = values.size() / 2;
	return values.size() % 2 == 1 ? values[mid] : (values[mid - 1] + values[mid]) / 2.0;
}

std::vector<std::string> availableBackends() {
	std::vector<std::string> backends;
#ifdef ENABLE_MLIR_BACKEND
	backends.emplace_back("mlir");
#endif
#ifdef ENABLE_C_BACKEND
	backends.emplace_back("cpp");
#endif
#ifdef ENABLE_BC_BACKEND
	backends.emplace_back("bc");
#endif
#ifdef ENABLE_TBC_BACKEND
	backends.emplace_back("tbc");
#endif
#ifdef ENABLE_ASMJIT_BACKEND
	backends.emplace_back("asmjit");
#endif
	return backends;
}

bool tbcJitAvailable() {
#ifdef ENABLE_TBC_JIT
	return compiler::tbc::jit::jitRuntimeAvailable();
#else
	return false;
#endif
}

/// `--opt key=value`: true/false become bools, integers ints, other numbers doubles, anything else a string.
/// `--opt-str key=value` always sets a string (for a string option whose value looks like a number).
void setOption(Options& options, const std::string& assignment, bool forceString) {
	const auto eq = assignment.find('=');
	if (eq == std::string::npos) {
		throw std::invalid_argument("expected key=value, got '" + assignment + "'");
	}
	const auto key = assignment.substr(0, eq);
	const auto value = assignment.substr(eq + 1);
	if (forceString) {
		options.setOption(key, value);
		return;
	}
	if (value == "true" || value == "false") {
		options.setOption(key, value == "true");
		return;
	}
	char* end = nullptr;
	const long asInt = std::strtol(value.c_str(), &end, 10);
	if (!value.empty() && *end == '\0') {
		options.setOption(key, static_cast<int>(asInt));
		return;
	}
	const double asDouble = std::strtod(value.c_str(), &end);
	if (!value.empty() && *end == '\0') {
		options.setOption(key, asDouble);
		return;
	}
	options.setOption(key, value);
}

struct RunSettings {
	int compileReps = 5;
	int samples = 10;
	double sampleMs = 5.0;
	double kernelBudgetMs = 2000.0;
};

/// Compiles @p kernel `compileReps` times (each a full trace + IR + backend compile on the same engine), then
/// measures execution: one warm-up run that also yields the checksum, a batch size calibrated so one sample takes
/// about `sampleMs`, and up to `samples` samples within `kernelBudgetMs`.
std::string runKernel(const BenchmarkKernel& kernel, const Options& options, const RunSettings& settings) {
	std::ostringstream out;
	out << "{\"kernel\":" << jsonString(kernel.name);
	try {
		auto engine = NautilusEngine(options);

		std::vector<double> compileWallMs;
		std::map<std::string, std::vector<double>> numericStats;
		std::map<std::string, std::string> textStats;
		CompiledKernel compiled;
		for (int rep = 0; rep < std::max(1, settings.compileReps); ++rep) {
			const auto start = Clock::now();
			compiled = kernel.compile(engine);
			compileWallMs.push_back(std::chrono::duration<double, std::milli>(Clock::now() - start).count());
			if (compiled.statistics != nullptr) {
				for (const auto& [key, value] : *compiled.statistics) {
					if (const auto* asInt = std::get_if<int64_t>(&value)) {
						numericStats[key].push_back(static_cast<double>(*asInt));
					} else if (const auto* asDouble = std::get_if<double>(&value)) {
						numericStats[key].push_back(*asDouble);
					} else if (const auto* asString = std::get_if<std::string>(&value)) {
						textStats[key] = *asString;
					}
				}
			}
		}

		const auto firstStart = Clock::now();
		const int64_t checksum = compiled.run();
		const double firstRunNs = std::chrono::duration<double, std::nano>(Clock::now() - firstStart).count();

		const double sampleNs = settings.sampleMs * 1e6;
		const int64_t batch = std::max<int64_t>(1, static_cast<int64_t>(sampleNs / std::max(firstRunNs, 1.0)));
		std::vector<double> perCallNs;
		const auto measureStart = Clock::now();
		volatile int64_t sink = 0;
		for (int sample = 0; sample < std::max(1, settings.samples); ++sample) {
			const auto start = Clock::now();
			for (int64_t i = 0; i < batch; ++i) {
				sink = sink + compiled.run();
			}
			perCallNs.push_back(std::chrono::duration<double, std::nano>(Clock::now() - start).count() /
			                    static_cast<double>(batch));
			const double spentMs = std::chrono::duration<double, std::milli>(Clock::now() - measureStart).count();
			if (sample >= 2 && spentMs > settings.kernelBudgetMs) {
				break;
			}
		}

		out << ",\"status\":\"ok\",\"checksum\":" << jsonString(std::to_string(checksum));
		out << ",\"compileWallMs\":" << jsonArray(compileWallMs);
		out << ",\"firstRunNs\":" << jsonNumber(firstRunNs);
		out << ",\"batch\":" << batch;
		out << ",\"runNs\":" << jsonArray(perCallNs);
		out << ",\"stats\":{";
		bool first = true;
		for (const auto& [key, values] : numericStats) {
			out << (first ? "" : ",") << jsonString(key) << ":" << jsonNumber(median(values));
			first = false;
		}
		for (const auto& [key, value] : textStats) {
			out << (first ? "" : ",") << jsonString(key) << ":" << jsonString(value);
			first = false;
		}
		out << "}";
	} catch (const std::exception& e) {
		out << ",\"status\":\"error\",\"error\":" << jsonString(e.what());
	}
	out << "}";
	return out.str();
}

int printList(const std::vector<BenchmarkKernel>& kernels) {
	std::cout << "{\"backends\":[";
	const auto backends = availableBackends();
	for (size_t i = 0; i < backends.size(); ++i) {
		std::cout << (i == 0 ? "" : ",") << jsonString(backends[i]);
	}
	std::cout << "],\"tbcJit\":" << (tbcJitAvailable() ? "true" : "false") << ",\"kernels\":[";
	for (size_t i = 0; i < kernels.size(); ++i) {
		std::cout << (i == 0 ? "" : ",") << "{\"name\":" << jsonString(kernels[i].name)
		          << ",\"category\":" << jsonString(kernels[i].category)
		          << ",\"description\":" << jsonString(kernels[i].description) << "}";
	}
	std::cout << "]}" << std::endl;
	return 0;
}

/// Prints the fully expanded textual LLVM pipeline the given options run (see `mlir.recordLLVMPipeline`).
int printPipeline(const std::vector<BenchmarkKernel>& kernels, Options options) {
	options.setOption("engine.backend", std::string("mlir"));
	options.setOption("mlir.recordLLVMPipeline", true);
	auto engine = NautilusEngine(options);
	auto compiled = kernels.front().compile(engine);
	const auto* pipeline = compiled.statistics != nullptr ? compiled.statistics->find("llvm.pipeline") : nullptr;
	if (pipeline == nullptr) {
		std::cerr << "the MLIR backend did not record a pipeline\n";
		return 1;
	}
	std::cout << std::get<std::string>(*pipeline) << std::endl;
	return 0;
}

int runMain(int argc, char** argv) {
	Options options;
	RunSettings settings;
	std::string backend;
	std::vector<std::string> kernelNames;
	bool list = false;
	bool pipeline = false;
	for (int i = 1; i < argc; ++i) {
		const std::string arg = argv[i];
		auto next = [&]() -> std::string {
			if (i + 1 >= argc) {
				throw std::invalid_argument("missing value for " + arg);
			}
			return argv[++i];
		};
		if (arg == "--list") {
			list = true;
		} else if (arg == "--print-pipeline") {
			pipeline = true;
		} else if (arg == "--backend") {
			backend = next();
		} else if (arg == "--opt") {
			setOption(options, next(), false);
		} else if (arg == "--opt-str") {
			setOption(options, next(), true);
		} else if (arg == "--kernel") {
			kernelNames.push_back(next());
		} else if (arg == "--compile-reps") {
			settings.compileReps = std::stoi(next());
		} else if (arg == "--samples") {
			settings.samples = std::stoi(next());
		} else if (arg == "--sample-ms") {
			settings.sampleMs = std::stod(next());
		} else if (arg == "--kernel-budget-ms") {
			settings.kernelBudgetMs = std::stod(next());
		} else {
			throw std::invalid_argument("unknown argument '" + arg + "'");
		}
	}

	const auto kernels = benchmarkKernels();
	if (list) {
		return printList(kernels);
	}
	if (pipeline) {
		return printPipeline(kernels, options);
	}
	if (backend.empty()) {
		throw std::invalid_argument("--backend is required");
	}
	options.setOption("engine.backend", backend);
	if (backend == "mlir") {
		// Generate machine code inside the compile so it is timed (and its size recorded) there.
		options.setOption("mlir.eager_compilation", true);
	}
	for (const auto& kernel : kernels) {
		if (!kernelNames.empty() &&
		    std::find(kernelNames.begin(), kernelNames.end(), kernel.name) == kernelNames.end()) {
			continue;
		}
		std::cout << runKernel(kernel, options, settings) << std::endl;
	}
	return 0;
}

}} // namespace nautilus::engine::benchmark

int main(int argc, char** argv) {
	try {
		return nautilus::engine::benchmark::runMain(argc, argv);
	} catch (const std::exception& e) {
		std::cerr << "nautilus-backend-explorer: " << e.what() << "\n";
		return 2;
	}
}

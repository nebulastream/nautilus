#include "catch2/catch_test_macros.hpp"
#include "nautilus/CompilationStatistics.hpp"
#include "nautilus/Engine.hpp"
#include "nautilus/RuntimeBinding.hpp"
#include "nautilus/config.hpp"
#include "nautilus/val.hpp"
#include <algorithm>
#include <array>
#include <atomic>
#include <charconv>
#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#if defined(ENABLE_TRACING) && defined(ENABLE_MLIR_BACKEND)
#include <llvm/Config/llvm-config.h>
#include <llvm/Support/Process.h>
#include <llvm/TargetParser/Host.h>

namespace nautilus::engine {
namespace {

using BindingBenchmarkClock = std::chrono::steady_clock;
using BindingBenchmarkKernel = uint64_t (*)(uint64_t**, int32_t);
constexpr int32_t BINDING_BENCHMARK_WIDTH = 256;

uint64_t benchmarkEnvironmentCount(const char* name, uint64_t defaultValue) {
	const auto* value = std::getenv(name);
	if (value == nullptr) {
		return defaultValue;
	}
	const std::string_view text(value);
	uint64_t result = 0;
	const auto parsed = std::from_chars(text.data(), text.data() + text.size(), result);
	if (parsed.ec != std::errc {} || parsed.ptr != text.data() + text.size() || result == 0) {
		throw std::invalid_argument(std::string(name) + " must be a positive integer");
	}
	return result;
}

class BindingBenchmarkDirectory {
public:
	BindingBenchmarkDirectory() {
		const auto* output = std::getenv("NAUTILUS_BINDING_BENCHMARK_DIR");
		preserve_ = output != nullptr && *output != '\0';
		const auto root = preserve_ ? std::filesystem::path(output) : std::filesystem::temp_directory_path();
		path_ = root / ("runtime-bindings-" + std::to_string(BindingBenchmarkClock::now().time_since_epoch().count()));
		std::filesystem::create_directories(path_);
		if (preserve_) {
			std::cout << "RuntimeBindings benchmark evidence: " << path_ << '\n';
		}
	}

	~BindingBenchmarkDirectory() {
		if (!preserve_) {
			std::error_code error;
			std::filesystem::remove_all(path_, error);
		}
	}

	const std::filesystem::path& path() const {
		return path_;
	}

private:
	std::filesystem::path path_;
	bool preserve_ = false;
};

struct BindingBenchmarkMemory {
	std::array<uint64_t, BINDING_BENCHMARK_WIDTH> input;
	std::array<uint64_t, BINDING_BENCHMARK_WIDTH> state;

	void reset(uint64_t seed) {
		for (std::size_t index = 0; index < input.size(); ++index) {
			input[index] = (index + 1) * 17 + seed;
			state[index] = (index + 3) * 31 + seed;
		}
	}
};

val<uint64_t> bindingBenchmarkBody(val<uint64_t*> input, val<uint64_t*> state, val<int32_t> count) {
	val<uint64_t> checksum = 0;
	for (val<int32_t> index = 0; index < count; index = index + 1) {
		auto slot = index & (BINDING_BENCHMARK_WIDTH - 1);
		val<uint64_t> value = input[slot];
		val<uint64_t> previous = state[slot];
		val<uint64_t> next = (previous ^ value) * uint64_t {33} + static_cast<val<uint64_t>>(index);
		state[slot] = next;
		checksum = checksum + next;
	}
	return checksum;
}

uint64_t bindingBenchmarkReference(BindingBenchmarkMemory& memory, int32_t count) {
	uint64_t checksum = 0;
	for (int32_t index = 0; index < count; ++index) {
		const auto slot = index & (BINDING_BENCHMARK_WIDTH - 1);
		const auto next = (memory.state[slot] ^ memory.input[slot]) * uint64_t {33} + static_cast<uint64_t>(index);
		memory.state[slot] = next;
		checksum += next;
	}
	return checksum;
}

struct PreparedBindingBenchmark {
	std::shared_ptr<compiler::Executable> executable;
	BindingBenchmarkKernel kernel;
	double compileMs;
	double resolveMs;
	int64_t traces;
};

PreparedBindingBenchmark prepareBindingBenchmark(const std::string& variant, BindingBenchmarkMemory& memory,
                                                 const std::filesystem::path& directory, bool dump) {
	Options options;
	options.setOption("engine.backend", std::string("mlir"));
	options.setOption("mlir.enableMultithreading", false);
	options.setOption("mlir.eager_compilation", true);
	if (variant != "raw_capture") {
		options.setOption("engine.Blob.CacheDir", (directory / "cache" / variant).string());
		options.setOption("engine.Blob.CacheKey", "runtime-binding-benchmark-" + variant + "-v1");
	}
	if (dump) {
		options.setOption("dump.after_mlir_generation", true);
		options.setOption("dump.before_llvm_optimization", true);
		options.setOption("dump.after_llvm_generation", true);
		options.setOption("dump.file", true);
	}
	NautilusEngine engine(options);
	auto module = engine.createModule();
	int64_t traces = 0;
	using Signature = val<uint64_t>(val<uint64_t**>, val<int32_t>);
	if (variant == "pointer_table") {
		module.registerFunction<Signature>("execute", [&traces](val<uint64_t**> table, val<int32_t> count) {
			++traces;
			val<uint64_t*> input = table[0];
			val<uint64_t*> state = table[1];
			return bindingBenchmarkBody(input, state, count);
		});
	} else if (variant == "binding_get") {
		RuntimeBindings bindings;
		auto input = bindings.bind<uint64_t>("operator/17/input", memory.input.data());
		auto state = bindings.bind<uint64_t>("operator/17/state", memory.state.data());
		module.setRuntimeBindings(bindings);
		module.registerFunction<Signature>("execute", [input, state, &traces](val<uint64_t**>, val<int32_t> count) {
			++traces;
			return bindingBenchmarkBody(input.get(), state.get(), count);
		});
	} else {
		REQUIRE(variant == "raw_capture");
		module.registerFunction<Signature>("execute", [input = memory.input.data(), state = memory.state.data(),
		                                               &traces](val<uint64_t**>, val<int32_t> count) {
			++traces;
			return bindingBenchmarkBody(val<uint64_t*>(input), val<uint64_t*>(state), count);
		});
	}
	const auto compileStart = BindingBenchmarkClock::now();
	auto compiled = module.compile();
	const auto compileEnd = BindingBenchmarkClock::now();
	auto executable = compiled.releaseExecutable();
	REQUIRE(executable != nullptr);
	REQUIRE(executable->hasInvocableFunctionPtr());
	const auto resolveStart = BindingBenchmarkClock::now();
	auto kernel = reinterpret_cast<BindingBenchmarkKernel>(executable->getInvocableFunctionPtr("execute"));
	const auto resolveEnd = BindingBenchmarkClock::now();
	REQUIRE(kernel != nullptr);
	return {std::move(executable), kernel, std::chrono::duration<double, std::milli>(compileEnd - compileStart).count(),
	        std::chrono::duration<double, std::milli>(resolveEnd - resolveStart).count(), traces};
}

void recordBindingCompilation(const PreparedBindingBenchmark& compiled, const std::string& variant,
                              const std::string& phase, const std::filesystem::path& directory, std::ostream& csv) {
	const auto stats = compiled.executable->getCompilationStatistics();
	REQUIRE(stats != nullptr);
	const auto artifactDirectory = directory / (variant + "-" + phase);
	std::filesystem::create_directories(artifactDirectory);
	std::ofstream report(artifactDirectory / "compilation.txt");
	report << std::setprecision(17) << "compile_ms=" << compiled.compileMs << '\n';
	report << "resolve_ms=" << compiled.resolveMs << '\n';
	report << "compile_link_ms=" << compiled.compileMs + compiled.resolveMs << '\n';
	report << "kernel_address=" << reinterpret_cast<uintptr_t>(compiled.kernel) << '\n';
	report << "trace_count=" << compiled.traces << '\n';
	report << stats->toString();
	report.close();
	REQUIRE(report.good());
	const auto cacheValue = [&](const char* key) {
		const auto* value = stats->find(key);
		return value == nullptr ? std::string("not_requested") : std::get<std::string>(*value);
	};
	csv << variant << ',' << phase << ',' << compiled.compileMs << ',' << compiled.resolveMs << ','
	    << compiled.compileMs + compiled.resolveMs << ',' << cacheValue("cache.object") << ','
	    << cacheValue("cache.mlir") << ',' << cacheValue("cache.fallback") << ',' << compiled.traces << '\n';
	std::cout << variant << ' ' << phase << " compile_ms=" << compiled.compileMs << " resolve_ms=" << compiled.resolveMs
	          << " cache.object=" << cacheValue("cache.object") << " cache.fallback=" << cacheValue("cache.fallback")
	          << " traces=" << compiled.traces << '\n';
	for (const auto* key : {"after_mlir_generation", "before_llvm_optimization", "after_llvm_generation"}) {
		const auto generated = compiled.executable->getGeneratedFile(key);
		if (!generated.empty()) {
			const auto source = std::filesystem::path(generated);
			std::filesystem::copy_file(source, artifactDirectory / source.filename(),
			                           std::filesystem::copy_options::overwrite_existing);
		}
	}
}

struct BindingSteadySample {
	std::size_t variant;
	int32_t batch;
	double nsPerCall;
};

} // namespace

TEST_CASE("RuntimeBindings pointer access benchmark", "[.runtime-bindings-benchmark]") {
#ifndef __linux__
	SKIP("Persistent module caching requires Linux ELF build IDs");
#endif
	const auto calls = benchmarkEnvironmentCount("NAUTILUS_BINDING_BENCHMARK_CALLS", 250000);
	const auto samples = benchmarkEnvironmentCount("NAUTILUS_BINDING_BENCHMARK_SAMPLES", 5);
	const auto* dumpValue = std::getenv("NAUTILUS_BINDING_BENCHMARK_DUMP");
	const bool dump = dumpValue != nullptr && std::string_view(dumpValue) == "1";
	BindingBenchmarkDirectory directory;
	std::ofstream metadata(directory.path() / "environment.txt");
	metadata << "compiler=" << __VERSION__ << "\nllvm=" << LLVM_VERSION_STRING
	         << "\ntarget=" << llvm::sys::getProcessTriple()
	         << "\npage_bytes=" << llvm::sys::Process::getPageSizeEstimate() << '\n';
	metadata << "live_code_sharing=none; each live executable owns an LLJIT and separately relocated code pages\n";
#ifdef NDEBUG
	metadata << "build=release\n";
#else
	metadata << "build=debug\n";
#endif
	metadata << "calls_per_sample=" << calls << "\nsamples=" << samples << "\narray_width=" << BINDING_BENCHMARK_WIDTH
	         << "\ndump=" << dump << '\n';
	metadata.close();
	REQUIRE(metadata.good());
	std::ofstream compileCsv(directory.path() / "compile.csv");
	compileCsv
	    << std::setprecision(17)
	    << "variant,phase,compile_ms,resolve_ms,compile_link_ms,cache_object,cache_mlir,cache_fallback,trace_count\n";
	std::ofstream steadyCsv(directory.path() / "steady.csv");
	steadyCsv << std::setprecision(17) << "variant,batch,calls,sample,elapsed_ns,ns_per_call,ns_per_element,checksum\n";

	const std::array<std::string, 3> variants {"pointer_table", "binding_get", "raw_capture"};
	std::array<BindingBenchmarkMemory, 3> memories;
	std::vector<PreparedBindingBenchmark> kernels;
	kernels.reserve(variants.size());
	for (std::size_t index = 0; index < variants.size(); ++index) {
		memories[index].reset(0);
		auto cold = prepareBindingBenchmark(variants[index], memories[index], directory.path(), dump);
		recordBindingCompilation(cold, variants[index], "cold", directory.path(), compileCsv);
		CAPTURE(variants[index]);
		REQUIRE(cold.traces > 0);
		if (variants[index] == "raw_capture") {
			const auto stats = cold.executable->getCompilationStatistics();
			REQUIRE_FALSE(stats->contains("cache.eligible"));
			REQUIRE_FALSE(stats->contains("cache.object"));
			REQUIRE_FALSE(stats->contains("cache.mlir"));
			REQUIRE_FALSE(stats->contains("cache.fallback"));
			REQUIRE_FALSE(std::filesystem::exists(directory.path() / "cache" / variants[index]));
			kernels.push_back(std::move(cold));
		} else {
			const bool cacheable = variants[index] == "binding_get";
			const auto checkCache = [&](const PreparedBindingBenchmark& compiled, bool repeat) {
				const bool hit = cacheable && repeat;
				const auto stats = compiled.executable->getCompilationStatistics();
				REQUIRE(stats != nullptr);
				INFO(stats->toString());
				for (const auto* key : {"cache.object", "cache.mlir", "cache.fallback", "cache.tracingRan"}) {
					REQUIRE(stats->contains(key));
				}
				REQUIRE(std::get<std::string>(*stats->find("cache.object")) ==
				        (cacheable ? (hit ? "hit" : "written") : "miss"));
				REQUIRE(std::get<std::string>(*stats->find("cache.mlir")) ==
				        (cacheable ? (hit ? "not_checked" : "written") : "miss"));
				REQUIRE(std::get<std::string>(*stats->find("cache.fallback")) ==
				        (cacheable ? "none" : "non_relocatable_pointer"));
				REQUIRE(std::get<int64_t>(*stats->find("cache.tracingRan")) == (hit ? 0 : 1));
				REQUIRE((hit ? compiled.traces == 0 : compiled.traces > 0));
				if (!cacheable) {
					REQUIRE(std::filesystem::is_empty(directory.path() / "cache" / variants[index]));
				}
			};
			checkCache(cold, false);
			auto repeated = prepareBindingBenchmark(variants[index], memories[index], directory.path(), dump);
			recordBindingCompilation(repeated, variants[index], cacheable ? "warm" : "repeat_uncached",
			                         directory.path(), compileCsv);
			checkCache(repeated, true);
			kernels.push_back(std::move(repeated));
		}
	}
	compileCsv.close();
	REQUIRE(compileCsv.good());

	const std::array<int32_t, 3> batches {1, 64, 1024};
	for (std::size_t index = 0; index < variants.size(); ++index) {
		CAPTURE(variants[index]);
		memories[index].reset(17);
		auto expected = memories[index];
		uint64_t* table[] = {memories[index].input.data(), memories[index].state.data()};
		for (const auto batch : batches) {
			for (int iteration = 0; iteration < 4; ++iteration) {
				const auto reference = bindingBenchmarkReference(expected, batch);
				REQUIRE(kernels[index].kernel(table, batch) == reference);
				REQUIRE(memories[index].state == expected.state);
				REQUIRE(memories[index].input == expected.input);
			}
		}
	}

	std::vector<BindingSteadySample> timings;
	for (const auto batch : batches) {
		for (uint64_t sample = 0; sample < samples; ++sample) {
			std::optional<uint64_t> expectedChecksum;
			std::array<uint64_t, BINDING_BENCHMARK_WIDTH> expectedState {};
			for (std::size_t offset = 0; offset < variants.size(); ++offset) {
				const auto index = (sample + offset) % variants.size();
				CAPTURE(variants[index], batch, sample);
				auto& memory = memories[index];
				auto kernel = kernels[index].kernel;
				uint64_t* table[] = {memory.input.data(), memory.state.data()};
				memory.reset(sample + 1);
				for (int warmup = 0; warmup < 256; ++warmup) {
					(void) kernel(table, batch);
				}
				memory.reset(sample + 1);
				uint64_t checksum = 0;
				std::atomic_signal_fence(std::memory_order_seq_cst);
				const auto start = BindingBenchmarkClock::now();
				for (uint64_t call = 0; call < calls; ++call) {
					checksum += kernel(table, batch);
				}
				const auto end = BindingBenchmarkClock::now();
				std::atomic_signal_fence(std::memory_order_seq_cst);
				const auto elapsed = std::chrono::duration<double, std::nano>(end - start).count();
				const auto nsPerCall = elapsed / static_cast<double>(calls);
				steadyCsv << variants[index] << ',' << batch << ',' << calls << ',' << sample << ',' << elapsed << ','
				          << nsPerCall << ',' << nsPerCall / batch << ',' << checksum << '\n';
				timings.push_back({index, batch, nsPerCall});
				if (expectedChecksum) {
					REQUIRE(checksum == *expectedChecksum);
					REQUIRE(memory.state == expectedState);
				} else {
					expectedChecksum = checksum;
					expectedState = memory.state;
				}
			}
		}
	}
	steadyCsv.close();
	REQUIRE(steadyCsv.good());
	for (const auto batch : batches) {
		for (std::size_t index = 0; index < variants.size(); ++index) {
			std::vector<double> values;
			for (const auto& timing : timings) {
				if (timing.variant == index && timing.batch == batch) {
					values.push_back(timing.nsPerCall);
				}
			}
			std::ranges::sort(values);
			std::cout << variants[index] << " batch=" << batch << " median_ns_per_call=" << values[values.size() / 2]
			          << " min_ns_per_call=" << values.front() << '\n';
		}
	}
}

} // namespace nautilus::engine
#endif

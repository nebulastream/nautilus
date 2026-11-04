#include "nautilus/CompilationStatistics.hpp"
#include "nautilus/compiler/DumpHandler.hpp"
#include "nautilus/compiler/backends/nir/NirBackend.hpp"
#include "nautilus/config.hpp"
#include "nautilus/nir/Parser.hpp"
#include "nautilus/options.hpp"
#include <catch2/catch_all.hpp>
#include <filesystem>
#include <fstream>
#include <sstream>

// Guards the quality of the bytecode that `tbc` on NIR emits for the two call kernels of ExecutionBenchmark.cpp
// (internalCall, externalCall). The 0.3 lowering produced, per iteration of the loop, one fused compare-and-branch, an
// immediate add for the counter and a callee with a 3-register frame; a lowering without these optimizations ran the
// kernels 1.13x (internal) and 1.25x (external) slower in the interpreter. The numbers below are the 0.3 lowering's.
#if defined(ENABLE_TBC_BACKEND)

namespace nautilus::compiler {

namespace {

constexpr const char* internalCallLoop = R"(nir 1.0
callee #c0 = internal @callee : (i64, i64) -> i64

func @callee : (i64, i64) -> i64 {
^b0(%0: i64, %1: i64):
  %2 = mul %0, 2:i64
  %3 = add %2, %1
  ret %3
}

func export @loop : (i32) -> i64 {
^b0(%0: i32):
  br ^b1(0:i32, 0:i64)
^b1(%2: i32, %3: i64):
  %4 = cmp.lt.s %2, %0
  condbr %4, ^b2, ^b3
^b2:
  %6 = call #c0(%3, 1:i64)
  %7 = add %2, 1:i32
  br ^b1(%7, %6)
^b3:
  ret %3
}
)";

constexpr const char* externalCallLoop = R"(nir 1.0
pointer #p0 = callee symbol "rt_add"
callee #c0 = external #p0 : (i64, i64) -> i64 attrs(noUnwind)

func export @loop : (i32) -> i64 {
^b0(%0: i32):
  br ^b1(0:i32, 0:i64)
^b1(%2: i32, %3: i64):
  %4 = cmp.lt.s %2, %0
  condbr %4, ^b2, ^b3
^b2:
  %6 = call #c0(%3, 1:i64)
  %7 = add %2, 1:i32
  br ^b1(%7, %6)
^b3:
  ret %3
}
)";

int64_t dummyCallee(int64_t a, int64_t b) {
	return a + b;
}

struct Lowered {
	int64_t instructions;
	int64_t registers;
	std::string listing;
};

Lowered lowerToTbc(const char* text) {
	auto parsed = nir::parse(text);
	REQUIRE(parsed.ok());
	auto module = std::make_shared<nir::Module>(std::move(*parsed.module));

	engine::Options options;
	options.setOption("dump.after_tbc_generation", true);
	options.setOption("dump.console", false);
	const CompilationUnitID unitId = "tbc-nir-lowering-shape-test";
	DumpHandler dumpHandler(options, unitId);
	CompilationStatistics statistics;
	nirbackend::HostEnvironment host;
	host.resolveSymbol = [](std::string_view) -> void* {
		return reinterpret_cast<void*>(&dummyCallee);
	};
	nirbackend::CompileContext context {dumpHandler, options, &statistics, nullptr, host};
	const auto* backend = nirbackend::NirBackendRegistry::instance().find("tbc");
	REQUIRE(backend != nullptr);
	auto executable = backend->compile(module, context);
	REQUIRE(executable != nullptr);

	Lowered result;
	result.instructions = std::get<int64_t>(*statistics.find("tbc.instructions"));
	result.registers = std::get<int64_t>(*statistics.find("tbc.registers.max"));
	const auto& files = dumpHandler.getGeneratedFiles();
	REQUIRE(files.contains("after_tbc_generation"));
	const std::filesystem::path path = files.at("after_tbc_generation");
	std::ifstream in(path);
	std::stringstream buffer;
	buffer << in.rdbuf();
	result.listing = buffer.str();
	std::error_code ignored;
	std::filesystem::remove_all(path.parent_path(), ignored);
	return result;
}

bool contains(const std::string& text, const char* needle) {
	return text.find(needle) != std::string::npos;
}

} // namespace

TEST_CASE("tbc on NIR: the internal call loop keeps the 0.3 bytecode quality") {
	const auto lowered = lowerToTbc(internalCallLoop);
	INFO(lowered.listing);
	// the comparison and the branch are one instruction, the counter and the callee use immediates
	REQUIRE(contains(lowered.listing, "CJMP_LT_i32"));
	REQUIRE_FALSE(contains(lowered.listing, "\tLT_i32 r"));
	REQUIRE(contains(lowered.listing, "ADD_imm_i32"));
	REQUIRE(contains(lowered.listing, "MUL_imm_i64"));
	// 0.3: 15 words (loop 12, callee 3); the largest frame is 5 registers, and the callee's has 3
	REQUIRE(lowered.instructions <= 15);
	REQUIRE(lowered.registers <= 7);
	REQUIRE(contains(lowered.listing, "function callee (regs=3,"));
}

TEST_CASE("tbc on NIR: the external call loop keeps the 0.3 bytecode quality") {
	const auto lowered = lowerToTbc(externalCallLoop);
	INFO(lowered.listing);
	REQUIRE(contains(lowered.listing, "CJMP_LT_i32"));
	REQUIRE_FALSE(contains(lowered.listing, "\tLT_i32 r"));
	REQUIRE(contains(lowered.listing, "ADD_imm_i32"));
	// 0.3: 12 words
	REQUIRE(lowered.instructions <= 12);
	REQUIRE(lowered.registers <= 7);
}

} // namespace nautilus::compiler

#endif

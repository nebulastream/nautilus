#include "common/GPUFunctions.hpp"
#include "nautilus/CompilableFunction.hpp"
#include "nautilus/Engine.hpp"
#include "nautilus/config.hpp"
#include "nautilus/gpu/backends.hpp"
#include "nautilus/gpu/config.hpp"
#include "nautilus/tracing/ExceptionBasedTraceContext.hpp"
#include "nautilus/tracing/phases/SSACreationPhase.hpp"
#include "nautilus/tracing/phases/TraceToIRConversionPhase.hpp"
#include <catch2/catch_all.hpp>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <unistd.h>

namespace nautilus::log::options {
bool getLogAddresses();
void setLogAddresses(bool);
} // namespace nautilus::log::options

namespace nautilus::engine {

static bool checkCodegenFile(const std::string& actual, const std::string& category, const std::string& backend,
                             const std::string& name, const std::string& ext = ".txt") {
	auto dir = std::string(GPU_TEST_DATA_FOLDER) + category + "/" + backend + "/";
	if (!std::filesystem::exists(dir)) {
		std::filesystem::create_directories(dir);
	}
	std::string filePath = dir + name + ext;
	if (!std::filesystem::exists(filePath)) {
		std::cerr << "Reference file does not exist: " << filePath << " Initializing. Please rerun.\n";
		std::ofstream file {filePath};
		file << actual;
		return false;
	}
	std::ifstream file(filePath);
	std::stringstream expected;
	expected << file.rdbuf();
	if (expected.str() == actual) {
		return true;
	}
	std::cerr << "Codegen mismatch for " << filePath << "\n=== Expected ===\n"
	          << expected.str() << "\n=== Actual ===\n"
	          << actual << "\n=== End ===\n";
	return false;
}

static std::shared_ptr<compiler::ir::IRGraph> traceToIR(std::function<void()> wrapper) {
	auto rootFunction = compiler::CompilableFunction("execute", std::move(wrapper));
	std::list<compiler::CompilableFunction> functionsToTrace;
	functionsToTrace.push_back(rootFunction);
	auto traceModule = tracing::ExceptionBasedTraceContext::Trace(functionsToTrace, engine::Options());
	auto ssaPhase = tracing::SSACreationPhase();
	auto afterSSA = ssaPhase.apply(std::shared_ptr<tracing::TraceModule>(std::move(traceModule)));
	auto irPhase = tracing::TraceToIRConversionPhase();
	return irPhase.apply(std::move(afterSSA));
}

#ifdef ENABLE_CUDA_BACKEND
TEST_CASE("GPU CUDA Codegen Test") {
	nautilus::log::options::setLogAddresses(false);
	engine::Options options;
	options.setOption("gpu.blockDimX", 256);
	options.setOption("gpu.gridDimX", 1);

	SECTION("gpuThreadIdxX") {
		auto ir = traceToIR(details::createFunctionWrapper(gpuThreadIdxX));
		auto code = gpu::lowerToCUDA(ir, options);
		REQUIRE(checkCodegenFile(code, "gpu-tests", "cuda", "gpuThreadIdxX", ".cu"));
	}
	SECTION("gpuBlockIdxX") {
		auto ir = traceToIR(details::createFunctionWrapper(gpuBlockIdxX));
		auto code = gpu::lowerToCUDA(ir, options);
		REQUIRE(checkCodegenFile(code, "gpu-tests", "cuda", "gpuBlockIdxX", ".cu"));
	}
	SECTION("gpuBlockDimX") {
		auto ir = traceToIR(details::createFunctionWrapper(gpuBlockDimX));
		auto code = gpu::lowerToCUDA(ir, options);
		REQUIRE(checkCodegenFile(code, "gpu-tests", "cuda", "gpuBlockDimX", ".cu"));
	}
	SECTION("gpuGridDimX") {
		auto ir = traceToIR(details::createFunctionWrapper(gpuGridDimX));
		auto code = gpu::lowerToCUDA(ir, options);
		REQUIRE(checkCodegenFile(code, "gpu-tests", "cuda", "gpuGridDimX", ".cu"));
	}
	SECTION("gpuSyncThreadsOnly") {
		auto ir = traceToIR(details::createFunctionWrapper(gpuSyncThreadsOnly));
		auto code = gpu::lowerToCUDA(ir, options);
		REQUIRE(checkCodegenFile(code, "gpu-tests", "cuda", "gpuSyncThreadsOnly", ".cu"));
	}
	SECTION("gpuGlobalThreadIdx") {
		auto ir = traceToIR(details::createFunctionWrapper(gpuGlobalThreadIdx));
		auto code = gpu::lowerToCUDA(ir, options);
		REQUIRE(checkCodegenFile(code, "gpu-tests", "cuda", "gpuGlobalThreadIdx", ".cu"));
	}
	SECTION("gpuThreadIdx3D") {
		auto ir = traceToIR(details::createFunctionWrapper(gpuThreadIdx3D));
		auto code = gpu::lowerToCUDA(ir, options);
		REQUIRE(checkCodegenFile(code, "gpu-tests", "cuda", "gpuThreadIdx3D", ".cu"));
	}
	SECTION("gpuBoundedIndex") {
		auto ir = traceToIR(details::createFunctionWrapper(gpuBoundedIndex));
		auto code = gpu::lowerToCUDA(ir, options);
		REQUIRE(checkCodegenFile(code, "gpu-tests", "cuda", "gpuBoundedIndex", ".cu"));
	}
	SECTION("gpuVectorAdd") {
		auto ir = traceToIR(details::createFunctionWrapper(gpuVectorAdd));
		auto code = gpu::lowerToCUDA(ir, options);
		REQUIRE(checkCodegenFile(code, "gpu-tests", "cuda", "gpuVectorAdd", ".cu"));
	}
	SECTION("gpuLaunchVecAdd") {
		auto ir = traceToIR(details::createFunctionWrapper(gpuLaunchVecAdd));
		auto code = gpu::lowerToCUDA(ir, options);
		REQUIRE(checkCodegenFile(code, "gpu-tests", "cuda", "gpuLaunchVecAdd", ".cu"));
	}
	SECTION("gpuLaunchVecAddDynamic") {
		auto ir = traceToIR(details::createFunctionWrapper(gpuLaunchVecAddDynamic));
		auto code = gpu::lowerToCUDA(ir, options);
		REQUIRE(checkCodegenFile(code, "gpu-tests", "cuda", "gpuLaunchVecAddDynamic", ".cu"));
	}
	SECTION("gpuLaunchVecAddBounded") {
		auto ir = traceToIR(details::createFunctionWrapper(gpuLaunchVecAddBounded));
		auto code = gpu::lowerToCUDA(ir, options);
		REQUIRE(checkCodegenFile(code, "gpu-tests", "cuda", "gpuLaunchVecAddBounded", ".cu"));
	}
	SECTION("gpuLaunchClassify") {
		auto ir = traceToIR(details::createFunctionWrapper(gpuLaunchClassify));
		auto code = gpu::lowerToCUDA(ir, options);
		REQUIRE(checkCodegenFile(code, "gpu-tests", "cuda", "gpuLaunchClassify", ".cu"));
	}
	SECTION("gpuLaunchSaxpy") {
		auto ir = traceToIR(details::createFunctionWrapper(gpuLaunchSaxpy));
		auto code = gpu::lowerToCUDA(ir, options);
		REQUIRE(checkCodegenFile(code, "gpu-tests", "cuda", "gpuLaunchSaxpy", ".cu"));
	}
	SECTION("gpuLaunchVecScale") {
		auto ir = traceToIR(details::createFunctionWrapper(gpuLaunchVecScale));
		auto code = gpu::lowerToCUDA(ir, options);
		REQUIRE(checkCodegenFile(code, "gpu-tests", "cuda", "gpuLaunchVecScale", ".cu"));
	}
	SECTION("gpuLaunchPrefixSum") {
		auto ir = traceToIR(details::createFunctionWrapper(gpuLaunchPrefixSum));
		auto code = gpu::lowerToCUDA(ir, options);
		REQUIRE(checkCodegenFile(code, "gpu-tests", "cuda", "gpuLaunchPrefixSum", ".cu"));
	}
}
#endif

#ifdef ENABLE_METAL_BACKEND
TEST_CASE("GPU Metal Codegen Test") {
	nautilus::log::options::setLogAddresses(false);
	engine::Options options;

	SECTION("gpuThreadIdxX") {
		auto ir = traceToIR(details::createFunctionWrapper(gpuThreadIdxX));
		auto out = gpu::lowerToMetal(ir, options);
		REQUIRE(checkCodegenFile(out.deviceCode, "gpu-tests", "metal", "gpuThreadIdxX", ".metal"));
	}
	SECTION("gpuBlockIdxX") {
		auto ir = traceToIR(details::createFunctionWrapper(gpuBlockIdxX));
		auto out = gpu::lowerToMetal(ir, options);
		REQUIRE(checkCodegenFile(out.deviceCode, "gpu-tests", "metal", "gpuBlockIdxX", ".metal"));
	}
	SECTION("gpuBlockDimX") {
		auto ir = traceToIR(details::createFunctionWrapper(gpuBlockDimX));
		auto out = gpu::lowerToMetal(ir, options);
		REQUIRE(checkCodegenFile(out.deviceCode, "gpu-tests", "metal", "gpuBlockDimX", ".metal"));
	}
	SECTION("gpuGridDimX") {
		auto ir = traceToIR(details::createFunctionWrapper(gpuGridDimX));
		auto out = gpu::lowerToMetal(ir, options);
		REQUIRE(checkCodegenFile(out.deviceCode, "gpu-tests", "metal", "gpuGridDimX", ".metal"));
	}
	SECTION("gpuSyncThreadsOnly") {
		auto ir = traceToIR(details::createFunctionWrapper(gpuSyncThreadsOnly));
		auto out = gpu::lowerToMetal(ir, options);
		REQUIRE(checkCodegenFile(out.deviceCode, "gpu-tests", "metal", "gpuSyncThreadsOnly", ".metal"));
	}
	SECTION("gpuGlobalThreadIdx") {
		auto ir = traceToIR(details::createFunctionWrapper(gpuGlobalThreadIdx));
		auto out = gpu::lowerToMetal(ir, options);
		REQUIRE(checkCodegenFile(out.deviceCode, "gpu-tests", "metal", "gpuGlobalThreadIdx", ".metal"));
	}
	SECTION("gpuThreadIdx3D") {
		auto ir = traceToIR(details::createFunctionWrapper(gpuThreadIdx3D));
		auto out = gpu::lowerToMetal(ir, options);
		REQUIRE(checkCodegenFile(out.deviceCode, "gpu-tests", "metal", "gpuThreadIdx3D", ".metal"));
	}
	SECTION("gpuBoundedIndex") {
		auto ir = traceToIR(details::createFunctionWrapper(gpuBoundedIndex));
		auto out = gpu::lowerToMetal(ir, options);
		REQUIRE(checkCodegenFile(out.deviceCode, "gpu-tests", "metal", "gpuBoundedIndex", ".metal"));
	}
	SECTION("gpuVectorAdd") {
		auto ir = traceToIR(details::createFunctionWrapper(gpuVectorAdd));
		auto out = gpu::lowerToMetal(ir, options);
		REQUIRE(checkCodegenFile(out.deviceCode, "gpu-tests", "metal", "gpuVectorAdd", ".metal"));
	}
	SECTION("gpuLaunchVecAdd - device") {
		auto ir = traceToIR(details::createFunctionWrapper(gpuLaunchVecAdd));
		auto out = gpu::lowerToMetal(ir, options);
		REQUIRE(checkCodegenFile(out.deviceCode, "gpu-tests", "metal", "gpuLaunchVecAdd.device", ".metal"));
	}
	SECTION("gpuLaunchVecAdd - host") {
		auto ir = traceToIR(details::createFunctionWrapper(gpuLaunchVecAdd));
		auto out = gpu::lowerToMetal(ir, options);
		REQUIRE(checkCodegenFile(out.hostCode, "gpu-tests", "metal", "gpuLaunchVecAdd.host", ".cpp"));
	}
	SECTION("gpuLaunchVecAddDynamic - device") {
		auto ir = traceToIR(details::createFunctionWrapper(gpuLaunchVecAddDynamic));
		auto out = gpu::lowerToMetal(ir, options);
		REQUIRE(checkCodegenFile(out.deviceCode, "gpu-tests", "metal", "gpuLaunchVecAddDynamic.device", ".metal"));
	}
	SECTION("gpuLaunchVecAddDynamic - host") {
		auto ir = traceToIR(details::createFunctionWrapper(gpuLaunchVecAddDynamic));
		auto out = gpu::lowerToMetal(ir, options);
		REQUIRE(checkCodegenFile(out.hostCode, "gpu-tests", "metal", "gpuLaunchVecAddDynamic.host", ".cpp"));
	}
	SECTION("gpuLaunchClassify - device") {
		auto ir = traceToIR(details::createFunctionWrapper(gpuLaunchClassify));
		auto out = gpu::lowerToMetal(ir, options);
		REQUIRE(checkCodegenFile(out.deviceCode, "gpu-tests", "metal", "gpuLaunchClassify.device", ".metal"));
	}
	SECTION("gpuLaunchClassify - host") {
		auto ir = traceToIR(details::createFunctionWrapper(gpuLaunchClassify));
		auto out = gpu::lowerToMetal(ir, options);
		REQUIRE(checkCodegenFile(out.hostCode, "gpu-tests", "metal", "gpuLaunchClassify.host", ".cpp"));
	}
	SECTION("gpuLaunchPrefixSum - device") {
		auto ir = traceToIR(details::createFunctionWrapper(gpuLaunchPrefixSum));
		auto out = gpu::lowerToMetal(ir, options);
		REQUIRE(checkCodegenFile(out.deviceCode, "gpu-tests", "metal", "gpuLaunchPrefixSum.device", ".metal"));
	}
	SECTION("gpuLaunchPrefixSum - host") {
		auto ir = traceToIR(details::createFunctionWrapper(gpuLaunchPrefixSum));
		auto out = gpu::lowerToMetal(ir, options);
		REQUIRE(checkCodegenFile(out.hostCode, "gpu-tests", "metal", "gpuLaunchPrefixSum.host", ".cpp"));
	}
	SECTION("gpuLaunchVecAddBounded - device") {
		auto ir = traceToIR(details::createFunctionWrapper(gpuLaunchVecAddBounded));
		auto out = gpu::lowerToMetal(ir, options);
		REQUIRE(checkCodegenFile(out.deviceCode, "gpu-tests", "metal", "gpuLaunchVecAddBounded.device", ".metal"));
	}
	SECTION("gpuLaunchVecAddBounded - host") {
		auto ir = traceToIR(details::createFunctionWrapper(gpuLaunchVecAddBounded));
		auto out = gpu::lowerToMetal(ir, options);
		REQUIRE(checkCodegenFile(out.hostCode, "gpu-tests", "metal", "gpuLaunchVecAddBounded.host", ".cpp"));
	}
	SECTION("gpuLaunchSaxpy - device") {
		auto ir = traceToIR(details::createFunctionWrapper(gpuLaunchSaxpy));
		auto out = gpu::lowerToMetal(ir, options);
		REQUIRE(checkCodegenFile(out.deviceCode, "gpu-tests", "metal", "gpuLaunchSaxpy.device", ".metal"));
	}
	SECTION("gpuLaunchSaxpy - host") {
		auto ir = traceToIR(details::createFunctionWrapper(gpuLaunchSaxpy));
		auto out = gpu::lowerToMetal(ir, options);
		REQUIRE(checkCodegenFile(out.hostCode, "gpu-tests", "metal", "gpuLaunchSaxpy.host", ".cpp"));
	}
	SECTION("gpuLaunchVecScale - device") {
		auto ir = traceToIR(details::createFunctionWrapper(gpuLaunchVecScale));
		auto out = gpu::lowerToMetal(ir, options);
		REQUIRE(checkCodegenFile(out.deviceCode, "gpu-tests", "metal", "gpuLaunchVecScale.device", ".metal"));
	}
	SECTION("gpuLaunchVecScale - host") {
		auto ir = traceToIR(details::createFunctionWrapper(gpuLaunchVecScale));
		auto out = gpu::lowerToMetal(ir, options);
		REQUIRE(checkCodegenFile(out.hostCode, "gpu-tests", "metal", "gpuLaunchVecScale.host", ".cpp"));
	}
}
#endif

} // namespace nautilus::engine

#include "nautilus/Engine.hpp"
#include <catch2/catch_all.hpp>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>

namespace nautilus::engine {

val<int32_t> debugAdd(val<int32_t> x) {
        val<int32_t> y = x + 1;
        return y;
}

} // namespace nautilus::engine

TEST_CASE("LLVM IR Test: debug info emission", "[llvm][debug-info]") {
        using nautilus::engine::debugAdd;

        std::string dumpPath = "/tmp/nautilus-ir-test";
        std::filesystem::remove_all(dumpPath);
        std::filesystem::create_directories(dumpPath);

        nautilus::engine::Options options;
        options.setOption("engine.backend", "mlir");
        options.setOption("dump.after_llvm_generation", true);
        options.setOption("dump.path", dumpPath);
        options.setOption("engine.normalizeFunctionNames", true);
        options.setOption("mlir.enableIntrinsics", true);
        options.setOption("mlir.enableDebugInfo", true);

        auto engine = nautilus::engine::NautilusEngine(options);
        auto function = engine.registerFunction(debugAdd);

        auto generatedLLVMFile = std::string(function.getExecutable()->getGeneratedFile("after_llvm_generation"));
        REQUIRE(!generatedLLVMFile.empty());

        std::ifstream irFile(generatedLLVMFile);
        REQUIRE(irFile.good());
        std::stringstream buffer;
        buffer << irFile.rdbuf();
        std::string ir = buffer.str();

        // Check for both per-instruction debug locations and global debug metadata.
        REQUIRE(ir.find("!dbg") != std::string::npos);
        REQUIRE(ir.find("!DICompileUnit") != std::string::npos);
        REQUIRE(ir.find("!DISubprogram") != std::string::npos);
}

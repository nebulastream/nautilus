#pragma once

#include "nautilus/compiler/backends/CompilationBackend.hpp"
#include <string>
#include <string_view>
#include <vector>

namespace nautilus::compiler::mlir {

struct MLIRCacheArtifacts {
	std::string bytecode;
	std::string object;
	std::string moduleManifest;
	std::vector<std::string> externalSymbols;
	std::vector<void*> externalAddresses;
};

/**
 * @brief Compilation backend that uses MLIR.
 */
class MLIRCompilationBackend : public CompilationBackend {
public:
	MLIRCompilationBackend();
	std::unique_ptr<Executable> compile(const std::shared_ptr<ir::IRGraph>& ir, const DumpHandler& dumpHandler,
	                                    const engine::Options& options,
	                                    CompilationStatistics* statistics = nullptr) const override;

	[[nodiscard]] ExceptionPropagationMode getExceptionPropagationMode() const override {
		return ExceptionPropagationMode::NativeUnwind;
	}

	/// LLVM's pipeline folds constants, simplifies the CFG and eliminates
	/// dead code itself, so those IR passes would only repeat that work
	/// before the lowering -- but it is slower on a graph whose block
	/// arguments were never pruned (see `IROptimizationLevel`).
	[[nodiscard]] IROptimizationLevel irOptimizationLevel() const override {
		return IROptimizationLevel::ArgumentPruning;
	}

	std::unique_ptr<Executable>
	compileWithCacheArtifacts(const std::shared_ptr<ir::IRGraph>& ir, const std::vector<std::string>& exportNames,
	                          const DumpHandler& dumpHandler, const engine::Options& options,
	                          CompilationStatistics* statistics, MLIRCacheArtifacts& artifacts) const;

	std::unique_ptr<Executable> compileCachedBytecode(
	    std::string_view bytecode, std::string_view moduleManifest, const std::vector<std::string>& externalSymbols,
	    const std::vector<void*>& externalAddresses, const std::vector<std::string>& exportNames,
	    const DumpHandler& dumpHandler, const engine::Options& options, CompilationStatistics* statistics,
	    MLIRCacheArtifacts* regeneratedArtifacts = nullptr, const std::string& compilationUnitId = {}) const;

	std::unique_ptr<Executable> compileCachedObject(std::string_view object,
	                                                const std::vector<std::string>& externalSymbols,
	                                                const std::vector<void*>& externalAddresses,
	                                                const std::vector<std::string>& exportNames,
	                                                const engine::Options& options, CompilationStatistics* statistics,
	                                                const std::string& compilationUnitId = {}) const;

private:
	std::unique_ptr<Executable> compileIR(const std::shared_ptr<ir::IRGraph>& ir,
	                                      const std::vector<std::string>* exportNames, const DumpHandler& dumpHandler,
	                                      const engine::Options& options, CompilationStatistics* statistics,
	                                      MLIRCacheArtifacts* artifacts) const;
};

} // namespace nautilus::compiler::mlir

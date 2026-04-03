#pragma once

#include "nautilus/JITCompiler.hpp"
#include <functional>
#include <list>
#include <memory>

namespace nautilus::compiler {

/**
 * @brief Standard single-tier JIT compiler implementation.
 *
 * Compiles functions using a single backend (selected via options).
 * This is the original Nautilus compilation strategy.
 */
class LegacyCompiler : public JITCompiler {
public:
	LegacyCompiler();
	LegacyCompiler(engine::Options options);
	~LegacyCompiler() override;

	[[nodiscard]] std::unique_ptr<Executable> compile(wrapper_function function) const override;
	[[nodiscard]] std::unique_ptr<Executable> compile(std::list<CompilableFunction>& functions) const override;

	std::string getName() const override;
	const engine::Options& getOptions() const override {
		return options;
	}

	/**
	 * @brief Trace and convert functions to IR without backend compilation.
	 * @return Shared IR graph that can be compiled by any backend
	 */
	[[nodiscard]] std::shared_ptr<ir::IRGraph> compileToIR(std::list<CompilableFunction>& functions) const;

	/**
	 * @brief Compile a pre-built IR graph with a specific backend.
	 * @param ir The IR graph
	 * @param backendName Which backend to use
	 * @return Compiled executable
	 */
	[[nodiscard]] std::unique_ptr<Executable> compileIR(const std::shared_ptr<ir::IRGraph>& ir,
	                                                    const std::string& backendName) const;

private:
	const engine::Options options;
	const CompilationBackendRegistry* backends;
};
} // namespace nautilus::compiler

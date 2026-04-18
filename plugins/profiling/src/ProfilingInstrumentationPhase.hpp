#pragma once

#include "nautilus/compiler/ir/operations/FunctionOperation.hpp"
#include "nautilus/compiler/ir/operations/Operation.hpp"
#include "nautilus/compiler/ir/passes/IRPass.hpp"
#include <memory>

namespace nautilus::profiling {

class Profiler;
class ProfileRegion;

/// Plugin IR pass that auto-instruments every function in an `IRGraph`
/// with `ProfileRegion::runtimeStart` / `runtimeStop` proxy calls. Owns
/// the `Profiler` via a `shared_ptr`; a copy of that handle is returned
/// to the user by `Profiler::enableForModule` so the profiler outlives
/// both the pass and the `NautilusModule`.
class ProfilingIRPass : public nautilus::IRPass {
public:
	ProfilingIRPass();

	std::string getName() const override;
	void apply(compiler::ir::IRGraph& ir) override;

	std::shared_ptr<Profiler> getProfiler() const {
		return profiler_;
	}

private:
	std::shared_ptr<Profiler> profiler_;
	uint32_t nextId_ = 0;

	compiler::ir::OperationIdentifier nextIdentifier();
	void instrumentFunction(compiler::ir::IRGraph& ir, compiler::ir::FunctionOperation& func, ProfileRegion* region);
	void insertProxyCall(compiler::ir::IRGraph& ir, compiler::ir::BasicBlock& block, compiler::ir::Operation* before,
	                     const char* symbol, void* functionPtr, compiler::ir::Operation* regionPtr);
};

} // namespace nautilus::profiling

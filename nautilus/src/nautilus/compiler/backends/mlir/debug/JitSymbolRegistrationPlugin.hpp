#pragma once

#if defined(__linux__)

#include "nautilus/compiler/JitSymbolRegistry.hpp"
#include <llvm/ExecutionEngine/Orc/ObjectLinkingLayer.h>

namespace nautilus::compiler::mlir {

// Publishes every JIT-linked code range into the process-wide
// JitSymbolRegistry, so a profiler running inside this process can name
// nautilus's generated code.
//
// This is the in-process counterpart of PerfJitDumpPlugin, and deliberately a
// separate plugin rather than a mode of it. The two answer different questions
// -- one writes a file for `perf inject --jit` to synthesize ELFs from later,
// the other hands addresses to a sampler running right now -- and they are
// enabled by different options (`perf` and `perf.sample`). Keeping them apart
// means neither can perturb the other's records, and both can be on at once.
//
// What they do share is how a code range gets its name, which is the part with
// all the subtlety in it: see JitCodeRanges.hpp.
//
// Unlike the jitdump writer this keeps no file, needs no external tooling, and
// does not care about record ordering -- it only ever appends to a table.
class JitSymbolRegistrationPlugin : public llvm::orc::ObjectLinkingLayer::Plugin {
public:
	JitSymbolRegistrationPlugin(bool emitRegionSymbols, ModuleIndex moduleIndex);

	void modifyPassConfig(llvm::orc::MaterializationResponsibility& mr, llvm::jitlink::LinkGraph& graph,
	                      llvm::jitlink::PassConfiguration& config) override;

	llvm::Error notifyFailed(llvm::orc::MaterializationResponsibility&) override {
		return llvm::Error::success();
	}
	// Withdrawal is not done here. A ResourceKey is an ORC-internal grouping
	// that does not correspond to "this module's code is gone", and the code
	// outlives the key in the cases that matter. MLIRJit removes the module's
	// ranges in its destructor instead, which is the point at which the memory
	// they describe is actually freed.
	llvm::Error notifyRemovingResources(llvm::orc::JITDylib&, llvm::orc::ResourceKey) override {
		return llvm::Error::success();
	}
	void notifyTransferringResources(llvm::orc::JITDylib&, llvm::orc::ResourceKey, llvm::orc::ResourceKey) override {
	}

private:
	bool emitRegionSymbols_;
	ModuleIndex moduleIndex_;
};

} // namespace nautilus::compiler::mlir

#endif // __linux__

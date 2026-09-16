#pragma once

#if defined(__linux__)

#include <atomic>
#include <cstdint>
#include <llvm/ExecutionEngine/Orc/Core.h>
#include <llvm/ExecutionEngine/Orc/ObjectLinkingLayer.h>

namespace nautilus::compiler::mlir {

// Writes perf jitdump records for every JIT-linked object, in place of
// llvm::orc::PerfSupportPlugin. Two things that plugin cannot do make a
// Nautilus-side writer necessary:
//
// 1. Record order. `perf inject --jit` pairs JIT_CODE_DEBUG_INFO with
//    JIT_CODE_LOAD *positionally*: it keeps one pending line table, hands it
//    to the next load record, and drops it. PerfSupportPlugin collects a whole
//    object into one PerfJITRecordBatch of two separate vectors, and
//    JITLoaderPerf writes every debug record before every code record -- so
//    with more than one function in the object (every Nautilus compile, since
//    even a trivial kernel emits `execute` plus its `_mlir_ciface_*`
//    trampolines) each load record receives the wrong function's line table,
//    or none. This plugin emits one batch per code range, so each debug record
//    is immediately followed by the load record it describes.
//
// 2. region() visibility. The jitdump format has no scope tree -- its debug
//    record is a flat line table, with no subprogram records and no inline
//    nesting -- so the DW_TAG_inlined_subroutine chain that makes a region its
//    own GDB backtrace frame (docs/region.md) cannot travel through it, and
//    every region collapses into its enclosing function in a profile. Symbol
//    names are the one channel the format does carry, so `emitRegionSymbols`
//    re-expresses the DWARF inline stack as qualified symbol names
//    (`execute::outer::hot`), recovered per address from the very DWARF the
//    MLIR backend already emits.
class PerfJitDumpPlugin : public llvm::orc::ObjectLinkingLayer::Plugin {
public:
	PerfJitDumpPlugin(llvm::orc::ExecutorProcessControl& epc, llvm::orc::ExecutorAddr registerStart,
	                  llvm::orc::ExecutorAddr registerEnd, llvm::orc::ExecutorAddr registerImpl, bool emitDebugInfo,
	                  bool emitUnwindInfo, bool emitRegionSymbols);
	~PerfJitDumpPlugin() override;

	void modifyPassConfig(llvm::orc::MaterializationResponsibility& mr, llvm::jitlink::LinkGraph& graph,
	                      llvm::jitlink::PassConfiguration& config) override;

	llvm::Error notifyFailed(llvm::orc::MaterializationResponsibility&) override {
		return llvm::Error::success();
	}
	llvm::Error notifyRemovingResources(llvm::orc::JITDylib&, llvm::orc::ResourceKey) override {
		return llvm::Error::success();
	}
	void notifyTransferringResources(llvm::orc::JITDylib&, llvm::orc::ResourceKey, llvm::orc::ResourceKey) override {
	}

private:
	llvm::orc::ExecutorProcessControl& epc_;
	llvm::orc::ExecutorAddr registerEnd_;
	llvm::orc::ExecutorAddr registerImpl_;
	std::atomic<uint64_t> codeIndex_ {0};
	bool emitDebugInfo_;
	bool emitUnwindInfo_;
	bool emitRegionSymbols_;
};

} // namespace nautilus::compiler::mlir

#endif // __linux__

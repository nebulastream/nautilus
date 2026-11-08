#pragma once

#include <llvm/ExecutionEngine/Orc/CompileUtils.h>
#include <llvm/ExecutionEngine/Orc/IRCompileLayer.h>
#include <llvm/ExecutionEngine/Orc/JITTargetMachineBuilder.h>
#include <llvm/Support/CodeGen.h>
#include <memory>

namespace nautilus::compiler::mlir {

/// The code generator of a JIT: compiles each module with the target machine that the calling thread keeps for the
/// optimization level (`MLIRThreadState::targetMachine`), instead of building a target machine of its own.
///
/// LLJIT builds one target machine per JIT, and again for its data layout; with the machine comes its subtarget, whose
/// feature string is parsed on the first function it compiles. On a small module that is about 40% of all the work of
/// the compile. A machine of the thread is built once, together with the subtargets it has cached, and outlives the
/// JITs. It is the same machine -- the host builder's, at the same level -- that LLJIT would have built, so the code is
/// the same.
///
/// A module is compiled by whichever thread looks up its first symbol (code is generated lazily), which need not be the
/// thread that created the JIT, so the machine is looked up per module and never stored.
std::unique_ptr<llvm::orc::IRCompileLayer::IRCompiler>
makeThreadTargetMachineCompiler(const llvm::orc::JITTargetMachineBuilder& builder, llvm::CodeGenOptLevel level);

} // namespace nautilus::compiler::mlir

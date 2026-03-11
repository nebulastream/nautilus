
#pragma once

#include <llvm/IR/Module.h>

namespace nautilus::compiler::mlir {

/**
 * @brief Adds DWARF debug metadata to a JIT-compiled LLVM module.
 *
 * Creates DICompileUnit, DIFile, DISubprogram, and attaches
 * debug locations (from MLIR FileLineColLoc) to LLVM instructions.
 * The "execute" function gets a DISubprogram so the debugger can step through it.
 * Instructions without DebugLoc get one propagated from a neighboring instruction.
 */
class DebugInfoPass {
public:
	/// Add debug info to the module.
	static void run(llvm::Module& module);
};

} // namespace nautilus::compiler::mlir

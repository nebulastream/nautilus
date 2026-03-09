
#pragma once

#include <llvm/IR/Module.h>
#include <string>
#include <unordered_map>

namespace nautilus::compiler::mlir {

/**
 * @brief Inserts llvm.dbg.value intrinsics for SSA values,
 * making them visible as local variables in the debugger.
 *
 * Variables are named using the nameMap (IR identifier -> human name).
 * If no entry exists for a value, it is named positionally: v0, v1, v2, etc.
 */
class DebugVariablePass {
public:
	/// Add debug variable intrinsics to the "execute" function.
	/// nameMap: maps LLVM value names to human-readable debugger names.
	static void run(llvm::Module& module, const std::unordered_map<std::string, std::string>& nameMap = {});
};

} // namespace nautilus::compiler::mlir

#include "nautilus/compiler/backends/mlir/LLVMBackendHooks.hpp"

namespace nautilus::compiler::mlir {

LLVMBackendHooks& getLLVMBackendHooks() {
	static LLVMBackendHooks instance;
	return instance;
}

} // namespace nautilus::compiler::mlir

#include "nautilus/compiler/backends/mlir/DebugInfoBuilder.hpp"
#include <mlir/IR/Builders.h>
#include <mlir/IR/BuiltinAttributes.h>

namespace nautilus::compiler::mlir {

DebugInfoBuilder::DebugInfoBuilder(::mlir::MLIRContext& ctx, ::mlir::OpBuilder& b)
    : context(&ctx), builder(&b) {}

void DebugInfoBuilder::createModuleDebugInfo([[maybe_unused]] ::mlir::ModuleOp moduleOp,
                                             [[maybe_unused]] const std::string& sourceFileName) {
	if (moduleDebugInfoCreated) {
		return;
	}

	// Reserved for future debug metadata generation
	// Currently, location information is set directly on operations

	moduleDebugInfoCreated = true;
}

void DebugInfoBuilder::annotateOperationWithDebugInfo(::mlir::Operation* op,
                                                      const std::string& semanticName,
                                                      const std::string& fileName, unsigned line,
                                                      unsigned column) {
	if (!moduleDebugInfoCreated) {
		return;
	}

	// Create a fused location combining file/line/column with the semantic name
	// This preserves the semantic name through MLIR->LLVM lowering
	auto baseLocation = ::mlir::FileLineColLoc::get(::mlir::StringAttr::get(context, fileName), line, column);
	auto namedLocation = ::mlir::NameLoc::get(::mlir::StringAttr::get(context, semanticName), baseLocation);

	// Set the operation's location with the combined information
	op->setLoc(namedLocation);
}

} // namespace nautilus::compiler::mlir

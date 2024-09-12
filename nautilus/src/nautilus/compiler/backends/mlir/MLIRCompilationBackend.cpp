

#include "nautilus/compiler/backends/mlir/MLIRCompilationBackend.hpp"
#include "nautilus/compiler/backends/mlir/JITCompiler.hpp"
#include "nautilus/compiler/backends/mlir/LLVMIROptimizer.hpp"
#include "nautilus/compiler/backends/mlir/MLIRExecutable.hpp"
#include "nautilus/compiler/backends/mlir/MLIRLoweringProvider.hpp"
#include "nautilus/compiler/backends/mlir/MLIRPassManager.hpp"
#include "nautilus/compiler/ir/IRGraph.hpp"
#include <mlir/Parser/Parser.h>
#include <iostream>
#include <mlir/Debug/DebuggerExecutionContextHook.h>
#include <mlir/Dialect/Func/Extensions/AllExtensions.h>
#include <mlir/Dialect/Func/IR/FuncOps.h>
#include <mlir/IR/MLIRContext.h>
#include <mlir/Dialect/LLVMIR/LLVMDialect.h>
#include <mlir/Target/LLVMIR/Dialect/Builtin/BuiltinToLLVMIRTranslation.h>
#include <mlir/Target/LLVMIR/Dialect/LLVMIR/LLVMToLLVMIRTranslation.h>
#include "llvm/BinaryFormat/Dwarf.h"

#include "llvm/Support/Debug.h"
#include "llvm/Support/Path.h"
namespace nautilus::compiler::mlir {

static ::mlir::WalkResult interruptIfValidLocation(::mlir::Operation *op) {
	return isa<::mlir::UnknownLoc>(op->getLoc()) ? ::mlir::WalkResult::advance()
	                                     : ::mlir::WalkResult::interrupt();
}


::mlir::FileLineColLoc extractFileLoc(::mlir::Location loc) {
	if (auto fileLoc = dyn_cast<::mlir::FileLineColLoc>(loc))
		return fileLoc;
	if (auto nameLoc = dyn_cast<::mlir::NameLoc>(loc))
		return extractFileLoc(nameLoc.getChildLoc());
	if (auto opaqueLoc = dyn_cast<::mlir::OpaqueLoc>(loc))
		return extractFileLoc(opaqueLoc.getFallbackLocation());
	return ::mlir::FileLineColLoc();
}

std::unique_ptr<Executable> MLIRCompilationBackend::compile(const std::shared_ptr<ir::IRGraph>& ir, const DumpHandler& dumpHandler, const engine::Options& options) {

	// 1. Create the MLIRLoweringProvider and lower the given NESIR. Return an
	// MLIR module.
	::mlir::DialectRegistry registry;
	::mlir::func::registerAllExtensions(registry);
	registerBuiltinDialectTranslation(registry);
	registerLLVMDialectTranslation(registry);
	    ::mlir::MLIRContext context(registry);
	context.allowsUnregisteredDialects();

	auto loweringProvider = std::make_unique<MLIRLoweringProvider>(context);
	auto mlirModule = loweringProvider->generateModuleFromIR(ir);
	if (*mlirModule == nullptr) {
		throw RuntimeException("verification of MLIR module failed!");
	};

	// 2.b Take the MLIR module from the MLIRLoweringProvider and apply lowering
	// and optimization passes.
	if (mlir::MLIRPassManager::lowerAndOptimizeMLIRModule(mlirModule, {}, {})) {
		throw RuntimeException("Could not lower and optimize MLIR module.");
	}

	// 2.a dump MLIR to console or a file

	 context.getDiagEngine().registerHandler([]([[maybe_unused]]::mlir::Diagnostic &diag) {

		return;
	});
	//::mlir::OwningOpRef<::mlir::Operation*> res = ::mlir::parseSourceFile(path, &context);

	//auto mod = static_cast<::mlir::ModuleOp>(res.release());
	//mlirModule = ::mlir::OwningOpRef<::mlir::ModuleOp>(mod);




	// 3. Lower MLIR module to LLVM IR and create LLVM IR optimization pipeline.
	auto optPipeline = LLVMIROptimizer::getLLVMOptimizerPipeline(options, dumpHandler);

	if (!mlirModule->walk(interruptIfValidLocation).wasInterrupted()){
		assert(false);
	}


	::mlir::LLVM::DICompileUnitAttr compileUnitAttr;
	auto fusedCompileUnitAttr =
	    mlirModule->getLoc()
	        ->findInstanceOf<::mlir::FusedLocWith<::mlir::LLVM::DICompileUnitAttr>>();
	if (fusedCompileUnitAttr) {
		compileUnitAttr = fusedCompileUnitAttr.getMetadata();
	} else {
		::mlir::LLVM::DIFileAttr fileAttr;
		if (::mlir::FileLineColLoc fileLoc = extractFileLoc(mlirModule->getLoc())) {
			::mlir::StringRef inputFilePath = fileLoc.getFilename().getValue();
			fileAttr = ::mlir::LLVM::DIFileAttr::get(
			    mlirModule->getContext(), llvm::sys::path::filename(inputFilePath),
			    llvm::sys::path::parent_path(inputFilePath));
		} else {
			fileAttr =::mlir::LLVM::DIFileAttr::get(mlirModule->getContext(), "<unknown>", "");
		}

		compileUnitAttr = ::mlir::LLVM::DICompileUnitAttr::get(
		    mlirModule->getContext(), ::mlir::DistinctAttr::create(::mlir::UnitAttr::get(mlirModule->getContext())),
		    llvm::dwarf::DW_LANG_C, fileAttr, ::mlir::StringAttr::get(mlirModule->getContext(), "NAUTILUS"),
		    /*isOptimized=*/true, ::mlir::LLVM::DIEmissionKind::Full);
	}
	mlirModule->walk([&](::mlir::LLVM::LLVMFuncOp llvmFunc) {
		::mlir::LLVM::DICompileUnitAttr cu = compileUnitAttr;
		::mlir::Location loc = llvmFunc.getLoc();
		if (loc->findInstanceOf<::mlir::FusedLocWith<::mlir::LLVM::DISubprogramAttr>>())
			return;

		::mlir::MLIRContext *context = llvmFunc->getContext();

		// Filename, line and colmun to associate to the function.
		::mlir::LLVM::DIFileAttr fileAttr;
		int64_t line = 1, col = 1;
		::mlir::FileLineColLoc fileLoc = extractFileLoc(loc);
		if (!fileLoc && compileUnitAttr) {
			fileAttr = compileUnitAttr.getFile();
		} else if (!fileLoc) {
			fileAttr = ::mlir::LLVM::DIFileAttr::get(context, "<unknown>", "");
		} else {
			line = fileLoc.getLine();
			col = fileLoc.getColumn();
			::mlir::StringRef inputFilePath = fileLoc.getFilename().getValue();
			fileAttr =
			    ::mlir::LLVM::DIFileAttr::get(context, llvm::sys::path::filename(inputFilePath),
			                          llvm::sys::path::parent_path(inputFilePath));
		}
		auto subroutineTypeAttr =
		    ::mlir::LLVM::DISubroutineTypeAttr::get(context, llvm::dwarf::DW_CC_normal, {});

		::mlir::StringAttr funcNameAttr = llvmFunc.getNameAttr();
		// Only definitions need a distinct identifier and a compilation unit.
		::mlir::DistinctAttr id;
		auto subprogramFlags = ::mlir::LLVM::DISubprogramFlags::Optimized;
		if (!llvmFunc.isExternal()) {
			id = ::mlir::DistinctAttr::create(::mlir::UnitAttr::get(context));
			subprogramFlags = subprogramFlags | ::mlir::LLVM::DISubprogramFlags::Definition;
		} else {
			cu = {};
		}
		auto subprogramAttr = ::mlir::LLVM::DISubprogramAttr::get(
		    context, id, cu, fileAttr, funcNameAttr, funcNameAttr,
		    fileAttr,
		    /*line=*/line,
		    /*scopeline=*/col, subprogramFlags, subroutineTypeAttr);
		llvmFunc->setLoc(::mlir::FusedLoc::get(context, {loc}, subprogramAttr));
	});


	mlirModule->walk([&](::mlir::LLVM::AddOp addOp) {
        auto lo = addOp.getLoc();
		auto nameLoc = dyn_cast<::mlir::NameLoc>(lo);
		auto fileLoc = extractFileLoc(nameLoc);
		auto fileAttr = compileUnitAttr.getFile();
		// TODO: finish the DI logic
		unsigned arg = 0;
		// TODO: finish the DI logic
		unsigned alignInBits = 8;
		auto int32_t_str = ::mlir::StringAttr::get(mlirModule->getContext(), "NAUTILUS");
		auto diType = ::mlir::LLVM::DIDerivedTypeAttr::get(
    mlirModule->getContext(), llvm::dwarf::DW_TAG_typedef,int32_t_str, ::mlir::LLVM::DIBasicTypeAttr::get(
		                                                                            mlirModule->getContext(), llvm::dwarf::DW_TAG_base_type, "int",
                                         32, llvm::dwarf::DW_ATE_signed_char), 32,
    0, 0);

		auto name = nameLoc.getName();
		auto scope = ::mlir::LLVM::DIScopeAttr();
		auto varInfo = ::mlir::LLVM::DILocalVariableAttr::get(mlirModule->getContext(), scope, name, fileAttr, fileLoc.getLine(), arg, alignInBits, diType);

		addOp->setLoc(::mlir::FusedLoc::get(mlirModule->getContext(), {nameLoc}, varInfo));
	});


	std::string path;
	dumpHandler.dump("after_mlir_generation", "mlir", [&](auto p) {
		::mlir::OpPrintingFlags flags;
		flags.enableDebugInfo(true, false);
		std::string result;
		auto output = llvm::raw_string_ostream(result);
		mlirModule->print(output, flags);
		path = p + "_2";
		auto* basicError = new std::error_code();
		llvm::raw_fd_ostream fileStream(path, *basicError);
		fileStream.write(result.c_str(), result.length());
		return result;
	});

	// 4. JIT compile LLVM IR module and return engine that provides access
	// compiled execute function.
	auto engine = JITCompiler::jitCompileModule(mlirModule, optPipeline, loweringProvider->getJitProxyFunctionSymbols(), loweringProvider->getJitProxyTargetAddresses());


	::mlir::tracing::ExecutionContext executionContext;
	::mlir::setupDebuggerExecutionContextHook(executionContext);
	::mlirDebuggerPrintContext();

   // engine->dumpToObjectFile("./test.ob");

	// 5. Get execution function from engine. Create and return execution context.
	return std::make_unique<MLIRExecutable>(std::move(engine));
}

} // namespace nautilus::compiler::mlir

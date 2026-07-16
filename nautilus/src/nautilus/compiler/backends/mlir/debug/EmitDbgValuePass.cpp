
#include "nautilus/compiler/backends/mlir/debug/EmitDbgValuePass.hpp"
#include <llvm/ADT/DenseMap.h>
#include <llvm/ADT/SetVector.h>
#include <llvm/ADT/StringRef.h>
#include <llvm/BinaryFormat/Dwarf.h>
#include <mlir/Dialect/LLVMIR/LLVMAttrs.h>
#include <mlir/Dialect/LLVMIR/LLVMDialect.h>
#include <mlir/Dialect/LLVMIR/LLVMTypes.h>
#include <mlir/IR/Builders.h>
#include <mlir/IR/BuiltinAttributes.h>
#include <mlir/IR/Location.h>
#include <mlir/IR/Operation.h>
#include <mlir/IR/Value.h>
#include <mlir/Pass/Pass.h>
#include <string>
#include <unordered_map>

namespace nautilus::compiler::mlir {

namespace {

// Parse `$<digits>` and return the digits, or empty if the string is not
// a Nautilus-identifier-form NameLoc name.
llvm::StringRef nautilusIdFromName(llvm::StringRef s) {
	if (s.size() < 2 || s.front() != '$') {
		return {};
	}
	llvm::StringRef rest = s.drop_front();
	for (char c : rest) {
		if (c < '0' || c > '9') {
			return {};
		}
	}
	return rest;
}

// Walk the Location tree and return the first NameLoc whose name is a
// `$N` identifier, or a null NameLoc if none exists.
::mlir::NameLoc findDollarNameLoc(::mlir::Location loc) {
	::mlir::NameLoc result;
	loc->walk([&](::mlir::Location inner) {
		if (auto name = llvm::dyn_cast<::mlir::NameLoc>(inner)) {
			if (!nautilusIdFromName(name.getName().strref()).empty()) {
				result = name;
				return ::mlir::WalkResult::interrupt();
			}
		}
		return ::mlir::WalkResult::advance();
	});
	return result;
}

// Walk the Location tree looking for a FusedLoc whose metadata is a
// DISubprogramAttr.  Returns the attribute or a null attr if absent.
::mlir::LLVM::DISubprogramAttr findSubprogram(::mlir::Location loc) {
	::mlir::LLVM::DISubprogramAttr result;
	loc->walk([&](::mlir::Location inner) {
		if (auto fused = llvm::dyn_cast<::mlir::FusedLoc>(inner)) {
			if (auto sp = llvm::dyn_cast_or_null<::mlir::LLVM::DISubprogramAttr>(fused.getMetadata())) {
				result = sp;
				return ::mlir::WalkResult::interrupt();
			}
		}
		return ::mlir::WalkResult::advance();
	});
	return result;
}

// Derive a DIBasicTypeAttr for the given LLVM-dialect / builtin type.
// Keeps the mapping intentionally simple: integer widths map to signed
// DW_ATE_signed, floating widths to DW_ATE_float, pointers to an
// address-typed basic type.  Returns null if the type is not supported
// (void, composite, etc.); callers skip emission in that case.
::mlir::LLVM::DITypeAttr basicTypeFor(::mlir::Type type, ::mlir::MLIRContext* ctx) {
	using ::mlir::LLVM::DIBasicTypeAttr;
	auto basic = [&](const char* name, uint64_t bits, unsigned encoding) -> ::mlir::LLVM::DITypeAttr {
		return DIBasicTypeAttr::get(ctx, llvm::dwarf::DW_TAG_base_type, name, bits, encoding);
	};
	if (auto intTy = llvm::dyn_cast<::mlir::IntegerType>(type)) {
		const unsigned bits = intTy.getWidth();
		const char* name = bits == 1 ? "bool" : bits == 8 ? "i8" : bits == 16 ? "i16" : bits == 32 ? "i32" : "i64";
		const unsigned enc = bits == 1 ? llvm::dwarf::DW_ATE_boolean : llvm::dwarf::DW_ATE_signed;
		return basic(name, bits, enc);
	}
	if (llvm::isa<::mlir::Float32Type>(type)) {
		return basic("f32", 32, llvm::dwarf::DW_ATE_float);
	}
	if (llvm::isa<::mlir::Float64Type>(type)) {
		return basic("f64", 64, llvm::dwarf::DW_ATE_float);
	}
	if (llvm::isa<::mlir::LLVM::LLVMPointerType>(type)) {
		return basic("ptr", 64, llvm::dwarf::DW_ATE_address);
	}
	return {};
}

struct EmitDbgValuePass : public ::mlir::PassWrapper<EmitDbgValuePass, ::mlir::OperationPass<::mlir::ModuleOp>> {
	MLIR_DEFINE_EXPLICIT_INTERNAL_INLINE_TYPE_ID(EmitDbgValuePass)

	llvm::StringRef getArgument() const final {
		return "nautilus-emit-dbg-value";
	}
	llvm::StringRef getDescription() const final {
		return "Scope Nautilus $N shadow allocas per basic block and attach llvm.intr.dbg.declare";
	}

	void getDependentDialects(::mlir::DialectRegistry& registry) const override {
		registry.insert<::mlir::LLVM::LLVMDialect>();
	}

	void runOnOperation() override {
		::mlir::ModuleOp module = getOperation();
		::mlir::MLIRContext* ctx = module.getContext();

		// Shadow allocas pair with a dbg.declare whose DIExpression
		// simply states "the variable lives at this address" — no
		// stack-value rewriting needed since every $N has a real
		// memory slot.
		auto emptyExpr = ::mlir::LLVM::DIExpressionAttr::get(ctx, {});

		module.walk([&](::mlir::LLVM::LLVMFuncOp funcOp) {
			auto subprogram = findSubprogram(funcOp->getLoc());
			if (!subprogram || funcOp.empty()) {
				return;
			}
			auto file = subprogram.getFile();
			::mlir::OpBuilder builder(ctx);

			// Walk allocas BEFORE any location rewriting so the `$N`
			// NameLoc is still directly reachable from each alloca's
			// location root.
			std::unordered_map<std::string, ::mlir::LLVM::AllocaOp> allocaForId;
			funcOp.walk([&](::mlir::LLVM::AllocaOp alloca) {
				if (auto nameLoc = findDollarNameLoc(alloca->getLoc())) {
					allocaForId.try_emplace(nameLoc.getName().str(), alloca);
				}
			});

			// One DILexicalBlock per MLIR basic block.  Wrapping every
			// op's location in a FusedLoc tagged with the block scope
			// tells MLIR's debug translator to emit !DILocation with
			// that scope, which in turn defines the DILexicalBlock's
			// PC range in the emitted DWARF.  Variables scoped to the
			// block then only appear in GDB when execution is inside
			// it.
			auto firstLineIn = [](::mlir::Block& block) -> unsigned {
				for (auto& op : block) {
					if (auto fl = op.getLoc()->findInstanceOf<::mlir::FileLineColLoc>()) {
						if (fl.getLine() != 0) {
							return fl.getLine();
						}
					}
				}
				return 0;
			};
			llvm::DenseMap<::mlir::Block*, ::mlir::LLVM::DILexicalBlockAttr> blockScopes;
			for (auto& block : funcOp.getBody()) {
				blockScopes[&block] =
				    ::mlir::LLVM::DILexicalBlockAttr::get(ctx, subprogram, file, firstLineIn(block), /*column=*/1);
			}
			for (auto& block : funcOp.getBody()) {
				auto scope = blockScopes[&block];
				for (auto& op : block) {
					op.setLoc(::mlir::FusedLoc::get({op.getLoc()}, scope, ctx));
				}
			}

			// Discover which blocks store into each alloca.  Every
			// store is a definition/refresh of $N in that block — we
			// use the set to emit one dbg.declare per (block, $N).
			llvm::DenseMap<::mlir::Operation*, llvm::SetVector<::mlir::Block*>> blocksStoringAlloca;
			for (auto& kv : allocaForId) {
				blocksStoringAlloca[kv.second.getOperation()];
			}
			funcOp.walk([&](::mlir::LLVM::StoreOp store) {
				if (auto* defOp = store.getAddr().getDefiningOp()) {
					if (auto it = blocksStoringAlloca.find(defOp); it != blocksStoringAlloca.end()) {
						it->second.insert(store->getBlock());
					}
				}
			});

			for (const auto& kv : allocaForId) {
				auto alloca = kv.second;
				auto diType = basicTypeFor(alloca.getElemType(), ctx);
				if (!diType) {
					continue;
				}
				// DILocalVariable names drop the leading `$` for a
				// `v` prefix — GDB's value-history syntax reserves
				// `$<digits>` and would confuse DAP clients otherwise.
				auto rawName = findDollarNameLoc(alloca->getLoc()).getName().strref();
				std::string displayName = "v";
				displayName.append((!rawName.empty() && rawName.front() == '$') ? rawName.drop_front(1) : rawName);

				unsigned declLine = 0;
				if (auto attr = alloca->getAttrOfType<::mlir::IntegerAttr>("nautilus.debug.decl_line")) {
					declLine = static_cast<unsigned>(attr.getInt());
				}

				for (auto* block : blocksStoringAlloca[alloca.getOperation()]) {
					auto blockScope = blockScopes[block];
					auto localVar = ::mlir::LLVM::DILocalVariableAttr::get(
					    ctx, blockScope, ::mlir::StringAttr::get(ctx, displayName), file, declLine,
					    /*arg=*/0, /*alignInBits=*/0, diType, ::mlir::LLVM::DIFlags::Zero);
					auto declLoc = ::mlir::FusedLoc::get(
					    {::mlir::FileLineColLoc::get(file.getName(), firstLineIn(*block), 1)}, blockScope, ctx);
					// In the entry block the dbg.declare must come
					// after its alloca to preserve dominance; elsewhere
					// anywhere inside the block will do.
					if (block == &funcOp.getBody().front()) {
						builder.setInsertionPointAfter(alloca);
					} else {
						builder.setInsertionPointToStart(block);
					}
					::mlir::LLVM::DbgDeclareOp::create(builder, declLoc, alloca.getResult(), localVar, emptyExpr);
				}
			}
		});
	}
};

} // namespace

std::unique_ptr<::mlir::Pass> createEmitDbgValuePass() {
	return std::make_unique<EmitDbgValuePass>();
}

} // namespace nautilus::compiler::mlir

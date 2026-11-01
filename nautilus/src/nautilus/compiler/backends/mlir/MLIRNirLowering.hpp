#pragma once

#include "nautilus/compiler/backends/mlir/intrinsics/MLIRBackendIntrinsic.hpp"
#include "nautilus/compiler/backends/nir/NirBackend.hpp"
#include "nautilus/nir/Module.hpp"
#include "nautilus/options.hpp"
#include <functional>
#include <mlir/IR/BuiltinOps.h>
#include <mlir/IR/MLIRContext.h>
#include <mlir/IR/OwningOpRef.h>
#include <string>
#include <vector>

namespace nautilus::compiler::mlir {

/// The hook for the types and operations of extensions (M6.5 maps `!simd.vec` and friends through it): the MLIR type
/// of an extension `TypeId`, and the lowering of an extension instruction. Nothing is registered by the core.
class NirExtensionHooks {
public:
	using TypeMapper = std::function<::mlir::Type(::mlir::MLIRContext&)>;
	/// Lowers extension instruction `inst` of module `m` given the lowered operands; returns its result (null for
	/// `void`). The builder is positioned at the instruction.
	using OpLowering = std::function<::mlir::Value(::mlir::OpBuilder&, ::mlir::Location, const nir::Module& m,
	                                               const nir::Inst& inst, llvm::ArrayRef<::mlir::Value> operands)>;

	static NirExtensionHooks& instance();
	void registerType(nir::TypeId type, TypeMapper mapper);
	void registerOp(uint16_t op, OpLowering lowering);
	[[nodiscard]] const TypeMapper* type(nir::TypeId type) const;
	[[nodiscard]] const OpLowering* op(uint16_t op) const;

private:
	std::unordered_map<uint8_t, TypeMapper> types_;
	std::unordered_map<uint16_t, OpLowering> ops_;
};

/// The MLIR module of a NIR module and the native functions it calls: the JIT resolves `proxySymbols[i]` to
/// `proxyAddresses[i]`.
struct MLIRNirModule {
	::mlir::OwningOpRef<::mlir::ModuleOp> module;
	std::vector<std::string> proxySymbols;
	std::vector<void*> proxyAddresses;
};

/// Lowers every function of the SSA module `module` to `func.func` (blocks and terminators to `cf`, arithmetic to
/// `arith`, memory and pointers to `llvm`), with the checks of D10 and the saturating casts of D11 spelled out.
/// `intrinsics` may be null. Verifies the result once. Throws if the module cannot be lowered.
MLIRNirModule lowerNirToMLIR(::mlir::MLIRContext& context, const nir::Module& module,
                             const nirbackend::HostEnvironment& host, const engine::Options& options,
                             const MLIRIntrinsicManager* intrinsics);

} // namespace nautilus::compiler::mlir

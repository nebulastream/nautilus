#include "nautilus/compiler/backends/mlir/intrinsics/MLIRBackendIntrinsic.hpp"
#include "nautilus/tracing/Types.hpp"
#include <mlir/Dialect/LLVMIR/LLVMTypes.h>
#include <mlir/IR/Builders.h>

namespace nautilus::compiler::mlir {

void MLIRIntrinsicManager::addIntrinsic(IntrinsicTarget target, IntrinsicFunction function, std::string_view name) {
	// Interning here is what gives the intrinsic an identity in the IR: the
	// function table stamps this id onto the callee's entry when it is
	// interned, so by the time lowering runs the linkage is already decided
	// and there is no address left to match against.
	const auto id = ir::IntrinsicRegistry::instance().registerIntrinsic(target, name);
	intrinsicMap[id] = std::move(function);
}

std::optional<IntrinsicFunction> MLIRIntrinsicManager::getIntrinsic(ir::IntrinsicId id) const {
	if (id == ir::IntrinsicId::None) {
		return std::nullopt;
	}
	auto it = intrinsicMap.find(id);
	if (it != intrinsicMap.end()) {
		return it->second;
	}
	return std::nullopt;
}

namespace {
/// The MLIR type of the stamp of a 0.3 call result; null for `void`.
::mlir::Type stampType(::mlir::OpBuilder& builder, Type stamp) {
	switch (stamp) {
	case Type::v:
		return {};
	case Type::b:
		return builder.getI1Type();
	case Type::ui8:
	case Type::i8:
		return builder.getI8Type();
	case Type::ui16:
	case Type::i16:
		return builder.getI16Type();
	case Type::ui32:
	case Type::i32:
		return builder.getI32Type();
	case Type::ui64:
	case Type::i64:
		return builder.getI64Type();
	case Type::f32:
		return builder.getF32Type();
	case Type::f64:
		return builder.getF64Type();
	case Type::ptr:
		return ::mlir::LLVM::LLVMPointerType::get(builder.getContext());
	}
	return {};
}
} // namespace

void MLIRIntrinsicManager::addIntrinsic(IntrinsicTarget target, NirIntrinsicFunction function, std::string_view name) {
	// the 0.3 interface: the operands come from the value frame and the result goes back into it
	addIntrinsic(target,
	             IntrinsicFunction([function](std::unique_ptr<::mlir::OpBuilder>& builder,
	                                          const ir::CallOperation* call, MLIRLoweringProvider::ValueFrame& frame) {
		             llvm::SmallVector<::mlir::Value> args;
		             for (auto* argument : call->getInputArguments()) {
			             args.push_back(frame.getValue(argument->getIdentifier()));
		             }
		             NirIntrinsicCall nirCall {builder, builder->getUnknownLoc(), args,
		                                       stampType(*builder, call->getStamp()), ::mlir::Value()};
		             if (!function(nirCall)) {
			             return false;
		             }
		             if (nirCall.result) {
			             frame.setValue(call->getIdentifier(), nirCall.result);
		             }
		             return true;
	             }),
	             name);
	addNirIntrinsic(target, std::move(function), name);
}

void MLIRIntrinsicManager::addNirIntrinsic(IntrinsicTarget target, NirIntrinsicFunction function,
                                           std::string_view /*name*/) {
	nirIntrinsicMap[target] = std::move(function);
}

const NirIntrinsicFunction* MLIRIntrinsicManager::getNirIntrinsic(IntrinsicTarget target) const {
	auto it = nirIntrinsicMap.find(target);
	return it == nirIntrinsicMap.end() ? nullptr : &it->second;
}

void MLIRIntrinsicManager::merge(const MLIRIntrinsicManager& other) {
	for (const auto& [id, function] : other.intrinsicMap) {
		intrinsicMap[id] = function;
	}
	for (const auto& [target, function] : other.nirIntrinsicMap) {
		nirIntrinsicMap[target] = function;
	}
}

void MLIRIntrinsicPluginRegistry::addPlugin(std::shared_ptr<MLIRIntrinsicPlugin> plugin) {
	std::lock_guard lock(mutex_);
	if (plugin) {
		// Harvest now rather than at compile time. Interning an intrinsic is
		// what makes a call to it read as Intrinsic in the IR, and that
		// happens during trace-to-IR conversion -- so the identities have to
		// exist by then, not merely by the time a backend lowers.
		plugin->registerIntrinsics(harvested_);
	}
	plugins_.push_back(std::move(plugin));
}

void MLIRIntrinsicPluginRegistry::registerAllIntrinsics(MLIRIntrinsicManager& manager) const {
	std::lock_guard lock(mutex_);
	manager.merge(harvested_);
}

MLIRIntrinsicPluginRegistry& MLIRIntrinsicPluginRegistry::instance() {
	static MLIRIntrinsicPluginRegistry registry;
	return registry;
}

} // namespace nautilus::compiler::mlir

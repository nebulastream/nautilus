#include "nautilus/compiler/artifact/ArtifactPreflight.hpp"
#include "nautilus/common/ConstantOrigin.hpp"
#include "nautilus/compiler/ir/IRGraph.hpp"
#include "nautilus/compiler/ir/operations/AllocaOperation.hpp"
#include "nautilus/compiler/ir/operations/BranchOperation.hpp"
#include "nautilus/compiler/ir/operations/CallOperation.hpp"
#include "nautilus/compiler/ir/operations/ConstBooleanOperation.hpp"
#include "nautilus/compiler/ir/operations/ConstFloatOperation.hpp"
#include "nautilus/compiler/ir/operations/ConstIntOperation.hpp"
#include "nautilus/compiler/ir/operations/ConstPtrOperation.hpp"
#include "nautilus/compiler/ir/operations/FunctionAddressOfOperation.hpp"
#include "nautilus/compiler/ir/operations/FunctionOperation.hpp"
#include "nautilus/compiler/ir/operations/IfOperation.hpp"
#include "nautilus/compiler/ir/operations/IndirectCallOperation.hpp"
#include "nautilus/compiler/ir/operations/RuntimeBindingOperation.hpp"
#include "nautilus/exceptions/RuntimeException.hpp"
#include <algorithm>
#include <bit>
#include <cstdint>
#include <limits>
#include <optional>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

namespace nautilus::compiler::artifact {
namespace {

struct OperationLocation {
	const ir::FunctionOperation* function = nullptr;
	const ir::BasicBlock* block = nullptr;
};

std::string operationTypeName(ir::Operation::OperationType type) {
	using Op = ir::Operation::OperationType;
	switch (type) {
	case Op::AddOp:
		return "AddOp";
	case Op::AndOp:
		return "AndOp";
	case Op::NotOp:
		return "NotOp";
	case Op::BasicBlockArgument:
		return "BasicBlockArgument";
	case Op::BlockInvocation:
		return "BlockInvocation";
	case Op::BranchOp:
		return "BranchOp";
	case Op::ConstIntOp:
		return "ConstIntOp";
	case Op::ConstBooleanOp:
		return "ConstBooleanOp";
	case Op::ConstPtrOp:
		return "ConstPtrOp";
	case Op::ConstFloatOp:
		return "ConstFloatOp";
	case Op::CastOp:
		return "CastOp";
	case Op::CompareOp:
		return "CompareOp";
	case Op::DivOp:
		return "DivOp";
	case Op::ModOp:
		return "ModOp";
	case Op::FunctionOp:
		return "FunctionOp";
	case Op::IfOp:
		return "IfOp";
	case Op::LoadOp:
		return "LoadOp";
	case Op::MulOp:
		return "MulOp";
	case Op::MLIR_YIELD:
		return "MLIR_YIELD";
	case Op::NegateOp:
		return "NegateOp";
	case Op::OrOp:
		return "OrOp";
	case Op::CallOp:
		return "CallOp";
	case Op::IndirectCallOp:
		return "IndirectCallOp";
	case Op::ReturnOp:
		return "ReturnOp";
	case Op::SelectOp:
		return "SelectOp";
	case Op::StoreOp:
		return "StoreOp";
	case Op::SubOp:
		return "SubOp";
	case Op::BinaryComp:
		return "BinaryComp";
	case Op::ShiftOp:
		return "ShiftOp";
	case Op::AllocaOp:
		return "AllocaOp";
	case Op::FunctionAddressOfOp:
		return "FunctionAddressOfOp";
	case Op::RuntimeBindingOp:
		return "RuntimeBindingOp";
	}
	return "Unknown(" + std::to_string(static_cast<unsigned>(type)) + ")";
}

std::string functionBindingRejection(const ir::IRGraph& graph, const ir::FunctionOperation* function) {
	const auto& table = graph.getFunctionTable();
	const auto id = table.findByDefinition(function);
	if (!table.contains(id)) {
		return "missing_function_binding";
	}
	const auto& target = table.get(id);
	if (target.getId() != id) {
		return "mismatched_function_target=" + std::to_string(id);
	}
	if (target.getLinkage() != ir::Linkage::Internal || target.getDefinition() != function) {
		return "mismatched_function_binding=" + std::to_string(id);
	}
	return {};
}

std::string describeOperation(const ir::Operation* operation, OperationLocation location) {
	std::string description =
	    "function=" + (location.function ? location.function->getName() : std::string {"<module>"});
	if (location.block != nullptr) {
		description += " block=" + std::to_string(location.block->getIdentifier().getId());
		const auto& operations = location.block->getOperations();
		if (const auto position = std::ranges::find(operations, operation); position != operations.end()) {
			description += " index=" + std::to_string(position - operations.begin());
		}
	}
	if (operation == nullptr) {
		return description + " operation=null";
	}
	return description + " operation=" + operation->getIdentifier().toString() +
	       " type=" + operationTypeName(operation->getOperationType());
}

} // namespace

bool hasOnlyInvariantScalars(const ir::IRGraph& graph, std::string* reason) {
	if (reason != nullptr) {
		reason->clear();
	}
	if (!graph.hasRecordedConstantOrigins()) {
		if (reason != nullptr) {
			*reason = "constant_origins_not_recorded";
		}
		return false;
	}
	const auto reject = [&](const ir::Operation* operation, OperationLocation location, const std::string& cause) {
		if (reason != nullptr) {
			*reason = describeOperation(operation, location) + " cause=" + cause;
		}
		return false;
	};
	std::unordered_set<const ir::FunctionOperation*> functions;
	std::unordered_map<const ir::BasicBlock*, const ir::FunctionOperation*> blocks;
	std::unordered_map<const ir::Operation*, const ir::FunctionOperation*> arguments;
	std::unordered_set<const ir::Operation*> definitions;
	std::unordered_map<const ir::Operation*, const ir::FunctionOperation*> included;
	std::vector<std::pair<const ir::Operation*, OperationLocation>> operations;
	const auto include = [&](const ir::Operation* operation, OperationLocation location) {
		if (operation == nullptr) {
			return reject(operation, location, "null_operand");
		}
		const auto [owner, inserted] = included.emplace(operation, location.function);
		if (!inserted && owner->second != location.function) {
			const auto cause = operation->getOperationType() == ir::Operation::OperationType::BasicBlockArgument
			                       ? "block_argument_outside_function"
			                       : "operand_outside_function";
			return reject(operation, location, cause);
		}
		if (inserted) {
			operations.emplace_back(operation, location);
		}
		return true;
	};
	const auto includeDefinition = [&](const ir::Operation* operation, OperationLocation location) {
		if (!definitions.insert(operation).second) {
			return reject(operation, location, "shared_operation");
		}
		return include(operation, location);
	};
	for (const auto* function : graph.getFunctionOperations()) {
		if (function == nullptr || function->getBasicBlocks().empty()) {
			return reject(function, {function, nullptr}, "missing_function_body");
		}
		functions.insert(function);
		if (!includeDefinition(function, {function, nullptr})) {
			return false;
		}
		for (const auto* block : function->getBasicBlocks()) {
			if (block == nullptr) {
				return reject(function, {function, nullptr}, "null_basic_block");
			}
			if (!blocks.emplace(block, function).second) {
				return reject(function, {function, block}, "shared_basic_block");
			}
			for (const auto* argument : block->getArguments()) {
				if (argument == nullptr) {
					return reject(argument, {function, block}, "null_block_argument");
				}
				arguments.emplace(argument, function);
				if (!includeDefinition(argument, {function, block})) {
					return false;
				}
			}
			for (const auto* operation : block->getOperations()) {
				if (!includeDefinition(operation, {function, block})) {
					return false;
				}
			}
		}
	}
	const auto& table = graph.getFunctionTable();
	const auto targetRejection = [&](ir::FunctionId id) -> std::string {
		if (!table.contains(id)) {
			return "missing_function_target=" + std::to_string(id);
		}
		const auto& target = table.get(id);
		if (target.getId() != id) {
			return "mismatched_function_target=" + std::to_string(id);
		}
		switch (target.getLinkage()) {
		case ir::Linkage::Internal:
			if (!functions.contains(target.getDefinition())) {
				return "internal_definition_outside_graph=" + std::to_string(id);
			}
			if (table.findByDefinition(target.getDefinition()) != id) {
				return "internal_definition_binding_mismatch=" + std::to_string(id);
			}
			break;
		case ir::Linkage::External:
		case ir::Linkage::Intrinsic:
			if (target.getAddress() == nullptr) {
				return "missing_native_address=" + std::to_string(id);
			}
			break;
		default:
			return "unsupported_function_linkage=" + std::to_string(id);
		}
		return {};
	};
	for (std::size_t index = 0; index < operations.size(); ++index) {
		const auto [operation, location] = operations[index];
		if (operation == nullptr) {
			return reject(operation, location, "null_operand");
		}
		const auto inputs = operation->getInputs();
		for (const auto* input : inputs) {
			if (input == nullptr) {
				return reject(operation, location, "null_input");
			}
			if (!include(input, location)) {
				return false;
			}
		}
		const auto includeDestructors = [&](const auto& call) {
			for (const auto& destructor : call.getDestructors()) {
				if (destructor.functionPtr == nullptr) {
					return reject(operation, location, "missing_cleanup_function");
				}
				if (destructor.address == nullptr) {
					return reject(nullptr, location, "null_operand");
				}
				if (destructor.address->getStamp() != Type::ptr) {
					return reject(operation, location, "invalid_cleanup_address_type");
				}
				if (!include(destructor.address, location)) {
					return false;
				}
			}
			return true;
		};
		std::optional<std::size_t> arity;
		using Op = ir::Operation::OperationType;
		switch (operation->getOperationType()) {
		case Op::ConstIntOp:
			if (!isInteger(operation->getStamp()) ||
			    operation->dynCast<ir::ConstIntOperation>()->getConstantOrigin() != ConstantOrigin::CacheInvariant) {
				return reject(operation, location, "uncertified_scalar");
			}
			arity = 0;
			break;
		case Op::ConstBooleanOp:
			if (operation->getStamp() != Type::b ||
			    operation->dynCast<ir::ConstBooleanOperation>()->getConstantOrigin() !=
			        ConstantOrigin::CacheInvariant) {
				return reject(operation, location, "uncertified_scalar");
			}
			arity = 0;
			break;
		case Op::ConstFloatOp:
			if (!isFloat(operation->getStamp()) ||
			    operation->dynCast<ir::ConstFloatOperation>()->getConstantOrigin() != ConstantOrigin::CacheInvariant) {
				return reject(operation, location, "uncertified_scalar");
			}
			arity = 0;
			break;
		case Op::ConstPtrOp:
			if (operation->dynCast<ir::ConstPtrOperation>()->getValue() != nullptr) {
				return reject(operation, location, "embedded_non_null_pointer");
			}
			arity = 0;
			break;
		case Op::RuntimeBindingOp: {
			const auto& binding = operation->dynCast<ir::RuntimeBindingOperation>()->getBinding();
			if (operation->getStamp() != Type::ptr || binding.identity.empty() || binding.type.empty() ||
			    binding.symbol != runtime_binding::symbolName(binding.identity) || binding.address == nullptr) {
				return reject(operation, location, "invalid_runtime_binding");
			}
			arity = 0;
			break;
		}
		case Op::BasicBlockArgument:
			if (!arguments.contains(operation)) {
				return reject(operation, location, "block_argument_outside_graph");
			}
			if (arguments.at(operation) != location.function) {
				return reject(operation, location, "block_argument_outside_function");
			}
			arity = 0;
			break;
		case Op::FunctionOp: {
			const auto* function = operation->dynCast<ir::FunctionOperation>();
			if (!functions.contains(function)) {
				return reject(operation, location, "function_outside_graph");
			}
			const auto cause = functionBindingRejection(graph, function);
			if (!cause.empty()) {
				return reject(operation, location, cause);
			}
			const auto& specs = function->getAllocaSpecs();
			for (std::size_t index = 0; index < specs.size(); ++index) {
				const auto& spec = specs[index];
				if (spec.size == 0 || spec.size > static_cast<std::size_t>(std::numeric_limits<int64_t>::max()) ||
				    !std::has_single_bit(spec.align) || spec.align > std::numeric_limits<uint32_t>::max()) {
					return reject(operation, location, "invalid_alloca_spec=" + std::to_string(index));
				}
			}
			arity = 0;
			break;
		}
		case Op::FunctionAddressOfOp: {
			const auto cause = targetRejection(operation->dynCast<ir::FunctionAddressOfOperation>()->getCalleeId());
			if (!cause.empty()) {
				return reject(operation, location, cause);
			}
			arity = 0;
			break;
		}
		case Op::CallOp: {
			const auto* call = operation->dynCast<ir::CallOperation>();
			const auto cause = targetRejection(call->getCalleeId());
			if (!cause.empty()) {
				return reject(operation, location, cause);
			}
			const auto& target = table.get(call->getCalleeId());
			const auto parameters = target.getParamTypes();
			if (parameters.size() != inputs.size() || target.getResultType() != operation->getStamp()) {
				return reject(operation, location, "callee_signature_mismatch");
			}
			for (std::size_t argument = 0; argument < inputs.size(); ++argument) {
				if (parameters[argument] != inputs[argument]->getStamp()) {
					return reject(operation, location, "callee_argument_type_mismatch=" + std::to_string(argument));
				}
			}
			if (!includeDestructors(*call)) {
				return false;
			}
			break;
		}
		case Op::IndirectCallOp:
			if (inputs.empty() || inputs.front()->getStamp() != Type::ptr) {
				return reject(operation, location, "invalid_indirect_callee_operand");
			}
			if (!includeDestructors(*operation->dynCast<ir::IndirectCallOperation>())) {
				return false;
			}
			break;
		case Op::BranchOp:
			if (!include(&operation->dynCast<ir::BranchOperation>()->getNextBlockInvocation(), location)) {
				return false;
			}
			arity = 0;
			break;
		case Op::IfOp: {
			const auto* branch = operation->dynCast<ir::IfOperation>();
			if (!include(&branch->getTrueBlockInvocation(), location) ||
			    !include(&branch->getFalseBlockInvocation(), location)) {
				return false;
			}
			arity = 1;
			break;
		}
		case Op::BlockInvocation: {
			const auto* invocation = operation->dynCast<ir::BasicBlockInvocation>();
			const auto owner = blocks.find(invocation->getBlock());
			if (owner == blocks.end() || owner->second != location.function) {
				return reject(operation, location, "branch_target_outside_function");
			}
			const auto& parameters = invocation->getBlock()->getArguments();
			if (parameters.size() != inputs.size()) {
				return reject(operation, location, "branch_argument_count_mismatch");
			}
			for (std::size_t argument = 0; argument < inputs.size(); ++argument) {
				if (parameters[argument]->getStamp() != inputs[argument]->getStamp()) {
					return reject(operation, location, "branch_argument_type_mismatch=" + std::to_string(argument));
				}
			}
			break;
		}
		case Op::AllocaOp: {
			const auto index = operation->dynCast<ir::AllocaOperation>()->getIndex();
			if (operation->getStamp() != Type::ptr || index >= location.function->getAllocaSpecs().size()) {
				return reject(operation, location, "alloca_index_outside_function=" + std::to_string(index));
			}
			arity = 0;
			break;
		}
		case Op::CastOp:
		case Op::LoadOp:
		case Op::NotOp:
		case Op::NegateOp:
			arity = 1;
			break;
		case Op::AddOp:
		case Op::SubOp:
		case Op::MulOp:
		case Op::DivOp:
		case Op::ModOp:
		case Op::BinaryComp:
		case Op::ShiftOp:
		case Op::AndOp:
		case Op::OrOp:
		case Op::CompareOp:
		case Op::StoreOp:
			arity = 2;
			break;
		case Op::SelectOp:
			arity = 3;
			break;
		case Op::ReturnOp: {
			if (inputs.size() > 1) {
				return reject(operation, location, "invalid_return_arity");
			}
			const auto result = location.function->getOutputArg();
			if (operation->getStamp() != result || inputs.size() != (result == Type::v ? 0U : 1U) ||
			    (!inputs.empty() && inputs.front()->getStamp() != result)) {
				return reject(operation, location, "return_signature_mismatch");
			}
			break;
		}
		default:
			return reject(operation, location, "unsupported_operation");
		}
		if (arity && inputs.size() != *arity) {
			return reject(operation, location, "invalid_operand_count");
		}
	}
	for (std::size_t index = 0; index < table.size(); ++index) {
		const auto cause = targetRejection(static_cast<ir::FunctionId>(index));
		if (!cause.empty()) {
			return reject(nullptr, {}, cause);
		}
	}
	return true;
}

void validateArtifactRoots(const ir::IRGraph& graph, const std::list<CompilableFunction>& functions) {
	if (functions.empty()) {
		throw RuntimeException("Artifact exports are empty");
	}
	std::unordered_set<std::string> names;
	for (const auto& function : functions) {
		if (function.getName().empty() || !names.insert(function.getName()).second) {
			throw RuntimeException("Invalid artifact export name '" + function.getName() + "'");
		}
		if (!function.getSignature()) {
			throw RuntimeException("Artifact export has no declared signature: '" + function.getName() + "'");
		}
		const auto* traced = graph.getFunctionOperation(function.getName());
		const auto& signature = *function.getSignature();
		std::vector<Type> argumentTypes;
		if (traced && traced->getEntryBlock()) {
			for (const auto* argument : traced->getEntryBlock()->getArguments()) {
				if (argument == nullptr) {
					throw RuntimeException("Null artifact export argument: '" + function.getName() + "'");
				}
				argumentTypes.push_back(argument->getStamp());
			}
		}
		if (!traced || !traced->getEntryBlock() || argumentTypes != signature.argumentTypes ||
		    traced->getOutputArg() != signature.returnType) {
			throw RuntimeException("Traced root signature does not match artifact export '" + function.getName() + "'");
		}
		const auto cause = functionBindingRejection(graph, traced);
		if (!cause.empty()) {
			throw RuntimeException("Artifact export has invalid function binding: '" + function.getName() +
			                       "' cause=" + cause);
		}
		const auto& emissionName = graph.getEmissionName(traced);
		if (emissionName != function.getName()) {
			throw RuntimeException("Artifact export name does not match traced emission name '" + function.getName() +
			                       "' (emitted as '" + emissionName + "')");
		}
	}
}

bool hasOnlyTypedAllocations(const ir::IRGraph& graph, std::string* reason) {
	if (reason) {
		reason->clear();
	}
	for (const auto* function : graph.getFunctionOperations()) {
		if (!function) {
			if (reason) {
				*reason = "allocation_metadata_missing_function";
			}
			return false;
		}
		const auto& specs = function->getAllocaSpecs();
		for (std::size_t index = 0; index < specs.size(); ++index) {
			const auto& spec = specs[index];
			std::string cause;
			if (!graph.hasRecordedConstantOrigins() || !spec.origin) {
				cause = "allocation_metadata_origins_unavailable";
			} else if (spec.size == 0 || spec.size > static_cast<std::size_t>(std::numeric_limits<int64_t>::max()) ||
			           !std::has_single_bit(spec.align) || spec.align > std::numeric_limits<uint32_t>::max() ||
			           spec.origin->getSize() != spec.size || spec.origin->getAlignment() != spec.align ||
			           spec.origin->getType().empty()) {
				cause = "allocation_metadata_layout_mismatch";
			}
			if (!cause.empty()) {
				if (reason) {
					*reason = cause + " function=" + function->getName() + " index=" + std::to_string(index);
				}
				return false;
			}
		}
	}
	return true;
}

void validateArtifactPreflight(const ir::IRGraph& graph, const std::list<CompilableFunction>& functions) {
	validateArtifactRoots(graph, functions);
	std::string reason;
	if (!hasOnlyTypedAllocations(graph, &reason) || !hasOnlyInvariantScalars(graph, &reason)) {
		throw RuntimeException("Artifact preflight failed: " + reason);
	}
}

} // namespace nautilus::compiler::artifact

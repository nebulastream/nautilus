#include "CApiInternal.hpp"
#include "nautilus/Executable.hpp"
#include "nautilus/c/ir.h"
#include "nautilus/compiler/CompilationPipeline.hpp"
#include "nautilus/compiler/DumpHandler.hpp"
#include "nautilus/compiler/backends/CompilationBackend.hpp"
#include "nautilus/compiler/ir/IRGraph.hpp"
#include "nautilus/compiler/ir/blocks/BasicBlock.hpp"
#include "nautilus/compiler/ir/blocks/BasicBlockArgument.hpp"
#include "nautilus/compiler/ir/blocks/BasicBlockInvocation.hpp"
#include "nautilus/compiler/ir/operations/AllocaOperation.hpp"
#include "nautilus/compiler/ir/operations/ArithmeticOperations/AddOperation.hpp"
#include "nautilus/compiler/ir/operations/ArithmeticOperations/DivOperation.hpp"
#include "nautilus/compiler/ir/operations/ArithmeticOperations/ModOperation.hpp"
#include "nautilus/compiler/ir/operations/ArithmeticOperations/MulOperation.hpp"
#include "nautilus/compiler/ir/operations/ArithmeticOperations/SubOperation.hpp"
#include "nautilus/compiler/ir/operations/BinaryOperations/BinaryCompOperation.hpp"
#include "nautilus/compiler/ir/operations/BinaryOperations/NegateOperation.hpp"
#include "nautilus/compiler/ir/operations/BinaryOperations/ShiftOperation.hpp"
#include "nautilus/compiler/ir/operations/BranchOperation.hpp"
#include "nautilus/compiler/ir/operations/CallOperation.hpp"
#include "nautilus/compiler/ir/operations/CastOperation.hpp"
#include "nautilus/compiler/ir/operations/ConstBooleanOperation.hpp"
#include "nautilus/compiler/ir/operations/ConstFloatOperation.hpp"
#include "nautilus/compiler/ir/operations/ConstIntOperation.hpp"
#include "nautilus/compiler/ir/operations/ConstPtrOperation.hpp"
#include "nautilus/compiler/ir/operations/FunctionAddressOfOperation.hpp"
#include "nautilus/compiler/ir/operations/FunctionOperation.hpp"
#include "nautilus/compiler/ir/operations/IfOperation.hpp"
#include "nautilus/compiler/ir/operations/IndirectCallOperation.hpp"
#include "nautilus/compiler/ir/operations/LoadOperation.hpp"
#include "nautilus/compiler/ir/operations/LogicalOperations/AndOperation.hpp"
#include "nautilus/compiler/ir/operations/LogicalOperations/CompareOperation.hpp"
#include "nautilus/compiler/ir/operations/LogicalOperations/NotOperation.hpp"
#include "nautilus/compiler/ir/operations/LogicalOperations/OrOperation.hpp"
#include "nautilus/compiler/ir/operations/OperationProperties.hpp"
#include "nautilus/compiler/ir/operations/ReturnOperation.hpp"
#include "nautilus/compiler/ir/operations/SelectOperation.hpp"
#include "nautilus/compiler/ir/operations/StoreOperation.hpp"
#include "nautilus/compiler/ir/passes/IRVerifier.hpp"
#include "nautilus/compiler/ir/util/ControlFlowUtil.hpp"
#include "nautilus/options.hpp"
#include <cstdlib>
#include <cstring>
#include <exception>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace ir = nautilus::compiler::ir;
using nautilus::Type;
using namespace nautilus::capi;

namespace {

static_assert(static_cast<int>(Type::ptr) == NAUTILUS_IR_TYPE_PTR, "NautilusIRType out of sync with nautilus::Type");
static_assert(static_cast<int>(ir::Operation::OperationType::FunctionAddressOfOp) == NAUTILUS_IR_OP_FUNCTION_ADDRESS_OF,
              "NautilusIROpKind out of sync with Operation::OperationType");
static_assert(static_cast<int>(ir::CompareOperation::GE) == NAUTILUS_IR_CMP_GE,
              "NautilusIRComparator out of sync with CompareOperation::Comparator");
static_assert(static_cast<int>(nautilus::ModRefInfo::ModRef) == NAUTILUS_IR_MOD_REF_MOD_REF,
              "NautilusIRModRef out of sync with ModRefInfo");

Type toType(NautilusIRType type) {
	require(type >= NAUTILUS_IR_TYPE_VOID && type <= NAUTILUS_IR_TYPE_PTR, "invalid NautilusIRType");
	return static_cast<Type>(type);
}

NautilusIRType fromType(Type type) {
	return static_cast<NautilusIRType>(type);
}

bool isIntegerType(Type type) {
	switch (type) {
	case Type::i8:
	case Type::i16:
	case Type::i32:
	case Type::i64:
	case Type::ui8:
	case Type::ui16:
	case Type::ui32:
	case Type::ui64:
		return true;
	default:
		return false;
	}
}

bool isFloatType(Type type) {
	return type == Type::f32 || type == Type::f64;
}

ir::Operation* unwrap(NautilusIRValueRef value) {
	return reinterpret_cast<ir::Operation*>(value);
}

NautilusIRValueRef wrap(const ir::Operation* op) {
	return reinterpret_cast<NautilusIRValueRef>(const_cast<ir::Operation*>(op));
}

ir::BasicBlock* unwrap(NautilusIRBlockRef block) {
	return reinterpret_cast<ir::BasicBlock*>(block);
}

NautilusIRBlockRef wrap(const ir::BasicBlock* block) {
	return reinterpret_cast<NautilusIRBlockRef>(const_cast<ir::BasicBlock*>(block));
}

ir::FunctionOperation* unwrap(NautilusIRFunctionRef function) {
	return reinterpret_cast<ir::FunctionOperation*>(function);
}

NautilusIRFunctionRef wrap(const ir::FunctionOperation* function) {
	return reinterpret_cast<NautilusIRFunctionRef>(const_cast<ir::FunctionOperation*>(function));
}

nautilus::FunctionAttributes toAttributes(const NautilusIRFunctionAttributes& attributes) {
	require(attributes.mod_ref >= NAUTILUS_IR_MOD_REF_NONE && attributes.mod_ref <= NAUTILUS_IR_MOD_REF_MOD_REF,
	        "invalid NautilusIRModRef");
	return nautilus::FunctionAttributes {.modRefInfo = static_cast<nautilus::ModRefInfo>(attributes.mod_ref),
	                                     .willReturn = attributes.will_return != 0,
	                                     .noUnwind = attributes.no_unwind != 0};
}

char* copyString(const std::string& str) {
	auto* result = static_cast<char*>(std::malloc(str.size() + 1));
	if (result == nullptr) {
		throw std::bad_alloc();
	}
	std::memcpy(result, str.c_str(), str.size() + 1);
	return result;
}

ir::OperationIdentifier nextId(NautilusIROpaqueFunctionBuilder* builder) {
	return ir::OperationIdentifier {builder->graph->nextOperationId++};
}

/// Validates that @p block may be appended to through @p builder, and returns it.
ir::BasicBlock* openBlock(NautilusIROpaqueFunctionBuilder* builder, NautilusIRBlockRef blockRef) {
	require(builder != nullptr, "builder is NULL");
	require(blockRef != nullptr, "block is NULL");
	auto* block = unwrap(blockRef);
	require(builder->ownedBlocks.contains(block), "block does not belong to this function builder");
	const auto& operations = block->getOperations();
	require(operations.empty() || !ir::isTerminatorOp(operations.back()->getOperationType()),
	        "block already ends in a terminator");
	return block;
}

ir::Operation* operand(NautilusIRValueRef value, const char* message) {
	require(value != nullptr, message);
	auto* op = unwrap(value);
	require(op->getStamp() != Type::v, "operand does not produce a value");
	return op;
}

std::vector<ir::Operation*> operands(const NautilusIRValueRef* values, size_t count) {
	require(count == 0 || values != nullptr, "argument array is NULL");
	std::vector<ir::Operation*> result;
	result.reserve(count);
	for (size_t i = 0; i < count; ++i) {
		result.push_back(operand(values[i], "argument is NULL"));
	}
	return result;
}

/// Checks that @p args match the parameters of @p target, as a branch to it must.
void checkInvocation(NautilusIROpaqueFunctionBuilder* builder, NautilusIRBlockRef targetRef,
                     const std::vector<ir::Operation*>& args) {
	require(targetRef != nullptr, "target block is NULL");
	auto* target = unwrap(targetRef);
	require(builder->ownedBlocks.contains(target), "target block does not belong to this function builder");
	require(target != builder->blocks.front(), "the entry block cannot be branched to");
	const auto& params = target->getArguments();
	require(params.size() == args.size(), "argument count does not match the target block's arguments");
	for (size_t i = 0; i < args.size(); ++i) {
		require(params[i]->getStamp() == args[i]->getStamp(),
		        "argument type does not match the target block's argument type");
	}
}

/// Signature of a callee: read off the table, or off the builder of an
/// internal function that has not been finished yet.
struct Signature {
	Type result;
	std::vector<Type> params;
};

Signature signatureOf(NautilusIROpaqueGraph* graph, ir::FunctionId callee) {
	const auto& table = graph->ir->getFunctionTable();
	require(table.contains(callee), "unknown callee id");
	if (auto it = graph->pendingByCallee.find(callee); it != graph->pendingByCallee.end()) {
		const auto* pending = it->second;
		require(!pending->blocks.empty(),
		        "callee has no entry block yet; add it before building calls to the function");
		Signature signature {pending->returnType, {}};
		for (const auto* arg : pending->blocks.front()->getArguments()) {
			signature.params.push_back(arg->getStamp());
		}
		return signature;
	}
	const auto& target = table.get(callee);
	if (target.getLinkage() == ir::Linkage::Internal) {
		require(target.getDefinition() != nullptr, "callee has no definition");
		Signature signature {target.getResultType(), {}};
		for (const auto* arg : target.getDefinition()->getEntryBlock()->getArguments()) {
			signature.params.push_back(arg->getStamp());
		}
		return signature;
	}
	return Signature {target.getResultType(), target.getParamTypes()};
}

nautilus::compiler::IROptimizationLevel toLevel(NautilusIROptimizationLevel level,
                                                const nautilus::compiler::CompilationBackend* backend) {
	switch (level) {
	case NAUTILUS_IR_OPTIMIZE_NONE:
		return nautilus::compiler::IROptimizationLevel::None;
	case NAUTILUS_IR_OPTIMIZE_ARGUMENT_PRUNING:
		return nautilus::compiler::IROptimizationLevel::ArgumentPruning;
	case NAUTILUS_IR_OPTIMIZE_FULL:
		return nautilus::compiler::IROptimizationLevel::Full;
	case NAUTILUS_IR_OPTIMIZE_BACKEND_DEFAULT:
		return backend != nullptr ? backend->irOptimizationLevel() : nautilus::compiler::IROptimizationLevel::Full;
	}
	throw ApiError("invalid NautilusIROptimizationLevel");
}

void optimize(NautilusIROpaqueGraph* graph, nautilus::compiler::IROptimizationLevel level,
              const nautilus::engine::ModuleOptions& options) {
	prepareForPasses(graph);
	if (graph->optimized) {
		return;
	}
	nautilus::compiler::CompilationPipeline::runIRPasses(*graph->ir, options, level);
	graph->optimized = true;
}

template <typename T>
int detail(NautilusIRValueRef value, auto&& read) {
	return guarded(1, [&] {
		require(value != nullptr, "value is NULL");
		const auto* op = ir::dyn_cast<T>(unwrap(value));
		if (op == nullptr) {
			setError("value has a different operation kind");
			return 1;
		}
		read(op);
		return 0;
	});
}

} // namespace

extern "C" {

/* ── Errors and strings ─────────────────────────────────────────────────── */

const char* nautilus_ir_get_last_error(void) {
	return hasLastError ? lastError.c_str() : nullptr;
}

void nautilus_ir_string_dispose(char* str) {
	std::free(str);
}

const char* nautilus_ir_type_name(NautilusIRType type) {
	return guarded<const char*>(nullptr, [&] { return nautilus::toString(toType(type)); });
}

NautilusIRFunctionAttributes nautilus_ir_function_attributes_default(void) {
	const nautilus::FunctionAttributes defaults;
	return NautilusIRFunctionAttributes {.mod_ref = static_cast<NautilusIRModRef>(defaults.modRefInfo),
	                                     .will_return = defaults.willReturn ? 1 : 0,
	                                     .no_unwind = defaults.noUnwind ? 1 : 0};
}

/* ── Graphs ─────────────────────────────────────────────────────────────── */

NautilusIRGraphRef nautilus_ir_graph_create(const char* id) {
	return guarded<NautilusIRGraphRef>(nullptr, [&] {
		auto graph = std::make_unique<NautilusIROpaqueGraph>();
		graph->ir = std::make_shared<ir::IRGraph>(id != nullptr ? id : "c-api");
		return graph.release();
	});
}

void nautilus_ir_graph_dispose(NautilusIRGraphRef graph) {
	delete graph;
}

char* nautilus_ir_graph_to_string(NautilusIRGraphRef graph) {
	return guarded<char*>(nullptr, [&] {
		require(graph != nullptr, "graph is NULL");
		return copyString(graph->ir->toString());
	});
}

int nautilus_ir_graph_verify(NautilusIRGraphRef graph, char** error_message) {
	if (error_message != nullptr) {
		*error_message = nullptr;
	}
	return guarded(1, [&] {
		require(graph != nullptr, "graph is NULL");
		ir::rebuildPredecessorLists(*graph->ir);
		const auto result = ir::IRVerifier::verify(*graph->ir);
		if (result.ok()) {
			return 0;
		}
		if (error_message != nullptr) {
			*error_message = copyString(result.toString());
		}
		return 1;
	});
}

size_t nautilus_ir_graph_get_function_count(NautilusIRGraphRef graph) {
	return graph != nullptr ? graph->ir->getFunctionOperations().size() : 0;
}

NautilusIRFunctionRef nautilus_ir_graph_get_function(NautilusIRGraphRef graph, size_t index) {
	return guarded<NautilusIRFunctionRef>(nullptr, [&] {
		require(graph != nullptr, "graph is NULL");
		return wrap(graph->ir->getFunctionOperations().at(index));
	});
}

NautilusIRFunctionRef nautilus_ir_graph_get_function_by_name(NautilusIRGraphRef graph, const char* name) {
	return guarded<NautilusIRFunctionRef>(nullptr, [&] {
		require(graph != nullptr, "graph is NULL");
		require(name != nullptr, "name is NULL");
		return wrap(graph->ir->getFunctionOperation(name));
	});
}

NautilusIRCalleeId nautilus_ir_graph_declare_external_function(NautilusIRGraphRef graph, const char* symbol,
                                                               const char* display_name, void* address,
                                                               NautilusIRType result_type,
                                                               const NautilusIRType* param_types, size_t param_count,
                                                               NautilusIRFunctionAttributes attributes) {
	return guarded<NautilusIRCalleeId>(NAUTILUS_IR_INVALID_CALLEE, [&] {
		require(graph != nullptr, "graph is NULL");
		require(address != nullptr, "address is NULL");
		require(param_count == 0 || param_types != nullptr, "param_types is NULL");
		ir::CalleeDescriptor descriptor;
		descriptor.kind = ir::CalleeDescriptor::Kind::External;
		descriptor.key = address;
		descriptor.mangledName = symbol != nullptr ? symbol : "";
		descriptor.demangledName = display_name != nullptr ? display_name : descriptor.mangledName;
		descriptor.resultType = toType(result_type);
		for (size_t i = 0; i < param_count; ++i) {
			const auto type = toType(param_types[i]);
			require(type != Type::v, "a parameter cannot have type void");
			descriptor.paramTypes.push_back(type);
		}
		descriptor.attrs = toAttributes(attributes);
		return graph->ir->internCallee(descriptor);
	});
}

size_t nautilus_ir_graph_get_callee_count(NautilusIRGraphRef graph) {
	return graph != nullptr ? graph->ir->getFunctionTable().size() : 0;
}

NautilusIRLinkage nautilus_ir_graph_get_callee_linkage(NautilusIRGraphRef graph, NautilusIRCalleeId callee) {
	return guarded(NAUTILUS_IR_LINKAGE_INTERNAL, [&] {
		require(graph != nullptr, "graph is NULL");
		require(graph->ir->getFunctionTable().contains(callee), "unknown callee id");
		return static_cast<NautilusIRLinkage>(graph->ir->getFunctionTarget(callee).getLinkage());
	});
}

const char* nautilus_ir_graph_get_callee_name(NautilusIRGraphRef graph, NautilusIRCalleeId callee) {
	return guarded<const char*>(nullptr, [&] {
		require(graph != nullptr, "graph is NULL");
		require(graph->ir->getFunctionTable().contains(callee), "unknown callee id");
		return graph->ir->getFunctionTarget(callee).getName().forEmission().c_str();
	});
}

void* nautilus_ir_graph_get_callee_address(NautilusIRGraphRef graph, NautilusIRCalleeId callee) {
	return guarded<void*>(nullptr, [&]() -> void* {
		require(graph != nullptr, "graph is NULL");
		require(graph->ir->getFunctionTable().contains(callee), "unknown callee id");
		const auto* native = graph->ir->getFunctionTarget(callee).getNative();
		return native != nullptr ? native->address : nullptr;
	});
}

NautilusIRFunctionRef nautilus_ir_graph_get_callee_function(NautilusIRGraphRef graph, NautilusIRCalleeId callee) {
	return guarded<NautilusIRFunctionRef>(nullptr, [&] {
		require(graph != nullptr, "graph is NULL");
		require(graph->ir->getFunctionTable().contains(callee), "unknown callee id");
		return wrap(graph->ir->getFunctionTarget(callee).getDefinition());
	});
}

/* ── Function builders ──────────────────────────────────────────────────── */

NautilusIRFunctionBuilderRef nautilus_ir_function_builder_create(NautilusIRGraphRef graph, const char* name,
                                                                 NautilusIRType return_type) {
	return guarded<NautilusIRFunctionBuilderRef>(nullptr, [&] {
		require(graph != nullptr, "graph is NULL");
		require(name != nullptr && *name != '\0', "function name is empty");
		require(!graph->optimized, "graph has already been optimized or compiled");
		require(graph->ir->getFunctionOperation(name) == nullptr, "a function with this name already exists");
		for (const auto& [_, pending] : graph->builders) {
			require(pending->name != name, "a function with this name already exists");
		}
		auto builder = std::make_unique<NautilusIROpaqueFunctionBuilder>();
		builder->graph = graph;
		builder->name = name;
		builder->returnType = toType(return_type);
		// A null identity key mints a fresh entry, as the trace path does for
		// its entry function; finish() binds the definition to it.
		ir::CalleeDescriptor descriptor;
		descriptor.kind = ir::CalleeDescriptor::Kind::Internal;
		descriptor.customName = name;
		builder->calleeId = graph->ir->internCallee(descriptor);
		auto* raw = builder.get();
		graph->pendingByCallee[raw->calleeId] = raw;
		graph->builders.emplace(raw, std::move(builder));
		return raw;
	});
}

NautilusIRCalleeId nautilus_ir_function_builder_get_callee(NautilusIRFunctionBuilderRef builder) {
	return builder != nullptr ? builder->calleeId : NAUTILUS_IR_INVALID_CALLEE;
}

int nautilus_ir_function_builder_set_attribute(NautilusIRFunctionBuilderRef builder, const char* key,
                                               const char* value) {
	return guarded(1, [&] {
		require(builder != nullptr, "builder is NULL");
		require(key != nullptr && value != nullptr, "attribute key or value is NULL");
		builder->attributes[key] = value;
		return 0;
	});
}

NautilusIRBlockRef nautilus_ir_function_builder_add_block(NautilusIRFunctionBuilderRef builder,
                                                          const NautilusIRType* arg_types, size_t arg_count) {
	return guarded<NautilusIRBlockRef>(nullptr, [&] {
		require(builder != nullptr, "builder is NULL");
		require(arg_count == 0 || arg_types != nullptr, "arg_types is NULL");
		auto& arena = builder->graph->ir->getArena();
		std::vector<ir::BasicBlockArgument*> arguments;
		arguments.reserve(arg_count);
		for (size_t i = 0; i < arg_count; ++i) {
			const auto type = toType(arg_types[i]);
			require(type != Type::v, "a block argument cannot have type void");
			arguments.push_back(arena.create<ir::BasicBlockArgument>(nextId(builder), type));
		}
		auto* block =
		    arena.create<ir::BasicBlock>(arena, ir::BlockIdentifier {builder->nextBlockId++}, std::move(arguments));
		builder->blocks.push_back(block);
		builder->ownedBlocks.insert(block);
		return wrap(block);
	});
}

uint32_t nautilus_ir_function_builder_add_stack_slot(NautilusIRFunctionBuilderRef builder, size_t size, size_t align) {
	return guarded<uint32_t>(UINT32_MAX, [&] {
		require(builder != nullptr, "builder is NULL");
		require(size > 0, "stack slot size must be positive");
		require(align > 0 && (align & (align - 1)) == 0, "stack slot alignment must be a power of two");
		builder->allocaSpecs.push_back(ir::AllocaSpec {size, align});
		return static_cast<uint32_t>(builder->allocaSpecs.size() - 1);
	});
}

NautilusIRFunctionRef nautilus_ir_function_builder_finish(NautilusIRFunctionBuilderRef builder) {
	if (builder == nullptr) {
		setError("builder is NULL");
		return nullptr;
	}
	auto* graph = builder->graph;
	auto it = graph->builders.find(builder);
	if (it == graph->builders.end()) {
		setError("builder has already been finished");
		return nullptr;
	}
	// Detach first so the builder is consumed whatever happens below.
	auto owned = std::move(it->second);
	graph->builders.erase(it);
	graph->pendingByCallee.erase(builder->calleeId);
	return guarded<NautilusIRFunctionRef>(nullptr, [&] {
		require(!owned->blocks.empty(), "function has no blocks");
		for (const auto* block : owned->blocks) {
			const auto& operations = block->getOperations();
			require(!operations.empty() && ir::isTerminatorOp(operations.back()->getOperationType()),
			        "every block must end in a terminator");
		}
		auto& arena = graph->ir->getArena();
		auto* function = arena.create<ir::FunctionOperation>(
		    owned->name, std::move(owned->blocks), std::vector<Type> {}, std::vector<std::string> {}, owned->returnType,
		    std::move(owned->allocaSpecs), std::move(owned->attributes));
		graph->ir->addFunctionOperation(function);
		graph->ir->defineFunction(owned->calleeId, function);
		return wrap(function);
	});
}

/* ── Instruction building ───────────────────────────────────────────────── */

NautilusIRValueRef nautilus_ir_build_const_int(NautilusIRFunctionBuilderRef builder, NautilusIRBlockRef block,
                                               int64_t value, NautilusIRType type) {
	return guarded<NautilusIRValueRef>(nullptr, [&] {
		auto* bb = openBlock(builder, block);
		const auto stamp = toType(type);
		require(isIntegerType(stamp), "integer constants need an integer type");
		return wrap(bb->addOperation<ir::ConstIntOperation>(nextId(builder), value, stamp));
	});
}

NautilusIRValueRef nautilus_ir_build_const_float(NautilusIRFunctionBuilderRef builder, NautilusIRBlockRef block,
                                                 double value, NautilusIRType type) {
	return guarded<NautilusIRValueRef>(nullptr, [&] {
		auto* bb = openBlock(builder, block);
		const auto stamp = toType(type);
		require(isFloatType(stamp), "float constants need a float type");
		return wrap(bb->addOperation<ir::ConstFloatOperation>(nextId(builder), value, stamp));
	});
}

NautilusIRValueRef nautilus_ir_build_const_bool(NautilusIRFunctionBuilderRef builder, NautilusIRBlockRef block,
                                                int value) {
	return guarded<NautilusIRValueRef>(nullptr, [&] {
		auto* bb = openBlock(builder, block);
		return wrap(bb->addOperation<ir::ConstBooleanOperation>(nextId(builder), value != 0));
	});
}

NautilusIRValueRef nautilus_ir_build_const_ptr(NautilusIRFunctionBuilderRef builder, NautilusIRBlockRef block,
                                               void* value) {
	return guarded<NautilusIRValueRef>(nullptr, [&] {
		auto* bb = openBlock(builder, block);
		return wrap(bb->addOperation<ir::ConstPtrOperation>(nextId(builder), value));
	});
}

NautilusIRValueRef nautilus_ir_build_binary(NautilusIRFunctionBuilderRef builder, NautilusIRBlockRef block,
                                            NautilusIRBinaryOp op, NautilusIRValueRef lhs, NautilusIRValueRef rhs) {
	return guarded<NautilusIRValueRef>(nullptr, [&]() -> NautilusIRValueRef {
		auto* bb = openBlock(builder, block);
		auto* left = operand(lhs, "lhs is NULL");
		auto* right = operand(rhs, "rhs is NULL");
		const auto id = nextId(builder);
		switch (op) {
		case NAUTILUS_IR_BINARY_ADD:
			return wrap(bb->addOperation<ir::AddOperation>(id, left, right));
		case NAUTILUS_IR_BINARY_SUB:
			return wrap(bb->addOperation<ir::SubOperation>(id, left, right));
		case NAUTILUS_IR_BINARY_MUL:
			return wrap(bb->addOperation<ir::MulOperation>(id, left, right));
		case NAUTILUS_IR_BINARY_DIV:
			return wrap(bb->addOperation<ir::DivOperation>(id, left, right));
		case NAUTILUS_IR_BINARY_MOD:
			return wrap(bb->addOperation<ir::ModOperation>(id, left, right));
		case NAUTILUS_IR_BINARY_LOGICAL_AND:
		case NAUTILUS_IR_BINARY_LOGICAL_OR:
			require(left->getStamp() == Type::b && right->getStamp() == Type::b, "logical and/or need bool operands");
			if (op == NAUTILUS_IR_BINARY_LOGICAL_AND) {
				return wrap(bb->addOperation<ir::AndOperation>(id, left, right));
			}
			return wrap(bb->addOperation<ir::OrOperation>(id, left, right));
		case NAUTILUS_IR_BINARY_BITWISE_AND:
			return wrap(bb->addOperation<ir::BinaryCompOperation>(id, left, right, ir::BinaryCompOperation::BAND));
		case NAUTILUS_IR_BINARY_BITWISE_OR:
			return wrap(bb->addOperation<ir::BinaryCompOperation>(id, left, right, ir::BinaryCompOperation::BOR));
		case NAUTILUS_IR_BINARY_BITWISE_XOR:
			return wrap(bb->addOperation<ir::BinaryCompOperation>(id, left, right, ir::BinaryCompOperation::XOR));
		case NAUTILUS_IR_BINARY_SHIFT_LEFT:
			return wrap(bb->addOperation<ir::ShiftOperation>(id, left, right, ir::ShiftOperation::LS));
		case NAUTILUS_IR_BINARY_SHIFT_RIGHT:
			return wrap(bb->addOperation<ir::ShiftOperation>(id, left, right, ir::ShiftOperation::RS));
		}
		throw ApiError("invalid NautilusIRBinaryOp");
	});
}

NautilusIRValueRef nautilus_ir_build_compare(NautilusIRFunctionBuilderRef builder, NautilusIRBlockRef block,
                                             NautilusIRComparator comparator, NautilusIRValueRef lhs,
                                             NautilusIRValueRef rhs) {
	return guarded<NautilusIRValueRef>(nullptr, [&] {
		auto* bb = openBlock(builder, block);
		require(comparator >= NAUTILUS_IR_CMP_EQ && comparator <= NAUTILUS_IR_CMP_GE, "invalid NautilusIRComparator");
		auto* left = operand(lhs, "lhs is NULL");
		auto* right = operand(rhs, "rhs is NULL");
		return wrap(bb->addOperation<ir::CompareOperation>(nextId(builder), left, right,
		                                                   static_cast<ir::CompareOperation::Comparator>(comparator)));
	});
}

NautilusIRValueRef nautilus_ir_build_not(NautilusIRFunctionBuilderRef builder, NautilusIRBlockRef block,
                                         NautilusIRValueRef value) {
	return guarded<NautilusIRValueRef>(nullptr, [&] {
		auto* bb = openBlock(builder, block);
		auto* input = operand(value, "value is NULL");
		require(input->getStamp() == Type::b, "not needs a bool operand");
		return wrap(bb->addOperation<ir::NotOperation>(nextId(builder), input));
	});
}

NautilusIRValueRef nautilus_ir_build_negate(NautilusIRFunctionBuilderRef builder, NautilusIRBlockRef block,
                                            NautilusIRValueRef value) {
	return guarded<NautilusIRValueRef>(nullptr, [&] {
		auto* bb = openBlock(builder, block);
		auto* input = operand(value, "value is NULL");
		require(isIntegerType(input->getStamp()), "negate needs an integer operand");
		return wrap(bb->addOperation<ir::NegateOperation>(nextId(builder), input));
	});
}

NautilusIRValueRef nautilus_ir_build_cast(NautilusIRFunctionBuilderRef builder, NautilusIRBlockRef block,
                                          NautilusIRValueRef value, NautilusIRType target_type) {
	return guarded<NautilusIRValueRef>(nullptr, [&] {
		auto* bb = openBlock(builder, block);
		auto* input = operand(value, "value is NULL");
		const auto target = toType(target_type);
		require(target != Type::v, "cannot cast to void");
		return wrap(bb->addOperation<ir::CastOperation>(nextId(builder), input, target));
	});
}

NautilusIRValueRef nautilus_ir_build_select(NautilusIRFunctionBuilderRef builder, NautilusIRBlockRef block,
                                            NautilusIRValueRef condition, NautilusIRValueRef true_value,
                                            NautilusIRValueRef false_value) {
	return guarded<NautilusIRValueRef>(nullptr, [&] {
		auto* bb = openBlock(builder, block);
		auto* cond = operand(condition, "condition is NULL");
		auto* onTrue = operand(true_value, "true_value is NULL");
		auto* onFalse = operand(false_value, "false_value is NULL");
		require(cond->getStamp() == Type::b, "select needs a bool condition");
		require(onTrue->getStamp() == onFalse->getStamp(), "select needs operands of the same type");
		return wrap(bb->addOperation<ir::SelectOperation>(nextId(builder), cond, onTrue, onFalse, onTrue->getStamp()));
	});
}

NautilusIRValueRef nautilus_ir_build_load(NautilusIRFunctionBuilderRef builder, NautilusIRBlockRef block,
                                          NautilusIRValueRef address, NautilusIRType type) {
	return guarded<NautilusIRValueRef>(nullptr, [&] {
		auto* bb = openBlock(builder, block);
		auto* addr = operand(address, "address is NULL");
		require(addr->getStamp() == Type::ptr, "load needs a pointer address");
		const auto stamp = toType(type);
		require(stamp != Type::v, "cannot load void");
		return wrap(bb->addOperation<ir::LoadOperation>(nextId(builder), addr, stamp));
	});
}

NautilusIRValueRef nautilus_ir_build_store(NautilusIRFunctionBuilderRef builder, NautilusIRBlockRef block,
                                           NautilusIRValueRef value, NautilusIRValueRef address) {
	return guarded<NautilusIRValueRef>(nullptr, [&] {
		auto* bb = openBlock(builder, block);
		auto* stored = operand(value, "value is NULL");
		auto* addr = operand(address, "address is NULL");
		require(addr->getStamp() == Type::ptr, "store needs a pointer address");
		return wrap(bb->addOperation<ir::StoreOperation>(stored, addr));
	});
}

NautilusIRValueRef nautilus_ir_build_alloca(NautilusIRFunctionBuilderRef builder, NautilusIRBlockRef block,
                                            uint32_t slot) {
	return guarded<NautilusIRValueRef>(nullptr, [&] {
		auto* bb = openBlock(builder, block);
		require(slot < builder->allocaSpecs.size(), "unknown stack slot");
		return wrap(bb->addOperation<ir::AllocaOperation>(nextId(builder), slot));
	});
}

NautilusIRValueRef nautilus_ir_build_call(NautilusIRFunctionBuilderRef builder, NautilusIRBlockRef block,
                                          NautilusIRCalleeId callee, const NautilusIRValueRef* args, size_t arg_count) {
	return guarded<NautilusIRValueRef>(nullptr, [&] {
		auto* bb = openBlock(builder, block);
		auto* graph = builder->graph;
		const auto signature = signatureOf(graph, callee);
		const auto inputs = operands(args, arg_count);
		require(inputs.size() == signature.params.size(), "argument count does not match the callee's parameters");
		for (size_t i = 0; i < inputs.size(); ++i) {
			require(inputs[i]->getStamp() == signature.params[i],
			        "argument type does not match the callee's parameter type");
		}
		const auto& target = graph->ir->getFunctionTarget(callee);
		const auto& name = target.getName().forEmission();
		const bool internal = target.getLinkage() == ir::Linkage::Internal;
		const auto attributes = internal ? nautilus::FunctionAttributes {} : target.getNative()->attrs;
		const auto& symbol = internal ? name : target.getName().getMangled();
		return wrap(bb->addOperation<ir::CallOperation>(
		    symbol, name, internal ? nullptr : target.getAddress(), nextId(builder), inputs, signature.result,
		    attributes, callee, std::vector<ir::CallOperation::Destructor> {}, false, nullptr, internal));
	});
}

NautilusIRValueRef nautilus_ir_build_indirect_call(NautilusIRFunctionBuilderRef builder, NautilusIRBlockRef block,
                                                   NautilusIRValueRef function_pointer, const NautilusIRValueRef* args,
                                                   size_t arg_count, NautilusIRType result_type,
                                                   NautilusIRFunctionAttributes attributes) {
	return guarded<NautilusIRValueRef>(nullptr, [&] {
		auto* bb = openBlock(builder, block);
		auto* fnPtr = operand(function_pointer, "function_pointer is NULL");
		require(fnPtr->getStamp() == Type::ptr, "indirect call needs a pointer callee");
		const auto inputs = operands(args, arg_count);
		return wrap(bb->addOperation<ir::IndirectCallOperation>(nextId(builder), fnPtr, inputs, toType(result_type),
		                                                        toAttributes(attributes)));
	});
}

NautilusIRValueRef nautilus_ir_build_function_address(NautilusIRFunctionBuilderRef builder, NautilusIRBlockRef block,
                                                      NautilusIRCalleeId callee) {
	return guarded<NautilusIRValueRef>(nullptr, [&] {
		auto* bb = openBlock(builder, block);
		const auto& table = builder->graph->ir->getFunctionTable();
		require(table.contains(callee), "unknown callee id");
		const auto& target = table.get(callee);
		const auto& name = target.getName().forEmission();
		const bool internal = target.getLinkage() == ir::Linkage::Internal;
		return wrap(bb->addOperation<ir::FunctionAddressOfOperation>(internal ? name : target.getName().getMangled(),
		                                                             name, internal ? nullptr : target.getAddress(),
		                                                             nextId(builder), callee));
	});
}

NautilusIRValueRef nautilus_ir_build_branch(NautilusIRFunctionBuilderRef builder, NautilusIRBlockRef block,
                                            NautilusIRBlockRef target, const NautilusIRValueRef* args,
                                            size_t arg_count) {
	return guarded<NautilusIRValueRef>(nullptr, [&] {
		auto* bb = openBlock(builder, block);
		const auto inputs = operands(args, arg_count);
		checkInvocation(builder, target, inputs);
		return wrap(bb->addNextBlock(unwrap(target), inputs));
	});
}

NautilusIRValueRef nautilus_ir_build_if(NautilusIRFunctionBuilderRef builder, NautilusIRBlockRef block,
                                        NautilusIRValueRef condition, NautilusIRBlockRef true_block,
                                        const NautilusIRValueRef* true_args, size_t true_arg_count,
                                        NautilusIRBlockRef false_block, const NautilusIRValueRef* false_args,
                                        size_t false_arg_count, double probability) {
	return guarded<NautilusIRValueRef>(nullptr, [&] {
		auto* bb = openBlock(builder, block);
		auto* cond = operand(condition, "condition is NULL");
		require(cond->getStamp() == Type::b, "if needs a bool condition");
		require(probability >= 0.0 && probability <= 1.0, "probability must be within [0, 1]");
		const auto trueInputs = operands(true_args, true_arg_count);
		const auto falseInputs = operands(false_args, false_arg_count);
		checkInvocation(builder, true_block, trueInputs);
		checkInvocation(builder, false_block, falseInputs);
		auto& arena = builder->graph->ir->getArena();
		auto* ifOp = arena.create<ir::IfOperation>(arena, cond, probability);
		ifOp->setTrueBlockInvocation(unwrap(true_block));
		for (auto* input : trueInputs) {
			ifOp->getTrueBlockInvocation().addArgument(arena, input);
		}
		ifOp->setFalseBlockInvocation(unwrap(false_block));
		for (auto* input : falseInputs) {
			ifOp->getFalseBlockInvocation().addArgument(arena, input);
		}
		bb->addOperation(ifOp);
		return wrap(ifOp);
	});
}

NautilusIRValueRef nautilus_ir_build_return(NautilusIRFunctionBuilderRef builder, NautilusIRBlockRef block,
                                            NautilusIRValueRef value) {
	return guarded<NautilusIRValueRef>(nullptr, [&] {
		auto* bb = openBlock(builder, block);
		if (value == nullptr) {
			require(builder->returnType == Type::v, "a non-void function must return a value");
			return wrap(bb->addOperation<ir::ReturnOperation>());
		}
		auto* returned = operand(value, "value is NULL");
		require(returned->getStamp() == builder->returnType, "return value does not match the function's return type");
		return wrap(bb->addOperation<ir::ReturnOperation>(returned));
	});
}

/* ── Function inspection ────────────────────────────────────────────────── */

const char* nautilus_ir_function_get_name(NautilusIRFunctionRef function) {
	return function != nullptr ? unwrap(function)->getName().c_str() : nullptr;
}

NautilusIRType nautilus_ir_function_get_return_type(NautilusIRFunctionRef function) {
	return function != nullptr ? fromType(unwrap(function)->getOutputArg()) : NAUTILUS_IR_TYPE_VOID;
}

NautilusIRCalleeId nautilus_ir_function_get_callee(NautilusIRGraphRef graph, NautilusIRFunctionRef function) {
	if (graph == nullptr || function == nullptr) {
		return NAUTILUS_IR_INVALID_CALLEE;
	}
	return graph->ir->getFunctionTable().findByDefinition(unwrap(function));
}

size_t nautilus_ir_function_get_block_count(NautilusIRFunctionRef function) {
	return function != nullptr ? unwrap(function)->getBasicBlocks().size() : 0;
}

NautilusIRBlockRef nautilus_ir_function_get_block(NautilusIRFunctionRef function, size_t index) {
	return guarded<NautilusIRBlockRef>(nullptr, [&] {
		require(function != nullptr, "function is NULL");
		return wrap(unwrap(function)->getBasicBlocks().at(index));
	});
}

size_t nautilus_ir_function_get_stack_slot_count(NautilusIRFunctionRef function) {
	return function != nullptr ? unwrap(function)->getAllocaSpecs().size() : 0;
}

int nautilus_ir_function_get_stack_slot(NautilusIRFunctionRef function, uint32_t slot, size_t* size, size_t* align) {
	return guarded(1, [&] {
		require(function != nullptr, "function is NULL");
		const auto& spec = unwrap(function)->getAllocaSpecs().at(slot);
		if (size != nullptr) {
			*size = spec.size;
		}
		if (align != nullptr) {
			*align = spec.align;
		}
		return 0;
	});
}

char* nautilus_ir_function_get_attribute(NautilusIRFunctionRef function, const char* key) {
	return guarded<char*>(nullptr, [&]() -> char* {
		require(function != nullptr, "function is NULL");
		require(key != nullptr, "key is NULL");
		const auto value = unwrap(function)->getAttribute(key);
		return value ? copyString(*value) : nullptr;
	});
}

/* ── Block inspection ───────────────────────────────────────────────────── */

uint32_t nautilus_ir_block_get_id(NautilusIRBlockRef block) {
	return block != nullptr ? unwrap(block)->getIdentifier().getId() : 0;
}

size_t nautilus_ir_block_get_argument_count(NautilusIRBlockRef block) {
	return block != nullptr ? unwrap(block)->getArguments().size() : 0;
}

NautilusIRValueRef nautilus_ir_block_get_argument(NautilusIRBlockRef block, size_t index) {
	return guarded<NautilusIRValueRef>(nullptr, [&] {
		require(block != nullptr, "block is NULL");
		return wrap(unwrap(block)->getArguments().at(index));
	});
}

size_t nautilus_ir_block_get_operation_count(NautilusIRBlockRef block) {
	return block != nullptr ? unwrap(block)->getOperations().size() : 0;
}

NautilusIRValueRef nautilus_ir_block_get_operation(NautilusIRBlockRef block, size_t index) {
	return guarded<NautilusIRValueRef>(nullptr, [&] {
		require(block != nullptr, "block is NULL");
		return wrap(unwrap(block)->getOperations().at(index));
	});
}

NautilusIRValueRef nautilus_ir_block_get_terminator(NautilusIRBlockRef block) {
	if (block == nullptr || unwrap(block)->getOperations().empty()) {
		return nullptr;
	}
	auto* last = unwrap(block)->getOperations().back();
	return ir::isTerminatorOp(last->getOperationType()) ? wrap(last) : nullptr;
}

/* ── Operation / value inspection ───────────────────────────────────────── */

NautilusIROpKind nautilus_ir_value_get_kind(NautilusIRValueRef value) {
	return guarded(NAUTILUS_IR_OP_ADD, [&] {
		require(value != nullptr, "value is NULL");
		return static_cast<NautilusIROpKind>(unwrap(value)->getOperationType());
	});
}

NautilusIRType nautilus_ir_value_get_type(NautilusIRValueRef value) {
	return value != nullptr ? fromType(unwrap(value)->getStamp()) : NAUTILUS_IR_TYPE_VOID;
}

uint32_t nautilus_ir_value_get_id(NautilusIRValueRef value) {
	return value != nullptr ? unwrap(value)->getIdentifier().getId() : 0;
}

int nautilus_ir_value_is_terminator(NautilusIRValueRef value) {
	return value != nullptr && ir::isTerminatorOp(unwrap(value)->getOperationType()) ? 1 : 0;
}

size_t nautilus_ir_value_get_operand_count(NautilusIRValueRef value) {
	if (value == nullptr) {
		return 0;
	}
	auto* op = unwrap(value);
	if (ir::isTerminatorOp(op->getOperationType())) {
		size_t count = op->getInputs().size();
		for (const auto* invocation : ir::getSuccessorInvocations(*op)) {
			count += invocation->getArguments().size();
		}
		return count;
	}
	return op->getInputs().size();
}

NautilusIRValueRef nautilus_ir_value_get_operand(NautilusIRValueRef value, size_t index) {
	return guarded<NautilusIRValueRef>(nullptr, [&] {
		require(value != nullptr, "value is NULL");
		auto* op = unwrap(value);
		const auto inputs = op->getInputs();
		if (index < inputs.size()) {
			return wrap(inputs[index]);
		}
		index -= inputs.size();
		// Branch and if keep their block arguments on the invocation
		// sub-objects; expose them after the terminator's own inputs.
		for (const auto* invocation : ir::getSuccessorInvocations(*op)) {
			const auto args = invocation->getArguments();
			if (index < args.size()) {
				return wrap(args[index]);
			}
			index -= args.size();
		}
		throw ApiError("operand index out of range");
	});
}

int nautilus_ir_value_get_const_int(NautilusIRValueRef value, int64_t* out) {
	return detail<ir::ConstIntOperation>(value, [&](const ir::ConstIntOperation* op) { *out = op->getValue(); });
}

int nautilus_ir_value_get_const_float(NautilusIRValueRef value, double* out) {
	return detail<ir::ConstFloatOperation>(value, [&](const ir::ConstFloatOperation* op) { *out = op->getValue(); });
}

int nautilus_ir_value_get_const_bool(NautilusIRValueRef value, int* out) {
	return detail<ir::ConstBooleanOperation>(
	    value, [&](const ir::ConstBooleanOperation* op) { *out = op->getValue() ? 1 : 0; });
}

int nautilus_ir_value_get_const_ptr(NautilusIRValueRef value, void** out) {
	return detail<ir::ConstPtrOperation>(value, [&](const ir::ConstPtrOperation* op) { *out = op->getValue(); });
}

int nautilus_ir_value_get_comparator(NautilusIRValueRef value, NautilusIRComparator* out) {
	return detail<ir::CompareOperation>(
	    value, [&](const ir::CompareOperation* op) { *out = static_cast<NautilusIRComparator>(op->getComparator()); });
}

int nautilus_ir_value_get_bitwise_kind(NautilusIRValueRef value, NautilusIRBitwiseKind* out) {
	return detail<ir::BinaryCompOperation>(
	    value, [&](const ir::BinaryCompOperation* op) { *out = static_cast<NautilusIRBitwiseKind>(op->getType()); });
}

int nautilus_ir_value_get_shift_kind(NautilusIRValueRef value, NautilusIRShiftKind* out) {
	return detail<ir::ShiftOperation>(
	    value, [&](const ir::ShiftOperation* op) { *out = static_cast<NautilusIRShiftKind>(op->getType()); });
}

int nautilus_ir_value_get_stack_slot(NautilusIRValueRef value, uint32_t* out) {
	return detail<ir::AllocaOperation>(value, [&](const ir::AllocaOperation* op) { *out = op->getIndex(); });
}

int nautilus_ir_value_get_callee(NautilusIRValueRef value, NautilusIRCalleeId* out) {
	return guarded(1, [&] {
		require(value != nullptr, "value is NULL");
		const auto* op = unwrap(value);
		if (const auto* call = ir::dyn_cast<ir::CallOperation>(op)) {
			*out = call->getCalleeId();
			return 0;
		}
		if (const auto* address = ir::dyn_cast<ir::FunctionAddressOfOperation>(op)) {
			*out = address->getCalleeId();
			return 0;
		}
		setError("value is neither a call nor a function address");
		return 1;
	});
}

int nautilus_ir_value_get_branch_probability(NautilusIRValueRef value, double* out) {
	return detail<ir::IfOperation>(value, [&](const ir::IfOperation* op) { *out = op->getProbability(); });
}

size_t nautilus_ir_value_get_successor_count(NautilusIRValueRef value) {
	return value != nullptr ? ir::getSuccessorInvocations(*unwrap(value)).size() : 0;
}

NautilusIRBlockRef nautilus_ir_value_get_successor(NautilusIRValueRef value, size_t index) {
	return guarded<NautilusIRBlockRef>(nullptr, [&] {
		require(value != nullptr, "value is NULL");
		return wrap(ir::getSuccessorInvocations(*unwrap(value)).at(index)->getBlock());
	});
}

size_t nautilus_ir_value_get_successor_argument_count(NautilusIRValueRef value, size_t index) {
	return guarded<size_t>(0, [&] {
		require(value != nullptr, "value is NULL");
		return ir::getSuccessorInvocations(*unwrap(value)).at(index)->getArguments().size();
	});
}

NautilusIRValueRef nautilus_ir_value_get_successor_argument(NautilusIRValueRef value, size_t index, size_t arg_index) {
	return guarded<NautilusIRValueRef>(nullptr, [&] {
		require(value != nullptr, "value is NULL");
		const auto args = ir::getSuccessorInvocations(*unwrap(value)).at(index)->getArguments();
		require(arg_index < args.size(), "successor argument index out of range");
		return wrap(args[arg_index]);
	});
}

/* ── Compilation ────────────────────────────────────────────────────────── */

NautilusIROptionsRef nautilus_ir_options_create(void) {
	return guarded<NautilusIROptionsRef>(nullptr, [] { return new NautilusIROpaqueOptions(); });
}

void nautilus_ir_options_dispose(NautilusIROptionsRef options) {
	delete options;
}

void nautilus_ir_options_set_bool(NautilusIROptionsRef options, const char* name, int value) {
	if (options != nullptr && name != nullptr) {
		options->options.setOption(name, value != 0);
	}
}

void nautilus_ir_options_set_int(NautilusIROptionsRef options, const char* name, int value) {
	if (options != nullptr && name != nullptr) {
		options->options.setOption(name, value);
	}
}

void nautilus_ir_options_set_double(NautilusIROptionsRef options, const char* name, double value) {
	if (options != nullptr && name != nullptr) {
		options->options.setOption(name, value);
	}
}

void nautilus_ir_options_set_string(NautilusIROptionsRef options, const char* name, const char* value) {
	if (options != nullptr && name != nullptr && value != nullptr) {
		options->options.setOption(name, std::string(value));
	}
}

int nautilus_ir_backend_is_available(const char* backend) {
	return backend != nullptr && nautilus::compiler::CompilationBackendRegistry::getInstance()->hasBackend(backend) ? 1
	                                                                                                                : 0;
}

int nautilus_ir_graph_optimize(NautilusIRGraphRef graph, NautilusIROptimizationLevel level,
                               NautilusIROptionsRef options) {
	return guarded(1, [&] {
		require(graph != nullptr, "graph is NULL");
		optimize(graph, toLevel(level, nullptr), optionsOf(options));
		return 0;
	});
}

NautilusIRExecutableRef nautilus_ir_graph_compile(NautilusIRGraphRef graph, const char* backend,
                                                  NautilusIROptionsRef options) {
	return guarded<NautilusIRExecutableRef>(nullptr, [&] {
		require(graph != nullptr, "graph is NULL");
		const auto* registry = nautilus::compiler::CompilationBackendRegistry::getInstance();
		const std::string backendName = backend != nullptr ? backend : registry->getDefaultBackendName();
		require(registry->hasBackend(backendName), "backend is not available in this build");
		const auto* compilationBackend = registry->getBackend(backendName);
		const auto& moduleOptions = optionsOf(options);
		optimize(graph, toLevel(NAUTILUS_IR_OPTIMIZE_BACKEND_DEFAULT, compilationBackend), moduleOptions);
		const auto dumpHandler = nautilus::compiler::DumpHandler(moduleOptions, graph->ir->getId());
		auto executable = compilationBackend->compile(graph->ir, dumpHandler, moduleOptions);
		executable->setGeneratedFiles(dumpHandler.getGeneratedFiles());
		auto result = std::make_unique<NautilusIROpaqueExecutable>();
		result->executable = std::move(executable);
		return result.release();
	});
}

void* nautilus_ir_executable_get_function(NautilusIRExecutableRef executable, const char* name) {
	return guarded<void*>(nullptr, [&]() -> void* {
		require(executable != nullptr, "executable is NULL");
		require(name != nullptr, "name is NULL");
		auto* function = executable->executable->getInvocableFunctionPtr(name);
		require(function != nullptr, "no compiled function with this name");
		return function;
	});
}

void nautilus_ir_executable_dispose(NautilusIRExecutableRef executable) {
	delete executable;
}

} // extern "C"

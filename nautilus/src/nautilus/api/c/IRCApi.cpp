#include "CApiInternal.hpp"
#include "nautilus/compiler/CompilationPipeline.hpp"
#include "nautilus/compiler/DumpHandler.hpp"
#include "nautilus/compiler/backends/CompilationBackend.hpp"
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
#include <span>

namespace ir = nautilus::compiler::ir;
using nautilus::Type;
using namespace nautilus::capi;

namespace {

/* ── Conversions between the C ABI and the IR ───────────────────────────────
 * The C enumerations have their own, frozen values; nothing here relies on
 * them matching the C++ enumerations' order, which is free to change. */

Type toType(NautilusIRType type) {
	switch (type) {
	case NAUTILUS_IR_TYPE_VOID:
		return Type::v;
	case NAUTILUS_IR_TYPE_BOOL:
		return Type::b;
	case NAUTILUS_IR_TYPE_I8:
		return Type::i8;
	case NAUTILUS_IR_TYPE_I16:
		return Type::i16;
	case NAUTILUS_IR_TYPE_I32:
		return Type::i32;
	case NAUTILUS_IR_TYPE_I64:
		return Type::i64;
	case NAUTILUS_IR_TYPE_UI8:
		return Type::ui8;
	case NAUTILUS_IR_TYPE_UI16:
		return Type::ui16;
	case NAUTILUS_IR_TYPE_UI32:
		return Type::ui32;
	case NAUTILUS_IR_TYPE_UI64:
		return Type::ui64;
	case NAUTILUS_IR_TYPE_F32:
		return Type::f32;
	case NAUTILUS_IR_TYPE_F64:
		return Type::f64;
	case NAUTILUS_IR_TYPE_PTR:
		return Type::ptr;
	default:
		throw ApiError(NAUTILUS_ERROR_INVALID_ARGUMENT, "invalid NautilusIRType");
	}
}

NautilusIRType fromType(Type type) noexcept {
	switch (type) {
	case Type::v:
		return NAUTILUS_IR_TYPE_VOID;
	case Type::b:
		return NAUTILUS_IR_TYPE_BOOL;
	case Type::i8:
		return NAUTILUS_IR_TYPE_I8;
	case Type::i16:
		return NAUTILUS_IR_TYPE_I16;
	case Type::i32:
		return NAUTILUS_IR_TYPE_I32;
	case Type::i64:
		return NAUTILUS_IR_TYPE_I64;
	case Type::ui8:
		return NAUTILUS_IR_TYPE_UI8;
	case Type::ui16:
		return NAUTILUS_IR_TYPE_UI16;
	case Type::ui32:
		return NAUTILUS_IR_TYPE_UI32;
	case Type::ui64:
		return NAUTILUS_IR_TYPE_UI64;
	case Type::f32:
		return NAUTILUS_IR_TYPE_F32;
	case Type::f64:
		return NAUTILUS_IR_TYPE_F64;
	case Type::ptr:
		return NAUTILUS_IR_TYPE_PTR;
	}
	return NAUTILUS_IR_TYPE_VOID;
}

NautilusIROpKind fromOperationType(ir::Operation::OperationType type) noexcept {
	using OT = ir::Operation::OperationType;
	switch (type) {
	case OT::BasicBlockArgument:
		return NAUTILUS_IR_OP_BLOCK_ARGUMENT;
	case OT::ConstIntOp:
		return NAUTILUS_IR_OP_CONST_INT;
	case OT::ConstFloatOp:
		return NAUTILUS_IR_OP_CONST_FLOAT;
	case OT::ConstBooleanOp:
		return NAUTILUS_IR_OP_CONST_BOOL;
	case OT::ConstPtrOp:
		return NAUTILUS_IR_OP_CONST_PTR;
	case OT::AddOp:
		return NAUTILUS_IR_OP_ADD;
	case OT::SubOp:
		return NAUTILUS_IR_OP_SUB;
	case OT::MulOp:
		return NAUTILUS_IR_OP_MUL;
	case OT::DivOp:
		return NAUTILUS_IR_OP_DIV;
	case OT::ModOp:
		return NAUTILUS_IR_OP_MOD;
	case OT::AndOp:
		return NAUTILUS_IR_OP_LOGICAL_AND;
	case OT::OrOp:
		return NAUTILUS_IR_OP_LOGICAL_OR;
	case OT::NotOp:
		return NAUTILUS_IR_OP_NOT;
	case OT::BinaryComp:
		return NAUTILUS_IR_OP_BITWISE;
	case OT::ShiftOp:
		return NAUTILUS_IR_OP_SHIFT;
	case OT::NegateOp:
		return NAUTILUS_IR_OP_NEGATE;
	case OT::CompareOp:
		return NAUTILUS_IR_OP_COMPARE;
	case OT::CastOp:
		return NAUTILUS_IR_OP_CAST;
	case OT::SelectOp:
		return NAUTILUS_IR_OP_SELECT;
	case OT::LoadOp:
		return NAUTILUS_IR_OP_LOAD;
	case OT::StoreOp:
		return NAUTILUS_IR_OP_STORE;
	case OT::AllocaOp:
		return NAUTILUS_IR_OP_ALLOCA;
	case OT::CallOp:
		return NAUTILUS_IR_OP_CALL;
	case OT::IndirectCallOp:
		return NAUTILUS_IR_OP_INDIRECT_CALL;
	case OT::FunctionAddressOfOp:
		return NAUTILUS_IR_OP_FUNCTION_ADDRESS;
	case OT::BranchOp:
		return NAUTILUS_IR_OP_BRANCH;
	case OT::IfOp:
		return NAUTILUS_IR_OP_IF;
	case OT::ReturnOp:
		return NAUTILUS_IR_OP_RETURN;
	default:
		// Kinds that never appear in a block (FunctionOp, BlockInvocation,
		// MLIR_YIELD) and any added to the IR after this API.
		return NAUTILUS_IR_OP_UNKNOWN;
	}
}

NautilusIRLinkage fromLinkage(ir::Linkage linkage) noexcept {
	switch (linkage) {
	case ir::Linkage::Internal:
		return NAUTILUS_IR_LINKAGE_INTERNAL;
	case ir::Linkage::External:
		return NAUTILUS_IR_LINKAGE_EXTERNAL;
	case ir::Linkage::Intrinsic:
		return NAUTILUS_IR_LINKAGE_INTRINSIC;
	}
	return NAUTILUS_IR_LINKAGE_EXTERNAL;
}

nautilus::FunctionAttributes toAttributes(const NautilusIRFunctionAttributes& attributes) {
	constexpr NautilusIRFunctionFlags knownFlags = NAUTILUS_IR_FUNCTION_WILL_RETURN | NAUTILUS_IR_FUNCTION_NO_UNWIND;
	require((attributes.flags & ~knownFlags) == 0, "unknown NautilusIRFunctionFlags bits");
	nautilus::ModRefInfo modRef;
	switch (attributes.mod_ref) {
	case NAUTILUS_IR_MOD_REF_NONE:
		modRef = nautilus::ModRefInfo::NoModRef;
		break;
	case NAUTILUS_IR_MOD_REF_READS:
		modRef = nautilus::ModRefInfo::Ref;
		break;
	case NAUTILUS_IR_MOD_REF_WRITES:
		modRef = nautilus::ModRefInfo::Mod;
		break;
	case NAUTILUS_IR_MOD_REF_READS_WRITES:
		modRef = nautilus::ModRefInfo::ModRef;
		break;
	default:
		throw ApiError(NAUTILUS_ERROR_INVALID_ARGUMENT, "invalid NautilusIRModRef");
	}
	return nautilus::FunctionAttributes {.modRefInfo = modRef,
	                                     .willReturn = (attributes.flags & NAUTILUS_IR_FUNCTION_WILL_RETURN) != 0,
	                                     .noUnwind = (attributes.flags & NAUTILUS_IR_FUNCTION_NO_UNWIND) != 0};
}

nautilus::compiler::IROptimizationLevel toLevel(NautilusIROptimizationLevel level,
                                                const nautilus::compiler::CompilationBackend* backend) {
	using Level = nautilus::compiler::IROptimizationLevel;
	switch (level) {
	case NAUTILUS_IR_OPTIMIZE_BACKEND_DEFAULT:
		return backend != nullptr ? backend->irOptimizationLevel() : Level::Full;
	case NAUTILUS_IR_OPTIMIZE_NONE:
		return Level::None;
	case NAUTILUS_IR_OPTIMIZE_ARGUMENT_PRUNING:
		return Level::ArgumentPruning;
	case NAUTILUS_IR_OPTIMIZE_FULL:
		return Level::Full;
	default:
		throw ApiError(NAUTILUS_ERROR_INVALID_ARGUMENT, "invalid NautilusIROptimizationLevel");
	}
}

bool isIntegerType(Type type) noexcept {
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

bool isFloatType(Type type) noexcept {
	return type == Type::f32 || type == Type::f64;
}

/* ── Handles ────────────────────────────────────────────────────────────────
 * Function, block and value refs are the IR's own pointers, so taking and
 * using one costs nothing. */

ir::Operation* unwrap(NautilusIRValueRef value) noexcept {
	return reinterpret_cast<ir::Operation*>(value);
}

NautilusIRValueRef wrap(const ir::Operation* op) noexcept {
	return reinterpret_cast<NautilusIRValueRef>(const_cast<ir::Operation*>(op));
}

ir::BasicBlock* unwrap(NautilusIRBlockRef block) noexcept {
	return reinterpret_cast<ir::BasicBlock*>(block);
}

NautilusIRBlockRef wrap(const ir::BasicBlock* block) noexcept {
	return reinterpret_cast<NautilusIRBlockRef>(const_cast<ir::BasicBlock*>(block));
}

ir::FunctionOperation* unwrap(NautilusIRFunctionRef function) noexcept {
	return reinterpret_cast<ir::FunctionOperation*>(function);
}

NautilusIRFunctionRef wrap(const ir::FunctionOperation* function) noexcept {
	return reinterpret_cast<NautilusIRFunctionRef>(const_cast<ir::FunctionOperation*>(function));
}

/// The block invocation of successor @p index of @p op, or nullptr. Unlike
/// ir::getSuccessorInvocations this allocates nothing, which matters for an
/// accessor a binding calls once per terminator.
const ir::BasicBlockInvocation* successorAt(const ir::Operation* op, size_t index) noexcept {
	if (const auto* branch = ir::dyn_cast<ir::BranchOperation>(op)) {
		return index == 0 ? &branch->getNextBlockInvocation() : nullptr;
	}
	if (const auto* ifOp = ir::dyn_cast<ir::IfOperation>(op)) {
		if (index == 0) {
			return &ifOp->getTrueBlockInvocation();
		}
		return index == 1 ? &ifOp->getFalseBlockInvocation() : nullptr;
	}
	return nullptr;
}

size_t successorCount(const ir::Operation* op) noexcept {
	if (ir::isa<ir::BranchOperation>(op)) {
		return 1;
	}
	return ir::isa<ir::IfOperation>(op) ? 2 : 0;
}

/* ── Building ───────────────────────────────────────────────────────────── */

ir::OperationIdentifier nextId(NautilusIROpaqueFunctionBuilder* builder) noexcept {
	return ir::OperationIdentifier {builder->graph->nextOperationId++};
}

/// The builder's callee id, interning it on first use.
ir::FunctionId calleeOf(NautilusIROpaqueFunctionBuilder* builder) {
	if (builder->calleeId == ir::INVALID_FUNCTION_ID) {
		// A null identity key mints a fresh entry, as the trace path does for
		// its entry function; finish() binds the definition to it.
		ir::CalleeDescriptor descriptor;
		descriptor.kind = ir::CalleeDescriptor::Kind::Internal;
		descriptor.customName = builder->name;
		builder->calleeId = builder->graph->ir->internCallee(descriptor);
		builder->graph->pendingByCallee[builder->calleeId] = builder;
	}
	return builder->calleeId;
}

/// Detaches @p builder from its graph and hands back ownership of it.
std::unique_ptr<NautilusIROpaqueFunctionBuilder> detach(NautilusIROpaqueFunctionBuilder* builder) {
	auto* graph = builder->graph;
	auto it = graph->builders.find(builder);
	check(it != graph->builders.end(), NAUTILUS_ERROR_INVALID_STATE, "builder has already been finished");
	auto owned = std::move(it->second);
	graph->builders.erase(it);
	graph->pendingByCallee.erase(builder->calleeId);
	return owned;
}

/// Validates that @p block may be appended to through @p builder.
ir::BasicBlock* openBlock(NautilusIROpaqueFunctionBuilder* builder, NautilusIRBlockRef blockRef) {
	require(builder != nullptr, "builder is NULL");
	require(blockRef != nullptr, "block is NULL");
	auto* block = unwrap(blockRef);
	require(builder->ownedBlocks.contains(block), "block does not belong to this function builder");
	const auto& operations = block->getOperations();
	check(operations.empty() || !ir::isTerminatorOp(operations.back()->getOperationType()),
	      NAUTILUS_ERROR_INVALID_STATE, "block already ends in a terminator");
	return block;
}

ir::Operation* operand(NautilusIRValueRef value, const char* message) {
	require(value != nullptr, message);
	auto* op = unwrap(value);
	check(op->getStamp() != Type::v, NAUTILUS_ERROR_TYPE_MISMATCH, "operand does not produce a value");
	return op;
}

void requireType(bool condition, const char* message) {
	check(condition, NAUTILUS_ERROR_TYPE_MISMATCH, message);
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

/// Checks that @p args match the arguments of @p target, as an edge to it must.
void checkInvocation(NautilusIROpaqueFunctionBuilder* builder, NautilusIRBlockRef targetRef,
                     const std::vector<ir::Operation*>& args) {
	require(targetRef != nullptr, "target block is NULL");
	auto* target = unwrap(targetRef);
	require(builder->ownedBlocks.contains(target), "target block does not belong to this function builder");
	require(target != builder->blocks.front(), "the entry block cannot be branched to");
	const auto& params = target->getArguments();
	require(params.size() == args.size(), "argument count does not match the target block's arguments");
	for (size_t i = 0; i < args.size(); ++i) {
		requireType(params[i]->getStamp() == args[i]->getStamp(),
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
	const ir::BasicBlock* entry = nullptr;
	Type result;
	if (auto it = graph->pendingByCallee.find(callee); it != graph->pendingByCallee.end()) {
		check(!it->second->blocks.empty(), NAUTILUS_ERROR_INVALID_STATE,
		      "callee has no entry block yet; add it before building calls to the function");
		entry = it->second->blocks.front();
		result = it->second->returnType;
	} else {
		const auto& target = table.get(callee);
		if (target.getLinkage() != ir::Linkage::Internal) {
			return Signature {target.getResultType(), target.getParamTypes()};
		}
		check(target.getDefinition() != nullptr, NAUTILUS_ERROR_INVALID_STATE,
		      "callee's function was abandoned before it was finished");
		entry = target.getDefinition()->getEntryBlock();
		result = target.getResultType();
	}
	Signature signature {result, {}};
	for (const auto* arg : entry->getArguments()) {
		signature.params.push_back(arg->getStamp());
	}
	return signature;
}

/// A builder's guarded body: @p body gets the validated block and returns the
/// new operation.
template <typename F>
NautilusIRValueRef build(NautilusIRFunctionBuilderRef builder, NautilusIRBlockRef block, F&& body) noexcept {
	return guarded<NautilusIRValueRef>(nullptr, [&] { return wrap(body(openBlock(builder, block))); });
}

/* ── Inspection ─────────────────────────────────────────────────────────── */

template <typename T, typename Out, typename F>
NautilusStatus detail(NautilusIRValueRef value, Out* out, F&& read) noexcept {
	return status([&] {
		require(value != nullptr, "value is NULL");
		outParam(out);
		const auto* op = ir::dyn_cast<T>(unwrap(value));
		require(op != nullptr, "value has a different operation kind");
		*out = read(op);
	});
}

void optimize(NautilusIROpaqueGraph* graph, nautilus::compiler::IROptimizationLevel level,
              const nautilus::engine::ModuleOptions& options) {
	prepareForPasses(graph);
	if (graph->optimized) {
		return;
	}
	// Claimed before running: the passes rewrite the graph in place, so even
	// a run that fails part way must not be repeated on the result.
	graph->optimized = true;
	compiling([&] { nautilus::compiler::CompilationPipeline::runIRPasses(*graph->ir, options, level); });
}

} // namespace

extern "C" {

NautilusStringRef nautilus_ir_type_name(NautilusIRType type) {
	if (type > NAUTILUS_IR_TYPE_PTR) {
		return NautilusStringRef {"", 0};
	}
	const char* name = nautilus::toString(toType(type));
	return NautilusStringRef {name, std::strlen(name)};
}

NautilusIRFunctionAttributes nautilus_ir_function_attributes_default(void) {
	return NautilusIRFunctionAttributes {.mod_ref = NAUTILUS_IR_MOD_REF_READS_WRITES, .flags = 0};
}

/* ── Graphs ─────────────────────────────────────────────────────────────── */

NautilusIRGraphRef nautilus_ir_graph_create(NautilusStringRef id) {
	return guarded<NautilusIRGraphRef>(nullptr, [&] {
		auto graph = std::make_unique<NautilusIROpaqueGraph>();
		auto name = toString(id);
		graph->ir = std::make_shared<ir::IRGraph>(name.empty() ? "c-api" : name);
		return graph.release();
	});
}

void nautilus_ir_graph_dispose(NautilusIRGraphRef graph) {
	delete graph;
}

NautilusStatus nautilus_ir_graph_to_string(NautilusIRGraphRef graph, NautilusString* out) {
	return status([&] {
		require(graph != nullptr, "graph is NULL");
		*outParam(out) = own(graph->ir->toString());
	});
}

NautilusStatus nautilus_ir_graph_verify(NautilusIRGraphRef graph, NautilusString* diagnostics) {
	return status([&] {
		require(graph != nullptr, "graph is NULL");
		ir::rebuildPredecessorLists(*graph->ir);
		const auto result = ir::IRVerifier::verify(*graph->ir);
		if (result.ok()) {
			return;
		}
		const auto text = result.toString();
		if (diagnostics != nullptr) {
			*diagnostics = own(text);
		}
		throw ApiError(NAUTILUS_ERROR_VERIFICATION_FAILED, text);
	});
}

size_t nautilus_ir_graph_get_functions(NautilusIRGraphRef graph, NautilusIRFunctionRef* out, size_t capacity) {
	if (graph == nullptr) {
		return 0;
	}
	return copyOut(graph->ir->getFunctionOperations(), out, capacity,
	               [](const ir::FunctionOperation* fn) { return wrap(fn); });
}

NautilusStatus nautilus_ir_graph_find_function(NautilusIRGraphRef graph, NautilusStringRef name,
                                               NautilusIRFunctionRef* out) {
	return status([&] {
		require(graph != nullptr, "graph is NULL");
		outParam(out);
		const auto* function = graph->ir->getFunctionOperation(toString(name));
		check(function != nullptr, NAUTILUS_ERROR_NOT_FOUND, "no finished function with this name");
		*out = wrap(function);
	});
}

NautilusStatus nautilus_ir_graph_declare_external_function(NautilusIRGraphRef graph, NautilusStringRef symbol,
                                                           NautilusStringRef display_name, void* address,
                                                           NautilusIRType result_type,
                                                           const NautilusIRType* param_types, size_t param_count,
                                                           NautilusIRFunctionAttributes attributes,
                                                           NautilusIRCalleeId* out) {
	return status([&] {
		require(graph != nullptr, "graph is NULL");
		require(address != nullptr, "address is NULL");
		require(param_count == 0 || param_types != nullptr, "param_types is NULL");
		outParam(out);
		ir::CalleeDescriptor descriptor;
		descriptor.kind = ir::CalleeDescriptor::Kind::External;
		descriptor.key = address;
		descriptor.mangledName = toString(symbol);
		descriptor.demangledName = toString(display_name);
		if (descriptor.demangledName.empty()) {
			descriptor.demangledName = descriptor.mangledName;
		}
		descriptor.resultType = toType(result_type);
		descriptor.paramTypes.reserve(param_count);
		for (size_t i = 0; i < param_count; ++i) {
			const auto type = toType(param_types[i]);
			requireType(type != Type::v, "a parameter cannot have type void");
			descriptor.paramTypes.push_back(type);
		}
		descriptor.attrs = toAttributes(attributes);
		*out = graph->ir->internCallee(descriptor);
	});
}

size_t nautilus_ir_graph_get_callee_count(NautilusIRGraphRef graph) {
	return graph != nullptr ? graph->ir->getFunctionTable().size() : 0;
}

NautilusStatus nautilus_ir_graph_get_callee_info(NautilusIRGraphRef graph, NautilusIRCalleeId callee,
                                                 NautilusIRCalleeInfo* out) {
	return status([&] {
		require(graph != nullptr, "graph is NULL");
		outParam(out);
		const auto& table = graph->ir->getFunctionTable();
		check(table.contains(callee), NAUTILUS_ERROR_NOT_FOUND, "unknown callee id");
		const auto& target = table.get(callee);
		const auto* native = target.getNative();
		*out = NautilusIRCalleeInfo {.linkage = fromLinkage(target.getLinkage()),
		                             .name = borrow(target.getName().forEmission()),
		                             .address = native != nullptr ? native->address : nullptr,
		                             .function = wrap(target.getDefinition())};
	});
}

/* ── Function builders ──────────────────────────────────────────────────── */

NautilusIRFunctionBuilderRef nautilus_ir_function_builder_create(NautilusIRGraphRef graph, NautilusStringRef name,
                                                                 NautilusIRType return_type) {
	return guarded<NautilusIRFunctionBuilderRef>(nullptr, [&] {
		require(graph != nullptr, "graph is NULL");
		auto functionName = toString(name);
		require(!functionName.empty(), "function name is empty");
		check(!graph->optimized, NAUTILUS_ERROR_INVALID_STATE, "graph has already been optimized or compiled");
		const auto taken = graph->ir->getFunctionOperation(functionName) != nullptr ||
		                   std::any_of(graph->builders.begin(), graph->builders.end(),
		                               [&](const auto& entry) { return entry.second->name == functionName; });
		check(!taken, NAUTILUS_ERROR_INVALID_STATE, "a function with this name already exists");
		auto builder = std::make_unique<NautilusIROpaqueFunctionBuilder>();
		builder->graph = graph;
		builder->name = std::move(functionName);
		builder->returnType = toType(return_type);
		auto* raw = builder.get();
		graph->builders.emplace(raw, std::move(builder));
		return raw;
	});
}

void nautilus_ir_function_builder_dispose(NautilusIRFunctionBuilderRef builder) {
	if (builder != nullptr) {
		guarded(false, [&] {
			detach(builder);
			return true;
		});
	}
}

NautilusStatus nautilus_ir_function_builder_get_callee(NautilusIRFunctionBuilderRef builder, NautilusIRCalleeId* out) {
	return status([&] {
		require(builder != nullptr, "builder is NULL");
		*outParam(out) = calleeOf(builder);
	});
}

NautilusStatus nautilus_ir_function_builder_set_attribute(NautilusIRFunctionBuilderRef builder, NautilusStringRef key,
                                                          NautilusStringRef value) {
	return status([&] {
		require(builder != nullptr, "builder is NULL");
		auto attributeKey = toString(key);
		require(!attributeKey.empty(), "attribute key is empty");
		builder->attributes[std::move(attributeKey)] = toString(value);
	});
}

NautilusIRBlockRef nautilus_ir_function_builder_add_block(NautilusIRFunctionBuilderRef builder,
                                                          const NautilusIRType* arg_types, size_t arg_count) {
	return guarded<NautilusIRBlockRef>(nullptr, [&] {
		require(builder != nullptr, "builder is NULL");
		require(arg_count == 0 || arg_types != nullptr, "arg_types is NULL");
		std::vector<Type> types;
		types.reserve(arg_count);
		for (size_t i = 0; i < arg_count; ++i) {
			types.push_back(toType(arg_types[i]));
			requireType(types.back() != Type::v, "a block argument cannot have type void");
		}
		auto& arena = builder->graph->ir->getArena();
		std::vector<ir::BasicBlockArgument*> arguments;
		arguments.reserve(arg_count);
		for (const auto type : types) {
			arguments.push_back(arena.create<ir::BasicBlockArgument>(nextId(builder), type));
		}
		auto* block =
		    arena.create<ir::BasicBlock>(arena, ir::BlockIdentifier {builder->nextBlockId++}, std::move(arguments));
		builder->blocks.push_back(block);
		builder->ownedBlocks.insert(block);
		return wrap(block);
	});
}

NautilusStatus nautilus_ir_function_builder_add_stack_slot(NautilusIRFunctionBuilderRef builder, size_t size,
                                                           size_t align, uint32_t* out_slot) {
	return status([&] {
		require(builder != nullptr, "builder is NULL");
		outParam(out_slot);
		require(size > 0, "stack slot size must be positive");
		require(align > 0 && (align & (align - 1)) == 0, "stack slot alignment must be a power of two");
		builder->allocaSpecs.push_back(ir::AllocaSpec {size, align});
		*out_slot = static_cast<uint32_t>(builder->allocaSpecs.size() - 1);
	});
}

NautilusIRFunctionRef nautilus_ir_function_builder_finish(NautilusIRFunctionBuilderRef builder) {
	return guarded<NautilusIRFunctionRef>(nullptr, [&] {
		require(builder != nullptr, "builder is NULL");
		auto* graph = builder->graph;
		// Detach first so the builder is consumed whatever happens below.
		auto owned = detach(builder);
		check(!owned->blocks.empty(), NAUTILUS_ERROR_INVALID_STATE, "function has no blocks");
		for (const auto* block : owned->blocks) {
			const auto& operations = block->getOperations();
			check(!operations.empty() && ir::isTerminatorOp(operations.back()->getOperationType()),
			      NAUTILUS_ERROR_INVALID_STATE, "every block must end in a terminator");
		}
		const auto calleeId = calleeOf(owned.get());
		graph->pendingByCallee.erase(calleeId);
		auto& arena = graph->ir->getArena();
		auto* function = arena.create<ir::FunctionOperation>(
		    owned->name, std::move(owned->blocks), std::vector<Type> {}, std::vector<std::string> {}, owned->returnType,
		    std::move(owned->allocaSpecs), std::move(owned->attributes));
		graph->ir->addFunctionOperation(function);
		graph->ir->defineFunction(calleeId, function);
		return wrap(function);
	});
}

/* ── Instruction building ───────────────────────────────────────────────── */

NautilusIRValueRef nautilus_ir_build_const_int(NautilusIRFunctionBuilderRef builder, NautilusIRBlockRef block,
                                               int64_t value, NautilusIRType type) {
	return build(builder, block, [&](ir::BasicBlock* bb) {
		const auto stamp = toType(type);
		requireType(isIntegerType(stamp), "integer constants need an integer type");
		return bb->addOperation<ir::ConstIntOperation>(nextId(builder), value, stamp);
	});
}

NautilusIRValueRef nautilus_ir_build_const_float(NautilusIRFunctionBuilderRef builder, NautilusIRBlockRef block,
                                                 double value, NautilusIRType type) {
	return build(builder, block, [&](ir::BasicBlock* bb) {
		const auto stamp = toType(type);
		requireType(isFloatType(stamp), "float constants need a float type");
		return bb->addOperation<ir::ConstFloatOperation>(nextId(builder), value, stamp);
	});
}

NautilusIRValueRef nautilus_ir_build_const_bool(NautilusIRFunctionBuilderRef builder, NautilusIRBlockRef block,
                                                bool value) {
	return build(builder, block, [&](ir::BasicBlock* bb) {
		return bb->addOperation<ir::ConstBooleanOperation>(nextId(builder), value);
	});
}

NautilusIRValueRef nautilus_ir_build_const_ptr(NautilusIRFunctionBuilderRef builder, NautilusIRBlockRef block,
                                               void* value) {
	return build(builder, block,
	             [&](ir::BasicBlock* bb) { return bb->addOperation<ir::ConstPtrOperation>(nextId(builder), value); });
}

NautilusIRValueRef nautilus_ir_build_binary(NautilusIRFunctionBuilderRef builder, NautilusIRBlockRef block,
                                            NautilusIRBinaryOp op, NautilusIRValueRef lhs, NautilusIRValueRef rhs) {
	return build(builder, block, [&](ir::BasicBlock* bb) -> ir::Operation* {
		auto* left = operand(lhs, "lhs is NULL");
		auto* right = operand(rhs, "rhs is NULL");
		const auto id = nextId(builder);
		switch (op) {
		case NAUTILUS_IR_BINARY_ADD:
			return bb->addOperation<ir::AddOperation>(id, left, right);
		case NAUTILUS_IR_BINARY_SUB:
			return bb->addOperation<ir::SubOperation>(id, left, right);
		case NAUTILUS_IR_BINARY_MUL:
			return bb->addOperation<ir::MulOperation>(id, left, right);
		case NAUTILUS_IR_BINARY_DIV:
			return bb->addOperation<ir::DivOperation>(id, left, right);
		case NAUTILUS_IR_BINARY_MOD:
			return bb->addOperation<ir::ModOperation>(id, left, right);
		case NAUTILUS_IR_BINARY_LOGICAL_AND:
			requireType(left->getStamp() == Type::b && right->getStamp() == Type::b, "logical and needs bool operands");
			return bb->addOperation<ir::AndOperation>(id, left, right);
		case NAUTILUS_IR_BINARY_LOGICAL_OR:
			requireType(left->getStamp() == Type::b && right->getStamp() == Type::b, "logical or needs bool operands");
			return bb->addOperation<ir::OrOperation>(id, left, right);
		case NAUTILUS_IR_BINARY_BITWISE_AND:
			return bb->addOperation<ir::BinaryCompOperation>(id, left, right, ir::BinaryCompOperation::BAND);
		case NAUTILUS_IR_BINARY_BITWISE_OR:
			return bb->addOperation<ir::BinaryCompOperation>(id, left, right, ir::BinaryCompOperation::BOR);
		case NAUTILUS_IR_BINARY_BITWISE_XOR:
			return bb->addOperation<ir::BinaryCompOperation>(id, left, right, ir::BinaryCompOperation::XOR);
		case NAUTILUS_IR_BINARY_SHIFT_LEFT:
			return bb->addOperation<ir::ShiftOperation>(id, left, right, ir::ShiftOperation::LS);
		case NAUTILUS_IR_BINARY_SHIFT_RIGHT:
			return bb->addOperation<ir::ShiftOperation>(id, left, right, ir::ShiftOperation::RS);
		default:
			throw ApiError(NAUTILUS_ERROR_INVALID_ARGUMENT, "invalid NautilusIRBinaryOp");
		}
	});
}

NautilusIRValueRef nautilus_ir_build_compare(NautilusIRFunctionBuilderRef builder, NautilusIRBlockRef block,
                                             NautilusIRComparator comparator, NautilusIRValueRef lhs,
                                             NautilusIRValueRef rhs) {
	return build(builder, block, [&](ir::BasicBlock* bb) {
		auto* left = operand(lhs, "lhs is NULL");
		auto* right = operand(rhs, "rhs is NULL");
		ir::CompareOperation::Comparator kind;
		switch (comparator) {
		case NAUTILUS_IR_CMP_EQ:
			kind = ir::CompareOperation::EQ;
			break;
		case NAUTILUS_IR_CMP_NE:
			kind = ir::CompareOperation::NE;
			break;
		case NAUTILUS_IR_CMP_LT:
			kind = ir::CompareOperation::LT;
			break;
		case NAUTILUS_IR_CMP_LE:
			kind = ir::CompareOperation::LE;
			break;
		case NAUTILUS_IR_CMP_GT:
			kind = ir::CompareOperation::GT;
			break;
		case NAUTILUS_IR_CMP_GE:
			kind = ir::CompareOperation::GE;
			break;
		default:
			throw ApiError(NAUTILUS_ERROR_INVALID_ARGUMENT, "invalid NautilusIRComparator");
		}
		return bb->addOperation<ir::CompareOperation>(nextId(builder), left, right, kind);
	});
}

NautilusIRValueRef nautilus_ir_build_not(NautilusIRFunctionBuilderRef builder, NautilusIRBlockRef block,
                                         NautilusIRValueRef value) {
	return build(builder, block, [&](ir::BasicBlock* bb) {
		auto* input = operand(value, "value is NULL");
		requireType(input->getStamp() == Type::b, "not needs a bool operand");
		return bb->addOperation<ir::NotOperation>(nextId(builder), input);
	});
}

NautilusIRValueRef nautilus_ir_build_negate(NautilusIRFunctionBuilderRef builder, NautilusIRBlockRef block,
                                            NautilusIRValueRef value) {
	return build(builder, block, [&](ir::BasicBlock* bb) {
		auto* input = operand(value, "value is NULL");
		requireType(isIntegerType(input->getStamp()), "negate needs an integer operand");
		return bb->addOperation<ir::NegateOperation>(nextId(builder), input);
	});
}

NautilusIRValueRef nautilus_ir_build_cast(NautilusIRFunctionBuilderRef builder, NautilusIRBlockRef block,
                                          NautilusIRValueRef value, NautilusIRType target_type) {
	return build(builder, block, [&](ir::BasicBlock* bb) {
		auto* input = operand(value, "value is NULL");
		const auto target = toType(target_type);
		requireType(target != Type::v, "cannot cast to void");
		return bb->addOperation<ir::CastOperation>(nextId(builder), input, target);
	});
}

NautilusIRValueRef nautilus_ir_build_select(NautilusIRFunctionBuilderRef builder, NautilusIRBlockRef block,
                                            NautilusIRValueRef condition, NautilusIRValueRef true_value,
                                            NautilusIRValueRef false_value) {
	return build(builder, block, [&](ir::BasicBlock* bb) {
		auto* cond = operand(condition, "condition is NULL");
		auto* onTrue = operand(true_value, "true_value is NULL");
		auto* onFalse = operand(false_value, "false_value is NULL");
		requireType(cond->getStamp() == Type::b, "select needs a bool condition");
		requireType(onTrue->getStamp() == onFalse->getStamp(), "select needs operands of the same type");
		return bb->addOperation<ir::SelectOperation>(nextId(builder), cond, onTrue, onFalse, onTrue->getStamp());
	});
}

NautilusIRValueRef nautilus_ir_build_load(NautilusIRFunctionBuilderRef builder, NautilusIRBlockRef block,
                                          NautilusIRValueRef address, NautilusIRType type) {
	return build(builder, block, [&](ir::BasicBlock* bb) {
		auto* addr = operand(address, "address is NULL");
		requireType(addr->getStamp() == Type::ptr, "load needs a pointer address");
		const auto stamp = toType(type);
		requireType(stamp != Type::v, "cannot load void");
		return bb->addOperation<ir::LoadOperation>(nextId(builder), addr, stamp);
	});
}

NautilusIRValueRef nautilus_ir_build_store(NautilusIRFunctionBuilderRef builder, NautilusIRBlockRef block,
                                           NautilusIRValueRef value, NautilusIRValueRef address) {
	return build(builder, block, [&](ir::BasicBlock* bb) {
		auto* stored = operand(value, "value is NULL");
		auto* addr = operand(address, "address is NULL");
		requireType(addr->getStamp() == Type::ptr, "store needs a pointer address");
		return bb->addOperation<ir::StoreOperation>(stored, addr);
	});
}

NautilusIRValueRef nautilus_ir_build_alloca(NautilusIRFunctionBuilderRef builder, NautilusIRBlockRef block,
                                            uint32_t slot) {
	return build(builder, block, [&](ir::BasicBlock* bb) {
		require(slot < builder->allocaSpecs.size(), "unknown stack slot");
		return bb->addOperation<ir::AllocaOperation>(nextId(builder), slot);
	});
}

NautilusIRValueRef nautilus_ir_build_call(NautilusIRFunctionBuilderRef builder, NautilusIRBlockRef block,
                                          NautilusIRCalleeId callee, const NautilusIRValueRef* args, size_t arg_count) {
	return build(builder, block, [&](ir::BasicBlock* bb) {
		auto* graph = builder->graph;
		const auto signature = signatureOf(graph, callee);
		const auto inputs = operands(args, arg_count);
		require(inputs.size() == signature.params.size(), "argument count does not match the callee's parameters");
		for (size_t i = 0; i < inputs.size(); ++i) {
			requireType(inputs[i]->getStamp() == signature.params[i],
			            "argument type does not match the callee's parameter type");
		}
		const auto& target = graph->ir->getFunctionTarget(callee);
		const auto& name = target.getName().forEmission();
		const bool internal = target.getLinkage() == ir::Linkage::Internal;
		const auto attributes = internal ? nautilus::FunctionAttributes {} : target.getNative()->attrs;
		const auto& symbol = internal ? name : target.getName().getMangled();
		return bb->addOperation<ir::CallOperation>(
		    symbol, name, internal ? nullptr : target.getAddress(), nextId(builder), inputs, signature.result,
		    attributes, callee, std::vector<ir::CallOperation::Destructor> {}, false, nullptr, internal);
	});
}

NautilusIRValueRef nautilus_ir_build_indirect_call(NautilusIRFunctionBuilderRef builder, NautilusIRBlockRef block,
                                                   NautilusIRValueRef function_pointer, const NautilusIRValueRef* args,
                                                   size_t arg_count, NautilusIRType result_type,
                                                   NautilusIRFunctionAttributes attributes) {
	return build(builder, block, [&](ir::BasicBlock* bb) {
		auto* fnPtr = operand(function_pointer, "function_pointer is NULL");
		requireType(fnPtr->getStamp() == Type::ptr, "indirect call needs a pointer callee");
		const auto inputs = operands(args, arg_count);
		return bb->addOperation<ir::IndirectCallOperation>(nextId(builder), fnPtr, inputs, toType(result_type),
		                                                   toAttributes(attributes));
	});
}

NautilusIRValueRef nautilus_ir_build_function_address(NautilusIRFunctionBuilderRef builder, NautilusIRBlockRef block,
                                                      NautilusIRCalleeId callee) {
	return build(builder, block, [&](ir::BasicBlock* bb) {
		const auto& table = builder->graph->ir->getFunctionTable();
		require(table.contains(callee), "unknown callee id");
		const auto& target = table.get(callee);
		const auto& name = target.getName().forEmission();
		const bool internal = target.getLinkage() == ir::Linkage::Internal;
		return bb->addOperation<ir::FunctionAddressOfOperation>(internal ? name : target.getName().getMangled(), name,
		                                                        internal ? nullptr : target.getAddress(),
		                                                        nextId(builder), callee);
	});
}

NautilusIRValueRef nautilus_ir_build_branch(NautilusIRFunctionBuilderRef builder, NautilusIRBlockRef block,
                                            NautilusIRBlockRef target, const NautilusIRValueRef* args,
                                            size_t arg_count) {
	return build(builder, block, [&](ir::BasicBlock* bb) {
		const auto inputs = operands(args, arg_count);
		checkInvocation(builder, target, inputs);
		return bb->addNextBlock(unwrap(target), inputs);
	});
}

NautilusIRValueRef nautilus_ir_build_if(NautilusIRFunctionBuilderRef builder, NautilusIRBlockRef block,
                                        NautilusIRValueRef condition, NautilusIRBlockRef true_block,
                                        const NautilusIRValueRef* true_args, size_t true_arg_count,
                                        NautilusIRBlockRef false_block, const NautilusIRValueRef* false_args,
                                        size_t false_arg_count, double probability) {
	return build(builder, block, [&](ir::BasicBlock* bb) {
		auto* cond = operand(condition, "condition is NULL");
		requireType(cond->getStamp() == Type::b, "if needs a bool condition");
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
		return ifOp;
	});
}

NautilusIRValueRef nautilus_ir_build_return(NautilusIRFunctionBuilderRef builder, NautilusIRBlockRef block,
                                            NautilusIRValueRef value) {
	return build(builder, block, [&](ir::BasicBlock* bb) -> ir::Operation* {
		if (value == nullptr) {
			requireType(builder->returnType == Type::v, "a non-void function must return a value");
			return bb->addOperation<ir::ReturnOperation>();
		}
		auto* returned = operand(value, "value is NULL");
		requireType(returned->getStamp() == builder->returnType,
		            "return value does not match the function's return type");
		return bb->addOperation<ir::ReturnOperation>(returned);
	});
}

/* ── Functions ──────────────────────────────────────────────────────────── */

NautilusStringRef nautilus_ir_function_get_name(NautilusIRFunctionRef function) {
	return function != nullptr ? borrow(unwrap(function)->getName()) : NautilusStringRef {"", 0};
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

NautilusIRBlockRef nautilus_ir_function_get_entry_block(NautilusIRFunctionRef function) {
	return function != nullptr ? wrap(unwrap(function)->getEntryBlock()) : nullptr;
}

size_t nautilus_ir_function_get_blocks(NautilusIRFunctionRef function, NautilusIRBlockRef* out, size_t capacity) {
	if (function == nullptr) {
		return 0;
	}
	return copyOut(unwrap(function)->getBasicBlocks(), out, capacity,
	               [](const ir::BasicBlock* block) { return wrap(block); });
}

size_t nautilus_ir_function_get_stack_slots(NautilusIRFunctionRef function, NautilusIRStackSlot* out, size_t capacity) {
	if (function == nullptr) {
		return 0;
	}
	return copyOut(unwrap(function)->getAllocaSpecs(), out, capacity,
	               [](const ir::AllocaSpec& spec) { return NautilusIRStackSlot {spec.size, spec.align}; });
}

NautilusStatus nautilus_ir_function_get_attribute(NautilusIRFunctionRef function, NautilusStringRef key,
                                                  NautilusString* out) {
	return status([&] {
		require(function != nullptr, "function is NULL");
		outParam(out);
		const auto value = unwrap(function)->getAttribute(toString(key));
		check(value.has_value(), NAUTILUS_ERROR_NOT_FOUND, "attribute is not set");
		*out = own(*value);
	});
}

/* ── Blocks ─────────────────────────────────────────────────────────────── */

uint32_t nautilus_ir_block_get_id(NautilusIRBlockRef block) {
	return block != nullptr ? unwrap(block)->getIdentifier().getId() : 0;
}

size_t nautilus_ir_block_get_arguments(NautilusIRBlockRef block, NautilusIRValueRef* out, size_t capacity) {
	if (block == nullptr) {
		return 0;
	}
	return copyOut(unwrap(block)->getArguments(), out, capacity,
	               [](const ir::BasicBlockArgument* arg) { return wrap(arg); });
}

size_t nautilus_ir_block_get_operations(NautilusIRBlockRef block, NautilusIRValueRef* out, size_t capacity) {
	if (block == nullptr) {
		return 0;
	}
	return copyOut(unwrap(block)->getOperations(), out, capacity, [](const ir::Operation* op) { return wrap(op); });
}

NautilusIRValueRef nautilus_ir_block_get_argument(NautilusIRBlockRef block, size_t index) {
	return guarded<NautilusIRValueRef>(nullptr, [&] {
		require(block != nullptr, "block is NULL");
		const auto& arguments = unwrap(block)->getArguments();
		require(index < arguments.size(), "block argument index out of range");
		return wrap(arguments[index]);
	});
}

NautilusIRValueRef nautilus_ir_block_get_terminator(NautilusIRBlockRef block) {
	if (block == nullptr || unwrap(block)->getOperations().empty()) {
		return nullptr;
	}
	auto* last = unwrap(block)->getOperations().back();
	return ir::isTerminatorOp(last->getOperationType()) ? wrap(last) : nullptr;
}

/* ── Values and operations ──────────────────────────────────────────────── */

NautilusIROpKind nautilus_ir_value_get_kind(NautilusIRValueRef value) {
	return value != nullptr ? fromOperationType(unwrap(value)->getOperationType()) : NAUTILUS_IR_OP_UNKNOWN;
}

NautilusIRType nautilus_ir_value_get_type(NautilusIRValueRef value) {
	return value != nullptr ? fromType(unwrap(value)->getStamp()) : NAUTILUS_IR_TYPE_VOID;
}

uint32_t nautilus_ir_value_get_id(NautilusIRValueRef value) {
	return value != nullptr ? unwrap(value)->getIdentifier().getId() : 0;
}

bool nautilus_ir_value_is_terminator(NautilusIRValueRef value) {
	return value != nullptr && ir::isTerminatorOp(unwrap(value)->getOperationType());
}

size_t nautilus_ir_value_get_operands(NautilusIRValueRef value, NautilusIRValueRef* out, size_t capacity) {
	if (value == nullptr) {
		return 0;
	}
	return copyOut(unwrap(value)->getInputs(), out, capacity, [](const ir::Operation* op) { return wrap(op); });
}

size_t nautilus_ir_value_get_successors(NautilusIRValueRef value, NautilusIRBlockRef* out, size_t capacity) {
	if (value == nullptr) {
		return 0;
	}
	const auto* op = unwrap(value);
	const size_t count = successorCount(op);
	if (out != nullptr) {
		for (size_t i = 0; i < std::min(count, capacity); ++i) {
			out[i] = wrap(successorAt(op, i)->getBlock());
		}
	}
	return count;
}

size_t nautilus_ir_value_get_successor_arguments(NautilusIRValueRef value, size_t successor, NautilusIRValueRef* out,
                                                 size_t capacity) {
	return guarded<size_t>(0, [&] {
		require(value != nullptr, "value is NULL");
		const auto* invocation = successorAt(unwrap(value), successor);
		require(invocation != nullptr, "no such successor");
		return copyOut(invocation->getArguments(), out, capacity, [](const ir::Operation* op) { return wrap(op); });
	});
}

NautilusStatus nautilus_ir_value_get_const_int(NautilusIRValueRef value, int64_t* out) {
	return detail<ir::ConstIntOperation>(value, out, [](const ir::ConstIntOperation* op) { return op->getValue(); });
}

NautilusStatus nautilus_ir_value_get_const_float(NautilusIRValueRef value, double* out) {
	return detail<ir::ConstFloatOperation>(value, out,
	                                       [](const ir::ConstFloatOperation* op) { return op->getValue(); });
}

NautilusStatus nautilus_ir_value_get_const_bool(NautilusIRValueRef value, bool* out) {
	return detail<ir::ConstBooleanOperation>(value, out,
	                                         [](const ir::ConstBooleanOperation* op) { return op->getValue(); });
}

NautilusStatus nautilus_ir_value_get_const_ptr(NautilusIRValueRef value, void** out) {
	return detail<ir::ConstPtrOperation>(value, out, [](const ir::ConstPtrOperation* op) { return op->getValue(); });
}

NautilusStatus nautilus_ir_value_get_comparator(NautilusIRValueRef value, NautilusIRComparator* out) {
	return detail<ir::CompareOperation>(value, out, [](const ir::CompareOperation* op) -> NautilusIRComparator {
		switch (op->getComparator()) {
		case ir::CompareOperation::EQ:
			return NAUTILUS_IR_CMP_EQ;
		case ir::CompareOperation::NE:
			return NAUTILUS_IR_CMP_NE;
		case ir::CompareOperation::LT:
			return NAUTILUS_IR_CMP_LT;
		case ir::CompareOperation::LE:
			return NAUTILUS_IR_CMP_LE;
		case ir::CompareOperation::GT:
			return NAUTILUS_IR_CMP_GT;
		case ir::CompareOperation::GE:
			return NAUTILUS_IR_CMP_GE;
		}
		throw ApiError(NAUTILUS_ERROR_INTERNAL, "unknown comparator");
	});
}

NautilusStatus nautilus_ir_value_get_bitwise_kind(NautilusIRValueRef value, NautilusIRBitwiseKind* out) {
	return detail<ir::BinaryCompOperation>(value, out, [](const ir::BinaryCompOperation* op) -> NautilusIRBitwiseKind {
		switch (op->getType()) {
		case ir::BinaryCompOperation::BAND:
			return NAUTILUS_IR_BITWISE_AND;
		case ir::BinaryCompOperation::BOR:
			return NAUTILUS_IR_BITWISE_OR;
		case ir::BinaryCompOperation::XOR:
			return NAUTILUS_IR_BITWISE_XOR;
		}
		throw ApiError(NAUTILUS_ERROR_INTERNAL, "unknown bitwise operation");
	});
}

NautilusStatus nautilus_ir_value_get_shift_kind(NautilusIRValueRef value, NautilusIRShiftKind* out) {
	return detail<ir::ShiftOperation>(value, out, [](const ir::ShiftOperation* op) -> NautilusIRShiftKind {
		return op->getType() == ir::ShiftOperation::LS ? NAUTILUS_IR_SHIFT_LEFT : NAUTILUS_IR_SHIFT_RIGHT;
	});
}

NautilusStatus nautilus_ir_value_get_stack_slot(NautilusIRValueRef value, uint32_t* out) {
	return detail<ir::AllocaOperation>(value, out, [](const ir::AllocaOperation* op) { return op->getIndex(); });
}

NautilusStatus nautilus_ir_value_get_callee(NautilusIRValueRef value, NautilusIRCalleeId* out) {
	return status([&] {
		require(value != nullptr, "value is NULL");
		outParam(out);
		const auto* op = unwrap(value);
		if (const auto* call = ir::dyn_cast<ir::CallOperation>(op)) {
			*out = call->getCalleeId();
		} else if (const auto* address = ir::dyn_cast<ir::FunctionAddressOfOperation>(op)) {
			*out = address->getCalleeId();
		} else {
			throw ApiError(NAUTILUS_ERROR_INVALID_ARGUMENT, "value is neither a call nor a function address");
		}
	});
}

NautilusStatus nautilus_ir_value_get_branch_probability(NautilusIRValueRef value, double* out) {
	return detail<ir::IfOperation>(value, out, [](const ir::IfOperation* op) { return op->getProbability(); });
}

/* ── Optimization and compilation ───────────────────────────────────────── */

bool nautilus_ir_backend_is_available(NautilusStringRef backend) {
	return guarded(false, [&] {
		return nautilus::compiler::CompilationBackendRegistry::getInstance()->hasBackend(toString(backend));
	});
}

NautilusStatus nautilus_ir_graph_optimize(NautilusIRGraphRef graph, NautilusIROptimizationLevel level,
                                          NautilusOptionsRef options) {
	return status([&] {
		require(graph != nullptr, "graph is NULL");
		optimize(graph, toLevel(level, nullptr), optionsOf(options));
	});
}

NautilusExecutableRef nautilus_ir_graph_compile(NautilusIRGraphRef graph, NautilusStringRef backend,
                                                NautilusOptionsRef options) {
	return guarded<NautilusExecutableRef>(nullptr, [&] {
		require(graph != nullptr, "graph is NULL");
		const auto* registry = nautilus::compiler::CompilationBackendRegistry::getInstance();
		auto backendName = toString(backend);
		if (backendName.empty()) {
			backendName = registry->getDefaultBackendName();
		}
		check(registry->hasBackend(backendName), NAUTILUS_ERROR_UNAVAILABLE, "backend is not available in this build");
		const auto* compilationBackend = registry->getBackend(backendName);
		const auto& moduleOptions = optionsOf(options);
		optimize(graph, compilationBackend->irOptimizationLevel(), moduleOptions);
		auto result = std::make_unique<NautilusOpaqueExecutable>();
		result->executable = compiling([&] {
			const auto dumpHandler = nautilus::compiler::DumpHandler(moduleOptions, graph->ir->getId());
			auto executable = compilationBackend->compile(graph->ir, dumpHandler, moduleOptions);
			executable->setGeneratedFiles(dumpHandler.getGeneratedFiles());
			return executable;
		});
		return result.release();
	});
}

} // extern "C"

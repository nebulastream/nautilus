#include "IRGraphFixtures.hpp"
#include "nautilus/compiler/ir/OperationEffects.hpp"
#include "nautilus/compiler/ir/operations/CallOperation.hpp"
#include "nautilus/compiler/ir/operations/ConstPtrOperation.hpp"
#include "nautilus/compiler/ir/passes/DeadCodeEliminationPass.hpp"
#include "nautilus/compiler/ir/passes/FunctionAttributeInferencePass.hpp"
#include <catch2/catch_all.hpp>
#include <span>

namespace nautilus::testing {

namespace {

using compiler::ir::BasicBlock;
using compiler::ir::BasicBlockArgument;
using compiler::ir::BlockIdentifier;
using compiler::ir::FunctionAttributeInferencePass;
using compiler::ir::FunctionOperation;
using compiler::ir::IRGraph;
using compiler::ir::Operation;
using compiler::ir::OperationIdentifier;

/// Wraps @p entry in a function named @p name and registers it with the
/// graph's function table, the way conversion does.
compiler::ir::FunctionId defineFunction(std::shared_ptr<IRGraph>& ir, BasicBlock* entry, const std::string& name,
                                        Type returnStamp) {
	auto& arena = ir->getArena();
	auto* fn = arena.create<FunctionOperation>(
	    name, std::vector<BasicBlock*> {entry}, std::vector<Type> {}, std::vector<std::string> {}, returnStamp,
	    std::vector<compiler::ir::AllocaSpec> {}, std::unordered_map<std::string, std::string> {});
	ir->addFunctionOperation(fn);

	compiler::ir::CalleeDescriptor descriptor;
	descriptor.kind = compiler::ir::CalleeDescriptor::Kind::Internal;
	descriptor.customName = name;
	const auto id = ir->internCallee(descriptor);
	ir->defineFunction(id, fn);
	return id;
}

size_t countOpsOfType(const IRGraph& ir, Operation::OperationType type) {
	size_t count = 0;
	for (const auto* fn : ir.getFunctionOperations()) {
		for (const auto* block : fn->getBasicBlocks()) {
			for (const auto* op : block->getOperations()) {
				if (op != nullptr && op->getOperationType() == type) {
					++count;
				}
			}
		}
	}
	return count;
}

FunctionAttributes attributesOf(IRGraph& ir, compiler::ir::FunctionId id) {
	return ir.getFunctionTarget(id).getAttributes();
}

} // namespace

TEST_CASE("AttributeInference: a function that only computes is NoModRef and returns") {
	auto ir = std::make_shared<IRGraph>("infer-pure");
	auto& arena = ir->getArena();

	auto* entry = arena.create<BasicBlock>(arena, BlockIdentifier {0}, std::vector<BasicBlockArgument*> {});
	auto* c5 = entry->addOperation<compiler::ir::ConstIntOperation>(OperationIdentifier {1}, int64_t {5}, Type::i32);
	auto* c3 = entry->addOperation<compiler::ir::ConstIntOperation>(OperationIdentifier {2}, int64_t {3}, Type::i32);
	auto* add = entry->addOperation<compiler::ir::AddOperation>(OperationIdentifier {3}, c5, c3);
	entry->addOperation<compiler::ir::ReturnOperation>(add);
	const auto id = defineFunction(ir, entry, "pureFn", Type::i32);

	FunctionAttributeInferencePass().apply(*ir);

	const auto attrs = attributesOf(*ir, id);
	// Arithmetic touches no memory, there is no loop, and nothing it calls can
	// unwind -- so every guarantee holds. This is the answer the default
	// FunctionAttributes at each call site could never express.
	REQUIRE(attrs.modRefInfo == ModRefInfo::NoModRef);
	REQUIRE(attrs.willReturn);
	REQUIRE(attrs.noUnwind);
}

TEST_CASE("AttributeInference: a store makes a function Mod") {
	auto ir = std::make_shared<IRGraph>("infer-store");
	auto& arena = ir->getArena();

	auto* entry = arena.create<BasicBlock>(arena, BlockIdentifier {0}, std::vector<BasicBlockArgument*> {});
	auto* value = entry->addOperation<compiler::ir::ConstIntOperation>(OperationIdentifier {1}, int64_t {7}, Type::i32);
	auto* address =
	    entry->addOperation<compiler::ir::ConstPtrOperation>(OperationIdentifier {2}, reinterpret_cast<void*>(0x3000));
	entry->addOperation<compiler::ir::StoreOperation>(value, address);
	entry->addOperation<compiler::ir::ReturnOperation>();
	const auto id = defineFunction(ir, entry, "storeFn", Type::v);

	FunctionAttributeInferencePass().apply(*ir);

	const auto attrs = attributesOf(*ir, id);
	REQUIRE((static_cast<uint8_t>(attrs.modRefInfo) & static_cast<uint8_t>(ModRefInfo::Mod)) != 0);
}

TEST_CASE("AttributeInference: an unused call to an inferred-pure function is eliminated") {
	// The payoff. A caller invokes an in-module function that provably touches
	// no memory and returns, and never uses the result. Before inference the
	// callee declared ModRef/!willReturn at every call site, so the call was
	// immovable and undeletable no matter what its body did.
	auto ir = std::make_shared<IRGraph>("infer-dce");
	auto& arena = ir->getArena();

	// The callee: pure arithmetic.
	auto* calleeBlock = arena.create<BasicBlock>(arena, BlockIdentifier {0}, std::vector<BasicBlockArgument*> {});
	auto* k =
	    calleeBlock->addOperation<compiler::ir::ConstIntOperation>(OperationIdentifier {1}, int64_t {2}, Type::i32);
	calleeBlock->addOperation<compiler::ir::ReturnOperation>(k);
	const auto calleeId = defineFunction(ir, calleeBlock, "pureCallee", Type::i32);

	// The caller: calls it and drops the result.
	auto* callerBlock = arena.create<BasicBlock>(arena, BlockIdentifier {0}, std::vector<BasicBlockArgument*> {});
	auto* callOp = callerBlock->addOperation<compiler::ir::CallOperation>(
	    "pureCallee", "pureCallee", nullptr, OperationIdentifier {10}, std::span<compiler::ir::Operation* const> {},
	    Type::i32, FunctionAttributes {}, calleeId);
	callerBlock->addOperation<compiler::ir::ReturnOperation>();
	defineFunction(ir, callerBlock, "caller", Type::v);

	REQUIRE(countOpsOfType(*ir, Operation::OperationType::CallOp) == 1);

	FunctionAttributeInferencePass().apply(*ir);
	const auto calleeAttrs = attributesOf(*ir, calleeId);
	REQUIRE(calleeAttrs.modRefInfo == ModRefInfo::NoModRef);
	REQUIRE(calleeAttrs.willReturn);
	REQUIRE(calleeAttrs.noUnwind);
	// The effects query must agree before DCE can act on it.
	REQUIRE(compiler::ir::isPureOperation(*ir, *callOp));

	compiler::ir::DeadCodeEliminationPass().apply(*ir);
	REQUIRE(countOpsOfType(*ir, Operation::OperationType::CallOp) == 0);
}

TEST_CASE("AttributeInference: a self-recursive function stays pessimistic") {
	// Deliberately conservative: a fixed point over the cycle would do better,
	// but being wrong here would let a caller drop a call that never returns.
	auto ir = std::make_shared<IRGraph>("infer-recursive");
	auto& arena = ir->getArena();

	auto* block = arena.create<BasicBlock>(arena, BlockIdentifier {0}, std::vector<BasicBlockArgument*> {});
	const auto id = defineFunction(ir, block, "recursive", Type::v);

	block->addOperation<compiler::ir::CallOperation>("recursive", "recursive", nullptr, OperationIdentifier {1},
	                                                 std::span<compiler::ir::Operation* const> {}, Type::v,
	                                                 FunctionAttributes {}, id);
	block->addOperation<compiler::ir::ReturnOperation>();

	FunctionAttributeInferencePass().apply(*ir);

	const auto attrs = attributesOf(*ir, id);
	REQUIRE(attrs.modRefInfo == ModRefInfo::ModRef);
	REQUIRE_FALSE(attrs.willReturn);
}

TEST_CASE("AttributeInference: a native callee's own call-site attributes are not discarded") {
	// FunctionTable::intern keeps only the *first* descriptor for a given
	// native address -- a second call site's own FunctionAttributes never
	// reach the table entry. If inference read the table alone, whichever
	// call site happened to intern the address first would silently decide
	// what every other caller believes about it.
	//
	// Simulate that: the table entry for `sideEffecting` carries an
	// optimistic (and wrong) NoModRef, as if some other, unmodelled call site
	// had interned it that way first. This call site's own attributes tell
	// the truth -- it writes memory.
	auto ir = std::make_shared<IRGraph>("infer-native-call-site");
	auto& arena = ir->getArena();

	compiler::ir::CalleeDescriptor descriptor;
	descriptor.kind = compiler::ir::CalleeDescriptor::Kind::External;
	descriptor.key = reinterpret_cast<void*>(0x1234);
	descriptor.demangledName = "sideEffecting";
	descriptor.resultType = Type::v;
	descriptor.attrs = FunctionAttributes {.modRefInfo = ModRefInfo::NoModRef, .willReturn = true, .noUnwind = true};
	const auto calleeId = ir->internCallee(descriptor);

	auto* block = arena.create<BasicBlock>(arena, BlockIdentifier {0}, std::vector<BasicBlockArgument*> {});
	block->addOperation<compiler::ir::CallOperation>(
	    "sideEffecting", "sideEffecting", nullptr, OperationIdentifier {1},
	    std::span<compiler::ir::Operation* const> {}, Type::v,
	    FunctionAttributes {.modRefInfo = ModRefInfo::Mod, .willReturn = true, .noUnwind = true}, calleeId);
	block->addOperation<compiler::ir::ReturnOperation>();
	const auto callerId = defineFunction(ir, block, "caller", Type::v);

	FunctionAttributeInferencePass().apply(*ir);

	// The caller absorbs what this call site actually declared: it writes.
	// Missing that would let DeadCodeEliminationPass drop an unused call to
	// `caller` believing it has no effect.
	const auto attrs = attributesOf(*ir, callerId);
	REQUIRE((static_cast<uint8_t>(attrs.modRefInfo) & static_cast<uint8_t>(ModRefInfo::Mod)) != 0);
}

} // namespace nautilus::testing

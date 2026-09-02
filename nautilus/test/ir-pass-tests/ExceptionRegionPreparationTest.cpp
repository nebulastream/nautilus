#include "nautilus/common/FunctionAttributes.hpp"
#include "nautilus/compiler/ir/blocks/BasicBlock.hpp"
#include "nautilus/compiler/ir/blocks/BasicBlockArgument.hpp"
#include "nautilus/compiler/ir/operations/ConstPtrOperation.hpp"
#include "nautilus/compiler/ir/operations/FunctionOperation.hpp"
#include "nautilus/compiler/ir/operations/IndirectCallOperation.hpp"
#include "nautilus/compiler/ir/operations/CallOperation.hpp"
#include "nautilus/compiler/ir/operations/ReturnOperation.hpp"
#include "nautilus/compiler/ir/passes/ExceptionRegionPreparationPass.hpp"
#include "nautilus/compiler/ir/passes/IRPassManager.hpp"
#include "nautilus/options.hpp"
#include <catch2/catch_all.hpp>
#include <span>
#include <vector>

namespace nautilus::testing {

namespace {

using compiler::ir::BasicBlock;
using compiler::ir::BasicBlockArgument;
using compiler::ir::BlockIdentifier;
using compiler::ir::FunctionOperation;
using compiler::ir::IRGraph;
using compiler::ir::Operation;
using compiler::ir::OperationIdentifier;
using compiler::ir::CallOperation;

struct Fixture {
	std::shared_ptr<IRGraph> ir;
	FunctionOperation* fn;
	BasicBlock* entry;
};

Fixture makeFixture(const std::string& name = "exception-test") {
	auto ir = std::make_shared<IRGraph>(name);
	auto& arena = ir->getArena();
	auto* entry = arena.create<BasicBlock>(arena, BlockIdentifier {0}, std::vector<BasicBlockArgument*> {});
	auto* fn = arena.create<FunctionOperation>("execute", std::vector<BasicBlock*> {entry}, std::vector<Type> {},
	                                           std::vector<std::string> {}, Type::v);
	ir->addFunctionOperation(fn);
	return {ir, fn, entry};
}

void runPass(IRGraph& ir) {
	engine::Options opts;
	compiler::ir::IRPassManager mgr(opts);
	mgr.addPass(std::make_unique<compiler::ir::ExceptionRegionPreparationPass>());
	mgr.run(ir);
}

CallOperation::Destructor makeDestructor(Operation* address) {
	return {address, "dtorSym", "dtorName", nullptr};
}

CallOperation* addCall(BasicBlock* block, OperationIdentifier id, FunctionAttributes attrs,
                       std::vector<CallOperation::Destructor> destructors, bool exceptionHandling) {
	return block->addOperation<CallOperation>("sym", "name", nullptr, id, std::span<Operation* const> {}, Type::v,
	                                          attrs, compiler::ir::INVALID_FUNCTION_ID, std::move(destructors),
	                                          exceptionHandling);
}

const compiler::ir::FunctionExceptionRegion& regionOf(const FunctionOperation& fn) {
	REQUIRE(fn.exceptionRegion.has_value());
	return *fn.exceptionRegion;
}

} // namespace

TEST_CASE("ExceptionRegionPreparation: one alloc, one throwing call -> one pad with one dtor") {
	Fixture f = makeFixture();

	auto* addr = f.entry->addOperation<compiler::ir::ConstPtrOperation>(OperationIdentifier {1}, nullptr);
	auto* call = addCall(f.entry, OperationIdentifier {2}, FunctionAttributes {}, {makeDestructor(addr)}, true);
	f.entry->addOperation<compiler::ir::ReturnOperation>();

	runPass(*f.ir);

	const auto& region = regionOf(*f.fn);
	REQUIRE(region.pads.size() == 1);
	REQUIRE(region.pads[0].block != nullptr);
	REQUIRE(region.pads[0].block->getOperations().size() == 1);
	REQUIRE(region.callSites.size() == 1);
	REQUIRE(region.callSites[0].call == call);
	REQUIRE(region.callSites[0].padIndex == 0);

	auto* dtorCall = compiler::ir::dyn_cast<CallOperation>(region.pads[0].block->getOperations().front());
	REQUIRE(dtorCall != nullptr);
	REQUIRE(dtorCall->getInputArguments().size() == 1);
	REQUIRE(dtorCall->getInputArguments()[0] == addr);
	REQUIRE(dtorCall->getFunctionSymbol() == "dtorSym");
	REQUIRE(dtorCall->getFunctionAttributes().noUnwind);
	REQUIRE_FALSE(dtorCall->requiresExceptionHandling());
}

TEST_CASE("ExceptionRegionPreparation: two allocs -> pad has destructors in reverse order") {
	Fixture f = makeFixture();

	auto* addrA = f.entry->addOperation<compiler::ir::ConstPtrOperation>(OperationIdentifier {1}, nullptr);
	auto* addrB = f.entry->addOperation<compiler::ir::ConstPtrOperation>(OperationIdentifier {2}, nullptr);
	std::vector<CallOperation::Destructor> dtors {makeDestructor(addrA), makeDestructor(addrB)};
	addCall(f.entry, OperationIdentifier {3}, FunctionAttributes {}, std::move(dtors), true);
	f.entry->addOperation<compiler::ir::ReturnOperation>();

	runPass(*f.ir);

	const auto& region = regionOf(*f.fn);
	REQUIRE(region.pads.size() == 1);
	REQUIRE(region.callSites.size() == 1);
	auto* pad = region.pads[0].block;
	REQUIRE(pad->getOperations().size() == 2);
	auto* first = compiler::ir::dyn_cast<CallOperation>(pad->getOperations()[0]);
	auto* second = compiler::ir::dyn_cast<CallOperation>(pad->getOperations()[1]);
	REQUIRE(first != nullptr);
	REQUIRE(second != nullptr);
	REQUIRE(first->getInputArguments()[0] == addrB);
	REQUIRE(second->getInputArguments()[0] == addrA);
}

TEST_CASE("ExceptionRegionPreparation: two calls with same dtor set share a pad") {
	Fixture f = makeFixture();

	auto* addr = f.entry->addOperation<compiler::ir::ConstPtrOperation>(OperationIdentifier {1}, nullptr);
	auto* call1 = addCall(f.entry, OperationIdentifier {2}, FunctionAttributes {}, {makeDestructor(addr)}, true);
	auto* call2 = addCall(f.entry, OperationIdentifier {3}, FunctionAttributes {}, {makeDestructor(addr)}, true);
	f.entry->addOperation<compiler::ir::ReturnOperation>();

	runPass(*f.ir);

	const auto& region = regionOf(*f.fn);
	REQUIRE(region.pads.size() == 1);
	REQUIRE(region.callSites.size() == 2);
	REQUIRE(region.callSites[0].call == call1);
	REQUIRE(region.callSites[1].call == call2);
	REQUIRE(region.callSites[0].padIndex == 0);
	REQUIRE(region.callSites[1].padIndex == 0);
}

TEST_CASE("ExceptionRegionPreparation: two calls with different dtor sets use different pads") {
	Fixture f = makeFixture();

	auto* addrA = f.entry->addOperation<compiler::ir::ConstPtrOperation>(OperationIdentifier {1}, nullptr);
	auto* addrB = f.entry->addOperation<compiler::ir::ConstPtrOperation>(OperationIdentifier {2}, nullptr);
	auto* call1 = addCall(f.entry, OperationIdentifier {3}, FunctionAttributes {}, {makeDestructor(addrA)}, true);
	auto* call2 = addCall(f.entry, OperationIdentifier {4}, FunctionAttributes {}, {makeDestructor(addrB)}, true);
	f.entry->addOperation<compiler::ir::ReturnOperation>();

	runPass(*f.ir);

	const auto& region = regionOf(*f.fn);
	REQUIRE(region.pads.size() == 2);
	REQUIRE(region.callSites.size() == 2);
	REQUIRE(region.callSites[0].call == call1);
	REQUIRE(region.callSites[1].call == call2);
	REQUIRE(region.callSites[0].padIndex == 0);
	REQUIRE(region.callSites[1].padIndex == 1);
	REQUIRE(region.callSites[0].padIndex != region.callSites[1].padIndex);
}

TEST_CASE("ExceptionRegionPreparation: throwing call without destructors maps to a null pad") {
	Fixture f = makeFixture();

	auto* call = addCall(f.entry, OperationIdentifier {2}, FunctionAttributes {}, {}, false);
	f.entry->addOperation<compiler::ir::ReturnOperation>();

	runPass(*f.ir);

	const auto& region = regionOf(*f.fn);
	REQUIRE(region.pads.size() == 0);
	REQUIRE(region.callSites.size() == 1);
	REQUIRE(region.callSites[0].call == call);
	REQUIRE_FALSE(region.callSites[0].hasPad());
}

TEST_CASE("ExceptionRegionPreparation: noUnwind call is absent from the region") {
	Fixture f = makeFixture();

	FunctionAttributes attrs;
	attrs.noUnwind = true;
	addCall(f.entry, OperationIdentifier {2}, attrs, {}, false);
	f.entry->addOperation<compiler::ir::ReturnOperation>();

	runPass(*f.ir);

	const auto& region = regionOf(*f.fn);
	REQUIRE(region.pads.size() == 0);
	REQUIRE(region.callSites.size() == 0);
}

TEST_CASE("ExceptionRegionPreparation: a call not in the CFG is absent from the region") {
	Fixture f = makeFixture();
	auto& arena = f.ir->getArena();

	// Arena-allocated but never added to any basic block: unreachable.
	auto* addr = arena.create<compiler::ir::ConstPtrOperation>(arena, OperationIdentifier {99}, nullptr);
	arena.create<CallOperation>(arena, "sym", "name", nullptr, OperationIdentifier {100},
	                            std::span<Operation* const> {}, Type::v, FunctionAttributes {},
	                            compiler::ir::INVALID_FUNCTION_ID,
	                            std::vector<CallOperation::Destructor> {makeDestructor(addr)}, true);
	f.entry->addOperation<compiler::ir::ReturnOperation>();

	runPass(*f.ir);

	const auto& region = regionOf(*f.fn);
	REQUIRE(region.pads.size() == 0);
	REQUIRE(region.callSites.size() == 0);
}

TEST_CASE("ExceptionRegionPreparation: empty function produces an empty region") {
	Fixture f = makeFixture();
	f.entry->addOperation<compiler::ir::ReturnOperation>();

	runPass(*f.ir);

	REQUIRE(f.fn->exceptionRegion.has_value());
	REQUIRE(f.fn->exceptionRegion->pads.empty());
	REQUIRE(f.fn->exceptionRegion->callSites.empty());
}

TEST_CASE("ExceptionRegionPreparation: pad identifiers do not collide with main-CFG ids") {
	Fixture f = makeFixture();

	auto* addr = f.entry->addOperation<compiler::ir::ConstPtrOperation>(OperationIdentifier {7}, nullptr);
	addCall(f.entry, OperationIdentifier {8}, FunctionAttributes {}, {makeDestructor(addr)}, true);
	f.entry->addOperation<compiler::ir::ReturnOperation>();

	runPass(*f.ir);

	const auto& region = regionOf(*f.fn);
	REQUIRE(region.pads.size() == 1);
	auto* pad = region.pads[0].block;

	std::vector<uint32_t> blockIds;
	std::vector<uint32_t> opIds;
	for (auto* block : f.fn->getBasicBlocks()) {
		blockIds.push_back(block->getIdentifier().getId());
		for (auto* op : block->getOperations()) {
			opIds.push_back(op->getIdentifier().getId());
		}
	}

	const auto padBlockId = pad->getIdentifier().getId();
	REQUIRE(std::find(blockIds.begin(), blockIds.end(), padBlockId) == blockIds.end());
	for (auto* op : pad->getOperations()) {
		const auto opId = op->getIdentifier().getId();
		REQUIRE(std::find(opIds.begin(), opIds.end(), opId) == opIds.end());
	}
}

TEST_CASE("ExceptionRegionPreparation: is idempotent") {
	Fixture f = makeFixture();

	auto* addr = f.entry->addOperation<compiler::ir::ConstPtrOperation>(OperationIdentifier {1}, nullptr);
	addCall(f.entry, OperationIdentifier {2}, FunctionAttributes {}, {makeDestructor(addr)}, true);
	f.entry->addOperation<compiler::ir::ReturnOperation>();

	runPass(*f.ir);
	const auto& first = regionOf(*f.fn);
	const size_t padsAfterFirst = first.pads.size();
	const size_t sitesAfterFirst = first.callSites.size();
	auto* padAfterFirst = first.pads[0].block;

	runPass(*f.ir);
	const auto& second = regionOf(*f.fn);
	REQUIRE(second.pads.size() == padsAfterFirst);
	REQUIRE(second.callSites.size() == sitesAfterFirst);
	REQUIRE(second.pads[0].block == padAfterFirst);
	REQUIRE(second.pads[0].block->getOperations().size() == 1);
}

TEST_CASE("ExceptionRegionPreparation: indirect call with a destructor is collected") {
	Fixture f = makeFixture();

	auto* ptrOp = f.entry->addOperation<compiler::ir::ConstPtrOperation>(OperationIdentifier {1}, nullptr);
	auto* addr = f.entry->addOperation<compiler::ir::ConstPtrOperation>(OperationIdentifier {2}, nullptr);
	auto* call = f.entry->addOperation<compiler::ir::IndirectCallOperation>(
	    OperationIdentifier {3}, ptrOp, std::span<Operation* const> {}, Type::v, FunctionAttributes {},
	    std::vector<compiler::ir::IndirectCallOperation::Destructor> {
	        compiler::ir::IndirectCallOperation::Destructor {addr, "dtorSym", "dtorName", nullptr}},
	    true);
	f.entry->addOperation<compiler::ir::ReturnOperation>();

	runPass(*f.ir);

	const auto& region = regionOf(*f.fn);
	REQUIRE(region.pads.size() == 1);
	REQUIRE(region.callSites.size() == 1);
	REQUIRE(region.callSites[0].call == call);
	REQUIRE(region.callSites[0].padIndex == 0);
	auto* dtorCall = compiler::ir::dyn_cast<CallOperation>(region.pads[0].block->getOperations().front());
	REQUIRE(dtorCall != nullptr);
	REQUIRE(dtorCall->getInputArguments()[0] == addr);
}

} // namespace nautilus::testing

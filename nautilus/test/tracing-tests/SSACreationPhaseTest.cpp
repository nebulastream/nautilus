#include "nautilus/tracing/ExecutionTrace.hpp"
#include "nautilus/tracing/phases/SSACreationPhase.hpp"
#include "nautilus/tracing/phases/SSAVerifier.hpp"
#include <catch2/catch_test_macros.hpp>

namespace nautilus::tracing {

TEST_CASE("SSA creation propagates many live-in values without duplicates", "[SSA]") {
	constexpr size_t valueCount = 64;
	common::Arena arena;
	auto trace = std::make_shared<ExecutionTrace>(arena);
	std::vector<TypedValueRef> arguments;
	arguments.reserve(valueCount);
	for (size_t index = 0; index < valueCount; ++index) {
		arguments.push_back(trace->setArgument(Type::i64, index));
	}

	auto& root = trace->getBlock(0);
	const auto useBlockId = trace->createBlock();
	auto* target = arena.create<BlockRef>(useBlockId);
	root.addOperation(makeTraceOp(arena, Op::JMP, target));
	auto& useBlock = trace->getBlock(useBlockId);
	useBlock.predecessors.emplace_back(root.blockId);
	trace->setCurrentBlock(useBlockId);

	auto* call = arena.create<FunctionCall>(FunctionCall {
	    .functionName = "consume", .mangledName = "consume", .ptr = nullptr, .arguments = arguments, .fnAttrs = {}});
	auto snapshot = Snapshot();
	auto callOp = Op::CALL;
	auto resultType = Type::i64;
	auto result = trace->addOperationWithResult(snapshot, callOp, resultType, {call});
	trace->addReturn(snapshot, resultType, result);

	auto afterSsa = SSACreationPhase().apply(std::move(trace));
	REQUIRE(afterSsa->getBlock(0).arguments.size() == valueCount);
	REQUIRE(afterSsa->getBlock(useBlockId).arguments.size() == valueCount);
	REQUIRE(target->arguments.size() == valueCount);
	REQUIRE(VerifySSA(*afterSsa).valid);
}

} // namespace nautilus::tracing

#include "nautilus/region.hpp"
#include "nautilus/tracing/TraceContext.hpp"
#include "nautilus/val.hpp"
#include <array>
#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <initializer_list>
#include <stdexcept>
#include <vector>

namespace nautilus::tracing {
namespace {

struct ExpectedConstant {
	int64_t value;
	ConstantOrigin origin;
	RegionIndex regionIndex = NO_REGION;
};

void requireConstants(ExecutionTrace& trace, std::initializer_list<ExpectedConstant> expected) {
	auto next = expected.begin();
	for (const auto* block : trace.getBlocks()) {
		for (const auto* operation : block->operations) {
			if (operation->op != Op::CONST) {
				REQUIRE(operation->constantOrigin == ConstantOrigin::Unspecified);
				continue;
			}
			REQUIRE(next != expected.end());
			CAPTURE(next->value, next->regionIndex);
			REQUIRE(operation->resultType == Type::i64);
			REQUIRE(operation->input.size() == 1);
			REQUIRE(std::get<ConstantLiteral>(operation->input[0]) == ConstantLiteral {next->value});
			REQUIRE(operation->constantOrigin ==
			        (trace.recordsConstantOrigins() ? next->origin : ConstantOrigin::Unspecified));
			REQUIRE(operation->regionIndex == next->regionIndex);
			++next;
		}
	}
	REQUIRE(next == expected.end());
}

} // namespace

TEST_CASE("Constant origin recording is disabled by default", "[TraceContext][ExecutionTrace]") {
	common::Arena arena;
	{
		ExecutionTrace trace(arena);
		REQUIRE_FALSE(trace.recordsConstantOrigins());
	}
	arena.softReset();

	std::function<void()> wrapper = [] {
		val<int64_t> ordinary(11);
		auto invariant = cacheInvariant(int64_t {7});
		auto literal = cacheLiteral<int64_t {19}>();
		val<int64_t> zero;
		auto result = ordinary + invariant + literal + zero;
		traceReturnOperation(Type::i64, result.state);
	};
	auto trace = TraceContext::trace(wrapper, engine::Options {}, arena);
	REQUIRE(trace != nullptr);
	REQUIRE_FALSE(inTracer());
	REQUIRE_FALSE(trace->recordsConstantOrigins());
	REQUIRE(trace->getBlocks().size() == 1);
	requireConstants(*trace, {{11, ConstantOrigin::Unspecified},
	                          {7, ConstantOrigin::CacheInvariant},
	                          {19, ConstantOrigin::CacheInvariant},
	                          {0, ConstantOrigin::CacheInvariant}});
}

TEST_CASE("ExecutionTrace result insertion resets reused origins and preserves metadata",
          "[TraceContext][ExecutionTrace]") {
	for (const auto origin : {ConstantOrigin::Unspecified, ConstantOrigin::CacheInvariant}) {
		CAPTURE(origin);
		common::Arena arena;
		Tag ordinaryTag, explicitTag, sumTag;
		Snapshot ordinarySnapshot(&ordinaryTag, 0), explicitSnapshot(&explicitTag, 0), sumSnapshot(&sumTag, 0);
		auto op = Op::CONST;
		auto type = Type::i64;
		const ConstantLiteral ordinaryLiteral {int64_t {11}};
		const ConstantLiteral explicitLiteral {int64_t {7}};
		std::array<TraceOperation*, 2> dirtyOperations {};
		{
			ExecutionTrace dirtyTrace(arena, ConstantOriginTracking::Enabled);
			REQUIRE(dirtyTrace.recordsConstantOrigins());
			const auto region = dirtyTrace.addRegion(RegionAttributes {.name = "dirty origin storage"}, 0);
			dirtyTrace.setCurrentRegion(region);
			dirtyTrace.addOperationWithResult(ordinarySnapshot, op, type, {ordinaryLiteral},
			                                  ConstantOrigin::CacheInvariant);
			dirtyTrace.addOperationWithResult(explicitSnapshot, op, type, {explicitLiteral},
			                                  ConstantOrigin::CacheInvariant);
			for (std::size_t index = 0; index < dirtyOperations.size(); ++index) {
				dirtyOperations[index] = dirtyTrace.getBlock(0).operations[index];
				REQUIRE(dirtyOperations[index]->constantOrigin == ConstantOrigin::CacheInvariant);
				REQUIRE(dirtyOperations[index]->regionIndex == region);
			}
		}
		arena.softReset();

		ExecutionTrace trace(arena, ConstantOriginTracking::Enabled);
		REQUIRE(trace.recordsConstantOrigins());
		auto& ordinary = trace.addOperationWithResult(ordinarySnapshot, op, type, {ordinaryLiteral});
		const auto region = trace.addRegion(RegionAttributes {.name = "explicit origin"}, 0);
		REQUIRE(trace.setCurrentRegion(region) == NO_REGION);
		auto& explicitResult = trace.addOperationWithResult(explicitSnapshot, op, type, {explicitLiteral}, origin);
		REQUIRE(trace.setCurrentRegion(NO_REGION) == region);
		auto sumOp = Op::ADD;
		auto& sum = trace.addOperationWithResult(sumSnapshot, sumOp, type, {ordinary, explicitResult});

		const auto& operations = trace.getBlock(0).operations;
		REQUIRE(operations.size() == 3);
		REQUIRE(operations[0] == dirtyOperations[0]);
		REQUIRE(operations[1] == dirtyOperations[1]);
		REQUIRE(trace.globalTagMap.size() == 3);
		REQUIRE(trace.getRegions().size() == 1);
		REQUIRE(trace.getRegions()[region].parent == NO_REGION);
		REQUIRE(trace.getCurrentBlockIndex() == 0);
		REQUIRE(trace.currentOperationIndex == 2);
		REQUIRE(trace.lastValueRef == 3);

		const auto requireInsertion = [&](std::size_t index, const TypedValueRef& result, const Snapshot& snapshot,
		                                  Op expectedOp, ConstantOrigin expectedOrigin, RegionIndex expectedRegion) {
			const auto* operation = operations[index];
			REQUIRE(&result == &operation->resultRef);
			REQUIRE(result.ref == index + 1);
			REQUIRE(result.type == type);
			REQUIRE(operation->resultType == type);
			REQUIRE(operation->op == expectedOp);
			REQUIRE(operation->tag == snapshot);
			REQUIRE(operation->constantOrigin == expectedOrigin);
			REQUIRE(operation->regionIndex == expectedRegion);
			REQUIRE(trace.globalTagMap.at(snapshot).blockIndex == 0);
			REQUIRE(trace.globalTagMap.at(snapshot).operationIndex == index);
		};
		requireInsertion(0, ordinary, ordinarySnapshot, Op::CONST, ConstantOrigin::Unspecified, NO_REGION);
		requireInsertion(1, explicitResult, explicitSnapshot, Op::CONST, origin, region);
		requireInsertion(2, sum, sumSnapshot, Op::ADD, ConstantOrigin::Unspecified, NO_REGION);
		REQUIRE(std::get<ConstantLiteral>(operations[0]->input[0]) == ordinaryLiteral);
		REQUIRE(std::get<ConstantLiteral>(operations[1]->input[0]) == explicitLiteral);
		REQUIRE(std::get<TypedValueRef>(operations[2]->input[0]) == ordinary);
		REQUIRE(std::get<TypedValueRef>(operations[2]->input[1]) == explicitResult);
	}
}

TEST_CASE("TraceContext follows constants across reconciliation runs and jumps", "[TraceContext]") {
	const auto tracking = GENERATE(ConstantOriginTracking::Disabled, ConstantOriginTracking::Enabled);
	CAPTURE(tracking);
	const auto recordedOrigin =
	    tracking == ConstantOriginTracking::Enabled ? ConstantOrigin::CacheInvariant : ConstantOrigin::Unspecified;
	for (const std::size_t reconciliationCount : {0, 1, 3}) {
		for (const bool initialJump : {false, true}) {
			for (const bool crossBlockJump : {false, true}) {
				CAPTURE(reconciliationCount, initialJump, crossBlockJump);
				common::Arena arena;
				ExecutionTrace trace(arena, tracking);
				REQUIRE(trace.recordsConstantOrigins() == (tracking == ConstantOriginTracking::Enabled));
				Tag primaryTag, nextTag, returnTag, initialJumpTag, crossBlockJumpTag;
				Snapshot primarySnapshot(&primaryTag, 0), nextSnapshot(&nextTag, 0), returnSnapshot(&returnTag, 0);
				Snapshot initialJumpSnapshot(&initialJumpTag, 0), crossBlockJumpSnapshot(&crossBlockJumpTag, 0);
				if (initialJump) {
					const auto primaryBlock = trace.createBlock();
					trace.addJumpOperation(initialJumpSnapshot, primaryBlock);
					trace.getBlock(primaryBlock).predecessors.push_back(0);
					trace.setCurrentBlock(primaryBlock);
				}
				auto op = Op::CONST;
				auto type = Type::i64;
				const ConstantLiteral primaryLiteral {int64_t {7}};
				const ConstantLiteral nextLiteral {int64_t {19}};
				auto& original =
				    trace.addOperationWithResult(primarySnapshot, op, type, {primaryLiteral}, recordedOrigin);
				auto* primaryOperation = trace.getCurrentBlock().operations.back();
				const auto appendReconciliations = [&](std::size_t count) {
					for (std::size_t index = 0; index < count; ++index) {
						const TypedValueRef target(trace.getNextValueRef(), type);
						trace.addAssignmentOperation(primarySnapshot, target, original, type);
					}
				};
				const auto beforeJump = crossBlockJump ? reconciliationCount / 2 : reconciliationCount;
				appendReconciliations(beforeJump);
				if (crossBlockJump) {
					const auto successor = trace.createBlock();
					trace.getBlock(successor).predecessors.push_back(trace.getCurrentBlockIndex());
					trace.addJumpOperation(crossBlockJumpSnapshot, successor);
					trace.setCurrentBlock(successor);
				}
				appendReconciliations(reconciliationCount - beforeJump);
				auto& next = trace.addOperationWithResult(nextSnapshot, op, type, {nextLiteral}, recordedOrigin);
				auto* nextOperation = trace.getCurrentBlock().operations.back();
				const auto nextBlock = trace.getCurrentBlockIndex();
				const auto nextIndex = trace.currentOperationIndex;
				trace.addReturn(returnSnapshot, type, next);
				auto* returnOperation = trace.getCurrentBlock().operations.back();
				const auto lastValueRef = trace.lastValueRef;

				SymbolicExecutionContext symbolic;
				symbolic.next();
				const auto branch = symbolic.record(returnSnapshot);
				REQUIRE(branch.branchDirection);
				REQUIRE_FALSE(branch.shouldTerminate);
				symbolic.next();
				REQUIRE(symbolic.getCurrentMode() == SymbolicExecutionContext::MODE::FOLLOW);
				TagRecorder recorder(reinterpret_cast<TagAddress>(__builtin_return_address(0)), arena);
				engine::Options options;
				{
					ActiveTracerGuard guard;
					auto* context = TraceContext::initialize(recorder, trace, symbolic, options);
					constexpr std::array origins {ConstantOrigin::CacheInvariant, ConstantOrigin::Unspecified,
					                              ConstantOrigin::CacheInvariant};
					for (std::size_t replay = 0; replay < origins.size(); ++replay) {
						CAPTURE(replay);
						trace.resetExecution();
						context->resume();
						REQUIRE(trace.getCurrentBlockIndex() == 0);
						REQUIRE(trace.currentOperationIndex == 0);
						REQUIRE(trace.getBlock(0).operations[0]->op == (initialJump ? Op::JMP : Op::CONST));
						auto& followed = traceConstant(type, primaryLiteral, origins[replay]);
						REQUIRE(&followed == &original);
						REQUIRE(followed.type == type);
						REQUIRE(primaryOperation->constantOrigin ==
						        (replay == 0 ? recordedOrigin : ConstantOrigin::Unspecified));
						REQUIRE(trace.getCurrentBlockIndex() == nextBlock);
						REQUIRE(trace.currentOperationIndex == nextIndex);
						REQUIRE(&trace.getCurrentOperation() == nextOperation);
						auto& followedNext = traceConstant(type, nextLiteral, ConstantOrigin::CacheInvariant);
						REQUIRE(&followedNext == &next);
						REQUIRE(followedNext.type == type);
						REQUIRE(nextOperation->constantOrigin == recordedOrigin);
						REQUIRE(trace.getCurrentBlockIndex() == nextBlock);
						REQUIRE(trace.currentOperationIndex == nextIndex + 1);
						REQUIRE(&trace.getCurrentOperation() == returnOperation);
						REQUIRE(trace.lastValueRef == lastValueRef);
					}
				}
				REQUIRE_FALSE(inTracer());
				for (const auto* block : trace.getBlocks()) {
					for (const auto* operation : block->operations) {
						REQUIRE(operation->constantOrigin ==
						        (operation == nextOperation ? recordedOrigin : ConstantOrigin::Unspecified));
					}
				}
			}
		}
	}
}

TEST_CASE("TraceContext RECORD collisions reconcile constant origins and preserve the canonical result",
          "[TraceContext]") {
	const auto tracking = GENERATE(ConstantOriginTracking::Disabled, ConstantOriginTracking::Enabled);
	CAPTURE(tracking);
	for (const auto firstOrigin : {ConstantOrigin::Unspecified, ConstantOrigin::CacheInvariant}) {
		for (const auto secondOrigin : {ConstantOrigin::Unspecified, ConstantOrigin::CacheInvariant}) {
			CAPTURE(firstOrigin, secondOrigin);
			std::vector<TypedValueRef*> results;
			std::size_t trueVisits = 0, falseVisits = 0;
			const ConstantLiteral literal {int64_t {7}};
			std::function<void()> wrapper = [&] {
				auto& condition = registerFunctionArgument(Type::b, 0);
				if (traceBool(condition, 0.5)) {
					++trueVisits;
				} else {
					++falseVisits;
				}
				const auto origin = results.empty() ? firstOrigin : secondOrigin;
				results.push_back(&traceConstant(Type::i64, literal, origin));
				traceReturnOperation(Type::i64, *results.back());
			};
			common::Arena arena;
			auto trace = TraceContext::trace(wrapper, engine::Options {}, arena, tracking);
			REQUIRE(trace != nullptr);
			REQUIRE(trace->recordsConstantOrigins() == (tracking == ConstantOriginTracking::Enabled));
			REQUIRE_FALSE(inTracer());
			REQUIRE(trueVisits == 1);
			REQUIRE(falseVisits == 1);
			REQUIRE(results.size() == 2);
			REQUIRE(results[0] == results[1]);
			REQUIRE(trace->getBlocks().size() == 3);
			REQUIRE(trace->getBlock(0).operations.size() == 1);
			const auto* branch = trace->getBlock(0).operations[0];
			REQUIRE(branch->op == Op::CMP);
			REQUIRE(branch->constantOrigin == ConstantOrigin::Unspecified);
			const auto firstBlock = std::get<BlockRef*>(branch->input[1])->block;
			const auto secondBlock = std::get<BlockRef*>(branch->input[2])->block;
			REQUIRE(firstBlock != secondBlock);
			const auto& firstOperations = trace->getBlock(firstBlock).operations;
			const auto& secondOperations = trace->getBlock(secondBlock).operations;
			REQUIRE(firstOperations.size() == 2);
			REQUIRE(secondOperations.size() == 3);
			const auto* original = firstOperations[0];
			const auto* collision = secondOperations[0];
			const auto* reconciliation = secondOperations[1];
			REQUIRE(original->op == Op::CONST);
			REQUIRE(collision->op == Op::CONST);
			REQUIRE(reconciliation->op == Op::ASSIGN);
			REQUIRE(original->tag.getTag() != nullptr);
			REQUIRE(collision->tag == original->tag);
			REQUIRE(reconciliation->tag == original->tag);
			REQUIRE(original->resultType == Type::i64);
			REQUIRE(collision->resultType == Type::i64);
			REQUIRE(std::get<ConstantLiteral>(original->input[0]) == literal);
			REQUIRE(std::get<ConstantLiteral>(collision->input[0]) == literal);
			REQUIRE(&original->resultRef == results[0]);
			REQUIRE(collision->resultRef != original->resultRef);
			REQUIRE(reconciliation->resultRef == original->resultRef);
			REQUIRE(std::get<TypedValueRef>(reconciliation->input[0]) == collision->resultRef);
			const bool bothInvariant = firstOrigin == ConstantOrigin::CacheInvariant && firstOrigin == secondOrigin;
			const auto expectedOrigin = tracking == ConstantOriginTracking::Enabled && bothInvariant
			                                ? ConstantOrigin::CacheInvariant
			                                : ConstantOrigin::Unspecified;
			REQUIRE(original->constantOrigin == expectedOrigin);
			REQUIRE(collision->constantOrigin == expectedOrigin);
			REQUIRE(reconciliation->constantOrigin == ConstantOrigin::Unspecified);
			REQUIRE(trace->globalTagMap.at(original->tag).blockIndex == secondBlock);
			REQUIRE(trace->globalTagMap.at(original->tag).operationIndex == 1);
			REQUIRE(firstOperations[1]->op == Op::RETURN);
			REQUIRE(secondOperations[2]->op == Op::RETURN);
			REQUIRE(firstOperations[1]->constantOrigin == ConstantOrigin::Unspecified);
			REQUIRE(secondOperations[2]->constantOrigin == ConstantOrigin::Unspecified);
			REQUIRE(std::get<TypedValueRef>(firstOperations[1]->input[0]) == original->resultRef);
			REQUIRE(std::get<TypedValueRef>(secondOperations[2]->input[0]) == original->resultRef);
		}
	}
}

TEST_CASE("TraceContext resets origin recording across nested regions and exception recovery", "[TraceContext]") {
	const bool reuseArena = GENERATE(false, true);
	const bool throwBeforeTrace = GENERATE(false, true);
	CAPTURE(reuseArena, throwBeforeTrace);
	bool failInRegion = false;
	std::size_t failures = 0;
	std::function<void()> wrapper = [&] {
		val<int64_t> result(11);
		auto invariant = cacheInvariant(int64_t {7});
		result = result + invariant;
		region("outer", [&] {
			auto outerInvariant = cacheLiteral<int64_t {13}>();
			val<int64_t> outerOrdinary(17);
			result = result + outerInvariant + outerOrdinary;
			region("inner", [&] {
				auto innerInvariant = cacheInvariant(int64_t {19});
				val<int64_t> innerOrdinary(23);
				result = result + innerInvariant + innerOrdinary;
				if (failInRegion) {
					++failures;
					throw std::runtime_error("constant origin recovery");
				}
			});
			result = result + cacheLiteral<int64_t {29}>();
		});
		result = result + cacheInvariant(int64_t {31});
		traceReturnOperation(Type::i64, result.state);
	};
	common::Arena reusedArena;
	const Block* previousBlock = nullptr;
	constexpr std::array modes {ConstantOriginTracking::Enabled, ConstantOriginTracking::Disabled,
	                            ConstantOriginTracking::Enabled};
	for (const auto tracking : modes) {
		CAPTURE(tracking);
		common::Arena freshArena;
		auto& arena = reuseArena ? reusedArena : freshArena;
		if (throwBeforeTrace) {
			failInRegion = true;
			const auto failingTracking = tracking == ConstantOriginTracking::Enabled ? ConstantOriginTracking::Disabled
			                                                                         : ConstantOriginTracking::Enabled;
			REQUIRE_THROWS_AS(TraceContext::trace(wrapper, engine::Options {}, arena, failingTracking),
			                  std::runtime_error);
			REQUIRE_FALSE(inTracer());
			arena.softReset();
			failInRegion = false;
		}
		auto trace = TraceContext::trace(wrapper, engine::Options {}, arena, tracking);
		REQUIRE(trace != nullptr);
		REQUIRE_FALSE(inTracer());
		REQUIRE(trace->recordsConstantOrigins() == (tracking == ConstantOriginTracking::Enabled));
		REQUIRE(&trace->getArena() == &arena);
		REQUIRE(trace->getBlocks().size() == 1);
		REQUIRE(trace->getRegions().size() == 2);
		REQUIRE(trace->getRegions()[0].parent == NO_REGION);
		REQUIRE(trace->getRegions()[1].parent == 0);
		requireConstants(*trace, {{11, ConstantOrigin::Unspecified},
		                          {7, ConstantOrigin::CacheInvariant},
		                          {13, ConstantOrigin::CacheInvariant, 0},
		                          {17, ConstantOrigin::Unspecified, 0},
		                          {19, ConstantOrigin::CacheInvariant, 1},
		                          {23, ConstantOrigin::Unspecified, 1},
		                          {29, ConstantOrigin::CacheInvariant, 0},
		                          {31, ConstantOrigin::CacheInvariant}});
		if (reuseArena && previousBlock != nullptr) {
			REQUIRE(&trace->getBlock(0) == previousBlock);
		}
		previousBlock = &trace->getBlock(0);
		trace.reset();
		arena.softReset();
	}
	REQUIRE(failures == (throwBeforeTrace ? modes.size() : 0));
}

} // namespace nautilus::tracing

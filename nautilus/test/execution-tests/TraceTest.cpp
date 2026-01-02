#include "nautilus/tracing/TracerTrace.hpp"
#include <catch2/catch_all.hpp>

using namespace nautilus;
using namespace nautilus::tracing;

TEST_CASE("Trace Basic Operations") {
	Trace trace;

	SECTION("Initial state") {
		REQUIRE(trace.view != nullptr);
		REQUIRE(trace.view->numberOfBlocks == 0);
	}

	SECTION("Get current block") {
		trace.createNewBlock();
		auto block = trace.getCurrentBlock();
		REQUIRE(block.view != nullptr);
		REQUIRE(block.view->magic == 0xDEADBEEF);
	}

	SECTION("Add constant operation") {
		trace.createNewBlock();
		auto [ref, oi] = trace.addConstOperation({}, Type::i64, int64_t(42));

		REQUIRE(oi.blockIndex.offset == sizeof(Trace::TraceHeader));
	}

	SECTION("Create new block") {
		[[maybe_unused]] auto block1 = trace.createNewBlock();
		REQUIRE(trace.view->numberOfBlocks == 1);

		[[maybe_unused]] auto block2 = trace.createNewBlock();
		REQUIRE(trace.view->numberOfBlocks == 2);
	}

	SECTION("Get operations") {
		trace.createNewBlock();
		trace.addConstOperation({}, Type::i64, int64_t(42));
		trace.createNewBlock();
		trace.addConstOperation({}, Type::i64, int64_t(43));
		trace.createNewBlock();
		trace.addConstOperation({}, Type::i64, int64_t(44));
		trace.addConstOperation({}, Type::i64, int64_t(45));

		auto blocks = trace.getBlocks();
		REQUIRE(blocks.size() == 3);
		REQUIRE(blocks[0].getOperations().size() == 1);
		REQUIRE(blocks[1].getOperations().size() == 1);
		REQUIRE(blocks[2].getOperations().size() == 2);
	}

	SECTION("Append jump") {
		trace.createNewBlock();
		auto [ref1, oi] = trace.appendJump(1);
		auto [ref2, oi1] = trace.appendJump(1);
		REQUIRE(oi.blockIndex.offset == sizeof(Trace::TraceHeader));
		REQUIRE(oi.operationOffset.offset == sizeof(Trace::TraceHeader) + sizeof(Trace::BlockHeader));
		REQUIRE(oi1.operationOffset.offset == sizeof(Trace::TraceHeader) + sizeof(Trace::BlockHeader) +
		                                          sizeof(Trace::OperationHeader) + sizeof(JmpInput));
	}
}

TEST_CASE("Trace Operation Size") {
	SECTION("Binary operations") {
		REQUIRE(getOperationSize(ADD) == sizeof(Trace::OperationHeader) + sizeof(BinaryInput));
		REQUIRE(getOperationSize(SUB) == sizeof(Trace::OperationHeader) + sizeof(BinaryInput));
		REQUIRE(getOperationSize(MUL) == sizeof(Trace::OperationHeader) + sizeof(BinaryInput));
	}

	SECTION("Unary operations") {
		REQUIRE(getOperationSize(NOT) == sizeof(Trace::OperationHeader) + sizeof(UnaryInput));
		REQUIRE(getOperationSize(NEGATE) == sizeof(Trace::OperationHeader) + sizeof(UnaryInput));
	}

	SECTION("Call with args") {
		REQUIRE(getOperationSize(CALL, 0) == sizeof(Trace::OperationHeader) + sizeof(CallInput));
		REQUIRE(getOperationSize(CALL, 3) ==
		        sizeof(Trace::OperationHeader) + sizeof(CallInput) + 3 * sizeof(TypedValueRef));
	}
}

TEST_CASE("Control Flow Merge") {
	Trace trace;
	trace.createNewBlock();
	trace.addConstOperation({}, Type::i64, int64_t(41));
	trace.addConstOperation({}, Type::i64, int64_t(42));
	auto [mergeRef, mergeOi] = trace.addConstOperation({}, Type::i64, int64_t(43));
	trace.addConstOperation({}, Type::i64, int64_t(44));
	trace.addConstOperation({}, Type::i64, int64_t(45));
	trace.addConstOperation({}, Type::i64, int64_t(47));
	trace.createNewBlock();

	SECTION("Merge blocks") {
		trace.addConstOperation({}, Type::i64, int64_t(46));
		trace.processControlFlowMerge(mergeOi);
		REQUIRE(trace.view->numberOfBlocks == 3);
		auto blocks = trace.getBlocks();
		REQUIRE(blocks[0].getOperations().size() == 3); // 2 const + jump
		REQUIRE(blocks[1].getOperations().size() == 2); // 1 const + jump
		REQUIRE(blocks[2].getOperations().size() == 4); // 4 const

		std::cout << toString(trace) << std::endl;
	}
}

TEST_CASE("Operation type classification") {
	SECTION("Binary ops") {
		REQUIRE(isBinaryOp(ADD));
		REQUIRE(isBinaryOp(SUB));
		REQUIRE(isBinaryOp(MUL));
		REQUIRE(isBinaryOp(DIV));
		REQUIRE(isBinaryOp(EQ));
		REQUIRE(isBinaryOp(AND));
		REQUIRE(isBinaryOp(OR));
		REQUIRE_FALSE(isBinaryOp(NOT));
		REQUIRE_FALSE(isBinaryOp(CONST));
	}

	SECTION("Unary ops") {
		REQUIRE(isUnaryOp(NOT));
		REQUIRE(isUnaryOp(NEGATE));
		REQUIRE(isUnaryOp(CAST));
		REQUIRE_FALSE(isUnaryOp(ADD));
		REQUIRE_FALSE(isUnaryOp(CONST));
	}
}

TEST_CASE("addOperationWithResult") {
	Trace trace;
	trace.createNewBlock();

	// Create some operands to work with
	auto [ref1, _1] = trace.addConstOperation({}, Type::i64, int64_t(10));
	auto [ref2, _2] = trace.addConstOperation({}, Type::i64, int64_t(20));
	auto [ptrRef, _3] = trace.addConstOperation({}, Type::ptr, (void*) nullptr);
	[[maybe_unused]] auto [boolRef, _4] = trace.addConstOperation({}, Type::b, true);

	SECTION("Binary operation - ADD") {
		auto [result, oi] = trace.addOperationWithResult({}, Type::i64, ADD, {ref1, ref2});
		REQUIRE(result.type == Type::i64);

		auto blocks = trace.getBlocks();
		auto ops = blocks[0].getOperations();
		// 4 consts + 1 ADD
		REQUIRE(ops.size() == 5);

		auto& addOp = ops[4];
		REQUIRE(addOp.view->op == ADD);
		auto& input = addOp.arguments<BinaryInput>();
		REQUIRE(input.lhs.ref == ref1.ref);
		REQUIRE(input.rhs.ref == ref2.ref);
	}

	SECTION("Unary operation - NEGATE") {
		auto [result, oi] = trace.addOperationWithResult({}, Type::i64, NEGATE, {ref1});
		REQUIRE(result.type == Type::i64);

		auto blocks = trace.getBlocks();
		auto ops = blocks[0].getOperations();
		REQUIRE(ops.size() == 5);

		auto& negOp = ops[4];
		REQUIRE(negOp.view->op == NEGATE);
		auto& input = negOp.arguments<UnaryInput>();
		REQUIRE(input.operand.ref == ref1.ref);
	}

	SECTION("LOAD operation") {
		auto [result, oi] = trace.addOperationWithResult({}, Type::i64, LOAD, {ptrRef});
		REQUIRE(result.type == Type::i64);

		auto blocks = trace.getBlocks();
		auto ops = blocks[0].getOperations();
		REQUIRE(ops.size() == 5);

		auto& loadOp = ops[4];
		REQUIRE(loadOp.view->op == LOAD);
		auto& input = loadOp.arguments<MemoryInput>();
		REQUIRE(input.ptr.ref == ptrRef.ref);
	}

	SECTION("STORE operation") {
		trace.addOperationWithResult({}, Type::v, STORE, {ptrRef, ref1});

		auto blocks = trace.getBlocks();
		auto ops = blocks[0].getOperations();
		REQUIRE(ops.size() == 5);

		auto& storeOp = ops[4];
		REQUIRE(storeOp.view->op == STORE);
		auto& input = storeOp.arguments<MemoryInput>();
		REQUIRE(input.ptr.ref == ptrRef.ref);
		REQUIRE(input.value.ref == ref1.ref);
	}

	// SECTION("SELECT operation") - Removed: SELECT operation no longer exists after TracerTrace refactor
}

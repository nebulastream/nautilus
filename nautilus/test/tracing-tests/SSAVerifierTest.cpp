#include "nautilus/tracing/ExecutionTrace.hpp"
#include "nautilus/tracing/phases/SSAVerifier.hpp"
#include <catch2/catch_test_macros.hpp>

namespace nautilus::tracing {

namespace {

void addReturn(ExecutionTrace& trace, Block& block, TypedValueRef ref) {
	auto snapshot = Snapshot();
	block.addOperation(
	    makeTraceOp(trace.getArena(), snapshot, Op::RETURN, ref.type, TypedValueRef(0, Type::v), ref));
}

void addIndirectCall(ExecutionTrace& trace, Block& block, TypedValueRef fnPtr, std::vector<TypedValueRef> arguments,
	                 TypedValueRef result) {
	auto* call = trace.getArena().create<IndirectFunctionCall>(
	    IndirectFunctionCall {.fnPtr = fnPtr, .arguments = std::move(arguments), .fnAttrs = {}});
	auto snapshot = Snapshot();
	block.addOperation(makeTraceOp(trace.getArena(), snapshot, Op::INDIRECT_CALL, result.type, result, call));
	addReturn(trace, block, result);
}

} // namespace

TEST_CASE("SSAVerifier rejects undefined indirect-call function pointers", "[SSAVerifier]") {
	common::Arena arena;
	ExecutionTrace trace(arena);
	auto& block = trace.getBlock(0);
	block.arguments.emplace_back(1, Type::i32);
	addIndirectCall(trace, block, TypedValueRef(2, Type::ptr), {TypedValueRef(1, Type::i32)},
	                TypedValueRef(3, Type::i32));

	REQUIRE_FALSE(VerifySSA(trace).valid);
}

TEST_CASE("SSAVerifier rejects undefined indirect-call arguments", "[SSAVerifier]") {
	common::Arena arena;
	ExecutionTrace trace(arena);
	auto& block = trace.getBlock(0);
	block.arguments.emplace_back(1, Type::ptr);
	addIndirectCall(trace, block, TypedValueRef(1, Type::ptr), {TypedValueRef(2, Type::i32)},
	                TypedValueRef(3, Type::i32));

	REQUIRE_FALSE(VerifySSA(trace).valid);
}

TEST_CASE("SSAVerifier preserves full-width value references", "[SSAVerifier]") {
	common::Arena arena;
	ExecutionTrace trace(arena);
	auto& block = trace.getBlock(0);
	block.arguments.emplace_back(1, Type::i32);
	addReturn(trace, block, TypedValueRef(65537, Type::i32));

	REQUIRE_FALSE(VerifySSA(trace).valid);
}

TEST_CASE("SSAVerifier accepts defined full-width value references", "[SSAVerifier]") {
	common::Arena arena;
	ExecutionTrace trace(arena);
	auto& block = trace.getBlock(0);
	block.arguments.emplace_back(70001, Type::i32);
	addReturn(trace, block, TypedValueRef(70001, Type::i32));

	REQUIRE(VerifySSA(trace).valid);
}

TEST_CASE("SSAVerifier rejects operation results that collide with block arguments", "[SSAVerifier]") {
	common::Arena arena;
	ExecutionTrace trace(arena);
	auto& block = trace.getBlock(0);
	block.arguments.emplace_back(1, Type::i32);
	auto snapshot = Snapshot();
	block.addOperation(makeTraceOp(trace.getArena(), snapshot, Op::CONST, Type::i32, TypedValueRef(1, Type::i32),
	                               createConstLiteral(int32_t {42})));
	addReturn(trace, block, TypedValueRef(1, Type::i32));

	REQUIRE_FALSE(VerifySSA(trace).valid);
}

} // namespace nautilus::tracing

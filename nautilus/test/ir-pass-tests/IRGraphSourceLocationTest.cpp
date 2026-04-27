#include "IRGraphFixtures.hpp"
#include "nautilus/compiler/ir/IRGraph.hpp"
#include "nautilus/compiler/ir/operations/ConstIntOperation.hpp"
#include "nautilus/compiler/ir/operations/ReturnOperation.hpp"
#include "nautilus/tracing/tag/SourceLocationResolver.hpp"
#include <catch2/catch_test_macros.hpp>
#include <vector>

namespace nautilus::compiler::ir {

namespace {

// Helpers — every test builds a tiny graph by hand so we can stamp
// frames on a known op pointer without going through the full
// trace-to-IR pipeline.
struct Fixture {
	std::shared_ptr<IRGraph> ir;
	Operation* op;
	Operation* op2;
};

Fixture makeFixture() {
	auto ir = std::make_shared<IRGraph>("source-location-test");
	auto& arena = ir->getArena();
	auto* op = arena.create<ConstIntOperation>(arena, OperationIdentifier {1}, int64_t {42}, Type::i64);
	auto* op2 = arena.create<ConstIntOperation>(arena, OperationIdentifier {2}, int64_t {7}, Type::i64);
	return {std::move(ir), op, op2};
}

tracing::SourceFrame makeFrame(std::string file, std::string function, uint32_t line) {
	tracing::SourceFrame f;
	f.file = std::move(file);
	f.function = std::move(function);
	f.line = line;
	f.column = 1;
	return f;
}

} // namespace

TEST_CASE("IRGraph::getSourceLocation returns an empty span for ops with no entry") {
	auto fx = makeFixture();
	REQUIRE(fx.ir->getSourceLocation(fx.op).empty());
}

TEST_CASE("IRGraph::setSourceLocation stores frames retrievable by Operation*") {
	auto fx = makeFixture();
	std::vector<tracing::SourceFrame> frames {makeFrame("outer.cpp", "outer", 10), makeFrame("inner.cpp", "inner", 20)};
	fx.ir->setSourceLocation(fx.op, frames);

	auto retrieved = fx.ir->getSourceLocation(fx.op);
	REQUIRE(retrieved.size() == 2);
	REQUIRE(retrieved[0].file == "outer.cpp");
	REQUIRE(retrieved[0].line == 10);
	REQUIRE(retrieved[1].file == "inner.cpp");
	REQUIRE(retrieved[1].line == 20);
}

TEST_CASE("IRGraph::setSourceLocation drops empty frame vectors so consumers still see an empty span") {
	auto fx = makeFixture();
	fx.ir->setSourceLocation(fx.op, {});
	REQUIRE(fx.ir->getSourceLocation(fx.op).empty());
}

TEST_CASE("IRGraph::setSourceLocation overwrites a previous entry for the same op") {
	auto fx = makeFixture();
	fx.ir->setSourceLocation(fx.op, {makeFrame("a.cpp", "a", 1)});
	fx.ir->setSourceLocation(fx.op, {makeFrame("b.cpp", "b", 2)});

	auto retrieved = fx.ir->getSourceLocation(fx.op);
	REQUIRE(retrieved.size() == 1);
	REQUIRE(retrieved[0].file == "b.cpp");
	REQUIRE(retrieved[0].line == 2);
}

TEST_CASE("IRGraph::copySourceLocation duplicates the entry under the new key") {
	auto fx = makeFixture();
	fx.ir->setSourceLocation(fx.op, {makeFrame("src.cpp", "fn", 5)});
	fx.ir->copySourceLocation(fx.op, fx.op2);

	auto retrieved = fx.ir->getSourceLocation(fx.op2);
	REQUIRE(retrieved.size() == 1);
	REQUIRE(retrieved[0].file == "src.cpp");
	REQUIRE(retrieved[0].line == 5);
	// Source entry must remain — copy, not move.
	REQUIRE(fx.ir->getSourceLocation(fx.op).size() == 1);
}

TEST_CASE("IRGraph::copySourceLocation is a no-op when the source has no entry") {
	auto fx = makeFixture();
	fx.ir->copySourceLocation(fx.op, fx.op2);
	REQUIRE(fx.ir->getSourceLocation(fx.op2).empty());
}

TEST_CASE("IRGraph::copySourceLocation tolerates null and self-copy without modifying state") {
	auto fx = makeFixture();
	fx.ir->setSourceLocation(fx.op, {makeFrame("src.cpp", "fn", 5)});
	fx.ir->copySourceLocation(nullptr, fx.op2);
	fx.ir->copySourceLocation(fx.op, nullptr);
	fx.ir->copySourceLocation(fx.op, fx.op);

	REQUIRE(fx.ir->getSourceLocation(fx.op).size() == 1);
	REQUIRE(fx.ir->getSourceLocation(fx.op2).empty());
}

} // namespace nautilus::compiler::ir

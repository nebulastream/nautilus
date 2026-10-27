#include "nautilus/compiler/NirToIR.hpp"
#include "nautilus/compiler/backends/nir/NirBackend.hpp"
#include "nautilus/compiler/ir/IRGraph.hpp"
#include "nautilus/compiler/ir/blocks/BasicBlock.hpp"
#include "nautilus/compiler/ir/operations/CallOperation.hpp"
#include "nautilus/compiler/ir/operations/CastOperation.hpp"
#include "nautilus/compiler/ir/operations/ConstIntOperation.hpp"
#include "nautilus/compiler/ir/operations/FunctionOperation.hpp"
#include "nautilus/compiler/ir/operations/LogicalOperations/CompareOperation.hpp"
#include "nautilus/compiler/ir/passes/ExceptionRegionPreparationPass.hpp"
#include "nautilus/compiler/ir/passes/IRPassManager.hpp"
#include "nautilus/compiler/ir/passes/IRVerifier.hpp"
#include "nautilus/nir/Parser.hpp"
#include "nautilus/nir/Verifier.hpp"
#include <catch2/catch_all.hpp>
#include <map>
#include <string>

// The bridge from SSA NIR to the 0.3 IR (compiler/NirToIR.hpp): what it makes of signless integers, exceptions and
// returns, checked on small modules in the text form. Execution on every backend is covered by the execution tests.

namespace nautilus::testing {

namespace {

using compiler::ir::CallOperation;
using compiler::ir::CastOperation;
using compiler::ir::CompareOperation;
using compiler::ir::ConstIntOperation;
using compiler::ir::FunctionOperation;
using compiler::ir::IRGraph;
using compiler::ir::Operation;

void bridgeCallee(int64_t) {
}
void bridgeThunk() {
}
void bridgeDestroy(void*) {
}
void bridgeDestroyOther(void*) {
}

void* resolveBridgeSymbol(std::string_view name) {
	static const std::map<std::string, void*, std::less<>> symbols {
	    {"callee", reinterpret_cast<void*>(&bridgeCallee)},
	    {"callee$thunk", reinterpret_cast<void*>(&bridgeThunk)},
	    {"destroy", reinterpret_cast<void*>(&bridgeDestroy)},
	    {"destroyOther", reinterpret_cast<void*>(&bridgeDestroyOther)},
	};
	auto it = symbols.find(name);
	return it == symbols.end() ? nullptr : it->second;
}

/// Parses `text`, builds its IR and runs the exception-region preparation like the pipeline does.
std::unique_ptr<IRGraph> bridge(const std::string& text) {
	auto parsed = nir::parse(text);
	INFO((parsed.error ? parsed.error->str() : std::string("parsed")));
	REQUIRE(parsed.ok());
	static std::vector<std::unique_ptr<nir::Module>> modules; // the graph refers to the module's data until it is gone
	modules.push_back(std::make_unique<nir::Module>(std::move(*parsed.module)));
	compiler::nirbackend::HostEnvironment host;
	host.resolveSymbol = resolveBridgeSymbol;
	auto graph = std::make_unique<IRGraph>("bridge-test");
	engine::Options options;
	compiler::buildIRFromNir(*modules.back(), *graph, options, &host);
	compiler::ir::IRPassManager passes(options);
	passes.addPass(std::make_unique<compiler::ir::ExceptionRegionPreparationPass>());
	passes.run(*graph);
	const auto verified = compiler::ir::IRVerifier::verify(*graph);
	INFO(verified.toString());
	REQUIRE(verified.ok());
	return graph;
}

const FunctionOperation& function(const IRGraph& graph, const std::string& name) {
	const auto* found = graph.getFunctionOperation(name);
	REQUIRE(found != nullptr);
	return *found;
}

std::vector<const Operation*> operationsOf(const FunctionOperation& f) {
	std::vector<const Operation*> result;
	for (const auto* block : f.getBasicBlocks()) {
		for (const auto* operation : block->getOperations()) {
			result.push_back(operation);
		}
	}
	return result;
}

template <class T>
std::vector<const T*> find(const FunctionOperation& f) {
	std::vector<const T*> result;
	for (const auto* operation : operationsOf(f)) {
		if (const auto* typed = compiler::ir::dyn_cast<T>(operation)) {
			result.push_back(typed);
		}
	}
	return result;
}

} // namespace

TEST_CASE("the bridge takes the sign of an integer from the operations that state it", "[nir][bridge]") {
	auto graph = bridge(R"(nir 1.0
func export @unsignedCompare : (i32, i32) -> bool zext {
^b0(%0: i32, %1: i32):
  %2 = cmp.lt.u %0, %1
  ret %2
}
func export @signedCompare : (i32, i32) -> bool zext {
^b0(%0: i32, %1: i32):
  %2 = cmp.lt.s %0, %1
  ret %2
}
)");
	auto unsignedCompare = find<CompareOperation>(function(*graph, "unsignedCompare"));
	REQUIRE(unsignedCompare.size() == 1);
	CHECK(unsignedCompare[0]->getInputs()[0]->getStamp() == Type::ui32);
	CHECK(unsignedCompare[0]->getInputs()[1]->getStamp() == Type::ui32);
	auto signedCompare = find<CompareOperation>(function(*graph, "signedCompare"));
	REQUIRE(signedCompare.size() == 1);
	CHECK(signedCompare[0]->getInputs()[0]->getStamp() == Type::i32);
	CHECK(function(*graph, "signedCompare").getInputArgs().empty()); // the parameters are the entry block's arguments
	CHECK(function(*graph, "signedCompare").getFunctionBasicBlock().getArguments().size() == 2);
}

TEST_CASE("the bridge casts where a use disagrees with the stamp of its operand", "[nir][bridge]") {
	// a parameter that the signature extends as signed, compared as unsigned, with a constant
	auto graph = bridge(R"(nir 1.0
func export @mixed : (i32 sext) -> bool zext {
^b0(%0: i32):
  %1 = cmp.lt.u %0, 5:i32
  ret %1
}
)");
	const auto& f = function(*graph, "mixed");
	CHECK(f.getFunctionBasicBlock().getArguments()[0]->getStamp() == Type::i32);
	auto casts = find<CastOperation>(f);
	REQUIRE(casts.size() == 1);
	CHECK(casts[0]->getStamp() == Type::ui32);
	auto constants = find<ConstIntOperation>(f);
	REQUIRE(constants.size() == 1);
	CHECK(constants[0]->getStamp() == Type::ui32);
	CHECK(constants[0]->getValue() == 5);
}

TEST_CASE("the bridge sign-extends a signed constant of its own width", "[nir][bridge]") {
	auto graph = bridge(R"(nir 1.0
func export @negative : (i8 sext) -> bool zext {
^b0(%0: i8):
  %1 = cmp.lt.s %0, 0xff:i8
  ret %1
}
)");
	auto constants = find<ConstIntOperation>(function(*graph, "negative"));
	REQUIRE(constants.size() == 1);
	CHECK(constants[0]->getStamp() == Type::i8);
	CHECK(constants[0]->getValue() == -1);
}

TEST_CASE("the bridge turns a landing pad into the destructors of the call", "[nir][bridge]") {
	auto graph = bridge(R"(nir 1.0
pointer #p0 = callee symbol "callee"
pointer #p1 = thunk symbol "callee$thunk"
pointer #p2 = destructor symbol "destroy"
pointer #p3 = destructor symbol "destroyOther"
callee #c0 = external #p0 : (i64) -> void
callee #c1 = external #p2 : (ptr) -> void attrs(noUnwind)
callee #c2 = external #p3 : (ptr) -> void attrs(noUnwind)

func export @guarded : (i64) -> void {
  alloca 8 align 8
  alloca 8 align 8
^b0(%0: i64):
  %1 = alloca 0
  %2 = alloca 1
  call #c0(%0) unwind ^b1 thunk #p1
  call #c1(%1)
  ret
^b1 pad:
  call #c2(%2)
  call #c1(%1)
  resume
}
)");
	const auto& f = function(*graph, "guarded");
	// the pad is not a block of 0.3
	CHECK(f.getBasicBlocks().size() == 1);
	CHECK(f.getAllocaSpecs().size() == 2);
	std::vector<const CallOperation*> mayThrow;
	for (const auto* call : find<CallOperation>(f)) {
		if (call->requiresExceptionHandling()) {
			mayThrow.push_back(call);
		}
	}
	REQUIRE(mayThrow.size() == 1);
	CHECK(mayThrow[0]->getCaptureFunc() == reinterpret_cast<void*>(&bridgeThunk));
	// 0.3 lists destructors in construction order and runs them in reverse: the pad runs destroyOther, then destroy
	const auto& destructors = mayThrow[0]->getDestructors();
	REQUIRE(destructors.size() == 2);
	CHECK(destructors[0].functionPtr == reinterpret_cast<void*>(&bridgeDestroy));
	CHECK(destructors[1].functionPtr == reinterpret_cast<void*>(&bridgeDestroyOther));
	REQUIRE(f.exceptionRegion.has_value());
	CHECK(f.exceptionRegion->callSites.size() == 1);
}

TEST_CASE("the bridge makes one return block of several returns", "[nir][bridge]") {
	auto graph = bridge(R"(nir 1.0
func export @twoReturns : (i64) -> i64 {
^b0(%0: i64):
  %1 = cmp.eq %0, 0:i64
  condbr %1, ^b1(), ^b2() prob 0.5
^b1:
  ret 1:i64
^b2:
  ret %0
}
)");
	const auto& f = function(*graph, "twoReturns");
	// entry, two arms and the block that takes the result
	REQUIRE(f.getBasicBlocks().size() == 4);
	const auto* returnBlock = f.getBasicBlocks().back();
	CHECK(returnBlock->getArguments().size() == 1);
	CHECK(returnBlock->getPredecessors().size() == 2);
}

TEST_CASE("the bridge links internal calls to their definitions", "[nir][bridge]") {
	auto graph = bridge(R"(nir 1.0
callee #c0 = internal @inc : (i64) -> i64

func export @outer : (i64) -> i64 {
^b0(%0: i64):
  %1 = call #c0(%0)
  ret %1
}
func @inc : (i64) -> i64 {
^b0(%0: i64):
  %1 = add %0, 1:i64
  ret %1
}
)");
	const auto calls = find<CallOperation>(function(*graph, "outer"));
	REQUIRE(calls.size() == 1);
	CHECK(calls[0]->isNautilusFunctionCall());
	const auto& target = graph->getFunctionTarget(calls[0]->getCalleeId());
	CHECK(target.getDefinition() == graph->getFunctionOperation("inc"));
	CHECK(function(*graph, "outer").hasAttribute("entry"));
	CHECK(!function(*graph, "inc").hasAttribute("entry"));
}

} // namespace nautilus::testing

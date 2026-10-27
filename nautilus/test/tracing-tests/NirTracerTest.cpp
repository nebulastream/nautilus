// The direct NIR tracer (tracing/nir/NirTracer): golden pre-SSA dumps of a small corpus, agreement with the 0.3 tracer
// after conversion, and the allocation counter of docs/plans/nautilus-0.4/12-validation.md (no malloc per recorded op).
#include "AllocationCounter.hpp"
#include "DistinctSitesFunctions.hpp"
#include "nautilus/CompilableFunction.hpp"
#include "nautilus/Engine.hpp"
#include "nautilus/common/Arena.hpp"
#include "nautilus/conversion/Conversion.hpp"
#include "nautilus/extensions/ExtensionRegistry.hpp"
#include "nautilus/nir/Printer.hpp"
#include "nautilus/nir/Verifier.hpp"
#include "nautilus/select.hpp"
#include "nautilus/tracing/TraceContext.hpp"
#include "nautilus/tracing/TracingUtil.hpp"
#include "nautilus/tracing/nir/NirTrace.hpp"
#include "nautilus/tracing/nir/NirTracer.hpp"
#include <catch2/catch_all.hpp>
#include <cstdlib>
#include <fstream>
#include <functional>
#include <list>
#include <sstream>

using namespace nautilus;
using namespace nautilus::engine;

namespace {

val<int32_t> linearSum(val<int32_t> a, val<int32_t> b) {
	return (a + b) * a - b;
}

val<int32_t> ifElse(val<int32_t> a, val<int32_t> b) {
	val<int32_t> r = 0;
	if (a < b) {
		r = a;
	} else {
		r = b;
	}
	return r;
}

val<int32_t> countedLoop(val<int32_t> n) {
	val<int32_t> sum = 0;
	for (val<int32_t> i = 0; i < n; i = i + 1) {
		sum = sum + i;
	}
	return sum;
}

// a val initialised from a constant and assigned later, in a loop
val<int32_t> constantAssignedInLoop(val<int32_t> n) {
	val<int32_t> k = 5;
	for (val<int32_t> i = 0; i < n; i = i + 1) {
		k = k + 1;
	}
	return k;
}

// a copy that is assigned later, in a loop
val<int32_t> copyAssignedInLoop(val<int32_t> a, val<int32_t> n) {
	val<int32_t> c = a;
	for (val<int32_t> i = 0; i < n; i = i + 1) {
		c = c + a;
	}
	return c;
}

// #95: a copy constructed from different sources on the two arms of a native ternary
val<int32_t> copyFromTwoSources(val<int32_t> a, val<int32_t> b) {
	val<int32_t> c = (a < b) ? a : b;
	return c + 1;
}

val<int32_t> selectOperation(val<bool> condition, val<int32_t> a, val<int32_t> b) {
	return select(condition, a, b);
}

val<int32_t> checkedDivision(val<int32_t> a, val<int32_t> b) {
	return a / b + a % b;
}

// a zero-trip loop in an if-arm, merged after the if: the constant after the merge is declared on one path only
val<float> zeroTripLoopMerge(val<float> p) {
	val<float> result = 0.0f;
	if (p != 0.0f) {
		val<float> acc = 0.0f;
		for (val<int32_t> i = 0; i < val<int32_t>(0); i = i + 1) {
			acc = 0.0f;
		}
		result = acc;
	}
	return select(result != 0.0f, val<float>(1.0f), val<float>(0.0f));
}

val<int32_t> nestedIf(val<int32_t> a, val<int32_t> b) {
	val<int32_t> r = a;
	if (a > 0) {
		if (b > 0) {
			r = a + b;
		} else {
			r = a - b;
		}
	}
	return r * 2;
}

// An extension operation recorded through traceExtOp. The registry is the process-wide one; the operation is
// target-restricted so that it needs no fallback.
uint16_t testOpcode() {
	static const uint16_t opcode = [] {
		ext::ExtOpInfo info;
		info.name = "test.fma";
		info.shape.a = ext::FieldKind::Value;
		info.shape.b = ext::FieldKind::Value;
		info.shape.c = ext::FieldKind::Value;
		info.sig.operands = 3;
		info.sig.result = nir::TypeId::I32;
		info.traits = ext::TraitPure;
		info.targets = 1;
		return ext::ExtensionRegistry::instance().registerOp(info, ext::Owner::Plugin);
	}();
	return opcode;
}

val<int32_t> extensionOperation(val<int32_t> a, val<int32_t> b) {
	return val<int32_t>(
	    tracing::traceExtOp(testOpcode(), static_cast<uint8_t>(nir::TypeId::I32), {a.state, b.state, a.state}));
}

struct CorpusEntry {
	std::string name;
	std::function<void()> wrapper;
	/// The 0.3 tracer cannot record it
	bool directOnly = false;
};

std::vector<CorpusEntry> corpus() {
	return {{"linearSum", engine::details::createFunctionWrapper(linearSum)},
	        {"ifElse", engine::details::createFunctionWrapper(ifElse)},
	        {"countedLoop", engine::details::createFunctionWrapper(countedLoop)},
	        {"constantAssignedInLoop", engine::details::createFunctionWrapper(constantAssignedInLoop)},
	        {"copyAssignedInLoop", engine::details::createFunctionWrapper(copyAssignedInLoop)},
	        {"copyFromTwoSources", engine::details::createFunctionWrapper(copyFromTwoSources)},
	        {"selectOperation", engine::details::createFunctionWrapper(selectOperation)},
	        {"checkedDivision", engine::details::createFunctionWrapper(checkedDivision)},
	        {"zeroTripLoopMerge", engine::details::createFunctionWrapper(zeroTripLoopMerge)},
	        {"nestedIf", engine::details::createFunctionWrapper(nestedIf)},
	        {"extensionOperation", engine::details::createFunctionWrapper(extensionOperation), true}};
}

nir::Module traceDirect(const std::function<void()>& wrapper, common::Arena& arena, uint64_t* operations = nullptr) {
	std::list<compiler::CompilableFunction> functions;
	functions.emplace_back("execute", wrapper);
	auto result = tracing::nirtrace::NirTracer::Trace(functions, engine::Options {}, arena);
	if (operations != nullptr) {
		*operations = result.statistics.operations;
	}
	return std::move(result.module);
}

nir::Module trace03(const std::function<void()>& wrapper, common::Arena& arena) {
	std::list<compiler::CompilableFunction> functions;
	functions.emplace_back("execute", wrapper);
	auto module = tracing::TraceContext::Trace(functions, engine::Options {}, arena);
	return tracing::nirtrace::toPreSSA(*module);
}

std::string diagnostics(const nir::Module& m) {
	std::string text;
	for (const auto& d : nir::verify(m)) {
		text += d.str() + "\n";
	}
	return text;
}

std::string goldenPath(const std::string& name) {
	return std::string(NIR_TRACE_GOLDEN_DIR) + "/" + name + ".nir";
}

std::string readFile(const std::string& path) {
	std::ifstream in(path);
	std::stringstream buffer;
	buffer << in.rdbuf();
	return buffer.str();
}

} // namespace

TEST_CASE("the direct tracer's pre-SSA output verifies, converts and matches the golden dump", "[nir-tracer][golden]") {
	for (const auto& entry : corpus()) {
		DYNAMIC_SECTION(entry.name) {
			common::Arena arena;
			auto module = traceDirect(entry.wrapper, arena);
			REQUIRE(diagnostics(module).empty());
			const auto text = nir::print(module);
			INFO(text);
			if (!entry.directOnly) { // the conversion does not handle extension instructions yet
				REQUIRE_NOTHROW(conversion::convert(module));
				REQUIRE(diagnostics(conversion::convert(module)).empty());
			}
			if (std::getenv("NAUTILUS_UPDATE_GOLDEN") != nullptr) {
				std::ofstream(goldenPath(entry.name)) << text;
			} else {
				REQUIRE(text == readFile(goldenPath(entry.name)));
			}
		}
	}
}

TEST_CASE("the direct tracer and the 0.3 tracer converge to the same SSA", "[nir-tracer][equivalence]") {
	for (const auto& entry : corpus()) {
		if (entry.directOnly) {
			continue;
		}
		DYNAMIC_SECTION(entry.name) {
			common::Arena arena03, arenaDirect;
			auto direct = conversion::convert(traceDirect(entry.wrapper, arenaDirect));
			auto reference = conversion::convert(trace03(entry.wrapper, arena03));
			REQUIRE(nir::print(direct) == nir::print(reference));
		}
	}
}

TEST_CASE("a trace in steady state makes no heap allocation per recorded operation", "[nir-tracer][allocations]") {
	struct Run {
		uint64_t operations;
		uint64_t allocations;
	};
	auto measure = [](const std::function<void()>& wrapper) {
		common::Arena arena;
		traceDirect(wrapper, arena); // the first trace warms the caches the tracer keeps
		arena.softReset();
		uint64_t operations = 0;
		const auto before = testing::allocations();
		auto module = traceDirect(wrapper, arena, &operations);
		return Run {operations, testing::allocations() - before};
	};
	// 1024 distinct call sites against a function of 4 operations: the allocations the longer trace makes beyond the
	// short one's are the per-operation cost
	const auto small = measure(engine::details::createFunctionWrapper(linearSum));
	const auto large = measure(engine::details::createFunctionWrapper(distinctSites1024));
	INFO("small: " << small.operations << " operations, " << small.allocations << " allocations");
	INFO("large: " << large.operations << " operations, " << large.allocations << " allocations");
	REQUIRE(large.operations > small.operations + 1000);
	const double perOperation =
	    static_cast<double>(large.allocations - std::min(large.allocations, small.allocations)) /
	    static_cast<double>(large.operations - small.operations);
	INFO("allocations per operation: " << perOperation);
	REQUIRE(perOperation < 0.1);
}

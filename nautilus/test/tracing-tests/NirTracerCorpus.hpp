// The corpus of the direct NIR tracer tests: traced functions and the helpers that trace them.
#pragma once

#include "DistinctSitesFunctions.hpp"
#include "nautilus/CompilableFunction.hpp"
#include "nautilus/Engine.hpp"
#include "nautilus/common/Arena.hpp"
#include "nautilus/conversion/Conversion.hpp"
#include "nautilus/extensions/ExtensionRegistry.hpp"
#include "nautilus/function.hpp"
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

namespace nautilus::testing::nirtracer {

inline val<int32_t> linearSum(val<int32_t> a, val<int32_t> b) {
	return (a + b) * a - b;
}

extern "C" inline int64_t nirTracerCorpusFirst(int64_t a, int64_t b) noexcept {
	return a ^ (b + 1);
}
extern "C" inline int32_t nirTracerCorpusSecond(int32_t a) noexcept {
	return a + 1;
}

/// Runtime calls: the same function twice in a row and again after another, with other argument values, so that the
/// callee, signature and pointer entries are interned once and every later call finds them.
inline val<int64_t> runtimeCalls(val<int64_t> x) {
	val<int64_t> r = invoke(nirTracerCorpusFirst, x, val<int64_t>(1));
	r = invoke(nirTracerCorpusFirst, r, val<int64_t>(2));
	val<int32_t> small = invoke(nirTracerCorpusSecond, val<int32_t>(7));
	r = invoke(nirTracerCorpusFirst, r, val<int64_t>(small));
	val<int32_t> other = invoke(nirTracerCorpusSecond, small);
	return r + val<int64_t>(other);
}

extern "C" inline int32_t nirTracerCorpusDivideTenBy(int32_t a) noexcept {
	return a == 0 ? 0 : 10 / a;
}

/// The short-circuit guard written out: constants returned on several paths (the ones the tracer reaches again and
/// hoists) in a function whose calls have arguments.
inline val<int32_t> guardedCalls(val<int32_t> x) {
	if (x != 0) {
		if (invoke(nirTracerCorpusDivideTenBy, x) > 1) {
			return invoke(nirTracerCorpusDivideTenBy, x);
		}
	}
	return val<int32_t>(0);
}

inline val<int32_t> ifElse(val<int32_t> a, val<int32_t> b) {
	val<int32_t> r = 0;
	if (a < b) {
		r = a;
	} else {
		r = b;
	}
	return r;
}

inline val<int32_t> countedLoop(val<int32_t> n) {
	val<int32_t> sum = 0;
	for (val<int32_t> i = 0; i < n; i = i + 1) {
		sum = sum + i;
	}
	return sum;
}

// a val initialised from a constant and assigned later, in a loop
inline val<int32_t> constantAssignedInLoop(val<int32_t> n) {
	val<int32_t> k = 5;
	for (val<int32_t> i = 0; i < n; i = i + 1) {
		k = k + 1;
	}
	return k;
}

// a copy that is assigned later, in a loop
inline val<int32_t> copyAssignedInLoop(val<int32_t> a, val<int32_t> n) {
	val<int32_t> c = a;
	for (val<int32_t> i = 0; i < n; i = i + 1) {
		c = c + a;
	}
	return c;
}

// #95: a copy constructed from different sources on the two arms of a native ternary
inline val<int32_t> copyFromTwoSources(val<int32_t> a, val<int32_t> b) {
	val<int32_t> c = (a < b) ? a : b;
	return c + 1;
}

inline val<int32_t> selectOperation(val<bool> condition, val<int32_t> a, val<int32_t> b) {
	return select(condition, a, b);
}

inline val<int32_t> checkedDivision(val<int32_t> a, val<int32_t> b) {
	return a / b + a % b;
}

// a zero-trip loop in an if-arm, merged after the if: the constant after the merge is declared on one path only
inline val<float> zeroTripLoopMerge(val<float> p) {
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

inline val<int32_t> nestedIf(val<int32_t> a, val<int32_t> b) {
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
inline uint16_t testOpcode() {
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

inline val<int32_t> extensionOperation(val<int32_t> a, val<int32_t> b) {
	return val<int32_t>(
	    tracing::traceExtOp(testOpcode(), static_cast<uint8_t>(nir::TypeId::I32), {a.state, b.state, a.state}));
}

struct CorpusEntry {
	std::string name;
	std::function<void()> wrapper;
	/// The 0.3 tracer cannot record it
	bool directOnly = false;
};

inline std::vector<CorpusEntry> corpus() {
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

inline nir::Module traceDirect(const std::function<void()>& wrapper, common::Arena& arena,
                               uint64_t* operations = nullptr) {
	std::list<compiler::CompilableFunction> functions;
	functions.emplace_back("execute", wrapper);
	auto result = tracing::nirtrace::NirTracer::Trace(functions, engine::Options {}, arena);
	if (operations != nullptr) {
		*operations = result.statistics.operations;
	}
	return std::move(result.module);
}

inline nir::Module trace03(const std::function<void()>& wrapper, common::Arena& arena) {
	std::list<compiler::CompilableFunction> functions;
	functions.emplace_back("execute", wrapper);
	auto module = tracing::TraceContext::Trace(functions, engine::Options {}, arena);
	return tracing::nirtrace::toPreSSA(*module);
}

inline std::string diagnostics(const nir::Module& m) {
	std::string text;
	for (const auto& d : nir::verify(m)) {
		text += d.str() + "\n";
	}
	return text;
}

inline std::string goldenPath(const std::string& name) {
	return std::string(NIR_TRACE_GOLDEN_DIR) + "/" + name + ".nir";
}

inline std::string readFile(const std::string& path) {
	std::ifstream in(path);
	std::stringstream buffer;
	buffer << in.rdbuf();
	return buffer.str();
}

} // namespace nautilus::testing::nirtracer

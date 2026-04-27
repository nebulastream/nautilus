/// Reproducer for a tracer SSA-merge crash that fires from
/// processControlFlowMerge with `oi.blockIndex == currentBlockIndex`.
///
/// Shape: 4 byte loads (modelling 2 nullable booleans), Kleene 3-valued AND,
/// single `if (keep) { invoke(...); 4 stores; }` with no else and no early
/// return. The post-`if` join is the implicit function-end.

#include "ExecutionTest.hpp"
#include "nautilus/Engine.hpp"
#include "nautilus/common/FunctionAttributes.hpp"
#include "nautilus/function.hpp"
#include "nautilus/select.hpp"
#include "nautilus/val.hpp"
#include "nautilus/val_ptr.hpp"
#include <catch2/catch_all.hpp>
#include <cstdint>

namespace nautilus::engine {

// Opaque host function: stand-in for a row-append sink.
extern "C" std::uint8_t* opaqueAppend(std::uint8_t* outBase) {
	return outBase;
}

// (1) The repro shape from the bug report — invoke + 4 stores in the if body, no else.
void threeValuedAndRepro(val<const std::uint8_t*> row, val<std::uint8_t*> outBase) {
	val<bool> av = *static_cast<val<const bool*>>(row + val<std::uint32_t>(0));
	val<bool> aIsNull = *static_cast<val<const bool*>>(row + val<std::uint32_t>(1));
	val<bool> bv = *static_cast<val<const bool*>>(row + val<std::uint32_t>(2));
	val<bool> bIsNull = *static_cast<val<const bool*>>(row + val<std::uint32_t>(3));

	val<bool> naive = av && bv;
	val<bool> aIsFalseSure = (!aIsNull) && (!av);
	val<bool> bIsFalseSure = (!bIsNull) && (!bv);
	val<bool> eitherFalseSure = aIsFalseSure || bIsFalseSure;
	val<bool> anyNull = aIsNull || bIsNull;
	val<bool> resultIsNull = nautilus::select(eitherFalseSure, val<bool>(false), anyNull);

	val<bool> keep = nautilus::select(resultIsNull, val<bool>(false), naive);

	if (keep) {
		constexpr FunctionAttributes appendAttrs {ModRefInfo::ModRef, true, true};
		val<std::uint8_t*> outRow = invoke(appendAttrs, opaqueAppend, outBase);

		*static_cast<val<bool*>>(outRow + val<std::uint32_t>(0)) = av;
		*static_cast<val<bool*>>(outRow + val<std::uint32_t>(1)) = aIsNull;
		*static_cast<val<bool*>>(outRow + val<std::uint32_t>(2)) = bv;
		*static_cast<val<bool*>>(outRow + val<std::uint32_t>(3)) = bIsNull;
	}
}

// (Variant 1) drop invoke, keep stores
void threeValuedAndRepro_NoInvoke(val<const std::uint8_t*> row, val<std::uint8_t*> outBase) {
	val<bool> av = *static_cast<val<const bool*>>(row + val<std::uint32_t>(0));
	val<bool> aIsNull = *static_cast<val<const bool*>>(row + val<std::uint32_t>(1));
	val<bool> bv = *static_cast<val<const bool*>>(row + val<std::uint32_t>(2));
	val<bool> bIsNull = *static_cast<val<const bool*>>(row + val<std::uint32_t>(3));

	val<bool> naive = av && bv;
	val<bool> aIsFalseSure = (!aIsNull) && (!av);
	val<bool> bIsFalseSure = (!bIsNull) && (!bv);
	val<bool> eitherFalseSure = aIsFalseSure || bIsFalseSure;
	val<bool> anyNull = aIsNull || bIsNull;
	val<bool> resultIsNull = nautilus::select(eitherFalseSure, val<bool>(false), anyNull);
	val<bool> keep = nautilus::select(resultIsNull, val<bool>(false), naive);

	if (keep) {
		*static_cast<val<bool*>>(outBase + val<std::uint32_t>(0)) = av;
		*static_cast<val<bool*>>(outBase + val<std::uint32_t>(1)) = aIsNull;
		*static_cast<val<bool*>>(outBase + val<std::uint32_t>(2)) = bv;
		*static_cast<val<bool*>>(outBase + val<std::uint32_t>(3)) = bIsNull;
	}
}

// (Variant 2) keep invoke, drop stores
void threeValuedAndRepro_NoStores(val<const std::uint8_t*> row, val<std::uint8_t*> outBase) {
	val<bool> av = *static_cast<val<const bool*>>(row + val<std::uint32_t>(0));
	val<bool> aIsNull = *static_cast<val<const bool*>>(row + val<std::uint32_t>(1));
	val<bool> bv = *static_cast<val<const bool*>>(row + val<std::uint32_t>(2));
	val<bool> bIsNull = *static_cast<val<const bool*>>(row + val<std::uint32_t>(3));

	val<bool> naive = av && bv;
	val<bool> aIsFalseSure = (!aIsNull) && (!av);
	val<bool> bIsFalseSure = (!bIsNull) && (!bv);
	val<bool> eitherFalseSure = aIsFalseSure || bIsFalseSure;
	val<bool> anyNull = aIsNull || bIsNull;
	val<bool> resultIsNull = nautilus::select(eitherFalseSure, val<bool>(false), anyNull);
	val<bool> keep = nautilus::select(resultIsNull, val<bool>(false), naive);

	if (keep) {
		constexpr FunctionAttributes appendAttrs {ModRefInfo::ModRef, true, true};
		invoke(appendAttrs, opaqueAppend, outBase);
	}
}

// (Variant 3) add empty else { }
void threeValuedAndRepro_WithElse(val<const std::uint8_t*> row, val<std::uint8_t*> outBase) {
	val<bool> av = *static_cast<val<const bool*>>(row + val<std::uint32_t>(0));
	val<bool> aIsNull = *static_cast<val<const bool*>>(row + val<std::uint32_t>(1));
	val<bool> bv = *static_cast<val<const bool*>>(row + val<std::uint32_t>(2));
	val<bool> bIsNull = *static_cast<val<const bool*>>(row + val<std::uint32_t>(3));

	val<bool> naive = av && bv;
	val<bool> aIsFalseSure = (!aIsNull) && (!av);
	val<bool> bIsFalseSure = (!bIsNull) && (!bv);
	val<bool> eitherFalseSure = aIsFalseSure || bIsFalseSure;
	val<bool> anyNull = aIsNull || bIsNull;
	val<bool> resultIsNull = nautilus::select(eitherFalseSure, val<bool>(false), anyNull);
	val<bool> keep = nautilus::select(resultIsNull, val<bool>(false), naive);

	if (keep) {
		constexpr FunctionAttributes appendAttrs {ModRefInfo::ModRef, true, true};
		val<std::uint8_t*> outRow = invoke(appendAttrs, opaqueAppend, outBase);
		*static_cast<val<bool*>>(outRow + val<std::uint32_t>(0)) = av;
		*static_cast<val<bool*>>(outRow + val<std::uint32_t>(1)) = aIsNull;
		*static_cast<val<bool*>>(outRow + val<std::uint32_t>(2)) = bv;
		*static_cast<val<bool*>>(outRow + val<std::uint32_t>(3)) = bIsNull;
	} else {
		// no-op
	}
}

// (Variant 5) replace &&/||/! with branchless select
void threeValuedAndRepro_Branchless(val<const std::uint8_t*> row, val<std::uint8_t*> outBase) {
	val<bool> av = *static_cast<val<const bool*>>(row + val<std::uint32_t>(0));
	val<bool> aIsNull = *static_cast<val<const bool*>>(row + val<std::uint32_t>(1));
	val<bool> bv = *static_cast<val<const bool*>>(row + val<std::uint32_t>(2));
	val<bool> bIsNull = *static_cast<val<const bool*>>(row + val<std::uint32_t>(3));

	val<bool> naive = nautilus::select(av, bv, val<bool>(false));
	val<bool> notAIsNull = nautilus::select(aIsNull, val<bool>(false), val<bool>(true));
	val<bool> notAv = nautilus::select(av, val<bool>(false), val<bool>(true));
	val<bool> aIsFalseSure = nautilus::select(notAIsNull, notAv, val<bool>(false));
	val<bool> notBIsNull = nautilus::select(bIsNull, val<bool>(false), val<bool>(true));
	val<bool> notBv = nautilus::select(bv, val<bool>(false), val<bool>(true));
	val<bool> bIsFalseSure = nautilus::select(notBIsNull, notBv, val<bool>(false));
	val<bool> eitherFalseSure = nautilus::select(aIsFalseSure, val<bool>(true), bIsFalseSure);
	val<bool> anyNull = nautilus::select(aIsNull, val<bool>(true), bIsNull);
	val<bool> resultIsNull = nautilus::select(eitherFalseSure, val<bool>(false), anyNull);
	val<bool> keep = nautilus::select(resultIsNull, val<bool>(false), naive);

	if (keep) {
		constexpr FunctionAttributes appendAttrs {ModRefInfo::ModRef, true, true};
		val<std::uint8_t*> outRow = invoke(appendAttrs, opaqueAppend, outBase);
		*static_cast<val<bool*>>(outRow + val<std::uint32_t>(0)) = av;
		*static_cast<val<bool*>>(outRow + val<std::uint32_t>(1)) = aIsNull;
		*static_cast<val<bool*>>(outRow + val<std::uint32_t>(2)) = bv;
		*static_cast<val<bool*>>(outRow + val<std::uint32_t>(3)) = bIsNull;
	}
}

// (Variant 6) reduce to 2 loads + single column AND-of-(value, !isNull)
void threeValuedAndRepro_Minimal(val<const std::uint8_t*> row, val<std::uint8_t*> outBase) {
	val<bool> av = *static_cast<val<const bool*>>(row + val<std::uint32_t>(0));
	val<bool> aIsNull = *static_cast<val<const bool*>>(row + val<std::uint32_t>(1));

	val<bool> keep = av && (!aIsNull);

	if (keep) {
		*static_cast<val<bool*>>(outBase + val<std::uint32_t>(0)) = av;
	}
}

static std::string pickBackend() {
#if defined(ENABLE_TRACING) && defined(ENABLE_MLIR_BACKEND)
	return "mlir";
#elif defined(ENABLE_TRACING) && defined(ENABLE_C_BACKEND)
	return "cpp";
#elif defined(ENABLE_TRACING) && defined(ENABLE_BC_BACKEND)
	return "bc";
#else
	return "";
#endif
}

TEST_CASE("ThreeValuedAndMergeRepro main shape compiles - lazyTracing") {
	auto backend = pickBackend();
	if (backend.empty()) {
		SKIP("No compilation backend available");
	}
	engine::Options opts;
	opts.setOption("engine.backend", backend);
	opts.setOption("engine.traceMode", "lazyTracing");
	engine::NautilusEngine engine(opts);

	auto module = engine.createModule();
	module.registerFunction<void(val<const std::uint8_t*>, val<std::uint8_t*>)>("repro", threeValuedAndRepro);

	auto compiled = module.compile();
	auto fn = compiled.getFunction<void(const std::uint8_t*, std::uint8_t*)>("repro");
	std::uint8_t in[4] = {1, 0, 1, 0};
	std::uint8_t out[4] = {0, 0, 0, 0};
	fn(in, out);
	REQUIRE(out[0] == 1);
}

TEST_CASE("ThreeValuedAndMergeRepro main shape compiles - exceptionBased") {
	auto backend = pickBackend();
	if (backend.empty()) {
		SKIP("No compilation backend available");
	}
	engine::Options opts;
	opts.setOption("engine.backend", backend);
	opts.setOption("engine.traceMode", "exceptionBased");
	engine::NautilusEngine engine(opts);

	auto module = engine.createModule();
	module.registerFunction<void(val<const std::uint8_t*>, val<std::uint8_t*>)>("repro", threeValuedAndRepro);

	auto compiled = module.compile();
	auto fn = compiled.getFunction<void(const std::uint8_t*, std::uint8_t*)>("repro");
	std::uint8_t in[4] = {1, 0, 1, 0};
	std::uint8_t out[4] = {0, 0, 0, 0};
	fn(in, out);
	REQUIRE(out[0] == 1);
}

TEST_CASE("ThreeValuedAndMergeRepro variant 1 (no invoke)") {
	auto backend = pickBackend();
	if (backend.empty()) {
		SKIP("No compilation backend available");
	}
	engine::Options opts;
	opts.setOption("engine.backend", backend);
	engine::NautilusEngine engine(opts);
	auto module = engine.createModule();
	module.registerFunction<void(val<const std::uint8_t*>, val<std::uint8_t*>)>("v1", threeValuedAndRepro_NoInvoke);
	auto compiled = module.compile();
	(void) compiled;
}

TEST_CASE("ThreeValuedAndMergeRepro variant 2 (no stores)") {
	auto backend = pickBackend();
	if (backend.empty()) {
		SKIP("No compilation backend available");
	}
	engine::Options opts;
	opts.setOption("engine.backend", backend);
	engine::NautilusEngine engine(opts);
	auto module = engine.createModule();
	module.registerFunction<void(val<const std::uint8_t*>, val<std::uint8_t*>)>("v2", threeValuedAndRepro_NoStores);
	auto compiled = module.compile();
	(void) compiled;
}

TEST_CASE("ThreeValuedAndMergeRepro variant 3 (with empty else)") {
	auto backend = pickBackend();
	if (backend.empty()) {
		SKIP("No compilation backend available");
	}
	engine::Options opts;
	opts.setOption("engine.backend", backend);
	engine::NautilusEngine engine(opts);
	auto module = engine.createModule();
	module.registerFunction<void(val<const std::uint8_t*>, val<std::uint8_t*>)>("v3", threeValuedAndRepro_WithElse);
	auto compiled = module.compile();
	(void) compiled;
}

TEST_CASE("ThreeValuedAndMergeRepro variant 5 (branchless predicate)") {
	auto backend = pickBackend();
	if (backend.empty()) {
		SKIP("No compilation backend available");
	}
	engine::Options opts;
	opts.setOption("engine.backend", backend);
	engine::NautilusEngine engine(opts);
	auto module = engine.createModule();
	module.registerFunction<void(val<const std::uint8_t*>, val<std::uint8_t*>)>("v5", threeValuedAndRepro_Branchless);
	auto compiled = module.compile();
	(void) compiled;
}

TEST_CASE("ThreeValuedAndMergeRepro variant 6 (minimal: 2 loads, 1 AND, 1 store)") {
	auto backend = pickBackend();
	if (backend.empty()) {
		SKIP("No compilation backend available");
	}
	engine::Options opts;
	opts.setOption("engine.backend", backend);
	engine::NautilusEngine engine(opts);
	auto module = engine.createModule();
	module.registerFunction<void(val<const std::uint8_t*>, val<std::uint8_t*>)>("v6", threeValuedAndRepro_Minimal);
	auto compiled = module.compile();
	(void) compiled;
}

} // namespace nautilus::engine

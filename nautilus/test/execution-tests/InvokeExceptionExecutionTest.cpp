#include "ExecutionTest.hpp"
#include <catch2/catch_test_macros.hpp>
#include <cstdint>
#include <list>
#include <nautilus/CompilableFunction.hpp>
#include <nautilus/Engine.hpp>
#include <nautilus/common/ExceptionTransport.hpp>
#include <nautilus/compiler/DumpHandler.hpp>
#include <nautilus/compiler/backends/CompilationBackend.hpp>
#include <nautilus/compiler/ir/IRGraph.hpp>
#include <nautilus/compiler/ir/blocks/BasicBlock.hpp>
#include <nautilus/compiler/ir/operations/FunctionOperation.hpp>
#include <nautilus/compiler/ir/operations/IndirectCallOperation.hpp>
#include <nautilus/compiler/ir/passes/ExceptionRegionPreparationPass.hpp>
#include <nautilus/compiler/ir/passes/IRPassManager.hpp>
#include <nautilus/function.hpp>
#include <nautilus/nautilus_function.hpp>
#include <nautilus/tracing/ExceptionBasedTraceContext.hpp>
#include <nautilus/tracing/LazyTraceContext.hpp>
#include <nautilus/tracing/TracingUtil.hpp>
#include <nautilus/tracing/phases/SSACreationPhase.hpp>
#include <nautilus/tracing/phases/TraceToIRConversionPhase.hpp>
#include <nautilus/val_std.hpp>
#include <stdexcept>
#include <vector>

namespace nautilus { namespace {

// Every test in this file exercises the exception-handling trace/IR path,
// which is only compiled when tracing is enabled. Guard the whole file body so
// a TRACING=OFF build does not link against tracing symbols it lacks.
#ifdef ENABLE_TRACING

thread_local uint32_t destructorCalls = 0;
thread_local int32_t destructorValues[4] = {};

struct ExceptionResult {
	int32_t value = 0;
	~ExceptionResult() noexcept {
		destructorValues[destructorCalls++] = value;
	}
};

void writeResult(ExceptionResult* result, int32_t value) noexcept {
	result->value = value;
}

void throwWhileWriting(ExceptionResult*, int32_t) {
	throw std::runtime_error("invoke failed");
}

void throwWithoutStruct() {
	throw std::runtime_error("invoke failed without cleanup");
}

val<int32_t> invokeNoexceptWithStruct() {
	val<ExceptionResult> result;
	invoke(writeResult, &result, val<int32_t> {42});
	return result.get(&ExceptionResult::value);
}

val<int32_t> invokeThrowingWithStruct() {
	val<ExceptionResult> result;
	invoke(throwWhileWriting, &result, val<int32_t> {42});
	return result.get(&ExceptionResult::value);
}

val<int32_t> invokeThrowingWithTwoStructs() {
	val<ExceptionResult> first;
	invoke(writeResult, &first, val<int32_t> {1});
	val<ExceptionResult> second;
	invoke(writeResult, &second, val<int32_t> {2});
	invoke(throwWhileWriting, &second, val<int32_t> {42});
	return 0;
}

val<int32_t> invokeThrowingWithoutStruct() {
	invoke(throwWithoutStruct);
	return 42;
}

void throwIfTrue(int32_t flag) {
	if (flag != 0) {
		throw std::runtime_error("conditional throw");
	}
}

// Conditionally throws, so the same registered function object can be called
// twice -- once throwing, once not -- to check the executable is reusable
// after an exception. `result` is cleaned up on both paths: the exceptional
// one via the landing pad, the normal one via the traced destructor call at
// its natural scope exit.
val<int32_t> invokeMaybeThrowingWithStruct(val<int32_t> shouldThrow) {
	val<ExceptionResult> result;
	invoke(writeResult, &result, val<int32_t> {7});
	invoke(throwIfTrue, shouldThrow);
	return result.get(&ExceptionResult::value);
}

// Move-constructs a val<Struct> while a sibling val<Struct> is already live,
// then throws. Regression coverage for the move constructor's destructor
// bookkeeping: val<T>'s move ctor must not remove-then-re-append the moved
// value's landing-pad registration (same address, same destructor, so the
// existing registration from the original -- pre-move -- construction already
// covers it), or the destructor of the moved-into object would run out of
// its correct reverse-construction-order position relative to a sibling
// that's still alive. `first` moves into `movedInto` while `second` stays
// where it was constructed; the throw must still run `second` before the
// moved value, matching the order they were originally constructed in.
val<int32_t> invokeThrowingAfterMove() {
	val<ExceptionResult> first;
	invoke(writeResult, &first, val<int32_t> {1});
	val<ExceptionResult> second;
	invoke(writeResult, &second, val<int32_t> {2});
	val<ExceptionResult> movedInto = std::move(first);
	invoke(throwWhileWriting, &movedInto, val<int32_t> {99});
	return 0;
}

// ---------------------------------------------------------------------------
// Nested Nautilus function calls: the caller's live val<Struct> destructors
// must be carried across the nested-call boundary so they run if the nested
// function throws.
// ---------------------------------------------------------------------------
thread_local int nestedDtorCalls = 0;

struct NestedCleanup {
	int v = 0;
	~NestedCleanup() noexcept {
		nestedDtorCalls++;
	}
};

void nestedWriteResult(NestedCleanup* result, int32_t value) noexcept {
	result->v = value;
}

void nestedThrowRuntime() {
	throw std::runtime_error("nested");
}

val<int32_t> nestedInner() {
	invoke(nestedThrowRuntime);
	return 0;
}
static auto nestedInnerFn = NautilusFunction {"nestedInner", nestedInner};

val<int32_t> nestedOuter(val<int32_t> outer_arg) {
	val<NestedCleanup> outer;
	invoke(nestedWriteResult, &outer, val<int32_t> {1});
	nestedInnerFn();
	return outer_arg;
}

val<int64_t> nestedNoexceptInner(val<int64_t> x) noexcept {
	return x + 1;
}
static auto nestedNoexceptInnerFn = NautilusFunction {"nestedNoexceptInner", nestedNoexceptInner};

val<int32_t> nestedNoexceptOuter(val<int32_t> outer_arg) {
	val<NestedCleanup> outer;
	invoke(nestedWriteResult, &outer, val<int32_t> {1});
	[[maybe_unused]] auto r = nestedNoexceptInnerFn(val<int64_t> {outer_arg});
	return outer_arg;
}

engine::NautilusEngine makeMlirEngine(const std::string& traceMode) {
	engine::Options options;
	options.setOption("engine.Compilation", true);
	options.setOption("engine.backend", std::string("mlir"));
	options.setOption("engine.compilationStrategy", std::string("legacy"));
	options.setOption("engine.traceMode", traceMode);
	options.setOption("mlir.enableMultithreading", false);
	return engine::NautilusEngine {options};
}

engine::NautilusEngine makeInterpreterEngine() {
	engine::Options options;
	options.setOption("engine.Compilation", false);
	return engine::NautilusEngine {options};
}

engine::NautilusEngine makeCppEngine(const std::string& traceMode) {
	engine::Options options;
	options.setOption("engine.Compilation", true);
	options.setOption("engine.backend", std::string("cpp"));
	options.setOption("engine.compilationStrategy", std::string("legacy"));
	options.setOption("engine.traceMode", traceMode);
	return engine::NautilusEngine {options};
}

// These engines mirror the backends they target, which are only built when
// both their backend option and ENABLE_TRACING are on. Guard them identically
// so a TRACING=OFF / backend-off build doesn't trip -Werror=unused-function.
#ifdef ENABLE_BC_BACKEND
engine::NautilusEngine makeBcEngine(const std::string& traceMode) {
	engine::Options options;
	options.setOption("engine.Compilation", true);
	options.setOption("engine.backend", std::string("bc"));
	options.setOption("engine.compilationStrategy", std::string("legacy"));
	options.setOption("engine.traceMode", traceMode);
	return engine::NautilusEngine {options};
}

// Same as makeBcEngine, but pins bc.dispatch instead of leaving it at the
// "call" default. The interpreter's CHECK_PENDING_EXCEPTION handling differs
// by dispatch mode (BCInterpreter::execute's switch-path loop vs. its
// call-path loop, each gated by CodeBlock::hasPendingCheck; the threaded path
// takes a separate computed-goto label entirely) -- exercise every mode
// against a throwing call so all three are covered, not just the default.
engine::NautilusEngine makeBcEngine(const std::string& traceMode, const std::string& dispatch) {
	engine::Options options;
	options.setOption("engine.Compilation", true);
	options.setOption("engine.backend", std::string("bc"));
	options.setOption("engine.compilationStrategy", std::string("legacy"));
	options.setOption("engine.traceMode", traceMode);
	options.setOption("bc.dispatch", dispatch);
	return engine::NautilusEngine {options};
}
#endif // ENABLE_BC_BACKEND

#ifdef ENABLE_TBC_BACKEND
engine::NautilusEngine makeTbcEngine(const std::string& traceMode) {
	engine::Options options;
	options.setOption("engine.Compilation", true);
	options.setOption("engine.backend", std::string("tbc"));
	options.setOption("engine.compilationStrategy", std::string("legacy"));
	options.setOption("engine.traceMode", traceMode);
	return engine::NautilusEngine {options};
}

#ifdef ENABLE_TBC_JIT
// tbc's copy-and-patch JIT. Stitched code carries no unwind info, so it routes
// exceptions through VMContext::pendingException and the entry shim rethrows --
// a different mechanism from the interpreter's, and therefore worth its own
// peer here rather than trusting the interpreter's result to cover it.
engine::NautilusEngine makeTbcJitEngine(const std::string& traceMode) {
	engine::Options options;
	options.setOption("engine.Compilation", true);
	options.setOption("engine.backend", std::string("tbc"));
	options.setOption("tbc.mode", std::string("jit"));
	options.setOption("engine.compilationStrategy", std::string("legacy"));
	options.setOption("engine.traceMode", traceMode);
	return engine::NautilusEngine {options};
}
#endif // ENABLE_TBC_JIT
#endif // ENABLE_TBC_BACKEND

#if defined(ENABLE_ASMJIT_BACKEND)
engine::NautilusEngine makeAsmJitEngine(const std::string& traceMode) {
	engine::Options options;
	options.setOption("engine.Compilation", true);
	options.setOption("engine.backend", std::string("asmjit"));
	options.setOption("engine.compilationStrategy", std::string("legacy"));
	options.setOption("engine.traceMode", traceMode);
	return engine::NautilusEngine {options};
}
#endif // ENABLE_ASMJIT_BACKEND

TEST_CASE("MLIR invokes unwind live val<Struct> destructors") {
	for (const auto& traceMode : {std::string("exceptionBasedTracing"), std::string("lazyTracing")}) {
		DYNAMIC_SECTION(traceMode) {
			auto engine = makeMlirEngine(traceMode);
			auto function = engine.registerFunction(invokeThrowingWithStruct);
			destructorCalls = 0;
			REQUIRE_THROWS_AS(function(), std::runtime_error);
			REQUIRE(destructorCalls == 1);
		}
	}
}

TEST_CASE("noexcept MLIR invokes retain the direct call path") {
	auto engine = makeMlirEngine("lazyTracing");
	auto function = engine.registerFunction(invokeNoexceptWithStruct);
	destructorCalls = 0;
	REQUIRE(function() == 42);
	REQUIRE(destructorCalls == 1);
}

TEST_CASE("MLIR exceptional cleanups run in reverse construction order") {
	auto engine = makeMlirEngine("lazyTracing");
	auto function = engine.registerFunction(invokeThrowingWithTwoStructs);
	destructorCalls = 0;
	REQUIRE_THROWS_AS(function(), std::runtime_error);
	REQUIRE(destructorCalls == 2);
	REQUIRE(destructorValues[0] == 2);
	REQUIRE(destructorValues[1] == 1);
}

TEST_CASE("interpreter invokes use native C++ exception unwinding") {
	auto engine = makeInterpreterEngine();
	auto function = engine.registerFunction(invokeThrowingWithStruct);
	destructorCalls = 0;
	REQUIRE_THROWS_AS(function(), std::runtime_error);
	REQUIRE(destructorCalls == 1);
}

TEST_CASE("CPP backend invokes unwind live val<Struct> destructors") {
	for (const auto& traceMode : {std::string("exceptionBasedTracing"), std::string("lazyTracing")}) {
		DYNAMIC_SECTION(traceMode) {
			auto engine = makeCppEngine(traceMode);
			auto function = engine.registerFunction(invokeThrowingWithStruct);
			destructorCalls = 0;
			REQUIRE_THROWS_AS(function(), std::runtime_error);
			REQUIRE(destructorCalls == 1);
		}
	}
}

TEST_CASE("CPP backend exceptional cleanups run in reverse construction order") {
	auto engine = makeCppEngine("lazyTracing");
	auto function = engine.registerFunction(invokeThrowingWithTwoStructs);
	destructorCalls = 0;
	REQUIRE_THROWS_AS(function(), std::runtime_error);
	REQUIRE(destructorCalls == 2);
	REQUIRE(destructorValues[0] == 2);
	REQUIRE(destructorValues[1] == 1);
}

TEST_CASE("CPP backend throwing invokes without live structs rethrow") {
	auto engine = makeCppEngine("lazyTracing");
	auto function = engine.registerFunction(invokeThrowingWithoutStruct);
	REQUIRE_THROWS_AS(function(), std::runtime_error);
}

#ifdef ENABLE_BC_BACKEND
TEST_CASE("BC backend invokes unwind live val<Struct> destructors") {
	for (const auto& traceMode : {std::string("exceptionBasedTracing"), std::string("lazyTracing")}) {
		DYNAMIC_SECTION(traceMode) {
			auto engine = makeBcEngine(traceMode);
			auto function = engine.registerFunction(invokeThrowingWithStruct);
			destructorCalls = 0;
			REQUIRE_THROWS_AS(function(), std::runtime_error);
			REQUIRE(destructorCalls == 1);
		}
	}
}

TEST_CASE("BC backend exceptional cleanups run in reverse construction order") {
	auto engine = makeBcEngine("lazyTracing");
	auto function = engine.registerFunction(invokeThrowingWithTwoStructs);
	destructorCalls = 0;
	REQUIRE_THROWS_AS(function(), std::runtime_error);
	REQUIRE(destructorCalls == 2);
	REQUIRE(destructorValues[0] == 2);
	REQUIRE(destructorValues[1] == 1);
}

TEST_CASE("BC backend unwinds destructors under every dispatch mode") {
	// BCInterpreter::execute() picks a loop variant per basic block based on
	// CodeBlock::hasPendingCheck, nested inside the call/switch dispatch-mode
	// branch; the threaded path handles the check as its own computed-goto
	// label. All three need to actually take the pending-exception branch, not
	// just agree on ordinary (non-throwing) results the way BCDispatchModeTest
	// checks.
	for (const auto& dispatch : {std::string("call"), std::string("switch"), std::string("threaded")}) {
		DYNAMIC_SECTION(dispatch) {
			auto engine = makeBcEngine("lazyTracing", dispatch);
			auto function = engine.registerFunction(invokeThrowingWithStruct);
			destructorCalls = 0;
			REQUIRE_THROWS_AS(function(), std::runtime_error);
			REQUIRE(destructorCalls == 1);
		}
	}
}
#endif // ENABLE_BC_BACKEND

#ifdef ENABLE_TBC_BACKEND
TEST_CASE("TBC backend invokes unwind live val<Struct> destructors") {
	for (const auto& traceMode : {std::string("exceptionBasedTracing"), std::string("lazyTracing")}) {
		DYNAMIC_SECTION(traceMode) {
			auto engine = makeTbcEngine(traceMode);
			auto function = engine.registerFunction(invokeThrowingWithStruct);
			destructorCalls = 0;
			REQUIRE_THROWS_AS(function(), std::runtime_error);
			REQUIRE(destructorCalls == 1);
		}
	}
}

TEST_CASE("TBC backend exceptional cleanups run in reverse construction order") {
	auto engine = makeTbcEngine("lazyTracing");
	auto function = engine.registerFunction(invokeThrowingWithTwoStructs);
	destructorCalls = 0;
	REQUIRE_THROWS_AS(function(), std::runtime_error);
	REQUIRE(destructorCalls == 2);
	REQUIRE(destructorValues[0] == 2);
	REQUIRE(destructorValues[1] == 1);
}

#endif // ENABLE_TBC_BACKEND

#ifdef ENABLE_ASMJIT_BACKEND
TEST_CASE("AsmJit backend invokes unwind live val<Struct> destructors") {
	for (const auto& traceMode : {std::string("exceptionBasedTracing"), std::string("lazyTracing")}) {
		DYNAMIC_SECTION(traceMode) {
			auto engine = makeAsmJitEngine(traceMode);
			auto function = engine.registerFunction(invokeThrowingWithStruct);
			destructorCalls = 0;
			REQUIRE_THROWS_AS(function(), std::runtime_error);
			REQUIRE(destructorCalls == 1);
		}
	}
}

TEST_CASE("AsmJit backend exceptional cleanups run in reverse construction order") {
	auto engine = makeAsmJitEngine("lazyTracing");
	auto function = engine.registerFunction(invokeThrowingWithTwoStructs);
	destructorCalls = 0;
	REQUIRE_THROWS_AS(function(), std::runtime_error);
	REQUIRE(destructorCalls == 2);
	REQUIRE(destructorValues[0] == 2);
	REQUIRE(destructorValues[1] == 1);
}
#endif // ENABLE_ASMJIT_BACKEND

TEST_CASE("throwing invokes without live structs retain exception handling") {
	using TraceFn = std::unique_ptr<tracing::TraceModule> (*)(std::list<compiler::CompilableFunction>&,
	                                                          const engine::Options&, common::Arena&);
	for (const auto& [traceMode, traceFn] :
	     {std::pair {"exceptionBasedTracing", TraceFn {tracing::ExceptionBasedTraceContext::Trace}},
	      std::pair {"lazyTracing", TraceFn {tracing::LazyTraceContext::Trace}}}) {
		DYNAMIC_SECTION(traceMode) {
			common::Arena arena;
			std::list<compiler::CompilableFunction> functions;
			functions.emplace_back("execute", [] { (void) invokeThrowingWithoutStruct(); });
			auto module = traceFn(functions, engine::Options {}, arena);
			auto* trace = module->getFunction("execute");

			auto exceptionCalls = size_t {0};
			for (const auto* block : trace->getBlocks()) {
				for (const auto* operation : block->operations) {
					if (operation->op == tracing::Op::CALL_WITH_EXCEPTION_HANDLING) {
						const auto* call = std::get<tracing::FunctionCall*>(operation->input[0]);
						REQUIRE(call->destructors.empty());
						++exceptionCalls;
					}
				}
			}
			REQUIRE(exceptionCalls == 1);

			auto engine = makeMlirEngine(traceMode);
			auto function = engine.registerFunction(invokeThrowingWithoutStruct);
			REQUIRE_THROWS_AS(function(), std::runtime_error);
		}
	}
}

TEST_CASE("indirect invokes carry live struct destructors through trace and IR") {
	using TraceFn = std::unique_ptr<tracing::TraceModule> (*)(std::list<compiler::CompilableFunction>&,
	                                                          const engine::Options&, common::Arena&);
	for (const auto& [traceMode, traceFn] :
	     {std::pair {"exceptionBasedTracing", TraceFn {tracing::ExceptionBasedTraceContext::Trace}},
	      std::pair {"lazyTracing", TraceFn {tracing::LazyTraceContext::Trace}}}) {
		DYNAMIC_SECTION(traceMode) {
			common::Arena arena;
			std::list<compiler::CompilableFunction> functions;
			functions.emplace_back("execute", [] {
				val<ExceptionResult> result;
				auto& fnPtrRef = tracing::traceConstant(
				    Type::ptr, tracing::createConstLiteral(reinterpret_cast<void*>(&throwWhileWriting)));
				std::vector<tracing::TypedValueRef> arguments;
				tracing::traceIndirectCallWithExceptionHandling(
				    fnPtrRef, Type::v, arguments, {},
				    reinterpret_cast<void*>(&compiler::captureThrowingCall<void, ExceptionResult*, int32_t>));
			});
			auto module = traceFn(functions, engine::Options {}, arena);
			auto* trace = module->getFunction("execute");

			auto indirectEhCalls = size_t {0};
			for (const auto* block : trace->getBlocks()) {
				for (const auto* operation : block->operations) {
					if (operation->op == tracing::Op::INDIRECT_CALL_WITH_EXCEPTION_HANDLING) {
						const auto* call = std::get<tracing::IndirectFunctionCall*>(operation->input[0]);
						REQUIRE(call->destructors.size() == 1);
						++indirectEhCalls;
					}
				}
			}
			REQUIRE(indirectEhCalls == 1);

			auto ir = tracing::TraceToIRConversionPhase().apply(std::move(module));
			auto irIndirectEhCalls = size_t {0};
			for (const auto* functionOperation : ir->getFunctionOperations()) {
				for (const auto* block : functionOperation->getBasicBlocks()) {
					for (const auto* operation : block->getOperations()) {
						if (const auto* indirect =
						        compiler::ir::dyn_cast<compiler::ir::IndirectCallOperation>(operation)) {
							REQUIRE(indirect->requiresExceptionHandling());
							REQUIRE(indirect->getDestructors().size() == 1);
							++irIndirectEhCalls;
						}
					}
				}
			}
			REQUIRE(irIndirectEhCalls == 1);
		}
	}
}

// ---------------------------------------------------------------------------
// Cross-backend execution tests (spec §9.5)
// ---------------------------------------------------------------------------

struct BackendSpec {
	/// Section name. May be a pseudo-backend such as "tbc-jit", which is a
	/// backend plus an option rather than a registry entry.
	std::string name;
	engine::NautilusEngine (*makeEngine)(const std::string&);
	/// CompilationBackendRegistry key, for the tests that drive a backend
	/// directly instead of going through an engine. Differs from `name` for
	/// pseudo-backends.
	std::string registryName;
	/// Options those same tests must pass to compile() to select the mode
	/// `name` stands for (empty for a plain backend).
	void (*compileOptions)(engine::Options&) = nullptr;
};

std::vector<BackendSpec> exceptionBackends() {
	std::vector<BackendSpec> backends;
#ifdef ENABLE_MLIR_BACKEND
	backends.push_back({"mlir", makeMlirEngine, "mlir"});
#endif
#ifdef ENABLE_C_BACKEND
	backends.push_back({"cpp", makeCppEngine, "cpp"});
#endif
#ifdef ENABLE_BC_BACKEND
	backends.push_back({"bc", makeBcEngine, "bc"});
#endif
#ifdef ENABLE_TBC_BACKEND
	backends.push_back({"tbc", makeTbcEngine, "tbc"});
#ifdef ENABLE_TBC_JIT
	// Gated on runtime availability the same way testing::availableBackends is:
	// tbc.mode=jit is strict and throws where stitched code cannot run.
	if (compiler::tbc::jit::jitRuntimeAvailable()) {
		backends.push_back({"tbc-jit", makeTbcJitEngine, "tbc",
		                    [](engine::Options& o) { o.setOption("tbc.mode", std::string("jit")); }});
	}
#endif
#endif
#if defined(ENABLE_ASMJIT_BACKEND)
	backends.push_back({"asmjit", makeAsmJitEngine, "asmjit"});
#endif
	return backends;
}

thread_local int32_t postInvokeCounter = 0;

void markPostInvoke() noexcept {
	++postInvokeCounter;
}

val<int32_t> invokeThrowingThenMark() {
	val<ExceptionResult> result;
	invoke(throwWhileWriting, &result, val<int32_t> {42});
	invoke(markPostInvoke);
	return 0;
}

val<int32_t> invokeVoidThrowing() {
	invoke(throwWithoutStruct);
	return 42;
}

TEST_CASE("no-struct throwing call propagates across backends") {
	for (const auto& backend : exceptionBackends()) {
		DYNAMIC_SECTION(backend.name) {
			for (const auto& traceMode : {std::string("exceptionBasedTracing"), std::string("lazyTracing")}) {
				DYNAMIC_SECTION(traceMode) {
					auto engine = backend.makeEngine(traceMode);
					auto function = engine.registerFunction(invokeThrowingWithoutStruct);
					destructorCalls = 0;
					REQUIRE_THROWS_AS(function(), std::runtime_error);
					REQUIRE(destructorCalls == 0);
				}
			}
		}
	}
}

TEST_CASE("operations after a throwing call are not executed") {
	for (const auto& backend : exceptionBackends()) {
		DYNAMIC_SECTION(backend.name) {
			for (const auto& traceMode : {std::string("exceptionBasedTracing"), std::string("lazyTracing")}) {
				DYNAMIC_SECTION(traceMode) {
					auto engine = backend.makeEngine(traceMode);
					auto function = engine.registerFunction(invokeThrowingThenMark);
					postInvokeCounter = 0;
					destructorCalls = 0;
					REQUIRE_THROWS_AS(function(), std::runtime_error);
					REQUIRE(destructorCalls == 1);
					REQUIRE(postInvokeCounter == 0);
				}
			}
		}
	}
}

TEST_CASE("void-returning throw propagates across backends") {
	for (const auto& backend : exceptionBackends()) {
		DYNAMIC_SECTION(backend.name) {
			for (const auto& traceMode : {std::string("exceptionBasedTracing"), std::string("lazyTracing")}) {
				DYNAMIC_SECTION(traceMode) {
					auto engine = backend.makeEngine(traceMode);
					auto function = engine.registerFunction(invokeVoidThrowing);
					REQUIRE_THROWS_AS(function(), std::runtime_error);
				}
			}
		}
	}
}

// ---------------------------------------------------------------------------
// Indirect-call execution tests (deferred from Task 5)
// ---------------------------------------------------------------------------

using TraceFn = std::unique_ptr<tracing::TraceModule> (*)(std::list<compiler::CompilableFunction>&,
                                                          const engine::Options&, common::Arena&);

std::shared_ptr<compiler::ir::IRGraph> traceIndirectThrowIR(TraceFn traceFn) {
	common::Arena arena;
	std::list<compiler::CompilableFunction> functions;
	functions.emplace_back("execute", [] {
		{
			val<ExceptionResult> result;
			auto& fnPtrRef = tracing::traceConstant(
			    Type::ptr, tracing::createConstLiteral(reinterpret_cast<void*>(&throwWhileWriting)));
			std::vector<tracing::TypedValueRef> arguments;
			tracing::traceIndirectCallWithExceptionHandling(
			    fnPtrRef, Type::v, arguments, {},
			    reinterpret_cast<void*>(&compiler::captureThrowingCall<void, ExceptionResult*, int32_t>));
		}
		tracing::traceReturnOperation(Type::v, tracing::TypedValueRef {});
	});
	auto module = traceFn(functions, engine::Options {}, arena);
	auto afterSSA = tracing::SSACreationPhase().apply(std::shared_ptr<tracing::TraceModule>(std::move(module)));
	auto ir = tracing::TraceToIRConversionPhase().apply(std::move(afterSSA));
	engine::Options passOpts;
	compiler::ir::IRPassManager passManager(passOpts);
	passManager.addPass(std::make_unique<compiler::ir::ExceptionRegionPreparationPass>());
	passManager.run(*ir);
	return ir;
}

TEST_CASE("indirect throwing invokes unwind destructors across backends") {
	using BackendList = std::vector<std::pair<std::string, TraceFn>>;
	const BackendList traceModes {
	    {"exceptionBasedTracing", tracing::ExceptionBasedTraceContext::Trace},
	    {"lazyTracing", tracing::LazyTraceContext::Trace},
	};
	for (const auto& backend : exceptionBackends()) {
		DYNAMIC_SECTION(backend.name) {
			for (const auto& [traceMode, traceFn] : traceModes) {
				DYNAMIC_SECTION(traceMode) {
					auto ir = traceIndirectThrowIR(traceFn);
					auto* compilationBackend =
					    compiler::CompilationBackendRegistry::getInstance()->getBackend(backend.registryName);
					// DumpHandler stores Options by reference, so the options must
					// outlive the compile() call (no temporaries here).
					engine::Options dumpOptions;
					compiler::DumpHandler dumpHandler(dumpOptions, "indirect-throw-test");
					engine::Options compileOptions;
					if (backend.compileOptions != nullptr) {
						backend.compileOptions(compileOptions);
					}
					auto executable = compilationBackend->compile(ir, dumpHandler, compileOptions, nullptr);
					auto function = executable->getInvocableMember<void>("execute");
					destructorCalls = 0;
					REQUIRE_THROWS_AS(function(), std::runtime_error);
					REQUIRE(destructorCalls == 1);
				}
			}
		}
	}
}

// ---------------------------------------------------------------------------
// Nested Nautilus function calls must carry the caller's active destructors
// so that when the nested function throws, the caller's live val<Struct>
// destructors are run.
// ---------------------------------------------------------------------------
TEST_CASE("nested Nautilus throw cleans outer live struct") {
	for (const auto& backend : exceptionBackends()) {
		DYNAMIC_SECTION(backend.name) {
			for (const auto& traceMode : {std::string("exceptionBasedTracing"), std::string("lazyTracing")}) {
				DYNAMIC_SECTION(traceMode) {
					auto engine = backend.makeEngine(traceMode);
					auto function = engine.registerFunction(nestedOuter);
					nestedDtorCalls = 0;
					REQUIRE_THROWS_AS(function(7), std::runtime_error);
					REQUIRE(nestedDtorCalls == 1);
				}
			}
		}
	}
}

TEST_CASE("noexcept nested Nautilus call stays on the direct path") {
	for (const auto& backend : exceptionBackends()) {
		DYNAMIC_SECTION(backend.name) {
			for (const auto& traceMode : {std::string("exceptionBasedTracing"), std::string("lazyTracing")}) {
				DYNAMIC_SECTION(traceMode) {
					auto engine = backend.makeEngine(traceMode);
					auto function = engine.registerFunction(nestedNoexceptOuter);
					nestedDtorCalls = 0;
					REQUIRE(function(42) == 42);
					REQUIRE(nestedDtorCalls == 1);
				}
			}
		}
	}
}

// ---------------------------------------------------------------------------
// val<Struct> move construction must not disturb landing-pad destructor
// order relative to still-live siblings (see invokeThrowingAfterMove above).
// ---------------------------------------------------------------------------
TEST_CASE("moving a live struct preserves reverse construction order") {
	for (const auto& backend : exceptionBackends()) {
		DYNAMIC_SECTION(backend.name) {
			for (const auto& traceMode : {std::string("exceptionBasedTracing"), std::string("lazyTracing")}) {
				DYNAMIC_SECTION(traceMode) {
					auto engine = backend.makeEngine(traceMode);
					auto function = engine.registerFunction(invokeThrowingAfterMove);
					destructorCalls = 0;
					REQUIRE_THROWS_AS(function(), std::runtime_error);
					REQUIRE(destructorCalls == 2);
					// `second` was constructed after `first`/`movedInto`'s storage, so
					// it must be destroyed first; `movedInto` (== first's original
					// storage) destructs second, in its original construction-order slot.
					REQUIRE(destructorValues[0] == 2);
					REQUIRE(destructorValues[1] == 1);
				}
			}
		}
	}
}

// ---------------------------------------------------------------------------
// The same compiled/registered function must remain callable after one
// invocation throws: the pending exception must not leak into (or be
// mistaken for one belonging to) the next call. Regression coverage for
// Invocable::operator()'s ExceptionFrameScope -- each call pushes and pops
// its own frame, so a throw on call N cannot affect call N+1's frame.
// ---------------------------------------------------------------------------
TEST_CASE("an executable is reusable after a call throws") {
	for (const auto& backend : exceptionBackends()) {
		DYNAMIC_SECTION(backend.name) {
			for (const auto& traceMode : {std::string("exceptionBasedTracing"), std::string("lazyTracing")}) {
				DYNAMIC_SECTION(traceMode) {
					auto engine = backend.makeEngine(traceMode);
					auto function = engine.registerFunction(invokeMaybeThrowingWithStruct);

					destructorCalls = 0;
					REQUIRE_THROWS_AS(function(1), std::runtime_error);
					REQUIRE(destructorCalls == 1);

					// Same function object, called again: must run normally, not
					// inherit or rethrow anything left over from the first call.
					destructorCalls = 0;
					REQUIRE(function(0) == 7);
					REQUIRE(destructorCalls == 1);

					// And a third call confirms the second call's frame was popped
					// cleanly too, not just the first's.
					destructorCalls = 0;
					REQUIRE_THROWS_AS(function(1), std::runtime_error);
					REQUIRE(destructorCalls == 1);
				}
			}
		}
	}
}

#endif // ENABLE_TRACING

}} // namespace nautilus

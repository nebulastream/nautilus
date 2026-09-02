#include "ExecutionTest.hpp"
#include "nautilus/Engine.hpp"
#include "nautilus/nautilus_function.hpp"
#include "nautilus/val.hpp"
#include "nautilus/val_func.hpp"
#include <catch2/catch_all.hpp>
#include <cstdint>

/**
 * @file
 * @brief One matrix over the ways a traced function can name a callee.
 *
 * A Nautilus function can call four different things -- a native runtime
 * function, another Nautilus function, an intrinsic a backend expands, and a
 * value holding a function pointer -- and every backend has to tell them
 * apart. Historically each one re-derived the answer by looking a *name* up in
 * a side table it maintained itself, so the cases below were only ever covered
 * incidentally, by whichever feature test happened to exercise them.
 *
 * This file covers them on purpose, across every backend, so that the
 * "which kind of callee is this?" decision has a regression net independent of
 * the features that happen to use it.
 */
namespace nautilus::engine {

namespace {

// --- external callees -------------------------------------------------------

int32_t externalAdd(int32_t a, int32_t b) {
	return a + b;
}

// Overloaded, so its demangled name carries a parameter list and is therefore
// not a valid identifier in any language we generate source for. Backends that
// paste a callee's name into generated code must be going through the table's
// sanitised emission name, not the display name.
[[maybe_unused]] int32_t overloaded(int32_t a) {
	return a + 1;
}
int32_t overloaded(int32_t a, int32_t b) {
	return a * b;
}

// `static`, so it is absent from the dynamic symbol table: dladdr resolves the
// module but leaves dli_sname null, and the trace falls back to stringifying
// the address. The generated name is then ASLR-dependent, so this case pins
// that a callee nobody can name still compiles and runs.
static int32_t unexportedTriple(int32_t a) {
	return a * 3;
}

// --- internal callees -------------------------------------------------------

val<int32_t> internalDoubleImpl(val<int32_t> x) {
	return x * val<int32_t>(2);
}
auto internalDouble = NautilusFunction {"conformance_double", internalDoubleImpl};

// A user-chosen name is not required to be an identifier in any language we
// generate source for. Backends that emit an in-module call by name have to go
// through the table's sanitised emission name, or this lands verbatim in the
// generated source.
val<int32_t> awkwardlyNamedImpl(val<int32_t> x) {
	return x + val<int32_t>(1);
}
auto awkwardlyNamed = NautilusFunction {"conformance add(one)!", awkwardlyNamedImpl};

// Two distinct NautilusFunctions deliberately given the same name. Registration
// used to dedupe on the name, so the second was never traced and every call to
// it dispatched into the first one's body -- silently, with no diagnostic.
val<int32_t> collidingAImpl(val<int32_t> x) {
	return x + val<int32_t>(100);
}
val<int32_t> collidingBImpl(val<int32_t> x) {
	return x + val<int32_t>(200);
}
auto collidingA = NautilusFunction {"conformance_collide", collidingAImpl};
auto collidingB = NautilusFunction {"conformance_collide", collidingBImpl};

// Receives a compiled function pointer and calls it. The pointer must be a
// genuine machine address: this is native code, so nothing here can indirect
// through a Nautilus-side table.
int32_t applyTwiceNatively(int32_t (*fn)(int32_t), int32_t x) {
	return fn(fn(x));
}

// --- the traced kernels -----------------------------------------------------

val<int32_t> callsExternal(val<int32_t> a, val<int32_t> b) {
	return invoke(externalAdd, a, b);
}

val<int32_t> callsOverloadedExternal(val<int32_t> a, val<int32_t> b) {
	return invoke(static_cast<int32_t (*)(int32_t, int32_t)>(overloaded), a, b);
}

val<int32_t> callsUnexportedExternal(val<int32_t> a) {
	return invoke(unexportedTriple, a);
}

val<int32_t> callsInternal(val<int32_t> a) {
	return internalDouble(a);
}

val<int32_t> callsAwkwardlyNamedInternal(val<int32_t> a) {
	return awkwardlyNamed(a);
}

/// Calls both same-named functions. If registration still deduped on the name,
/// the second call would reach the first body and this returns 200, not 300.
val<int32_t> callsBothSameNamedInternals(val<int32_t> a) {
	return collidingA(a) + collidingB(a);
}

/// The same callee reached both ways in one function: a direct in-module call
/// and a call through its address. Both must resolve to the same target.
val<int32_t> callsInternalDirectlyAndIndirectly(val<int32_t> a) {
	auto direct = internalDouble(a);
	auto fnPtr = internalDouble.getFuncPtr();
	auto indirect = fnPtr(a);
	return direct + indirect;
}

/// Hands an in-module function's address to native code, which calls back into
/// compiled code through it. Unlike an indirect call inside traced code, this
/// cannot be resolved on the Nautilus side -- the pointer has to be real.
val<int32_t> passesInternalAddressToNativeCode(val<int32_t> a) {
	auto fnPtr = internalDouble.getFuncPtr();
	return invoke(applyTwiceNatively, fnPtr, a);
}

/// Address-of alone, with the result used as a value rather than called --
/// this is the FUNC_ADDR path with no call to hide behind it.
val<bool> takesAddressOfInternal() {
	auto fnPtr = internalDouble.getFuncPtr();
	return fnPtr != nullptr;
}

/// Two distinct callees whose names differ, so both must survive as separate
/// table entries even though a single traced function mentions both.
val<int32_t> callsExternalAndInternal(val<int32_t> a) {
	auto viaExternal = invoke(externalAdd, a, a);
	return internalDouble(viaExternal);
}

void calleeConformanceTests(engine::NautilusEngine& engine) {
	SECTION("call-external") {
		auto f = engine.registerFunction(callsExternal);
		REQUIRE(f(3, 4) == 7);
		REQUIRE(f(-1, 1) == 0);
	}

	SECTION("call-external-name-is-not-an-identifier") {
		auto f = engine.registerFunction(callsOverloadedExternal);
		REQUIRE(f(3, 4) == 12);
	}

	SECTION("call-external-dladdr-cannot-name") {
		auto f = engine.registerFunction(callsUnexportedExternal);
		REQUIRE(f(5) == 15);
	}

	SECTION("call-internal") {
		auto f = engine.registerFunction(callsInternal);
		REQUIRE(f(21) == 42);
	}

	SECTION("call-internal-name-is-not-an-identifier") {
		auto f = engine.registerFunction(callsAwkwardlyNamedInternal);
		REQUIRE(f(41) == 42);
	}

	SECTION("call-two-internals-sharing-a-name") {
		auto f = engine.registerFunction(callsBothSameNamedInternals);
		// a+100 plus a+200, so both bodies must have been traced and kept
		// distinct. Dedupe-by-name yields 2*(a+100) instead.
		REQUIRE(f(0) == 300);
		REQUIRE(f(5) == 310);
	}

	SECTION("call-internal-direct-and-through-address") {
		auto f = engine.registerFunction(callsInternalDirectlyAndIndirectly);
		REQUIRE(f(5) == 20);
	}

	SECTION("address-of-internal-passed-to-native-code") {
		auto f = engine.registerFunction(passesInternalAddressToNativeCode);
		REQUIRE(f(3) == 12);
	}

	SECTION("address-of-internal") {
		auto f = engine.registerFunction(takesAddressOfInternal);
		REQUIRE(f() == true);
	}

	SECTION("call-external-and-internal-in-one-function") {
		auto f = engine.registerFunction(callsExternalAndInternal);
		REQUIRE(f(3) == 12);
	}
}

// Recursive through its own handle, and registered as a module entry under the
// *same* name -- the shape the runtime-calls demo uses. The trace loop traces
// the body once and skips the duplicate name, so both identities denote one
// body and must resolve to one function-table entry.
val<int32_t> recursiveSumImpl(val<int32_t> n);
auto recursiveSum = NautilusFunction {"conformance_recursive", recursiveSumImpl};
val<int32_t> recursiveSumImpl(val<int32_t> n) {
	if (n <= val<int32_t>(0)) {
		return 0;
	}
	return n + recursiveSum(n - val<int32_t>(1));
}

/// The module API compiles several entry functions into one unit, so the
/// function table holds entries for functions nobody calls alongside the
/// callees they share. Registering the address-of kernel next to unrelated
/// entries pins that the shared table still resolves each one correctly.
void calleeConformanceModuleTests(engine::NautilusEngine& engine) {
	SECTION("module-with-address-of-and-unrelated-entries") {
		auto module = engine.createModule();
		module.registerFunction("callsExternal", callsExternal);
		module.registerFunction("callsInternal", callsInternal);
		module.registerFunction("passesAddress", passesInternalAddressToNativeCode);
		auto compiled = module.compile();

		auto external = compiled.getFunction<int32_t(int32_t, int32_t)>("callsExternal");
		auto internal = compiled.getFunction<int32_t(int32_t)>("callsInternal");
		auto passesAddress = compiled.getFunction<int32_t(int32_t)>("passesAddress");

		REQUIRE(external(3, 4) == 7);
		REQUIRE(internal(21) == 42);
		REQUIRE(passesAddress(3) == 12);
	}

	SECTION("module-entry-sharing-a-name-with-its-own-NautilusFunction") {
		auto module = engine.createModule();
		// Registered under the same name its NautilusFunction handle carries,
		// so the recursive call inside the body reaches this same entry.
		module.registerFunction("conformance_recursive", recursiveSumImpl);
		module.registerFunction("passesAddress", passesInternalAddressToNativeCode);
		auto compiled = module.compile();

		auto sum = compiled.getFunction<int32_t(int32_t)>("conformance_recursive");
		REQUIRE(sum(4) == 10);
		REQUIRE(sum(0) == 0);
	}
}

} // namespace

/// The demo binaries -- and any user who does not set engine.backend -- run on
/// the *default* engine, which is the tiered compiler rather than any single
/// backend the matrix above sweeps. It caches the IRGraph across compile
/// cycles and promotes asynchronously, so it is its own path through the
/// function table and needs its own coverage.
TEST_CASE("Callee Conformance: default engine (tiered)") {
	engine::Options options;
	auto engine = engine::NautilusEngine(options);

	auto module = engine.createModule();
	// The runtime-calls demo's exact shape: an entry function registered under
	// the same name as its own NautilusFunction handle, next to a kernel that
	// hands a compiled function pointer to native code.
	module.registerFunction("conformance_recursive", recursiveSumImpl);
	module.registerFunction("passesAddress", passesInternalAddressToNativeCode);
	module.registerFunction("callsInternal", callsInternal);
	auto compiled = module.compile();

	REQUIRE(compiled.getFunction<int32_t(int32_t)>("conformance_recursive")(4) == 10);
	REQUIRE(compiled.getFunction<int32_t(int32_t)>("passesAddress")(3) == 12);
	REQUIRE(compiled.getFunction<int32_t(int32_t)>("callsInternal")(21) == 42);
}

TEST_CASE("Callee Conformance Interpreter Test") {
	auto engine = nautilus::testing::makeEngine("interpreter");
	calleeConformanceTests(engine);
	calleeConformanceModuleTests(engine);
}

#ifdef ENABLE_TRACING
TEST_CASE("Callee Conformance Compiler Test") {
	nautilus::testing::forEachBackendWithTraceMode(
	    [](engine::NautilusEngine& engine) { calleeConformanceTests(engine); },
	    [](engine::Options& options) {
		    if (options.getOptionOrDefault<std::string>("engine.backend", "") == "mlir") {
			    options.setOption("engine.Compilation", true);
			    options.setOption("mlir.enableMultithreading", false);
		    }
	    });
}
#endif

} // namespace nautilus::engine

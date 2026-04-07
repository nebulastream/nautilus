#include "nautilus/Engine.hpp"
#include "nautilus/std/atomic.h"
#include <atomic>
#include <catch2/catch_all.hpp>
#include <cstdint>

namespace nautilus::engine {

// Member-function based traced functions ----------------------------------------------

val<int64_t> loadStoreFunction(val<int64_t> initial) {
	val<std::atomic<int64_t>> a(initial);
	a.store(initial + val<int64_t>(7));
	return a.load();
}

val<int64_t> exchangeFunction(val<int64_t> initial, val<int64_t> next) {
	val<std::atomic<int64_t>> a(initial);
	val<int64_t> old = a.exchange(next);
	return old + a.load();
}

val<int64_t> fetchAddSubFunction(val<int64_t> initial) {
	val<std::atomic<int64_t>> a(initial);
	val<int64_t> oldAdd = a.fetch_add(val<int64_t>(10));
	val<int64_t> oldSub = a.fetch_sub(val<int64_t>(3));
	return oldAdd + oldSub + a.load();
}

val<uint64_t> fetchBitwiseFunction(val<uint64_t> initial) {
	val<std::atomic<uint64_t>> a(initial);
	a.fetch_or(val<uint64_t>(0xF0));
	a.fetch_and(val<uint64_t>(0xFF));
	a.fetch_xor(val<uint64_t>(0x0F));
	return a.load();
}

// Free-function based: caller passes a pointer to a real std::atomic ----------------

val<int64_t> freeFunctionLoad(val<std::atomic<int64_t>*> p) {
	return atomic_load(p);
}

void freeFunctionStore(val<std::atomic<int64_t>*> p, val<int64_t> v) {
	atomic_store(p, v);
}

val<int64_t> freeFunctionFetchAdd(val<std::atomic<int64_t>*> p, val<int64_t> v) {
	return atomic_fetch_add(p, v);
}

val<int64_t> freeFunctionRelaxed(val<std::atomic<int64_t>*> p, val<int64_t> v) {
	atomic_store<std::memory_order_relaxed>(p, v);
	return atomic_load<std::memory_order_relaxed>(p);
}

void freeFunctionFence() {
	atomic_thread_fence();
}

// Atomic ops directly on val<T*> (no std::atomic wrapper) -------------------------
val<int64_t> rawPtrAtomicFunction(val<int64_t*> p, val<int64_t> v) {
	atomic_store(p, v);
	val<int64_t> old = atomic_fetch_add(p, val<int64_t>(8));
	return old + atomic_load(p);
}

val<int64_t> rawPtrRelaxedFunction(val<int64_t*> p, val<int64_t> v) {
	atomic_store<std::memory_order_relaxed>(p, v);
	return atomic_load<std::memory_order_relaxed>(p);
}

val<bool> rawPtrCmpxchgFunction(val<int64_t*> p, val<int64_t*> expected, val<int64_t> desired) {
	return atomic_compare_exchange_strong(p, expected, desired);
}

// Wrap an existing std::atomic<T>* in a val<std::atomic<T>> view (non-owning) ------
val<int64_t> wrapPtrFunction(val<std::atomic<int64_t>*> p, val<int64_t> v) {
	val<std::atomic<int64_t>> view(p);
	val<int64_t> old = view.fetch_add(v);
	return old + view.load();
}

// Compare exchange via free function ------------------------------------------------
val<bool> freeFunctionCmpxchg(val<std::atomic<int64_t>*> p, val<int64_t*> expected, val<int64_t> desired) {
	return atomic_compare_exchange_strong(p, expected, desired);
}

// 32-bit type coverage --------------------------------------------------------------
val<int32_t> i32MemberFunction(val<int32_t> initial) {
	val<std::atomic<int32_t>> a(initial);
	a.store(initial + val<int32_t>(1));
	val<int32_t> oldAdd = a.fetch_add(val<int32_t>(4));
	val<int32_t> oldSub = a.fetch_sub(val<int32_t>(2));
	val<int32_t> oldXchg = a.exchange(val<int32_t>(100));
	return oldAdd + oldSub + oldXchg + a.load();
}

val<uint32_t> u32RawPtrFunction(val<uint32_t*> p) {
	atomic_store(p, val<uint32_t>(0xF0));
	val<uint32_t> a = atomic_fetch_or(p, val<uint32_t>(0x0F));
	val<uint32_t> b = atomic_fetch_and(p, val<uint32_t>(0xFE));
	val<uint32_t> c = atomic_fetch_xor(p, val<uint32_t>(0x10));
	return a + b + c + atomic_load(p);
}

val<bool> u32CmpxchgFunction(val<uint32_t*> p, val<uint32_t*> expected, val<uint32_t> desired) {
	return atomic_compare_exchange_strong(p, expected, desired);
}

val<bool> i32StdAtomicCmpxchg(val<std::atomic<int32_t>*> p, val<int32_t*> expected, val<int32_t> desired) {
	return atomic_compare_exchange_strong(p, expected, desired);
}

// Acquire/release ordering coverage -------------------------------------------------
val<int64_t> acquireReleaseFunction(val<std::atomic<int64_t>*> p, val<int64_t> v) {
	atomic_store<std::memory_order_release>(p, v);
	return atomic_load<std::memory_order_acquire>(p);
}

val<int64_t> rawPtrAcquireReleaseFunction(val<int64_t*> p, val<int64_t> v) {
	atomic_store<std::memory_order_release>(p, v);
	return atomic_load<std::memory_order_acquire>(p);
}

// Member ops with explicit ordering -------------------------------------------------
val<int64_t> memberOrderingFunction(val<int64_t> initial) {
	val<std::atomic<int64_t>> a(initial);
	a.store<std::memory_order_release>(initial + val<int64_t>(1));
	val<int64_t> old = a.fetch_add<std::memory_order_relaxed>(val<int64_t>(2));
	return old + a.load<std::memory_order_acquire>();
}

// Default ctor + a.get() interop ----------------------------------------------------
val<int64_t> defaultCtorAndGetFunction(val<int64_t> v) {
	val<std::atomic<int64_t>> a;
	atomic_store(a.get(), v);
	return a.load() + atomic_load(a.get());
}

// Member compare_exchange_strong ----------------------------------------------------
val<bool> memberCmpxchgFunction(val<int64_t> initial, val<int64_t*> expected, val<int64_t> desired) {
	val<std::atomic<int64_t>> a(initial);
	val<bool> ok = a.compare_exchange_strong(expected, desired);
	atomic_store(expected, a.load()); // write current value back through expected for inspection
	return ok;
}

// Free-function exchange + bitwise on val<std::atomic<T>*> --------------------------
val<int64_t> freeExchangeFunction(val<std::atomic<int64_t>*> p, val<int64_t> v) {
	return atomic_exchange(p, v);
}

val<uint64_t> freeBitwiseFunction(val<std::atomic<uint64_t>*> p) {
	atomic_fetch_or(p, val<uint64_t>(0x0F));
	atomic_fetch_and(p, val<uint64_t>(0xFE));
	atomic_fetch_xor(p, val<uint64_t>(0x10));
	atomic_fetch_sub(p, val<uint64_t>(1));
	return atomic_load(p);
}

// Loop of fetch_add to flush out non-RMW lowerings ----------------------------------
val<int64_t> loopFetchAddFunction(val<std::atomic<int64_t>*> p, val<int64_t> n) {
	val<int64_t> i = 0;
	while (i < n) {
		atomic_fetch_add(p, val<int64_t>(1));
		i = i + val<int64_t>(1);
	}
	return atomic_load(p);
}

// Boundary values -------------------------------------------------------------------
val<int64_t> boundaryFunction(val<int64_t> initial) {
	val<std::atomic<int64_t>> a(initial);
	val<int64_t> old = a.fetch_add(val<int64_t>(1));
	val<int64_t> after = a.load();
	return old + after;
}

// Fence with non-default ordering ---------------------------------------------------
void fenceAcquireFunction() {
	atomic_thread_fence<std::memory_order_acquire>();
}
void fenceReleaseFunction() {
	atomic_thread_fence<std::memory_order_release>();
}
void fenceAcqRelFunction() {
	atomic_thread_fence<std::memory_order_acq_rel>();
}
void fenceRelaxedFunction() {
	atomic_thread_fence<std::memory_order_relaxed>();
}

// -------------------------------------------------------------------------------------

void atomicTest(engine::NautilusEngine& engine) {
	SECTION("member load/store") {
		auto f = engine.registerFunction(loadStoreFunction);
		REQUIRE(f((int64_t) 5) == (int64_t) 12);
		REQUIRE(f((int64_t) 0) == (int64_t) 7);
	}

	SECTION("member exchange") {
		auto f = engine.registerFunction(exchangeFunction);
		REQUIRE(f((int64_t) 3, (int64_t) 9) == (int64_t) (3 + 9));
	}

	SECTION("member fetch_add / fetch_sub") {
		auto f = engine.registerFunction(fetchAddSubFunction);
		// initial=4: oldAdd=4 (state→14), oldSub=14 (state→11), final=11 ⇒ 4+14+11 = 29
		REQUIRE(f((int64_t) 4) == (int64_t) 29);
	}

	SECTION("member fetch_or / fetch_and / fetch_xor") {
		auto f = engine.registerFunction(fetchBitwiseFunction);
		// initial=0x05: |=0xF0 → 0xF5; &=0xFF → 0xF5; ^=0x0F → 0xFA
		REQUIRE(f((uint64_t) 0x05) == (uint64_t) 0xFA);
	}

	SECTION("free function load/store/fetch_add on caller atomic") {
		std::atomic<int64_t> shared {0};
		auto store = engine.registerFunction(freeFunctionStore);
		store(&shared, (int64_t) 100);
		REQUIRE(shared.load() == 100);

		auto load = engine.registerFunction(freeFunctionLoad);
		REQUIRE(load(&shared) == 100);

		auto fadd = engine.registerFunction(freeFunctionFetchAdd);
		REQUIRE(fadd(&shared, (int64_t) 25) == 100);
		REQUIRE(shared.load() == 125);
	}

	SECTION("wrap raw atomic pointer in non-owning val<std::atomic>") {
		std::atomic<int64_t> shared {10};
		auto f = engine.registerFunction(wrapPtrFunction);
		// fetch_add(5): old=10, state=15; load=15 ⇒ 25
		REQUIRE(f(&shared, (int64_t) 5) == (int64_t) 25);
		REQUIRE(shared.load() == 15); // not deleted by the wrapper
	}

	SECTION("free function relaxed ordering overload") {
		std::atomic<int64_t> shared {0};
		auto f = engine.registerFunction(freeFunctionRelaxed);
		REQUIRE(f(&shared, (int64_t) 42) == 42);
		REQUIRE(shared.load() == 42);
	}

	SECTION("compare_exchange_strong success and failure") {
		std::atomic<int64_t> shared {7};
		int64_t expected = 7;
		auto f = engine.registerFunction(freeFunctionCmpxchg);
		REQUIRE(f(&shared, &expected, (int64_t) 99) == true);
		REQUIRE(shared.load() == 99);

		expected = 0; // wrong
		REQUIRE(f(&shared, &expected, (int64_t) 1234) == false);
		REQUIRE(shared.load() == 99);
		REQUIRE(expected == 99); // updated to current
	}

	SECTION("atomic ops directly on val<int64_t*>") {
		alignas(8) int64_t storage = 0;
		auto f = engine.registerFunction(rawPtrAtomicFunction);
		// store(7); fetch_add(8): old=7, state=15; load=15 ⇒ 22
		REQUIRE(f(&storage, (int64_t) 7) == (int64_t) 22);
		REQUIRE(storage == 15);
	}

	SECTION("relaxed ops on val<int64_t*>") {
		alignas(8) int64_t storage = 0;
		auto f = engine.registerFunction(rawPtrRelaxedFunction);
		REQUIRE(f(&storage, (int64_t) 99) == (int64_t) 99);
		REQUIRE(storage == 99);
	}

	SECTION("compare_exchange_strong on val<int64_t*>") {
		alignas(8) int64_t storage = 5;
		int64_t expected = 5;
		auto f = engine.registerFunction(rawPtrCmpxchgFunction);
		REQUIRE(f(&storage, &expected, (int64_t) 50) == true);
		REQUIRE(storage == 50);
		expected = 0;
		REQUIRE(f(&storage, &expected, (int64_t) 1) == false);
		REQUIRE(storage == 50);
		REQUIRE(expected == 50);
	}

	SECTION("atomic_thread_fence smoke test") {
		auto f = engine.registerFunction(freeFunctionFence);
		f();
	}

	SECTION("int32_t member ops") {
		auto f = engine.registerFunction(i32MemberFunction);
		// initial=2: store→3; fetch_add(4) old=3 state=7; fetch_sub(2) old=7 state=5;
		// exchange(100) old=5 state=100; load=100  ⇒ 3+7+5+100 = 115
		REQUIRE(f((int32_t) 2) == (int32_t) 115);
	}

	SECTION("uint32_t fetch_or/and/xor on val<uint32_t*>") {
		alignas(4) uint32_t storage = 0;
		auto f = engine.registerFunction(u32RawPtrFunction);
		// store(0xF0); or(0x0F) old=0xF0 state=0xFF; and(0xFE) old=0xFF state=0xFE;
		// xor(0x10) old=0xFE state=0xEE; load=0xEE ⇒ 0xF0+0xFF+0xFE+0xEE = 0x3DB
		REQUIRE(f(&storage) == (uint32_t) 0x3DB);
		REQUIRE(storage == 0xEE);
	}

	SECTION("compare_exchange_strong on val<uint32_t*>") {
		alignas(4) uint32_t storage = 11;
		uint32_t expected = 11;
		auto f = engine.registerFunction(u32CmpxchgFunction);
		REQUIRE(f(&storage, &expected, (uint32_t) 22) == true);
		REQUIRE(storage == 22);
		expected = 0;
		REQUIRE(f(&storage, &expected, (uint32_t) 99) == false);
		REQUIRE(storage == 22);
		REQUIRE(expected == 22);
	}

	SECTION("compare_exchange_strong on val<std::atomic<int32_t>*>") {
		std::atomic<int32_t> shared {3};
		int32_t expected = 3;
		auto f = engine.registerFunction(i32StdAtomicCmpxchg);
		REQUIRE(f(&shared, &expected, (int32_t) 8) == true);
		REQUIRE(shared.load() == 8);
		expected = -1;
		REQUIRE(f(&shared, &expected, (int32_t) 0) == false);
		REQUIRE(shared.load() == 8);
		REQUIRE(expected == 8);
	}

	SECTION("acquire/release on val<std::atomic<int64_t>*>") {
		std::atomic<int64_t> shared {0};
		auto f = engine.registerFunction(acquireReleaseFunction);
		REQUIRE(f(&shared, (int64_t) 77) == 77);
		REQUIRE(shared.load() == 77);
	}

	SECTION("acquire/release on val<int64_t*>") {
		alignas(8) int64_t storage = 0;
		auto f = engine.registerFunction(rawPtrAcquireReleaseFunction);
		REQUIRE(f(&storage, (int64_t) 88) == 88);
		REQUIRE(storage == 88);
	}

	SECTION("member functions with explicit ordering template arg") {
		auto f = engine.registerFunction(memberOrderingFunction);
		// initial=10: store(11); fetch_add(2) old=11 state=13; load=13 ⇒ 24
		REQUIRE(f((int64_t) 10) == (int64_t) 24);
	}

	SECTION("default constructor and a.get() interop") {
		auto f = engine.registerFunction(defaultCtorAndGetFunction);
		// default ctor → 0; store(v); load() + atomic_load(get()) = 2v
		REQUIRE(f((int64_t) 21) == (int64_t) 42);
	}

	SECTION("member compare_exchange_strong success and failure") {
		auto f = engine.registerFunction(memberCmpxchgFunction);
		int64_t expected = 5;
		// initial=5, expected=5 → success; cmpxchg sets state to 99; expected gets 99
		REQUIRE(f((int64_t) 5, &expected, (int64_t) 99) == true);
		REQUIRE(expected == 99);

		expected = 0;
		// initial=7, expected=0 → failure; expected updated to 7
		REQUIRE(f((int64_t) 7, &expected, (int64_t) 1) == false);
		REQUIRE(expected == 7);
	}

	SECTION("free atomic_exchange on std::atomic<int64_t>*") {
		std::atomic<int64_t> shared {12};
		auto f = engine.registerFunction(freeExchangeFunction);
		REQUIRE(f(&shared, (int64_t) 34) == 12);
		REQUIRE(shared.load() == 34);
	}

	SECTION("free atomic_fetch_or/and/xor/sub on std::atomic<uint64_t>*") {
		std::atomic<uint64_t> shared {0xF0};
		auto f = engine.registerFunction(freeBitwiseFunction);
		// or(0x0F) → 0xFF; and(0xFE) → 0xFE; xor(0x10) → 0xEE; sub(1) → 0xED
		REQUIRE(f(&shared) == (uint64_t) 0xED);
		REQUIRE(shared.load() == 0xED);
	}

	SECTION("loop of atomic_fetch_add") {
		std::atomic<int64_t> shared {0};
		auto f = engine.registerFunction(loopFetchAddFunction);
		REQUIRE(f(&shared, (int64_t) 1000) == 1000);
		REQUIRE(shared.load() == 1000);
	}

	SECTION("boundary fetch_add wrap-around") {
		auto f = engine.registerFunction(boundaryFunction);
		// initial=INT64_MAX: fetch_add(1) old=MAX state=MIN; load=MIN ⇒ MAX+MIN = -1
		REQUIRE(f((int64_t) INT64_MAX) == (int64_t) -1);
	}

	SECTION("atomic_thread_fence non-default orderings") {
		engine.registerFunction(fenceAcquireFunction)();
		engine.registerFunction(fenceReleaseFunction)();
		engine.registerFunction(fenceAcqRelFunction)();
		engine.registerFunction(fenceRelaxedFunction)();
	}
}

TEST_CASE("Atomic Interpreter Test") {
	engine::Options options;
	options.setOption("engine.Compilation", false);
	auto engine = engine::NautilusEngine(options);
	atomicTest(engine);
}

#ifdef ENABLE_TRACING
TEST_CASE("Atomic Compiler Test") {
	std::vector<std::string> backends = {};
#ifdef ENABLE_MLIR_BACKEND
	backends.emplace_back("mlir");
#endif
#ifdef ENABLE_C_BACKEND
	backends.emplace_back("cpp");
#endif
#ifdef ENABLE_BC_BACKEND
	backends.emplace_back("bc");
#endif
#ifdef ENABLE_ASMJIT_BACKEND
	backends.emplace_back("asmjit");
#endif
	for (auto& backend : backends) {
		DYNAMIC_SECTION(backend) {
			engine::Options options;
			options.setOption("engine.backend", backend);
			auto engine = engine::NautilusEngine(options);
			atomicTest(engine);
		}
	}
}
#endif

} // namespace nautilus::engine

// Thread-safety smoke test for InlineFunctionRegistry.
//
// The registry is normally populated by static ctors at program startup,
// but shared objects loaded later via dlopen can register additional
// entries while the JIT has already started reading the table via
// `getSymbolTable()` / `getBitcode()`. Prior to the hardening changes the
// non-const `getSymbolTable()` returned a live reference while holding no
// mutex, and `getBitcode()` returned a reference that escaped the lock.
//
// This test drives the registry through 4 threads (2 writers, 2 readers)
// for ~1k iterations each and asserts that no invariant is violated.
// It isn't a stress test — it's a regression guard to ensure the snapshot
// semantics stay intact when the APIs evolve. Run under TSAN for maximum
// effect: `cmake -DSAN=thread ..`.
#include "nautilus/inline.hpp"
#include <atomic>
#include <catch2/catch_all.hpp>
#include <string>
#include <thread>
#include <vector>

namespace nautilus {

TEST_CASE("InlineFunctionRegistry concurrent readers and writers", "[inlining][threading]") {
	auto& registry = InlineFunctionRegistry::instance();

	constexpr int iters = 1000;
	std::atomic<bool> start {false};

	// Two writer threads register distinct (fake) bitcodes and symbol
	// mappings. The bitcode keys are derived from stable stack-free
	// integer values to avoid colliding with any real registrations the
	// inlining pass may have populated.
	auto writer = [&](int base) {
		while (!start.load(std::memory_order_acquire)) {
		}
		for (int i = 0; i < iters; ++i) {
			auto* fakeFn = reinterpret_cast<void*>(static_cast<uintptr_t>(0xC0FFEE0000ULL) +
			                                      static_cast<uintptr_t>(base * iters + i));
			registerBitcodePleaseIgnoreThisThanks(fakeFn, "bc", 2);

			auto symName = "sym_" + std::to_string(base) + "_" + std::to_string(i);
			registerSymbolPleaseIgnoreThisThanks(symName.data(), symName.size(), fakeFn);
		}
	};

	// Two reader threads take snapshots and look up known-missing pointers.
	// The invariants checked here are:
	//   1. getBitcode returns nullopt (not an empty-string sentinel) for
	//      entirely fresh addresses.
	//   2. getSymbolTable is safe to call concurrently and returns a
	//      plain-value snapshot that doesn't tear.
	auto reader = [&]() {
		while (!start.load(std::memory_order_acquire)) {
		}
		int unknown_stack {};
		for (int i = 0; i < iters; ++i) {
			auto snapshot = registry.getSymbolTable();
			(void) snapshot.size(); // touch the snapshot to defeat optimisation
			auto bc = registry.getBitcode(&unknown_stack);
			REQUIRE_FALSE(bc.has_value());
		}
	};

	std::vector<std::thread> threads;
	threads.emplace_back(writer, 0);
	threads.emplace_back(writer, 1);
	threads.emplace_back(reader);
	threads.emplace_back(reader);

	start.store(true, std::memory_order_release);
	for (auto& t : threads) {
		t.join();
	}

	// After all writers are done the snapshot must include a stable number
	// of entries corresponding to what both writers inserted. We don't
	// assert the exact count because other tests may have registered
	// additional symbols, but the table must have grown by at least the
	// number we inserted.
	auto finalTable = registry.getSymbolTable();
	REQUIRE(finalTable.size() >= static_cast<size_t>(2 * iters));
}

} // namespace nautilus

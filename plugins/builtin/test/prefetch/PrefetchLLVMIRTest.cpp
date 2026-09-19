#include "LLVMIRTestUtil.hpp"
#include "nautilus/Engine.hpp"
#include "nautilus/builtin/plugin.hpp"
#include "nautilus/builtin/prefetch/prefetch.hpp"
#include <catch2/catch_all.hpp>
#include <filesystem>
#include <string>

namespace nautilus::engine {

// ============================================================================
// LLVM IR tests for prefetch().
//
// The execution tests only prove that prefetch() does not change results; they
// cannot tell a native lowering from a real function call. These tests pin the
// generated LLVM IR, so a regression back to a call to the fallback function
// (or a wrong rw/locality immediate) shows up as a diff. Each reference file
// contains a single `llvm.prefetch(ptr, rw, locality, cache type = data)`.
// ============================================================================

namespace {

template <PrefetchRW RW, PrefetchLocality Locality>
void prefetchOnly(val<const void*> address) {
	prefetch(address, RW, Locality);
}

void testPrefetchLLVMIR(const std::string& functionName, auto func) {
	std::filesystem::path testFile(__FILE__);
	std::filesystem::path referenceIRDir = testFile.parent_path() / "reference-ir";
	testLLVMIR(functionName, func, true, referenceIRDir);
}

} // namespace

TEST_CASE("Prefetch LLVM IR: prefetchReadNone", "[builtin][prefetch]") {
	testPrefetchLLVMIR("prefetchReadNone", prefetchOnly<PrefetchRW::Read, PrefetchLocality::None>);
}

TEST_CASE("Prefetch LLVM IR: prefetchReadLow", "[builtin][prefetch]") {
	testPrefetchLLVMIR("prefetchReadLow", prefetchOnly<PrefetchRW::Read, PrefetchLocality::Low>);
}

TEST_CASE("Prefetch LLVM IR: prefetchReadModerate", "[builtin][prefetch]") {
	testPrefetchLLVMIR("prefetchReadModerate", prefetchOnly<PrefetchRW::Read, PrefetchLocality::Moderate>);
}

TEST_CASE("Prefetch LLVM IR: prefetchReadHigh", "[builtin][prefetch]") {
	testPrefetchLLVMIR("prefetchReadHigh", prefetchOnly<PrefetchRW::Read, PrefetchLocality::High>);
}

TEST_CASE("Prefetch LLVM IR: prefetchWriteNone", "[builtin][prefetch]") {
	testPrefetchLLVMIR("prefetchWriteNone", prefetchOnly<PrefetchRW::Write, PrefetchLocality::None>);
}

TEST_CASE("Prefetch LLVM IR: prefetchWriteLow", "[builtin][prefetch]") {
	testPrefetchLLVMIR("prefetchWriteLow", prefetchOnly<PrefetchRW::Write, PrefetchLocality::Low>);
}

TEST_CASE("Prefetch LLVM IR: prefetchWriteModerate", "[builtin][prefetch]") {
	testPrefetchLLVMIR("prefetchWriteModerate", prefetchOnly<PrefetchRW::Write, PrefetchLocality::Moderate>);
}

TEST_CASE("Prefetch LLVM IR: prefetchWriteHigh", "[builtin][prefetch]") {
	testPrefetchLLVMIR("prefetchWriteHigh", prefetchOnly<PrefetchRW::Write, PrefetchLocality::High>);
}

} // namespace nautilus::engine

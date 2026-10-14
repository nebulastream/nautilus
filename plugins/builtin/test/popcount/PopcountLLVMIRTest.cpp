#include "LLVMIRTestUtil.hpp"
#include "nautilus/Engine.hpp"
#include "nautilus/builtin/plugin.hpp"
#include "nautilus/builtin/popcount/popcount.hpp"
#include <catch2/catch_all.hpp>
#include <filesystem>
#include <string>

namespace nautilus::engine {

// ============================================================================
// LLVM IR tests for builtin::popcount().
//
// The execution tests cannot tell a native lowering from a call to the
// fallback function. These tests pin the generated LLVM IR, so a regression
// back to a real call shows up as a diff. Each reference file contains a
// single `llvm.ctpop` at the operand's width.
// ============================================================================

namespace {

template <typename T>
val<T> popcountOnly(val<T> x) {
	return builtin::popcount(x);
}

void testPopcountLLVMIR(const std::string& functionName, auto func) {
	std::filesystem::path testFile(__FILE__);
	std::filesystem::path referenceIRDir = testFile.parent_path() / "reference-ir";
	testLLVMIR(functionName, func, true, referenceIRDir);
}

} // namespace

TEST_CASE("Popcount LLVM IR: popcountI8", "[builtin][popcount]") {
	testPopcountLLVMIR("popcountI8", popcountOnly<int8_t>);
}

TEST_CASE("Popcount LLVM IR: popcountI16", "[builtin][popcount]") {
	testPopcountLLVMIR("popcountI16", popcountOnly<int16_t>);
}

TEST_CASE("Popcount LLVM IR: popcountI32", "[builtin][popcount]") {
	testPopcountLLVMIR("popcountI32", popcountOnly<int32_t>);
}

TEST_CASE("Popcount LLVM IR: popcountI64", "[builtin][popcount]") {
	testPopcountLLVMIR("popcountI64", popcountOnly<int64_t>);
}

TEST_CASE("Popcount LLVM IR: popcountU8", "[builtin][popcount]") {
	testPopcountLLVMIR("popcountU8", popcountOnly<uint8_t>);
}

TEST_CASE("Popcount LLVM IR: popcountU16", "[builtin][popcount]") {
	testPopcountLLVMIR("popcountU16", popcountOnly<uint16_t>);
}

TEST_CASE("Popcount LLVM IR: popcountU32", "[builtin][popcount]") {
	testPopcountLLVMIR("popcountU32", popcountOnly<uint32_t>);
}

TEST_CASE("Popcount LLVM IR: popcountU64", "[builtin][popcount]") {
	testPopcountLLVMIR("popcountU64", popcountOnly<uint64_t>);
}

} // namespace nautilus::engine

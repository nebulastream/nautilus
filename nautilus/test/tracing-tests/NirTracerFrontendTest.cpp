// The frontend features of M2 on the direct NIR tracer: constant data (M2.6) and explicit control flow (M2.5). Golden
// pre-SSA dumps, verification of the pre-SSA and the converted module, and what the conversion folds.
#include "NirTracerCorpus.hpp"
#include "nautilus/control_flow.hpp"
#include "nautilus/data.hpp"
#include "nautilus/region.hpp"
#include "nautilus/static.hpp"
#include <catch2/catch_all.hpp>
#include <cstdlib>
#include <fstream>

using namespace nautilus;
using namespace nautilus::testing::nirtracer;

namespace {

const int32_t kTable[6] = {11, 22, 33, 44, 55, 66};

val<int32_t> constDataFixed() {
	val<const int32_t*> table = constData(kTable);
	return table[1] + table[4];
}

val<int32_t> constDataLookup(val<int32_t> i) {
	val<const int32_t*> table = constData(kTable);
	return table[i];
}

val<char> constDataString(val<int32_t> i) {
	val<const char*> text = constData("hello");
	val<char> c = text[i];
	return c;
}

val<int64_t> explicitIf(val<int64_t> x) {
	val<int64_t> r = 0;
	If(x > 0, [&] { r = r + 1; }, [&] { r = r - 1; });
	return r;
}

val<int64_t> explicitValueIf(val<int64_t> a, val<int64_t> b) {
	return If(a > b, [&] { return a; }, [&] { return b; });
}

val<int64_t> explicitWhileBreak(val<int64_t> n) {
	val<int64_t> i = 0;
	val<int64_t> sum = 0;
	While([&] { return i < 100; },
	      [&] {
		      If(i == n, [&] { Break(); });
		      sum = sum + i;
		      i = i + 1;
	      });
	return sum;
}

val<int64_t> explicitFor(val<int64_t> n) {
	val<int64_t> sum = 0;
	For(val<int64_t>(0), n, [&](val<int64_t> i) {
		If(i % 2 == 0, [&] { Continue(); });
		sum = sum + i;
	});
	return sum;
}

val<int64_t> explicitReturn(val<int64_t> x) {
	If(x < 0, [&] { Return(val<int64_t>(-1)); });
	return x * 2;
}

val<int64_t> explicitMixed(val<int64_t> x) {
	val<int64_t> r = 0;
	if (x > 3) {
		r = 1;
	}
	If(
	    x > 0,
	    [&] {
		    if (x > 10) {
			    r = r + 10;
		    }
	    },
	    [&] { r = r - 1; });
	if (x == 5) {
		r = r + 100;
	}
	return r;
}

struct Entry {
	std::string name;
	std::function<void()> wrapper;
};

std::vector<Entry> entries() {
	using engine::details::createFunctionWrapper;
	return {{"constDataFixed", createFunctionWrapper(constDataFixed)},
	        {"constDataLookup", createFunctionWrapper(constDataLookup)},
	        {"constDataString", createFunctionWrapper(constDataString)},
	        {"explicitIf", createFunctionWrapper(explicitIf)},
	        {"explicitValueIf", createFunctionWrapper(explicitValueIf)},
	        {"explicitWhileBreak", createFunctionWrapper(explicitWhileBreak)},
	        {"explicitFor", createFunctionWrapper(explicitFor)},
	        {"explicitReturn", createFunctionWrapper(explicitReturn)},
	        {"explicitMixed", createFunctionWrapper(explicitMixed)}};
}

} // namespace

TEST_CASE("frontend features: pre-SSA output verifies, converts and matches the golden dump",
          "[nir-tracer][frontend]") {
	for (const auto& entry : entries()) {
		DYNAMIC_SECTION(entry.name) {
			common::Arena arena;
			auto module = traceDirect(entry.wrapper, arena);
			REQUIRE(diagnostics(module).empty());
			const auto text = nir::print(module);
			INFO(text);
			auto converted = conversion::convert(module);
			INFO(nir::print(converted));
			REQUIRE(diagnostics(converted).empty());
			if (std::getenv("NAUTILUS_UPDATE_GOLDEN") != nullptr) {
				std::ofstream(goldenPath(entry.name)) << text;
			} else {
#ifdef __clang__
				REQUIRE(text == readFile(goldenPath(entry.name)));
#endif
			}
		}
	}
}

TEST_CASE("constData: the table is a data object and loads at constant offsets fold", "[nir-tracer][frontend]") {
	common::Arena arena;
	auto module = traceDirect(engine::details::createFunctionWrapper(constDataFixed), arena);
	REQUIRE(module.data.size() == 1);
	REQUIRE(module.data[0].size == sizeof(kTable));
	REQUIRE(module.data[0].align == alignof(int32_t));
	auto converted = conversion::convert(module);
	const auto text = nir::print(converted);
	INFO(text);
	// 22 + 55 = 77: both loads read the data object and the addition folds
	CHECK(text.find("load") == std::string::npos);
	CHECK(text.find("77") != std::string::npos);
	CHECK(diagnostics(converted).empty());
}

TEST_CASE("constData: a load at a runtime index stays a load of the data object", "[nir-tracer][frontend]") {
	common::Arena arena;
	auto module = traceDirect(engine::details::createFunctionWrapper(constDataLookup), arena);
	auto converted = conversion::convert(module);
	const auto text = nir::print(converted);
	INFO(text);
	CHECK(text.find("data.addr") != std::string::npos);
	CHECK(text.find("load") != std::string::npos);
	CHECK(text.find("ptr.const") == std::string::npos);
}

TEST_CASE("constData: the data does not depend on the host array", "[nir-tracer][frontend]") {
	static int32_t mutableTable[3] = {1, 2, 3};
	auto function = +[]() -> val<int32_t> {
		val<const int32_t*> table = constData(std::span<const int32_t>(mutableTable, 3));
		return table[0] + table[2];
	};
	common::Arena arena;
	auto module = traceDirect(engine::details::createFunctionWrapper(function), arena);
	mutableTable[0] = 100;
	mutableTable[2] = 200;
	const auto text = nir::print(conversion::convert(module));
	INFO(text);
	CHECK(text.find("4") != std::string::npos);
	CHECK(module.pointers.empty());
	mutableTable[0] = 1;
	mutableTable[2] = 3;
}
